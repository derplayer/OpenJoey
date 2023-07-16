#include "stdafx.h"
#include "function_callback.h"
//	YTLのなかの.cppはこちらのフォルダに置いておいたほうが
//	プロジェクトに挿入するのが楽ちんぽん

namespace yaneuraoGameSDK3rd {
namespace YTL {

//	非テンプレートなのでinlineで書けなかったぶん
function_callback_v* function_callback_v::Create(void (*f)()){
	return new function_callback_vg0(f);
}

template <class Result>
function_callback_r<Result>* function_callback_r<Result>::Create(Result (*f)()){
	return new function_callback_rg0<Result>(f);
}
	//	↑テンプレートを外に書くのは大変ですなぁ(;´Д`)

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd
