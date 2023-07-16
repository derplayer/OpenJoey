#include "stdafx.h"
#include "yaneSurface.h"
#include "yaneSurfaceBltter.h"
//#include "yaneGTL.h"	//	�����ǂݍ��ނ̂ŁA�����Ďw�肹��ł��������

#include "../math/yaneSinTable.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

//	inline�W�J�����̂ŁA�t�@�C���������Ă����Ȃ��ƁA�n���ł������Ȃ�

LRESULT CSurfaceInfo::GeneralMorph(EBltType type,const CSurfaceInfo*pSrc,const CMorphInfo* pInfo,DWORD*pAdditionalParameter /*=NULL*/){

	switch (type){
	case eSurfaceBltFast:
		return CSurfaceBltter::Morph2(pSrc,this,pInfo,
			CFastPlaneCopySrc());
	case eSurfaceBlt:
		CSurfaceBltter_MorphHelper2(pSrc,this,pInfo,
			CFastPlaneCopySrcColorKey,pSrc->GetColorKey());
		///	�������̂Ȃ���return���Ă���̂ŁA
		///	break���Ȃ��Ă��A�ł�ł���v��(���ށ�)o
	case eSurfaceBltAlphaFast:
		return CSurfaceBltter::Morph2(pSrc,this,pInfo,
			CFastPlaneCopyMul());

	default:
		return 1;	//	�������H
	}

//	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	��]�̎���

LRESULT CSurfaceInfo::InitRotateParam(CMorphInfo& info,const CSurfaceInfo* lpSrc, int x,int y, int nAngle,int nRate,int nType, LPCRECT lpSrcRect, LPCRECT lpClipDstRect,
	RECT& rcSrc,POINT aSrcPoint[4],POINT aDstPoint[4])
{
	if (lpSrc==NULL) return -1;

	int sx,sy;	//	�]�����T�C�Y
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
	int dx,dy;	//	�]����T�C�Y
	dx = ::MulDiv(sx,nRate,1<<16);
	dy = ::MulDiv(sy,nRate,1<<16);
	// dx==0 or dx==0 ���� Morph�ł��Ȃ�
	if(dx==0||dy==0) return 0;

	int px,py;	//	��]���S
	switch (nType){
	case 0: px = x;			py = y;			break;	//	����
	case 1: px = x+dx;		py = y;			break;	//	�E��
	case 2: px = x;			py = y+dy;		break;	//	����
	case 3: px = x+dx;		py = y+dy;		break;	//	�E��
	case 4: px = x+(dx>>1);	py = y+(dy>>1);	break;	//	�摜���S
	default:
#ifdef USE_EXCEPTION
		throw CRuntimeException("��]���S�̎w�肪�͈͊O");
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

	//	sintable�͔{���x����̂Ŋp�x��<<1���Ă����B
	nAngle <<= 1;
	const LONG nSin = gSinTable.Sin(nAngle);
	const LONG nCos = gSinTable.Cos(nAngle);
//	nAngle = -nAngle;	//	y���͉��ɂƂ�̂ŉ�]�����͋t�ɂȂ�
//	for(int i=0;i<4;i++){
		//	(px,py)���S�̉�]�Ȃ̂ŁA(px,py)�����_�ɕ��s�ړ������āA
		//	���_���S�ɉ�]���������ƁA(px,py)�������s�ړ�
//	aDstPoint[0].x = x;		aDstPoint[0].y = y;
//	aDstPoint[1].x = x+dx;	aDstPoint[1].y = y;
//	aDstPoint[2].x = x+dx;	aDstPoint[2].y = y+dy;
//	aDstPoint[3].x = x;		aDstPoint[3].y = y+dy;
	{
		const int ax0 = x - px;
		const int ay0 = y - py;
		//	0.5�ł̊ۂ߂̂��߂̏C���� ���@0x8000
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
