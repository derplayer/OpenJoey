/**
	Java��object lock/synchronized���T�|�[�g���邽�߂̃N���X

		programmed by M.Isozaki(yaneurao) '02/07/15

*/
#ifndef __yaneLockObject_h__
#define __yaneLockObject_h__


#include "../Thread/yaneEvent.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

class CLockObject {
/**
	java��object lock/synchonized���T�|�[�g���邽�߂̃N���X

	���ۂ̎g�����ɂ��Ă�Java��Thread�֌W�̖{��ǂނ��A
	class CThreadManager �̎������Q�l�ɂ��ׂ��B
*/
public:

	LRESULT	wait(int nTimeOut=-1);
	/**
		wait �L���[�ɕ���
		�P�D�������A�Ăяo���O�ɂ�critical lock���l�����Ă��Ȃ����
			�Ȃ�Ȃ��B���Ȃ킿�AGetCriticalSection()->Enter()
			���Ăяo���Ă����ԂłȂ���΂Ȃ�Ȃ��B

			�������b�N�������Ă��Ȃ����
				CIllegalMonitorStateException
			��O�������B

		�Q�D���ԑO�ɃX���b�h�́A
			�������񎩕���critical lock����������B
		�R�D�҂��́A���X���b�h����
			notify / notifyAll���Ăяo�����܂ő���
			CThread�ɑ΂��āAinterrupt���Ăяo���ꂽ�Ƃ���
			�҂����甲����@�ˁ@CThread::interrupt�Q�Ɓj
		�S�D������O�ɁA�ēx������critical lock���s�Ȃ�

		�T�DnTime�́A���̃^�C���A�E�g���Ԃ��w��ł���B[ms]�P�ʁB
			-1�Ȃ��endless�inotify / notifyAll�����܂ő҂j

		�Ԃ��l�F
			0 : ���X���b�h�ɂ����notify / notifyAll���ꂽ
			1 : �^�C���A�E�g�ɂȂ���
		//	2 : ���肦�Ȃ��͂������ACEvent���Ԃ��\������
			(��F�������s���Ȃ�)
	*/

	void	notify();
	/// wait�ő҂��Ă���X���b�h��@���N����(1�̂�)

	void	notifyAll();
	/// wait�ő҂��Ă��邷�ׂẴX���b�h��@���N����

	///////////////////////////////////////////////////////////////////
	//	���܂�

	void	eraseThreadInWaitList(DWORD dwThreadID);
	/**
		�҂��X���b�h���X�g�̂Ȃ�����A�w�肳�ꂽ�X���b�h�Ɠ���
		�X���b�hID�������̂���������
		�i���̌Ăяo���ɐ�s���ă��b�N���l�����Ă��Ȃ���΂Ȃ�Ȃ��j
			�l�����Ă��Ȃ��ꍇ��CIllegalMonitorStateException������

		�ӂ��́A�Ăяo�����Ƃ͖����i�͂��j
	*/

	void	notifyThreadInWaitList(DWORD dwThreadID);
	/**
		�҂��X���b�h���X�g�̂Ȃ�����A�w�肳�ꂽ�����b�h�Ɠ���
		�X���b�hID�������̂ɑ΂��āASetEvent���s�Ȃ�
	*/

public:
	ICriticalSection* GetCriticalSection() { return& m_cs;}
	/**
		���̃I�u�W�F�N�g�ɑ΂���critical lock
		��j
			CLockObject o;
			CCriticalLock cl(o.GetCriticalSection());
			{
				�Ȃ�₩��
			}
		�Ƃ���΁A�u�Ȃ�₩��v�̕����̏�����atomic�i���X���b�h��
		�ז�����Ȃ��j�ɂȂ�
	*/

	struct CWaitState {
	/**
		wait�ő҂��Ă���X���b�h��\�����邽�߂̍\����
	*/
		void	SetThreadId(DWORD dwThreadID) { m_dwThreadID = dwThreadID; }
		DWORD	GetThreadId() const { return m_dwThreadID; }
		/**
			���̑҂��Ă���X���b�h��ݒ�^�擾����
			�����Őݒ肳���l�́Aunique�ł���Εʂ�::GetThreadId�Ŏ擾
			�o����l�łȂ��Ƃ��ǂ��B
		*/

		IEvent* GetEvent() { return &m_vEvent; }
		///	�҂��̂��߂̃C�x���g

		CWaitState() : m_bInterrupt(false),m_dwThreadID((DWORD)-1) {}
	private:
		CEvent	m_vEvent;	//	�҂��Ă���C�x���g���X�g
		bool	m_bInterrupt;
		DWORD	m_dwThreadID;
	};

	smart_vector_ptr<CWaitState>* GetWaitThreadList()
		{ return &m_WaitThreadList; }

protected:
	CCriticalSection m_cs;
	smart_vector_ptr<CWaitState> m_WaitThreadList;
	//	������������Ƃ��́ACriticalLock��K��������̂ŁA
	//	volatile�ł���K�v�͖���

};

struct synchronized {
/**
	CLockObject�ɑ΂���java��synchronized����������
	�g������

	void test(){
		synchronized g(&lock);
		//	���̃��\�b�h�͋[���I��synchronized�ɂȂ�
	}
	CLockObject lock;
*/
	synchronized(CLockObject*lock) {
		cs_ = lock->GetCriticalSection();
		cs_->Enter();
	}
	synchronized(CLockObject& lock) {
		cs_ = lock.GetCriticalSection();
		cs_->Enter();
	}	
	synchronized(ICriticalSection*cs) {
		cs_ = cs;
		cs_->Enter();
	}
	synchronized(CCriticalSection& cs) {
		cs_ = &cs;
		cs_->Enter();
	}

	///	CriticalSection��enter�`leave�𖾎��I�ɍs�Ȃ��p�̃��\�b�h
	void lock(){ cs_->Enter(); }
	void unlock() { cs_->Leave(); }

	~synchronized(){
		cs_->Leave();
	}

protected:
	ICriticalSection* cs_;
};

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd

#endif
