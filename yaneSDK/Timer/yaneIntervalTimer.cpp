//	yaneIntervalTimer.cpp
#include "stdafx.h"
#include "yaneIntervalTimer.h"
#include "yaneTimer.h"

namespace yaneuraoGameSDK3rd {
namespace Timer {

CIntervalTimer::CIntervalTimer(){
	m_pTimer.Add(new CTimer);	//	ディフォルトタイマーは、これ
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
		p->m_dwCallBackTimer= GetTimer()->Get()+dwNext;	//	現在時刻
		p->m_dwInterval		= dwInterval;
		p->m_nRepeat		= nRepeat;
		p->m_fnCallBack		=	fn;
		p->m_bDelete		= false;
	}
	GetTimerInfo()->insert(p);
	//	そのまま登録
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
	DWORD dwNowTimer = GetTimer()->Get();	//	現在の時刻
	while (it!=GetTimerInfo()->end()){
		bool	bDelete = false;
		CTimerInfo& info = *(it->get());

		bDelete |= info.m_bDelete;	//	Deleteマークを見る

		//	時刻が経過する回数だけコールバック
		while (((int)(dwNowTimer-info.m_dwCallBackTimer))>=0 && !bDelete){
		//	↑引き算を無符号（DWORD）同士で行なって、
		//	それがマイナスであれば
			info.m_fnCallBack->run();

			int& nRepeat = info.m_nRepeat;
			if ((nRepeat!=0) && (--nRepeat==0)){ //	0ならば無限に呼び出す
				//	0になったので、削除
				bDelete = true;
			} else {
				info.m_dwCallBackTimer+=info.m_dwInterval;
				//	次のコールバックする時刻にずらす
			}
		}
		if (bDelete) {
			it = GetTimerInfo()->erase(it);
		} else {
			it ++;
		}
	}
	//	Deleteマークのついているやつは、
	//	次のフェイズにおいて、自動的に削除される
}

void	CIntervalTimer::SetTimer(const smart_ptr<ITimer>& pTimer){
	m_pTimer = pTimer;
}

} // end of namespace Timer
} // end of namespace yaneuraoGameSDK3rd
