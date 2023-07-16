#include "stdafx.h"

#ifdef USE_FastDraw

#include "yaneTextFastPlane.h"
#include "yaneTextDIBitmap.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {


CTextFastPlane::CTextFastPlane(CFastDraw* pFastDraw/* =NULL */)
	: CFastPlane(pFastDraw)
{
	m_nTextX = 0;
	m_nTextY = 0;
	m_nUpdateType = 0;
	m_bVertical = false;
}

LRESULT	CTextFastPlane::OnDraw(){
	//	ƒŠƒXƒgƒA—pŠÖ”
	switch (m_nUpdateType){
	case 1: return UpdateText();
	case 2: return UpdateTextA();
	case 3: return UpdateTextAA();
	}
	return 0;
}

LRESULT	CTextFastPlane::UpdateText(){
	int sx,sy;
	m_Font.GetSize(sx,sy);
	if (IsVertical()) {	// c‘‚«
		Swap(sx, sy);	// ‹t‚É‚È‚é
	}
	if (CreateSurface(sx,sy,false)) return 1;
	CTextDIBitmap dib;
	*dib.GetFont() = m_Font;
	dib.GetSurfaceInfo()->SetFillColor(GetFillColor());
	dib.SetTextPos(m_nTextX,m_nTextY);
	dib.SetVertical(IsVertical());	// c‘‚«‚©‚Ç‚¤‚©‚ðÝ’è
	dib.UpdateText();

	CSurfaceInfo::CBltInfo info;
	GetSurfaceInfo()->GeneralBlt(CSurfaceInfo::eSurfaceBltFast,
		dib.GetSurfaceInfo(),&info);

	m_nUpdateType = 1;

	return 0;
}

LRESULT	CTextFastPlane::UpdateTextA(){	//	ƒAƒ“ƒ`ƒFƒŠ•t‚«•`‰æ
	int sx,sy;
	m_Font.GetSize(sx,sy);
	if (IsVertical()) {	// c‘‚«
		Swap(sx, sy);	// ‹t‚É‚È‚é
	}
	if (CreateSurface(sx,sy,true)) return 1;
	CTextDIBitmap dib;
	*dib.GetFont() = m_Font;
	dib.SetTextPos(m_nTextX,m_nTextY);
	dib.SetVertical(IsVertical());	// c‘‚«‚©‚Ç‚¤‚©‚ðÝ’è
	dib.UpdateTextA();

	CSurfaceInfo::CBltInfo info;
	GetSurfaceInfo()->GeneralBlt(CSurfaceInfo::eSurfaceBltFast,
		dib.GetSurfaceInfo(),&info);

	m_nUpdateType = 2;

	return 0;
}

//	ªã‚ÌƒRƒsƒy‚µ‚½OOG
LRESULT	CTextFastPlane::UpdateTextAA(){	//	ƒAƒ“ƒ`ƒFƒŠ•t‚«•`‰æ
	int sx,sy;
	m_Font.GetSize(sx,sy);
	if (IsVertical()) {	// c‘‚«
		Swap(sx, sy);	// ‹t‚É‚È‚é
	}
	if (CreateSurface(sx,sy,true)) return 1;

	CTextDIBitmap dib;
	*dib.GetFont() = m_Font;
	dib.SetTextPos(m_nTextX,m_nTextY);
	dib.SetVertical(IsVertical());	// c‘‚«‚©‚Ç‚¤‚©‚ðÝ’è
	dib.UpdateTextAA();

	CSurfaceInfo::CBltInfo info;
	GetSurfaceInfo()->GeneralBlt(CSurfaceInfo::eSurfaceBltFast,
		dib.GetSurfaceInfo(),&info);

	m_nUpdateType = 3;

	return 0;
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
