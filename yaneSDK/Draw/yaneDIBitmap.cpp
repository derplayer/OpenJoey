#include "stdafx.h"
#include "../Draw/yaneDIBitmap.h"
#include "../Draw/yaneGraphicLoader.h"
#include "../Draw/yaneSurface.h"
#include "../Auxiliary/yaneFile.h"

//////////////////////////////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Draw {

CDIBitmap::CDIBitmap():
	m_vSurfaceInfo(new CSurfaceInfo)
{
	m_lpdwSrc = NULL;
	m_hDC	  = NULL;
	m_hBitmap = NULL;
}

CDIBitmap::~CDIBitmap() {
	Release();
}

CSurfaceInfo* CDIBitmap::GetSurfaceInfo(){
	return m_vSurfaceInfo.get();
}

LRESULT	CDIBitmap::Release(){
	if (m_hDC!=NULL){
		::DeleteDC(m_hDC);
		m_hDC = NULL;
	}
	if (m_hBitmap!=NULL){
		::DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
	m_lpdwSrc = NULL;
	m_vSurfaceInfo->SetInit(false);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CDIBitmap::CreateSurface(int nSizeX,int nSizeY,
	int nBpp/*=24*/
//	,bool bClear/*=true*/
	,bool bAlphaSurface/*=false*/
){

	Release();

	if (nSizeX<=0 || nSizeY<=0) return 1;

	if (bAlphaSurface){
		nBpp = 32;
	}

	smart_ptr<BYTE> pBmpInfo;
	if (nBpp==8){
		//	8bppのときは、palette
		pBmpInfo.AddArray(sizeof(BITMAPINFO)+sizeof(RGBQUAD[256]));
	} else {
		pBmpInfo.AddArray(sizeof(BITMAPINFO));
	}

	BITMAPINFO& bmi = *(BITMAPINFO*)(pBmpInfo.get());

	if (nBpp==8){
		//	256色モードならばパレットを初期化
		CGraphicLoader::ClearPalette(&bmi);
	}

	ZERO(bmi);
	bmi.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth		=  nSizeX;
	bmi.bmiHeader.biHeight		= -nSizeY;
	bmi.bmiHeader.biPlanes		= 1;
	bmi.bmiHeader.biBitCount	= nBpp;
	bmi.bmiHeader.biCompression	= BI_RGB;

	/*
		//	test code
		bmi.bmiHeader.biCompression = BI_BITFIELDS;
		*(LPDWORD)(bmi.bmiColors  ) = 0x1f<<11;		//	5
		*(LPDWORD)(bmi.bmiColors+1) = 0x3f<<5;		//	6
		*(LPDWORD)(bmi.bmiColors+2) = (DWORD)0x1f;	//	5
	*/

	HDC hdc = ::GetDC(NULL); // hWndのほうがええんか？
	if (hdc == NULL) return 1;
	m_hBitmap = ::CreateDIBSection(hdc /* NULL*/, &bmi , DIB_RGB_COLORS, (void**)&m_lpdwSrc, NULL, 0 );

	//	ここで取得されたポインタ、DWORDにアラインされているとは限らないから、
	//	yaneGTLで24bppモードでDWORD単位で読み書きするとアクセス違反になる可能性がある
	//	(→仕方ないのでGTL修正するか...?)

	::ReleaseDC(NULL,hdc);
	if (m_hBitmap==NULL) return 1;

	m_nSizeX = nSizeX;
	m_nSizeY = nSizeY;
	m_nBpp	 = nBpp;

	m_hDC = ::CreateCompatibleDC(NULL);
	::SelectObject(m_hDC,m_hBitmap);

//	if (bClear) Clear();

	SIZE size = { nSizeX,nSizeY };
	int nSurfaceType=0;
	int nBaseSize=0;	//	１pixelあたりのバイト数
	switch(nBpp){
	case 8:
		nSurfaceType = 1; nBaseSize=1; break; // PaletteIndex
	case 15:
	case 16:
		nSurfaceType = 4; nBaseSize=2; break; // RGB555
	case 24:
		nSurfaceType = 5; nBaseSize=3; break; // RGB888
	case 32:
		if (!bAlphaSurface){
			nSurfaceType = 7; nBaseSize=4; break; // XRGB8888
		} else {
			nSurfaceType = 12; nBaseSize=4; break; // ARGB8888
		}
	}
	LONG lPitch = nSizeX * nBaseSize;
	///	これDWORDでアラインする必要がある
	lPitch = (lPitch+3) & ~3;

	m_vSurfaceInfo->Init(
		(void*)m_lpdwSrc,	//	void* lpSurfacePtr
		lPitch,		//	LONG lPitch,
		size,		//	const SIZE &size
		nSurfaceType
	);

	return 0;
}

LRESULT	CDIBitmap::GetSize(LONG& x,LONG& y){
	if (m_hDC == NULL) return 1;

	x = m_nSizeX;
	y = m_nSizeY;

	return 0;
}

LRESULT CDIBitmap::Clear(LPRECT pRect){
	return GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, pRect);
}

///////////////////////////////////////////////////////////////////////////////

LRESULT CDIBitmap::Load(const string& BitmapFileName,int nBpp){
	CFile file;
	file.Read(BitmapFileName);

	smart_ptr<IGraphicLoader> gl = IGraphicLoader::GetGraphicFactory()->CreateInstance();
	if (gl->LoadPicture(file)) return 1;

	LONG x,y;
	if (gl->GetSize(x,y)) return 2;
	if (CreateSurface(x,y,nBpp)) return 3;
//	if (nBpp==32 && gl->Render((DWORD*)GetPtr(),m_nSizeX*4)==0) return 0;
	//	32bppのときは、自前によるbmp読み込みをトライする
	if (gl->Render(GetDC())) return 4;

	return 0;
}

LRESULT CDIBitmap::Blt(HDC hDC,LPCRECT lpRect,int nBpp){
	if (lpRect == NULL) return 1;

	int sx = lpRect->right - lpRect->left;
	int sy = lpRect->bottom - lpRect->top;
	if (CreateSurface(sx,sy,nBpp)) return 1;

	HDC hdc = GetDC();
	for(int y=lpRect->top,y2=0;y<lpRect->bottom;y++,y2++){
		for(int x=lpRect->left,x2=0;x<lpRect->right;x++,x2++){
			::SetPixel(hdc,x2,y2,::GetPixel(hDC,x,y));
		}
	}
	return 0;
}

LRESULT CDIBitmap::Save(const string& strBitmapFileName,LPCRECT lpRect){
	//	HDC経由ではなく、SurfaceInfo経由でセーブしてしもたほうがええじゃろ
	return GetSurfaceInfo()->Save(strBitmapFileName,lpRect);
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
