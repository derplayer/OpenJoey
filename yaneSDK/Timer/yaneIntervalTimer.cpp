//	yaneIntervalTimer.cpp
#include "stdafx.h"
#include "yaneIntervalTimer.h"
#include "yaneTimer.h"

namespace yaneuraoGameSDK3rd {
namespace Timer {

CIntervalTimer::CIntervalTimer(){
	m_pTimer.Add(new CTimer);	//	�f�B�t�H���g�^�C�}�[�́A����
}

CIntervalTimer::~CIntervalTimer(){
}

void	CIntervalTimer::RegistCallBack(void*pVoid,DWORD dwNext,
		DWORD dwInterval,int nRepeat,
		const smart_ptr<function_callback>&fn)
{
	CTimerInfo* p = new CTimerInfo;
	{
		p->m_pVoid			= pVoid;
		p->m_dwCallBackTimer= GetTimer()->Get()+dwNext;	//	���ݎ���
		p->m_dwInterval		= dwInterval;
		p->m_nRepeat		= nRepeat;
		p->m_fnCallBack		=	fn;
		p->m_bDelete		= false;
	}
	GetTimerInfo()->insert(p);
	//	���̂܂ܓo�^
}

void	CIntervalTimer::DeleteCallBack(void*pVoid){
	CTimerList::iterator it = GetTimerInfo()->begin();
	while (it!=GetTimerInfo()->end()){
		if ((*it)->m_pVoid == pVoid){
			(*it)->m_bDelete = true;
		}
		it++;
	}
}

void	CIntervalTimer::CallBack(){
	CTimerList::iterator it = GetTimerInfo()->begin();
	DWORD dwNowTimer = GetTimer()->Get();	//	���݂̎���
	while (it!=GetTimerInfo()->end()){
		bool	bDelete = false;
		CTimerInfo& info = *(it->get());

		bDelete |= info.m_bDelete;	//	Delete�}�[�N������

		//	�������o�߂���񐔂����R�[���o�b�N
		while (((int)(dwNowTimer-info.m_dwCallBackTimer))>=0 && !bDelete){
		//	�������Z�𖳕����iDWORD�j���m�ōs�Ȃ��āA
		//	���ꂪ�}�C�i�X�ł����
			info.m_fnCallBack->run();

			int& nRepeat = info.m_nRepeat;
			if ((nRepeat!=0) && (--nRepeat==0)){ //	0�Ȃ�Ζ����ɌĂяo��
				//	0�ɂȂ����̂ŁA�폜
				bDelete = true;
			} else {
				info.m_dwCallBackTimer+=info.m_dwInterval;
				//	���̃R�[���o�b�N���鎞���ɂ��炷
			}
		}
		if (bDelete) {
			it = GetTimerInfo()->erase(it);
		} else {
			it ++;
		}
	}
	//	Delete�}�[�N�̂��Ă����́A
	//	���̃t�F�C�Y�ɂ����āA�����I�ɍ폜�����
}

void	CIntervalTimer::SetTimer(const smart_ptr<ITimer>& pTimer){
	m_pTimer = pTimer;
}

} // end of namespace Timer
} // end of namespace yaneuraoGameSDK3rd
