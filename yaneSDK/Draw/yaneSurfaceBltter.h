//
//	yaneSurfaceBltter.h
//		サーフェースの転送補助
//

#ifndef __yaneSurfaceBltter_h__
#define __yaneSurfaceBltter_h__


#include "yaneSurface.h"
#include "yaneGTL.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CSurfaceBltter {
/**
	CSurfaceInfoのGeneralBltの転送の実装のためのヘルパ関数

	Blt1 : ほぼすべてのサーフェース間での転送をサポートする
			WORD型のサーフェース(RGB565等)に対してはDWORD扱いして、
			CFastPlaneEffect::BltDoubleを呼び出す。
			※　CFastPlaneCopySrcで使用。

	Blt2 : Blt1の縮退版
			WORD型サーフェースに対する特殊な転送は行なわない
			サポート：
			同じサーフェース間
			非αサーフェース⇒αサーフェース
			αサーフェース⇒非αサーフェース

	Blt3 : Blt2と同じだが、WORD型サーフェースに対して、
				RGB555⇒RGB555555
				RGB565⇒RGB565565
			と、それに対応するDWORDクラスを用いて転送する。

	Blt4 : Blt3と同じだが、異なるサーフェース間の転送はサポートしない
			(未実装)

	Blt5 : Blt2の縮退版
			異なるサーフェース間の転送はサポートしない
			(未実装)

	Blt6 :
			α⇒非α，α⇒α　への転送
			※ BltAlphaFast

	Blt7 :
			通常転送，α⇒非αへの転送
			※　BlendBltFast

	Effect1 : サーフェースに対して、エフェクトをかける
			WORD型のサーフェース(RGB565等)に対しては
			CFastPlaneEffect::BltDoubleを呼び出す。

		例：FnがCFastPlaneCopySrcColorKeyDoubleならば
		Fn ← CFastPlaneCopySrcColorKeyDouble<CFastPlaneRGB565565> rgb(dword);
		とバインドして、Blt2を呼び出すなど。

	Effect2 : サーフェースに対して、エフェクトをかける
			WORD型のサーフェース(RGB565等)に対して
			CFastPlaneEffect::BltDoubleを“呼び出さない”

	Morph2	:	Blt2のMorph版
		※	MorphBltFastで使用

	Mosaic2	:	Effect2のMosaic版

	Blt1Line : 1ライン分をBltする。
			ピクセルフォーマットのコンバージョンに使う

	＜マクロ＞
		※　Fnは、ISurfaceRGB dwで与えられた抜き色情報等が代入される
	CSurfaceBltter_BltHelper1 : 未実装（面倒なので）
	CSurfaceBltter_BltHelper2 : Blt2コンパチ
	CSurfaceBltter_BltHelper2_2 : Blt2コンパチ
			ISurfaceRGB dwと、DWORDパラメータひとつとる
			※	CFastPlaneBlendMulAlphaSrcColorKey(BlendBlt)等で使用。
	CSurfaceBltter_BltHelper2_3 : Blt2コンパチ
			ISurfaceRGB dwと、DWORDパラメータふたつとる
			※	CFastPlaneBlendMulAlphaSrcColorKeyAB(BlendConstBlt)等で使用。
	CSurfaceBltter_MorphHelper2 : Blt2コンパチ
			※	MorphBlt等で使用。
	CSurfaceBltter_BltHelper4 : Blt4コンパチ
			※	CFastPlaneCopyAddColor等で使用。
	CSurfaceBltter_BltHelper5_2 : Blt5コンパチ
			ISurfaceRGB dwと、DWORDパラメータひとつとる
			※	CFastPlaneCopyAddMulConstSrcColorKey等で使用。

	CSurfaceBltter_EffectHelper1 : Blt4とコンパチ
		こちらは、ISurfaceRGB(ARGB8888)をパラメータとしてとる
			※	CFastPlaneFillColorで使用。

		C++のテンプレート機構では、
			F<T> というような関数のF<T>ではなくFだけを引数として渡すことが出来ない。
			そこで、マクロが必要になる。

		BltHelper系は、マクロとして実装されているので呼び出し箇所にて
		インライン展開されるので、その点に注意すること。

*/
public:
	template <class Fn>
	static LRESULT Blt1(const CSurfaceInfo* pSrc,CSurfaceInfo* pDst,const CSurfaceInfo::CBltInfo *pInfo,Fn fn){
	//	このようにコーディングしておけば、この関数を抜けるときに自動的にデストラクタが呼び出され、
	//	正常にUnlockされる：
	if (pDst->Lock()!=0) return 1;
	CSurfaceLockerGuard g1(pDst); 
	if (pSrc->Lock()!=0) return 1;
	CSurfaceLockerGuard g2(pSrc); 

	int nS1 = pDst->GetSurfaceType();
	int nS2 = pSrc->GetSurfaceType();

	//	BltFastだけは、AutoRestoreで使用するため、
	//	ほぼすべての変換をサポート！

	switch (nS2){
	case 3:
		switch (nS1){
		//	RGB565 ->565
		case 3: {
			CFastPlaneEffect::BltDouble(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneRGB565(),pDst,
				CFastPlaneXRGB8888(),	//	DWORD扱いする
				CFastPlaneXRGB8888(),	//	DWORD扱いする
				fn,
				fn,
				pInfo);
			break; }
		//	RGB565->RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB565->RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB565->BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB565->XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB565->XRGB8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB565 -> ARGB4565はサポート
		case 10: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneARGB4565(),pDst,
				fn,
				pInfo);
			break; }
		default:
		//	残念ながら非サポートでした＾＾；
			return 2;
		} break;
	case 4:
		switch (nS1){
		//	RGB555 -> 8bpp はサポート
		case 2: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneBytePal(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB555 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		case 4: {
			CFastPlaneEffect::BltDouble(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneRGB555(),pDst,
				CFastPlaneXRGB8888(),	//	DWORD扱いする
				CFastPlaneXRGB8888(),	//	DWORD扱いする
				fn,
				fn,
				pInfo);
			break; }
		//	RGB555->RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB555->BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break;}
		//	RGB555->XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB555->XRGB8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		case 11: {
		//	RGB555 -> ARGB4555はサポート
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneARGB4555(),pDst,
				fn,
				pInfo);
			break; }
		default:
			//	残念ながら非サポートでした＾＾；
			return 2;
		} break;
	case 5:
		switch (nS1){
		//	RGB888 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB888 -> RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB888->RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB888->BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break;}
		//	RGB888->XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB888->XRGB8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ARGB8888はサポート
		case 12: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,
				pInfo);
			break;}
		default:
			//	残念ながら非サポートでした＾＾；
			return 2;
		} break;
	case 6:
		switch (nS1){
		//	BGR888 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		//	BGR888 -> RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	BGR888->RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	BGR888->BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break;}
		//	BGR888->XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	BGR888->XRGB8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ARGB8888はサポート
		case 12: {
			CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,
				pInfo);
			break;}
		//	BGR -> ABGRサポート
		case 13: {
			CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,
				pInfo);
			break;}
		default:
			//	残念ながら非サポートでした＾＾；
			return 2;
		} break;
	case 7:
		switch (nS1){
		//	XRGB8888 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		//	XRGB8888 -> RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	XRGB8888->RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	XRGB8888->BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break;}
		//	XRGB8888->XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	XRGB8888->XRGB8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ARGB8888はサポート
		case 12: {
			CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,
				pInfo);
			break;}
		default:
			//	残念ながら非サポートでした＾＾；
			return 2;
		} break;
	case 8:
		switch (nS1){
		//	XBGR8888 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		//	XBGR8888 -> RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	XBGR8888->RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	XBGR8888->BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break;}
		//	XBGR8888->XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	XBGR8888->XBGR8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ARGB8888はサポート
		case 12: {
			CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,
				pInfo);
			break;}
		//	BGR->ABGRサポート
		case 13: {
			CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,
				pInfo);
			break;}
		default:
			//	残念ながら非サポートでした＾＾；
			return 2;
		} break;

	//	αサーフェース間の変換
	case 10:
		switch (nS1){
		//	ARGB4565 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> XBGR8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> ARGB4565
		case 10: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneARGB4565(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> ARGB4555
		case 11: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneARGB4555(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> ARGB8888
		case 12: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> ABGR8888
		case 13: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,
				pInfo);
			break; }
		default:
			//	残念ながら非サポートでした＾＾；
			return 2;
		} break;

	case 11:
		switch (nS1){
		//	ARGB4555 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> XBGR8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> ARGB4565
		case 10: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneARGB4565(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> ARGB4555
		case 11: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneARGB4555(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> ARGB8888
		case 12: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> ABGR8888
		case 13: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,
				pInfo);
			break; }
		default:
			//	残念ながら非サポートでした＾＾；
			return 2;
		} break;

	case 12:
		switch (nS1){
		//	ARGB8888 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> XBGR8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> ARGB4565
		case 10: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneARGB4565(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> ARGB4555
		case 11: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneARGB4555(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> ARGB8888
		case 12: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> ABGR8888
		case 13: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,
				pInfo);
			break; }
		default:
			//	残念ながら非サポートでした＾＾；
			return 2;
		} break;

	case 13:
		switch (nS1){
		//	ABGR8888 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> XBGR8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> ARGB4565
		case 10: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneARGB4565(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> ARGB4555
		case 11: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneARGB4555(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> ARGB8888
		case 12: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> ABGR8888
		case 13: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,
				pInfo);
			break; }
		default:
			//	残念ながら非サポートでした＾＾；
			return 2;
		} break;

	//	このソースサーフェースタイプはなんやねん？
	default:
		//	残念ながら非サポートでした＾＾；
		;
	}

	return 0;
	}	//	the end of function Blt1

	/////////////////////////////////////////////////////////////////////////////

	template <class Fn>
	static LRESULT Blt2(const CSurfaceInfo* pSrc,CSurfaceInfo* pDst,const CSurfaceInfo::CBltInfo *pInfo,Fn fn){
	//	このようにコーディングしておけば、この関数を抜けるときに自動的にデストラクタが呼び出され、
	//	正常にUnlockされる：
	if (pDst->Lock()!=0) return 1;
	CSurfaceLockerGuard g1(pDst); 
	if (pSrc->Lock()!=0) return 1;
	CSurfaceLockerGuard g2(pSrc); 

	int nS1 = pDst->GetSurfaceType();
	int nS2 = pSrc->GetSurfaceType();

	//	異なるサーフェース間の転送は基本的にサポートしない！
	if (nS1!=nS2) {

		//	特別サポート！＾＾；
		//	αサーフェースからの転送
		//	αプレーンとの親和性
		if (nS1==3 && nS2==10) {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,pInfo);
		}ef(nS1==4 && nS2==11) {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,pInfo);
		}ef(nS1==5 && nS2==12){
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,pInfo);
		}ef(nS1==6 && nS2==13){
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,pInfo);
		}ef(nS1==7 && nS2==12){
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,pInfo);
		}ef(nS1==8 && nS2==13){
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,pInfo);
		}
		
		//	おまけでサポート＾＾；
		//	αサーフェースへの転送

		//	RGB565 -> ARGB4565はサポート
		ef (nS2==3 && nS1==10){
			CFastPlaneEffect::Blt(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneARGB4565(),pDst,
				fn,pInfo);
		}ef(nS2==4 && nS1==11){
		//	RGB555 -> ARGB4555はサポート
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneARGB4555(),pDst,
				fn,pInfo);
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ARGB8888はサポート
		}ef(nS1==12){
			switch (nS2){
			case 5: {
				CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 6: {
				CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 7: {
				CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 8: {
				CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			default:
				//	残念ながら非サポートでした＾＾；
				return 2;
			}
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ABGR8888はサポート
		}ef(nS1==13){
			switch (nS2){
			case 5: {
				CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 6: {
				CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 7: {
				CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 8: {
				CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			default:
				//	残念ながら非サポートでした＾＾；
				return 2;
			}
		} else {
			//	残念ながら非サポートでした＾＾；
			return 2;
		}
		return 0;
	}

	switch( nS1 ) {
	case 3:	{
		CFastPlaneEffect::Blt(
			CFastPlaneRGB565(),pSrc,
			CFastPlaneRGB565(),pDst,
				fn,pInfo);
				break;
		}
	case 4: {
		CFastPlaneEffect::Blt(
			CFastPlaneRGB555(),pSrc,
			CFastPlaneRGB555(),pDst,
				fn,pInfo);
				break;
		}
	case 5:	{
		CFastPlaneEffect::Blt(
			CFastPlaneRGB888(),pSrc,
			CFastPlaneRGB888(),pDst,
				fn,pInfo);
				break;
		}
	case 6: {
		CFastPlaneEffect::Blt(
			CFastPlaneBGR888(),pSrc,
			CFastPlaneBGR888(),pDst,
				fn,pInfo);
				break;
		}
	case 7: {
		CFastPlaneEffect::Blt(
			CFastPlaneXRGB8888(),pSrc,
			CFastPlaneXRGB8888(),pDst,
				fn,pInfo);
				break;
		}
	case 8: {
		CFastPlaneEffect::Blt(
			CFastPlaneXBGR8888(),pSrc,
			CFastPlaneXBGR8888(),pDst,
				fn,pInfo);
				break;
		}
	case 10: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4565(),pSrc,
			CFastPlaneARGB4565(),pDst,
				fn,pInfo);
				break;
		}
	case 11: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4555(),pSrc,
			CFastPlaneARGB4555(),pDst,
				fn,pInfo);
				break;
		}
	case 12: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB8888(),pSrc,
			CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
		}
	case 13: {
		CFastPlaneEffect::Blt(
			CFastPlaneABGR8888(),pSrc,
			CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
		}
	}

	return 0;
	}	//	the end of function Blt2

// この関数の引数はBlt2とコンパチ
//	ただし、fnは、dwを食わせてから実体化
#define	CSurfaceBltter_BltHelper2(pSrc,pDst,pInfo,fn,dw){		\
	if (pDst->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g1(pDst);						\
	if (pSrc->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g2(pSrc);						\
	int nS1 = pDst->GetSurfaceType();					\
	int nS2 = pSrc->GetSurfaceType();					\
	BYTE a = (BYTE)(dw>>24) , r = (BYTE)((dw>>16)&0xff),	\
		g = (BYTE)((dw>>8)&0xff) , b = (BYTE)(dw & 0xff);	\
	if (nS1!=nS2) {										\
		if (nS1==3 && nS2==10) {						\
			CFastPlaneARGB4565 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB4565(),pSrc,				\
				CFastPlaneRGB565(),pDst,				\
				fn<CFastPlaneARGB4565>(rgb),pInfo);		\
		}ef(nS1==4 && nS2==11) {						\
			CFastPlaneARGB4555 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB4555(),pSrc,				\
				CFastPlaneRGB555(),pDst,				\
				fn<CFastPlaneARGB4555>(rgb),pInfo);		\
		}ef(nS1==5 && nS2==12){							\
			CFastPlaneARGB8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB8888(),pSrc,				\
				CFastPlaneRGB888(),pDst,				\
				fn<CFastPlaneARGB8888>(rgb),pInfo);		\
		}ef(nS1==6 && nS2==13){							\
			CFastPlaneABGR8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneABGR8888(),pSrc,				\
				CFastPlaneBGR888(),pDst,				\
				fn<CFastPlaneABGR8888>(rgb),pInfo);		\
		}ef(nS1==7 && nS2==12){							\
			CFastPlaneARGB8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB8888(),pSrc,				\
				CFastPlaneXRGB8888(),pDst,				\
				fn<CFastPlaneARGB8888>(rgb),pInfo);		\
		}ef(nS1==8 && nS2==13){							\
			CFastPlaneABGR8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneABGR8888(),pSrc,				\
				CFastPlaneXBGR8888(),pDst,				\
				fn<CFastPlaneABGR8888>(rgb),pInfo);		\
		}												\
														\
		ef (nS2==3 && nS1==10){							\
			CFastPlaneRGB565 rgb;						\
			rgb.SetRGB(r,g,b);							\
			CFastPlaneEffect::Blt(						\
				CFastPlaneRGB565(),pSrc,				\
				CFastPlaneARGB4565(),pDst,				\
				fn<CFastPlaneRGB565>(rgb),pInfo);		\
		}ef(nS2==4 && nS1==11){							\
			CFastPlaneRGB555 rgb;						\
			rgb.SetRGB(r,g,b);							\
			CFastPlaneEffect::Blt(						\
				CFastPlaneRGB555(),pSrc,				\
				CFastPlaneARGB4555(),pDst,				\
				fn<CFastPlaneRGB555>(rgb),pInfo);		\
		}ef(nS1==12){									\
			switch (nS2){								\
			case 5: {									\
				CFastPlaneRGB888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneRGB888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneRGB888>(rgb),pInfo);		\
				break;									\
					}									\
			case 6: {									\
				CFastPlaneBGR888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneBGR888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneBGR888>(rgb),pInfo);		\
				break;									\
					}									\
			case 7: {									\
				CFastPlaneXRGB8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXRGB8888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneXRGB8888>(rgb),pInfo);		\
				break;									\
					}									\
			case 8: {									\
				CFastPlaneXBGR8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXBGR8888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneXBGR8888>(rgb),pInfo);		\
				break;									\
					}									\
			default:									\
				return 2;								\
			}											\
		}ef(nS1==13){									\
			switch (nS2){								\
			case 5: {									\
				CFastPlaneRGB888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneRGB888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneRGB888>(rgb),pInfo);		\
				break;									\
					}									\
			case 6: {									\
				CFastPlaneBGR888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneBGR888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneBGR888>(rgb),pInfo);		\
				break;									\
					}									\
			case 7: {									\
				CFastPlaneXRGB8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXRGB8888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneXRGB8888>(rgb),pInfo);		\
				break;									\
					}									\
			case 8: {									\
				CFastPlaneXBGR8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXBGR8888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneXBGR8888>(rgb),pInfo);		\
				break;									\
					}									\
			default:									\
				return 2;								\
			}											\
		} else {										\
			return 2;									\
		}												\
		return 0;										\
	}													\
														\
	switch( nS1 ) {										\
	case 3:	{											\
		CFastPlaneRGB565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB565(),pSrc,					\
			CFastPlaneRGB565(),pDst,					\
			fn<CFastPlaneRGB565>(rgb),pInfo);			\
				break;									\
		}												\
	case 4: {											\
		CFastPlaneRGB555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB555(),pSrc,					\
			CFastPlaneRGB555(),pDst,					\
			fn<CFastPlaneRGB555>(rgb),pInfo);			\
				break;									\
		}												\
	case 5:	{											\
		CFastPlaneRGB888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB888(),pSrc,					\
			CFastPlaneRGB888(),pDst,					\
			fn<CFastPlaneRGB888>(rgb),pInfo);			\
				break;									\
		}												\
	case 6: {											\
		CFastPlaneBGR888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneBGR888(),pSrc,					\
			CFastPlaneBGR888(),pDst,					\
			fn<CFastPlaneBGR888>(rgb),pInfo);			\
				break;									\
		}												\
	case 7: {											\
		CFastPlaneXRGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXRGB8888(),pSrc,					\
			CFastPlaneXRGB8888(),pDst,					\
			fn<CFastPlaneXRGB8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 8: {											\
		CFastPlaneXBGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXBGR8888(),pSrc,					\
			CFastPlaneXBGR8888(),pDst,					\
			fn<CFastPlaneXBGR8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 10: {											\
		CFastPlaneARGB4565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4565(),pSrc,					\
			CFastPlaneARGB4565(),pDst,					\
			fn<CFastPlaneARGB4565>(rgb),pInfo);			\
				break;									\
		}												\
	case 11: {											\
		CFastPlaneARGB4555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4555(),pSrc,					\
			CFastPlaneARGB4555(),pDst,					\
			fn<CFastPlaneARGB4555>(rgb),pInfo);			\
				break;									\
		}												\
	case 12: {											\
		CFastPlaneARGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB8888(),pSrc,					\
			CFastPlaneARGB8888(),pDst,					\
			fn<CFastPlaneARGB8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 13: {											\
		CFastPlaneABGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneABGR8888(),pSrc,					\
			CFastPlaneABGR8888(),pDst,					\
			fn<CFastPlaneABGR8888>(rgb),pInfo);			\
				break;									\
		}												\
	}													\
														\
	return 0;											\
														\
	}	//	the end of function BltHelper2

// この関数の引数はBlt2とコンパチ
//	ただし、fnは、dwを食わせてから実体化
#define	CSurfaceBltter_BltHelper2_2(pSrc,pDst,pInfo,fn,dw,p1){		\
	if (pDst->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g1(pDst);						\
	if (pSrc->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g2(pSrc);						\
	int nS1 = pDst->GetSurfaceType();					\
	int nS2 = pSrc->GetSurfaceType();					\
	BYTE a = (BYTE)(dw>>24) , r = (BYTE)((dw>>16)&0xff),	\
		g = (BYTE)((dw>>8)&0xff) , b = (BYTE)(dw & 0xff);	\
	if (nS1!=nS2) {										\
		if (nS1==3 && nS2==10) {						\
			CFastPlaneARGB4565 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB4565(),pSrc,				\
				CFastPlaneRGB565(),pDst,				\
				fn<CFastPlaneARGB4565>(rgb,p1),pInfo);	\
		}ef(nS1==4 && nS2==11) {						\
			CFastPlaneARGB4555 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB4555(),pSrc,				\
				CFastPlaneRGB555(),pDst,				\
				fn<CFastPlaneARGB4555>(rgb,p1),pInfo);	\
		}ef(nS1==5 && nS2==12){							\
			CFastPlaneARGB8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB8888(),pSrc,				\
				CFastPlaneRGB888(),pDst,				\
				fn<CFastPlaneARGB8888>(rgb,p1),pInfo);	\
		}ef(nS1==6 && nS2==13){							\
			CFastPlaneABGR8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneABGR8888(),pSrc,				\
				CFastPlaneBGR888(),pDst,				\
				fn<CFastPlaneABGR8888>(rgb,p1),pInfo);	\
		}ef(nS1==7 && nS2==12){							\
			CFastPlaneARGB8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB8888(),pSrc,				\
				CFastPlaneXRGB8888(),pDst,				\
				fn<CFastPlaneARGB8888>(rgb,p1),pInfo);	\
		}ef(nS1==8 && nS2==13){							\
			CFastPlaneABGR8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneABGR8888(),pSrc,				\
				CFastPlaneXBGR8888(),pDst,				\
				fn<CFastPlaneABGR8888>(rgb,p1),pInfo);	\
		}												\
														\
		ef (nS2==3 && nS1==10){							\
			CFastPlaneRGB565 rgb;						\
			rgb.SetRGB(r,g,b);							\
			CFastPlaneEffect::Blt(						\
				CFastPlaneRGB565(),pSrc,				\
				CFastPlaneARGB4565(),pDst,				\
				fn<CFastPlaneRGB565>(rgb,p1),pInfo);	\
		}ef(nS2==4 && nS1==11){							\
			CFastPlaneRGB555 rgb;						\
			rgb.SetRGB(r,g,b);							\
			CFastPlaneEffect::Blt(						\
				CFastPlaneRGB555(),pSrc,				\
				CFastPlaneARGB4555(),pDst,				\
				fn<CFastPlaneRGB555>(rgb,p1),pInfo);	\
		}ef(nS1==12){									\
			switch (nS2){								\
			case 5: {									\
				CFastPlaneRGB888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneRGB888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneRGB888>(rgb,p1),pInfo);	\
				break;									\
					}									\
			case 6: {									\
				CFastPlaneBGR888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneBGR888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneBGR888>(rgb,p1),pInfo);	\
				break;									\
					}									\
			case 7: {									\
				CFastPlaneXRGB8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXRGB8888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneXRGB8888>(rgb,p1),pInfo);	\
				break;									\
					}									\
			case 8: {									\
				CFastPlaneXBGR8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXBGR8888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneXBGR8888>(rgb,p1),pInfo);	\
				break;									\
					}									\
			default:									\
				return 2;								\
			}											\
		}ef(nS1==13){									\
			switch (nS2){								\
			case 5: {									\
				CFastPlaneRGB888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneRGB888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneRGB888>(rgb,p1),pInfo);	\
				break;									\
					}									\
			case 6: {									\
				CFastPlaneBGR888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneBGR888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneBGR888>(rgb,p1),pInfo);	\
				break;									\
					}									\
			case 7: {									\
				CFastPlaneXRGB8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXRGB8888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneXRGB8888>(rgb,p1),pInfo);	\
				break;									\
					}									\
			case 8: {									\
				CFastPlaneXBGR8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXBGR8888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneXBGR8888>(rgb,p1),pInfo);	\
				break;									\
					}									\
			default:									\
				return 2;								\
			}											\
		} else {										\
			return 2;									\
		}												\
		return 0;										\
	}													\
														\
	switch( nS1 ) {										\
	case 3:	{											\
		CFastPlaneRGB565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB565(),pSrc,					\
			CFastPlaneRGB565(),pDst,					\
			fn<CFastPlaneRGB565>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 4: {											\
		CFastPlaneRGB555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB555(),pSrc,					\
			CFastPlaneRGB555(),pDst,					\
			fn<CFastPlaneRGB555>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 5:	{											\
		CFastPlaneRGB888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB888(),pSrc,					\
			CFastPlaneRGB888(),pDst,					\
			fn<CFastPlaneRGB888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 6: {											\
		CFastPlaneBGR888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneBGR888(),pSrc,					\
			CFastPlaneBGR888(),pDst,					\
			fn<CFastPlaneBGR888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 7: {											\
		CFastPlaneXRGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXRGB8888(),pSrc,					\
			CFastPlaneXRGB8888(),pDst,					\
			fn<CFastPlaneXRGB8888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 8: {											\
		CFastPlaneXBGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXBGR8888(),pSrc,					\
			CFastPlaneXBGR8888(),pDst,					\
			fn<CFastPlaneXBGR8888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 10: {											\
		CFastPlaneARGB4565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4565(),pSrc,					\
			CFastPlaneARGB4565(),pDst,					\
			fn<CFastPlaneARGB4565>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 11: {											\
		CFastPlaneARGB4555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4555(),pSrc,					\
			CFastPlaneARGB4555(),pDst,					\
			fn<CFastPlaneARGB4555>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 12: {											\
		CFastPlaneARGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB8888(),pSrc,					\
			CFastPlaneARGB8888(),pDst,					\
			fn<CFastPlaneARGB8888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 13: {											\
		CFastPlaneABGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneABGR8888(),pSrc,					\
			CFastPlaneABGR8888(),pDst,					\
			fn<CFastPlaneABGR8888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	}													\
														\
	return 0;											\
														\
	}	//	the end of function BltHelper2_2

// この関数の引数はBlt2とコンパチ
//	ただし、fnは、dwを食わせてから実体化
#define	CSurfaceBltter_BltHelper2_3(pSrc,pDst,pInfo,fn,dw,p1,p2){		\
	if (pDst->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g1(pDst);						\
	if (pSrc->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g2(pSrc);						\
	int nS1 = pDst->GetSurfaceType();					\
	int nS2 = pSrc->GetSurfaceType();					\
	BYTE a = (BYTE)(dw>>24) , r = (BYTE)((dw>>16)&0xff),	\
		g = (BYTE)((dw>>8)&0xff) , b = (BYTE)(dw & 0xff);	\
	if (nS1!=nS2) {										\
		if (nS1==3 && nS2==10) {						\
			CFastPlaneARGB4565 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB4565(),pSrc,				\
				CFastPlaneRGB565(),pDst,				\
				fn<CFastPlaneARGB4565>(rgb,p1,p2),pInfo);	\
		}ef(nS1==4 && nS2==11) {						\
			CFastPlaneARGB4555 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB4555(),pSrc,				\
				CFastPlaneRGB555(),pDst,				\
				fn<CFastPlaneARGB4555>(rgb,p1,p2),pInfo);	\
		}ef(nS1==5 && nS2==12){							\
			CFastPlaneARGB8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB8888(),pSrc,				\
				CFastPlaneRGB888(),pDst,				\
				fn<CFastPlaneARGB8888>(rgb,p1,p2),pInfo);	\
		}ef(nS1==6 && nS2==13){							\
			CFastPlaneABGR8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneABGR8888(),pSrc,				\
				CFastPlaneBGR888(),pDst,				\
				fn<CFastPlaneABGR8888>(rgb,p1,p2),pInfo);	\
		}ef(nS1==7 && nS2==12){							\
			CFastPlaneARGB8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB8888(),pSrc,				\
				CFastPlaneXRGB8888(),pDst,				\
				fn<CFastPlaneARGB8888>(rgb,p1,p2),pInfo);	\
		}ef(nS1==8 && nS2==13){							\
			CFastPlaneABGR8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneABGR8888(),pSrc,				\
				CFastPlaneXBGR8888(),pDst,				\
				fn<CFastPlaneABGR8888>(rgb,p1,p2),pInfo);	\
		}												\
														\
		ef (nS2==3 && nS1==10){							\
			CFastPlaneRGB565 rgb;						\
			rgb.SetRGB(r,g,b);							\
			CFastPlaneEffect::Blt(						\
				CFastPlaneRGB565(),pSrc,				\
				CFastPlaneARGB4565(),pDst,				\
				fn<CFastPlaneRGB565>(rgb,p1,p2),pInfo);	\
		}ef(nS2==4 && nS1==11){							\
			CFastPlaneRGB555 rgb;						\
			rgb.SetRGB(r,g,b);							\
			CFastPlaneEffect::Blt(						\
				CFastPlaneRGB555(),pSrc,				\
				CFastPlaneARGB4555(),pDst,				\
				fn<CFastPlaneRGB555>(rgb,p1,p2),pInfo);	\
		}ef(nS1==12){									\
			switch (nS2){								\
			case 5: {									\
				CFastPlaneRGB888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneRGB888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneRGB888>(rgb,p1,p2),pInfo);	\
				break;									\
					}									\
			case 6: {									\
				CFastPlaneBGR888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneBGR888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneBGR888>(rgb,p1,p2),pInfo);	\
				break;									\
					}									\
			case 7: {									\
				CFastPlaneXRGB8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXRGB8888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneXRGB8888>(rgb,p1,p2),pInfo);	\
				break;									\
					}									\
			case 8: {									\
				CFastPlaneXBGR8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXBGR8888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneXBGR8888>(rgb,p1,p2),pInfo);	\
				break;									\
					}									\
			default:									\
				return 2;								\
			}											\
		}ef(nS1==13){									\
			switch (nS2){								\
			case 5: {									\
				CFastPlaneRGB888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneRGB888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneRGB888>(rgb,p1,p2),pInfo);	\
				break;									\
					}									\
			case 6: {									\
				CFastPlaneBGR888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneBGR888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneBGR888>(rgb,p1,p2),pInfo);	\
				break;									\
					}									\
			case 7: {									\
				CFastPlaneXRGB8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXRGB8888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneXRGB8888>(rgb,p1,p2),pInfo);	\
				break;									\
					}									\
			case 8: {									\
				CFastPlaneXBGR8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXBGR8888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneXBGR8888>(rgb,p1,p2),pInfo);	\
				break;									\
					}									\
			default:									\
				return 2;								\
			}											\
		} else {										\
			return 2;									\
		}												\
		return 0;										\
	}													\
														\
	switch( nS1 ) {										\
	case 3:	{											\
		CFastPlaneRGB565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB565(),pSrc,					\
			CFastPlaneRGB565(),pDst,					\
			fn<CFastPlaneRGB565>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 4: {											\
		CFastPlaneRGB555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB555(),pSrc,					\
			CFastPlaneRGB555(),pDst,					\
			fn<CFastPlaneRGB555>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 5:	{											\
		CFastPlaneRGB888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB888(),pSrc,					\
			CFastPlaneRGB888(),pDst,					\
			fn<CFastPlaneRGB888>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 6: {											\
		CFastPlaneBGR888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneBGR888(),pSrc,					\
			CFastPlaneBGR888(),pDst,					\
			fn<CFastPlaneBGR888>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 7: {											\
		CFastPlaneXRGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXRGB8888(),pSrc,					\
			CFastPlaneXRGB8888(),pDst,					\
			fn<CFastPlaneXRGB8888>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 8: {											\
		CFastPlaneXBGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXBGR8888(),pSrc,					\
			CFastPlaneXBGR8888(),pDst,					\
			fn<CFastPlaneXBGR8888>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 10: {											\
		CFastPlaneARGB4565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4565(),pSrc,					\
			CFastPlaneARGB4565(),pDst,					\
			fn<CFastPlaneARGB4565>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 11: {											\
		CFastPlaneARGB4555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4555(),pSrc,					\
			CFastPlaneARGB4555(),pDst,					\
			fn<CFastPlaneARGB4555>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 12: {											\
		CFastPlaneARGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB8888(),pSrc,					\
			CFastPlaneARGB8888(),pDst,					\
			fn<CFastPlaneARGB8888>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 13: {											\
		CFastPlaneABGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneABGR8888(),pSrc,					\
			CFastPlaneABGR8888(),pDst,					\
			fn<CFastPlaneABGR8888>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	}													\
														\
	return 0;											\
														\
	}	//	the end of function BltHelper2_3

/** この関数の引数はBlt2とコンパチ
	ただし、fnは、dwを食わせてから実体化

		CSurfaceInfo* pDst	:	エフェクトを施す対象
		LPCRECT		lpRect	:	エフェクトを施す範囲
		template<FN>	Fn	:	エフェクター
		ISurfaceRGB		dw	:	エフェクターのためのパラメータ
		(ARGB8888)
*/
#define CSurfaceBltter_EffectHelper1(pDst,lpRect,fn,dw){			\
	if (pDst->Lock()!=0) return 1;						\
	CSurfaceLockerGuard guard(pDst);					\
	int nS1 = pDst->GetSurfaceType();					\
	BYTE a = (BYTE)(dw>>24) , r = (BYTE)((dw>>16)&0xff),	\
		g = (BYTE)((dw>>8)&0xff) , b = (BYTE)(dw & 0xff);	\
	switch( nS1 ) {										\
	case 3: {											\
		CFastPlaneRGB565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneRGB565565 rgb2;						\
		rgb2.SetRGBWORD(rgb.GetRGB());					\
		CFastPlaneEffect::EffectDouble(					\
			CFastPlaneRGB565(),pDst,					\
			CFastPlaneRGB565565(),						\
			fn<CFastPlaneRGB565>(rgb),					\
			fn<CFastPlaneRGB565565>(rgb2),				\
			lpRect); } break;							\
	case 4: {											\
		CFastPlaneRGB555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneRGB555555 rgb2;						\
		rgb2.SetRGBWORD(rgb.GetRGB());					\
		CFastPlaneEffect::EffectDouble(					\
			CFastPlaneRGB555(),pDst,					\
			CFastPlaneRGB555555(),						\
			fn<CFastPlaneRGB555>(rgb),					\
			fn<CFastPlaneRGB555555>(rgb2),				\
			lpRect); } break;							\
	case 5: {											\
		CFastPlaneRGB888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Effect(						\
			CFastPlaneRGB888(),pDst,					\
			fn<CFastPlaneRGB888>(rgb),					\
			lpRect); } break;							\
	case 6: {											\
		CFastPlaneBGR888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Effect(						\
			CFastPlaneBGR888(),pDst,					\
			fn<CFastPlaneBGR888>(rgb),					\
			lpRect); } break;							\
	case 7: {											\
		CFastPlaneXRGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Effect(						\
			CFastPlaneXRGB8888(),pDst,					\
			fn<CFastPlaneXRGB8888>(rgb),				\
			lpRect); } break;							\
	case 8: {											\
		CFastPlaneXBGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Effect(						\
			CFastPlaneXBGR8888(),pDst,					\
			fn<CFastPlaneXBGR8888>(rgb),				\
			lpRect); } break;							\
	case 10: {											\
		CFastPlaneARGB4565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Effect(						\
			CFastPlaneARGB4565(),pDst,					\
			fn<CFastPlaneARGB4565>(rgb),				\
			lpRect); } break;							\
	case 11: {											\
		CFastPlaneARGB4555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Effect(						\
			CFastPlaneARGB4555(),pDst,					\
			fn<CFastPlaneARGB4555>(rgb),				\
			lpRect); } break;							\
	case 12: {											\
		CFastPlaneARGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Effect(						\
			CFastPlaneARGB8888(),pDst,					\
			fn<CFastPlaneARGB8888>(rgb),				\
			lpRect); } break;							\
	case 13: {											\
		CFastPlaneABGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Effect(						\
			CFastPlaneABGR8888(),pDst,					\
			fn<CFastPlaneABGR8888>(rgb),				\
			lpRect); } break;							\
	}													\
	return 0;											\
	}	//	end of CSurfaceBltter_EffectHelper1

#define CSurfaceBltter_EffectHelper1b(pDst,lpRect,fn,dw){			\
	if (pDst->Lock()!=0) return 1;						\
	CSurfaceLockerGuard guard(pDst);					\
	int nS1 = pDst->GetSurfaceType();					\
	BYTE a = (BYTE)(dw>>24) , r = (BYTE)((dw>>16)&0xff),	\
		g = (BYTE)((dw>>8)&0xff) , b = (BYTE)(dw & 0xff);	\
	switch( nS1 ) {										\
	case 3: {											\
		CFastPlaneRGB565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneXRGB8888 rgb2;						\
		rgb2.SetRGB(rgb.GetRGB_DWORD());				\
		CFastPlaneEffect::EffectDouble(					\
			CFastPlaneRGB565(),pDst,					\
			CFastPlaneXRGB8888(),						\
			fn<CFastPlaneRGB565>(rgb),					\
			fn<CFastPlaneXRGB8888>(rgb2),				\
			lpRect); } break;							\
	case 4: {											\
		CFastPlaneRGB555 rgb;							\
		CFastPlaneXRGB8888 rgb2;						\
		rgb.SetRGB(r,g,b);								\
		rgb2.SetRGB(rgb.GetRGB_DWORD());				\
		CFastPlaneEffect::EffectDouble(					\
			CFastPlaneRGB555(),pDst,					\
			CFastPlaneXRGB8888(),						\
			fn<CFastPlaneRGB555>(rgb),					\
			fn<CFastPlaneXRGB8888>(rgb2),				\
			lpRect); } break;							\
	case 5: {											\
		CFastPlaneRGB888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Effect(						\
			CFastPlaneRGB888(),pDst,					\
			fn<CFastPlaneRGB888>(rgb),					\
			lpRect); } break;							\
	case 6: {											\
		CFastPlaneBGR888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Effect(						\
			CFastPlaneBGR888(),pDst,					\
			fn<CFastPlaneBGR888>(rgb),					\
			lpRect); } break;							\
	case 7: {											\
		CFastPlaneXRGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Effect(						\
			CFastPlaneXRGB8888(),pDst,					\
			fn<CFastPlaneXRGB8888>(rgb),				\
			lpRect); } break;							\
	case 8: {											\
		CFastPlaneXBGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Effect(						\
			CFastPlaneXBGR8888(),pDst,					\
			fn<CFastPlaneXBGR8888>(rgb),				\
			lpRect); } break;							\
	case 10: {											\
		CFastPlaneARGB4565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Effect(						\
			CFastPlaneARGB4565(),pDst,					\
			fn<CFastPlaneARGB4565>(rgb),				\
			lpRect); } break;							\
	case 11: {											\
		CFastPlaneARGB4555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Effect(						\
			CFastPlaneARGB4555(),pDst,					\
			fn<CFastPlaneARGB4555>(rgb),				\
			lpRect); } break;							\
	case 12: {											\
		CFastPlaneARGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Effect(						\
			CFastPlaneARGB8888(),pDst,					\
			fn<CFastPlaneARGB8888>(rgb),				\
			lpRect); } break;							\
	case 13: {											\
		CFastPlaneABGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Effect(						\
			CFastPlaneABGR8888(),pDst,					\
			fn<CFastPlaneABGR8888>(rgb),				\
			lpRect); } break;							\
	}													\
	return 0;											\
	}	//	end of CSurfaceBltter_EffectHelper1b

	template <class Fn>
	static LRESULT Blt3(const CSurfaceInfo* pSrc,CSurfaceInfo* pDst,
		const CSurfaceInfo::CBltInfo *pInfo,Fn fn){
	//	このようにコーディングしておけば、この関数を抜けるときに自動的にデストラクタが呼び出され、
	//	正常にUnlockされる：
	if (pDst->Lock()!=0) return 1;
	CSurfaceLockerGuard g1(pDst); 
	if (pSrc->Lock()!=0) return 1;
	CSurfaceLockerGuard g2(pSrc); 

	int nS1 = pDst->GetSurfaceType();
	int nS2 = pSrc->GetSurfaceType();

	//	異なるサーフェース間の転送は基本的にサポートしない！
	if (nS1!=nS2) {

		//	特別サポート！＾＾；
		//	αサーフェースからの転送
		//	αプレーンとの親和性
		if (nS1==3 && nS2==10) {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,pInfo);
		}ef(nS1==4 && nS2==11) {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,pInfo);
		}ef(nS1==5 && nS2==12){
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,pInfo);
		}ef(nS1==6 && nS2==13){
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,pInfo);
		}ef(nS1==7 && nS2==12){
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,pInfo);
		}ef(nS1==8 && nS2==13){
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,pInfo);
		}
		
		//	おまけでサポート＾＾；
		//	αサーフェースへの転送

		//	RGB565 -> ARGB4565はサポート
		ef (nS2==3 && nS1==10){
			CFastPlaneEffect::Blt(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneARGB4565(),pDst,
				fn,pInfo);
		}ef(nS2==4 && nS1==11){
		//	RGB555 -> ARGB4555はサポート
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneARGB4555(),pDst,
				fn,pInfo);
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ARGB8888はサポート
		}ef(nS1==12){
			switch (nS2){
			case 5: {
				CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 6: {
				CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 7: {
				CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 8: {
				CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			default:
				//	残念ながら非サポートでした＾＾；
				return 2;
			}
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ABGR8888はサポート
		}ef(nS1==13){
			switch (nS2){
			case 5: {
				CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 6: {
				CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 7: {
				CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 8: {
				CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			default:
				//	残念ながら非サポートでした＾＾；
				return 2;
			}
		} else {
			//	残念ながら非サポートでした＾＾；
			return 2;
		}
		return 0;
	}

	switch( nS1 ) {
		///////////////////////////////////
		//	↓↓　ここだけがBlt2と違う　↓↓
	case 3:	{
		CFastPlaneEffect::BltDouble(
			CFastPlaneRGB565(),pSrc,
			CFastPlaneRGB565(),pDst,
			CFastPlaneRGB565565(),
			CFastPlaneRGB565565(),
				fn,fn,pInfo);
				break;
		}
	case 4: {
		CFastPlaneEffect::BltDouble(
			CFastPlaneRGB555(),pSrc,
			CFastPlaneRGB555(),pDst,
			CFastPlaneRGB555555(),
			CFastPlaneRGB555555(),
				fn,fn,pInfo);
				break;
		}
		//	↑↑　ここだけがBlt2と違う　↑↑
		///////////////////////////////////
	case 5:	{
		CFastPlaneEffect::Blt(
			CFastPlaneRGB888(),pSrc,
			CFastPlaneRGB888(),pDst,
				fn,pInfo);
				break;
		}
	case 6: {
		CFastPlaneEffect::Blt(
			CFastPlaneBGR888(),pSrc,
			CFastPlaneBGR888(),pDst,
				fn,pInfo);
				break;
		}
	case 7: {
		CFastPlaneEffect::Blt(
			CFastPlaneXRGB8888(),pSrc,
			CFastPlaneXRGB8888(),pDst,
				fn,pInfo);
				break;
		}
	case 8: {
		CFastPlaneEffect::Blt(
			CFastPlaneXBGR8888(),pSrc,
			CFastPlaneXBGR8888(),pDst,
				fn,pInfo);
				break;
		}
	case 10: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4565(),pSrc,
			CFastPlaneARGB4565(),pDst,
				fn,pInfo);
				break;
		}
	case 11: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4555(),pSrc,
			CFastPlaneARGB4555(),pDst,
				fn,pInfo);
				break;
		}
	case 12: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB8888(),pSrc,
			CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
		}
	case 13: {
		CFastPlaneEffect::Blt(
			CFastPlaneABGR8888(),pSrc,
			CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
		}
	}

	return 0;
	}	//	the end of function Blt3

// この関数の引数はBlt4とコンパチ
//	ただし、fnは、dwを食わせてから実体化
#define	CSurfaceBltter_BltHelper4(pSrc,pDst,pInfo,fn,dw){		\
	if (pDst->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g1(pDst);						\
	if (pSrc->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g2(pSrc);						\
	int nS1 = pDst->GetSurfaceType();					\
	int nS2 = pSrc->GetSurfaceType();					\
	BYTE a = (BYTE)(dw>>24) , r = (BYTE)((dw>>16)&0xff),	\
		g = (BYTE)((dw>>8)&0xff) , b = (BYTE)(dw & 0xff);	\
	if (nS1!=nS2) {										\
		return 2;										\
	}													\
	switch( nS1 ) {										\
	case 3: {											\
		CFastPlaneRGB565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneRGB565565 rgb2;						\
		rgb2.SetRGBWORD(rgb.GetRGB());					\
		CFastPlaneEffect::BltDouble(					\
			CFastPlaneRGB565(),pSrc,					\
			CFastPlaneRGB565(),pDst,					\
			CFastPlaneRGB565565(),						\
			CFastPlaneRGB565565(),						\
			fn<CFastPlaneRGB565>(rgb),					\
			fn<CFastPlaneRGB565565>(rgb2),				\
				pInfo);									\
					} break;							\
	case 4: {											\
		CFastPlaneRGB555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneRGB555555 rgb2;						\
		rgb2.SetRGBWORD(rgb.GetRGB());					\
		CFastPlaneEffect::BltDouble(					\
			CFastPlaneRGB555(),pSrc,					\
			CFastPlaneRGB555(),pDst,					\
			CFastPlaneRGB555555(),						\
			CFastPlaneRGB555555(),						\
			fn<CFastPlaneRGB555>(rgb),					\
			fn<CFastPlaneRGB555555>(rgb2),				\
				pInfo);									\
					} break;							\
	case 5:	{											\
		CFastPlaneRGB888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB888(),pSrc,					\
			CFastPlaneRGB888(),pDst,					\
			fn<CFastPlaneRGB888>(rgb),pInfo);			\
				break;									\
		}												\
	case 6: {											\
		CFastPlaneBGR888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneBGR888(),pSrc,					\
			CFastPlaneBGR888(),pDst,					\
			fn<CFastPlaneBGR888>(rgb),pInfo);			\
				break;									\
		}												\
	case 7: {											\
		CFastPlaneXRGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXRGB8888(),pSrc,					\
			CFastPlaneXRGB8888(),pDst,					\
			fn<CFastPlaneXRGB8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 8: {											\
		CFastPlaneXBGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXBGR8888(),pSrc,					\
			CFastPlaneXBGR8888(),pDst,					\
			fn<CFastPlaneXBGR8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 10: {											\
		CFastPlaneARGB4565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4565(),pSrc,					\
			CFastPlaneARGB4565(),pDst,					\
			fn<CFastPlaneARGB4565>(rgb),pInfo);			\
				break;									\
		}												\
	case 11: {											\
		CFastPlaneARGB4555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4555(),pSrc,					\
			CFastPlaneARGB4555(),pDst,					\
			fn<CFastPlaneARGB4555>(rgb),pInfo);			\
				break;									\
		}												\
	case 12: {											\
		CFastPlaneARGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB8888(),pSrc,					\
			CFastPlaneARGB8888(),pDst,					\
			fn<CFastPlaneARGB8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 13: {											\
		CFastPlaneABGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneABGR8888(),pSrc,					\
			CFastPlaneABGR8888(),pDst,					\
			fn<CFastPlaneABGR8888>(rgb),pInfo);			\
				break;									\
		}												\
	}													\
														\
	return 0;											\
														\
	}	//	the end of function BltHelper4

// この関数の引数はBlt5とコンパチ
//	ただし、fnは、dw(ISurfaceRGB),p1を食わせてから実体化
#define	CSurfaceBltter_BltHelper5_2(pSrc,pDst,pInfo,fn,dw,p1){	\
	if (pDst->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g1(pDst);						\
	if (pSrc->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g2(pSrc);						\
	int nS1 = pDst->GetSurfaceType();					\
	int nS2 = pSrc->GetSurfaceType();					\
	BYTE a = (BYTE)(dw>>24) , r = (BYTE)((dw>>16)&0xff),	\
		g = (BYTE)((dw>>8)&0xff) , b = (BYTE)(dw & 0xff);	\
	if (nS1!=nS2) {										\
		return 2;										\
	}													\
	switch( nS1 ) {										\
	case 3:	{											\
		CFastPlaneRGB565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB565(),pSrc,					\
			CFastPlaneRGB565(),pDst,					\
			fn<CFastPlaneRGB565>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 4:	{											\
		CFastPlaneRGB555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB555(),pSrc,					\
			CFastPlaneRGB555(),pDst,					\
			fn<CFastPlaneRGB555>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 6: {											\
		CFastPlaneBGR888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneBGR888(),pSrc,					\
			CFastPlaneBGR888(),pDst,					\
			fn<CFastPlaneBGR888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 7: {											\
		CFastPlaneXRGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXRGB8888(),pSrc,					\
			CFastPlaneXRGB8888(),pDst,					\
			fn<CFastPlaneXRGB8888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 8: {											\
		CFastPlaneXBGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXBGR8888(),pSrc,					\
			CFastPlaneXBGR8888(),pDst,					\
			fn<CFastPlaneXBGR8888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 10: {											\
		CFastPlaneARGB4565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4565(),pSrc,					\
			CFastPlaneARGB4565(),pDst,					\
			fn<CFastPlaneARGB4565>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 11: {											\
		CFastPlaneARGB4555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4555(),pSrc,					\
			CFastPlaneARGB4555(),pDst,					\
			fn<CFastPlaneARGB4555>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 12: {											\
		CFastPlaneARGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB8888(),pSrc,					\
			CFastPlaneARGB8888(),pDst,					\
			fn<CFastPlaneARGB8888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 13: {											\
		CFastPlaneABGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneABGR8888(),pSrc,					\
			CFastPlaneABGR8888(),pDst,					\
			fn<CFastPlaneABGR8888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	}													\
														\
	return 0;											\
														\
	}	//	the end of function BltHelper5_2

/**
	fn , fn2(αサーフェースに対するエフェクタ)
*/
	template <class Fn,class Fn2>
	static LRESULT Blt6(const CSurfaceInfo* pSrc,CSurfaceInfo* pDst,
		const CSurfaceInfo::CBltInfo *pInfo,Fn fn,Fn2 fn2){
	//	このようにコーディングしておけば、この関数を抜けるときに自動的にデストラクタが呼び出され、
	//	正常にUnlockされる：
	if (pDst->Lock()!=0) return 1;
	CSurfaceLockerGuard g1(pDst); 
	if (pSrc->Lock()!=0) return 1;
	CSurfaceLockerGuard g2(pSrc); 

	int nS1 = pDst->GetSurfaceType();
	int nS2 = pSrc->GetSurfaceType();

	//	α付きサーフェースから、α無しサーフェースへの転送しかサポートしない！
	//	α無しから、α付きへは、Blt,BltFastでいいでしょ
	//	α付きサーフェースからα付きサーフェースへの転送もサポートしとこっかな．．
	if (((nS1==3 || nS1==10) && nS2==10) ||
		((nS1==4 || nS1==11) && nS2==11) ||
		((nS1==5 || nS1==7 || nS1==12)&&(nS2==12))||
		((nS1==6 || nS1==8 || nS1==13)&&(nS2==13))
		) {
		//	ok , surface with alpha to normal surface
	} else {
		return 2;
	}

	switch( nS1 ) {
	case 3: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4565(),pSrc,
			CFastPlaneRGB565(),pDst,
			fn,pInfo); break;
		}
	case 4: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4555(),pSrc,
			CFastPlaneRGB555(),pDst,
			fn,pInfo); break;
		}
	case 5: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB8888(),pSrc,
			CFastPlaneRGB888(),pDst,
			fn,pInfo); break;
		}
	case 6: {
		CFastPlaneEffect::Blt(
			CFastPlaneABGR8888(),pSrc,
			CFastPlaneBGR888(),pDst,
			fn,pInfo); break;
		}
	case 7: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB8888(),pSrc,
			CFastPlaneXRGB8888(),pDst,
			fn,pInfo); break;
		}
	case 8: {
		CFastPlaneEffect::Blt(
			CFastPlaneABGR8888(),pSrc,
			CFastPlaneXBGR8888(),pDst,
			fn,pInfo); break;
		}

	case 10: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4565(),pSrc,
			CFastPlaneARGB4565(),pDst,
			fn2,pInfo); break;
		}
	case 11: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4555(),pSrc,
			CFastPlaneARGB4555(),pDst,
			fn2,pInfo); break;
		}
	case 12: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB8888(),pSrc,
			CFastPlaneARGB8888(),pDst,
			fn2,pInfo); break;
		}
	case 13: {
		CFastPlaneEffect::Blt(
			CFastPlaneABGR8888(),pSrc,
			CFastPlaneABGR8888(),pDst,
			fn2,pInfo); break;
		}
	}

	return 0;
	}	//	the end of function Blt6


	template <class Fn>
	static LRESULT Blt7(CSurfaceInfo* pSrc,CSurfaceInfo* pDst,CSurfaceInfo::CBltInfo *pInfo,Fn fn){
	//	このようにコーディングしておけば、この関数を抜けるときに自動的にデストラクタが呼び出され、
	//	正常にUnlockされる：
	if (pDst->Lock()!=0) return 1;
	CSurfaceLockerGuard g1(pDst); 
	if (pSrc->Lock()!=0) return 1;
	CSurfaceLockerGuard g2(pSrc); 

	int nS1 = pDst->GetSurfaceType();
	int nS2 = pSrc->GetSurfaceType();

	//	異なるサーフェース間の転送は基本的にサポートしない！
	if (nS1!=nS2) {

		//	特別サポート！＾＾；
		//	αサーフェースからの転送
		//	αプレーンとの親和性
		if (nS1==3 && nS2==10) {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,pInfo);
		}ef(nS1==4 && nS2==11) {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,pInfo);
		}ef(nS1==5 && nS2==12){
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,pInfo);
		}ef(nS1==6 && nS2==13){
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,pInfo);
		}ef(nS1==7 && nS2==12){
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,pInfo);
		}ef(nS1==8 && nS2==13){
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,pInfo);
		} else {
			//	残念ながら非サポートでした＾＾；
			return 2;
		}
		return 0;
	}

	switch( nS1 ) {
	case 3:	{
		CFastPlaneEffect::Blt(
			CFastPlaneRGB565(),pSrc,
			CFastPlaneRGB565(),pDst,
				fn,pInfo);
				break;
		}
	case 4: {
		CFastPlaneEffect::Blt(
			CFastPlaneRGB555(),pSrc,
			CFastPlaneRGB555(),pDst,
				fn,pInfo);
				break;
		}
	case 5:	{
		CFastPlaneEffect::Blt(
			CFastPlaneRGB888(),pSrc,
			CFastPlaneRGB888(),pDst,
				fn,pInfo);
				break;
		}
	case 6: {
		CFastPlaneEffect::Blt(
			CFastPlaneBGR888(),pSrc,
			CFastPlaneBGR888(),pDst,
				fn,pInfo);
				break;
		}
	case 7: {
		CFastPlaneEffect::Blt(
			CFastPlaneXRGB8888(),pSrc,
			CFastPlaneXRGB8888(),pDst,
				fn,pInfo);
				break;
		}
	case 8: {
		CFastPlaneEffect::Blt(
			CFastPlaneXBGR8888(),pSrc,
			CFastPlaneXBGR8888(),pDst,
				fn,pInfo);
				break;
		}
	case 10: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4565(),pSrc,
			CFastPlaneARGB4565(),pDst,
				fn,pInfo);
				break;
		}
	case 11: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4555(),pSrc,
			CFastPlaneARGB4555(),pDst,
				fn,pInfo);
				break;
		}
	case 12: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB8888(),pSrc,
			CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
		}
	case 13: {
		CFastPlaneEffect::Blt(
			CFastPlaneABGR8888(),pSrc,
			CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
		}
	}

	return 0;
	}	//	the end of function Blt7


	template <class Fn>
	static LRESULT Effect2(CSurfaceInfo* pSrc,LPCRECT prc,Fn fn){
	//	このようにコーディングしておけば、この関数を抜けるときに自動的にデストラクタが呼び出され、
	//	正常にUnlockされる：
	if (pSrc->Lock()!=0) return 1;
	CSurfaceLockerGuard g1(pSrc); 

	int nS1 = pSrc->GetSurfaceType();

	switch( nS1 ) {
		///////////////////////////////////
		//	↓↓　ここだけがEffect1と違う　↓↓
	case 3:	{
		CFastPlaneEffect::Effect(
			CFastPlaneRGB565(),pSrc,
			fn,prc); } break;
	case 4: {
		CFastPlaneEffect::Effect(
			CFastPlaneRGB555(),pSrc,
			fn,prc); } break;
		//	↑↑　ここだけがBlt2と違う　↑↑
		///////////////////////////////////
	case 5:	{
		CFastPlaneEffect::Effect(
			CFastPlaneRGB888(),pSrc,
			fn,prc); } break;
	case 6: {
		CFastPlaneEffect::Effect(
			CFastPlaneBGR888(),pSrc,
			fn,prc); } break;
	case 7: {
		CFastPlaneEffect::Effect(
			CFastPlaneXRGB8888(),pSrc,
			fn,prc); } break;
	case 8: {
		CFastPlaneEffect::Effect(
			CFastPlaneXBGR8888(),pSrc,
			fn,prc); } break;
	case 10: {
		CFastPlaneEffect::Effect(
			CFastPlaneARGB4565(),pSrc,
			fn,prc); } break;
	case 11: {
		CFastPlaneEffect::Effect(
			CFastPlaneARGB4555(),pSrc,
			fn,prc); } break;
	case 12: {
		CFastPlaneEffect::Effect(
			CFastPlaneARGB8888(),pSrc,
			fn,prc); } break;
	case 13: {
		CFastPlaneEffect::Effect(
			CFastPlaneABGR8888(),pSrc,
			fn,prc); } break;
	}

	return 0;
	}	//	the end of function Effect2

	template <class Fn>
	static LRESULT Mosaic2(CSurfaceInfo* pSrc,LPCRECT prc,Fn fn,int d){
	//	このようにコーディングしておけば、この関数を抜けるときに自動的にデストラクタが呼び出され、
	//	正常にUnlockされる：
	if (pSrc->Lock()!=0) return 1;
	CSurfaceLockerGuard g1(pSrc); 

	int nS1 = pSrc->GetSurfaceType();

	switch( nS1 ) {
	case 3:	{
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneRGB565(),pSrc,
			fn,d,prc); } break;
	case 4: {
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneRGB555(),pSrc,
			fn,d,prc); } break;
	case 5:	{
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneRGB888(),pSrc,
			fn,d,prc); } break;
	case 6: {
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneBGR888(),pSrc,
			fn,d,prc); } break;
	case 7: {
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneXRGB8888(),pSrc,
			fn,d,prc); } break;
	case 8: {
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneXBGR8888(),pSrc,
			fn,d,prc); } break;
	case 10: {
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneARGB4565(),pSrc,
			fn,d,prc); } break;
	case 11: {
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneARGB4555(),pSrc,
			fn,d,prc); } break;
	case 12: {
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneARGB8888(),pSrc,
			fn,d,prc); } break;
	case 13: {
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneABGR8888(),pSrc,
			fn,d,prc); } break;
	}

	return 0;
	}	//	the end of function Mosaic2

	//////////////////////////////////////////////////////////////////////
	///	Morph系

	template <class Fn>
	static LRESULT Morph2(const CSurfaceInfo* pSrc,CSurfaceInfo* pDst,const CSurfaceInfo::CMorphInfo *pInfo,Fn fn){
	//	このようにコーディングしておけば、この関数を抜けるときに自動的にデストラクタが呼び出され、
	//	正常にUnlockされる：
	if (pDst->Lock()!=0) return 1;
	CSurfaceLockerGuard g1(pDst); 
	if (pSrc->Lock()!=0) return 1;
	CSurfaceLockerGuard g2(pSrc); 

	int nS1 = pDst->GetSurfaceType();
	int nS2 = pSrc->GetSurfaceType();

	//	異なるサーフェース間の転送は基本的にサポートしない！
	if (nS1!=nS2) {

		//	特別サポート！＾＾；
		//	αサーフェースからの転送
		//	αプレーンとの親和性
		if (nS1==3 && nS2==10) {
			CFastPlaneEffect::Morph(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,pInfo);
		}ef(nS1==4 && nS2==11) {
			CFastPlaneEffect::Morph(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,pInfo);
		}ef(nS1==5 && nS2==12){
			CFastPlaneEffect::Morph(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,pInfo);
		}ef(nS1==6 && nS2==13){
			CFastPlaneEffect::Morph(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,pInfo);
		}ef(nS1==7 && nS2==12){
			CFastPlaneEffect::Morph(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,pInfo);
		}ef(nS1==8 && nS2==13){
			CFastPlaneEffect::Morph(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,pInfo);
		}
		
		//	おまけでサポート＾＾；
		//	αサーフェースへの転送

		//	RGB565 -> ARGB4565はサポート
		ef (nS2==3 && nS1==10){
			CFastPlaneEffect::Morph(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneARGB4565(),pDst,
				fn,pInfo);
		}ef(nS2==4 && nS1==11){
		//	RGB555 -> ARGB4555はサポート
			CFastPlaneEffect::Morph(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneARGB4555(),pDst,
				fn,pInfo);
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ARGB8888はサポート
		}ef(nS1==12){
			switch (nS2){
			case 5: {
				CFastPlaneEffect::Morph(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 6: {
				CFastPlaneEffect::Morph(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 7: {
				CFastPlaneEffect::Morph(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 8: {
				CFastPlaneEffect::Morph(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			default:
				//	残念ながら非サポートでした＾＾；
				return 2;
			}
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ABGR8888はサポート
		}ef(nS1==13){
			switch (nS2){
			case 5: {
				CFastPlaneEffect::Morph(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 6: {
				CFastPlaneEffect::Morph(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 7: {
				CFastPlaneEffect::Morph(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 8: {
				CFastPlaneEffect::Morph(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			default:
				//	残念ながら非サポートでした＾＾；
				return 2;
			}
		} else {
			//	残念ながら非サポートでした＾＾；
			return 2;
		}
		return 0;
	}

	switch( nS1 ) {
	case 3:	{
		CFastPlaneEffect::Morph(
			CFastPlaneRGB565(),pSrc,
			CFastPlaneRGB565(),pDst,
				fn,pInfo);
				break;
		}
	case 4: {
		CFastPlaneEffect::Morph(
			CFastPlaneRGB555(),pSrc,
			CFastPlaneRGB555(),pDst,
				fn,pInfo);
				break;
		}
	case 5:	{
		CFastPlaneEffect::Morph(
			CFastPlaneRGB888(),pSrc,
			CFastPlaneRGB888(),pDst,
				fn,pInfo);
				break;
		}
	case 6: {
		CFastPlaneEffect::Morph(
			CFastPlaneBGR888(),pSrc,
			CFastPlaneBGR888(),pDst,
				fn,pInfo);
				break;
		}
	case 7: {
		CFastPlaneEffect::Morph(
			CFastPlaneXRGB8888(),pSrc,
			CFastPlaneXRGB8888(),pDst,
				fn,pInfo);
				break;
		}
	case 8: {
		CFastPlaneEffect::Morph(
			CFastPlaneXBGR8888(),pSrc,
			CFastPlaneXBGR8888(),pDst,
				fn,pInfo);
				break;
		}
	case 10: {
		CFastPlaneEffect::Morph(
			CFastPlaneARGB4565(),pSrc,
			CFastPlaneARGB4565(),pDst,
				fn,pInfo);
				break;
		}
	case 11: {
		CFastPlaneEffect::Morph(
			CFastPlaneARGB4555(),pSrc,
			CFastPlaneARGB4555(),pDst,
				fn,pInfo);
				break;
		}
	case 12: {
		CFastPlaneEffect::Morph(
			CFastPlaneARGB8888(),pSrc,
			CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
		}
	case 13: {
		CFastPlaneEffect::Morph(
			CFastPlaneABGR8888(),pSrc,
			CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
		}
	}

	return 0;
	}	//	the end of function Morph2

// この関数の引数はBlt2とコンパチ
//	ただし、fnは、dwを食わせてから実体化
#define	CSurfaceBltter_MorphHelper2(pSrc,pDst,pInfo,fn,dw){		\
	if (pDst->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g1(pDst);						\
	if (pSrc->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g2(pSrc);						\
	int nS1 = pDst->GetSurfaceType();					\
	int nS2 = pSrc->GetSurfaceType();					\
	BYTE a = (BYTE)(dw>>24) , r = (BYTE)((dw>>16)&0xff),	\
		g = (BYTE)((dw>>8)&0xff) , b = (BYTE)(dw & 0xff);	\
	if (nS1!=nS2) {										\
		if (nS1==3 && nS2==10) {						\
			CFastPlaneARGB4565 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Morph(					\
				CFastPlaneARGB4565(),pSrc,				\
				CFastPlaneRGB565(),pDst,				\
				fn<CFastPlaneARGB4565>(rgb),pInfo);		\
		}ef(nS1==4 && nS2==11) {						\
			CFastPlaneARGB4555 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Morph(					\
				CFastPlaneARGB4555(),pSrc,				\
				CFastPlaneRGB555(),pDst,				\
				fn<CFastPlaneARGB4555>(rgb),pInfo);		\
		}ef(nS1==5 && nS2==12){							\
			CFastPlaneARGB8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Morph(					\
				CFastPlaneARGB8888(),pSrc,				\
				CFastPlaneRGB888(),pDst,				\
				fn<CFastPlaneARGB8888>(rgb),pInfo);		\
		}ef(nS1==6 && nS2==13){							\
			CFastPlaneABGR8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Morph(					\
				CFastPlaneABGR8888(),pSrc,				\
				CFastPlaneBGR888(),pDst,				\
				fn<CFastPlaneABGR8888>(rgb),pInfo);		\
		}ef(nS1==7 && nS2==12){							\
			CFastPlaneARGB8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Morph(					\
				CFastPlaneARGB8888(),pSrc,				\
				CFastPlaneXRGB8888(),pDst,				\
				fn<CFastPlaneARGB8888>(rgb),pInfo);		\
		}ef(nS1==8 && nS2==13){							\
			CFastPlaneABGR8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Morph(					\
				CFastPlaneABGR8888(),pSrc,				\
				CFastPlaneXBGR8888(),pDst,				\
				fn<CFastPlaneABGR8888>(rgb),pInfo);		\
		}												\
														\
		ef (nS2==3 && nS1==10){							\
			CFastPlaneRGB565 rgb;						\
			rgb.SetRGB(r,g,b);							\
			CFastPlaneEffect::Morph(					\
				CFastPlaneRGB565(),pSrc,				\
				CFastPlaneARGB4565(),pDst,				\
				fn<CFastPlaneRGB565>(rgb),pInfo);		\
		}ef(nS2==4 && nS1==11){							\
			CFastPlaneRGB555 rgb;						\
			rgb.SetRGB(r,g,b);							\
			CFastPlaneEffect::Morph(					\
				CFastPlaneRGB555(),pSrc,				\
				CFastPlaneARGB4555(),pDst,				\
				fn<CFastPlaneRGB555>(rgb),pInfo);		\
		}ef(nS1==12){									\
			switch (nS2){								\
			case 5: {									\
				CFastPlaneRGB888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Morph(				\
				CFastPlaneRGB888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneRGB888>(rgb),pInfo);		\
				break;									\
					}									\
			case 6: {									\
				CFastPlaneBGR888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Morph(				\
				CFastPlaneBGR888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneBGR888>(rgb),pInfo);		\
				break;									\
					}									\
			case 7: {									\
				CFastPlaneXRGB8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Morph(				\
				CFastPlaneXRGB8888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneXRGB8888>(rgb),pInfo);		\
				break;									\
					}									\
			case 8: {									\
				CFastPlaneXBGR8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Morph(				\
				CFastPlaneXBGR8888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneXBGR8888>(rgb),pInfo);		\
				break;									\
					}									\
			default:									\
				return 2;								\
			}											\
		}ef(nS1==13){									\
			switch (nS2){								\
			case 5: {									\
				CFastPlaneRGB888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Morph(				\
				CFastPlaneRGB888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneRGB888>(rgb),pInfo);		\
				break;									\
					}									\
			case 6: {									\
				CFastPlaneBGR888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Morph(				\
				CFastPlaneBGR888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneBGR888>(rgb),pInfo);		\
				break;									\
					}									\
			case 7: {									\
				CFastPlaneXRGB8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Morph(				\
				CFastPlaneXRGB8888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneXRGB8888>(rgb),pInfo);		\
				break;									\
					}									\
			case 8: {									\
				CFastPlaneXBGR8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Morph(				\
				CFastPlaneXBGR8888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneXBGR8888>(rgb),pInfo);		\
				break;									\
					}									\
			default:									\
				return 2;								\
			}											\
		} else {										\
			return 2;									\
		}												\
		return 0;										\
	}													\
														\
	switch( nS1 ) {										\
	case 3:	{											\
		CFastPlaneRGB565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Morph(						\
			CFastPlaneRGB565(),pSrc,					\
			CFastPlaneRGB565(),pDst,					\
			fn<CFastPlaneRGB565>(rgb),pInfo);			\
				break;									\
		}												\
	case 4: {											\
		CFastPlaneRGB555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Morph(						\
			CFastPlaneRGB555(),pSrc,					\
			CFastPlaneRGB555(),pDst,					\
			fn<CFastPlaneRGB555>(rgb),pInfo);			\
				break;									\
		}												\
	case 5:	{											\
		CFastPlaneRGB888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Morph(						\
			CFastPlaneRGB888(),pSrc,					\
			CFastPlaneRGB888(),pDst,					\
			fn<CFastPlaneRGB888>(rgb),pInfo);			\
				break;									\
		}												\
	case 6: {											\
		CFastPlaneBGR888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Morph(						\
			CFastPlaneBGR888(),pSrc,					\
			CFastPlaneBGR888(),pDst,					\
			fn<CFastPlaneBGR888>(rgb),pInfo);			\
				break;									\
		}												\
	case 7: {											\
		CFastPlaneXRGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Morph(						\
			CFastPlaneXRGB8888(),pSrc,					\
			CFastPlaneXRGB8888(),pDst,					\
			fn<CFastPlaneXRGB8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 8: {											\
		CFastPlaneXBGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Morph(						\
			CFastPlaneXBGR8888(),pSrc,					\
			CFastPlaneXBGR8888(),pDst,					\
			fn<CFastPlaneXBGR8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 10: {											\
		CFastPlaneARGB4565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Morph(						\
			CFastPlaneARGB4565(),pSrc,					\
			CFastPlaneARGB4565(),pDst,					\
			fn<CFastPlaneARGB4565>(rgb),pInfo);			\
				break;									\
		}												\
	case 11: {											\
		CFastPlaneARGB4555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Morph(						\
			CFastPlaneARGB4555(),pSrc,					\
			CFastPlaneARGB4555(),pDst,					\
			fn<CFastPlaneARGB4555>(rgb),pInfo);			\
				break;									\
		}												\
	case 12: {											\
		CFastPlaneARGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Morph(						\
			CFastPlaneARGB8888(),pSrc,					\
			CFastPlaneARGB8888(),pDst,					\
			fn<CFastPlaneARGB8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 13: {											\
		CFastPlaneABGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Morph(						\
			CFastPlaneABGR8888(),pSrc,					\
			CFastPlaneABGR8888(),pDst,					\
			fn<CFastPlaneABGR8888>(rgb),pInfo);			\
				break;									\
		}												\
	}													\
														\
	return 0;											\
														\
	}	//	the end of function MorphHelper2

	//////////////////////////////////////////////////////////////////////
	template <class Src,class Dst,class Fn>
	static void Blt1Line(const Src* pSrc,Dst *pDst,int nLength,Fn fn){
	/**
		Src から Dstに１ライン分(nLength)分、転送する
		Surfaceのlockはしない
	*/
		for(int i=0;i<nLength;i++){
			fn(pDst[i],pSrc[i]);
		}
	}

	static LRESULT Blt1Line(const void* pSrc,int nSrcSurfaceType,void *pDst,int nDstSurfaceType,int nLength);
	/**
		Src から Dstに１ライン分(nLength)分、転送する
		Surfaceのlockはしない。サーフェースのタイプとは、
		CSurfaceInfoのサーフェースタイプと同じ意味。

		ただし、ソースサーフェースはすべてのパターンをサポートしている
		わけではない。

		サポートしている組み合わせ
		１．
			nSrcSurfaceType : 4(RGB555),5(RGB888),12(ARGB8888)
			nDstSurfaceType : 0,1,2,9はエラーリターン。2～13。
			転送元がRGB555,RGB888は、bmp画像でありうるから。
			転送元がARGB8888は、yga画像の読み込み用。
		２．
			nSrcSurfaceType : 0,1,2,9はエラーリターン。2～13。
			nDstSurfaceType : 5(RGB888),12(ARGB8888)
			bmpとygaの保存用
	*/

};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
