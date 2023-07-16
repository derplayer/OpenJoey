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
	//	リストア用関数
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
	if (IsVertical()) {	// 縦書き
		Swap(sx, sy);	// 逆になる
	}
	if (CreateSurface(sx,sy,false)) return 1;
	CTextDIBitmap dib;
	*dib.GetFont() = m_Font;
	dib.GetSurfaceInfo()->SetFillColor(GetFillColor());
	dib.SetTextPos(m_nTextX,m_nTextY);
	dib.SetVertical(IsVertical());	// 縦書きかどうかを設定
	dib.UpdateText();

	CSurfaceInfo::CBltInfo info;
	GetSurfaceInfo()->GeneralBlt(CSurfaceInfo::eSurfaceBltFast,
		dib.GetSurfaceInfo(),&info);

	m_nUpdateType = 1;

	return 0;
}

LRESULT	CTextFastPlane::UpdateTextA(){	//	アンチェリ付き描画
	int sx,sy;
	m_Font.GetSize(sx,sy);
	if (IsVertical()) {	// 縦書き
		Swap(sx, sy);	// 逆になる
	}
	if (CreateSurface(sx,sy,true)) return 1;
	CTextDIBitmap dib;
	*dib.GetFont() = m_Font;
	dib.SetTextPos(m_nTextX,m_nTextY);
	dib.SetVertical(IsVertical());	// 縦書きかどうかを設定
	dib.UpdateTextA();

	CSurfaceInfo::CBltInfo info;
	GetSurfaceInfo()->GeneralBlt(CSurfaceInfo::eSurfaceBltFast,
		dib.GetSurfaceInfo(),&info);

	m_nUpdateType = 2;

	return 0;
}

//	↑上のコピペした＾＾；
LRESULT	CTextFastPlane::UpdateTextAA(){	//	アンチェリ付き描画
	int sx,sy;
	m_Font.GetSize(sx,sy);
	if (IsVertical()) {	// 縦書き
		Swap(sx, sy);	// 逆になる
	}
	if (CreateSurface(sx,sy,true)) return 1;

	CTextDIBitmap dib;
	*dib.GetFont() = m_Font;
	dib.SetTextPos(m_nTextX,m_nTextY);
	dib.SetVertical(IsVertical());	// 縦書きかどうかを設定
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
