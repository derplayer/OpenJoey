#include "stdafx.h"
#include "yaneSurface.h"
#include "yaneSurfaceBltter.h"
//#include "yaneGTL.h"	//	�����ǂݍ��ނ̂ŁA�����Ďw�肹��ł��������

namespace yaneuraoGameSDK3rd {
namespace Draw {

//	inline�W�J�����̂ŁA�t�@�C���������Ă����Ȃ��ƁA�n���ł������Ȃ�

LRESULT CSurfaceInfo::GeneralEffect(EEffectType type,LPCRECT prc,DWORD*pAdditionalParameter/*=NULL*/){

	switch (type){
	case eSurfaceFill:
		CSurfaceBltter_EffectHelper1(this,prc,CFastPlaneFillColor,GetFillColor());
		///	�������̂Ȃ���return���Ă���̂ŁA
		///	break���Ȃ��Ă��A�ł�ł���v��(���ށ�)o

	//
	case eSurfaceFade:
		return CSurfaceBltter::Effect2(this,prc,CFastPlaneConstMul(
			/*nFadeRate=*/pAdditionalParameter[0]));

	///	���F�����^���F�����̃G�t�F�N�g�́A�ǉ��p�����[�^��[0]��
	///	ISurfaceRGB�̒l�ɂȂ��Ă���
	case eSurfaceAddColor:
		CSurfaceBltter_EffectHelper1(this,prc,CFastPlaneConstAdd,pAdditionalParameter[0]);
	case eSurfaceSubColor:
		CSurfaceBltter_EffectHelper1(this,prc,CFastPlaneConstSub,pAdditionalParameter[0]);

	case eSurfaceMosaic:
		return CSurfaceBltter::Mosaic2(this,prc,CFastPlaneCopySrc(),pAdditionalParameter[0]);

	///	flush����
	case eSurfaceFlush:
		return CSurfaceBltter::Effect2(this,prc,CFastPlaneFlush());

	default:
		return 1;	//	�������H
	}

//	return 0;
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
