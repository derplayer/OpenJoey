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
	case WAIT_OBJECT_0	:	//	���L�����l���ł���
	case WAIT_ABANDONED :	//	������ꂽ�C�x���g
		return 0;
	case WAIT_TIMEOUT	:	//	�^�C���A�E�g�ɂȂ���
		return 1;
	default:
		//	�����ȊO���ĉ���H�H
		return 2;
	}
}

bool	CEvent::IsSignal() const {
	CCriticalLock guard(const_cast<CEvent*>(this)->GetCriticalSection());
	/*
		���̊֐�virtual�ɂȂ��Ă���̂�
		m_bSignal��volatile�ɂ���K�v�͖����̂���..
	*/
	return m_bSignal;
};

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd
