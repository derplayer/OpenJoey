#include "stdafx.h"
#include "yaneSurface.h"
#include "yaneSurfaceBltter.h"
//#include "yaneGTL.h"	//	�����ǂݍ��ނ̂ŁA�����Ďw�肹��ł��������

namespace yaneuraoGameSDK3rd {
namespace Draw {

//	inline�W�J�����̂ŁA�t�@�C���������Ă����Ȃ��ƁA�n���ł������Ȃ�

LRESULT CSurfaceInfo::GeneralBlt(EBltType type,const CSurfaceInfo*pSrc,const CBltInfo* pInfo,DWORD*pAdditionalParameter)
{
	switch (type){
	case eSurfaceBltFast:
		return CSurfaceBltter::Blt1(pSrc,this,pInfo,CFastPlaneCopySrc());
	case eSurfaceBlt:
		CSurfaceBltter_BltHelper2(pSrc,this,pInfo,
			CFastPlaneCopySrcColorKey,pSrc->GetColorKey());
		///	�������̂Ȃ���return���Ă���̂ŁA
		///	break���Ȃ��Ă��A�ł�ł���v��(���ށ�)o
	case eSurfaceBltAddColorFast:	///	���F�����B�����F����
		return CSurfaceBltter::Blt3(pSrc,this,pInfo,CFastPlaneCopyAdd());
	case eSurfaceBltSubColorFast:	///	���F�����B�����F����
		return CSurfaceBltter::Blt3(pSrc,this,pInfo,CFastPlaneCopySub());
	case eSurfaceBltAddColor:		///	���F�����B�����F�L��
		CSurfaceBltter_BltHelper2(pSrc,this,pInfo,
			CFastPlaneCopyAddSrcColorKey,pSrc->GetColorKey());
	case eSurfaceBltSubColor:		///	���F�����B�����F�L��
		CSurfaceBltter_BltHelper2(pSrc,this,pInfo,
			CFastPlaneCopySubSrcColorKey,pSrc->GetColorKey());

	///	�ȉ��̂S�͒ǉ��p�����[�^pAdditionalParameter[0]�Ń����w��(0-255)
	case eSurfaceBltAddColorFastFade:	///	�~�����Ă�����F�����B�����F����
		return CSurfaceBltter::Blt3(pSrc,this,pInfo,CFastPlaneCopyAddMulConst(pAdditionalParameter[0]));
	case eSurfaceBltSubColorFastFade:	///	�~�����Ă��猸�F�����B�����F����
		return CSurfaceBltter::Blt3(pSrc,this,pInfo,CFastPlaneCopySubMulConst(pAdditionalParameter[0]));
	case eSurfaceBltAddColorFade:		///	�~�����Ă�����F�����B�����F�L��
		CSurfaceBltter_BltHelper5_2(pSrc,this,pInfo,
			CFastPlaneCopyAddMulConstSrcColorKey,pSrc->GetColorKey(),pAdditionalParameter[0]);
	case eSurfaceBltSubColorFade:		///	�~�����Ă��猸�F�����B�����F�L��
		CSurfaceBltter_BltHelper5_2(pSrc,this,pInfo,
			CFastPlaneCopySubMulConstSrcColorKey,pSrc->GetColorKey(),pAdditionalParameter[0]);

/*
	///	�~�����Ă���̓]��(�]����f�[�^����)
	case eSurfaceBltFastFade:			///	�~�����Ă���BltFast
	case eSurfaceBltAlphaFade:			///	�~�����Ă���BltAlpha(�]���������T�[�t�F�[�X��)
	case eSurfaceBltFade:				///	�~�����Ă���Blt
*/

	case eSurfaceBltAlphaFast:			///	���T�[�t�F�[�X����̓]��
		return CSurfaceBltter::Blt6(pSrc,this,pInfo,
			CFastPlaneCopyMul(),CFastPlaneCopyMulAlphaToAlpha());

	case eSurfaceBlendBltFast: {
		return CSurfaceBltter::Blt2(pSrc,this,pInfo,CFastPlaneCopyMulAlpha(pAdditionalParameter[0]));
							   }
	case eSurfaceBlendBlt:
		CSurfaceBltter_BltHelper2_2(pSrc,this,pInfo,
			CFastPlaneBlendMulAlphaSrcColorKey,pSrc->GetColorKey(),pAdditionalParameter[0]);

	case eSurfaceBlendConstBltFast:
		return CSurfaceBltter::Blt2(pSrc,this,pInfo,CFastPlaneCopyMulAlphaAB(/*bySrcFadeRate*/pAdditionalParameter[0],/*byDstFadeRate*/pAdditionalParameter[1]));
	case eSurfaceBlendConstBlt:
		CSurfaceBltter_BltHelper2_3(pSrc,this,pInfo,
			CFastPlaneBlendMulAlphaSrcColorKeyAB,pSrc->GetColorKey(),pAdditionalParameter[0],pAdditionalParameter[1]);

	default:
		return 1;	//	�������H
	}

//	return 0;
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
