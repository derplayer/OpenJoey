/*
	proxy_ptr	: a pointer treating proxy object
		reprogrammed & desinged by yaneurao(M.Isozaki) '02/03/07
*/

#ifndef __YTLProxyPtr_h__
#define __YTLProxyPtr_h__

namespace yaneuraoGameSDK3rd {
namespace YTL {

template<class T> class proxy_ptr {
/**
	このポインタは、
	必要に迫られたときに始めてobjectをnewするようなポインタ

	要求があるまで生成を保留する。いわゆる代理オブジェクト(virtual proxy)

	非ローカルなstaticなproxy_ptrを外部からsingleton的に使うときには
	初期化の問題があるので、
	class singleton のほうを使ってください。
*/
public:
	proxy_ptr() : m_lpObj(NULL) {}

	///	ポインタのふりをするための仕掛け
	T& operator*() { return *get(); }
	T* operator->() { return get();	}
	T* get() { CheckInstance(); return m_lpObj; }

	///	オブジェクトがNullかどうかを検証する
	bool	isNull() const { return m_lpObj == NULL; }

	///	オブジェクトを強制的に解体する
	///	（デストラクタでも解放している）
	void Release() {
		if (m_lpObj!=NULL) {
			delete m_lpObj;
			m_lpObj = NULL;
		}
	}

	void CheckInstance(){
		if (m_lpObj==NULL) m_lpObj = new T;
	}
	/**
		インスタンスが存在するのかそのチェックを行ない、
		存在しなければ生成する。本来、明示的に行なう必要は無いが、
		このオブジェクト自体が非常に初期化の時間のかかり、
		リアルタイムにその初期化を行なわれては困るときには、
		（必要なのがわかっていれば）事前に生成を行なっておく
		ことで、それを回避できる。
	*/

	~proxy_ptr() { Release(); }
protected:

	T*	m_lpObj;
};

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
