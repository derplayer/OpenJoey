#include "stdafx.h"
#include "yaneSurface.h"
#include "../Draw/yaneGTL.h"			//	CFastPlaneなんとかに使う
#include "../Draw/yaneGraphicLoader.h"	//	Loadで使う
#include "../Draw/yaneSurfaceBltter.h"	//	Clearで使う
#include "../Draw/yaneDIBitmap.h"		//	LoadでHDC経由で読み込むときに必要
#include "../Auxiliary/yaneLZSS.h"		//	YGAの保存で使う

////////////////////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Draw {

ISurface::ISurface():
	m_vSurfaceInfo(new CSurfaceInfo)
{
}

/////////////////////////////////////////////////////////////////////
//	fill color関連

void		ISurface::SetFillColor(ISurfaceRGB c){
		///	Clearする色を指定する(Default==RGB(0,0,0))
	GetSurfaceInfo()->SetFillColor(c);
}

ISurfaceRGB	ISurface::GetFillColor() const{
				///	Clearする色の取得
	ISurface* pThis = const_cast<ISurface*>(this);
	return pThis->GetSurfaceInfo()->GetFillColor();
}

LRESULT		ISurface::Clear(LPCRECT lpRect/*=NULL*/){
			///	矩形クリア
	return GeneralEffect(CSurfaceInfo::eSurfaceFill,lpRect);
}

/////////////////////////////////////////////////////////////////////
///	透過キー関連事項

LRESULT ISurface::SetColorKey(ISurfaceRGB rgb){
	m_bUsePosColorKey = false;
	m_rgbColorKey = rgb;
	return UpdateColorKey();
}

LRESULT ISurface::SetColorKeyPos(int x,int y){
	m_bUsePosColorKey = true;
	m_nColorKeyX = x; m_nColorKeyY = y;
	//	そのポイントの色を取得する
	return UpdateColorKey();
}

ISurfaceRGB ISurface::GetColorKey() const{
	return m_rgbColorKey;
}

void	ISurface::ResetColorKey(){
	m_bUsePosColorKey	= true;
	m_nColorKeyX = m_nColorKeyY = 0;
	m_rgbColorKey =	0;
}

LRESULT ISurface::UpdateColorKey(){
	LRESULT lr;
	if (m_bUsePosColorKey) {	// 位置指定型透過キー
		lr =GetSurfaceInfo()->GetPixel(m_nColorKeyX,m_nColorKeyY,m_rgbColorKey);
	} else {	// 色指定型透過キー
		//	カラーキーはISurfaceRGBで保持することにしたので
		//	ここで特に変換が必要になるわけではない
		lr = 0;
	}
	//	サーフェース構造体のほうに、この情報を反映させる
	GetSurfaceInfo()->SetColorKey(GetColorKey());
	return lr;
}

/////////////////////////////////////////////////////////////////////
//	委譲系bltter

/*
LRESULT ISurface::BltNaturalPos(const ISurface* lpSrc,int x,int y,int nMode,BYTE nFade,
	LPCSIZE pDstSize,LPCRECT pSrcRect,LPCRECT	pDstClip,int nBasePoint)
{
	//	nMode == ベース位置(0:画像中心 1:左上 2:右上 3:左下 4:右下)
	if (lpSrc==NULL) return -1;

	//	サイズ取得
	int sx,sy;
	if (pDstSize==NULL) {
		//	転送先サイズが指定されていない⇒転送先サイズは転送元サーフェースと同じおおきさ
		lpSrc->GetSize(sx,sy);
	} else {
		//	転送先サイズが指定されていない⇒転送先サイズは、それを利用
		sx = pDstSize->cx;
		sy = pDstSize->cy;
	}

	switch (nMode){	//	フェードレベル
	case 0: return BltNatural(lpSrc,x-(sx>>1),y-(sy>>1)	,nFade,pDstSize,pSrcRect,pDstClip,nBasePoint);
	case 1: return BltNatural(lpSrc,x	,y				,nFade,pDstSize,pSrcRect,pDstClip,nBasePoint);
	case 2: return BltNatural(lpSrc,x-sx,y				,nFade,pDstSize,pSrcRect,pDstClip,nBasePoint);
	case 3: return BltNatural(lpSrc,x	,y-sy			,nFade,pDstSize,pSrcRect,pDstClip,nBasePoint);
	case 4: return BltNatural(lpSrc,x-sx,y-sy			,nFade,pDstSize,pSrcRect,pDstClip,nBasePoint);
	}
	return -2;	//	なんや？nMode範囲外
}
*/

/////////////////////////////////////////////////////////////////////

RECT	CSurfaceInfo::GetClipRect(LPCRECT lpRect) const{
	RECT r;
	if(lpRect == NULL){
		::SetRect(&r,0,0,GetSize().cx,GetSize().cy);
	} else {
		r = *lpRect;
	}

	// クリッピングする
	RECT lpClip = { 0,0,m_size.cx,m_size.cy };

	if (lpClip.left	 > r.left)	{ r.left   = lpClip.left;	 }
	if (lpClip.right < r.right) { r.right  = lpClip.right;	 }
	if (lpClip.top	 > r.top)	{ r.top	   = lpClip.top;	 }
	if (lpClip.bottom<r.bottom) { r.bottom = lpClip.bottom; }

	//	invalid rect,but..
	//	if (r.left >= r.right || r.top	>= r.bottom) return 1;

	return r;
}

LRESULT CSurfaceInfo::Lock() const
#ifdef USE_EXCEPTION
	throw(CRuntimeException)
#endif
{
	if (!IsInit()) return -1;	//	初期化自体がされていない
	if (IsLocked()) {
		#ifdef USE_EXCEPTION
			throw CRuntimeException("２重Lock(CSurfaceInfo::Lock)");
		#else
			return 1;
		#endif
	}
	SetLock(true);
	return GetLocker()->Lock();
}

LRESULT CSurfaceInfo::Unlock() const
#ifdef USE_EXCEPTION
	throw(CRuntimeException)
#endif
{
	if (!IsInit()) return -1;	//	初期化自体がされていない
	if (!IsLocked()) {
		#ifdef USE_EXCEPTION
			throw CRuntimeException("２重UnlockCSurfaceInfo::Unlock");
		#else
			return 1;
		#endif
	}
	SetLock(false);
	return GetLocker()->Unlock();
}

/////////////////////////////////////////////////////////////////////

///	サーフェースの１ピクセルのバイト数
int		CSurfaceInfo::GetPixelSize() const {
	int nSize;
	switch (GetSurfaceType()){
	case 2: nSize = sizeof(CFastPlaneBytePal); break;
	case 3: nSize = sizeof(CFastPlaneRGB565); break;
	case 4: nSize = sizeof(CFastPlaneRGB555); break;
	case 5: nSize = sizeof(CFastPlaneRGB888); break;
	case 6: nSize = sizeof(CFastPlaneBGR888); break;
	case 7: nSize = sizeof(CFastPlaneXRGB8888); break;
	case 8: nSize = sizeof(CFastPlaneXBGR8888); break;
	case 10: nSize = sizeof(CFastPlaneARGB4565); break;
	case 11: nSize = sizeof(CFastPlaneARGB4555); break;
	case 12: nSize = sizeof(CFastPlaneARGB8888); break;
	case 13: nSize = sizeof(CFastPlaneABGR8888); break;
	default: nSize = 0; break;
	}
	return nSize;
}

/////////////////////////////////////////////////////////////////////

void	CSurfaceInfo::iterator::SetRGB(BYTE r,BYTE g,BYTE b,BYTE a){

//	ISurfaceRGBはARGB8888
	int n = GetSurfaceInfo()->GetSurfaceType();
	BYTE *p = (BYTE*)GetSurfaceInfo()->GetPtr() +
				GetSurfaceInfo()->GetPitch() * m_nY;
	switch(n){
	case 3:
		p += sizeof(CFastPlaneRGB565)*m_nX;
		((CFastPlaneRGB565*)p) ->SetRGB(r,g,b);
		break;
	case 4:
		p += sizeof(CFastPlaneRGB555)*m_nX;
		((CFastPlaneRGB555*)p) ->SetRGB(r,g,b);
		break;
	case 5:
		p += sizeof(CFastPlaneRGB888)*m_nX;
		((CFastPlaneRGB888*)p) ->SetRGB(r,g,b);
		break;
	case 6:
		p += sizeof(CFastPlaneBGR888)*m_nX;
		((CFastPlaneBGR888*)p) ->SetRGB(r,g,b);
		break;
	case 7:
		p += sizeof(CFastPlaneXRGB8888)*m_nX;
		((CFastPlaneXRGB8888*)p) ->SetRGB(r,g,b);
		break;
	case 8:
		p += sizeof(CFastPlaneXBGR8888)*m_nX;
		((CFastPlaneXBGR8888*)p) ->SetRGB(r,g,b);
		break;
	case 10:
		p += sizeof(CFastPlaneARGB4565)*m_nX;
		((CFastPlaneARGB4565*)p) ->SetRGB(r,g,b);
		((CFastPlaneARGB4565*)p) ->SetA(a);
		break;
	case 11:
		p += sizeof(CFastPlaneARGB4555)*m_nX;
		((CFastPlaneARGB4555*)p) ->SetRGB(r,g,b);
		((CFastPlaneARGB4555*)p) ->SetA(a);
		break;
	case 12:
		p += sizeof(CFastPlaneARGB8888)*m_nX;
		((CFastPlaneARGB8888*)p) ->SetRGB(r,g,b);
		((CFastPlaneARGB8888*)p) ->SetA(a);
		break;
	case 13:
		p += sizeof(CFastPlaneABGR8888)*m_nX;
		((CFastPlaneABGR8888*)p) ->SetRGB(r,g,b);
		((CFastPlaneABGR8888*)p) ->SetA(a);
		break;
	}
}

ISurfaceRGB	CSurfaceInfo::iterator::GetRGB() const{

//	ISurfaceRGBはARGB8888
	int n = GetSurfaceInfo()->GetSurfaceType();
	int r,g,b,a;
	BYTE *p = (BYTE*)GetSurfaceInfo()->GetPtr() +
				GetSurfaceInfo()->GetPitch() * m_nY;
	switch(n){
	case 3:
		p += sizeof(CFastPlaneRGB565)*m_nX;
		r = ((CFastPlaneRGB565*)p) ->GetR();
		g = ((CFastPlaneRGB565*)p) ->GetG();
		b = ((CFastPlaneRGB565*)p) ->GetB();
		a = 0;
		break;
	case 4:
		p += sizeof(CFastPlaneRGB555)*m_nX;
		r = ((CFastPlaneRGB555*)p) ->GetR();
		g = ((CFastPlaneRGB555*)p) ->GetG();
		b = ((CFastPlaneRGB555*)p) ->GetB();
		a = 0;
		break;
	case 5:
		p += sizeof(CFastPlaneRGB888)*m_nX;
		r = ((CFastPlaneRGB888*)p) ->GetR();
		g = ((CFastPlaneRGB888*)p) ->GetG();
		b = ((CFastPlaneRGB888*)p) ->GetB();
		a = 0;
		break;
	case 6:
		p += sizeof(CFastPlaneBGR888)*m_nX;
		r = ((CFastPlaneBGR888*)p) ->GetR();
		g = ((CFastPlaneBGR888*)p) ->GetG();
		b = ((CFastPlaneBGR888*)p) ->GetB();
		a = 0;
		break;
	case 7:
		p += sizeof(CFastPlaneXRGB8888)*m_nX;
		r = ((CFastPlaneXRGB8888*)p) ->GetR();
		g = ((CFastPlaneXRGB8888*)p) ->GetG();
		b = ((CFastPlaneXRGB8888*)p) ->GetB();
		a = 0;
		break;
	case 8:
		p += sizeof(CFastPlaneXBGR8888)*m_nX;
		r = ((CFastPlaneXBGR8888*)p) ->GetR();
		g = ((CFastPlaneXBGR8888*)p) ->GetG();
		b = ((CFastPlaneXBGR8888*)p) ->GetB();
		a = 0;
		break;
	case 10:
		p += sizeof(CFastPlaneARGB4565)*m_nX;
		r = ((CFastPlaneARGB4565*)p) ->GetR();
		g = ((CFastPlaneARGB4565*)p) ->GetG();
		b = ((CFastPlaneARGB4565*)p) ->GetB();
		a = ((CFastPlaneARGB4565*)p) ->GetA();
		break;
	case 11:
		p += sizeof(CFastPlaneARGB4555)*m_nX;
		r = ((CFastPlaneARGB4555*)p) ->GetR();
		g = ((CFastPlaneARGB4555*)p) ->GetG();
		b = ((CFastPlaneARGB4555*)p) ->GetB();
		a = ((CFastPlaneARGB4555*)p) ->GetA();
		break;
	case 12:
		p += sizeof(CFastPlaneARGB8888)*m_nX;
		r = ((CFastPlaneARGB8888*)p) ->GetR();
		g = ((CFastPlaneARGB8888*)p) ->GetG();
		b = ((CFastPlaneARGB8888*)p) ->GetB();
		a = ((CFastPlaneARGB8888*)p) ->GetA();
		break;
	case 13:
		p += sizeof(CFastPlaneABGR8888)*m_nX;
		r = ((CFastPlaneABGR8888*)p) ->GetR();
		g = ((CFastPlaneABGR8888*)p) ->GetG();
		b = ((CFastPlaneABGR8888*)p) ->GetB();
		a = ((CFastPlaneABGR8888*)p) ->GetA();
		break;
	default:
		r = g = b = a = 0;
	}
	return ISurface::makeRGB(r,g,b,a);
}

LRESULT CSurfaceInfo::GetPixel(int x,int y,ISurfaceRGB&rgb) const{
	CSurfaceInfo* pThis = const_cast<CSurfaceInfo*>(this);
	if (pThis->Lock()!=0) return 0;
	CSurfaceLockerGuard guard(pThis /* ->GetLocker()*/ );
	//	Unlockは抜けるときに勝手に行なってくれる

	//	範囲外？
	if (x<0 || y<0 || x>=GetSize().cx || y>=GetSize().cy) {
		return -1;
	}

	int nType = GetSurfaceType();
	switch (nType) {
	case 2: {
		CFastPlaneBytePal* p = (CFastPlaneBytePal*)((BYTE*)GetPtr() + GetPitch() * y + x * sizeof(CFastPlaneBytePal));
		rgb = ISurface::makeRGB(p->GetR(),p->GetG(),p->GetB()); break; }
	case 3: {
		CFastPlaneRGB565* p = (CFastPlaneRGB565*)((BYTE*)GetPtr() + GetPitch() * y + x * sizeof(CFastPlaneRGB565));
		rgb = ISurface::makeRGB(p->GetR(),p->GetG(),p->GetB()); break; }
	case 4: {
		CFastPlaneRGB555* p = (CFastPlaneRGB555*)((BYTE*)GetPtr() + GetPitch() * y + x * sizeof(CFastPlaneRGB555));
		rgb = ISurface::makeRGB(p->GetR(),p->GetG(),p->GetB()); break; }
	case 5: {
		CFastPlaneRGB888* p = (CFastPlaneRGB888*)((BYTE*)GetPtr() + GetPitch() * y + x * sizeof(CFastPlaneRGB888));
		rgb = ISurface::makeRGB(p->GetR(),p->GetG(),p->GetB()); break; }
	case 6: {
		CFastPlaneBGR888* p = (CFastPlaneBGR888*)((BYTE*)GetPtr() + GetPitch() * y + x * sizeof(CFastPlaneBGR888));
		rgb = ISurface::makeRGB(p->GetR(),p->GetG(),p->GetB()); break; }
	case 7: {
		CFastPlaneXRGB8888* p = (CFastPlaneXRGB8888*)((BYTE*)GetPtr() + GetPitch() * y + x * sizeof(CFastPlaneXRGB8888));
		rgb = ISurface::makeRGB(p->GetR(),p->GetG(),p->GetB()); break; }
	case 8: {
		CFastPlaneXBGR8888* p = (CFastPlaneXBGR8888*)((BYTE*)GetPtr() + GetPitch() * y + x * sizeof(CFastPlaneXBGR8888));
		rgb = ISurface::makeRGB(p->GetR(),p->GetG(),p->GetB()); break; }
	case 10: {
		CFastPlaneARGB4565* p = (CFastPlaneARGB4565*)((BYTE*)GetPtr() + GetPitch() * y + x * sizeof(CFastPlaneARGB4565));
		rgb = ISurface::makeRGB(p->GetR(),p->GetG(),p->GetB(),p->GetA()); break; }
	case 11: {
		CFastPlaneARGB4555* p = (CFastPlaneARGB4555*)((BYTE*)GetPtr() + GetPitch() * y + x * sizeof(CFastPlaneARGB4555));
		rgb = ISurface::makeRGB(p->GetR(),p->GetG(),p->GetB(),p->GetA()); break; }
	case 12: {
		CFastPlaneARGB8888* p = (CFastPlaneARGB8888*)((BYTE*)GetPtr() + GetPitch() * y + x * sizeof(CFastPlaneARGB8888));
		rgb = ISurface::makeRGB(p->GetR(),p->GetG(),p->GetB(),p->GetA()); break; }
	case 13: {
		CFastPlaneABGR8888* p = (CFastPlaneABGR8888*)((BYTE*)GetPtr() + GetPitch() * y + x * sizeof(CFastPlaneARGB8888));
		rgb = ISurface::makeRGB(p->GetR(),p->GetG(),p->GetB(),p->GetA()); break; }
	default:	//	なんや、、不明サーフェースか．．
		rgb = 0; return -2;
	}
	return 0;
}

LRESULT CSurfaceInfo::GetMatchColor(ISurfaceRGB rgb,DWORD&dw) const {

	int nType = GetSurfaceType();
	switch (nType) {
	case 2:
		//	こんなん無理なんやよねー
		break;
	case 3: {
		CFastPlaneRGB565 p;
		p.SetRGB((BYTE)(rgb & 0xff),(BYTE)((rgb>>8) & 0xff),(BYTE)((rgb>>16)& 0xff));
		dw = p.GetRGB(); break; }
	case 4: {
		CFastPlaneRGB555 p;
		p.SetRGB((BYTE)(rgb & 0xff),(BYTE)((rgb>>8) & 0xff),(BYTE)((rgb>>16)& 0xff));
		dw = p.GetRGB(); break; }
	case 5: {
		CFastPlaneRGB888 p;
		p.SetRGB((BYTE)(rgb & 0xff),(BYTE)((rgb>>8) & 0xff),(BYTE)((rgb>>16)& 0xff));
		dw = p.GetRGB(); break; }
	case 6: {
		CFastPlaneBGR888 p;
		p.SetRGB((BYTE)(rgb & 0xff),(BYTE)((rgb>>8) & 0xff),(BYTE)((rgb>>16)& 0xff));
		dw = p.GetRGB(); break; }
	case 7: {
		CFastPlaneXRGB8888 p;
		p.SetRGB((BYTE)(rgb & 0xff),(BYTE)((rgb>>8) & 0xff),(BYTE)((rgb>>16)& 0xff));
		dw = p.GetRGB(); break; }
	case 8: {
		CFastPlaneXBGR8888 p;
		p.SetRGB((BYTE)(rgb & 0xff),(BYTE)((rgb>>8) & 0xff),(BYTE)((rgb>>16)& 0xff));
		dw = p.GetRGB(); break; }
	case 10: {
		CFastPlaneARGB4565 p;
		p.SetRGB((BYTE)(rgb & 0xff),(BYTE)((rgb>>8) & 0xff),(BYTE)((rgb>>16)& 0xff));
		p.SetA((BYTE)((rgb>>24)&0xff));
		dw = p.GetRGBA(); break; }
	case 11: {
		CFastPlaneARGB4555 p;
		p.SetRGB((BYTE)(rgb & 0xff),(BYTE)((rgb>>8) & 0xff),(BYTE)((rgb>>16)& 0xff));
		p.SetA((BYTE)((rgb>>24)&0xff));
		dw = p.GetRGBA(); break; }
	case 12: {
		CFastPlaneARGB8888 p;
		p.SetRGB((BYTE)(rgb & 0xff),(BYTE)((rgb>>8) & 0xff),(BYTE)((rgb>>16)& 0xff));
		p.SetA((BYTE)((rgb>>24)&0xff));
		dw = p.GetRGBA(); break; }
	case 13: {
		CFastPlaneABGR8888 p;
		p.SetRGB((BYTE)(rgb & 0xff),(BYTE)((rgb>>8) & 0xff),(BYTE)((rgb>>16)& 0xff));
		p.SetA((BYTE)((rgb>>24)&0xff));
		dw = p.GetRGBA(); break; }
	default:	//	なんや、、不明サーフェースか．．
		return -1; break;
	}

	return 0;
}

/*
void	ISurface::ClearMSB(){
	//	ビデオカードのバグ対策で最上位バイトを潰す

	int nType = GetSurfaceType();
	if (nType == 7 || nType == 8){
		if (GetSurfaceInfo()->Lock()!=0) return ;
		CFastPlaneEffect::Effect(
			CFastPlaneARGB8888(),GetSurfaceInfo(),
			CFastPlaneClearAlpha(),
			NULL);
		GetSurfaceInfo()->Unlock();
	}
}
*/

int	ISurface::GetYGASurfaceType(int nSurfaceType) const{
	switch(nSurfaceType){
	case 2: nSurfaceType = 11; break;
	case 3: nSurfaceType = 10; break;
	case 4: nSurfaceType = 11; break;
	case 5: nSurfaceType = 12; break;
	case 6: nSurfaceType = 13; break;
	case 7: nSurfaceType = 12; break;
	case 8: nSurfaceType = 13; break;
	//	αサーフェース作成するんなら、それはそれでええ。
	}
	return nSurfaceType;
}

LRESULT	ISurface::LoadByType(const string&strBitmapName,int nSurfaceType){
/**
	このメソッドは、CSurfaceInfoに持たせてそちらに委譲したいのだが、
	画像の読み込みには、画像サイズがわからないといけないので
	IGraphicLoaderを生成して、こいつに画像を読み込ませてからで
	ないといけない。画像を読み込ませ、そのサイズでサーフェースを
	作成したのち、IGraphicLoaderを渡して、それをrenderingするメソッドを
	CSurfaceInfoに実装することは出来るが、そのようなメソッドを
	作る意味はほとんど無い。
*/
	//	１．画像ファイルの読み込み
	smart_ptr<IGraphicLoader> loader(IGraphicLoader::GetGraphicFactory()->CreateInstance());
	CFile file;
	if (file.Read(strBitmapName)!=0) return -1;
	//	読み込み失敗しとるでー

	if (loader->LoadPicture(file)!=0) return -2;

	if (loader->IsYGA()) {
		nSurfaceType = GetYGASurfaceType(nSurfaceType);
		//	αサーフェース作らなきゃ！
	}

	//	２．サーフェースの作成
	LONG sx,sy;
	{
		loader->GetSize(sx,sy);
		if (CreateSurfaceByType(sx,sy,nSurfaceType)!=0) return -3;
	}

	//	３．ファイルの内容を、サーフェースに反映させる
	{
		CSurfaceLockerGuard locker(GetSurfaceInfo());
		GetSurfaceInfo()->Lock();
		if (loader->Render(GetSurfaceInfo())==0) return 0;
	}

	//	このrenderingに失敗したらどないするんやろ．．
	//	HDC経由で描画するか？

	//	CDIBitmap作って、そこ経由で描画するか．．？
	//	GetDC()

	CDIBitmap dib;
	if (dib.CreateSurface(sx,sy)!=0) return -4;
	LRESULT lr;
	{
		HDC hdc = dib.GetDC();
		if (loader->Render(hdc)!=0) {
			// エラーリターンするときはサーフェースのクリアぐらいしたれよ
			Clear();	return -5;
		}

		//	lr = dib.GetSurfaceInfo()->ClearMSB();
		//	DIBは24bpp surfaceなので上位にゴミは乗り上げてこない

		#pragma warning(disable:4238)
		lr = GetSurfaceInfo()->GeneralBlt(
			CSurfaceInfo::eSurfaceBltFast,
			dib.GetSurfaceInfo(),
			&CSurfaceInfo::CBltInfo()
			//	↑テンポラリオブジェクトの引渡しは、
			//	このあとこれを参照しないならばセーフ
		);
		#pragma warning(default:4238)
	}
	return lr;
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CSurfaceInfo::Save(const string& strBitmapFileName,LPCRECT lpRect/*=NULL*/){
	///	画像のファイルへの書き出し(lpRectは書き出し領域。NULLならば全域)
	RECT	rc = GetClipRect(lpRect);

	int sx = rc.right-rc.left;
	int sy = rc.bottom-rc.top;
	if (sx<=0 || sy<=0) return -1; // 変な値渡すなってヽ(`Д´)ノ

	//	ビットマップメモリ確保
	long lDstPitch = ((rc.right - rc.left)*3 + 3) & ~3;

	// 一ライン当たりのバイト数
	long size = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)
		+lDstPitch * (rc.bottom - rc.top);
		//	これが画像ファイルサイズ（のはず）

	smart_ptr<BYTE> bitmap;
	bitmap.AddArray(size);

	//	ビットマップヘッダーの定義
	BITMAPFILEHEADER &BF = *(BITMAPFILEHEADER*)bitmap.get();
	BF.bfType			= 0x4D42;
	BF.bfSize			= size;
	BF.bfReserved1		= 0;
	BF.bfReserved2		= 0;
	BF.bfOffBits		= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	
	BITMAPINFOHEADER &BI = *(BITMAPINFOHEADER*)(bitmap.get() + sizeof(BITMAPFILEHEADER));
	BI.biSize			= sizeof(BITMAPINFOHEADER);
	BI.biWidth			= sx;	// size to be saved...
	BI.biHeight			= sy;
	BI.biPlanes			= 1;
	BI.biBitCount		= 24;		//	フルカラー
	BI.biCompression	= 0;		//	非圧縮
	BI.biSizeImage		= 0;		//	非圧縮のときは0
	BI.biXPelsPerMeter	= 3780;	//	96dpi(こんなところ見ているソフトはないだろうけど)
	BI.biYPelsPerMeter	= 3780;
	BI.biClrUsed		= 0;
	BI.biClrImportant	= 0;

	//	イメージ転送なりり！
	BYTE* pSrcPtr	= (BYTE*)GetPtr();
	LONG lSrcPitch	= GetPitch();
	int nSrcSurfaceType = GetSurfaceType();
	BYTE *pDstPtr = bitmap.get() + sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	for(int y=rc.bottom-1,y0=0;y>=rc.top;y--,y0++){
	//	上下は反転させる必要がある
		void* pSrc = (void*)((BYTE*)(pSrcPtr+	y	* lSrcPitch));
		void* pDst = (void*)((BYTE*)(pDstPtr+	y0	* lDstPitch));
		CSurfaceBltter::Blt1Line(pSrc,nSrcSurfaceType,pDst,5,sx);
			//	src Surface -> RGB888
		//	このあと、DWORDにアラインしたときに出来る右の余白を
		//	０で埋める必要があるような気もするが
		//	そこまで気にする必要は無い．．か？
	}
	return CFile().Write(strBitmapFileName,bitmap.get(),size);	//	そんだけ:p
}

LRESULT CSurfaceInfo::SaveYGA(const string& strBitmapFileName,LPCRECT lpRect/*=NULL*/,bool bCompress/*=true*/){
	///	YGA画像形式でのファイル書き出し(lpRectは書き出し領域。NULLならば全域)
	///	bCompress==trueならばCLZSSで圧縮して保存。ディフォルトでtrue

	///	画像のファイルへの書き出し(lpRectは書き出し領域。NULLならば全域)
	RECT	rc = GetClipRect(lpRect);

	int sx = rc.right-rc.left;
	int sy = rc.bottom-rc.top;
	if (sx<=0 || sy<=0) return -1; // 変な値渡すなって

	int nSrcSurfaceType = GetSurfaceType();
	smart_ptr<BYTE> pSrcCopyed;	//	コピーされたソース領域
	{
		bool	bAllScreen	//	全画面を保存するのか？
			= (GetSize().cx==sx && GetSize().cy==sy);
		if (bAllScreen && GetSurfaceType()==12){
			//	全画面対象かつ、ARGB8888やん!?
			pSrcCopyed.Set((BYTE*)GetPtr());
		} else {
			pSrcCopyed.AddArray(4*sx*sy);
			//	あらたにARGB8888サーフェース作成

			BYTE* pSrcPtr	= (BYTE*)GetPtr() + GetPixelSize()*rc.left;
			LONG  lSrcPitch = GetPixelSize()*sx;
			BYTE* pDstPtr	= pSrcCopyed.get();
			LONG  lDstPitch = 4*sx;
			for(int y=rc.top,y0=0;y<rc.bottom;y++,y0++){
				void* pSrc = (void*)((BYTE*)(pSrcPtr+	y	* lSrcPitch));
				void* pDst = (void*)((BYTE*)(pDstPtr+	y0	* lDstPitch));
				CSurfaceBltter::Blt1Line(pSrc,nSrcSurfaceType,pDst,12,sx);
			}
		}
	}

	//	圧縮を掛けて、ヘッダー情報を付与する
	CLZSS lzss;
	BYTE* lpDst;
	DWORD dwDstSize;
	bool bDelete;
	DWORD nSize = sx * sy * 4;
	if (! bCompress || lzss.Encode(pSrcCopyed.get(),lpDst,nSize,dwDstSize)!=0 ) {
		// 圧縮しない or 圧縮かからんわーい
		lpDst = (BYTE*)GetPtr();
		dwDstSize = nSize;
		bDelete = false;
	} else {
		bDelete = true;
	}

	//	ヘッダーの設定
	CYGAHeader header;
	header.dwSizeX = sx;
	header.dwSizeY = sy;
	header.bCompress = (DWORD) bDelete;
	header.dwOriginalSize  = nSize;
	header.dwCompressSize = dwDstSize;

	nSize = sizeof(CYGAHeader) + dwDstSize;
	smart_ptr<BYTE> writebuf;

	writebuf.AddArray(nSize);
	::CopyMemory(writebuf.get(),&header,sizeof(CYGAHeader));
	::CopyMemory(writebuf.get()+sizeof(CYGAHeader),lpDst,dwDstSize);	
	if (bDelete) DELETEPTR_SAFE(lpDst);

	return CFile().Write(strBitmapFileName,writebuf.get(),nSize);
}

//	↓で使うための構造体
namespace {
	struct GeneralBltTmp {
		CSurfaceInfo::CBltInfo bltinfo;
		//	deep copyするときのための実体
		DWORD dw[2];
		POINT	dstPoint;
		SIZE	dstSize;
		RECT	srcRect;
		RECT	dstClip;
	};
}
smart_ptr<void> CSurfaceInfo::getWrappedPtr(
	CSurfaceInfo::EBltType type,CSurfaceInfo::CBltInfo*&pInfo,DWORD*&pAdditionalParameter)
{
	//	ここで渡されるパラメータは、
	//	callbackするまでvalidとは限らないのでコピーを用意する必要がある
	GeneralBltTmp* tmp = new GeneralBltTmp;
	tmp->bltinfo = *pInfo;
	//	pSrcとlpDstはこれを指すように繋ぎ変える
	pInfo = &tmp->bltinfo;

	//	deep copy
	if (tmp->bltinfo.pDstPoint != NULL){
		tmp->dstPoint = *tmp->bltinfo.pDstPoint;
		tmp->bltinfo.pDstPoint = &tmp->dstPoint;
	}
	if (tmp->bltinfo.pDstSize != NULL){
		tmp->dstSize = *tmp->bltinfo.pDstSize;
		tmp->bltinfo.pDstSize = &tmp->dstSize;
	}
	if (tmp->bltinfo.pSrcRect != NULL){
		tmp->srcRect = *tmp->bltinfo.pSrcRect;
		tmp->bltinfo.pSrcRect = &tmp->srcRect;
	}
	if (tmp->bltinfo.pDstClip != NULL){
		tmp->dstClip = *tmp->bltinfo.pDstClip;
		tmp->bltinfo.pDstClip = &tmp->dstClip;
	}

	switch (type){
		//	1パラメータ
	case eSurfaceBltAddColorFastFade:
	case eSurfaceBltSubColorFastFade:
	case eSurfaceBltAddColorFade:
	case eSurfaceBltSubColorFade:
	case eSurfaceBlendBltFast:
	case eSurfaceBlendBlt:
		tmp->dw[0] = pAdditionalParameter[0];
		pAdditionalParameter = &tmp->dw[0];
		break;

		//	2パラメータ
	case eSurfaceBlendConstBltFast:
	case eSurfaceBlendConstBlt:
		tmp->dw[0] = pAdditionalParameter[0];
		tmp->dw[1] = pAdditionalParameter[1];
		pAdditionalParameter = &tmp->dw[0];
		break;
	}

	return smart_ptr<void>(tmp);
}

namespace {
	struct GeneralMorphTmp {
		CSurfaceInfo::CMorphInfo morphinfo;
		//	deep copyするときのための実体
		DWORD dw[2];
		POINT srcPoint;	///	転送元座標列
		POINT dstPoint;	///	転送先座標列
		RECT clipRect;		///	転送先クリップ矩形
	};
}

smart_ptr<void> CSurfaceInfo::getWrappedPtr(CSurfaceInfo::EBltType type,
											CSurfaceInfo::CMorphInfo*& pInfo,
											DWORD*& pAdditionalParameter)
{
	//	ここで渡されるパラメータは、
	//	callbackするまでvalidとは限らないのでコピーを用意する必要がある
	GeneralMorphTmp* tmp = new GeneralMorphTmp;
	tmp->morphinfo = *pInfo;
	//	pSrcとlpDstはこれを指すように繋ぎ変える
	pInfo = &tmp->morphinfo;

	//	deep copy
	if (tmp->morphinfo.lpSrcPoint != NULL){
		tmp->srcPoint = *tmp->morphinfo.lpSrcPoint;
		tmp->morphinfo.lpSrcPoint = &tmp->srcPoint;
	}
	if (tmp->morphinfo.lpDstPoint != NULL){
		tmp->dstPoint = *tmp->morphinfo.lpDstPoint;
		tmp->morphinfo.lpDstPoint = &tmp->dstPoint;
	}
	if (tmp->morphinfo.lpClipRect != NULL){
		tmp->clipRect = *tmp->morphinfo.lpClipRect;
		tmp->morphinfo.lpClipRect = &tmp->clipRect;
	}

	switch (type){
		//	1パラメータ
	case eSurfaceBltAddColorFastFade:
	case eSurfaceBltSubColorFastFade:
	case eSurfaceBltAddColorFade:
	case eSurfaceBltSubColorFade:
	case eSurfaceBlendBltFast:
	case eSurfaceBlendBlt:
		tmp->dw[0] = pAdditionalParameter[0];
		pAdditionalParameter = &tmp->dw[0];
		break;

		//	2パラメータ
	case eSurfaceBlendConstBltFast:
	case eSurfaceBlendConstBlt:
		tmp->dw[0] = pAdditionalParameter[0];
		tmp->dw[1] = pAdditionalParameter[1];
		pAdditionalParameter = &tmp->dw[0];
		break;
	}

	return smart_ptr<void>(tmp);
}

namespace {
	struct GeneralEffectTmp {
		//	deep copyするときのための実体
		DWORD dw[2];	
		RECT rect;
	};
}

smart_ptr<void> CSurfaceInfo::getWrappedPtr(CSurfaceInfo::EEffectType type,
											LPCRECT& prc,
											DWORD*& pAdditionalParameter)
{
	GeneralEffectTmp* tmp = new GeneralEffectTmp;
	//	rectが指定されていれば、それのコピーを用意してそちらを指すようにする
	if (prc!=NULL) {
		tmp->rect = *prc;
		prc = &tmp->rect;
	}

	switch (type){
		//	1パラメータ
	case eSurfaceFade:
	case eSurfaceAddColor:
	case eSurfaceSubColor:
	case eSurfaceMosaic:
		tmp->dw[0] = pAdditionalParameter[0];
		pAdditionalParameter = &tmp->dw[0];
		break;
	}

	return smart_ptr<void>(tmp);
}


} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
