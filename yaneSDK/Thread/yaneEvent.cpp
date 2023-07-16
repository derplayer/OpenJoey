#include "stdafx.h"
#include "yaneEvent.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

CEvent::CEvent(){
	m_hHandle = ::CreateEvent(NULL,FALSE,FALSE,NULL);
	m_bSignal = false;
}

CEvent::~CEvent(){
	if (m_hHandle!=NULL){
		::CloseHandle(m_hHandle);
	}
}
/////////////////////////////////////////////////////////////////////
void	CEvent::SetEvent(){
	CCriticalLock guard(GetCriticalSection());
	m_bSignal = true;
	::SetEvent(m_hHandle);
}

void	CEvent::ResetEvent(){
	CCriticalLock guard(GetCriticalSection());
	::ResetEvent(m_hHandle);
	m_bSignal = false;
}

LRESULT	CEvent::Wait(int nTime){
	DWORD dwResult;
	if (nTime == -1){
		dwResult = ::WaitForSingleObject(m_hHandle,INFINITE);
	} else {
		dwResult = ::WaitForSingleObject(m_hHandle,nTime);
	}

	switch (dwResult){
	case WAIT_OBJECT_0	:	//	所有権が獲得できた
	case WAIT_ABANDONED :	//	解放されたイベント
		return 0;
	case WAIT_TIMEOUT	:	//	タイムアウトになった
		return 1;
	default:
		//	しょれ以外って何よ？？
		return 2;
	}
}

bool	CEvent::IsSignal() const {
	CCriticalLock guard(const_cast<CEvent*>(this)->GetCriticalSection());
	/*
		この関数virtualになっているので
		m_bSignalをvolatileにする必要は無いのだが..
	*/
	return m_bSignal;
};

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd
