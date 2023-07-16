/*
	�X���b�h�ŗL�̃I�u�W�F�N�g��p�ӂ��邽�߂̃e���v���[�g

		programmed & designed by yaneurao '02/03/10
*/

#ifndef __YTLThreadLocal_h__
#define __YTLThreadLocal_h__


#include "../Thread/yaneCriticalSection.h"
#include "../YTL/singleton.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

class ThreadLocalBase;
class ThreadLocalBaseHelper {
/**
	class ThreadLocalBase �̂��߂�Singleton�I�u�W�F�N�g
	�i��local�ȃI�u�W�F�N�g�̏����������͕s��̂��߂��������̂��K�v�ɂȂ�j

	class ThreadLocalBase ����Ăяo�����̂ŁA
	�����̊֐��͒��ڌĂяo���Ďg���Ƃ������̂ł͂Ȃ��B
*/
public:
	static ThreadLocalBaseHelper* GetObj(){ return m_obj.get(); }

	///	�`�F�C���ɒǉ��^�`�F�C���������
	void	insert(ThreadLocalBase*p);
	void	erase(ThreadLocalBase*p);

	///	�X���b�h����~����Ƃ��ɂ��̃X���b�h����Ăяo�����
	void	OnThreadEnd();

	///	WinMain���甲����Ƃ��ɁACAppInitializer����R�[���o�b�N�����
	void	OnExit();

	//	�������ACAppInitalizer�ɁAOnExit�̃R�[���o�b�N���˗�
	ThreadLocalBaseHelper();

protected:
	static singleton<ThreadLocalBaseHelper> m_obj;

	///	�`�F�C���̎擾
	list_chain<ThreadLocalBase>* GetChain(){ return &m_threadlocal_list;}
	list_chain<ThreadLocalBase>	m_threadlocal_list;

	//	�����̃`�F�C����������Ƃ��̂��߂̃N���e�B�J���Z�N�V����
	CCriticalSection	ms_cr;
	CCriticalSection*	GetCriticalSection() { return &ms_cr; }
};

class ThreadLocalBase {
/**
	class ThreadLocal �e���v���[�g�̂��߂̊��N���X
	class ThreadLocal ���Q�Ƃ̂��ƁB
*/
public:
	virtual void	Clear()=0;
	virtual void	ClearAll()=0;
	virtual void	SetExit(bool)=0;
	virtual ~ThreadLocalBase(){} // merely place holder

	static void	OnThreadEnd()
	{	ThreadLocalBaseHelper::GetObj()->OnThreadEnd(); }
	/**
		�X���b�h�͏I������Ƃ��ɕK�����̊֐����Ăяo���āA��������
		���ׂĂ�ThreadLocal�̃C���X�^���X��Clear���Ăяo���Ȃ����
		�Ȃ�Ȃ�
	*/

	///	�`�F�C���ɒǉ��^�`�F�C���������
	static void	insert(ThreadLocalBase*p)
	{	ThreadLocalBaseHelper::GetObj()->insert(p); }
	static void	erase(ThreadLocalBase*p)
	{	ThreadLocalBaseHelper::GetObj()->erase(p);	}

	///	WinMain���甲����O�ɁA���̊֐����Ăяo���āA
	///	�`�F�C����f���؂��Ă����K�v������
	///	�������A���̊֐���CAppInitializer���玩���I��
	///	�R�[���o�b�N�����̂Ń��[�U�[�͌Ăяo���K�v�͖���
	static void OnExit()
	{	ThreadLocalBaseHelper::GetObj()->OnExit(); }

};

template <class T>
class ThreadLocal : public ThreadLocalBase {
/**
	�X���b�h�ŗL�̃I�u�W�F�N�g��p�ӂ��邽�߂̃e���v���[�g�N���X

	ThreadLocal<HWND> hWnd;

	�Ƃ��΁A
	hWnd = GetHWnd();
	�Ƃ����悤�ɃX���b�h�ŗL�̕ϐ��������I�ɗp�ӂ���āA
	�����ɑ�����o����B

	���邢�́A
	ThreadLocal<HWND> hWnd;
	HWND	h = hWnd;
	�Ƃ����悤�ɁA��������̑�����o����B

	���ׂẴX���b�h�͏��ł���Ƃ��ɁA
	class CThreadLocalBase::OnThreadEnd() ���Ăяo���āA��������
	ThreadLocal�̃C���X�^���X���ׂĂ�Clear���Ăяo���Ȃ���΂Ȃ�Ȃ��B

	�i�Ō�̃X���b�h�́A���̃N���X�̃f�X�g���N�^���ۗL����I�u�W�F�N�g��
	��̂��Ă����̂ŁA���̊֐����Ăяo���K�v�͖����j

	���@�Q�l�ɂ���Ɨǂ��N���X
	class CCOMBase
*/
public:
	///	�f�[�^�̐ݒ�^�擾
	void	Set(const T& t);
	T*		Get();

	///	�v�f���܂��������Ă��Ȃ������e�X�g����
	bool	isEmpty();

	///	�Öٕϊ��q
	operator T() { return *Get(); }

	///	������Z�q
	const T operator = (const T& t){
		Set(t);
		return *Get();
	}

	///	���̃X���b�h�ɑ�����f�[�^���N���A
	virtual void	Clear();

	///	�S�X���b�h�ɑ�����f�[�^���N���A
	virtual void	ClearAll();

	///	�I���������I����Ă���̂Ȃ�΁A�`�F�C������O�����̑���͕s�v
	virtual void	SetExit(bool b) { m_bExit = b; }

	ThreadLocal(): m_bExit(false)
		{ ThreadLocalBase::insert(this); }
	virtual ~ThreadLocal() {
		ClearAll();
		if (!m_bExit) { // �I���������I����Ă��Ȃ��Ƃ��̂�
			ThreadLocalBase::erase(this);
		}
	}

	//	processId �� HWND��map
	typedef map <DWORD,T*> mymap;

protected:
	mymap	m_map;
	mymap* GetMap() { return &m_map; }

	CCriticalSection	m_cr;
	CCriticalSection*	GetCriticalSection() { return &m_cr; }

	bool	m_bExit;
};

///////////////////////////////////////////////////////////////////////

template <class T>
T*	ThreadLocal<T>::Get() {
	/// ���̃X���b�h�ɑΉ�����f�[�^�̎擾
	DWORD	dwThreadID = ::GetCurrentThreadId();

	CCriticalLock cl(GetCriticalSection());

	mymap::iterator it;
	it = GetMap()->find(dwThreadID);
	T* p;
	if (it==GetMap()->end()){
		//	�������H�Hnew���邩�H
		p = new T; // new���Ă܂��ŁI
		GetMap()->insert(pair<DWORD,T*>(dwThreadID,p));
		//	insert���Ƃ����ƁB

		//	get�ő��݂��Ȃ��͖̂��̂悤�ȋC������Ȃ�..
	} else {
		p = it->second;
	}

	return p;
}

template <class T>
void	ThreadLocal<T>::Set(const T& t){
	///	���̃X���b�h�ɑΉ�����T�̐ݒ�
	DWORD	dwThreadID = ::GetCurrentThreadId();

	CCriticalLock cl(GetCriticalSection());

	mymap::iterator it;
	it = GetMap()->find(dwThreadID);

	T* p;
	if (it==GetMap()->end()){
		//	�������H�H
		p = new T; // new���Ă܂��ŁI
		*p = t;	//	�R�s���Ă��܂��B
		GetMap()->insert(pair<DWORD,T*>(dwThreadID,p));
	} else {
		*(it->second) = t;
	}

}

template <class T>
void	ThreadLocal<T>::Clear() {
	/// ���̃X���b�h�ɑΉ�����f�[�^�̎擾
	DWORD	dwThreadID = ::GetCurrentThreadId();

	CCriticalLock cl(GetCriticalSection());

	//	�e���v���[�g���ł�using std::map���Ă���̂������炵�� > VC++6(;�L�D`)
	mymap::iterator it;
	it = GetMap()->find(dwThreadID);
	if (it==GetMap()->end()){
		//	�������H�H
	} else {
		delete it->second;		//	���̗v�f������
		GetMap()->erase(it);	//	�����Ă܂��ŁI
	}
}

template <class T>
void	ThreadLocal<T>::ClearAll() {
	CCriticalLock cl(GetCriticalSection());
	mymap::iterator it;
	it = GetMap()->begin();
	while (it!=GetMap()->end()){
		delete it->second;
		it++;
	}
	GetMap()->clear();
}

template <class T>
bool	ThreadLocal<T>::isEmpty() {
	DWORD	dwThreadID = ::GetCurrentThreadId();
	CCriticalLock cl(GetCriticalSection());

	mymap::iterator it;
	it = GetMap()->find(dwThreadID);
	bool b = (it==GetMap()->end());	//	���쐬�f�[�^

	return b;
}

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd

#endif
