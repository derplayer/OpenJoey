/**
 *	Type Selecting Template Class
 *		programmed by ENRA(S.Takebata)	 '02/10/06
 *		reprogrammed by ENRA(S.Takebata) '03/07/04	BCC5.5.1Ç…ëŒâû
 */

#ifndef __YTL_DELEGATE_SELECT_TYPE_HPP__
#define __YTL_DELEGATE_SELECT_TYPE_HPP__

namespace yaneuraoGameSDK3rd {
namespace YTL {
namespace YTLdelegate {
namespace select_type_detail {

struct if_true {
	template<typename T, typename F> struct _result {
		typedef T type;
	};
};
struct if_false {
	template<typename T, typename F> struct _result {
		typedef F type;
	};
};
template<bool b> struct select_selector {
	typedef if_true type;
};
template<> struct select_selector<false> {
	typedef if_false type;
};

} // end of namespace select_type_detail

//	iÇ™0à»äOÇ»ÇÁTÅA0Ç»ÇÁFÇtypeå^Ç∆ÇµÇƒï‘Ç∑
template<bool b, typename T, typename F>
struct select_type {
	typedef typename select_type_detail::select_selector<b>::type selector_type;
	typedef typename selector_type::template _result<T,F>::type type;
};

} // end of namespace YTLdelegate
} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif // __YTL_DELEGATE_SELECT_TYPE_HPP__
