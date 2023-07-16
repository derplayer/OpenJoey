//	yaneTimer.cpp
#include "stdafx.h"
#include "yaneTimer.h"

namespace yaneuraoGameSDK3rd {
namespace Timer {

//////////////////////////////////////////////////////////////////////////////

CTimeGetTimeWrapper::CTimeGetTimeWrapper() {
	if (::timeGetDevCaps(&m_dwTimeCaps,sizeof(TIMECAPS))!=TIMERR_NOERROR) {
		//	なんや？タイマ腐ってんのか？
		m_dwTimeCaps.wPeriodMin = 1;	//	のつもりになる:p
		m_bUseTGT	= false;			// GetTickCountを使用する
		return ;
	}
	if (m_dwTimeCaps.wPeriodMin>=10) {
		//	これより大きいと使いものにならん
		m_dwTimeCaps.wPeriodMin = 1;		//	のつもりになる:p
		m_bUseTGT	= false;		// GetTickCountを使用する
	} else {
		m_bUseTGT	= true;			// timeGetTimeを使用する

		//	最小時間を設定する
		::timeBeginPeriod(m_dwTimeCaps.wPeriodMin);
	}
}

CTimeGetTimeWrapper::~CTimeGetTimeWrapper(){
	if (m_bUseTGT){
		::timeEndPeriod(m_dwTimeCaps.wPeriodMin);
	}
}

//////////////////////////////////////////////////////////////////////////////

ref_creater<CTimeGetTimeWrapper>	CTimeGetTimeWrapper::m_vTimeGetTime;

CTimer::CTimer(){
	GetRefObj()->inc_ref();
	Reset();
}

CTimer::~CTimer(){
	GetRefObj()->dec_ref();
}

DWORD CTimer::Get() {
	if (m_bPaused>0) return m_dwPauseTime-m_dwOffsetTime;
	return (DWORD)(GetObj()->GetTime() - m_dwOffsetTime);
}

void CTimer::Set(DWORD dw) {
	if (m_bPaused>0) {
		//	いらんかな～？
		m_dwOffsetTime = m_dwPauseTime-dw;
	} else {
		m_dwOffsetTime = (DWORD)(GetObj()->GetTime() - dw);
	}
}

void CTimer::Reset() {
	m_dwOffsetTime = GetObj()->GetTime();
	m_bPaused = 0;
}

void CTimer::Pause(){
	if (m_bPaused++ == 0) {
	//	Pauseさせる
		m_dwPauseTime = GetObj()->GetTime();
	}
}

void CTimer::Restart(){
	if (--m_bPaused == 0) {
	//	Pause解除
		m_dwOffsetTime += GetObj()->GetTime() - m_dwPauseTime;
	}
}

//////////////////////////////////////////////////////////////////////////////
//	class CFixTimer

CFixTimer::CFixTimer() { m_dwTimeGetTime = 0; }

void	CFixTimer::Reset() { GetTimer()->Reset(); }
DWORD	CFixTimer::Get(){ return m_dwTimeGetTime; }
void	CFixTimer::Set(DWORD dw) { GetTimer()->Set(dw); }
void	CFixTimer::Pause() { GetTimer()->Pause(); }
void	CFixTimer::Restart(){ GetTimer()->Restart(); }

void	CFixTimer::Flush(){
	m_dwTimeGetTime = GetTimer()->Get();
}

} // end of namespace Timer
} // end of namespace yaneuraoGameSDK3rd
