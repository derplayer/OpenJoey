// DirectDraw Wrapper
//

#include "stdafx.h"
#include "yaneDirectDraw.h"

/////////////////////////////////////////////////////////////////////////////
//	DirectDraw(COM) wrapper

namespace yaneuraoGameSDK3rd {
namespace Draw {

CDirectDraw::CDirectDraw(bool bEmulationOnly){
	m_nStatus = 0;
	if (GetDirectDraw()->CreateInstance(CLSID_DirectDraw,IID_IDirectDraw)!=0){
			m_nStatus = 1;
			return ;
	}
	if(GetDirectDraw()->get()==NULL) {
		Err.Out("CDirectDraw::CDirectDrawでDirectDrawInterfaceが得られない");
		// DirectX3は入っとらんのか？
		m_nStatus = 2;
		return ;
	}

	if (bEmulationOnly) {
		if (GetDirectDraw()->get()->Initialize((GUID*)DDCREATE_EMULATIONONLY)!=DD_OK) {
			Err.Out("CDirectDraw::CDirectDrawでEmulationOnlyに出来なかった");
			//	一応、HAL初期化をトライする
			if (GetDirectDraw()->get()->Initialize(NULL)!=DD_OK) {
				Err.Out("CDirectDraw::CDirectDrawでHAL初期化に失敗");
				m_vDirectDraw.Release();	//	絶望的
				m_nStatus = 4;
				return ;
			} else {
				///	HALの初期化に成功した！
				m_nStatus = 3;
			}
			return ;
		}
	} else {
		if (GetDirectDraw()->get()->Initialize(NULL)!=DD_OK) {
			Err.Out("CDirectDraw::CDirectDrawでHAL初期化に失敗");
			m_vDirectDraw.Release();
			m_nStatus = 4;
			return ;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//	DirectDrawSurfaceのタイプを調べる

int CDirectDrawSurfaceManager::GetSurfaceType(LPDIRECTDRAWSURFACE pSurface){
	//	↑画面のbppが変わったときは、この関数呼び出してね！
	if (pSurface==NULL) return -1;

	DDSURFACEDESC dddesc = { sizeof(dddesc) };
	// PixelFormatを得るのに、Lockする必要はないし、Millenium G400/450などは
	// PrimaryをLockすることができない。
	dddesc.ddsCaps.dwCaps = DDSD_PIXELFORMAT;
	if(DD_OK!=pSurface->GetSurfaceDesc( &dddesc )){
		Err.Out("CDirectDrawSurfaceManager::GetSurfaceTypeでGetSurfaceDescに失敗");
		return 0;	//	特定に失敗
	}

	//-------------------------------
	int nBpp = dddesc.ddpfPixelFormat.dwRGBBitCount;
	DWORD	RMask = dddesc.ddpfPixelFormat.dwRBitMask;
	DWORD	GMask = dddesc.ddpfPixelFormat.dwGBitMask;
	DWORD	BMask = dddesc.ddpfPixelFormat.dwBBitMask;

	int nType;
	switch (nBpp){
	case 8:
		nType = 2;	//	8bpp
		break;
	case 16:
		if		(RMask == 0xf800 && GMask == 0x07e0 && BMask == 0x001f) {
			nType = 3;	// RGB565
		} ef	(RMask == 0x7c00 && GMask == 0x03e0 && BMask == 0x001f) {
			nType = 4;	// RGB555
		} else {
			Err.Out("CDirectDrawSurfaceManager::16bppの不明ピクセルフォーマット検出");
			Err.Out("BMask=0x%x BMask=0x%x BMask=0x%x",RMask,GMask,BMask);
			nType = 1;	//	16bppの不明ピクセルフォーマット．．
		}
		break;
	case 24:
		if		(RMask == 0xff0000 && GMask == 0xff00 && BMask == 0xff) {
			nType = 5;	// RGB888
		}	ef	(RMask == 0xff	   && GMask == 0xff00 && BMask == 0xff0000) {
			nType = 6;	// BGR888
		} else {
			Err.Out("CDirectDrawSurfaceManager::24bppの不明ピクセルフォーマット検出");
			Err.Out("BMask=0x%x BMask=0x%x BMask=0x%x",RMask,GMask,BMask);
			nType = 1;	//	24bppの不明ピクセルフォーマット．．
		}
		break;
	case 32:
		if		(RMask == 0xff0000 && GMask == 0xff00 && BMask == 0xff) {
			nType = 7;	// XRGB888
		}	ef	(RMask == 0xff	   && GMask == 0xff00 && BMask == 0xff0000) {
			nType = 8;	// XBGR888
		} else {
			Err.Out("CDirectDrawSurfaceManager::32bppの不明ピクセルフォーマット検出");
			Err.Out("BMask=0x%x BMask=0x%x BMask=0x%x",RMask,GMask,BMask);
			nType = 1;	//	32bppの不明ピクセルフォーマット．．
		}
		break;
	default:
		Err.Out("CDirectDrawSurfaceManager::bpp不明のピクセルフォーマット検出");
		Err.Out("nBpp=%d BMask=0x%x BMask=0x%x BMask=0x%x",nBpp,RMask,GMask,BMask);
		nType = 1;	//	なんですの、それ？＾＾；
		break;
	}
	//	終了ですわ、終了～○(≧∇≦)o
	return nType;
}

/////////////////////////////////////////////////////////////////////////////
//	WindowClipper
LRESULT CDirectDrawClipper::SetClipper(LPDIRECTDRAW lpDraw,LPDIRECTDRAWSURFACE lpPrimary,HWND hWnd){
	//	Windowモードでなければ意味は無い

	Release();

	if (lpDraw==NULL || lpPrimary==NULL) return 1;

	if (lpDraw->CreateClipper(0,&m_lpClipper,NULL)!=DD_OK){
		Err.Out("CDirectDrawClipper::ClipでClipper構築失敗");
		return 2;
	}
	if (m_lpClipper->SetHWnd(0,hWnd)!=DD_OK){
		Err.Out("CDirectDrawClipper::ClipでhWNDをセットできない");
		return 3;
	}
	if (lpPrimary->SetClipper(m_lpClipper)!=DD_OK){
		Err.Out("CDirectDrawClipper::ClipでSetClipperに失敗");
		return 4;
	}

	return 0;
}

void	CDirectDrawClipper::Release(){
	RELEASE_SAFE(m_lpClipper);
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
