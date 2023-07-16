/*
	factory_permutation	: クラス置換用
		programmed & desinged by yaneurao(M.Isozaki) '02/03/07
*/

#ifndef __YTLfactory__
#define __YTLfactory__

namespace yaneuraoGameSDK3rd {
namespace YTL {

class factory_base {
/**
	factory の基底クラス
*/
public:
	virtual void* Create()=0;
	virtual ~factory_base(){}
};

template <class T>
class factory : public factory_base {
/**
	factory
*/
public:
	virtual T* CreateInstance() { return new T; }
	virtual void* Create() { return new T; }
};

template <class T,class S>
class factory2 : public factory<T> {
///	Tは基底クラス。Sはその派生クラス。
	virtual T* CreateInstance() { return new S; }
	virtual void* Create() { return new S; }
};

///	Tが抽象クラスのときのfactory
template <class T>
class factory_i : public factory_base {
/**
	factory
*/
public:
	virtual T* CreateInstance()=0;
	virtual void* Create()=0;
};

template <class T,class S>
class factory_i2 : public factory_i<T> {
///	Tは基底クラス(抽象)。Sはその派生クラス。
	virtual T* CreateInstance() { return new S; }
	virtual void* Create() { return new S; }
};

#endif

//////////////////////////////////////////////////////////////////

#ifndef __YTLfactory_permutation_h__
#define __YTLfactory_permutation_h__

#include "smart_ptr.h"

template <class T,class S>
class factory_permutation {
/**
	クラス置換を行なうためのテンプレート。

	ディフォルトで、Sクラスを生成する。
	これは、Tの派生クラスであること。

	SetFactoryでは、Tの派生クラスのポインタを渡す。
	CreateInstanceが呼び出されると、現在保持しているfactoryによって
	オブジェクトを生成し、そのsmart_ptrを返す。

	Tが抽象クラスのときは:
		class factory_permutation_i
	を用いること

	使用例）
	factory_permutation<IHoge,CHogeSakura> f;
	//	CHogeSakuraはIHoge派生クラス
	//	ディフォルトで、このオブジェクトが生成される
	smart_ptr<IHoge> p = f.CreateInstance();
	//	こうやれば、CHogeSakuraのオブジェクトが生成される
	f.SetFactory((CHogeTomoyo*)0);
	//	生成するオブジェクトをIHoge派生クラスであるCHogeTomoyoに
	//　変更してみる
	smart_ptr<IHoge> p = f.CreateInstance();
	//	こうやれば、CHogeTomoyoのオブジェクトが生成される
	smart_ptr<factory<IHoge> > h = f.GetFactory();
	//	ここでは、↑でfactoryを置換したためfactory<CHogeTomoyo>
	//	オブジェクトが生成される

*/
public:
	factory_permutation() :
	  m_vFactory(new factory2<T,S>){}

	///		factoryの設定／取得
	template <class U>
	void	SetFactory(const U*) {
	//	↑ここで指定しているclass Uは、Tの派生クラスと仮定。
		m_vFactory.Add(new factory2<T,U>);
	}
	smart_ptr<factory<T> > GetFactory() const
	{ return m_vFactory; }

	///		factoryによってインスタンスの生成
	smart_ptr<T> CreateInstance() {
		return smart_ptr<T>(m_vFactory->CreateInstance());
	}

protected:
	smart_ptr<factory<T> >	m_vFactory;
};

template <class T,class S>
class factory_permutation_i {
/**
	クラス置換を行なうためのテンプレート。
	class factory_permutation の基底クラスTが抽象クラスのときの
	バージョン
*/
public:
	factory_permutation_i() :
	  m_vFactory(new factory_i2<T,S>){}

	///		factoryの設定／取得
	template <class U>
	void	SetFactory(const U*) {
	//	↑ここで指定しているclass Uは、Tの派生クラスと仮定。
		m_vFactory.Add(new factory_i2<T,U>);
	}
	smart_ptr<factory_i<T> > GetFactory() const
	{ return m_vFactory; }

	///		factoryによってインスタンスの生成
	smart_ptr<T> CreateInstance() {
		return smart_ptr<T>(m_vFactory->CreateInstance());
	}

protected:
	smart_ptr<factory_i<T> >	m_vFactory;
};

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
