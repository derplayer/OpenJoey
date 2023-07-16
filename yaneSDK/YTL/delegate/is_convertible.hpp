/**
 *	Type Convertibility Testing Template Class
 *		programmed by ENRA(S.Takebata)	 '03/06/23
 *		reprogrammed by ENRA(S.Takebata) '03/07/04	BCC5.5.1‚É‘Î‰
 *		reprogrammed by ENRA(S.Takebata) '03/07/10	makeFrom()ŠÖ”‚ğŒÄ‚Ô‚æ‚¤‚É•ÏX
 */

#ifndef __YTL_DELEGATE_IS_CONVERTIBLE_HPP__
#define __YTL_DELEGATE_IS_CONVERTIBLE_HPP__

#include "yes_no_type.hpp"

namespace yaneuraoGameSDK3rd {
namespace YTL {
namespace YTLdelegate {

#ifdef yaneSDK_BCC_Mode

///	FromŒ^‚ğToŒ^‚É•ÏŠ·o—ˆ‚é‚È‚çtrue‚ğ•Ô‚·
template <typename From, typename To>
struct is_convertible
{
	template <typename T> struct checker {
		static no_type check(...);
		static yes_type check(T);
	};
	static From makeFrom();
	static bool const value = sizeof(checker<To>::check(makeFrom()))==sizeof(yes_type);
};
template<typename T> struct is_convertible<void,T> { static const bool value = false; };
template<> struct is_convertible<void,void> { static const bool value = true; };

#else

namespace is_convertible_detail {

template<typename From> struct does_conversion_exist
{
	template<typename To> struct result_
	{
		static yes_type check(To);
		static no_type check(...);
		static From makeFrom();
		enum { value = sizeof(check(makeFrom()))==sizeof(yes_type) };
	};
};

template<> struct does_conversion_exist<void>
{
	template<typename To> struct result_
	{
		enum { value = is_same<void,To>::value };
	};
};

} // end of namespace is_convertible_detail

///	FromŒ^‚ğToŒ^‚É•ÏŠ·o—ˆ‚é‚È‚çtrue‚ğ•Ô‚·
template<typename From,typename To>
struct is_convertible
{
	enum { value = is_convertible_detail::does_conversion_exist<From>::template result_<To>::value, };
};

#endif	//	ifdef yaneSDK_BCC_Mode

} // end of namespace YTLdelegate
} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif // __YTL_DELEGATE_IS_CONVERTIBLE_HPP__
