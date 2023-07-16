/*
	ref_deleter	: 参照カウンタ付き解体子
		programmed & desinged by yaneurao(M.Isozaki) '02/03/03
*/

#ifndef __YTLRefDeleter_h__
#define __YTLRefDeleter_h__

namespace yaneuraoGameSDK3rd {
namespace YTL {

class ref_object {
/**
	参照カウント管理をやりつつ、
	任意の要素をdeleteするための基底オブジェクト

	独自の解体処理がしたいならば、
	たとえばref_object派生クラスを以下のように用意する。

class my_ref_object : public ref_object {
public:
	my_ref_object(CSound* p,const smart_ptr<function_callback>&func)
		: m_p(p),m_func(func) {}
	virtual void	delete_instance() {
		delete m_p;
		m_func->run();
	}
private:
	CSound* m_p;
	smart_ptr<function_callback_v> m_func;
	//	↑汎用コールバックテンプレート
};

*/
public:
	virtual void	delete_instance()=0;
	///	保持しているオブジェクトをdeleteする。派生クラスで定義する

	//	参照カウンタの増減
	void	inc_ref(){ m_nRef++; }
	bool	dec_ref(){ m_nRef--;
		//	参照カウンタが0になったときにオブジェクトの所有権があれば
		//	自動的にdeleteする。そのときにこの関数の返し値がtrueになる
		if (m_nRef == 0) {
			if (m_bOwner) delete_instance();
			return true;
		}
		return false;
	}
	///	参照カウンタの設定／取得
	void	set_ref(int n) { m_nRef = n; }
	int		get_ref() const { return m_nRef; }

	///	オーナー権の設定／取得
	void	set_owner(bool b) { m_bOwner = b; }
	bool	get_owner() const { return m_bOwner; }

	///	オブジェクトサイズの設定／取得
	void	set_obj_size(int n) { m_nObjSize = n; }
	int		get_obj_size() const { return m_nObjSize; }

	///	配列オブジェクトの最大数の設定／取得
	void	set_size(int n) { m_nMaxSize = n;}
	int		get_size() const { return m_nMaxSize; }

	virtual ~ref_object() {}	//	place holder

protected:
	//	かならず以下の変数をアクセッサを通じて初期化して使うこと！

	int		m_nRef;		//	参照カウント
	bool	m_bOwner;	//	オーナー権
	int		m_nObjSize;	//	オブジェクトひとつのサイズ
	int		m_nMaxSize;	//	配列オブジェクトの最大
};

//	非配列オブジェクトをdeleteするためのテンプレート
template<class T>
class nonarray_ref_object : public ref_object {
public:
	nonarray_ref_object(T* p) : m_p(p) {}
	virtual void	delete_instance() {
		delete m_p;
	}
private:
	T* m_p;	//	削除すべきアドレスを保持しておく
};

//	任意の配列をdeleteするためのテンプレート
template<class T>
class array_ref_object : public ref_object {
public:
	array_ref_object(T* pa) : m_pa(pa) {}
	virtual void	delete_instance() {
		delete [] m_pa;
	}
private:
	T* m_pa;	//	削除すべきアドレスを保持しておく
};

/*	//	こんなものを作っても仕方ないのか？
//	deallocatorをfunctorで指定するためのテンプレート
template<class T>
class deallocator_ref_object : public ref_object {
public:
	deallocator_ref_object(T t_) : t(t_) {}
	virtual void	delete_instance() {
		t();
	}
private:
	T	t;
};
*/

/*	//	どうせowner権が無ければ何もしないのでこのクラスは不要
//	何もしないref_object
class null_ref_object : public ref_object{
public:
	virtual void	delete_instance() {}
	//	何もしない
};
*/

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
