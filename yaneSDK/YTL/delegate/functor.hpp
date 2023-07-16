/**
 *	Functor Template Class
 *		programmed by ENRA(S.Takebata)	 '02/10/06
 *		reprogrammed by ENRA(S.Takebata) '03/07/04	BCC5.5.1に対応
 *		reprogrammed by ENRA(S.Takebata) '03/07/10	set関数にfunctor型を代入できるようにした
 */

#ifndef __YTL_DELEGATE_FUNCTOR_HPP__
#define __YTL_DELEGATE_FUNCTOR_HPP__

#include "smart_ptr.h"

namespace yaneuraoGameSDK3rd {
namespace YTL {
namespace YTLdelegate {

//	マクロを使ってfunctorクラスを宣言^^;
#define DELEGATE_FUNCTOR_IMPREMENTS(ARGS, FUNCTION_ARG1, FUNCTION_ARG2, COMMA) \
template<typename R, class A> \
struct functor_r##ARGS { \
	typedef R result_type; \
	typedef A arguments_type; \
	typedef functor_r##ARGS<result_type,arguments_type> this_type; \
	typedef result_type (this_type::*operator_type)(FUNCTION_ARG1) const; \
	template<class ClassPointee, class FunctionPtr> \
	struct class_method_invoker { \
		class_method_invoker(const ClassPointee& p, FunctionPtr f) : _p(p), _f(f) {}; \
		inline static result_type invoke(const void* p COMMA FUNCTION_ARG1) { \
			const class_method_invoker& rThis = *(const class_method_invoker*)p; \
			return (rThis._p->*rThis._f)(FUNCTION_ARG2); \
		} \
	private: \
		ClassPointee _p; \
		FunctionPtr	 _f; \
	}; \
	template<class FunctionPtr> \
	struct method_invoker { \
		method_invoker(FunctionPtr f) : _f(f) {}; \
		inline static result_type invoke(const void* p COMMA FUNCTION_ARG1) { \
			const method_invoker& rThis = *(const method_invoker*)p; \
			return rThis._f(FUNCTION_ARG2); \
		} \
	private: \
		FunctionPtr _f; \
	}; \
	template<class Functor> \
	struct functor_invoker { \
		functor_invoker(const Functor& f) : _f(f) {}; \
		inline static result_type invoke(const void* p COMMA FUNCTION_ARG1) { \
			const functor_invoker& rThis = *(const functor_invoker*)p; \
			return rThis._f(FUNCTION_ARG2); \
		} \
	private: \
		Functor _f; \
	}; \
	template<class FunctorSmartPtr> \
	struct functor_smart_ptr_invoker { \
		functor_smart_ptr_invoker(const FunctorSmartPtr& p) : _p(p) {}; \
		inline static result_type invoke(const void* p COMMA FUNCTION_ARG1) { \
			const functor_smart_ptr_invoker& rThis = *(const functor_smart_ptr_invoker*)p; \
			return (*rThis._p)(FUNCTION_ARG2); \
		} \
	private: \
		FunctorSmartPtr _p; \
	}; \
	struct null_invoker { \
		inline static result_type invoke(const void* p COMMA FUNCTION_ARG1) { return result_type(); } \
	}; \
	this_type() : _invoke_function(&null_invoker::invoke) {}; \
	this_type(const this_type& f) : _invoke_function(f._invoke_function), _pInvoker(f._pInvoker) {}; \
	template<class Invoker>	this_type(const smart_ptr<Invoker>& p) { set(p); } \
	result_type operator()(FUNCTION_ARG1) const { \
		return _invoke_function(_pInvoker.getPointer() COMMA FUNCTION_ARG2); \
	} \
	template<class Invoker> \
	void set(const smart_ptr<Invoker>& p) { \
		if ( !p.isNull()&&!p.isEnd() ) { \
			_pInvoker = smart_ptr_static_cast<void>(p); \
			_invoke_function = &Invoker::invoke; \
		} else { \
			clear(); \
		} \
	} \
	void set(const this_type& f) { \
		_pInvoker = f._pInvoker; \
		_invoke_function = f._invoke_function; \
	} \
	void clear() { \
		_pInvoker.Delete(); \
		_invoke_function = &null_invoker::invoke; \
	} \
	bool isNull() const { \
		return (_invoke_function == &null_invoker::invoke); \
	} \
private: \
	typedef result_type (*invoke_function_type)(const void* p COMMA FUNCTION_ARG1); \
	invoke_function_type	_invoke_function; \
	smart_obj				_pInvoker; \
}; \
template<class A> \
struct functor_v##ARGS { \
	typedef void result_type; \
	typedef A arguments_type; \
	typedef functor_v##ARGS<arguments_type> this_type; \
	typedef result_type (this_type::*operator_type)(FUNCTION_ARG1) const; \
	template<class ClassPointee, class FunctionPtr> \
	struct class_method_invoker { \
		class_method_invoker(const ClassPointee& p, FunctionPtr f) : _p(p), _f(f) {}; \
		inline static result_type invoke(const void* p COMMA FUNCTION_ARG1) { \
			const class_method_invoker& rThis = *(const class_method_invoker*)p; \
			(rThis._p->*rThis._f)(FUNCTION_ARG2); \
		} \
	private: \
		ClassPointee _p; \
		FunctionPtr	 _f; \
	}; \
	template<class FunctionPtr> \
	struct method_invoker { \
		method_invoker(FunctionPtr f) : _f(f) {}; \
		inline static result_type invoke(const void* p COMMA FUNCTION_ARG1) { \
			const method_invoker& rThis = *(const method_invoker*)p; \
			rThis._f(FUNCTION_ARG2); \
		} \
	private: \
		FunctionPtr _f; \
	}; \
	template<class Functor> \
	struct functor_invoker { \
		functor_invoker(const Functor& f) : _f(f) {}; \
		inline static result_type invoke(const void* p COMMA FUNCTION_ARG1) { \
			const functor_invoker& rThis = *(const functor_invoker*)p; \
			rThis._f(FUNCTION_ARG2); \
		} \
	private: \
		Functor _f; \
	}; \
	template<class FunctorSmartPtr> \
	struct functor_smart_ptr_invoker { \
		functor_smart_ptr_invoker(const FunctorSmartPtr& p) : _p(p) {}; \
		inline static result_type invoke(const void* p COMMA FUNCTION_ARG1) { \
			const functor_smart_ptr_invoker& rThis = *(const functor_smart_ptr_invoker*)p; \
			(*rThis._p)(FUNCTION_ARG2); \
		} \
	private: \
		FunctorSmartPtr _p; \
	}; \
	struct null_invoker { \
		inline static result_type invoke(const void* p COMMA FUNCTION_ARG1) {}; \
	}; \
	this_type() : _invoke_function(&null_invoker::invoke) {}; \
	this_type(const this_type& f) : _invoke_function(f._invoke_function), _pInvoker(f._pInvoker) {}; \
	template<class Invoker>	this_type(const smart_ptr<Invoker>& p) { set(p); } \
	result_type operator()(FUNCTION_ARG1) const { \
		_invoke_function(_pInvoker.getPointer() COMMA FUNCTION_ARG2); \
	} \
	template<class Invoker> \
	void set(const smart_ptr<Invoker>& p) { \
		if ( !p.isNull()&&!p.isEnd() ) { \
			_pInvoker = smart_ptr_static_cast<void>(p); \
			_invoke_function = &Invoker::invoke; \
		} else { \
			clear(); \
		} \
	} \
	void set(const this_type& f) { \
		_pInvoker = f._pInvoker; \
		_invoke_function = f._invoke_function; \
	} \
	void clear() { \
		_pInvoker.Delete(); \
		_invoke_function = &null_invoker::invoke; \
	} \
	bool isNull() const { \
		return (_invoke_function == &null_invoker::invoke); \
	} \
private: \
	typedef result_type (*invoke_function_type)(const void* p COMMA FUNCTION_ARG1); \
	invoke_function_type	_invoke_function; \
	smart_obj				_pInvoker; \
};

//	ここでコンパイルエラーが発生した場合、このdelegateと互換性のないfunctorをsetした可能性がある。
#define NON_LITERAL
#define COMMA ,
DELEGATE_FUNCTOR_IMPREMENTS(0, NON_LITERAL, NON_LITERAL, NON_LITERAL);
DELEGATE_FUNCTOR_IMPREMENTS(1, typename A::A1 a1, a1, COMMA);
DELEGATE_FUNCTOR_IMPREMENTS(2, typename A::A1 a1 COMMA typename A::A2 a2, a1 COMMA a2, COMMA);
DELEGATE_FUNCTOR_IMPREMENTS(3, typename A::A1 a1 COMMA typename A::A2 a2 COMMA typename A::A3 a3, a1 COMMA a2 COMMA a3, COMMA);
DELEGATE_FUNCTOR_IMPREMENTS(4, typename A::A1 a1 COMMA typename A::A2 a2 COMMA typename A::A3 a3 COMMA typename A::A4 a4, a1 COMMA a2 COMMA a3 COMMA a4, COMMA);
DELEGATE_FUNCTOR_IMPREMENTS(5, typename A::A1 a1 COMMA typename A::A2 a2 COMMA typename A::A3 a3 COMMA typename A::A4 a4 COMMA typename A::A5 a5, a1 COMMA a2 COMMA a3 COMMA a4 COMMA a5, COMMA);
DELEGATE_FUNCTOR_IMPREMENTS(6, typename A::A1 a1 COMMA typename A::A2 a2 COMMA typename A::A3 a3 COMMA typename A::A4 a4 COMMA typename A::A5 a5 COMMA typename A::A6 a6, a1 COMMA a2 COMMA a3 COMMA a4 COMMA a5 COMMA a6, COMMA);
DELEGATE_FUNCTOR_IMPREMENTS(7, typename A::A1 a1 COMMA typename A::A2 a2 COMMA typename A::A3 a3 COMMA typename A::A4 a4 COMMA typename A::A5 a5 COMMA typename A::A6 a6 COMMA typename A::A7 a7, a1 COMMA a2 COMMA a3 COMMA a4 COMMA a5 COMMA a6 COMMA a7, COMMA);
DELEGATE_FUNCTOR_IMPREMENTS(8, typename A::A1 a1 COMMA typename A::A2 a2 COMMA typename A::A3 a3 COMMA typename A::A4 a4 COMMA typename A::A5 a5 COMMA typename A::A6 a6 COMMA typename A::A7 a7 COMMA typename A::A8 a8, a1 COMMA a2 COMMA a3 COMMA a4 COMMA a5 COMMA a6 COMMA a7 COMMA a8, COMMA);
DELEGATE_FUNCTOR_IMPREMENTS(9, typename A::A1 a1 COMMA typename A::A2 a2 COMMA typename A::A3 a3 COMMA typename A::A4 a4 COMMA typename A::A5 a5 COMMA typename A::A6 a6 COMMA typename A::A7 a7 COMMA typename A::A8 a8 COMMA typename A::A9 a9, a1 COMMA a2 COMMA a3 COMMA a4 COMMA a5 COMMA a6 COMMA a7 COMMA a8 COMMA a9, COMMA);
DELEGATE_FUNCTOR_IMPREMENTS(10, typename A::A1 a1 COMMA typename A::A2 a2 COMMA typename A::A3 a3 COMMA typename A::A4 a4 COMMA typename A::A5 a5 COMMA typename A::A6 a6 COMMA typename A::A7 a7 COMMA typename A::A8 a8 COMMA typename A::A9 a9 COMMA typename A::A10 a10, a1 COMMA a2 COMMA a3 COMMA a4 COMMA a5 COMMA a6 COMMA a7 COMMA a8 COMMA a9 COMMA a10, COMMA);
#undef NON_LITERAL
#undef COMMA
#undef DELEGATE_FUNCTOR_IMPREMENTS


///	使わないという事を表す型
typedef void void_t;	//	エラーメッセージ見難いし void にしとこ(´ー`)y-~~

///	引数の型をまとめるためのテンプレートクラス
template<
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
struct args_type {
	typedef A1  A1;
	typedef A2  A2;
	typedef A3  A3;
	typedef A4  A4;
	typedef A5  A5;
	typedef A6  A6;
	typedef A7  A7;
	typedef A8  A8;
	typedef A9  A9;
	typedef A10 A10;
	#ifdef yaneSDK_MSVC_Mode
	enum {
	#else
	static const int
	#endif
		   args_num = ((is_same<A1 ,void_t>::value) ? 0 : 1
					+ ((is_same<A2 ,void_t>::value) ? 0 : 1
					+ ((is_same<A3 ,void_t>::value) ? 0 : 1
					+ ((is_same<A4 ,void_t>::value) ? 0 : 1
					+ ((is_same<A5 ,void_t>::value) ? 0 : 1
					+ ((is_same<A6 ,void_t>::value) ? 0 : 1
					+ ((is_same<A7 ,void_t>::value) ? 0 : 1
					+ ((is_same<A8 ,void_t>::value) ? 0 : 1
					+ ((is_same<A9 ,void_t>::value) ? 0 : 1
					+ ((is_same<A10,void_t>::value) ? 0 : 1
					))))))))))
	#ifdef yaneSDK_MSVC_Mode
	};
	#else
	;
	#endif
};

///	指定された仮引数と戻り値に適合するfunctorクラスの型を返す
template<typename R, class A>
struct get_functor_type {
	typedef R result_type;
	typedef A arguments_type;

	template<int L, int R> struct is_equal {
		#ifdef yaneSDK_MSVC_Mode
		enum { value = L==R };
		#else
		static const bool value = L==R;
		#endif
	};

	typedef typename select_type<
		#ifdef yaneSDK_MSVC_Mode	//	なんで括弧ついてたらあかんのん？(;´Д`)
    	(is_equal<arguments_type::args_num,0>::value), typename select_type<(is_same<result_type,void>::value), functor_v0<arguments_type>, functor_r0<result_type,arguments_type> >::type, typename select_type<
    	(is_equal<arguments_type::args_num,1>::value), typename select_type<(is_same<result_type,void>::value), functor_v1<arguments_type>, functor_r1<result_type,arguments_type> >::type, typename select_type<
		(is_equal<arguments_type::args_num,2>::value), typename select_type<(is_same<result_type,void>::value), functor_v2<arguments_type>, functor_r2<result_type,arguments_type> >::type, typename select_type<
		(is_equal<arguments_type::args_num,3>::value), typename select_type<(is_same<result_type,void>::value), functor_v3<arguments_type>, functor_r3<result_type,arguments_type> >::type, typename select_type<
		(is_equal<arguments_type::args_num,4>::value), typename select_type<(is_same<result_type,void>::value), functor_v4<arguments_type>, functor_r4<result_type,arguments_type> >::type, typename select_type<
		(is_equal<arguments_type::args_num,5>::value), typename select_type<(is_same<result_type,void>::value), functor_v5<arguments_type>, functor_r5<result_type,arguments_type> >::type, typename select_type<
		(is_equal<arguments_type::args_num,6>::value), typename select_type<(is_same<result_type,void>::value), functor_v6<arguments_type>, functor_r6<result_type,arguments_type> >::type, typename select_type<
		(is_equal<arguments_type::args_num,7>::value), typename select_type<(is_same<result_type,void>::value), functor_v7<arguments_type>, functor_r7<result_type,arguments_type> >::type, typename select_type<
		(is_equal<arguments_type::args_num,8>::value), typename select_type<(is_same<result_type,void>::value), functor_v8<arguments_type>, functor_r8<result_type,arguments_type> >::type, typename select_type<
		(is_equal<arguments_type::args_num,9>::value), typename select_type<(is_same<result_type,void>::value), functor_v9<arguments_type>, functor_r9<result_type,arguments_type> >::type, typename select_type<
		(is_equal<arguments_type::args_num,10>::value), typename select_type<(is_same<result_type,void>::value), functor_v10<arguments_type>, functor_r10<result_type,arguments_type> >::type, typename select_type<
		#else
    	(is_equal<(arguments_type::args_num),0>::value), typename select_type<(is_same<result_type,void>::value), functor_v0<arguments_type>, functor_r0<result_type,arguments_type> >::type, typename select_type<
    	(is_equal<(arguments_type::args_num),1>::value), typename select_type<(is_same<result_type,void>::value), functor_v1<arguments_type>, functor_r1<result_type,arguments_type> >::type, typename select_type<
		(is_equal<(arguments_type::args_num),2>::value), typename select_type<(is_same<result_type,void>::value), functor_v2<arguments_type>, functor_r2<result_type,arguments_type> >::type, typename select_type<
		(is_equal<(arguments_type::args_num),3>::value), typename select_type<(is_same<result_type,void>::value), functor_v3<arguments_type>, functor_r3<result_type,arguments_type> >::type, typename select_type<
		(is_equal<(arguments_type::args_num),4>::value), typename select_type<(is_same<result_type,void>::value), functor_v4<arguments_type>, functor_r4<result_type,arguments_type> >::type, typename select_type<
		(is_equal<(arguments_type::args_num),5>::value), typename select_type<(is_same<result_type,void>::value), functor_v5<arguments_type>, functor_r5<result_type,arguments_type> >::type, typename select_type<
		(is_equal<(arguments_type::args_num),6>::value), typename select_type<(is_same<result_type,void>::value), functor_v6<arguments_type>, functor_r6<result_type,arguments_type> >::type, typename select_type<
		(is_equal<(arguments_type::args_num),7>::value), typename select_type<(is_same<result_type,void>::value), functor_v7<arguments_type>, functor_r7<result_type,arguments_type> >::type, typename select_type<
		(is_equal<(arguments_type::args_num),8>::value), typename select_type<(is_same<result_type,void>::value), functor_v8<arguments_type>, functor_r8<result_type,arguments_type> >::type, typename select_type<
		(is_equal<(arguments_type::args_num),9>::value), typename select_type<(is_same<result_type,void>::value), functor_v9<arguments_type>, functor_r9<result_type,arguments_type> >::type, typename select_type<
		(is_equal<(arguments_type::args_num),10>::value), typename select_type<(is_same<result_type,void>::value), functor_v10<arguments_type>, functor_r10<result_type,arguments_type> >::type, typename select_type<
		#endif
		false, void, void>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type
    >::type type;
};

} // end of namespace YTLdelegate
} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif // __YTL_DELEGATE_FUNCTOR_HPP__
