#include "stdafx.h"
#include "../Thread/yaneThread.h"
#include "../AppFrame/yaneAppBase.h"
#include <process.h> 

namespace yaneuraoGameSDK3rd {
namespace Thread {

//////////////////////////////////////////////////////////////////////////////
//	static members..
ThreadLocal<IThread*> IThread::pThread;

//////////////////////////////////////////////////////////////////////////////

CThread::CThread(){
	m_dwThreadHandle	 = (HANDLE)0;
	m_dwRunningThreadID	 = (DWORD)-1;
	//	-1�̓G���[�l�B(�X���b�h�n���h���Ƃ��Ă͂��肦�Ȃ��l)
	//	�i_beginthread�͗��_��͂O���Ԃ��Ă��邱�Ƃ�����j

	m_bCreatedThread = false;
	m_nStackSize = 0; // default
}

CThread::~CThread(){
	StopThread();
}

//	�C�x���g�X�e�[�^�X�̏�����
void	CThread::InitEvent(){
	GetExistEvent()->ResetEvent();	//	�m���V�O�i����Ԃɂ���
	//	�X���b�h�������Ă����Ԃ̂Ƃ��̓m���V�O�i�����

	GetThreadStarting()->ResetEvent();
	//	�X���b�h���J�n���Ă��Ȃ���Ԃ̂Ƃ��̓m���V�O�i�����

	GetExceptionEvent()->ResetEvent();	//	�m���V�O�i����Ԃɂ���
	//	��O���������Ă��Ȃ��Ƃ��́A�m���V�O�i�����

	GetThreadExecute()->ResetEvent();	//	�m���V�O�i����Ԃɂ���
	//	�X���b�h�����s����Ă��Ȃ��Ƃ��́A�m���V�O�i�����

	m_bCreatedThread = false;
	//	CreateThread�Ő������ꂽ�X���b�h�Ȃ��true
}

bool CThread::IsThreadValid()const{
	///	���荞�ݏ�ԂɂȂ��Ă��邩
	CThread* pThis = const_cast<CThread*>(this);
	return !pThis->GetExistEvent()->IsSignal();
}

bool CThread::IsThreadExecute()const /// �X���b�h�͎��s�����H
{
	CThread* pThis = const_cast<CThread*>(this);
	return pThis->GetThreadExecute()->IsSignal();
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CThread::CreateThread(const smart_ptr<function_callback>& fn) {
	CCriticalLock cs(GetCriticalSection());
	m_fn = fn;
	return CreateThread();
}

void CThread::SetThreadFunction(const smart_ptr<function_callback>& fn){
	CCriticalLock cs(GetCriticalSection());
	m_fn = fn;
}

LRESULT CThread::CreateThread() {
	CCriticalLock cs(GetCriticalSection());

	StopThread();
	InitEvent();
	m_bCreatedThread = true;

//	return !::CreateThread(NULL,0,ThreadCallBack,this,NULL,&m_dwThreadHandle);
//	m_dwThreadHandle = ::_beginthread(0,ThreadCallBack,0,(void*)this);
	HANDLE dwThreadHandle =
		(HANDLE)::_beginthreadex(0,GetStackSize(),ThreadCallBack,(void*)this,0
			,(unsigned int*)&m_dwRunningThreadID);
	if (dwThreadHandle ==0 ){
	/*
					�������ŃR���p�C���G���[���o��Ƃ�����A
		�����N���郉�C�u�������V���O���X���b�h�ɂȂ��Ă��邩��ł��B
		�v���W�F�N�g�̐ݒ�ˁuC/C++�v�ˁu�R�[�h�����v�ŁA
		�u�g�p���郉���^�C�����C�u�����v�́A
		�}���`�X���b�h�p�̂��̂��g�p���Ă��������B
	*/
		return -1;	//	�X���b�h�N���Ɏ��s
	}
	m_dwThreadHandle = dwThreadHandle;

	/**
		�X���b�h���N������̂�҂H
	*/
	return GetThreadStarting()->Wait();
		//	��_beginthreadex�́A�G���[���A-1���Ԃ��Ă���
	/*
		CreateThread�ł͂Ȃ�_beginthread�ŃX���b�h�𐶐����Ȃ��ƁA
		C����̃����^�C�����Ăяo���Ă���ꍇ�A���K�͂ȃ��������[�N����������B
		(80�o�C�g���x)
		���ƁA�R���p�C������Ƃ��́A�}���`�X���b�h�p�̃��C�u������
		�����N����悤�ɂ��Ă��������B

	�܂��A

   Thread.c�t�@�C����`���Ă݂�ƁA
1. _beginthread ���� CreateThread ��A������ ResumeThread
   ���R�[������B
2. ����ƁA_threadstart ���R�[�������B
3. _threadstart���ŃR�[���o�b�N�֐����R�[�������B
4. ��L�R�[���o�b�N�֐��I����A������ 
		_endthread ���R�[��
�@�@�����B

�@���ʁA�X���b�h�n���h���������ɂȂ�B
�@���������āAWaitForSingleObject������ɃX���b�h��
�R�[���o�b�N�֐����I�������ꍇ�A�����ȃX���b�h�n���h��
�ɑ΂��ҋ@����悤�ɂȂ�B

	����āA_beginthreadex�ő҂ׂ�

	*/
}

LRESULT CThread::StopThread() {
	CCriticalLock cs(GetCriticalSection());

	InvalidateThread();			//	�~�܂��ė~�����˂�I

	if (m_dwThreadHandle==(HANDLE)0) {
		return 0;
		//	�X���b�h���Ƃ��瓮���Ăւ�ŁH
	}

	//	����X���b�h�A�������g��������A����ȂƂ���Ŏ����̃X���b�h�̏I����҂����Ƃ����
	//	�f�b�h���b�N�ɂȂ邾��
	if (GetThreadId() == ::GetCurrentThreadId()) return 2;

	//	�����I�ɁA���̃X���b�h���폜����R�[�h�͊댯�Ȃ̂œ������Ȃ�

	DWORD dwResult = 0;	
	HANDLE dwThreadHandle = m_dwThreadHandle;
	m_dwThreadHandle = (HANDLE)0;
	m_dwRunningThreadID = (DWORD)-1;
	//	�������Ńn���h�����R�s�[���āA�N���A���Ă����Ȃ��ƁALeave�����r�[��
	//	���炽�ȃX���b�h��Create����āA�n���h�����㏑������Ă��܂�
	//	�\��������

	if (m_bCreatedThread){
		cs.Leave();
			//�@����lock�������܂ܑ҂��Ă�����A���[�J�[�X���b�h��Lock���l���ł�����
			//	dead lock�ɐ��蓾��
		dwResult = ::WaitForSingleObject(dwThreadHandle,INFINITE);
		cs.Enter();
	}
	//	�������҂͈̂ꌩ�A���S�ȃR�[�h�����A�f�b�h���b�N�ɂȂ肤��
	//	(�R��Ԃ����Ƃ�����̂��H�H��_beginthread �B_beginthreadex�Ȃ�ok)
	//	���@_beginthread�̂ق��́A�����I�ɃX���b�h�n���h�������悤��

	if (dwResult != WAIT_OBJECT_0) {
		//	�����ƁA���̃X���b�h�n���h���̏��L�������ł�
		//	�j������Ă����̂��Ǝv��..����͋C�ɂ��Ȃ��ŗǂ�
		return 1;
	}
	::CloseHandle(dwThreadHandle);
	// ���̃n���h�������I�ɕ��ă��\�[�X������Ƃ�����

	//	�I�������̂ŁD�D
	return 0;
}

LRESULT CThread::JumpToThread() {
	StopThread();
	InitEvent();

	m_bCreatedThread = false;

	m_dwRunningThreadID = ::GetCurrentThreadId();
	//	�X���b�hID�̕ۑ�
	//	(���͂���A�X���b�h�n���h���Ɠ������̂ł���͕̂ۏ؂���Ă���̂����D�D)

	{
		//	�ꉞ�X���b�h�}�l�[�W���ɓo�^���Ă���
		CThreadManager::GetObj()->AddThread(this);
		GetThreadExecute()->SetEvent();

		ThreadProc();				//	��������s����̂���

		GetThreadExecute()->ResetEvent();
		//	�X���b�h�}�l�[�W������폜����
		CThreadManager::GetObj()->DelThread(this);

	}

	m_dwRunningThreadID = (DWORD)-1;	//	�X���b�h�I�������ۂ�
	return 0;
}

//void CThread::ThreadCallBack(LPVOID lpVoid){
unsigned __stdcall CThread::ThreadCallBack( void * lpVoid){
	//	��_beginthreadex��p����̂ŕύX���܂���
	
	CThread* pThread = reinterpret_cast<CThread*>(lpVoid);

	pThread->m_dwRunningThreadID = ::GetCurrentThreadId();
	//	�X���b�hID�̕ۑ�

	//	�X���b�h�}�l�[�W���ɓo�^���Ă���
	CThreadManager::GetObj()->AddThread(pThread);

	//	���̃t���O�͓ǂݎ���p�Ȃ̂ł����ŏ��������ėǂ�
	const smart_ptr<function_callback> fn(pThread->GetCallBack());
	pThread->GetThreadExecute()->SetEvent();
		//	�����̃t���O���������ƁA���̃X���b�h�𐶐������X���b�h��
		//	CreateThread���甲���āA���̂���SetFunction��CallBack���ׂ�
		//	�֐������������Ă��܂��A�����ŏՓ˂��邱�Ƃ�����
	pThread->GetThreadStarting()->SetEvent();

	IThread::setThread(pThread);	//	�X���b�h��ݒ肵�Ă���

	if (fn.isNull()){
		pThread->ThreadProc();
	} else {
		fn->run();	//	�R�[���o�b�N����
		pThread->m_fn.Delete();			//	function_callback����̂���
	}

	IThread::unsetThread();		//	�ݒ肵�Ă������X���b�h*���폜

	/**
		Thread��O�̂��߂�Invalidate���āA
		���̃X���b�h�ɑ΂���wait���Ă���X���b�h�ɒʒm���Ă��
	*/
	pThread->InvalidateThread();

	//	�X���b�h�}�l�[�W������폜����
	CThreadManager::GetObj()->DelThread(pThread);
		//	DeleteThread�ɂ�����StopThread�����

	return 0;
}

void	CThread::InvalidateThread(){
	CCriticalLock cs(GetCriticalSection());
	{
		CCriticalLock cl(GetLockObject()->GetCriticalSection());

		//	�҂����X�g�ɓ����Ă���z���A������Ă��
		GetLockObject()->notifyThreadInWaitList(GetThreadId());

		//	�����I�u�W�F�N�g���V�O�i����Ԃɂ��Ă��
		GetExistEvent()->SetEvent();		//	���݂��Ȃ�
//		GetThreadExecute()->ResetEvent();	//	���s����Ă��Ȃ�
	}
}

//////////////////////////////////////////////////////////////////////////////
//	Java�݊�Thread�@�\���A�y����Ȃ���D�D

LRESULT	CThread::wait(int nTimeOut /*=-1*/)
		#ifdef USE_EXCEPTION
			throw(CInterruptedException)
		#endif
{
	///		wait �L���[�ɕ���
	if (interrupted()){
		#ifdef USE_EXCEPTION
			throw CInterruptedException();
		#endif
	}
	LRESULT lr = GetLockObject()->wait(nTimeOut);
	/*	//	����͐S�z����ł�CLockObject���������
	if (interrupted()){
		#ifdef USE_EXCEPTION
			throw CInterruptedException();
		#endif
	}
	*/
	return lr;
}

LRESULT CThread::sleep(int nTime)
		#ifdef USE_EXCEPTION
			throw(CInterruptedException)
		#endif
{
	//	nTime [ms]�҂B�҂��Ă���Œ��ɁA�����I�u�W�F�N�g���V�O�i�����
	//	�i�X���b�h���I�������Ȃ����A�Ƃ�����ԁj�ɂȂ�΁A��0���Ԃ�B

	if (interrupted()){
		#ifdef USE_EXCEPTION
			throw CInterruptedException();
		#endif
	}

	const HANDLE h[] = {
		//	��O�C�x���g�Ɛ����C�x���g�Ƃ̗������Ď�����K�v������
		GetExistEvent()->GetHandle(),
		GetExceptionEvent()->GetHandle()
	};

	DWORD dwResult =
		::WaitForMultipleObjects(
			2,&h[0],FALSE,nTime
		);

	if (interrupted()){
		#ifdef USE_EXCEPTION
			throw CInterruptedException();
		#endif
	}
	return dwResult == WAIT_TIMEOUT ? 0:1;
}

void	CThread::notify(){
	/// wait�ő҂��Ă���X���b�h��@���N����(1�̂�)
	GetLockObject()->notify();
}

void	CThread::notifyAll(){
	/// wait�ő҂��Ă��邷�ׂẴX���b�h��@���N����
	GetLockObject()->notifyAll();
}

void	CThread::interrupt(){
	/**
		���荞�݂�������B���̊֐��́A���X���b�h����Ăяo���B
		��������ƁA���̃X���b�h�́A����wait��sleep��
			CInterruptedException��O�𔭐�������B
	*/
	CCriticalLock cl(GetLockObject()->GetCriticalSection());
	GetLockObject()->eraseThreadInWaitList(GetThreadId());
		//	�҂��Ă���Ȃ�΁A���̑҂����폜����
	GetExceptionEvent()->SetEvent();
	//	sleep���Ă���\��������̂ŁA������L���ɂ��Ă��

}

bool	CThread::isInterrupted() const{
	///	���荞�ݏ�ԂɂȂ��Ă��邩
	CThread* pThis = const_cast<CThread*>(this);
	return pThis->GetExceptionEvent()->IsSignal();
}

bool	CThread::interrupted() {
	///	���荞�ݏ�ԂɂȂ��Ă��邩��Ԃ����A���̌�A
	///	���荞�ݏ�Ԃ̓��Z�b�g����B
	CCriticalLock cl(GetLockObject()->GetCriticalSection());
		//	�ꉞLock���Ă������B

	bool bInterrupted = GetExceptionEvent()->IsSignal();
	GetExceptionEvent()->ResetEvent();
	return bInterrupted;
}

//////////////////////////////////////////////////////////////////////////////
//	ThreadManager
//////////////////////////////////////////////////////////////////////////////
singleton<CThreadManager> CThreadManager::m_obj;

void	CThreadManager::AddThread(IThread* pThread){
	CCriticalLock cl(GetLockObject()->GetCriticalSection());
	//	�d���o�^�܂ŐS�z����ł����ł���
	GetThreadList()->push_back(pThread);
	GetLockObject()->notifyAll();
}

void	CThreadManager::DelThread(IThread* pThread){
	CCriticalLock cl(GetLockObject()->GetCriticalSection());

	{
		threadlist::iterator it =
			find(GetThreadList()->begin(),GetThreadList()->end(),pThread);
		if (it!=GetThreadList()->end()){
		//	�������͂���������₯�ǂȂ��D�D
			GetThreadList()->erase(it);
		}
	}

	//	���̂��ƁA���������L���������Ă���X���b�h���I�������Ȃ�΁A
	//	�����erase���Ă���
	{
		smart_list_ptr<CThreadList>::iterator it =
			GetMyThreadList()->begin();
		while (it!=GetMyThreadList()->end()){
			if ((*it)->pThread.get() == pThread){
			//	�������Ă�����̂������ł���΁D�D
//				it = GetMyThreadList()->erase(it);
				//	�����̃X���b�h���������폜���邱�Ƃ͏o���Ȃ�
				(*it)->bValid = false; // delete�}�[�N������
				it++;
			} else {
				it++;
			}
		}
	}

	GetLockObject()->notifyAll();
}

bool	CThreadManager::StopAllThread() {
	if (isStopAllThread()) return false;
	/*
		�S�X���b�h�̒�~���������łɏo�Ă���̂Ȃ�΁A
		���̃X���b�h����~�����悤�Ƃ��Ă��A
		���̒�~�������o���Ă���X���b�h���~�����邱�Ƃ�
		�o���Ȃ��̂ŁA�f�b�h���b�N�Ɋׂ�B
		����āA�����ł́Abalk���ׂ�
	*/

	CCriticalLock cl(GetLockObject()->GetCriticalSection());

	GarbageThread();	//	�K�[�x�W�������ʁA���ׂďI�����Ă��邩���m��Ȃ�

			//	���������g�̃X���b�h�����ɓ���͂��Ȃ̂ŁA1�ȉ��������Ƃ���
	while (!(
				(
					(GetThreadList()->empty())
				||
					(
						(GetThreadList()->size()==1)
					&&
						((*GetThreadList()->begin())->GetThreadId()
							==GetCurrentThreadId())
					)
				)
			&&
			//	�Ǘ����Ă���X���b�h���������A�P�������Ƃ��Ă��A���ꂪ���̃X���b�h���g�ł��邩
				(
					(GetMyThreadList()->empty())
				||
					(
						(GetMyThreadList()->size()==1)
					&&
						((*GetMyThreadList()->begin())->pThread->GetThreadId()
							==GetCurrentThreadId())
					)
				)
			)){

				/** /	//	debug�p�e�X�g�R�[�h
				int n1 = GetThreadList()->size();
				int n2 = GetMyThreadList()->size();
				DWORD n3 = (*GetThreadList()->begin())->GetThreadId();
				DWORD n4 = (*GetMyThreadList()->begin())->pThread->GetThreadId();
				DWORD n5 = GetCurrentThreadId();
				//*/

	///	GetMyThreadList�́A���ꂩ����s�����ׂ��X���b�h���܂܂��
	///	�v����ɁAGetThreadList�Ŏ擾�����X���b�h������empty�ł������Ƃ��Ă�
	///	GetMyThreadList��empty�łȂ���΁A�I�������𖞂����Ă��Ȃ����ƂɂȂ�
	//	�S�X���b�h�ɒ�~���߂�����
		threadlist::iterator it = GetThreadList()->begin();
		while (it!=GetThreadList()->end()){
//			(*it)->StopThread();
			/**
				���̃X���b�h�����̃X���b�h���~�����悤�ɂ��A
				���̃X���b�h���I������Ƃ��ɁA��L�̃X���b�h���X�g����
				��������菜�����Ƃ��邪�A���̂��ߏ�L�`�F�C����
				critical lock�������邱�ƂɂȂ��āA�����Ńf�b�h���b�N�Ɋׂ�B
			*/
			(*it)->InvalidateThread();	//	���������H
			it++;
		}
		//	�����񂱂���s�Ȃ�Ȃ��ƁA
		//	�V���ȃX���b�h���쐬�����\��������
		GetLockObject()->wait();
		GarbageThread();
	}
	return true;
}

//	���ׂẴX���b�h���I������̂�҂�
void	CThreadManager::WaitAllThreadEnded() {
	CCriticalLock cl(GetLockObject()->GetCriticalSection());

	GarbageThread();	//	�K�[�x�W�������ʁA���ׂďI�����Ă��邩���m��Ȃ�
	while (!(GetThreadList()->empty() && GetMyThreadList()->empty())){
	///	GetMyThreadList�́A���ꂩ����s�����ׂ��X���b�h���܂܂��
	///	�v����ɁAGetThreadList�Ŏ擾�����X���b�h������empty�ł������Ƃ��Ă�
	///	GetMyThreadList��empty�łȂ���΁A�I�������𖞂����Ă��Ȃ����ƂɂȂ�
		GetLockObject()->wait();
		//	����wait���Ăяo����busy wait�ɂ͂Ȃ�Ȃ�
		GarbageThread();
	}
}

void	CThreadManager::_RegistThread(
	const shared_ptr<IThread>& pThread,
	const shared_obj&	obj/*=NULL*/
){
	CCriticalLock cl(GetLockObject()->GetCriticalSection());
	CThreadList* p = new CThreadList;
	p->pThread	= pThread;
	p->obj = obj;
	GetMyThreadList()->push_back(smart_ptr<CThreadList>(p));
}

void	CThreadManager::GarbageThread(){
	//	lock�͂����Ȃ�����A�O����lock���Ƃ��Ă�
	//	���̂��ƁA���������L���������Ă���X���b�h���I�������Ȃ�΁A
	//	�����erase���Ă���
	{
		smart_list_ptr<CThreadList>::iterator it =
			GetMyThreadList()->begin();
		while (it!=GetMyThreadList()->end()){
			if (!(*it)->bValid){
			//	delete�}�[�N�����Ă���Ȃ��
				it = GetMyThreadList()->erase(it);
			} else {
				it++;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////

///	RunThread�̏ꍇ�́ACAppFrame��Run���Ăяo�����Ƃ�ۏ؂���
LRESULT CThreadManager::RunThread(IAppBase*pAppBase){
	return RunThread(shared_ptr<IAppBase>(pAppBase));
}

LRESULT CThreadManager::RunThread(const shared_ptr<IAppBase>& pAppBase){
	shared_ptr<IAppBase>* p = new shared_ptr<IAppBase>(pAppBase);

	shared_ptr<IThread> pThread(pAppBase->GetWorkerThread(),false);
		//	�����̃X���b�h�̒�~���A���̃I�u�W�F�N�g�̉�̃t�F�C�Y�Ƃ���

	GetObj()->_RegistThread(pThread,shared_obj(p));
	//	smart_ptr_base�ɃL���X�g�����

	return pAppBase->Run();	//	���̂��Ǝ��s����
}

///	CreateThread�̏ꍇ�́AIThread��CreateThread���Ăяo�����Ƃ�ۏ؂���
LRESULT CThreadManager::CreateThread(IThread*pThread){
	return CreateThread(shared_ptr<IThread>(pThread));
}

LRESULT CThreadManager::CreateThread(const shared_ptr<IThread>& pThread){
	RegistThread(pThread);
	return pThread->CreateThread();	//	�X���b�h�̍쐬
}

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd
