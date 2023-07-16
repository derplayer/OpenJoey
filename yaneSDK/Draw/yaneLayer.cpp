
#include "stdafx.h"
#include "yaneLayer.h"
#include "yaneSurface.h"

//////////////////////////////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Draw {

///		LayerListをコンストラクタで渡せば、自動的にフックする
ILayerBase::ILayerBase(ILayerList*pLayerList)
	:	m_nX(0),m_nY(0),m_bEnable(true),m_pLayerList(pLayerList)
{
	if (pLayerList!=NULL) { pLayerList->Add(this); }
}

void	ILayerBase::SetLayerList(ILayerList*pLayerList)
/**
		LayerListを設定する。(その瞬間にフックする)
*/
{
	if (m_pLayerList!=NULL) { m_pLayerList->Del(this); }
	if (pLayerList!=NULL) { pLayerList->Add(this); }
	m_pLayerList = pLayerList;
}

///		デストラクタで、LayerListからフックしていたのを外す
ILayerBase::~ILayerBase(){
	if (m_pLayerList!=NULL) { m_pLayerList->Del(this); }
}

//////////////////////////////////////////////////////////////////////////////

void CLayerList::Add(ILayerBase* hook)
{
	GetList()->push_back(hook);
}

void CLayerList::Del(ILayerBase* hook){

	// 自分がHookしたやつを探して削除してゆく（複数ありうる）
	for(list_chain<ILayerBase>::iterator it=GetList()->begin();
		it!=GetList()->end();) {
		if (*it==hook) {
			it = GetList()->erase(it);
		} else {
			it ++;
		}
	}
}

void CLayerList::Clear()
{
	GetList()->clear();
}

bool CLayerList::IsEmpty() const {
	return const_cast<CLayerList*>(this)->GetList()->empty();
}

void CLayerList::OnDraw(ISurface* lpDraw){

	list_chain<ILayerBase>::iterator it=GetList()->begin();
	while (it!=GetList()->end()) {
		if ((*it)->IsEnable()){
			(*it)->OnDraw(lpDraw);
		}
		it ++ ;
	}
}

void CLayerList::OnDraw(HDC hDC){

	list_chain<ILayerBase>::iterator it=GetList()->begin();
	while (it!=GetList()->end()) {
		if ((*it)->IsEnable()){
			(*it)->OnDraw(hDC);
		}
		it ++ ;
	}
}

//////////////////////////////////////////////////////////////////////////////

void	CSurfaceLayer::OnDraw(ISurface*pSurface){
	if (IsEnable()){
		int x,y;
		GetPos(x,y);
		pSurface->BltNatural(m_pSurface.get(),x,y);
	}
}

//////////////////////////////////////////////////////////////////////////////
//	CTextLayer

void	CTextLayer::OnDraw(HDC hdc){
	if (IsEnable()){
		int x,y;
		GetPos(x,y);
		GetFont()->OnDraw(hdc,x,y);
	}
}

//////////////////////////////////////////////////////////////////////////////
//	CFPSLayer
}}
#include "../Timer/yaneFPSTimer.h"
namespace yaneuraoGameSDK3rd {
namespace Draw {

CFPSLayer::CFPSLayer(CFPSTimer*pFPSTimer,ILayerList*pLayerList)
	: m_pFPSTimer(pFPSTimer) , CTextLayer(pLayerList)
{
	//	ディフォルトの文字を設定しておく
	GetFont()->SetSize(30);
	GetFont()->SetColor(RGB(150,200,150));
	GetFont()->SetBackColor(RGB(32,64,32));
}

void	CFPSLayer::OnDraw(HDC hdc){
	DWORD dwFps		= GetFPSTimer()->GetFPS();
	DWORD dwRealFps	= GetFPSTimer()->GetRealFPS();

	CHAR buf[128];
	wsprintf(&buf[0],"FPS %d/%d",dwRealFps,dwFps);

	GetFont()->SetText(buf);

	CTextLayer::OnDraw(hdc);
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
