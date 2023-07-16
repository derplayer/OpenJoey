#include "stdafx.h"
#include "yaneSurface.h"
#include "yaneSurfaceBltter.h"
//#include "yaneGTL.h"	//	↑が読み込むので、あえて指定せんでもええやろ

#include "../math/yaneSinTable.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

//	inline展開されるので、ファイル分割しておかないと、馬鹿でっかくなる

LRESULT CSurfaceInfo::GeneralMorph(EBltType type,const CSurfaceInfo*pSrc,const CMorphInfo* pInfo,DWORD*pAdditionalParameter /*=NULL*/){

	switch (type){
	case eSurfaceBltFast:
		return CSurfaceBltter::Morph2(pSrc,this,pInfo,
			CFastPlaneCopySrc());
	case eSurfaceBlt:
		CSurfaceBltter_MorphHelper2(pSrc,this,pInfo,
			CFastPlaneCopySrcColorKey,pSrc->GetColorKey());
		///	↑こいつのなかでreturnしているので、
		///	breakしなくても、でんでん大丈夫○(≧∇≦)o
	case eSurfaceBltAlphaFast:
		return CSurfaceBltter::Morph2(pSrc,this,pInfo,
			CFastPlaneCopyMul());

	default:
		return 1;	//	未実装？
	}

//	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	回転の実装

LRESULT CSurfaceInfo::InitRotateParam(CMorphInfo& info,const CSurfaceInfo* lpSrc, int x,int y, int nAngle,int nRate,int nType, LPCRECT lpSrcRect, LPCRECT lpClipDstRect,
	RECT& rcSrc,POINT aSrcPoint[4],POINT aDstPoint[4])
{
	if (lpSrc==NULL) return -1;

	int sx,sy;	//	転送元サイズ
	if (lpSrcRect==NULL) {
		SIZE size = lpSrc->GetSize();
		sx = size.cx; sy = size.cy;
		::SetRect(&rcSrc,0,0,sx,sy);
		lpSrcRect = &rcSrc;
	} else {
		sx = lpSrcRect->right  - lpSrcRect->left;
		sy = lpSrcRect->bottom - lpSrcRect->top;
	}

	// for zero devide exception
	if(nRate==0) return 0;
	int dx,dy;	//	転送先サイズ
	dx = ::MulDiv(sx,nRate,1<<16);
	dy = ::MulDiv(sy,nRate,1<<16);
	// dx==0 or dx==0 だと Morphできない
	if(dx==0||dy==0) return 0;

	int px,py;	//	回転中心
	switch (nType){
	case 0: px = x;			py = y;			break;	//	左上
	case 1: px = x+dx;		py = y;			break;	//	右上
	case 2: px = x;			py = y+dy;		break;	//	左下
	case 3: px = x+dx;		py = y+dy;		break;	//	右上
	case 4: px = x+(dx>>1);	py = y+(dy>>1);	break;	//	画像中心
	default:
#ifdef USE_EXCEPTION
		throw CRuntimeException("回転中心の指定が範囲外");
#endif
		break;
	}
//	px+=x; py+=y;

	aSrcPoint[0].x = lpSrcRect->left;
	aSrcPoint[0].y = lpSrcRect->top;
	aSrcPoint[1].x = lpSrcRect->right - 1;
	aSrcPoint[1].y = lpSrcRect->top;
	aSrcPoint[2].x = lpSrcRect->right - 1;
	aSrcPoint[2].y = lpSrcRect->bottom - 1;
	aSrcPoint[3].x = lpSrcRect->left;
	aSrcPoint[3].y = lpSrcRect->bottom - 1;

	//	sintableは倍精度あるので角度は<<1しておく。
	nAngle <<= 1;
	const LONG nSin = gSinTable.Sin(nAngle);
	const LONG nCos = gSinTable.Cos(nAngle);
//	nAngle = -nAngle;	//	y軸は下にとるので回転方向は逆になる
//	for(int i=0;i<4;i++){
		//	(px,py)中心の回転なので、(px,py)を原点に平行移動させて、
		//	原点中心に回転させたあと、(px,py)だけ平行移動
//	aDstPoint[0].x = x;		aDstPoint[0].y = y;
//	aDstPoint[1].x = x+dx;	aDstPoint[1].y = y;
//	aDstPoint[2].x = x+dx;	aDstPoint[2].y = y+dy;
//	aDstPoint[3].x = x;		aDstPoint[3].y = y+dy;
	{
		const int ax0 = x - px;
		const int ay0 = y - py;
		//	0.5での丸めのための修正項 →　0x8000
		aDstPoint[0].x = roundRShift(ax0 * nCos - ay0 * nSin,16)+px;
		aDstPoint[0].y = roundRShift(ax0 * nSin + ay0 * nCos,16)+py;
		const int ax1 = x+dx - px;
		const int ay1 = y - py;
		aDstPoint[1].x = roundRShift(ax1 * nCos - ay1 * nSin,16)+px;
		aDstPoint[1].y = roundRShift(ax1 * nSin + ay1 * nCos,16)+py;
		const int ax2 = x+dx - px;
		const int ay2 = y+dy - py;
		aDstPoint[2].x = roundRShift(ax2 * nCos - ay2 * nSin,16)+px;
		aDstPoint[2].y = roundRShift(ax2 * nSin + ay2 * nCos,16)+py;
		const int ax3 = x - px;
		const int ay3 = y+dy - py;
		aDstPoint[3].x = roundRShift(ax3 * nCos - ay3 * nSin,16)+px;
		aDstPoint[3].y = roundRShift(ax3 * nSin + ay3 * nCos,16)+py;
	}
//	}
	info.Init(aSrcPoint,aDstPoint,lpClipDstRect,false,4);
	return 0;
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
