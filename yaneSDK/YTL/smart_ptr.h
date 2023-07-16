/*
	smart_ptr	: an intelligent pointer
		reprogrammed & desinged by yaneurao(M.Isozaki) '02/02/28
*/

#ifndef __YTLSmartPtr_h__
#define __YTLSmartPtr_h__

#include "../Thread/yaneCriticalSection.h"
#include "ref_deleter.h"
#include "exceptions.h"

namespace yaneuraoGameSDK3rd {
namespace YTL {

//	smart_ptr_baseは削除
//	smart_ptr<void> => smart_obj
//	shared_ptr<void> => shared_obj
//	とtypedefしておくので、以降はそっちを使ってチョ

namespace smart_ptr_detail
{
	template<class T> struct smart_ptr_traits {
		typedef T& reference_type;
	};
	template<> struct smart_ptr_traits<void> {
		typedef void reference_type;
	};
}

template<
	class T,
	class ThreadingModel = ThreadingModels::SingleThread
>
class smart_ptr : /*public smart_ptr_base,*/ public ThreadingModel
{
/**
	a real smart pointer
	スマートポインタクラス（非配列／配列統合版）

	（参照カウントを持っていて、どこからも参照されなくなったときに
	自動的にオブジェクトを解体するような賢いポインタ）

	使い方：
			smart_ptr<CHoge> s1(new CHoge);

		というようにすれば、あとは、

			smart_ptr<CHoge> s2;
			s2 = s1;

		というようなコピー等をやっても、最後に残ったスマートポインタさんが
		責任を持ってオブジェクトを解体してくれる！

		☆　配列オブジェクトも可能：

			smart_ptr<CHoge> s1;
			s1.AddArray(10);
			//	↑CHoge* s1 = new CHoge[10]; の意味に近い

			//	CHogeDerivedはCHoge派生クラス
			s1.AddArray(new CHogeDerived[10],10);
			も可。

		☆　配列ポインタの正しいアップキャスト等も可能。

		また、コンストラクタはexplicitで定義されているので、
			void function(smart_array<CHoge> av);
		というような関数の呼び出しで、普通のポインタを渡したいときは、
		所有権無しのスマートポインタを生成して渡すこと！

		つまり、
			function(smart_array<CHoge>(pHoge,false));
		とやって、コンストラクタの第２パラメータをfalseにして呼び出す。

		☆　ThreadingModel::LockPolicyポリシークラスを変えることによって排他制御の有無を選択可能

		排他制御が必要ないとき：
			smart_ptr<CHoge> s1;
		排他制御が必要であるとき：
			smart_ptr<CHoge, ThreadingModels::MultiThread> s1;


		備考。このクラスは、
			gcc version 2.95.3-6 (mingw)
			[command line] g++ -pedantic -Wall -ansi
		でコンパイル通るはず？（今はわからない）
*/
public:
	typedef smart_ptr<T, ThreadingModel> self_type;
	typedef typename smart_ptr_detail::smart_ptr_traits<T>::reference_type reference_type;

	///	他のポインタ（非配列オブジェクト）を取るコンストラクタ
	template <class S>
	explicit smart_ptr(S* _P, bool bOwner=true) :
	m_lpObj(NULL) , m_nIndex(0) , m_lpRefObj(NULL)
	/**
		_Pは、オブジェクトを渡します。
		bOwnerは、この配列スマートポインタに発令オブジェクトを
			自動削除して欲しいときにtrueにします。defaultではtrue。
	*/
	{
		if (_P!=NULL) { Init(_P,bOwner,1); }
			//	Sが仮想クラスであることは無い．．はず
	}

	///	ディフォルトコンストラクタ
	smart_ptr() :	m_lpObj(NULL) , m_nIndex(0) , m_lpRefObj(NULL) {}

	//	コピーコンストラクタ
	smart_ptr(const self_type& s){
		inc_ref(s);
	}

	smart_ptr& operator = (const self_type& s){
		if (this != &s) { dec_ref(); inc_ref(s); }
		return *this;
	}

	~smart_ptr() {
		dec_ref();
	}

	///	ポインタのふりをするための仕掛け
	reference_type operator*() const  {return *get(); }
	T* operator->() const {return get(); }

	/// 要素へのアクセス
	reference_type operator [] (const int n) { return *get(n); }

	/**
		ポリモーフィックな配列オブジェクトを取得する
		範囲外への要素へのアクセス時には、
		CIndexOutOfBoundsException例外が送出される
		NULLポインタに対するアクセスはCNullPointerException例外
		スマートポインタがNULLかどうかはisNullでチェックするので
		こういう仕様で良い
	*/
	T* get(const int n=0) const
#ifdef USE_EXCEPTION
		throw(CIndexOutOfBoundsException,CNullPointerException)
#endif
	{
		ThreadingModel::Lock guard(*this);
#ifdef USE_EXCEPTION
		//	このポインタがNullのときはNullポインタの取得（コピーも禁止されることになるが、それで良い）
		if ( isNull() ) { throw CNullPointerException(); }
		else if ( isEnd() ) { throw CIndexOutOfBoundsException(); }
#endif

		return getPointer(n);
	}

	///	get()のアクセス違反なポインタでも例外が発生しないバージョン
	T* getPointer(const int n=0) const
	{
		ThreadingModel::Lock guard(*this);
		if ( isNull() ) return NULL;
		return (T*)((BYTE*)m_lpObj + (n+m_nIndex)*get_obj_size());
	}

	/**
		以下のメソッドで取得できるsmart_ptrは、
		参照カウント等も共有しているので、こいつが解放されない限り
		配列オブジェクトも解放されない
	*/
	///	配列オブジェクトのある要素へのスマートポインタを取得する
	self_type get_smart_ptr(const int n=0) const {
		ThreadingModel::Lock guard(*this);
		return self_type(*this)+=n;	//	このindexを加算
	}
	///	配列オブジェクトの先頭要素へのスマートポインタを取得する
	self_type begin() const {
		return get_smart_ptr(0-m_nIndex);
	}
	///	配列の最後の要素の次の要素へのスマートポインタを取得する
	self_type end() const {
		return get_smart_ptr(size()-m_nIndex);
	}
	///	このポインタが配列の範囲外を指しているかをチェックする
	bool	isEnd() const {
		return (((ULONG)m_nIndex) >= (ULONG)size());
	}

	void	Add(){
	///	所有権を持ったインスタンスの追加生成構文
		dec_ref();				//	前のやつ解放
		Init(new T,true,1);		//	遅延生成
	}
	template <class S>
	void	Add(S*_P){				//	ポリモーフィックな型でも可
		dec_ref();				//	前のやつ解放
		if (_P!=NULL) {
			Init(_P,true,1);
		}
	}
	//	↑この２つの関数を１つにまとめると
	//	new Tの部分で、T が抽象クラスだとコンパイルエラーになる。

	template <class S>
	void	Add(S*_P,ref_object*ref){
	/**
		解体手段を規定したポインタを渡す
		（その後、このスマートポインタは所有権を持つ）

		例)	smart_ptr<CHoge> pHoge;
			pHoge.Add(new CHoge,new my_ref_object);
	*/
		dec_ref();
		if (_P!=NULL){
			Init(_P,true,1,false,ref);
		}
	}

	///	所有権の無いポインタの設定
	template <class S>
	void	Set(S*_P){			//	ポリモーフィックな型でも可
		dec_ref();				//	前のやつ解放
		if (_P!=NULL) {
			Init(_P,false,1);
		}
	}

	///	所有権を持たない配列インスタンスの追加構文
	template <class S>
	void	SetArray(S* p, const int n) {
		dec_ref();
		Init(p,false,n,true);
	}

	///	所有権を持った配列インスタンスの追加生成構文
	void	AddArray(const int n){
		dec_ref();
		if (n<=0) return ; // なんでやねん．．
		Init(new T[n],true,n,true);
	}

	template <class S>
	void	AddArray(S*_P, const int n, ref_object*ref=NULL){
	/**
		解体手段を規定した配列ポインタを渡す
		（その後、このスマートポインタは所有権を持つ）

		例)	smart_ptr<CHoge> pHoge;
			pHoge.Add(new CHoge[10],new my_ref_object);
	*/
		dec_ref();
		if (n<=0) return ; // なんでやねん．．
		Init(_P,true,n,true,ref);
	}

	template <class RelateType>
	self_type&	UpCast(const smart_ptr<RelateType,ThreadingModel>& s){
	///	明示的アップキャスト構文　→　cf.smart_ptr_static_cast
	/**	ただし、同じ実体であるsmart_ptr間でアップキャストしてはいけない
		すなわち、
			smart_ptr<CHoge> p;
			p.UpCast(p); // これはやってはいけない
	*/
		ThreadingModel::Lock guard(*this);
		if ((void*)this != (void*)&s) {
		//	↑型が違うかも知れないのでこの程度のチェックしか出来ない
			dec_ref();

			//	以下はinc_ref(s)と等価
			m_lpObj		= static_cast<T*>(s.getObj());
			//	これはキャストを含むのでアップキャストできなければ
			//	コンパイルエラーになる
			m_nIndex	= *(const_cast<smart_ptr<RelateType,ThreadingModel>*>(&s))->getIndex();
			m_lpRefObj	= s.getRefObj();
			if (m_lpRefObj!=NULL) m_lpRefObj->inc_ref();
		}
		return *this;
	}

	template <class RelateType>
	self_type&	DownCast(const smart_ptr<RelateType,ThreadingModel>& s){
	///	明示的アップキャスト構文　→　cf.smart_ptr_dynamic_cast
	/**	ただし、同じ実体であるsmart_ptr間でダウンキャストしてはいけない
		すなわち、
			smart_ptr<CHoge> p;
			p.DownCast(p); // これはやってはいけない
	*/
		ThreadingModel::Lock guard(*this);
		if ((void*)this != (void*)&s) {
		//	↑型が違うかも知れないのでこの程度のチェックしか出来ない
			dec_ref();

			//	以下はinc_ref(s)と等価
			m_lpObj		= dynamic_cast<T*>(s.getObj());
			//	これはキャストを含むのでダウンキャストできなければ
			//	コンパイルエラーになる
			m_nIndex	=
				*(const_cast<smart_ptr<RelateType,ThreadingModel>*>(&s))->getIndex();
			m_lpRefObj	= s.getRefObj();
			if (m_lpRefObj!=NULL) m_lpRefObj->inc_ref();
		}
		return *this;
	}

	bool	unique() const {
	///	オブジェクトを参照しているのが唯一か？
		return count()==1;
	}
	int		count()	const {
	///	オブジェクトの参照数を返す
		ThreadingModel::Lock guard(*this);
		return m_lpRefObj!=NULL?m_lpRefObj->get_ref():0;
	}

	bool	isNull() const {
	///	指しているオブジェクトはNULLか？
		return m_lpObj==NULL;
	}

	int		size() const {
	///	配列オブジェクトならば、最大数を返す
	///	非配列オブジェクトならば、1が返る
		if (m_lpRefObj==NULL) return 0;
		ThreadingModel::Lock guard(*this);
		return m_lpRefObj->get_size();
	}

	void	Delete() {	dec_ref(); }
	///	このオブジェクトを解体する（ただし、他のスマートポインタから参照
	///	されていれば、実際にdeleteまでは出来ない）

	///	スマートポインタの加減算
	void dec() { 
		ThreadingModel::Lock guard(*this);
		m_nIndex--; }
	void inc() { 
		ThreadingModel::Lock guard(*this);
		m_nIndex++; }

	///	加減算のためのオペレータ
	self_type& operator++() {
		inc(); return (*this); }
	self_type operator++(int){
		ThreadingModel::Lock guard(*this);
		self_type _Tmp = *this; inc(); return (_Tmp); }
	self_type& operator--() {
		dec(); return (*this); }
	self_type operator--(int)
		{	ThreadingModel::Lock guard(*this);
		self_type _Tmp = *this; dec(); return (_Tmp); }
	self_type& operator+=(int _N)
		{	ThreadingModel::Lock guard(*this);
		m_nIndex += _N; return (*this); }
	self_type& operator-=(int _N)
		{	ThreadingModel::Lock guard(*this);
		m_nIndex -= _N; return (*this); }
	self_type operator+(int _N) const
		{ self_type _Tmp = *this; return (_Tmp += _N); }
	self_type operator-(int _N) const
		{ self_type _Tmp = *this; return (_Tmp -= _N); }

	///	比較のためのオペレータ
	bool operator==(const self_type& _X) const
		{ThreadingModel::Lock guard(*this);
		return (getPointer() == _X.getPointer()); }
	bool operator!=(const self_type& _X) const
		{return (!(*this == _X)); }
	bool operator<(const self_type& _X) const
		{ThreadingModel::Lock guard(*this);
		return (getPointer() < _X.getPointer()); }
	bool operator>(const self_type& _X) const
		{return (_X < *this); }
	bool operator<=(const self_type& _X) const
		{return (!(_X < *this)); }
	bool operator>=(const self_type& _X) const
		{return (!(*this < _X)); }

	///	解体子オブジェクトへのポインタを返す（内部実装用）
	ref_object* getRefObj() const { return m_lpRefObj; }
	///	オブジェクトの実装（内部実装用）
	T* getObj() const { return m_lpObj; }
	///	indexの取得(iterator的に使用しているとき)／内部実装用
	int* getIndex() { return &m_nIndex; }

protected:
	int		get_obj_size() const {
	//	オブジェクトひとつの要素のサイズを返す（内部実装用）
		ThreadingModel::Lock guard(*this);
		return m_lpRefObj!=NULL?m_lpRefObj->get_obj_size():0;
	}

	void	set_size(int nSize) {
	///	size関数で取得できるサイズを変更する
		ThreadingModel::Lock guard(*this);
		if (m_lpRefObj==NULL) return ;
		m_lpRefObj->set_size(nSize);
	}

	//	初期化補助関数
	template <class S>
	void	Init(S *s,bool bOwner,int nSize,bool bArray=false,ref_object*ref=NULL){
		ThreadingModel::Lock guard(*this);
		//	引数は、
		//	１．オブジェクトのクラス　２．このオブジェクトの所有権
		//	３．このオブジェクトの数　４．配列オブジェクトかどうか
		if (m_lpRefObj!=NULL) delete m_lpRefObj;
		m_nIndex = 0;
		m_lpObj = s;
		//	↑ここで、コンパイルエラーが出るのは、
		//	smart_ptrのコンストラクタの引数に指定している型が
		//	このスマートポインタで指定している型の派生クラス等で無いのが原因

		if (nSize==0) return ; // なんで？？
#ifdef USE_EXCEPTION
		if (!bOwner && ref!=NULL){
			//	Ownerで無いのに解体オブジェクトを渡しているとはどういうこっちゃ？
			throw CSyntaxException("所有権が無いのに解体オブジェクトが指定されている");
		}
#endif
		/*	//	Owner権が無ければ、どうせオブジェクトは削除されない
			//	(⇒ref_deleter::dec_ref)
		if (!bOwner) {
			m_lpRefObj = new null_ref_object;
			//	所有権が無いので解体も何もしないオブジェクト
		} else
		*/	
		if (ref!=NULL) {
			m_lpRefObj = ref;
		} else if (!bArray) {
			m_lpRefObj = new nonarray_ref_object<S>(s);
		} else {
			m_lpRefObj = new array_ref_object<S>(s);
		}
		m_lpRefObj->set_ref(1);
		m_lpRefObj->set_owner(bOwner);
		//	オーナーで無いのであれば決して解体されないようにしておく
		m_lpRefObj->set_obj_size(sizeof(S));
		m_lpRefObj->set_size(nSize);
	}

	//	参照カウントの増減（内部利用のみ）
	void	inc_ref(const self_type& s) {
		ThreadingModel::Lock guard(*this);
		m_lpObj		= s.m_lpObj;
		m_nIndex	= s.m_nIndex;
		m_lpRefObj	= s.m_lpRefObj;
		if (m_lpRefObj!=NULL) m_lpRefObj->inc_ref();
	}
	void	dec_ref() {
		ThreadingModel::Lock guard(*this);
		if (m_lpRefObj!=NULL){
			if (m_lpRefObj->dec_ref()){
				//	オブジェクトは解体子経由で削除する
				delete m_lpRefObj;
			}
			m_lpRefObj = NULL;
			m_lpObj = NULL;
		}
	}

private:
	T*		m_lpObj;			//	実オブジェクト
	int		m_nIndex;			//	このsmart_ptrが、このオブジェクトの
								//	何番目の要素を指しているか
	ref_object* m_lpRefObj;		//	解体用オブジェクト
};

template<typename T>
class shared_ptr : public smart_ptr<T, ThreadingModels::MultiThread>
{
public:
	typedef	shared_ptr<T> self_type;
	typedef	smart_ptr<T, ThreadingModels::MultiThread> base_type;

	template <class S>
	explicit shared_ptr(S* _P, bool bOwner=true) : base_type(_P,bOwner) {};
	shared_ptr() : base_type() {}
	shared_ptr(const base_type& s) : base_type(s) {}
};

typedef	smart_ptr<void> smart_obj;
typedef	shared_ptr<void> shared_obj;

//////////////////////////////////////////////////////////////////////////
//	cast用演算子(smart_ptr,shared_ptr両方に対して使える)

template <typename T, typename U, class ThreadingModel>
smart_ptr<T, ThreadingModel> smart_ptr_static_cast(const smart_ptr<U, ThreadingModel>& r)
/**
	smart_ptr<U>の持つ参照カウンタと、ここで返されるsmart_ptr<T>の
	参照カウンタとは共有される。(すなわちsmart_ptr<U>の持つ所有権は
	剥奪されない)

	使用例)
		smart_ptr<RealResult> result;
		return smart_ptr_static_cast<IResult>(result);
*/
{	return smart_ptr<T, ThreadingModel>().UpCast(r);	}

template <typename T, typename U, class ThreadingModel>
smart_ptr<T, ThreadingModel> smart_ptr_dynamic_cast(const smart_ptr<U, ThreadingModel>& r)
/**
	smart_ptr<U>の持つ参照カウンタと、ここで返されるsmart_ptr<T>の
	参照カウンタとは共有される。(すなわちsmart_ptr<U>の持つ所有権は
	剥奪されない)

	使用例)
		smart_ptr<RealResult> result;
		return smart_ptr_static_cast<IResult>(result);
*/
{	return smart_ptr<T, ThreadingModel>().DownCast(r);	}

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
