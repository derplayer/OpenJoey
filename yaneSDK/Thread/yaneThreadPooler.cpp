#include "stdafx.h"
#include "yaneThreadPooler.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

//	worker threadがqueue見ながらタスクを実行するのだ
void CThreadPoolerThread::ThreadProc(){
	while (IsThreadValid()){
		smart_ptr<function_callback> fn;
		fn = queue->getRequest();
		if (fn.isNull()) break;	//	もう仕事あらへん
		queue->incInvokeCount();	//	実行中のタスク数カウントを増やす
		fn->run();
		queue->decInvokeCount();	//	実行中のタスク数カウントを減らす
	}
	queue->decThreadCount();		//	実行中のスレッドカウントを減らす
}

////////////////////////////////////////////////////////////////////

LRESULT	CThreadPooler::StartThread(int nThreadNum){
	//	いま起動しているスレッドをすべて停止させる
	ClearAllThread();

	//	それぞれのスレッドを生成～起動する
	for(int i=0;i<nThreadNum;i++){
		CThreadPoolerThread* p = new CThreadPoolerThread(m_pQueue);
		p->CreateThread();
		m_aThread.insert(p);
	}

	return 0;
}

///	保持しているスレッドをすべて停止させる
void	CThreadPooler::ClearAllThread(){
	/*
	for(UINT i=0;i<GetThreadList()->size();i++){
		(*GetThreadList())[i]->InvalidateThread();
	}
	*/
	//	↑こういう実装も考えられるが、ここではqueueに伝達して
	//	queueからリクエストをもらおうと待機しているスレッドに
	//	対して空のcallbackを返すことによって終了を予告する

	m_pQueue->invalidateAllThread();

	//	smart_ptrなので、削除されるときにCThreadのデストラクタが
	//	呼び出され、そのなかでStopThreadしているのでこれで正しく動く
	//　ように思われるが、派生クラスの場合はそうではない。
//	for(int i=0;i<(int)m_aThread.size();++i) m_aThread[i]->StopThread();
	m_aThread.clear();
	m_pQueue->setValid(true); // invalidateしたので元に戻しておく
}

void	CThreadPooler::Invoke(const smart_ptr<function_callback>&fn){
	m_pQueue->setRequest(fn);
}

LRESULT	CThreadPooler::wait(int nTimeOut/*=-1*/){
	LRESULT lr = m_pQueue->waitAllThreadEnded(nTimeOut);
	if (lr!=0) return lr;
	ClearAllThread();	//	全スレッドの停止を待つ
	return 0;
}

///	Invokeしたのに処理されていない仕事の数が0になればtrue
bool	CThreadPooler::IsDone() const{
	CThreadPooler *pThis = const_cast<CThreadPooler*> (this);
	return pThis->m_pQueue->isDone();
}

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd
