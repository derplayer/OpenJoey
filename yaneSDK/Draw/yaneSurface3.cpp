#include "stdafx.h"
#include "yaneSurface.h"
#include "yaneSurfaceBltter.h"
//#include "yaneGTL.h"	//	↑が読み込むので、あえて指定せんでもええやろ

namespace yaneuraoGameSDK3rd {
namespace Draw {

//	inline展開されるので、ファイル分割しておかないと、馬鹿でっかくなる

LRESULT CSurfaceInfo::GeneralEffect(EEffectType type,LPCRECT prc,DWORD*pAdditionalParameter/*=NULL*/){

	switch (type){
	case eSurfaceFill:
		CSurfaceBltter_EffectHelper1(this,prc,CFastPlaneFillColor,GetFillColor());
		///	↑こいつのなかでreturnしているので、
		///	breakしなくても、でんでん大丈夫○(≧∇≦)o

	//
	case eSurfaceFade:
		return CSurfaceBltter::Effect2(this,prc,CFastPlaneConstMul(
			/*nFadeRate=*/pAdditionalParameter[0]));

	///	加色合成／減色合成のエフェクトは、追加パラメータの[0]が
	///	ISurfaceRGBの値になっている
	case eSurfaceAddColor:
		CSurfaceBltter_EffectHelper1(this,prc,CFastPlaneConstAdd,pAdditionalParameter[0]);
	case eSurfaceSubColor:
		CSurfaceBltter_EffectHelper1(this,prc,CFastPlaneConstSub,pAdditionalParameter[0]);

	case eSurfaceMosaic:
		return CSurfaceBltter::Mosaic2(this,prc,CFastPlaneCopySrc(),pAdditionalParameter[0]);

	///	flushする
	case eSurfaceFlush:
		return CSurfaceBltter::Effect2(this,prc,CFastPlaneFlush());

	default:
		return 1;	//	未実装？
	}

//	return 0;
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
