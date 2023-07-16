/*
	ref_callback_deleter	: 参照カウンタ付きコールバック解体子
		programmed & desinged by yaneurao(M.Isozaki) '02/03/10
*/

#ifndef __YTLRefCallbackDeleter_h__
#define __YTLRefCallbackDeleter_h__

#include "function_callback.h"
#include "smart_ptr.h"
//	解体時（解体後）にコールバックもしてくれるオブジェクト

namespace yaneuraoGameSDK3rd {
namespace YTL {

//	非配列オブジェクトをdeleteするためのテンプレート
template<class T>
class nonarray_callback_ref_object : public ref_object {
/**
		非配列オブジェクトをdeleteするためのテンプレート
		解体時（解体後）にコールバックもしてくれる
		class ref_object class function_callback も参照のこと

		//	使用例
		smart_ptr<CSound> s;
		CSound* sound = new CSound;
		smart_ptr<function_callback>
			func(function_callback_v::Create(&test));
		s.Add(sound,new nonarray_callback_ref_object<CSound>(sound,func));
		↑このsmart_ptr<CSound>は、解体時（解体後）にtest関数が呼び出される
*/
public:
	nonarray_callback_ref_object(T* p,const smart_ptr<function_callback>& func)
		: m_p(p),m_func(func) {}
	virtual void	delete_instance() {
		delete m_p;
		m_func->run();
	}
private:
	T* m_p;	//	削除すべきアドレスを保持しておく
	smart_ptr<function_callback> m_func;
	//	↑汎用コールバックテンプレート
};

//	任意の配列をdeleteするためのテンプレート
template<class T>
class array_callback_ref_object : public ref_object {
/**
		任意の配列をdeleteするためのテンプレート
		解体時（解体後）にコールバックもしてくれる
		class ref_object class function_callback も参照のこと

*/
public:
	array_callback_ref_object(T* pa,const smart_ptr<function_callback>& func)
		: m_pa(pa),m_func(func) {}
	virtual void	delete_instance() {
		delete [] m_pa;
		m_func->run();
	}
private:
	T* m_pa;	//	削除すべきアドレスを保持しておく
	smart_ptr<function_callback> m_func;
	//	↑汎用コールバックテンプレート
};

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
