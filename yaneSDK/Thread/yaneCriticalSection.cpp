#include "stdafx.h"
#include "yaneCriticalSection.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

CCriticalSection::CCriticalSection(){
	::InitializeCriticalSection(&m_csCriticalSection);
	m_dwLockedThread = (DWORD)-1;
	m_nLockCount	= 0;
}

CCriticalSection::~CCriticalSection(){
	::DeleteCriticalSection(&m_csCriticalSection);
	if (m_nLockCount!=0) {
#ifdef USE_EXCEPTION
		throw CRuntimeException("CCriticalSection��Enter�����񐔂���Leave���Ȃ��܂܏I�����Ă���");
#endif
	}
}

void CCriticalSection::Enter(){
	::EnterCriticalSection(&m_csCriticalSection);
	m_dwLockedThread = ::GetCurrentThreadId();
	//	���قȂ�X���b�h���炱���ɓ����Ă��邱�Ƃ͂ł��Ȃ�
	//	(CriticalSection�̒�`���)
	m_nLockCount++;
}

void CCriticalSection::Leave(){
#ifdef USE_EXCEPTION
	if (m_nLockCount==0){
		throw CRuntimeException("CCriticalSection��Enter���Ă��Ȃ��̂�Leave���Ă���");
	}
#endif
	if (--m_nLockCount==0) {
		m_dwLockedThread = (DWORD)-1;
	}
	//	��Leave��������ɑ��X���b�h��Enter����\��������
	::LeaveCriticalSection(&m_csCriticalSection);
}

bool CCriticalSection::IsLockedByThisThread() const{
	DWORD dw = ::GetCurrentThreadId();
	return m_dwLockedThread == dw;
}

/////////////////////////////////////////////////////////////////////

CCriticalLock::CCriticalLock(ICriticalSection* cs): m_cs(cs),m_nLockCount(0)
{
	Enter();
}

void	CCriticalLock::Leave(){
	if (m_nLockCount-- == 0){
/*
#ifdef USE_EXCEPTION
		throw CRuntimeException("CCriticalLock::Leave(Enter����Ă��Ȃ�)");
#endif
*/
		//	������͎��͂��肤��
		//	��)CriticalLock���Ă���Ȃ��ŁA��������Leave���āA���̂���
		//		��O����������Enter���Ȃ����O�ɔ������Ȃ�..
		return ;
	}
	m_cs->Leave();
}

void	CCriticalLock::Enter(){
	m_cs->Enter();
	m_nLockCount++;
	//	�� if (++m_nLockCount==0) { m_cs->Enter(); }
	//	�Ƃ͏����Ȃ��B�Ȃ��Ȃ�A++m_nLockCount��atomic�ł͖���
}

CCriticalLock::~CCriticalLock()
{
	//	guard����Ă����Ƃ��̂݉��
	if (0<m_nLockCount){
		for(int i=0;i<m_nLockCount;i++) m_cs->Leave();
	}
}

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd
