#include "stdafx.h"

#include "CEffect.h"

CEffect::CEffect(void){
}

CEffect::~CEffect(){
}

// DIBをぼかす 
// nLevel = 3 - 30 ?
LRESULT CEffect::ShadeOff(CDIB32 *lpDib,int nEffectLevel,LPRECT lpRect){
//	CTimer time;Err.Out("Effec::ShadeOff St:%d",time.Get());

	int d = nEffectLevel;
	if ( d < 2 ) return -2 ; // 2 < nEffectLebel 

	RECT rdst,rsrc;

	rdst = *lpDib->GetRect();
	rsrc = lpDib->GetClipRect(lpRect);

	int sx = rsrc.right  - rsrc.left;
	int sy = rsrc.bottom - rsrc.top;

	if (sx<=3 || sy<=3) return 2; // too small area..

	int bx = sx / d;int fbx = sx % d;
	int by = sy / d;int fby = sy % d;
	int bbx = bx;int bby = by;
	if ( fbx ) bbx++;if ( fby ) bby++;
//	if ( fbx ) bbx++;if ( fbx ) bby++;

	if ( bbx < 2 || bby < 2 ) return 2; // too small area after MosaicEffect

	MosaicEffect(lpDib,d,lpRect);

	DWORD* lpSrc = lpDib->GetPtr();

	auto_array<DWORD> lpTable(bbx*bby);
	ZeroMemory((LPBYTE)(DWORD*)&lpTable[0],bbx*bby*4);

	DWORD dwColorKey = lpDib->GetColorKey() & 0x00ffffff;
	int i;
	int x,y;
	DWORD pixel;
	for ( y = 0 ; y < bby ; y++){
		for ( x = 0 ; x < bbx; x++){
			pixel = *( lpSrc + rsrc.left + rdst.right*rsrc.top + x*d + rdst.right*d*y ) & 0x00ffffff;
			lpTable[x+(y*bbx)] = pixel;
		}
	}
	for ( y = 1 ; y < bby-1 ; y++ ){
		//	中央
		DWORD	dwDy = y * bbx;
		for ( x = 1 ; x < bbx-1 ; x++ ){
			DWORD pixPoint	= lpTable[x+dwDy];
			DWORD pixRight	= lpTable[(x+1)+dwDy];
			DWORD pixDown	= lpTable[x+dwDy+bbx];
			DWORD pixRDown	= lpTable[(x+1)+dwDy+bbx];

			if ( pixPoint == dwColorKey ) continue;
			// 右とのblend
			DWORD pix_right =	(pixPoint & 0x00fefefe) + 
								(pixRight & 0x00fefefe);
			pix_right >>= 1;
			// 下とのblend
			DWORD pix_down =	(pixPoint & 0x00fefefe) + 
								(pixDown & 0x00fefefe);
			pix_down >>= 1;
			// 右下頂点
			DWORD pix_rdown = (pixPoint & 0xfffcfcfc) + 
								(pixRight & 0xfffcfcfc) + 
								(pixDown & 0xfffcfcfc) +
								(pixRDown & 0xfffcfcfc);
			pix_rdown >>=2;
			DWORD* lpNowBlock = lpSrc+rsrc.left + rdst.right*rsrc.top + x*d + rdst.right * d * y;
			DWORD* lpTarget = lpNowBlock + (d-1) + 1*rdst.right;
			for ( i = 0 ; i < d - 1 ; i++){
				if ( pixRight == dwColorKey ) break;
				*lpTarget = pix_right;
				*(lpTarget+1) = pix_right;
				lpTarget += rdst.right;
			}
			lpTarget = lpNowBlock + 1 + ( (d-1) * rdst.right);
			for ( i = 0 ; i < d - 1 ; i++){
				if ( pixDown == dwColorKey ) break;
				*lpTarget = pix_down;
				*(lpTarget+ rdst.right) = pix_down;
				lpTarget++;
			}
			if ( pixRDown == dwColorKey ) continue;
			lpTarget = lpNowBlock + (d-1) +  (d-1) * rdst.right;
			*lpTarget = pix_rdown;
			*(lpTarget+1) = pix_rdown;
			*(lpTarget+ rdst.right) = pix_rdown;
			*(lpTarget+ rdst.right + 1) = pix_rdown;

		}
	}
//	Err.Out("Effect::ShadeOff Ed:%d",time.Get());
	return 0;
}	

LRESULT CEffect::MosaicEffect(CDIB32*lpDib,int nEffectLevel, LPRECT lpRect){
	//　プレーンに対してモザイクをかける機能 from CDIB32P5
	//	d :	量子化レベル
	int d = nEffectLevel;
	if (d==0) return -2;	//	これで落ちるのはまずかろう...
	
	RECT r = lpDib->GetClipRect(lpRect);
	LONG lPitch	 = lpDib->GetRect()->right;
	DWORD* pSurface = lpDib->GetPtr();

	for(int y=r.top;y<r.bottom;y+=d){
		int d2;		//	下端の端数
		if (y+d>r.bottom) d2=r.bottom-y; else d2=d;
		for(int x=r.left;x<r.right;x+=d){
			int d1;	//	右端の端数
			if (x+d>r.right) d1=r.right-x; else d1=d;
			
			DWORD *p,*p2;
			p = pSurface + y*lPitch + x;
			DWORD c;	// 代表点の色
			c = *p & 0x00ffffff;
			for(int py=0;py<d2;py++){
				p2 = p;
				for(int px=0;px<d1;px++){
					*(p++) = c;
				}
				p = p2 + lPitch;	//	next line
			}
		}
	}
	return 0;
}
