#include "stdafx.h"
#include "yaneLockObject.h"

#include "../Thread/yaneThread.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

LRESULT	CLockObject::wait(int nTimeOut/*=-1*/)
{
	//	wait����Ƃ��̓L���[�ɕ��Ԃׂ�

	//	Lock���l�����Ă��Ȃ��Ȃ�΁A��O���X���[
#ifdef USE_EXCEPTION
	if (!GetCriticalSection()->IsLockedByThisThread()){
		throw CIllegalMonitorStateException();
	}
#endif

	//	�����Ŏ��s���Ɂ���throw�ɂ��G���[���o����A
	//	synchronized�ɂ�����wait���Ă�(����G���[)�Ƃ������Ƃ�

	CWaitState* pWaitState = new CWaitState;
	GetWaitThreadList()->push_back(smart_ptr<CWaitState>(pWaitState));

	DWORD dwThreadId = ::GetCurrentThreadId();
	pWaitState->SetThreadId(dwThreadId);

//	pWaitState->GetEvent()->GetCriticalSection()->Enter();
	GetCriticalSection()->Leave();
		//	��������Lock���������

	/*
		LRESULT lr = pWaitState->GetEvent()->Wait(nTimeOut);
		//	�V�O�i����ԂɂȂ�̂�҂�(���łɂȂ��Ă��Ă���)
	*/

	//	�X���b�h�̗�O�C�x���g�ƁA��L�̃C�x���g�̗������Ď�����K�v������

	IThread* pThread = IThread::getThread();
	LRESULT lr;
	if (pThread!=NULL){
		const HANDLE h[] = {
			//	��O�C�x���g�Ɛ����C�x���g�Ƃ̗������Ď�����K�v������
			pThread->GetExceptionEvent()->GetHandle(),
			pWaitState->GetEvent()->GetHandle()
		};
		DWORD dwResult = ::WaitForMultipleObjects(2,&h[0],FALSE,nTimeOut);
		lr = dwResult == WAIT_TIMEOUT ? 1:0;
	} else {
		lr = pWaitState->GetEvent()->Wait(nTimeOut);
	}

	GetCriticalSection()->Enter();
		//	�ēxlock���l������

	eraseThreadInWaitList(dwThreadId);
	//	�҂��Ă���X���b�h���X�g���炱�̃X���b�h�̏�������

	#ifdef USE_EXCEPTION
	//	�����A���荞�ݏ�ԂɂȂ��Ă�����A��O���X���[
	if (pThread!=NULL && pThread->interrupted()){
			throw CInterruptedException();
	}
	#endif

	return lr;
}

void	CLockObject::notify(){
	/// wait�ő҂��Ă���X���b�h��@���N����(1�̂�)
//	CCriticalLock cl(GetCriticalSection());
	//	Lock���l�����Ă��Ȃ��Ȃ�΁A��O���X���[
#ifdef USE_EXCEPTION
	if (!GetCriticalSection()->IsLockedByThisThread()){
		throw CIllegalMonitorStateException();
	}
#endif

	if (GetWaitThreadList()->empty()) return ;
	(*GetWaitThreadList()->rbegin())->GetEvent()->SetEvent();
}

void	CLockObject::notifyAll() {
	/// wait�ő҂��Ă��邷�ׂẴX���b�h��@���N����
	//	Lock���l�����Ă��Ȃ��Ȃ�΁A��O���X���[
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

	//	Lock���l�����Ă��Ȃ��Ȃ�΁A��O���X���[
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

	//	Lock���l�����Ă��Ȃ��Ȃ�΁A��O���X���[
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
			//	�ʒm����̂��I
		}
		it++;
	}
}

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd
