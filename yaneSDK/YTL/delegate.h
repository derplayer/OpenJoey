/**
  * Delegate Template Class
  * programmed by ENRA(S.Takebata) '02/10/06
  * reprogrammed by ENRA(S.Takebata) '03/07/04 Change delegation architecture
  * reprogrammed by ENRA(S.Takebata) '03/07/11 operator= etc.
  * reprogrammed by ENRA(S.Takebata) '03/07/12 Compatible with functor set (no compatibility check)
  * reprogrammed by ENRA(S.Takebata) '03/07/12 Changed not to check if functor has operator()
  *
  * Note: Not thread-safe. Please use the lock object accordingly.
  */

#ifndef __YTL_DELEGATE_H__
#define __YTL_DELEGATE_H__

#ifdef yaneSDK_USE_delegate

#include "../config/yaneConfig.h"
#include "smart_ptr.h"
#include "delegate/type_test.hpp"
#include "delegate/is_compatible_function.hpp"
#include "delegate/functor.hpp"

//	If it is not a const reference, the overloaded function will fail to resolve (;´Д`)
#if defined(_MSC_VER) && (_MSC_VER<=1300) || defined(__MWERKS__) && (__MWERKS__<0x2406)
	#define DELEGATE_TARGET_FIX(Type) const Type &
#else
	#define DELEGATE_TARGET_FIX(Type) Type
#endif

namespace yaneuraoGameSDK3rd {
namespace YTL {
namespace YTLdelegate {
//	static assertion
template<bool expression> struct static_assert;
template<> struct static_assert<true> {};

//	tag-dispatch
struct tag_unknown {};
struct tag_function_ptr {
	template<typename T> struct types {
		typedef T function_type;
	};
	enum { compatibility_check = true };
};
struct tag_pair {
	template<typename T> struct types {
		typedef typename T::first_type	class_pointee_type;
		typedef typename T::second_type	function_type;
	};
	enum { compatibility_check = true };
};
struct tag_self {
	template<typename T> struct types {
		typedef typename T::operator_type function_type;
	};
	enum { compatibility_check = true };
};
struct tag_functor {
	template<typename T> struct types {
		typedef T* function_type;	//	意味はない
	};
	enum { compatibility_check = false };
};
struct tag_functor_smart_ptr {
	template<typename T> struct types {
		typedef T* function_type;	//	意味はない
	};
	enum { compatibility_check = false };
};


//	delegate
template<
	typename R,
	typename A1	 = void_t,
	typename A2	 = void_t,
	typename A3	 = void_t,
	typename A4	 = void_t,
	typename A5	 = void_t,
	typename A6	 = void_t,
	typename A7	 = void_t,
	typename A8	 = void_t,
	typename A9	 = void_t,
	typename A10 = void_t
>
struct delegate : public get_functor_type<R, args_type<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> >::type {
/**
	Template to realize Delegate
	When setting a function, whether the operator () of this delegate can call that function (compatibility)
	Check at compile time. If an error occurs in the VC++ system, the location where the setting was performed is reported.

	例)
		class CHoge {
			void		TestFunction1();
			int			TestFunction2();
			int			TestFunction3(const string&) const;
			static void	TestFunction4();
		} hoge;
		void	TestFunction5();

		//	Class Method Delegate
		delegate<void>
		OnTestFunc1(&hoge, &CHoge::TestFunction1);	//	VC6 requires & to specify functions
		delegate<void>
		OnTestFunc2(&hoge, &CHoge::TestFunction2);	//	No error if callable
		delegate<int, const char*>
		OnTestFunc3(&hoge, &CHoge::TestFunction3);	//	const methodもset出来ます
		OnTestFunc1();								//	CHoge::TestFunction1が呼ばれる
		OnTestFunc2();								//	CHoge::TestFunction2が呼ばれ、戻り値は無視される
		int ret = OnTestFunc3("ほげほげ");			//	const char*から変換されたstringを引数として
													//	CHoge::TestFunction2が呼ばれ、戻り値も得られる
		//	Static Method Delegate
		delegate<void> OnTestFunc4(&CHoge::TestFunction4);
		delegate<void> OnTestFunc5(&TestFunction5);
		OnTestFunc3();								//	CHoge::TestFunction3が呼ばれる
		OnTestFunc4();								//	TestFunction4が呼ばれる

		//	何もセットせずにoperator()を呼ぶ
		delegate<void> OnTestFunc5;
		OnTestFunc5();									//	何も起こらない
*/

	typedef typename get_functor_type<R, args_type<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> >::type base_type;
	typedef delegate self_type;

	delegate() : base_type() {};

	/**	コンストラクタによるセット
	 *	ex) delegate<void> f1(&method_name);
	 *		delegate<void> f2(pointee_to_object, &class_name::method_name);
	 *		delegate<void> f3(functor_object);
	 *		delegate<void> f4(f1);
	 *		delegate<void> f5(0);	//	clear()を呼ぶことと等価
	 */
	template<typename F>
	delegate(DELEGATE_TARGET_FIX(F) f) : base_type()
	{
		//	F型が何の型なのかを分析する
		enum {	//	BCCでは一旦enumに代入しないと即値と判断されない場合がある
			b1 = is_same<F,self_type>::value,
			b2 = is_function_ptr<F>::value,
			b3 = false,				// TODO: is_smart_pointer<F>
			b4 = (b1|b2|b3)==false,	// TODO: is_class<F>
		};
		typedef typename select_type<b1, tag_self, tag_unknown>::type temp_type1;
		typedef typename select_type<b2, tag_function_ptr, temp_type1>::type temp_type2;
		typedef typename select_type<b3, tag_functor_smart_ptr, temp_type2>::type temp_type3;
		typedef typename select_type<b4, tag_functor, temp_type3>::type tag_type;
		//	tag_unknownが選択された場合、ここでコンパイルエラーになる
		typedef typename tag_type::template types<F>::function_type function_type;
		enum { must_be_true = (tag_type::compatibility_check==false) || is_compatible_function<base_type::operator_type, function_type>::value };
		static_assert<must_be_true>();	//	互換性のない関数だとコンパイルエラーになる

		set_method(f, tag_type());
	}
	template<typename TPtr, typename F>
	delegate(const TPtr& p, F f) {
		enum { must_be_true = is_member_function_ptr<F>::value && is_compatible_function<base_type::operator_type, F>::value };
		//	Fがメンバ関数ポインタでないか、互換性のないメンバ関数だとコンパイルエラーになる
		static_assert<must_be_true>();

		set_class_method(p,f);
	};
	delegate(const self_type& f) : base_type(static_cast<const base_type&>(f)) {};
	delegate(const base_type& f) : base_type(f) {};
	delegate(int must_be_zero) : base_type() {};

	/**	代入演算子によるセット
	 *	ex) delegate<void> f1, f2;
	 *		f1 = &method_name;
	 *		f1 = std::make_pair(pointee_to_object, &class_name::method_name);
	 *		f1 = functor_object;
	 *		f1 = f2;
	 *		f1 = 0;		//	clear()を呼ぶことと等価
	 */
	template<typename F>
	self_type& operator=(DELEGATE_TARGET_FIX(F) f) {
		//	F型が何の型なのかを分析する
		enum {	//	BCCでは一旦enumに代入しないと即値と判断されない場合がある
			b0 = is_pair<F>::value,
			b1 = is_same<F,self_type>::value,
			b2 = is_function_ptr<F>::value,
			b3 = false,					// TODO: is_smart_pointer<F>
			b4 = (b0|b1|b2|b3)==false,	// TODO: is_class<F>
		};
		typedef typename select_type<b0, tag_pair, tag_unknown>::type temp_type0;
		typedef typename select_type<b1, tag_self, temp_type0>::type temp_type1;
		typedef typename select_type<b2, tag_function_ptr, temp_type1>::type temp_type2;
		typedef typename select_type<b3, tag_functor_smart_ptr, temp_type2>::type temp_type3;
		typedef typename select_type<b4, tag_functor, temp_type3>::type tag_type;
		//	tag_unknownが選択された場合、ここでコンパイルエラーになる
		typedef typename tag_type::template types<F>::function_type function_type;
		enum { must_be_true = (tag_type::compatibility_check==false) || is_compatible_function<base_type::operator_type, function_type>::value };
		static_assert<must_be_true>();	//	互換性のない関数だとコンパイルエラーになる

		set_method(f, tag_type());
		return *this;
	};
	self_type& operator=(const self_type& f)	//	BCCでは↑が呼ばれてしまう
	{
		base_type::set(static_cast<const base_type&>(f));
		return *this;
	}
	self_type& operator=(const base_type& f)
	{
		base_type::set(f);
		return *this;
	}
	self_type& operator=(int must_be_zero)
	{
		clear();
		return *this;
	}

	/**	set関数によるセット
	 *	ex) delegate<void> f1, f2;
	 *		f1.set(&method_name);
	 *		f1.set(pointee_to_object, &class_name::method_name);
	 *		f1.set(f2);
	 */
	template<typename F>
	void set(DELEGATE_TARGET_FIX(F) f) {
		//	F型が何の型なのかを分析する
		enum {	//	BCCでは一旦enumに代入しないと即値と判断されない場合がある
			b1 = is_same<F,self_type>::value,
			b2 = is_function_ptr<F>::value,
			b3 = false,				// TODO: is_smart_pointer<F>
			b4 = (b1|b2|b3)==false,	// TODO: is_class<F>
		};
		typedef typename select_type<b1, tag_self, tag_unknown>::type temp_type1;
		typedef typename select_type<b2, tag_function_ptr, temp_type1>::type temp_type2;
		typedef typename select_type<b3, tag_functor_smart_ptr, temp_type2>::type temp_type3;
		typedef typename select_type<b4, tag_functor, temp_type3>::type tag_type;
		//	tag_unknownが選択された場合、ここでコンパイルエラーになる
		typedef typename tag_type::template types<F>::function_type function_type;
		enum { must_be_true = (tag_type::compatibility_check==false) || is_compatible_function<base_type::operator_type, function_type>::value };
		static_assert<must_be_true>();	//	互換性のない関数だとコンパイルエラーになる

		set_method(f, tag_type());
	}
	template<typename TPtr, typename F>
	void set(const TPtr& p, F f) {
		enum { must_be_true = is_member_function_ptr<F>::value && is_compatible_function<base_type::operator_type, F>::value };
		//	Fがメンバ関数ポインタでないか、互換性のないメンバ関数だとコンパイルエラーになる
		static_assert<must_be_true>();

		set_class_method(p,f);
	}

	///	セットした関数を取り外す
	void clear() {
		base_type::clear();
	}

	///	関数がセットされていないならtrue
	bool isNull() const {
		return base_type::isNull();
	}

	///	operator!
	bool operator!() const {
		return !isNull();
	}

private:
	template<typename F>
	void set_method(F f, tag_function_ptr) {
		typedef base_type::method_invoker<F> invoker_type;
		base_type::set(smart_ptr<invoker_type>(new invoker_type(f)));
	}

	template<typename F>
	void set_method(const F& f, tag_functor) {	
		typedef base_type::functor_invoker<F> invoker_type;
		base_type::set(smart_ptr<invoker_type>(new invoker_type(f)));
	}

	template<typename TPtr>
	void set_method(const TPtr& p, tag_functor_smart_ptr) {
		typedef base_type::method_invoker<TPtr> invoker_type;
		base_type::set(smart_ptr<invoker_type>(new invoker_type(p)));
	}

	template<typename TPtr, typename F>
	void set_method(const pair<TPtr,F>& v, tag_pair) {
		set_class_method(v.first, v.second);
	}

	void set_method(const self_type& f, tag_self) {
		base_type::set(static_cast<const base_type&>(f));
	}

	template<typename TPtr, typename F>
	void set_class_method(const TPtr& p, F f) {
		typedef base_type::class_method_invoker<TPtr,F> invoker_type;
		base_type::set(smart_ptr<invoker_type>(new invoker_type(p,f)));
	}
};

} // end of namespace YTLdelegate
} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

using yaneuraoGameSDK3rd::YTL::YTLdelegate::delegate;

#undef DELEGATE_TARGET_FIX

#endif // yaneSDK_USE_delegate

#endif // __YTL_DELEGATE_H__
