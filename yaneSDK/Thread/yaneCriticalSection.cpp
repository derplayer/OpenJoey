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
		throw CRuntimeException("CCriticalSectionをEnterした回数だけLeaveしないまま終了している");
#endif
	}
}

void CCriticalSection::Enter(){
	::EnterCriticalSection(&m_csCriticalSection);
	m_dwLockedThread = ::GetCurrentThreadId();
	//	↑異なるスレッドからここに入ってくることはできない
	//	(CriticalSectionの定義より)
	m_nLockCount++;
}

void CCriticalSection::Leave(){
#ifdef USE_EXCEPTION
	if (m_nLockCount==0){
		throw CRuntimeException("CCriticalSectionをEnterしていないのにLeaveしている");
	}
#endif
	if (--m_nLockCount==0) {
		m_dwLockedThread = (DWORD)-1;
	}
	//	↑Leaveした直後に他スレッドがEnterする可能性がある
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
		throw CRuntimeException("CCriticalLock::Leave(Enterされていない)");
#endif
*/
		//	↑これは実はありうる
		//	例)CriticalLockしているなかで、いったんLeaveして、そのあと
		//		例外が発生してEnterしなおす前に抜けたなど..
		return ;
	}
	m_cs->Leave();
}

void	CCriticalLock::Enter(){
	m_cs->Enter();
	m_nLockCount++;
	//	↑ if (++m_nLockCount==0) { m_cs->Enter(); }
	//	とは書けない。なぜなら、++m_nLockCountがatomicでは無い
}

CCriticalLock::~CCriticalLock()
{
	//	guardされていたときのみ解放
	if (0<m_nLockCount){
		for(int i=0;i<m_nLockCount;i++) m_cs->Leave();
	}
}

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd
