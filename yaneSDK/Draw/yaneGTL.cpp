#include "stdafx.h"
#include "yaneGTL.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

BYTE	CFastPlaneBlendTable::abyMulTable[256*256];
BYTE	CFastPlaneBlendTable::abyMulTable2[256*256];
WORD	CFastPlaneBlendTable::awdMulTableRGB565[65536*16];
WORD	CFastPlaneBlendTable::awdMulTableRGB555[65536*16];
/*
WORD	CFastPlaneBlendTable::awdAddTableRGB565[65536*8];
WORD	CFastPlaneBlendTable::awdAddTableRGB555[65536*8];
WORD	CFastPlaneBlendTable::awdSubTableRGB565[65536*8];
WORD	CFastPlaneBlendTable::awdSubTableRGB555[65536*8];
*/
BYTE	CFastPlaneBlendTable::abyConvertTable555[32768];
	//	これで遅ければ、32bppに対しても、こういうのを用意するが．．．

void	CFastPlaneBlendTable::InitTable()
	{
		{
			for(int i=0;i<256;i++){
				for(int j=0;j<256;j++){
					abyMulTable[i + (j<<8)] = (BYTE)(i*j/255);
					//	このnarrowing変換がオーバーフローしないことは、
					//	255*255/255 == 255であることから明らか。
					abyMulTable2[i + (j<<8)] = (BYTE)(i*(255-j)/255);
				}
			}
		}

		{
			//	for rgb565 & argb3565
			for(int a=0;a<16;a++){
			 for(int r=0;r<32;r++){
			  for(int g=0;g<64;g++){
			   for(int b=0;b<32;b++){
					DWORD dw = b + (g<<5) + (r<<11) + (a<<16);
					WORD data;
					data= b*a/15 + (g*a/15 << 5) + (r*a/15 <<11);
					awdMulTableRGB565[dw] = data;

/*
	//	飽和加算および減算のテーブル化はやんぺー＾＾；

					//	こんなところにif書くなってか＾＾；
					if (a<8) {
						data = ((b+a5>31)?31:b+a5) + (((g+a6>63)?63:g+a6)<<5) + (((r+a5)>31?31:r+a5)<<11);
						awdAddTableRGB565[dw] = data;
						data = ((b<a5)?0:b-a5) + (((g<a6)?0:g-a6)<<5) + (((r<a5)?0:r-a5)<<11);
						awdSubTableRGB565[dw] = data;
					}
*/

			   }
			  }
			 }
			}
		}
		{
			//	for rgb555 & argb3555
			for(int a=0;a<16;a++){
			 for(int r=0;r<32;r++){
			  for(int g=0;g<32;g++){
			   for(int b=0;b<32;b++){
					DWORD dw =	b + (g<<5) + (r<<10) + (a<<16);
					WORD  data;
					data = b*a/15 + (g*a/15 << 5) + (r*a/15 <<10);
					awdMulTableRGB555[dw		  ] = data;
					awdMulTableRGB555[dw + (1<<15)] = data; // 不届きなビデオカード対策＾＾；
					//	↑こいつは、本当は、この半分で良いのだが、
					//	最上位が不定になっている可能性があるので、ビットマスクを
					//	とるのがだるいので、これでいってまう！＾＾；

/*
	//	飽和加算のテーブル化はやんぺー（笑）

					//	こんなところにif書くなってか＾＾；
					if (a<8) {
						data = ((b+a5>31)?31:b+a5) + (((g+a6>63)?63:g+a6)<<5) + (((r+a5)>31?31:r+a5)<<11);
						awdAddTableRGB555[dw] = data;
						awdAddTableRGB555[dw + (1<<15)] = data; // 不届きなビデオカード対策＾＾；
						data = ((b<a5)?0:b-a5) + (((g<a6)?0:g-a6)<<5) + (((r<a5)?0:r-a5)<<11);
						awdSubTableRGB555[dw] = data;
						awdSubTableRGB555[dw + (1<<15)] = data; // 不届きなビデオカード対策＾＾；
					}
*/

			   }
			  }
			 }
			}
		}
	}

void	CFastPlaneBlendTable::OnChangePalette(){
	HDC hdc = ::GetDC(NULL);
	if (hdc==NULL) return ;
	PALETTEENTRY pal[256];
	int nPal = GetSystemPaletteEntries(hdc,0,256,&pal[0]);
	//	パレットは256,16,4,2であることもありうる
	ReleaseDC(NULL,hdc);
	//	非256色モードでは、無視する
	if (nPal == 0) return ;

	for(int r=0;r<32;r++){
	 for(int g=0;g<32;g++){
	  for(int b=0;b<32;b++){
		DWORD dw =	b + (g<<5) + (r<<10);
		BYTE byR=(r << 3),byG=(g << 3),byB=(b << 3);
		BYTE nNearest = 0;
		// 0初期化する必要はないが、0代入しておかないと「値を割り当てていない変数に対する」参照のwarningが出る
		int nMinDistance=INT_MAX;
		for(int i=0;i<nPal;i++){
			BYTE byR2 = pal[i].peRed;
			BYTE byG2 = pal[i].peGreen;
			BYTE byB2 = pal[i].peBlue;

			//	線形距離で考えると？
			int nDistance =
				abs(byR-byR2) + abs(byG-byG2) + abs(byB-byB2);

			if (nDistance < nMinDistance){
				nMinDistance = nDistance;
				nNearest = (BYTE) i;
			}
		}
		abyConvertTable555[dw] = nNearest;
	  }
	 }
	}
}

///////////////////////////////////////////////////////////////////////////////

//	クリッパー
LRESULT CFastPlaneEffect::Clipping(CSurfaceInfo*lpDstInfo,const CSurfaceInfo*lpSrcInfo,const CSurfaceInfo::CBltInfo* pInfo,CFastPlaneEffectClipper* lpClipper){
	int		nSsizeX, nSsizeY, nDsizeX, nDsizeY;
	RECT rcSrcRect,rcDstRect,rcClipRect;
	int		nInitialX;	//	-DX　 :　εの初期値 = -DX
	int		nStepX;		//	 2*SX :　ε+=2*SX
	int		nCmpX;		//	 2*DX :　ε>0ならばε-=2*DXしてね
	int		nStepsX;	//	 SrcXの一回の加算量(整数部)
	int		nInitialY;
	int		nStepY;
	int		nCmpY;
	int		nStepsY;

	if (pInfo->pSrcRect==NULL) {
		::SetRect(&rcSrcRect,0,0,lpSrcInfo->GetSize().cx,lpSrcInfo->GetSize().cy);
	} else {
		//--- 修正 '02/12/04  by ENRA ---
//		rcSrcRect = *pInfo->pSrcRect;
		RECT rcTemp;
		::SetRect(&rcTemp,0,0,lpSrcInfo->GetSize().cx,lpSrcInfo->GetSize().cy);
		if ( ::IntersectRect(&rcSrcRect, pInfo->pSrcRect, &rcTemp)==0 ) {	//	←誰かoptimizeして
			return 1;	//	転送元RECTがとんでもない所でした
		}
		//-------------------------------
	}
	nSsizeX = rcSrcRect.right - rcSrcRect.left;
	nSsizeY = rcSrcRect.bottom - rcSrcRect.top;

	int x,y;
	if (pInfo->pDstPoint==NULL){
		x = y = 0;
	} else {
		x = pInfo->pDstPoint->x;
		y = pInfo->pDstPoint->y;
	}
	if (pInfo->pDstSize==NULL) {
//		rcDstRect = *(lpDstInfo->GetRect());
		//	lpDstSize == NULLのときは、
		//	Srcからの等倍の転送
		::SetRect(&rcDstRect,x,y,x+nSsizeX,y+nSsizeY);
	} else {
		::SetRect(&rcDstRect,x,y,x+pInfo->pDstSize->cx,y+pInfo->pDstSize->cy);
	}
	nDsizeX = rcDstRect.right - rcDstRect.left;
	nDsizeY = rcDstRect.bottom - rcDstRect.top;

	int nBasePoint = pInfo->nBasePoint;
	switch (nBasePoint){
		case 0:                                   break;
		case 1: x -= nDsizeX>>1;                  break;
		case 2: x -= nDsizeX;                     break;
		case 3:                  y -= nDsizeY>>1; break;
		case 4: x -= nDsizeX>>1; y -= nDsizeY>>1; break;
		case 5: x -= nDsizeX;    y -= nDsizeY>>1; break;
		case 6:                  y -= nDsizeY;    break;
		case 7: x -= nDsizeX>>1; y -= nDsizeY;    break;
		case 8: x -= nDsizeX;    y -= nDsizeY;    break;
	}

	//	クリップ領域
	if (pInfo->pDstClip == NULL){
		::SetRect(&rcClipRect,0,0,lpDstInfo->GetSize().cx,lpDstInfo->GetSize().cy);
	} else {
		rcClipRect = *pInfo->pDstClip;
	}
	LPRECT lpClip = &rcClipRect;

	//--- 追加 '02/03/04  by ENRA ---
	{	// クリップRectは、転送先Rectに内包されないといけない
		// しかしClipRectが的はずれな所にあった場合の処理はどうしよう…
		RECT rcDstRect2 = { 0,0,lpDstInfo->GetSize().cx,lpDstInfo->GetSize().cy };
		int t;
		//--- 修正 '02/04/08  by ENRA ---
		// 勝手に０にしたらあかんかった^^;;
		t = lpClip->left  - rcDstRect2.left;
		if (t<0)	{ lpClip->left	= rcDstRect2.left; }
		t = lpClip->top - rcDstRect2.top;
		if (t<0)	{ lpClip->top = rcDstRect2.top; }
		//-------------------------------
		t = lpClip->right  - rcDstRect2.right;
		if (t>0)	{ lpClip->right	 = rcDstRect2.right; }
		t = lpClip->bottom - rcDstRect2.bottom;
		if (t>0)	{ lpClip->bottom = rcDstRect2.bottom; }
	}
	//-------------------------------

	if ( (nSsizeX == nDsizeX) && (nSsizeY == nDsizeY)){
		lpClipper->bActualSize = true;
		// クリッピングする
		// this clipping algorithm is thought by yaneurao(M.Isozaki)
		int sx = rcSrcRect.right - rcSrcRect.left;
		int sy = rcSrcRect.bottom - rcSrcRect.top;
		::SetRect(&rcDstRect,x,y,x+sx,y+sy);

		int t;
		t = lpClip->left-x;
		if (t>0)	{ rcSrcRect.left += t;	rcDstRect.left	= lpClip->left; }
		t = lpClip->top -y;
		if (t>0)	{ rcSrcRect.top	 += t;	rcDstRect.top	= lpClip->top;	 }
		t = x+sx-lpClip->right;
		if (t>0)	{ rcSrcRect.right -= t;	rcDstRect.right	= lpClip->right; }
		t = y+sy-lpClip->bottom;
		if (t>0)	{ rcSrcRect.bottom -= t; rcDstRect.bottom = lpClip->bottom; }

		//	invalid rect ?
		if (rcSrcRect.left >= rcSrcRect.right ||
			rcSrcRect.top  >= rcSrcRect.bottom) return 1;

	}else{ 
		//	非等倍の転送？
		lpClipper->bActualSize = false;
		int dx = rcDstRect.right - rcDstRect.left;
		int dy = rcDstRect.bottom - rcDstRect.top;
		::SetRect(&rcDstRect,x,y,x+dx,y+dy);

		//	ブレゼンハムの初期値を計算する

		//	Initial(x,y) = -Dst size(x,y)
		nInitialX = rcDstRect.left - rcDstRect.right;
		nInitialY = rcDstRect.top  - rcDstRect.bottom;

		nStepX	= (rcSrcRect.right	- rcSrcRect.left) ;
		nStepY	= (rcSrcRect.bottom - rcSrcRect.top ) ;

		nCmpX = - (nInitialX);
		nCmpY = - (nInitialY);

		// invalid rect
		if (nCmpX == 0 || nCmpY == 0) return 1;

		// クリッピングする
		// this clipping algorithm is thought by yaneurao(M.Isozaki)

		//	ミラー時と非ミラー時でブレゼンハムの初期値が違う
		int t;
		t = lpClip->left-rcDstRect.left;
		if (t>0)	{
			nInitialX+=t*nStepX;	//	実際にブレゼンハムしてみる
			if (nInitialX > 0){
				int s = nInitialX / nCmpX + 1;
				rcSrcRect.left += s;	//	not mirror!
				nInitialX		-= s*nCmpX;
			}
			rcDstRect.left	 = lpClip->left;
		}
		t = lpClip->top -rcDstRect.top;
		if (t>0)	{
			nInitialY+=t*nStepY;	//	実際にブレゼンハムしてみる
			if (nInitialY > 0){
				int s = nInitialY / nCmpY +1;
				rcSrcRect.top += s;
				nInitialY		-= s*nCmpY;
			}
			rcDstRect.top = lpClip->top;
		}
		t = rcDstRect.right-lpClip->right;
		if (t>0)	{
			rcDstRect.right	 = lpClip->right;
		}
		//	srcRectの算出。dstのはみ出し分だけ控えるのだが、t*m_nStepX/m_nCmpをroundupする必要あり。

		t = rcDstRect.bottom-lpClip->bottom;
		if (t>0)	{ /*rcSrcRect.bottom -= t;*/
			rcDstRect.bottom = lpClip->bottom;}

		//	invalid rect ?
		if (rcSrcRect.left >= rcSrcRect.right ||
			rcSrcRect.top  >= rcSrcRect.bottom ||
			rcDstRect.left >= rcDstRect.right ||
			rcDstRect.top  >= rcDstRect.bottom) return 1;

		//	m_nStepX < m_nCmpXを保証する。
		nStepsX = nStepX/nCmpX;
		nStepX -= nCmpX*nStepsX;

		nStepsY = nStepY/nCmpY;
		nStepY -= nCmpY*nStepsY;

		// どこで１を引くのか分からないので、ここで引くのだ☆ By Tia
		nInitialX--;
		nInitialY--;

		lpClipper->nCmpX = nCmpX;
		lpClipper->nCmpY = nCmpY;
		lpClipper->nInitialX = nInitialX;
		lpClipper->nInitialY = nInitialY;
		lpClipper->nStepsX = nStepsX;
		lpClipper->nStepsY = nStepsY;
		lpClipper->nStepX = nStepX;
		lpClipper->nStepY = nStepY;
	}
	lpClipper->rcDstRect = rcDstRect;
	lpClipper->rcSrcRect = rcSrcRect;
	lpClipper->rcClipRect = rcClipRect;

	return 0;
}

#ifndef yaneSDK_CantUseInlineAssembler
///	XRGB8888->XRGB8888 α値を指定して転送
LRESULT CFastPlaneEffect::Blt(CFastPlaneXRGB8888, const CSurfaceInfo* lpSrcInfo,
							  CFastPlaneXRGB8888, CSurfaceInfo* lpDstInfo,
							  CFastPlaneCopyMulAlpha f, const CSurfaceInfo::CBltInfo* pInfo,
							  CFastPlaneEffect::BurstTags::MMXBurst)
{
	//	assertion
	WARNING(lpSrcInfo->GetPtr() == NULL,"CFastPlaneEffect::BltでlpSrcInfo->GetPtr() == NULL");
	WARNING(lpDstInfo->GetPtr() == NULL,"CFastPlaneEffect::BltでlpDstInfo->GetPtr() == NULL");

	// クリッピング等の処理
	// 転送範囲なしの時は、このまま帰る
	CFastPlaneEffectClipper clip;
	if ( Clipping( lpDstInfo , lpSrcInfo , pInfo , &clip ) != 0 ) return 1;

	RECT rcSrcRect = clip.rcSrcRect;
	RECT rcDstRect = clip.rcDstRect;

	// 転送先の横幅と縦幅の設定
	int		nDstWidth = rcDstRect.right - rcDstRect.left;
	int		nDstHeight = rcDstRect.bottom - rcDstRect.top;
	int		nSrcWidth = rcSrcRect.right - rcSrcRect.left;
	LONG	lPitchSrc = lpSrcInfo->GetPitch();
	LONG	lPitchDst = lpDstInfo->GetPitch();
	LONG	nAddSrcWidth = lpSrcInfo->GetPitch() - (nSrcWidth * sizeof(DWORD));	// クリッピングして飛ばす分の算出
	LONG	nAddDstWidth = lpDstInfo->GetPitch() - (nDstWidth * sizeof(DWORD));	// ASMで使用する 1ラインのバイト数の設定
	DWORD*	lpSrc = (DWORD*)((BYTE*)lpSrcInfo->GetPtr() +(rcSrcRect.left*sizeof(DWORD))+rcSrcRect.top*lPitchSrc );		// クリッピング部分のカット
	DWORD*	lpDst = (DWORD*)((BYTE*)lpDstInfo->GetPtr() +(rcDstRect.left*sizeof(DWORD))+rcDstRect.top*lPitchDst );		// 指定されたx,yの位置調整

	BYTE	bAlpha = *(BYTE*)(&f);
	DWORD	dwSrcRGBRate	= bAlpha*0x10000+bAlpha*0x100+bAlpha;
	DWORD	dwDstRGBRate	= (0xff-bAlpha)*0x10000+(0xff-bAlpha)*0x100+(0xff-bAlpha);

// -----------	 MMX   カラーキー:無   ミラー:無   拡縮:無	 α値:反映	  --------------
	if ( clip.bActualSize == true )
	{
		_asm
		{
			MOV			EDX, nDstHeight
			MOV			ECX, nDstWidth

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

			PXOR		MM0, MM0
			PXOR		MM1, MM1

			MOVD		MM2, dwSrcRGBRate
			PXOR		MM4, MM4

			MOVD		MM3, dwDstRGBRate
			PUNPCKLBW	MM2, MM4								// WORD単位で乗算するので

			PUNPCKLBW	MM3, MM4			// UnPair

		LoopY_11:
			SHR			ECX, 1
			JNB			LoopX_11								//	偶数か否かの判定
			// Step1
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM1, [EDI]
			PUNPCKLBW	MM0, MM4								// WORD単位で乗算するので

			PUNPCKLBW	MM1, MM4
			PMULLW		MM0, MM2

			PMULLW		MM1, MM3
			NOP

			NOP			// PMULストール回避 (2命令追加)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8				// UnPair

			PACKUSWB	MM0, MM4
			ADD			EDI, 4

			ADD			ESI, 4				// AGI回避

			MOVD		[EDI-4], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_11

		LoopX_11: // 16クロック･サイクル(乗算が 3クロックで 24クロック)
			// Step2
			MOVD		MM0, [ESI]			// 1// UnPair

			MOVD		MM5, [ESI+4]		// 2
			PUNPCKLBW	MM0, MM4			// 1				// WORD単位で乗算するので

			MOVD		MM1, [EDI]			// 1
			PMULLW		MM0, MM2			// 1

			MOVD		MM6, [EDI+4]		// 2
			PUNPCKLBW	MM1, MM4			// 1

			PMULLW		MM1, MM3			// 1// UnPair
			PUNPCKLBW	MM5, MM4			// 2				// WORD単位で乗算するので

			PMULLW		MM5, MM2			// 2
			PUNPCKLBW	MM6, MM4			// 2

			PMULLW		MM6, MM3			// 2
			NOP			// PMULストール回避

			PADDUSB		MM0, MM1			// 1// UnPair

			PSRLW		MM0, 8				// 1// UnPair

			PACKUSWB	MM0, MM4			// 1
			PADDUSB		MM5, MM6			// 2

			PSRLW		MM5, 8				// 2// UnPair

			PACKUSWB	MM5, MM4			// 2
			ADD			ESI, 8

			MOVD		[EDI], MM0			// 1

			MOVD		[EDI+4], MM5		// 2// UnPair

			ADD			EDI, 8
			DEC			ECX

			JNZ			LoopX_11			// UnPair

		EndLoop_11:
			ADD			EDI, nAddDstWidth						// Dstの先頭を次のラインに移動させる
			ADD			ESI, nAddSrcWidth						// クリップした領域分を飛ばす

			MOV			ECX, nDstWidth							//	ECXの更新
			DEC			EDX

			JNZ			LoopY_11			// UnPair

			EMMS
		}
	}
// -----------	 MMX   カラーキー:無   ミラー:無   拡縮:有	 α値:反映	  --------------
	else if ( 1 )
	{
		// 非等倍
		int		nInitialX = clip.nInitialX ;	//	-DX　 :　εの初期値 = -DX
		int		nStepX = clip.nStepX;		//	 2*SX :　ε+=2*SX
		int		nCmpX = clip.nCmpX;		//	 2*DX :　ε>0ならばε-=2*DXしてね
		int		nStepsX = clip.nStepsX;	//	 SrcXの一回の加算量(整数部)
		int		nInitialY = clip.nInitialY;
		int		nStepY = clip.nStepY;
		int		nCmpY = clip.nCmpY;
		int		nStepsY = clip.nStepsY;

		LONG	lPitchSrc = lpSrcInfo->GetPitch();
		DWORD	AddSrcPixel = sizeof(DWORD) * nStepsX;
		DWORD	AddWidthSrc = lPitchSrc * nStepsY;
		int		EIX= nInitialX;
		int		EIY= nInitialY;
		int		EX = nStepX;
		int		EY = nStepY;
		int		EX2= nCmpX;
		int		EY2= nCmpY;
		DWORD*	lpSrcBack;
		DWORD	Mask7f = 0x007f7f7f;

		_asm
		{
			MOVD		MM6, Mask7f
			MOV			EDX, nDstHeight

			MOVQ		MM1, MM6
			MOV			ECX, nDstWidth

			PUNPCKLDQ	MM6, MM1								// 0x007f7f7f
			MOV			EDI, lpDst

			MOV			ESI, lpSrc
			PXOR		MM0, MM0

			PXOR		MM1, MM1
			PXOR		MM4, MM4

			MOVD		MM2, dwSrcRGBRate
			MOV			EBX, EIY								// nEyCnt = EIY;

			MOVD		MM3, dwDstRGBRate
			PUNPCKLBW	MM2, MM4								// WORD単位で乗算するので

			PUNPCKLBW	MM3, MM4			// UnPair

		LoopY_12:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_12: // 14クロック･サイクル(乗算が 3クロックで 18クロック)
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM1, [EDI]
			PUNPCKLBW	MM0, MM4								// WORD単位で乗算するので

			PUNPCKLBW	MM1, MM4
			PMULLW		MM0, MM2

			PMULLW		MM1, MM3
			NOP

			NOP			// PMULストール回避 (2命令追加)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8
			ADD			ESI, AddSrcPixel						// 整数部の加算

			PACKUSWB	MM0, MM4
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_12								// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// Xの補正値

		SkipX_12:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_12

			MOV			ESI, lpSrcBack							// Srcの先頭に戻す
			ADD			EDI, nAddDstWidth						// Dstの先頭を次のラインに移動させる

			MOV			ECX, nDstWidth
			ADD			ESI, AddWidthSrc						// 整数部の加算 '00.09.12.byTia

			ADD			EBX, EY									// Yの増分
			JNB			SkipY_12								// if ( EY >= 0 )

			ADD			ESI, lPitchSrc						// 1ライン分加算して、次の行へ
			SUB			EBX, EY2								// Yの補正値

		SkipY_12:
			DEC			EDX
			JNZ			LoopY_12

			EMMS
		}
	} // if

	return 0;
}

#endif // #ifndef yaneSDK_CantUseInlineAssembler

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
