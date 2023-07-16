#include "stdafx.h"
#include "yaneSurface.h"
#include "yaneSurfaceBltter.h"
//#include "yaneGTL.h"	//	↑が読み込むので、あえて指定せんでもええやろ

namespace yaneuraoGameSDK3rd {
namespace Draw {

//	inline展開されるので、ファイル分割しておかないと、馬鹿でっかくなる

LRESULT CSurfaceInfo::GeneralBlt(EBltType type,const CSurfaceInfo*pSrc,const CBltInfo* pInfo,DWORD*pAdditionalParameter)
{
	switch (type){
	case eSurfaceBltFast:
		return CSurfaceBltter::Blt1(pSrc,this,pInfo,CFastPlaneCopySrc());
	case eSurfaceBlt:
		CSurfaceBltter_BltHelper2(pSrc,this,pInfo,
			CFastPlaneCopySrcColorKey,pSrc->GetColorKey());
		///	↑こいつのなかでreturnしているので、
		///	breakしなくても、でんでん大丈夫○(≧∇≦)o
	case eSurfaceBltAddColorFast:	///	加色合成。抜き色無し
		return CSurfaceBltter::Blt3(pSrc,this,pInfo,CFastPlaneCopyAdd());
	case eSurfaceBltSubColorFast:	///	減色合成。抜き色無し
		return CSurfaceBltter::Blt3(pSrc,this,pInfo,CFastPlaneCopySub());
	case eSurfaceBltAddColor:		///	加色合成。抜き色有り
		CSurfaceBltter_BltHelper2(pSrc,this,pInfo,
			CFastPlaneCopyAddSrcColorKey,pSrc->GetColorKey());
	case eSurfaceBltSubColor:		///	減色合成。抜き色有り
		CSurfaceBltter_BltHelper2(pSrc,this,pInfo,
			CFastPlaneCopySubSrcColorKey,pSrc->GetColorKey());

	///	以下の４つは追加パラメータpAdditionalParameter[0]でαを指定(0-255)
	case eSurfaceBltAddColorFastFade:	///	×αしてから加色合成。抜き色無し
		return CSurfaceBltter::Blt3(pSrc,this,pInfo,CFastPlaneCopyAddMulConst(pAdditionalParameter[0]));
	case eSurfaceBltSubColorFastFade:	///	×αしてから減色合成。抜き色無し
		return CSurfaceBltter::Blt3(pSrc,this,pInfo,CFastPlaneCopySubMulConst(pAdditionalParameter[0]));
	case eSurfaceBltAddColorFade:		///	×αしてから加色合成。抜き色有り
		CSurfaceBltter_BltHelper5_2(pSrc,this,pInfo,
			CFastPlaneCopyAddMulConstSrcColorKey,pSrc->GetColorKey(),pAdditionalParameter[0]);
	case eSurfaceBltSubColorFade:		///	×αしてから減色合成。抜き色有り
		CSurfaceBltter_BltHelper5_2(pSrc,this,pInfo,
			CFastPlaneCopySubMulConstSrcColorKey,pSrc->GetColorKey(),pAdditionalParameter[0]);

/*
	///	×αしてからの転送(転送先データ無視)
	case eSurfaceBltFastFade:			///	×αしてからBltFast
	case eSurfaceBltAlphaFade:			///	×αしてからBltAlpha(転送元がαサーフェースね)
	case eSurfaceBltFade:				///	×αしてからBlt
*/

	case eSurfaceBltAlphaFast:			///	αサーフェースからの転送
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
		return 1;	//	未実装？
	}

//	return 0;
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
