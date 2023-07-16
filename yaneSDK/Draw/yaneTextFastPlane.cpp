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
	//	���X�g�A�p�֐�
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
	if (IsVertical()) {	// �c����
		Swap(sx, sy);	// �t�ɂȂ�
	}
	if (CreateSurface(sx,sy,false)) return 1;
	CTextDIBitmap dib;
	*dib.GetFont() = m_Font;
	dib.GetSurfaceInfo()->SetFillColor(GetFillColor());
	dib.SetTextPos(m_nTextX,m_nTextY);
	dib.SetVertical(IsVertical());	// �c�������ǂ�����ݒ�
	dib.UpdateText();

	CSurfaceInfo::CBltInfo info;
	GetSurfaceInfo()->GeneralBlt(CSurfaceInfo::eSurfaceBltFast,
		dib.GetSurfaceInfo(),&info);

	m_nUpdateType = 1;

	return 0;
}

LRESULT	CTextFastPlane::UpdateTextA(){	//	�A���`�F���t���`��
	int sx,sy;
	m_Font.GetSize(sx,sy);
	if (IsVertical()) {	// �c����
		Swap(sx, sy);	// �t�ɂȂ�
	}
	if (CreateSurface(sx,sy,true)) return 1;
	CTextDIBitmap dib;
	*dib.GetFont() = m_Font;
	dib.SetTextPos(m_nTextX,m_nTextY);
	dib.SetVertical(IsVertical());	// �c�������ǂ�����ݒ�
	dib.UpdateTextA();

	CSurfaceInfo::CBltInfo info;
	GetSurfaceInfo()->GeneralBlt(CSurfaceInfo::eSurfaceBltFast,
		dib.GetSurfaceInfo(),&info);

	m_nUpdateType = 2;

	return 0;
}

//	����̃R�s�y�����O�O�G
LRESULT	CTextFastPlane::UpdateTextAA(){	//	�A���`�F���t���`��
	int sx,sy;
	m_Font.GetSize(sx,sy);
	if (IsVertical()) {	// �c����
		Swap(sx, sy);	// �t�ɂȂ�
	}
	if (CreateSurface(sx,sy,true)) return 1;

	CTextDIBitmap dib;
	*dib.GetFont() = m_Font;
	dib.SetTextPos(m_nTextX,m_nTextY);
	dib.SetVertical(IsVertical());	// �c�������ǂ�����ݒ�
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
