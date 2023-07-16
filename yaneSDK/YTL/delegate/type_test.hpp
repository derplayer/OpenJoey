/**
 *	Template Type Identity Testing Template Class
 *		programmed by ENRA(S.Takebata)	 '03/07/12
 */

#ifndef __YTL_DELEGATE_TYPE_TEST_HPP__
#define __YTL_DELEGATE_TYPE_TEST_HPP__

#include "yes_no_type.hpp"

namespace yaneuraoGameSDK3rd {
namespace YTL {
namespace YTLdelegate {

template<typename T>
struct is_pair
{
	struct inner {	//	BCCではクラスで囲んでやらないと内部コンパイルエラーになる
		template<typename U, typename V>
		static yes_type test(pair<U,V>);
		static no_type test(...);
	};
	static T makeT();
	enum { value = sizeof(inner::test(makeT()))==sizeof(yes_type) };
};

template<typename T>
struct is_function_ptr
{
	struct inner {	//	BCCではクラスで囲んでやらないと内部コンパイルエラーになる
		static no_type test(...);
		template <typename R>
		static yes_type test(R(*)());
		template <typename R,typename A0 >
		static yes_type test(R(*)(A0));
		template <typename R,typename A0,typename A1 >
		static yes_type test(R(*)(A0,A1));
		template <typename R,typename A0,typename A1,typename A2 >
		static yes_type test(R(*)(A0,A1,A2));
		template <typename R,typename A0,typename A1,typename A2,typename A3 >
		static yes_type test(R(*)(A0,A1,A2,A3));
		template <typename R,typename A0,typename A1,typename A2,typename A3,typename A4 >
		static yes_type test(R(*)(A0,A1,A2,A3,A4));
		template <typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5 >
		static yes_type test(R(*)(A0,A1,A2,A3,A4,A5));
		template <typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6 >
		static yes_type test(R(*)(A0,A1,A2,A3,A4,A5,A6));
		template <typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename A7 >
		static yes_type test(R(*)(A0,A1,A2,A3,A4,A5,A6,A7));
		template <typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename A7,typename A8 >
		static yes_type test(R(*)(A0,A1,A2,A3,A4,A5,A6,A7,A8));
		template <typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename A7,typename A8,typename A9 >
		static yes_type test(R(*)(A0,A1,A2,A3,A4,A5,A6,A7,A8,A9));
		template <typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename A7,typename A8,typename A9,typename A10 >
		static yes_type test(R(*)(A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10));
	};
	static T makeT();
	enum { value = sizeof(inner::test(makeT()))==sizeof(yes_type) };
};


template<typename T>
struct is_member_function_ptr
{
	struct inner {	//	BCCではクラスで囲んでやらないと内部コンパイルエラーになる
		static no_type test(...);
		template <class T,typename R>
		static yes_type test(R(T::*)());
		template <class T,typename R,typename A0 >
		static yes_type test(R(T::*)(A0));
		template <class T,typename R,typename A0,typename A1 >
		static yes_type test(R(T::*)(A0,A1));
		template <class T,typename R,typename A0,typename A1,typename A2 >
		static yes_type test(R(T::*)(A0,A1,A2));
		template <class T,typename R,typename A0,typename A1,typename A2,typename A3 >
		static yes_type test(R(T::*)(A0,A1,A2,A3));
		template <class T,typename R,typename A0,typename A1,typename A2,typename A3,typename A4 >
		static yes_type test(R(T::*)(A0,A1,A2,A3,A4));
		template <class T,typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5 >
		static yes_type test(R(T::*)(A0,A1,A2,A3,A4,A5));
		template <class T,typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6 >
		static yes_type test(R(T::*)(A0,A1,A2,A3,A4,A5,A6));
		template <class T,typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename A7 >
		static yes_type test(R(T::*)(A0,A1,A2,A3,A4,A5,A6,A7));
		template <class T,typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename A7,typename A8 >
		static yes_type test(R(T::*)(A0,A1,A2,A3,A4,A5,A6,A7,A8));
		template <class T,typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename A7,typename A8,typename A9 >
		static yes_type test(R(T::*)(A0,A1,A2,A3,A4,A5,A6,A7,A8,A9));
		template <class T,typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename A7,typename A8,typename A9,typename A10 >
		static yes_type test(R(T::*)(A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10));
		template <class T,typename R>
		static yes_type test(R(T::*)()const);
		template <class T,typename R,typename A0 >
		static yes_type test(R(T::*)(A0)const);
		template <class T,typename R,typename A0,typename A1 >
		static yes_type test(R(T::*)(A0,A1)const);
		template <class T,typename R,typename A0,typename A1,typename A2 >
		static yes_type test(R(T::*)(A0,A1,A2)const);
		template <class T,typename R,typename A0,typename A1,typename A2,typename A3 >
		static yes_type test(R(T::*)(A0,A1,A2,A3)const);
		template <class T,typename R,typename A0,typename A1,typename A2,typename A3,typename A4 >
		static yes_type test(R(T::*)(A0,A1,A2,A3,A4)const);
		template <class T,typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5 >
		static yes_type test(R(T::*)(A0,A1,A2,A3,A4,A5)const);
		template <class T,typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6 >
		static yes_type test(R(T::*)(A0,A1,A2,A3,A4,A5,A6)const);
		template <class T,typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename A7 >
		static yes_type test(R(T::*)(A0,A1,A2,A3,A4,A5,A6,A7)const);
		template <class T,typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename A7,typename A8 >
		static yes_type test(R(T::*)(A0,A1,A2,A3,A4,A5,A6,A7,A8)const);
		template <class T,typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename A7,typename A8,typename A9 >
		static yes_type test(R(T::*)(A0,A1,A2,A3,A4,A5,A6,A7,A8,A9)const);
		template <class T,typename R,typename A0,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename A7,typename A8,typename A9,typename A10 >
		static yes_type test(R(T::*)(A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10)const);
	};
	static T makeT();
	enum { value = sizeof(inner::test(makeT()))==sizeof(yes_type) };
};

} // end of namespace YTLdelegate
} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif // __YTL_DELEGATE_TYPE_TEST_HPP__
