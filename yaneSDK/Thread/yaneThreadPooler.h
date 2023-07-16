//
// yaneThreadPooler.h :
//
//	CThreadPool,CThreadManager��atl���Œ�`����Ă���(;�L�D`)
//	

#ifndef __yaneThreadPooler_h__
#define __yaneThreadPooler_h__


#include "../Thread/yaneThread.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

class CThreadPoolerRequestQueue {
/**
	CThreadPooler���p����R�[���o�b�N�̂��߂�queue
	����P�Ƃł��g�p���邱�Ƃ͏o����
*/
public:
	///	���܊֐������s���Ă���X���b�h�̐�
	int	getInvokeCount() const { return m_nInvokeCount; }
	void setInvokeCount(int nInvokeCount){
		synchronized g(getLock());
		m_nInvokeCount = nInvokeCount;
		getLock()->notifyAll();	//	�����҂��Ă������邩���m���
	}
	void incInvokeCount() {
		synchronized g(getLock());
		m_nInvokeCount++;
		getLock()->notifyAll();	//	�����҂��Ă������邩���m���
	}
	void decInvokeCount() {
		synchronized g(getLock());
		m_nInvokeCount--;
		getLock()->notifyAll();	//	�����҂��Ă������邩���m���
	}

	///	�v�[������Ă���S�X���b�h�̐��̐ݒ�/�擾
	int getThreadCount() const { return m_nThreadCount; }
	void setThreadCount(int nThreadNum) {
		synchronized g(getLock());
		m_nThreadCount = nThreadNum;
		getLock()->notifyAll();	//	�����҂��Ă������邩���m���
	}
	void decThreadCount() {
		synchronized g(getLock());
		m_nThreadCount--;
		getLock()->notifyAll();	//	�����҂��Ă������邩���m���
	}
	void incThreadCount() {
		synchronized g(getLock());
		m_nThreadCount++;
		getLock()->notifyAll();
	}

	///	request��get/set
	smart_ptr<function_callback> getRequest() {
		synchronized g(getLock());
		smart_ptr<function_callback> fn;
		if (m_bInvalidate) return fn; // �_�����

		while (m_aInvokeList.size()==0){
			getLock()->wait();
			if (m_bInvalidate) return fn; // �_�����
		}
		fn = *m_aInvokeList.begin();
		m_aInvokeList.pop_front();
		return fn;
	}
	void setRequest(const smart_ptr<function_callback>& req){
		synchronized g(getLock());
		m_aInvokeList.push_back(req);
		getLock()->notifyAll();
	}

	///	getRequest�őҋ@���Ă���X���b�h�ɑ΂���invalidate��ʒm
	void invalidateAllThread(){
		synchronized g(getLock());
		//	WorkerThread�̂Ȃ���wait���Ă���S�X���b�h�ɒʒm
		m_bInvalidate = true;
		getLock()->notifyAll();
	}
	void setValid(bool bValid) {
		m_bInvalidate = !bValid;
	}

	bool isDone(){
		synchronized g(getLock());
	//	�I���������󒍂��Ă���d�����Ȃ��āA���A���s���Ă���d��������
		return	getInvokeCount()==0 && getInvokeList()->size()==0;
	}

	LRESULT waitAllThreadEnded(int nTimeOut){
		synchronized g(getLock());
		while (!isDone()){
			LRESULT lr = getLock()->wait(nTimeOut);
			if (lr!=0) return lr;
		}
		return 0;
	}

	///	�󒍂��Ă���d���̃��X�g
	smart_list_ptr<function_callback>* getInvokeList()
		{ return &m_aInvokeList;}

	///	����������Ƃ��́A�I�u�W�F�N�g��lock���K�v
	CLockObject* getLock() { return& m_vLockObject; }

	CThreadPoolerRequestQueue()
		:m_nInvokeCount(0),m_nThreadCount(0),m_bInvalidate(false){}

private:
	CLockObject m_vLockObject;	//	�������̎d���̃J�E���g�̂��߂�LockObject	
	smart_list_ptr<function_callback> m_aInvokeList;
	int m_nInvokeCount;
	int m_nThreadCount;
	bool m_bInvalidate;
};

class CThreadPoolerThread : public CThread {
/**
	CThreadPooler�Ŏg�p����ҋ@�X���b�h
*/
public:
	CThreadPoolerThread(const shared_ptr<CThreadPoolerRequestQueue>& queue_)
		: queue(queue_){
		queue->incThreadCount();	//	���s���̃X���b�h�J�E���g�����Z����
	//	���s���̃X���b�h�J�E���g�́A�R���X�g���N�^��inc���āA
	//	ThreadProc�𔲂���Ƃ���dec����
	}

	virtual void ThreadProc();
	~CThreadPoolerThread(){ StopThread(); }
	//	�X���b�h�N���X����h��������ꍇ�́A
	//	�K���h���N���X����StopThread���s�Ȃ��K�v������

private:
	shared_ptr<CThreadPoolerRequestQueue> queue;
};

class CThreadPooler {
public:
	///	�X���b�h���w�肵���������N�����đҋ@������
	LRESULT	StartThread(int nThreadNum);

	///	�N�����Ă���X���b�h�Ɏw�肵���֐������s������
	void	Invoke(const smart_ptr<function_callback>&fn);

	///	�ێ����Ă���X���b�h�����ׂĒ�~������
	void	ClearAllThread();

	///	Invoke�����̂ɏ�������Ă��Ȃ��d���̐���0�ɂȂ��true
	bool	IsDone() const;

	///	��������Ă��Ȃ��d���̐���0�ɂȂ�̂�҂�
	///	0:ok  1:timeout
	LRESULT	wait(int nTimeOut=-1);

	CThreadPooler::CThreadPooler(){ m_pQueue.Add();}
	virtual ~CThreadPooler(){ ClearAllThread(); }

private:
	smart_vector_ptr<CThreadPoolerThread> m_aThread;
	shared_ptr<CThreadPoolerRequestQueue> m_pQueue;
};

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd

#endif
