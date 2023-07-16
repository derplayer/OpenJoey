/*
	smart_list_ptr	: a list of smart_ptr
		programmed & desinged by yaneurao(M.Isozaki) '02/03/20
*/

#ifndef __YTLChain_h__
#define __YTLChain_h__

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.

#pragma warning(disable:4786)
//#include <list>
//using namespace std ;

namespace yaneuraoGameSDK3rd {
namespace YTL {

template<class T> class list_chain : public list<T*> {
/**
	ポインタのlistを管理するためのクラス。
	基本的にはlist<T*> だが、ループ抽象化等の能力を持つ。

	class smart_list_ptr も参照のこと
*/
public:
        typedef list_chain<T> base_type;
        typedef list<T*> super_type;
	/**
		set::insert,eraseに準拠
	*/
	void	insert(T* ptr){
		list<T*>::push_back(ptr);
	}
	bool	erase(const T* ptr){
		iterator it = begin();
		while (it!=end()){
			if (*it==ptr){
				list<T*>::erase(it);
				return true;	//	要素を削除したのでtrue
			}
			it++;
		}
		return false;
	}
	//	もとの関数を隠蔽してはいかん
	super_type::iterator erase(const super_type::iterator& it){
		return list<T* >::erase(it);
	}

	/**
		このチェインのすべてのメンバの関数を呼び出す（イテレータ抽象）
		例)
			GetList()->for_each(ISoundBase::Play,5);
			// これならば、全てのインスタンスに対して
			Play(5)を呼び出したことになる
	*/
	template <class R>
	void	for_each(R (T::*fn)()){
		iterator it = begin();
		while (it!=end()){ ((**it).*fn)(); it++; }
	}
	template <class R,class Arg1>
	void	for_each(R (T::*fn)(Arg1),const Arg1& a1){
		iterator it = begin();
		while (it!=end()){ ((**it).*fn)(a1); it++; }
	}
	template <class R,class Arg1,class Arg2>
	void	for_each(R (T::*fn)(Arg1,Arg2),const Arg1& a1,const Arg2& a2){
		iterator it = begin();
		while (it!=end()){ ((**it).*fn)(a1,a2); it++; }
	}
	template <class R,class Arg1,class Arg2,class Arg3>
	void	for_each(R (T::*fn)(Arg1,Arg2,Arg3),const Arg1& a1
		,const Arg2& a2,const Arg3& a3){
		iterator it = begin();
		while (it!=end()){ ((**it).*fn)(a1,a2,a3); it++; }
	}
	template <class R,class Arg1,class Arg2,class Arg3,class Arg4>
	void	for_each(R (T::*fn)(Arg1,Arg2,Arg3,Arg4),const Arg1& a1
		,const Arg2& a2,const Arg3& a3,const Arg4& a4){
		iterator it = begin();
		while (it!=end()){ ((**it).*fn)(a1,a2,a3,a4); it++; }
	}
	template <class R,class Arg1,class Arg2,class Arg3,class Arg4,
		class Arg5>
	void	for_each(R (T::*fn)(Arg1,Arg2,Arg3,Arg4,Arg5),const Arg1& a1,
		const Arg2& a2,const Arg3& a3,const Arg4& a4,const Arg5& a5){
		iterator it = begin();
		while (it!=end()){ ((**it).*fn)(a1,a2,a3,a4,a5); it++; }
	}
	template <class R,class Arg1,class Arg2,class Arg3,class Arg4,
		class Arg5,class Arg6>
	void	for_each(R (T::*fn)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6),const Arg1& a1,
		const Arg2& a2,const Arg3& a3,const Arg4& a4,const Arg5& a5,
		const Arg6& a6){
		iterator it = begin();
		while (it!=end()){ ((**it).*fn)(a1,a2,a3,a4,a5,a6); it++; }
	}

	/**
		bool isValid() constというメンバを持つクラスに対して、
		これがtrueが返ってくるインスタンスに対してのみ、
		指定された関数を呼び出し、
		非Validな奴は消していくイテレータ抽象
	*/
	template <class R>
	void	for_each_valid(R (T::*fn)()){
		iterator it = begin();
		while (it!=end()){
			if ((*it)->isValid()){
				((**it).*fn)(); it++;
			} else {
				it = erase(it);
			}
		}
	}
	template <class R,class Arg1>
	void	for_each_valid(R (T::*fn)(Arg1),const Arg1& a1){
		iterator it = begin();
		while (it!=end()){
			if ((*it)->isValid()){
				((**it).*fn)(a1); it++;
			} else {
				it = erase(it);
			}
		}
	}
	template <class R,class Arg1,class Arg2>
	void	for_each_valid(R (T::*fn)(Arg1,Arg2),const Arg1& a1,const Arg2& a2){
		iterator it = begin();
		while (it!=end()){
			if ((*it)->isValid()){
				((**it).*fn)(a1,a2); it++;
			} else {
				it = erase(it);
			}
		}
	}
	template <class R,class Arg1,class Arg2,class Arg3>
	void	for_each_valid(R (T::*fn)(Arg1,Arg2,Arg3),const Arg1& a1
		,const Arg2& a2,const Arg3& a3){
		iterator it = begin();
		while (it!=end()){
			if ((*it)->isValid()){
				((**it).*fn)(a1,a2,a3); it++;
			} else {
				it = erase(it);
			}
		}
	}
	template <class R,class Arg1,class Arg2,class Arg3,class Arg4>
	void	for_each_valid(R (T::*fn)(Arg1,Arg2,Arg3,Arg4),const Arg1& a1
		,const Arg2& a2,const Arg3& a3,const Arg4& a4){
		iterator it = begin();
		while (it!=end()){
			if ((*it)->isValid()){
				((**it).*fn)(a1,a2,a3,a4); it++;
			} else {
				it = erase(it);
			}
		}
	}
	template <class R,class Arg1,class Arg2,class Arg3,class Arg4,
		class Arg5>
	void	for_each_valid(R (T::*fn)(Arg1,Arg2,Arg3,Arg4,Arg5),
		const Arg1& a1,const Arg2& a2,const Arg3& a3,
		const Arg4& a4,const Arg5& a5){
		iterator it = begin();
		while (it!=end()){
			if ((*it)->isValid()){
				((**it).*fn)(a1,a2,a3,a4,a5); it++;
			} else {
				it = erase(it);
			}
		}
	}
	template <class R,class Arg1,class Arg2,class Arg3,class Arg4,
		class Arg5,class Arg6>
	void	for_each_valid(R (T::*fn)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6),
		const Arg1& a1,const Arg2& a2,const Arg3& a3,
		const Arg4& a4,const Arg5& a5,const Arg6& a6){
		iterator it = begin();
		while (it!=end()){
			if ((*it)->isValid()){
				((**it).*fn)(a1,a2,a3,a4,a5,a6); it++;
			} else {
				it = erase(it);
			}
		}
	}

	/**
		すべての要素に対して、functorを適用する

	例：
		#define Lambda(arg,body,name) \
		class name { public: void operator() arg { body;} };

		Lambda((vector<int>::iterator it),(*it)->Play(),foo);

		list.for_each(foo);
		//	すべてのインスタンスに対して
		//	10が代入される

	*/
	template <class Fn>
	void	for_each_func(Fn fn){
		iterator it = begin();
		while (it!=end()){ fn(it); it++; }
	}

};

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
