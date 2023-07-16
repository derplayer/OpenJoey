//	yaneThread.h :
//		programmed by yaneurao	'00/02/26
//		�����I�u�W�F�N�g�̓��� '01/11/19

#ifndef __yaneThread_h__
#define __yaneThread_h__


#include "../Thread/yaneEvent.h"
#include "../Thread/yaneLockObject.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

class IThread {
public:
	virtual LRESULT CreateThread()=0;
	virtual LRESULT CreateThread(const smart_ptr<function_callback>& fn)=0;
	virtual void SetThreadFunction(const smart_ptr<function_callback>& fn)=0;
	virtual LRESULT StopThread()=0;
	virtual LRESULT JumpToThread()=0;
	virtual bool IsThreadExecute()const=0;
	virtual bool IsThreadValid()const=0;
	virtual void InvalidateThread()=0;
	virtual DWORD	GetThreadId() const=0;

	//	java�݊��X���b�h
	virtual LRESULT	wait(int nTimeOut=-1)
		#ifdef USE_EXCEPTION
			throw(CInterruptedException)
		#endif
			=0;
	virtual	LRESULT sleep(int nTime)
		#ifdef USE_EXCEPTION
			throw(CInterruptedException)
		#endif
			=0;
	virtual void	notify()=0;
	virtual void	notifyAll()=0;
	virtual void	interrupt()=0;
	virtual bool	isInterrupted() const=0;
	virtual bool	interrupted()=0;

	virtual ~IThread(){}

	virtual CEvent* GetExceptionEvent()=0;
	//	��O��������signal��ԂɂȂ�C�x���g

	//	���̃X���b�h�ɑΉ�����X���b�h�N���X�𓾂�
	//	(NULL�ł��邱�Ƃ����肤��)
	static IThread* getThread() { return pThread; }
	static void setThread(IThread*p) { pThread = p; }
		//	�������CThread��CreateThread�����Ƃ��ɏ���ɂ����
	static void unsetThread() { pThread.Clear(); }

protected:
	static ThreadLocal<IThread*> pThread;
};

}}

//	����namespace�ɂ�����̂�O���錾����
namespace yaneuraoGameSDK3rd { namespace AppFrame { class IAppBase; }}

namespace yaneuraoGameSDK3rd {
namespace Thread {

class CThread : public IThread {
/**
	�X���b�h�𐶐��i�Ǘ��j���܂��B

	�X���b�h�����Ɋւ��ẮA
	class CThreadManager �̂ق����Q�Ƃ��Ă��������B

	//	�X���b�h�N���X����h��������ꍇ�́A
	//	�K���h���N���X����StopThread���s�Ȃ��K�v������̂Œ���
	(����˖{�Q���Q�Ƃ̂���)
*/
public:
	virtual void ThreadProc() {}
	/**
		���[�J�[�X���b�h�̏������L�q���邽�߂ɁA
		���̊֐����I�[�o�[���C�h���܂��B
		�i���̃N���X��h�������Ďg���Ƃ��j
	*/

	virtual LRESULT CreateThread();
	virtual LRESULT CreateThread(const smart_ptr<function_callback>& fn);
	virtual void SetThreadFunction(const smart_ptr<function_callback>& fn);
	/**
		CreateThread�́A�X���b�h���쐬���AThreadProc�����s���͂��߂܂��B
		CreateThread�̈����Ƃ��āA�R�[���o�b�N����֐����w�肵�Ȃ��ꍇ�́A
		ThreadProc���Ăяo�����̂�ThreadProc���I�[�o�[���C�h���Ă������A
		SetThreadFunction�ŁA�X���b�h�֐��Ƃ��ăR�[���o�b�N����֐���
		�ݒ肵�Ă������ƁB
	*/

	virtual LRESULT StopThread();
	/**
		�X���b�h�̃X�g�b�v�B��~����܂ő҂��܂�
		�Ԃ��l�F
			�O�F����I��
			�P�F���łɃX���b�h�����݂��Ă��Ȃ������i����I���j
	*/

	virtual LRESULT JumpToThread();	///	�X���b�h����炸�ɃW�����v

	virtual bool IsThreadExecute()const; /// �X���b�h�͎��s�����H

	virtual bool IsThreadValid()const;
	/**
		�X���b�h��Stop�͂������Ă��Ȃ���
		���̊֐���false�ɂȂ����Ƃ��ɁAThreadProc����E�o����悤��
		�R�[�f�B���O���܂��B
	*/

	virtual void InvalidateThread();
	/**
		�X���b�h�ɒ�~�M���𑗂�
		�i���̒�~��҂킯�ł͂Ȃ��j
		���[�J�[�X���b�h�͎���IsThreadValid���`�F�b�N�����Ƃ���
		false���Ԃ��Ă��܂��̂ŁA���̂Ƃ���ThreadProc���甲���o��ł��傤�B
		���邢��ThreadSleep�����Ƃ��ɔ�O���Ԃ�̂ŁA�����ŃX���b�h��
		������R�[�h�������Ă��ǂ��ł��B
	*/

	/// ----- Java�̃X���b�h�@�\������������
	/**
		���ꂼ��̐����ɂ��ďڂ����� class CLockObject �̓����̊֐���
		Java��Thread�̓����̊֐����Q�Ƃ̂��ƁB
	*/

	virtual LRESULT	wait(int nTimeOut=-1)
		#ifdef USE_EXCEPTION
			throw(CInterruptedException)
		#endif
			;
	///		wait �L���[�ɕ���

	virtual void	notify();
	/// wait�ő҂��Ă���X���b�h��@���N����(1�̂�)

	virtual void	notifyAll();
	/// wait�ő҂��Ă��邷�ׂẴX���b�h��@���N����

	virtual void	interrupt();
	/**
		���荞�݂�������B���̊֐��́A���X���b�h����Ăяo���B
		��������ƁA���̃X���b�h�́A����wait��sleep��
			CInterruptedException��O�𔭐�������B
	*/

	virtual bool	isInterrupted() const;
	///	���荞�ݏ�ԂɂȂ��Ă��邩

	virtual bool	interrupted();
	///	���荞�ݏ�ԂɂȂ��Ă��邩��Ԃ����A���̌�A
	///	���荞�ݏ�Ԃ̓��Z�b�g����B

	virtual	LRESULT sleep(int nTime)
		#ifdef USE_EXCEPTION
			throw(CInterruptedException)
		#endif
			;
	/**
		nTime [ms]�҂B
	�Ԃ��l�F
		0�F����I���@
		1 : �҂��Ă���Œ��ɁA�ʃX���b�h����StopThread���Ăяo������
		�����I�u�W�F�N�g���V�O�i����ԁi�X���b�h���I�������Ȃ����A
		�Ƃ�����ԁj�ɂȂ�A�������Ɋ֐��𔲂���B���̏ꍇ�A��0���Ԃ�B
		���X���b�h����A���̃X���b�h��interrupt���\�b�h���Ăяo���ꂽ
		�ꍇ�Asleep�ł̑ҋ@�𑦍��ɏI�����ACInterruptedException��O��
		����������B

	�ʗ�A���[�J�[�X���b�h�́A
�@�@while ( IsThreadValid( ) ) {
�@�@�@	���� ;
�@�@�@	Sleep( 20 );
�@�@} 
	�Ƃ����悤�ȃR�[�f�B���O�ɂ���āA�����̏������s�Ȃ������������܂����A
	���̂Ƃ���Sleep(20)�����邽�߁A�X���b�h��Invalidate����āi�I���̍��}�j
	����A���̕��������X�|���X���ቺ���܂��B�����ŁA������Sleep�ł͂Ȃ��A
	if (thread.sleep( 20 )) break; �ȂǂƂ���΁A���̕��̃��X�|���X��
	���コ���邱�Ƃ��o���܂��Bthread.sleep�̎����́A�����I�u�W�F�N�g��҂�
	(::WaitForSingleObject)�悤�ɂȂ��Ă���̂ŁA���̕����ɂ�����CPU�p���[���A
	���܂�g�p�����A�����I��Sleep���邱�Ƃ��o���܂��B
	*/

	/// ----- property

	virtual DWORD	GetThreadId() const { return m_dwRunningThreadID; }
	/**
		�N�����Ă���X���b�h�́AGetCurrentThreadId�ŕԂ����l��Ԃ��B
		�X���b�h���N�����Ă��Ȃ��ꍇ�́A-1���Ԃ�
	*/
	virtual HANDLE	GetThreadHandle() const { return m_dwThreadHandle; }
	/**
		�N�����Ă���X���b�h�̃n���h����Ԃ�
	*/

	//	�X���b�h�̃X�^�b�N�T�C�Y�̎w��
	void	SetStackSize(size_t size) { m_nStackSize = size; }
	size_t	GetStackSize() const { return m_nStackSize; }

	CThread();
	virtual ~CThread();

protected:
	volatile HANDLE	m_dwThreadHandle;		//	�X���b�h�n���h��
	volatile DWORD	m_dwRunningThreadID;	//	�X���b�h��ID(��L�Ɠ������Ǝv����OS�ˑ��̉\��������̂ňꉞ�ʈ����ɂ���)

//	static void ThreadCallBack(LPVOID lpVoid);
	static unsigned __stdcall ThreadCallBack( void * lpVoid);

	void	InitEvent();
	//	�ȉ��̂Q�̃X���b�h�̃X�e�[�^�X������������

	CEvent	m_vExistEvent;
	//	����́A�X���b�h�������Ă���Ԃ́A�m���V�O�i�����
	//	�V�O�i����ԂɂȂ����Ƃ��́A�X���b�h���I�������Ȃ����Ƃ������}
	CEvent* GetExistEvent() { return &m_vExistEvent; }
	//	�˃V�O�i����ԂɂȂ�΁AIsThreadValid��false��Ԃ�

	CEvent m_vThreadExecute;
	///	�X���b�h����������A�X���b�h�R�[���o�b�N�֐����J�n�������H
	CEvent* GetThreadExecute() { return &m_vThreadExecute; }
	//	�����s���Ȃ�΃V�O�i����ԂɂȂ�

	//	sleep����Ƃ��Ȃǂɑ҂ׂ���O�C�x���g
	CEvent	m_vExceptionEvent;
	virtual CEvent* GetExceptionEvent() { return &m_vExceptionEvent; }

	//	thread��start����܂ł̏��
	CEvent	m_vThreadStarting;
	CEvent*	GetThreadStarting() { return &m_vThreadStarting; }
	//	���J�n����܂ł̑҂���ԁ��m���V�O�i�����

	//	Create��Stop�����r�����邽�߂̃N���e�B�J���Z�N�V����
	CCriticalSection m_vCriticalSection;
	CCriticalSection* GetCriticalSection() { return& m_vCriticalSection;}

	smart_ptr<function_callback> m_fn;
	smart_ptr<function_callback> GetCallBack() const { return m_fn; }

	CLockObject	m_vLockObject;
	/*volatile*/ CLockObject* GetLockObject() { return &m_vLockObject;}
	//	����X���b�h�̂��߂�Lock Object

	bool	m_bCreatedThread;
	/*
		CreateThread�Ő������ꂽ�X���b�h�Ȃ̂��������t���O
		JumpThread�Ő������ꂽ�X���b�h�́A�I������WaitForSingleObject���Ă�
		�����Ȃ��̂ŁA�t���O�𗧂ĂĂ���
	*/

	size_t	m_nStackSize;
};

class IThreadManager {
public:
	///	�X���b�h�̒ǉ��ƍ폜
	virtual void	AddThread(IThread* pThread)=0;
	virtual void	DelThread(IThread* pThread)=0;
	virtual bool	StopAllThread()=0;
	virtual void	WaitAllThreadEnded()=0;
	virtual ~IThreadManager(){}
};

class CThreadManager : public IThreadManager {
/**
	Thread�𓝊�����ThreadManager
	����͎��class CThread ��������I�ɌĂяo�����

	���[�U�[���g���Ƃ�����Arun���\�b�h���炢
	����́A
		CThreadManager::RunThread(new CThreadXXX);
	�Ƃ����悤�Ɏg���B���Ƃ́A���L���͂��̃X���b�h�N���X��
	�ʓ|�����Ă����B
	
*/
public:
	static	CThreadManager* GetObj() { return m_obj.get(); }
	///	�������ŁAsingleton�ȓz�𓾂�

	///	�X���b�h�̒ǉ��ƍ폜
	virtual void	AddThread(IThread* pThread);
	virtual void	DelThread(IThread* pThread);

	///	���ׂẴX���b�h���I��������i�I����҂j
	virtual bool	StopAllThread();
	/*
		���łɑ��X���b�h��StopAllThread���Ăяo���A�I����������
		����΁Abalk���āA���̂Ƃ���fasle���Ԃ�
	*/

	///	���ׂẴX���b�h���I������̂�҂i�I��������킯�ł͂Ȃ��j
	virtual void	WaitAllThreadEnded();

	/**
		�X���b�h��ێ�����B���L���́A���̃N���X���ʓ|������
		�X���b�h�𑖂点��O�ɁA�����ɏ��L�����������Ă����΁A
		���̃X���b�h���I������Ƃ��ɃC���X�^���X�������I�ɍ폜�����
	*/
	static void	RegistThread(IThread* pThread)
		{ GetObj()->RegistThread(shared_ptr<IThread>(pThread)); }
//	static void RegistThread(const smart_ptr<IThread>& pThread)
//		{ GetObj()->_RegistThread(pThread); }
	static void RegistThread(const shared_ptr<IThread>& pThread)
		{ GetObj()->_RegistThread(pThread); }

	/**
		RegistThread�Ɠ��������L���܂ł��̃N���X���ʓ|�����邪�A�����
		IAppBase��Run���Ăяo�����Ƃ�ۏ؂���
	*/
	static LRESULT RunThread(IAppBase*pAppBase);
//	static LRESULT RunThread(const smart_ptr<IAppBase>& pAppBase);
	static LRESULT RunThread(const shared_ptr<IAppBase>& pAppBase);

	/**	
		RegistThread�Ɠ��������L���܂ł��̃N���X���ʓ|�����邪�A�����
		IThread��CreateThread���Ăяo�����Ƃ�ۏ؂���
	*/
	static LRESULT CreateThread(IThread*pThread);
//	static LRESULT CreateThread(const smart_ptr<IThread>& pThread);
	static LRESULT CreateThread(const shared_ptr<IThread>& pThread);

protected:
	//	���W�����v��ɂ��ā��ɃW�����v����
	virtual void _RegistThread(
		const shared_ptr<IThread>& pThread,
			//	���̃X���b�h����~����Ƃ��ɁA
		const shared_obj& obj=shared_obj()
			//	���̃I�u�W�F�N�g���폜����
		//	������n���Ƃ��́A�Q�ƃJ�E���g������Ȃ��悤��
		//	smart_ptr::inc_ref���Ă���A���̃|�C���^��n���Ă��K�v����
	);
public:

	CThreadManager():m_bStopAllThread(false) {}

	typedef vector<IThread*> threadlist; 
protected:

	/**
		�Ǘ����Ă���X���b�h�ꗗ
	*/

	threadlist m_apThreadList;
	threadlist* GetThreadList() { return& m_apThreadList; }
		//	������{����volatile�Ȃ̂����AVC++�́A
		//	�����o�ϐ��ւ̃A�N�Z�X�̓��W�X�^���t���Ȃ�

	/**
		����͏��L���Ă���X���b�h�ꗗ
	*/
	struct CThreadList {
		shared_ptr<IThread> pThread;	//	���̃X���b�h����~����Ƃ��ɁA
		shared_obj obj;		//	���̃I�u�W�F�N�g���폜����
		bool	bValid;					//	���́��Q�̃f�[�^�����o���K����������
			//	�ʃX���b�h����폜���Ȃ��ƁA�����Ŏ����̃X���b�h���폜���邱�Ƃ͏o���Ȃ��̂ŁD�D
		CThreadList():bValid(true){}
	};
	smart_list_ptr<CThreadList> m_apMyThreadList;
	smart_list_ptr<CThreadList>* GetMyThreadList()
		{ return& m_apMyThreadList; }
	void	GarbageThread();
	//	delete�}�[�N(CThreadList::bValid==false)�̂��Ă���
	//	�X���b�h���폜����(ThreadList�ɑ΂���CriticalLock�́A
	//	�|������ԂŌĂяo������)

	CLockObject m_lockobject;
	CLockObject* GetLockObject() { return &m_lockobject; }

	/////////////////////////////////////////////////////////////
	///	�S�X���b�h�̒�~�̂߂̐���
	volatile bool	m_bStopAllThread;
	CCriticalSection m_csStopAllThread;
	CCriticalSection* GetStopAllThreadLock() { return &m_csStopAllThread;}
	/**
		�S�X���b�h�̒�~�������o�Ă��邩�ǂ����̃t���O��
		����ɑ΂���N���e�B�J���Z�N�V����
		(latch)
	*/
	bool	isStopAllThread() {
	///	���̃��\�b�h�́A��x�Ăяo���ƁA�ȍ~��true��Ԃ�
		CCriticalLock cl(GetStopAllThreadLock());
		bool b = m_bStopAllThread;
		m_bStopAllThread = true;
		return b;
	}

	/////////////////////////////////////////////////////////////

	///	singleton�p�^�[��
	static	singleton<CThreadManager> m_obj;
};

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd

#endif
