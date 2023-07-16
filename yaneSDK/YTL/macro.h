// yaneMacro.h :
//	簡単なマクロ集

#ifndef __yaneMacro__h__
#define __yaneMacro__h__

namespace yaneuraoGameSDK3rd {
namespace YTL {

/**
	値を交換するだけのマクロ＾＾；
*/
template<class T>
inline void Swap(T& x,T& y){
	T t;
	t = x;
	x = y;
	y = t;
};

///	ゼロで埋めるマクロ
template<class T>
inline void ZERO(T& var){
	::ZeroMemory(&var,sizeof(var));
}

/// ポインタが非NULLならばReleaseを掛けるマクロ。
template<class T>
inline void RELEASE_SAFE(T*& var){
	if (var != NULL) {
		var->Release();
		var = NULL;
	}
}

/// ポインタが非NULLならばdeleteを掛けるマクロ。
template<class T>
inline void DELETE_SAFE(T*& var){
	if (var != NULL) {
		delete var;
		var = NULL;
	}
}
///	↑これ使わずにsmart_ptrを使いましょうね＾＾；

/// 配列のポインタをdeleteするマクロ
template<class T>
	inline void DELETEPTR_SAFE(T*& var){
	if (var != NULL) {
		delete [] var;
		var = NULL;
	}
}
///	↑これ使わずにsmart_ptrを使いましょうね＾＾；

///	else ifマクロ（邪道ぎみ=p）
#define ef else if

///	配列の要素数を返すマクロ( Number of Elements)
#define NELEMS(array) (sizeof(array) / sizeof(array[0]))

///	WARNINGマクロ
#ifdef _DEBUG
	#define WARNING(b,s)				\
		if(b) {						\
			CHAR buf[256];				\
			wsprintf(buf,"%sファイルの%d行\n%s",__FILE__,__LINE__,s);	\
			MessageBox(NULL,buf,"WARNING!!",MB_OK);	\
			* LPLONG(0xcdcdcdcd) = 0;	\
		}
	//	最後の* LPLONG(0xcdcdcdcd) = 0;の部分は、メモリエラーをわざと起こすコード
#else
	#define WARNING(var,s) {}
	//	{}にしているのは、if文のあとWARNINGを入れているときなどのため。
#endif

///	トレース用のマクロ:ファイル名とラインナンバーをError.logに出力する
#define TRACE Err.Out("%s(%d)",__FILE__,__LINE__);

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
