#include "stdafx.h"
#include "yaneLockObject.h"

#include "../Thread/yaneThread.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

LRESULT	CLockObject::wait(int nTimeOut/*=-1*/)
{
	//	waitするときはキューに並ぶべし

	//	Lockを獲得していないならば、例外をスロー
#ifdef USE_EXCEPTION
	if (!GetCriticalSection()->IsLockedByThisThread()){
		throw CIllegalMonitorStateException();
	}
#endif

	//	ここで実行時に↑のthrowによるエラーが出たら、
	//	synchronizedにせずにwaitしてる(からエラー)ということだ

	CWaitState* pWaitState = new CWaitState;
	GetWaitThreadList()->push_back(smart_ptr<CWaitState>(pWaitState));

	DWORD dwThreadId = ::GetCurrentThreadId();
	pWaitState->SetThreadId(dwThreadId);

//	pWaitState->GetEvent()->GetCriticalSection()->Enter();
	GetCriticalSection()->Leave();
		//	いったんLockを解放する

	/*
		LRESULT lr = pWaitState->GetEvent()->Wait(nTimeOut);
		//	シグナル状態になるのを待つ(すでになっていても可)
	*/

	//	スレッドの例外イベントと、上記のイベントの両方を監視する必要がある

	IThread* pThread = IThread::getThread();
	LRESULT lr;
	if (pThread!=NULL){
		const HANDLE h[] = {
			//	例外イベントと生存イベントとの両方を監視する必要がある
			pThread->GetExceptionEvent()->GetHandle(),
			pWaitState->GetEvent()->GetHandle()
		};
		DWORD dwResult = ::WaitForMultipleObjects(2,&h[0],FALSE,nTimeOut);
		lr = dwResult == WAIT_TIMEOUT ? 1:0;
	} else {
		lr = pWaitState->GetEvent()->Wait(nTimeOut);
	}

	GetCriticalSection()->Enter();
		//	再度lockを獲得する

	eraseThreadInWaitList(dwThreadId);
	//	待っているスレッドリストからこのスレッドの情報を消す

	#ifdef USE_EXCEPTION
	//	もし、割り込み状態になっていたら、例外をスロー
	if (pThread!=NULL && pThread->interrupted()){
			throw CInterruptedException();
	}
	#endif

	return lr;
}

void	CLockObject::notify(){
	/// waitで待っているスレッドを叩き起こす(1つのみ)
//	CCriticalLock cl(GetCriticalSection());
	//	Lockを獲得していないならば、例外をスロー
#ifdef USE_EXCEPTION
	if (!GetCriticalSection()->IsLockedByThisThread()){
		throw CIllegalMonitorStateException();
	}
#endif

	if (GetWaitThreadList()->empty()) return ;
	(*GetWaitThreadList()->rbegin())->GetEvent()->SetEvent();
}

void	CLockObject::notifyAll() {
	/// waitで待っているすべてのスレッドを叩き起こす
	//	Lockを獲得していないならば、例外をスロー
#ifdef USE_EXCEPTION
	if (!GetCriticalSection()->IsLockedByThisThread()){
		throw CIllegalMonitorStateException();
	}
#endif
//	CCriticalLock cl(GetCriticalSection());
		
	smart_vector_ptr<CWaitState>::iterator it =
		GetWaitThreadList()->begin();
	while (it!=GetWaitThreadList()->end()){
		(*it)->GetEvent()->SetEvent();
		it++;
	}
}

void	CLockObject::eraseThreadInWaitList(DWORD dwThreadId){

	//	Lockを獲得していないならば、例外をスロー
#ifdef USE_EXCEPTION
	if (!GetCriticalSection()->IsLockedByThisThread()){
		throw CIllegalMonitorStateException();
	}
#endif

	smart_vector_ptr<CWaitState>::iterator it =
		GetWaitThreadList()->begin();
	while (it!=GetWaitThreadList()->end()){
		if ((*it)->GetThreadId() == dwThreadId){
			it = GetWaitThreadList()->erase(it);
		} else {
			it++;
		}
	}
}

void	CLockObject::notifyThreadInWaitList(DWORD dwThreadId){

	//	Lockを獲得していないならば、例外をスロー
#ifdef USE_EXCEPTION
	if (!GetCriticalSection()->IsLockedByThisThread()){
		throw CIllegalMonitorStateException();
	}
#endif

	smart_vector_ptr<CWaitState>::iterator it =
		GetWaitThreadList()->begin();
	while (it!=GetWaitThreadList()->end()){
		if ((*it)->GetThreadId() == dwThreadId){
			(*it)->GetEvent()->SetEvent();
			//	通知するのだ！
		}
		it++;
	}
}

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd
