/**
 *	Function Compatibility Testing Template Class
 *		programmed by ENRA(S.Takebata)		'03/07/06
 *		reprogrammed by ENRA(S.Takebata)	'03/07/12	クラス名の変更(;´Д`)
 *
 *	一応オリジナルのアルゴリズムなんでその辺よろしく^^;
 */

#ifndef __YTL_DELEGATE_IS_COMPATIBLE_FUNCTION_HPP__
#define __YTL_DELEGATE_IS_COMPATIBLE_FUNCTION_HPP__

#include "yes_no_type.hpp"
#include "is_same.hpp"
#include "is_convertible.hpp"
#include "select_type.hpp"

namespace yaneuraoGameSDK3rd {
namespace YTL {
namespace YTLdelegate {
namespace is_compatible_function_detail {

template<typename From, typename To>
struct result_checker {
	#ifdef _MSC_VER
	enum { value = is_same<void,From>::value || is_convertible<To,From>::value };
	#else
	static const bool value = is_same<void,From>::value || is_convertible<To,From>::value;
	#endif
	typedef typename select_type<(result_checker::value), yes_type, no_type>::type type;
};
template<typename From, typename To>
struct arg_checker {
	typedef typename select_type<(is_convertible<From,To>::value), yes_type, no_type>::type type;
};
//	10引数関数までの互換性チェックが可能
//	0引数: static method → static method
template<typename fR, typename tR>
static yes_type test(fR(*from)(), tR(*to)(), typename result_checker<fR,tR>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	0引数: class method → static method
template<class fC,typename fR, typename tR>
static yes_type test(fR(fC::*from)(), tR(*to)(), typename result_checker<fR,tR>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	0引数: const class method → static method
template<class fC,typename fR, typename tR>
static yes_type test(fR(fC::*from)()const, tR(*to)(), typename result_checker<fR,tR>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	0引数: static method → class method
template<class tC,typename fR, typename tR>
static yes_type test(fR(*from)(), tR(tC::*to)(), typename result_checker<fR,tR>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	0引数: static method → const class method
template<class tC,typename fR, typename tR>
static yes_type test(fR(*from)(), tR(tC::*to)()const, typename result_checker<fR,tR>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	0引数: class method → class method
template<class fC,class tC,typename fR, typename tR>
static yes_type test(fR(fC::*from)(), tR(tC::*to)(), typename result_checker<fR,tR>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	0引数: const class method → class method
template<class fC,class tC,typename fR, typename tR>
static yes_type test(fR(fC::*from)()const, tR(tC::*to)(), typename result_checker<fR,tR>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	0引数: class method → const class method
template<class fC,class tC,typename fR, typename tR>
static yes_type test(fR(fC::*from)(), tR(tC::*to)()const, typename result_checker<fR,tR>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	0引数: const class method → const class method
template<class fC,class tC,typename fR, typename tR>
static yes_type test(fR(fC::*from)()const, tR(tC::*to)()const, typename result_checker<fR,tR>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	1引数: static method → static method
template<typename fR, typename tR, typename fA1, typename tA1>
static yes_type test(fR(*from)(fA1), tR(*to)(tA1), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	1引数: class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1>
static yes_type test(fR(fC::*from)(fA1), tR(*to)(tA1), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	1引数: const class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1>
static yes_type test(fR(fC::*from)(fA1)const, tR(*to)(tA1), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	1引数: static method → class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1>
static yes_type test(fR(*from)(fA1), tR(tC::*to)(tA1), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	1引数: static method → const class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1>
static yes_type test(fR(*from)(fA1), tR(tC::*to)(tA1)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	1引数: class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1>
static yes_type test(fR(fC::*from)(fA1), tR(tC::*to)(tA1), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	1引数: const class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1>
static yes_type test(fR(fC::*from)(fA1)const, tR(tC::*to)(tA1), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	1引数: class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1>
static yes_type test(fR(fC::*from)(fA1), tR(tC::*to)(tA1)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	1引数: const class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1>
static yes_type test(fR(fC::*from)(fA1)const, tR(tC::*to)(tA1)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	2引数: static method → static method
template<typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2>
static yes_type test(fR(*from)(fA1,fA2), tR(*to)(tA1,tA2), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	2引数: class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2>
static yes_type test(fR(fC::*from)(fA1,fA2), tR(*to)(tA1,tA2), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	2引数: const class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2>
static yes_type test(fR(fC::*from)(fA1,fA2)const, tR(*to)(tA1,tA2), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	2引数: static method → class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2>
static yes_type test(fR(*from)(fA1,fA2), tR(tC::*to)(tA1,tA2), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	2引数: static method → const class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2>
static yes_type test(fR(*from)(fA1,fA2), tR(tC::*to)(tA1,tA2)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	2引数: class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2>
static yes_type test(fR(fC::*from)(fA1,fA2), tR(tC::*to)(tA1,tA2), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	2引数: const class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2>
static yes_type test(fR(fC::*from)(fA1,fA2)const, tR(tC::*to)(tA1,tA2), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	2引数: class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2>
static yes_type test(fR(fC::*from)(fA1,fA2), tR(tC::*to)(tA1,tA2)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	2引数: const class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2>
static yes_type test(fR(fC::*from)(fA1,fA2)const, tR(tC::*to)(tA1,tA2)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	3引数: static method → static method
template<typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3>
static yes_type test(fR(*from)(fA1,fA2,fA3), tR(*to)(tA1,tA2,tA3), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	3引数: class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3), tR(*to)(tA1,tA2,tA3), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	3引数: const class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3)const, tR(*to)(tA1,tA2,tA3), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	3引数: static method → class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3>
static yes_type test(fR(*from)(fA1,fA2,fA3), tR(tC::*to)(tA1,tA2,tA3), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	3引数: static method → const class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3>
static yes_type test(fR(*from)(fA1,fA2,fA3), tR(tC::*to)(tA1,tA2,tA3)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	3引数: class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3), tR(tC::*to)(tA1,tA2,tA3), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	3引数: const class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3)const, tR(tC::*to)(tA1,tA2,tA3), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	3引数: class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3), tR(tC::*to)(tA1,tA2,tA3)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	3引数: const class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3)const, tR(tC::*to)(tA1,tA2,tA3)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	4引数: static method → static method
template<typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4), tR(*to)(tA1,tA2,tA3,tA4), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	4引数: class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4), tR(*to)(tA1,tA2,tA3,tA4), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	4引数: const class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4)const, tR(*to)(tA1,tA2,tA3,tA4), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	4引数: static method → class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4), tR(tC::*to)(tA1,tA2,tA3,tA4), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	4引数: static method → const class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4), tR(tC::*to)(tA1,tA2,tA3,tA4)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	4引数: class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4), tR(tC::*to)(tA1,tA2,tA3,tA4), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	4引数: const class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4)const, tR(tC::*to)(tA1,tA2,tA3,tA4), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	4引数: class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4), tR(tC::*to)(tA1,tA2,tA3,tA4)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	4引数: const class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4)const, tR(tC::*to)(tA1,tA2,tA3,tA4)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	5引数: static method → static method
template<typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5), tR(*to)(tA1,tA2,tA3,tA4,tA5), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	5引数: class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5), tR(*to)(tA1,tA2,tA3,tA4,tA5), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	5引数: const class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5)const, tR(*to)(tA1,tA2,tA3,tA4,tA5), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	5引数: static method → class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	5引数: static method → const class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	5引数: class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	5引数: const class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5)const, tR(tC::*to)(tA1,tA2,tA3,tA4,tA5), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	5引数: class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	5引数: const class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5)const, tR(tC::*to)(tA1,tA2,tA3,tA4,tA5)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, yes_type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	6引数: static method → static method
template<typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5,fA6), tR(*to)(tA1,tA2,tA3,tA4,tA5,tA6), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	6引数: class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6), tR(*to)(tA1,tA2,tA3,tA4,tA5,tA6), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	6引数: const class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6)const, tR(*to)(tA1,tA2,tA3,tA4,tA5,tA6), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	6引数: static method → class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5,fA6), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	6引数: static method → const class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5,fA6), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	6引数: class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	6引数: const class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6)const, tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	6引数: class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	6引数: const class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6)const, tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, yes_type*, yes_type*, yes_type*, yes_type*);
//	7引数: static method → static method
template<typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7), tR(*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, yes_type*, yes_type*, yes_type*);
//	7引数: class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7), tR(*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, yes_type*, yes_type*, yes_type*);
//	7引数: const class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7)const, tR(*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, yes_type*, yes_type*, yes_type*);
//	7引数: static method → class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, yes_type*, yes_type*, yes_type*);
//	7引数: static method → const class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, yes_type*, yes_type*, yes_type*);
//	7引数: class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, yes_type*, yes_type*, yes_type*);
//	7引数: const class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7)const, tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, yes_type*, yes_type*, yes_type*);
//	7引数: class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, yes_type*, yes_type*, yes_type*);
//	7引数: const class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7)const, tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, yes_type*, yes_type*, yes_type*);
//	8引数: static method → static method
template<typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8), tR(*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, yes_type*, yes_type*);
//	8引数: class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8), tR(*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, yes_type*, yes_type*);
//	8引数: const class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8)const, tR(*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, yes_type*, yes_type*);
//	8引数: static method → class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, yes_type*, yes_type*);
//	8引数: static method → const class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, yes_type*, yes_type*);
//	8引数: class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, yes_type*, yes_type*);
//	8引数: const class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8)const, tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, yes_type*, yes_type*);
//	8引数: class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, yes_type*, yes_type*);
//	8引数: const class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8)const, tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, yes_type*, yes_type*);
//	9引数: static method → static method
template<typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9), tR(*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, yes_type*);
//	9引数: class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9), tR(*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, yes_type*);
//	9引数: const class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9)const, tR(*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, yes_type*);
//	9引数: static method → class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, yes_type*);
//	9引数: static method → const class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, yes_type*);
//	9引数: class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, yes_type*);
//	9引数: const class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9)const, tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, yes_type*);
//	9引数: class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, yes_type*);
//	9引数: const class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9)const, tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, yes_type*);
//	10引数: static method → static method
template<typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9, typename fA10, typename tA10>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9,fA10), tR(*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9,tA10), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, typename arg_checker<fA10,tA10>::type*);
//	10引数: class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9, typename fA10, typename tA10>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9,fA10), tR(*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9,tA10), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, typename arg_checker<fA10,tA10>::type*);
//	10引数: const class method → static method
template<class fC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9, typename fA10, typename tA10>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9,fA10)const, tR(*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9,tA10), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, typename arg_checker<fA10,tA10>::type*);
//	10引数: static method → class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9, typename fA10, typename tA10>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9,fA10), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9,tA10), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, typename arg_checker<fA10,tA10>::type*);
//	10引数: static method → const class method
template<class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9, typename fA10, typename tA10>
static yes_type test(fR(*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9,fA10), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9,tA10)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, typename arg_checker<fA10,tA10>::type*);
//	10引数: class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9, typename fA10, typename tA10>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9,fA10), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9,tA10), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, typename arg_checker<fA10,tA10>::type*);
//	10引数: const class method → class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9, typename fA10, typename tA10>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9,fA10)const, tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9,tA10), typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, typename arg_checker<fA10,tA10>::type*);
//	10引数: class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9, typename fA10, typename tA10>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9,fA10), tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9,tA10)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, typename arg_checker<fA10,tA10>::type*);
//	10引数: const class method → const class method
template<class fC,class tC,typename fR, typename tR, typename fA1, typename tA1, typename fA2, typename tA2, typename fA3, typename tA3, typename fA4, typename tA4, typename fA5, typename tA5, typename fA6, typename tA6, typename fA7, typename tA7, typename fA8, typename tA8, typename fA9, typename tA9, typename fA10, typename tA10>
static yes_type test(fR(fC::*from)(fA1,fA2,fA3,fA4,fA5,fA6,fA7,fA8,fA9,fA10)const, tR(tC::*to)(tA1,tA2,tA3,tA4,tA5,tA6,tA7,tA8,tA9,tA10)const, typename result_checker<fR,tR>::type*, typename arg_checker<fA1,tA1>::type*, typename arg_checker<fA2,tA2>::type*, typename arg_checker<fA3,tA3>::type*, typename arg_checker<fA4,tA4>::type*, typename arg_checker<fA5,tA5>::type*, typename arg_checker<fA6,tA6>::type*, typename arg_checker<fA7,tA7>::type*, typename arg_checker<fA8,tA8>::type*, typename arg_checker<fA9,tA9>::type*, typename arg_checker<fA10,tA10>::type*);

static no_type test(...);

} // end of namespace is_compatible_function_detail

template<class From, typename To>
struct is_compatible_function
{
	#ifdef _MSC_VER
	enum { value = sizeof(yes_type)==sizeof(is_compatible_function_detail::test((From)0,(To)0,(yes_type*)0,(yes_type*)0,(yes_type*)0,(yes_type*)0,(yes_type*)0,(yes_type*)0,(yes_type*)0,(yes_type*)0,(yes_type*)0,(yes_type*)0,(yes_type*)0)) };
	#else
	static const bool value = sizeof(yes_type)==sizeof(is_compatible_function_detail::test((From)0,(To)0,(yes_type*)0,(yes_type*)0,(yes_type*)0,(yes_type*)0,(yes_type*)0,(yes_type*)0,(yes_type*)0,(yes_type*)0,(yes_type*)0,(yes_type*)0,(yes_type*)0));
	#endif
};

} // end of namespace YTLdelegate
} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif // __YTL_DELEGATE_IS_COMPATIBLE_FUNCTION_HPP__
