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
		Err.Out("CDirectDraw::CDirectDraw��DirectDrawInterface�������Ȃ�");
		// DirectX3�͓����Ƃ��̂��H
		m_nStatus = 2;
		return ;
	}

	if (bEmulationOnly) {
		if (GetDirectDraw()->get()->Initialize((GUID*)DDCREATE_EMULATIONONLY)!=DD_OK) {
			Err.Out("CDirectDraw::CDirectDraw��EmulationOnly�ɏo���Ȃ�����");
			//	�ꉞ�AHAL���������g���C����
			if (GetDirectDraw()->get()->Initialize(NULL)!=DD_OK) {
				Err.Out("CDirectDraw::CDirectDraw��HAL�������Ɏ��s");
				m_vDirectDraw.Release();	//	��]�I
				m_nStatus = 4;
				return ;
			} else {
				///	HAL�̏������ɐ��������I
				m_nStatus = 3;
			}
			return ;
		}
	} else {
		if (GetDirectDraw()->get()->Initialize(NULL)!=DD_OK) {
			Err.Out("CDirectDraw::CDirectDraw��HAL�������Ɏ��s");
			m_vDirectDraw.Release();
			m_nStatus = 4;
			return ;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//	DirectDrawSurface�̃^�C�v�𒲂ׂ�

int CDirectDrawSurfaceManager::GetSurfaceType(LPDIRECTDRAWSURFACE pSurface){
	//	����ʂ�bpp���ς�����Ƃ��́A���̊֐��Ăяo���ĂˁI
	if (pSurface==NULL) return -1;

	DDSURFACEDESC dddesc = { sizeof(dddesc) };
	// PixelFormat�𓾂�̂ɁALock����K�v�͂Ȃ����AMillenium G400/450�Ȃǂ�
	// Primary��Lock���邱�Ƃ��ł��Ȃ��B
	dddesc.ddsCaps.dwCaps = DDSD_PIXELFORMAT;
	if(DD_OK!=pSurface->GetSurfaceDesc( &dddesc )){
		Err.Out("CDirectDrawSurfaceManager::GetSurfaceType��GetSurfaceDesc�Ɏ��s");
		return 0;	//	����Ɏ��s
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
			Err.Out("CDirectDrawSurfaceManager::16bpp�̕s���s�N�Z���t�H�[�}�b�g���o");
			Err.Out("BMask=0x%x BMask=0x%x BMask=0x%x",RMask,GMask,BMask);
			nType = 1;	//	16bpp�̕s���s�N�Z���t�H�[�}�b�g�D�D
		}
		break;
	case 24:
		if		(RMask == 0xff0000 && GMask == 0xff00 && BMask == 0xff) {
			nType = 5;	// RGB888
		}	ef	(RMask == 0xff	   && GMask == 0xff00 && BMask == 0xff0000) {
			nType = 6;	// BGR888
		} else {
			Err.Out("CDirectDrawSurfaceManager::24bpp�̕s���s�N�Z���t�H�[�}�b�g���o");
			Err.Out("BMask=0x%x BMask=0x%x BMask=0x%x",RMask,GMask,BMask);
			nType = 1;	//	24bpp�̕s���s�N�Z���t�H�[�}�b�g�D�D
		}
		break;
	case 32:
		if		(RMask == 0xff0000 && GMask == 0xff00 && BMask == 0xff) {
			nType = 7;	// XRGB888
		}	ef	(RMask == 0xff	   && GMask == 0xff00 && BMask == 0xff0000) {
			nType = 8;	// XBGR888
		} else {
			Err.Out("CDirectDrawSurfaceManager::32bpp�̕s���s�N�Z���t�H�[�}�b�g���o");
			Err.Out("BMask=0x%x BMask=0x%x BMask=0x%x",RMask,GMask,BMask);
			nType = 1;	//	32bpp�̕s���s�N�Z���t�H�[�}�b�g�D�D
		}
		break;
	default:
		Err.Out("CDirectDrawSurfaceManager::bpp�s���̃s�N�Z���t�H�[�}�b�g���o");
		Err.Out("nBpp=%d BMask=0x%x BMask=0x%x BMask=0x%x",nBpp,RMask,GMask,BMask);
		nType = 1;	//	�Ȃ�ł��́A����H�O�O�G
		break;
	}
	//	�I���ł���A�I���`��(���ށ�)o
	return nType;
}

/////////////////////////////////////////////////////////////////////////////
//	WindowClipper
LRESULT CDirectDrawClipper::SetClipper(LPDIRECTDRAW lpDraw,LPDIRECTDRAWSURFACE lpPrimary,HWND hWnd){
	//	Window���[�h�łȂ���ΈӖ��͖���

	Release();

	if (lpDraw==NULL || lpPrimary==NULL) return 1;

	if (lpDraw->CreateClipper(0,&m_lpClipper,NULL)!=DD_OK){
		Err.Out("CDirectDrawClipper::Clip��Clipper�\�z���s");
		return 2;
	}
	if (m_lpClipper->SetHWnd(0,hWnd)!=DD_OK){
		Err.Out("CDirectDrawClipper::Clip��hWND���Z�b�g�ł��Ȃ�");
		return 3;
	}
	if (lpPrimary->SetClipper(m_lpClipper)!=DD_OK){
		Err.Out("CDirectDrawClipper::Clip��SetClipper�Ɏ��s");
		return 4;
	}

	return 0;
}

void	CDirectDrawClipper::Release(){
	RELEASE_SAFE(m_lpClipper);
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
