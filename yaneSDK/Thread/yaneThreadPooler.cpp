#include "stdafx.h"
#include "yaneThreadPooler.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

//	worker thread��queue���Ȃ���^�X�N�����s����̂�
void CThreadPoolerThread::ThreadProc(){
	while (IsThreadValid()){
		smart_ptr<function_callback> fn;
		fn = queue->getRequest();
		if (fn.isNull()) break;	//	�����d������ւ�
		queue->incInvokeCount();	//	���s���̃^�X�N���J�E���g�𑝂₷
		fn->run();
		queue->decInvokeCount();	//	���s���̃^�X�N���J�E���g�����炷
	}
	queue->decThreadCount();		//	���s���̃X���b�h�J�E���g�����炷
}

////////////////////////////////////////////////////////////////////

LRESULT	CThreadPooler::StartThread(int nThreadNum){
	//	���܋N�����Ă���X���b�h�����ׂĒ�~������
	ClearAllThread();

	//	���ꂼ��̃X���b�h�𐶐��`�N������
	for(int i=0;i<nThreadNum;i++){
		CThreadPoolerThread* p = new CThreadPoolerThread(m_pQueue);
		p->CreateThread();
		m_aThread.insert(p);
	}

	return 0;
}

///	�ێ����Ă���X���b�h�����ׂĒ�~������
void	CThreadPooler::ClearAllThread(){
	/*
	for(UINT i=0;i<GetThreadList()->size();i++){
		(*GetThreadList())[i]->InvalidateThread();
	}
	*/
	//	�����������������l�����邪�A�����ł�queue�ɓ`�B����
	//	queue���烊�N�G�X�g�����炨���Ƒҋ@���Ă���X���b�h��
	//	�΂��ċ��callback��Ԃ����Ƃɂ���ďI����\������

	m_pQueue->invalidateAllThread();

	//	smart_ptr�Ȃ̂ŁA�폜�����Ƃ���CThread�̃f�X�g���N�^��
	//	�Ăяo����A���̂Ȃ���StopThread���Ă���̂ł���Ő���������
	//�@�悤�Ɏv���邪�A�h���N���X�̏ꍇ�͂����ł͂Ȃ��B
//	for(int i=0;i<(int)m_aThread.size();++i) m_aThread[i]->StopThread();
	m_aThread.clear();
	m_pQueue->setValid(true); // invalidate�����̂Ō��ɖ߂��Ă���
}

void	CThreadPooler::Invoke(const smart_ptr<function_callback>&fn){
	m_pQueue->setRequest(fn);
}

LRESULT	CThreadPooler::wait(int nTimeOut/*=-1*/){
	LRESULT lr = m_pQueue->waitAllThreadEnded(nTimeOut);
	if (lr!=0) return lr;
	ClearAllThread();	//	�S�X���b�h�̒�~��҂�
	return 0;
}

///	Invoke�����̂ɏ�������Ă��Ȃ��d���̐���0�ɂȂ��true
bool	CThreadPooler::IsDone() const{
	CThreadPooler *pThis = const_cast<CThreadPooler*> (this);
	return pThis->m_pQueue->isDone();
}

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd
