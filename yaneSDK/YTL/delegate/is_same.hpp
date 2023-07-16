/**
 *	Type Identity Testing Template Class
 *		programmed by ENRA(S.Takebata)	 '02/10/06
 *		reprogrammed by ENRA(S.Takebata) '03/07/04	BCC5.5.1‚É‘Î‰ž
 */

#ifndef __YTL_DELEGATE_IS_SAME_HPP__
#define __YTL_DELEGATE_IS_SAME_HPP__

#include "yes_no_type.hpp"

namespace yaneuraoGameSDK3rd {
namespace YTL {
namespace YTLdelegate {

#ifdef yaneSDK_MSVC_Mode

///	L‚ÆR‚ÌŒ^‚ª“¯‚¶‚È‚ç‚Îtrue‚ðresult’l‚Æ‚µ‚Ä•Ô‚·
template<typename L, typename R>
struct is_same {
	template<typename T> struct inner {
		enum { value = false };
	};
	template<> struct inner<R> {
		enum { value = true };
	};
	enum { value = inner<L>::value };
};

#else

///	L‚ÆR‚ÌŒ^‚ª“¯‚¶‚È‚ç‚Îtrue‚ðresult’l‚Æ‚µ‚Ä•Ô‚·
template<typename L, typename R>
struct is_same {
	static const bool value = false;
};
template<typename L>
struct is_same<L,L> {
	static const bool value = true;
};

#endif // ifdef yaneSDK_MSVC_Mode

} // end of namespace YTLdelegate
} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif // __YTL_DELEGATE_IS_SAME_HPP__
