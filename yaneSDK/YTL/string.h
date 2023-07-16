/**
	simple string class
		STLのstring classは、VC++6と7(.NET)で実装が異なるので
		自前で用意する。
		（STLPortも実装が不変とは限らないので）

	implemented by yaneurao(M.Isozaki) '03/04/02
	implemented by coLun(Y.Imamura) '03/04/07 - '03/05/19

	'03/04/25　→　substrの不具合を解消
	'03/04/27　→　rfindの不具合を解消(文字を引数に取るrfind)
	'03/05/01　→　replaceの不具合を解消
	'03/05/12　→　eraseの不具合を解消
	'03/05/12　→　reverse_iteratorの不具合を解消
	'03/05/13　→　rfindの不具合を解消(文字列を引数に取るrfind)
	'03/05/19　→　iteratorとconst_iteratorの定義の見直し（boost対応）
	'03/06/20　→　reverse_iteratorとconst_reverse_iteratorの見直し（VC6対応）

	UNICODEでもうまく動くといいなぁ．．ヽ(´Д｀)ノ

	なんか春休みの宿題って感じ．．

	いろいろ足りないメンバあるかも知れないけど、追加したら、
	やねうらおまでメールしてちょーだい（＾＾；
*/

#ifndef __YTLString_h__
#define __YTLString_h__

#include "../YTL/allocator.h"
//#include "../YTL/exceptions.h"
#include <stdexcept>
#include <string>
#include <iterator>
#include <xutility>
//#include <iostream>

namespace yaneuraoGameSDK3rd {
namespace YTL {
namespace YTLstring {
namespace detail {

//	ぬるぽ( ´∀｀)
struct null_pointer_exception : public std::exception
{
	null_pointer_exception(const char* str) : std::exception(str) {}
};
//	typedefにしときますか…
typedef std::out_of_range	index_out_of_bounds_exception;

//	VC6/VC7/STLPortで共通の名前がないので結局自分で定義しないといけない
template<class T> inline
const T& _max(const T& _X, const T& _Y) { return (_X < _Y ? _Y : _X); }
template<class T> inline
const T& _min(const T& _X, const T& _Y) { return (_Y < _X ? _Y : _X); }

}	//	end of namespace detail

template <class E, class T = std::char_traits<E>, class A = YTLallocator::allocator<E> >
class basic_string
{/**
	STL互換stringクラス(かなりしょぼめ)
	非常に単純な実装だけど勘弁してちょ．．(ρ_；)ﾉ
*/
public:
//#!定義等（イタレータなども）
	typedef basic_string		self_type;
	typedef T					traits_type;
	typedef A					allocator_type;
	typedef typename T::char_type		char_type;
	typedef typename A::size_type		size_type;
	typedef typename A::difference_type	difference_type;
	typedef typename A::pointer			pointer;
	typedef typename A::const_pointer	const_pointer;
	typedef typename A::reference		reference;
	typedef typename A::const_reference	const_reference;
	typedef typename A::value_type		value_type;
	static const size_type		npos;
	static const char_type		char_null;

	class iterator : public std::iterator<std::random_access_iterator_tag, value_type, difference_type> {
	public:
		typedef iterator					self_type;
		typedef basic_string				parent_type;
		typedef reference					reference_type;
		typedef pointer						pointer_type;
		typedef size_type					size_type;

		self_type() : m_nPos(0),m_pStr(NULL) {}
		self_type(const self_type& i) : m_nPos(i.get()), m_pStr(i.GetStringPtr()) {}
		self_type(size_type n, parent_type* pStr) : m_pStr(pStr) { set(n); }

		//	アクセッサ
		parent_type * GetStringPtr() const { return m_pStr; }
		size_type get() const { return m_nPos; }
		void set(size_type n) {
			size_type len=length();
			if(n==npos || len<n)m_nPos=len;
			else m_nPos = n;
		}

		//	*
		reference_type operator* () const {
			parent_type* p = GetStringPtr();
			if ( p==NULL ) {
				throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::iterator::operator* () const");
			}
			return (*p)[m_nPos];
		}
		pointer_type operator->() const { return &(operator*()); }

		//	演算子
		self_type& operator+= (size_type n){
			if(n==npos)set(npos);
			else set(m_nPos+n);
			return *this;
		}
		self_type& operator-= (size_type n){
			if (m_nPos < n) m_nPos = 0;
			else set(m_nPos-n);
			return *this;
		}
		///	加減算のためのオペレータ
		self_type& operator++() { inc(); return (*this); }
		self_type operator++(int) { self_type i = *this; inc(); return i; }
		self_type& operator--() { dec(); return (*this); }
		self_type operator--(int) { self_type i = *this; dec(); return i; }
		self_type operator+(size_type _N) const { self_type i = *this; return i+=_N; }
		self_type operator-(size_type _N) const { self_type i = *this; return i-=_N; }
		difference_type operator-(const self_type & it) const { return m_nPos-it.m_nPos; }

		//	比較オペレータ
		bool operator== (const self_type& it) const { return (m_nPos == it.m_nPos) && (m_pStr == it.m_pStr); }
		bool operator!= (const self_type& it) const { return !(*this == it); }
		//	size_typeとの比較も用意しないとstring::nposと比較できない
		bool operator== (size_type size) const { if(size==npos)size=length(); return m_nPos == size; }
		bool operator!= (size_type size) const { return !(*this == size); }
		reference_type operator[] (difference_type n) const { return *((*this)+n); }

		void inc(){ set(m_nPos+1); }
		void dec(){ if(0<m_nPos)set(m_nPos-1); }
		size_type length() const { return m_pStr!=0?m_pStr->size():0; }
	protected:
		size_type		m_nPos;	//	0オリジンなポジションで保存するとしよう
		parent_type*	m_pStr; // 親へのポインタが無いと加減算のときにendの判定がでけん
	};

	class const_iterator : public std::iterator<std::random_access_iterator_tag, value_type, difference_type> {
	public:
		typedef const_iterator				self_type;
		typedef basic_string				parent_type;
		typedef reference					reference_type;
		typedef pointer						pointer_type;
		typedef size_type					size_type;

		self_type() : m_nPos(0), m_pStr(NULL) {}
		self_type(const iterator& i) : m_nPos(i.get()), m_pStr(i.GetStringPtr()) {}
		self_type(const self_type& i) : m_nPos(i.get()), m_pStr(i.GetStringPtr()) {}
		self_type(size_type n, const parent_type* pStr) : m_pStr(pStr) { set(n); }

		//	アクセッサ
		parent_type * GetStringPtr() const { return m_pStr; }
		size_type get() const { return m_nPos; }
		void set(size_type n) {
			size_type len=length();
			if(n==npos || len<n)m_nPos=len;
			else m_nPos = n;
		}

		//	*
		reference_type operator* () const {
			parent_type* p = GetStringPtr();
			if ( p==NULL ) {
				throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::const_iterator::operator* () const");
			}
			return (*p)[m_nPos];
		}
		pointer_type operator-> () const { return &(operator*()); }

		//	演算子
		self_type& operator+= (size_type n){
			if(n==npos)set(npos);
			else set(m_nPos+n);
			return *this;
		}
		self_type& operator-= (size_type n){
			if (m_nPos < n) m_nPos = 0;
			else set(m_nPos-n);
			return *this;
		}
		///	加減算のためのオペレータ
		self_type& operator++() { inc(); return (*this); }
		self_type operator++(int) { self_type i = *this; inc(); return i; }
		self_type& operator--() { dec(); return (*this); }
		self_type operator--(int) { self_type i = *this; dec(); return i; }
		self_type operator+(size_type _N) const { self_type i = *this; return i+=_N; }
		self_type operator-(size_type _N) const { self_type i = *this; return i-=_N; }
		difference_type operator-(const self_type& it) const { return m_nPos-it.m_nPos; }

		//	比較オペレータ
		bool operator== (const self_type& it) const { return (m_nPos == it.m_nPos) && (m_pStr == it.m_pStr); }
		bool operator!= (const self_type& it) const { return !(*this == it); }
		//	size_typeとの比較も用意しないとstring::nposと比較できない
		bool operator== (size_type size) const { if(size==npos)size=length(); return m_nPos == size; }
		bool operator!= (size_type size) const { return !(*this == size); }
		reference_type operator[] (difference_type n) const { return *((*this)+n); }

		void inc(){ set(m_nPos+1); }
		void dec(){ if(0<m_nPos)set(m_nPos-1); }
		size_type length() const { return m_pStr!=0?m_pStr->size():0; }
	protected:
		size_type		m_nPos;	//	0オリジンなポジションで保存するとしよう
		const parent_type*	m_pStr; // 親へのポインタが無いと加減算のときにendの判定がでけん
	};

	class reverse_iterator : public std::iterator<std::random_access_iterator_tag, value_type, difference_type> {
	public:
		typedef iterator					base_type;
		typedef reverse_iterator			self_type;
		typedef typename base_type::parent_type		parent_type;
		typedef typename base_type::reference_type	reference_type;
		typedef typename base_type::pointer_type		pointer_type;
		typedef typename base_type::size_type		size_type;

		self_type() : m_nPos(0),m_pStr(NULL) {}
		self_type(const self_type& i) : m_nPos(i.get()), m_pStr(i.GetStringPtr()) {}
		self_type(const base_type& i) : m_nPos(i.get()), m_pStr(i.GetStringPtr()) {}
		self_type(size_type n, parent_type* pStr) : m_pStr(pStr) { set(n); }

		//  base
		base_type base() const {
			return base_type(get(), GetStringPtr());
		}

		//	アクセッサ
		parent_type * GetStringPtr() const { return m_pStr; }
		size_type get() const { return m_nPos; }
		void set(size_type n) {
			size_type len=length();
			if(n==npos || len<n)m_nPos=len;
			else m_nPos = n;
		}

		//	*
		reference_type operator* () const {
			parent_type* p = GetStringPtr();
			if ( p==NULL ) {
				throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::reverse_iterator::operator* () const");
			}
			return (*p)[m_nPos-1];
		}
		pointer_type operator-> () const { return &(operator*()); }

		//	演算子
		self_type& operator+= (size_type n){
			if (m_nPos < n) m_nPos = 0;
			else set(m_nPos-n);
			return *this;
		}
		self_type& operator-= (size_type n){
			if(n==npos)set(npos);
			else set(m_nPos+n);
			return *this;
		}
		///	加減算のためのオペレータ
		self_type& operator++() { inc(); return (*this); }
		self_type operator++(int) { self_type i = *this; inc(); return i; }
		self_type& operator--() { dec(); return (*this); }
		self_type operator--(int) { self_type i = *this; dec(); return i; }
		self_type operator+(size_type _N) const { self_type i = *this; return i+=_N; }
		self_type operator-(size_type _N) const { self_type i = *this; return i-=_N; }
		difference_type operator-(const self_type & it) const { return it.m_nPos-m_nPos; }

		//	比較オペレータ
		bool operator== (const self_type& it) const { return (m_nPos == it.m_nPos) && (m_pStr == it.m_pStr); }
		bool operator!= (const self_type& it) const { return !(*this == it); }
		//	size_typeとの比較も用意しないとstring::nposと比較できない
		bool operator== (size_type size) const { if(size==npos)size=length(); return m_nPos == size; }
		bool operator!= (size_type size) const { return !(*this == size); }
		reference_type operator[] (difference_type n) const { return *((*this)+n); }

		void dec(){ set(m_nPos+1); }
		void inc(){ if(0<m_nPos)set(m_nPos-1); }
		size_type length() const { return m_pStr!=0?m_pStr->size():0; }
	protected:
		size_type		m_nPos;	//	0オリジンなポジションで保存するとしよう
		parent_type*	m_pStr; // 親へのポインタが無いと加減算のときにendの判定がでけん
	};

	class const_reverse_iterator : public std::iterator<std::random_access_iterator_tag, value_type, difference_type> {
	public:
		typedef const_iterator				base_type;
		typedef const_reverse_iterator		self_type;
		typedef typename base_type::parent_type		parent_type;
		typedef typename base_type::reference_type	reference_type;
		typedef typename base_type::pointer_type		pointer_type;
		typedef typename base_type::size_type		size_type;

		self_type() : m_nPos(0),m_pStr(NULL) {}
		self_type(const self_type& i) : m_nPos(i.get()), m_pStr(i.GetStringPtr()) {}
		self_type(const base_type& i) : m_nPos(i.get()), m_pStr(i.GetStringPtr()) {}
		self_type(const reverse_iterator& i)  : m_nPos(i.get()), m_pStr(i.GetStringPtr()) {}
		self_type(const iterator& i)  : m_nPos(i.get()), m_pStr(i.GetStringPtr()) {}
		self_type(size_type n, const parent_type* pStr) : m_pStr(pStr) { set(n); }

		//  base
		base_type base() const {
			return base_type(get(), GetStringPtr());
		}

		//	アクセッサ
		parent_type * GetStringPtr() const { return m_pStr; }
		size_type get() const { return m_nPos; }
		void set(size_type n) {
			size_type len=length();
			if(n==npos || len<n)m_nPos=len;
			else m_nPos = n;
		}

		//	*
		reference_type operator* () const {
			parent_type* p = GetStringPtr();
			if ( p==NULL ) {
				throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::const_reverse_iterator::operator* () const");
			}
			return (*p)[m_nPos-1];
		}
		pointer_type operator-> () const { return &(operator*()); }

		//	演算子
		self_type& operator+= (size_type n){
			if(n==npos)set(npos);
			else set(m_nPos+n);
			return *this;
		}
		self_type& operator-= (size_type n){
			if (m_nPos < n) m_nPos = 0;
			else set(m_nPos-n);
			return *this;
		}
		///	加減算のためのオペレータ
		self_type& operator++() { inc(); return (*this); }
		self_type operator++(int) { self_type i = *this; inc(); return i; }
		self_type& operator--() { dec(); return (*this); }
		self_type operator--(int) { self_type i = *this; dec(); return i; }
		self_type operator+(size_type _N) const { self_type i = *this; return i+=_N; }
		self_type operator-(size_type _N) const { self_type i = *this; return i-=_N; }
		difference_type operator-(const self_type & it) const { return it.m_nPos-m_nPos; }

		//	比較オペレータ
		bool operator== (const self_type& it) const { return (m_nPos == it.m_nPos) && (m_pStr == it.m_pStr); }
		bool operator!= (const self_type& it) const { return !(*this == it); }
		//	size_typeとの比較も用意しないとstring::nposと比較できない
		bool operator== (size_type size) const { if(size==npos)size=length(); return m_nPos == size; }
		bool operator!= (size_type size) const { return !(*this == size); }
		reference_type operator[] (difference_type n) const { return *((*this)+n); }

		void dec(){ set(m_nPos+1); }
		void inc(){ if(0<m_nPos)set(m_nPos-1); }
		size_type length() const { return m_pStr!=0?m_pStr->size():0; }
	protected:
		size_type		m_nPos;	//	0オリジンなポジションで保存するとしよう
		const parent_type*	m_pStr; // 親へのポインタが無いと加減算のときにendの判定がでけん
	};


//#!コンストラクタ・デストラクタ
	///	空の文字列を内容として初期化します
	explicit basic_string(const allocator_type& a = allocator_type())
		: m_Allocator(a), m_pszBuffer(NULL), m_nLength(0), m_nCapacity(0)
	{
	}

	///	sが指す文字列のうち、頭からlen文字分の文字列を内容として初期化します
	basic_string(const char_type* s, size_type len, const allocator_type& a = allocator_type())
		: m_Allocator(a), m_pszBuffer(NULL), m_nLength(0), m_nCapacity(0)
	{
		assign(psz, len);
	}

	///	文字列sを内容として初期化します
	basic_string(const self_type& s)
		: m_Allocator(s.get_allocator()), m_pszBuffer(NULL), m_nLength(0), m_nCapacity(0)
	{
		assign(s);
	}
	///	\0終端文字列sを内容として初期化します
	basic_string(const char_type* s, const allocator_type& a = allocator_type())
		: m_Allocator(a), m_pszBuffer(NULL), m_nLength(0), m_nCapacity(0)
	{
		assign(s);
	}
	///	文字chを1個並べた文字列を内容として初期化します
	basic_string(const char_type ch, const allocator_type& a = allocator_type())
		: m_Allocator(a), m_pszBuffer(NULL), m_nLength(0), m_nCapacity(0)
	{
		assign(1, ch);
	}
	///	文字chをlen個並べた文字列を内容として初期化します
	basic_string(size_type len, char_type ch, const allocator_type& a = allocator_type())
		: m_Allocator(a), m_pszBuffer(NULL), m_nLength(0), m_nCapacity(0)
	{
		assign(len, ch);
	}
	///	[first,last)の範囲の文字列を内容として初期化します
	template <class InputIterator>
	basic_string(InputIterator first, InputIterator last, const allocator_type& a = allocator_type())
		: m_Allocator(a), m_pszBuffer(NULL), m_nLength(0), m_nCapacity(0)
	{
		assign(first, last);
	}
	///	std::basic_string<E,T,A>型のオブジェクトsが保持する文字列を内容として初期化します
	basic_string(const std::basic_string<E,T,A>& s)
		: m_Allocator(s.get_allocator()), m_pszBuffer(NULL), m_nLength(0), m_nCapacity(0)
	{
		assign(s.begin(), s.end());
	}
	///	デストラクトします(非virtual関数)
	~basic_string() {
		_Clear();
	}

//#!標準STLのstd::stringとの変換
	///	std::basic_string<E,T,A>型のオブジェクトsが保持する文字列を内容として初期化します 
	self_type& operator= (const std::basic_string<E,T,A>& s) {
		assign(s.begin(), s.end());
		return *this;
	}
	///	保持している文字列の内容でstd::basic_string<E,T,A>型のオブジェクトを生成して返します
	operator const std::basic_string<E,T,A> () const {
		return std::basic_string<E,T,A>(c_str(), size());
	}

//#!operator=
	self_type& operator= (const self_type& s){
		assign(s);
		return *this;
	}
	self_type& operator= (const char_type* s){
		assign(s);
		return *this;
	}
	self_type& operator= (char_type ch){
		assign(1, ch);
		return *this;
	}

//#!operator+=
	self_type& operator+= (const self_type & s){
		append(s);
		return *this;
	}
	self_type& operator+= (const char_type * str){
		append(str);
		return *this;
	}
	self_type& operator+= (char_type ch){
		append(1, ch);
		return *this;
	}

//#!operator+
	self_type operator+ (const self_type& s) const {
		self_type tmp;
		tmp.reserve(size() + s.size() + 1);
		tmp.append(*this);
		tmp.append(s);
		return tmp;
	}

//#!比較演算子
	bool	operator== (const self_type& s) const {
		return (compare(s)==0);
	}
	bool	operator!= (const self_type& s) const {
		return !(*this==s);
	}
	bool	operator< (const self_type& s) const {
		return (compare(s)<0);
	}
	bool	operator> (const self_type& s) const {
		return s < *this;
	}
	bool	operator<= (const self_type& s) const {
		return !(*this > s);
	}
	bool	operator>= (const self_type& s) const {
		return !(*this < s);
	}

//#!operator[]
	///	n文字目への参照を返します
	reference operator[] (size_type n) {
		pointer p = getBuffer();
		if ( p==NULL ) return *((pointer)&char_null);
		return *(p+n);
	}
	///	n文字目へのconst参照を返します
	const_reference operator[] (size_type n) const {
		pointer p = getBuffer();
		if ( p==NULL ) return char_null;
		return *(p+n);
	}

//#!at
	///	n文字目への参照を返します
	///	nが範囲外の場合はindex_out_of_bounds_exception例外をthrowします
	reference at (size_type n) {
		if (n<0 || size()<n) {
			throw detail::index_out_of_bounds_exception("in YTL::YTLstring::basic_string::at(size_type n)");
		}
		return (*this)[n];
	}
	///	n文字目へのconst参照を返します
	///	nが範囲外の場合はindex_out_of_bounds_exception例外をthrowします
	const_reference at (size_type n) const {
		if (n<0 || size()<n) {
			throw detail::index_out_of_bounds_exception("in YTL::YTLstring::basic_string::at(size_type n) const");
		}
		return (*this)[n];
	}

//#!append
	///	sが指す文字列のうち、頭からlen文字分の文字列を末尾に追加します
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	self_type& append(const char_type* s, size_type len) {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::append(const char_type* s, size_type len)");
		}
		_Insert(s, len, size());
		return *this;
	}
	///	文字列sを末尾に追加します
	self_type& append(const self_type& s) {
		if ( s.size()!=0 ) {
			_Insert(s.c_str(), s.size(), size());
		}
		return *this;
	}
	///	文字列sのpos文字目からlen文字分を末尾に追加します
	///	posが範囲外の場合はindex_out_of_bounds_exception例外をthrowします
	self_type& append(const self_type& s, size_type pos, size_type len) {
		if ( s.size()<pos ) {
			throw detail::index_out_of_bounds_exception("in YTL::YTLstring::basic_string::append(const self_type& s, size_type pos, size_type len)");
		}
		_Insert(s.c_str()+pos, detail::_min(len, s.size()-pos), size());
		return *this;
	}
	///	\0終端文字列sを末尾に追加します
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	self_type& append(const char_type* s) {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::append(const char_type* s)");
		}
		const size_type len = traits_type::length(s);
		_Insert(s, len, size());
		return *this;
	}
	///	文字chをlen個並べた文字列を末尾に追加します
	self_type& append(size_type len, char_type ch) {
		_InsertChar(ch, len, size());
		return *this;
	}
	///	[first,last)の範囲の文字列を末尾に追加します
	template<class InputIterator>
	self_type& append(InputIterator first, InputIterator last) {
		const size_type len = std::distance(first, last);
		_Insert(first, len, size());
		return *this;
	}

//#!assign
	///	sが指す文字列のうち、頭からlen文字分の文字列を割り当てます
	self_type& assign(const char_type* s, size_type len) {
		if ( len==0 ) {
			_Clear();
		} else {
			_MakeBuffer(len);
			erase();
			_Insert(s,len,0);
		}
		return *this;
	}
	///	文字列sを割り当てます
	self_type& assign(const self_type& s) {
		//	s = s;のような自己代入に対してセーフでなければならない
		if ( this==&s ) return *this; // コピーする必要なし！
		assign(s.c_str(), s.size());
		return *this;
	}
	///	文字列sのpos文字目からlen文字分を割り当てます
	///	posが範囲外の場合はindex_out_of_bounds_exception例外をthrowします
	self_type& assign(const self_type& s, size_type pos, size_type n) {
		if ( s.size()<pos ) {	//	if ( pos==npos || s.size()<=pos ) {
			//assign(NULL, 0);
			//return *this;
			throw detail::index_out_of_bounds_exception("in YTL::YTLstring::basic_string::assign(const self_type& s, size_type pos, size_type n)");
		}
		//if ( n == npos || s.size()<pos+n ) {
		//	n = s.size() - pos;
		//}
		assign(s.c_str()+pos, detail::_min(n, s.size() - pos));
		return *this;
	}
	///	文字列sを割り当てます
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	self_type& assign(const char_type* s) {
		if ( s==NULL ) {
			//assign(s, 0);
			//return *this;
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::assign(const char_type* s)");
		}
		const size_type len = traits_type::length(s);
		assign(s, len);
		return *this;
	}
	///	文字chをlen個並べた文字列を割り当てます
	self_type& assign(size_type len, char_type ch) {
		if ( len==0 ) {
			_Clear();
		} else {
			_MakeBuffer(len);
			erase();
			_InsertChar(ch, len, 0);
		}
		return *this;
	}
	///	[first,last)の範囲の文字列を割り当てます
	template<class InputIterator>
	self_type& assign(InputIterator first, InputIterator last) {
		const size_type len = std::distance(first, last);
		if ( len==0 ) {
			_Clear();
			return *this;
		} else {
			_MakeBuffer(len);
			erase();
			_Insert(first, len, 0);
		}
		return *this;
	}

//#!compare
	///	自身の文字列と、文字列sを比較します
	///	等しい時は0、そうでない時は0以外を返します
	int compare(const self_type& s) const {
		return _Compare(c_str(), size(), s.c_str(), s.size());
	}
	///	自身のpos文字目からlen文字分の文字列と、文字列sを比較します
	///	等しい時は0、そうでない時は0以外を返します
	///	posが範囲外の場合はindex_out_of_bounds_exception例外をthrowします
	int compare(size_type pos, size_type len, const self_type& s) const {
		if ( size()<=pos ) {
			//if ( s.size() ) return -1;
			//return 0;
			throw detail::index_out_of_bounds_exception("in YTL::YTLstring::basic_string::compare(size_type pos, size_type len, const self_type& s) const");
		}
		return _Compare(c_str()+pos, detail::_min(len, size() - pos), s.c_str(), s.size());
	}
	///	自身のpos文字目からlen文字分の文字列と、文字列sのpos2文字目からlen2文字分の文字列を比較します
	///	等しい時は0、そうでない時は0以外を返します
	///	pos/pos2が範囲外の場合はindex_out_of_bounds_exception例外をthrowします
	int compare(size_type pos, size_type len, const self_type& s, size_type pos2, size_type len2) const {
		//if ( size()<=pos ) {
		//	if ( s.size()!=0 ) return -1;
		//	return 0;
		//}
		//if ( s.size()<=pos2 ) {
		//	return 1;
		//}
		if ( size()<=pos || s.size()<=pos2 ) {
			throw detail::index_out_of_bounds_exception("in YTL::YTLstring::basic_string::compare(size_type pos, size_type len, const self_type& s, size_type pos2, size_type len2) const");
		}
		return _Compare(c_str()+pos, detail::_min(len, size() - pos), s.c_str()+pos2, detail::_min(len2, s.size() - pos2));
	}
	///	自身の文字列と、文字列sを比較します
	///	等しい時は0、そうでない時は0以外を返します
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	int compare(const char_type* s) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::compare(const char_type* s) const");
		}
		const size_type len = traits_type::length(s);
		return _Compare(c_str(), size(), s, len);
	}
	///	自身のpos文字目からlen文字分の文字列と、文字列sのpos2文字目以降の文字列を比較します
	///	等しい時は0、そうでない時は0以外を返します
	///	posが範囲外の場合はindex_out_of_bounds_exception例外をthrowします
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	int compare(size_type pos, size_type len, const char_type* s, size_type pos2) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::compare(size_type pos, size_type len, const char_type* s, size_type pos2) const");
		}
		if ( size()<=pos ) {
			//if(s.size())return -1;
			//return 0;
			throw detail::index_out_of_bounds_exception("in YTL::YTLstring::basic_string::compare(size_type pos, size_type len, const char_type* s, size_type pos2) const");
		}
		return _Compare(c_str()+pos, detail::_min(len, size() - pos), s, len2);
	}

//#!substr
	///	pos文字目からlen文字分の文字列を生成して返します
	///	posが範囲外の場合はindex_out_of_bounds_exception例外をthrowします
	self_type substr(size_type pos = 0, size_type len = npos) const {
		if ( size()<=pos ) {
			throw detail::index_out_of_bounds_exception("in YTL::YTLstring::basic_string::substr(size_type pos = 0,size_type n = npos) const");
		}
		return self_type().assign(*this, pos, len);
	}

//#!erase
	///	[first,last)の範囲の文字を削除します
	iterator erase(iterator first, iterator last) {
		// first←begin()　＆　last←end() のとき、文字列をまるごと消す
		_Erase(first.get(), last.get() - first.get());
		return first;
	}
	///	pos文字目からlen文字分を削除します
	self_type& erase(size_type pos = 0, size_type len = npos) {
		// pos←0　＆　len←npos のとき、文字列をまるごと消す
		_Erase(pos, len);
		return *this;
	}
	///	itが指す文字を削除します
	iterator erase(iterator it) {
		_Erase(it.get(), 1);
		return it;
	}

//#!copy
	///	バッファsに、自身のpos文字目からlen文字分の文字列をコピーし、コピーした文字数を返します
	///	posが範囲外の場合はindex_out_of_bounds_exception例外をthrowします
	size_type copy(char_type* s, size_type len, size_type pos=0) const {
		if ( size()<=pos ) {
			throw detail::index_out_of_bounds_exception("in YTL::YTLstring::basic_string::copy(char_type* s, size_type len, size_type pos=0) const");
		}
		//if (len == npos || size()<len || size()<pos+len)len=size()-pos;	//	detail::_min(len, size()-pos)でええやん
		_NCopy(s, c_str()+pos, detail::_max(detail::_min(len, size()-pos), 0));
		return len;
	}

//#!c_str()とdata()
	///	\0終端文字列を返します。（stringが'\0'を含む場合は、'\0'の手前までがC言語文字として正常に認識されます）
	const char_type* c_str() const {
		const char_type* p = getBuffer();
		return (p!=NULL) ? p : &char_null;	//	0文字の場合でも必ず確保してますんで・・・
	}
	///	内部データの先頭文字を示すポインタを返します。
	const char_type* data() const {
		return c_str();
	}

//#!find
	///	pos文字目から順方向に検索します（対象：文字列sの先頭からlen文字分）
	///	対象が見つかったとき、その位置をゼロオリジンで返します
	///	見つからない時はnposを返します
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	size_type find(const char_type* s, size_type pos, size_type len) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::find(const char_type* s, size_type pos, size_type len) const");
		}
		const size_type _size = size();
		if ( len==0 && pos<=_size ) return pos;		//	範囲内なら空文字は常にマッチせなあかんらしい
		if ( pos<_size && len<=_size-pos ) {
			const_pointer _first = getBuffer();
			const_pointer _last = _first + _size - len + 1;
			const_pointer _it = _first + pos;
			while ( _it != _last ) {	//	char_traits<E>::searchもあって然るべきだと思うが…
				if ( traits_type::eq(*_it, *s) ) {
					if ( 0==traits_type::compare(_it, s, len) ) { return (_it - _first); }
				}
				++_it;
			}
		}

		return npos;
	}
	///	pos文字目以降を順方向に検索します（対象：文字列s全体）
	///	対象が見つかったとき、その位置をゼロオリジンで返します
	///	見つからない時はnposを返します
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	size_type find(const char_type* s, size_type pos = 0) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::find(const char_type* s, size_type pos = 0) const");
		}
		const size_type len = traits_type::length(s);	//	こいつnullチェックしてない…
		return find(s, pos, len);
	}
	///	pos文字目以降を順方向に検索します（対象：文字列s全体）
	///	対象が見つかったとき、その位置をゼロオリジンで返します
	///	見つからない時はnposを返します
	size_type find(const basic_string& s, size_type pos = 0) const{
		return find(s.c_str(), pos, s.size());
	}
	///	pos文字目以降を順方向に検索します（対象：文字ch）
	///	対象が見つかったとき、その位置をゼロオリジンで返します
	///	見つからない時はnposを返します
	size_type find(char_type ch, size_type pos = 0) const{
		return find_first_of(ch, pos);
	}

//#!rfind
	///	pos文字目以前を逆方向に検索します（対象：文字列sの先頭からlen文字分）
	///	対象が見つかったとき、その位置をゼロオリジンで返します
	///	見つからない時はnposを返します
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	size_type rfind(const char_type* s, size_type pos, size_type len) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::rfind(const char_type* s, size_type pos, size_type len) const");
		}
		const size_type _size = size();
		if ( len==0 ) return detail::_min(pos, _size);
		if ( len <= _size ) {
			const_pointer _rev_first = _first + detail::_min(pos, _size - len);
			const_pointer _rev_last = getBuffer();
			for ( const_pointer _it = _rev_first; ; --_it; ) {
				if ( traits_type::eq(*_it, *s) ) {
					if ( 0==traits_type::compare(_it+1, s+1, len-1) ) { return (_it - _first); }
				}
				else if ( _it!=_rev_last ) return npos;
			}
		}
		return npos;
	}
	///	pos文字目以前を逆方向に検索します（対象：文字列s全体）
	///	対象が見つかったとき、その位置をゼロオリジンで返します
	///	見つからない時はnposを返します
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	size_type rfind(const char_type* s, size_type pos = npos) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::rfind(const char_type* s, size_type pos = npos) const");
		}
		const size_type len = traits_type::length(s);
		return rfind(s, pos, len);
	}
	///	pos文字目以前を逆方向に検索します（対象：文字列s全体）
	///	対象が見つかったとき、その位置をゼロオリジンで返します
	///	見つからない時はnposを返します
	size_type rfind(const basic_string& s, size_type pos = npos) const {
		return rfind(str.c_str(),pos,str.size());
	}
	///	pos文字目以前を逆方向に検索します（対象：文字ch）
	///	対象が見つかったとき、その位置をゼロオリジンで返します
	///	見つからない時はnposを返します
	size_type rfind(char_type ch, size_type pos = npos) const {
		return find_last_of(ch, pos);
	}

//#!find_first_of
	///	pos文字目以降を順方向に検索します（対象：文字列sの先頭からlen文字分の文字列に含まれる文字のいずれか）
	///	対象が見つかったとき、その位置をゼロオリジンで返します
	///	見つからない時はnposを返します
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	size_type find_first_of(const char_type* s, size_type pos, size_type len) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::find_first_of(const char_type* s, size_type pos, size_type len) const");
		}
		const size_type _size = size();
		if ( pos<_size ) {
			const_pointer _first = getBuffer();
			const_pointer _last = _first + _size - 1 + 1;
			const_pointer _it = _first + pos;
			while ( _it != _last ) {
				for ( size_type i=0; i<len; ++i ) {
					if ( traits_type::eq(*_it, s[i]) ) { return (_it - _first); }
				}		
				++_it;
			}
		}
		return npos;
	}
	///	pos文字目以降を順方向に検索します（対象：文字列sに含まれる文字のいずれか）
	///	対象が見つかったとき、その位置をゼロオリジンで返します
	///	見つからない時はnposを返します
	size_type find_first_of(const self_type& s, size_type pos=0) const {
		return find_first_of(s.c_str(), pos, s.size());
	}
	///	pos文字目以降を順方向に検索します（対象：文字列sに含まれる文字のいずれか）
	///	対象が見つかったとき、その位置をゼロオリジンで返します
	///	見つからない時はnposを返します
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	size_type find_first_of(const char_type* s, size_type pos=0) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::find_first_of(const char_type* s, size_type pos=0) const");
		}
		const size_type len = traits_type::length(s);
		return find_first_of(s, pos, len);
	}
	///	pos文字目以降を順方向に検索します（対象：文字ch）
	///	対象が見つかったとき、その位置をゼロオリジンで返します
	///	見つからない時はnposを返します
	size_type find_first_of(char_type ch, size_type pos=0) const {
		const size_type _size = size();
		if ( pos<_size ) {
			const_pointer _first = getBuffer();
			const_pointer _last = _first + _size;
			const_pointer _it = _first + pos;
			while ( _it != _last ) {
				if ( traits_type::eq(*_it, ch) ) { return (_it - _first); }
				++_it;
			}
		}
		return npos;
	}

//#!find_first_not_of
	///	pos文字目以降を順方向に検索します（対象：文字列sの先頭からlen文字分の文字列に含まれない文字）
	///	対象が見つかったとき、その位置をゼロオリジンで返します
	///	見つからない時はnposを返します
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	size_type find_first_not_of(const char_type* s, size_type pos, size_type len) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::find_first_not_of(const char_type* s, size_type pos, size_type len) const");
		}
		const size_type _size = size();
		if ( pos<size ) {
			const_pointer _first = getBuffer();
			const_pointer _last = _first + size - 1 + 1;
			const_pointer _it = _first + pos;
			while ( _it != _last ) {
				for ( size_type i=0; i<len; ++i ) {
					if ( !traits_type::eq(*_it, s[i]) ) { return (_it - _first); }
				}
				++_it;
			}
		}
		return npos;
	}
	///	pos文字目以降を順方向に検索します（対象：文字列sに含まれない文字）
	///	対象が見つかったとき、その位置をゼロオリジンで返します
	///	見つからない時はnposを返します
	size_type find_first_not_of(const self_type& s, size_type pos=0) const {
		return find_first_not_of(s.c_str(), pos, s.size());
	}
	///	pos文字目以降を順方向に検索します（対象：文字列sに含まれない文字）
	///	対象が見つかったとき、その位置をゼロオリジンで返します
	///	見つからない時はnposを返します
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	size_type find_first_not_of(const char_type* s, size_type pos=0) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::find_first_not_of(const char_type* s, size_type pos=0) const");
		}
		const size_type len = traits_type::length(s);
		return find_first_not_of(s, pos, len);
	}
	///	pos文字目以降を順方向に検索します（対象：文字ch以外の文字）
	///	対象が見つかったとき、その位置をゼロオリジンで返します
	///	見つからない時はnposを返します
	size_type find_first_not_of(char_type ch, size_type pos=0) const {
		const size_type _size = size();
		if ( pos<size ) {
			const_pointer _first = getBuffer();
			const_pointer _last = _first + size - 1 + 1;
			const_pointer _it = _first + pos;
			while ( _it != _last ) {
				if ( !traits_type::eq(*_it, ch) ) { return (_it - _first); }
				++_it;
			}
		}
		return npos;
	}

//#!find_last_of
	// sから始まるlen文字分の文字列に含まれるいずれかの文字を、stringのpos文字目以前から逆方向検索
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	size_type find_last_of(const char_type* s, size_type pos, size_type len) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::find_last_of(const char_type* s, size_type pos, size_type len) const");
		}
		const size_type _size = size();
		if ( 0<len && 0<_size ) {
			const_pointer _first = getBuffer();
			const_pointer _rev_first = _first + detail::_min(pos, _size - 1);
			for ( const_pointer _it = _rev_first; ; --_it; ) {
				if ( traits_type::find(s, len, *_it) ) { return (_it - _first); }
				else if ( _it==_first ) return npos;
			}
		}
		return npos;
	}
	// 文字列sに含まれるいずれかの文字を、stringのpos文字目以前から逆方向検索
	size_type find_last_of(const self_type& s, size_type pos=npos) const {
		return find_last_of(s.c_str(), pos, s.size());
	}
	// C言語文字列sに含まれるいずれかの文字を、stringのpos文字目以前から逆方向検索
	size_type find_last_of(const char_type* s, size_type pos=npos) const {
		if(s==NULL)return npos;
		const size_type len = traits_type::length(s);
		return find_last_of(s, pos, len);
	}
	// 文字cを、stringのpos文字目以前から逆方向検索
	size_type find_last_of(char_type ch, size_type pos=npos) const {
		const size_type _size = size();
		if ( 0<_size ) {
			const_pointer _first = getBuffer();
			const_pointer _rev_first = _first + detail::_min(pos, _size - 1);
			for ( const_pointer _it = _rev_first; ; --_it ) {
				if ( traits_type::eq(ch, *_it) ) { return (_it - _first); }
				else if ( _it==_first ) return npos;
			}
		}
		return npos;
	}

//#!find_last_not_of
	// sから始まるn文字分の文字列に含まれないいずれかの文字を、stringのpos文字目以前から逆方向検索
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	size_type find_last_not_of(const char_type* s, size_type pos, size_type len) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::find_last_not_of(const char_type* s, size_type pos, size_type len) const");
		}
		const size_type _size = size();
		if ( 0<_size ) {
			const_pointer _first = getBuffer();
			const_pointer _rev_first = _first + detail::_min(pos, _size - 1);
			for ( const_pointer _it = _rev_first; ; --_it ) {
				if ( !traits_type::find(s, len, *_it) ) { return (_it - _first); }
				else if ( _it==_first ) return npos;
			}
		}
		return npos;
	}
	// 文字列sに含まれないいずれかの文字を、stringのpos文字目以前から逆方向検索
	size_type find_last_not_of(const self_type & s, size_type pos=npos) const {
		return find_last_not_of(s.c_str(), pos, s.size());
	}
	// C言語文字列sに含まれないいずれかの文字を、stringのpos文字目以前から逆方向検索
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	size_type find_last_not_of(const char_type* s, size_type pos=npos) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::find_last_not_of(const char_type* s, size_type pos) const");
		}
		const size_type len = traits_type::length(s);
		return find_last_not_of(s, pos, len);
	}
	// 文字c以外の文字を、stringのpos文字目以前から逆方向検索
	size_type find_last_not_of(char_type ch, size_type pos=npos) const {
		const size_type _size = size();
		if ( 0<_size ) {
			const_pointer _first = getBuffer();
			const_pointer _rev_first = _first + detail::_min(pos, _size - 1);
			for ( const_pointer _it = _rev_first; ; --_it ) {
				if ( !traits_type::eq(ch, *_it) ) { return (_it - _first); }
				else if ( _it==_first ) return npos;
			}
		}
		return npos;
	}

//#!get_allocator
	// アロケータを返します
	allocator_type get_allocator() const {
		return m_Allocator;
	}

//#!insert
	// pos文字目にstrで始まるlen文字の文字列を挿入します
	self_type& insert(size_type pos, const char_type* s, size_type len) {
		_Insert(s, len, pos);
		return *this;
	}
	// pos文字目にC言語文字列sを挿入します
	///	sがNULLの場合はnull_pointer_exception例外をthrowします
	self_type& insert(size_type pos, const char_type* s) {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::insert(size_type pos, const char_type *str)");
		}
		const size_type len = traits_type::length(s);
		_Insert(s, len, pos);
		return *this;
	}
	// pos文字目に文字列strを挿入します
	self_type& insert(size_type pos, const self_type& s) {
		_Insert(s.c_str(), s.size(), pos);
		return *this;
	}
	// pos文字目にstrのindex2文字目からlen文字分の文字列を挿入します
	self_type& insert(size_type pos, const self_type& s, size_type pos2, size_type len) {
		const size_type _size = s.size();
		if ( pos2<_size() ) {
			_Insert(s.c_str()+pos2, detail::_min(len, _size-pos2), pos);
		}
		return *this;
	}
	// pos文字目に文字chをlen個並べた文字列を挿入します
	self_type& insert(size_type pos, size_type len, char_type ch) {
		_InsertChar(ch, len, pos);
		return *this;
	}
	// posの場所に文字chをlen個並べた文字列を挿入します
	void insert(iterator pos, size_type len, char_type ch) {
		_InsertChar(ch, len, pos.get());
		return *this;
	}
	// posの場所に文字chを挿入します
	iterator insert(iterator pos, char_type ch) {
		_InsertChar(ch, 1, pos.get());
		return pos;
	}
	// posの場所にstart～endの示す文字列を挿入します
	template<class InputIterator>
	void insert(iterator pos, InputIterator first, InputIterator last) {
		const size_type len = std::distance(first, last);
		_Insert(first, len, pos.get());
	}

//#!max_size
	///	最大の文字の長さを得ます
	size_type max_size() const{
		return get_allocator().max_size();
	}

//#!begin()とend()
	// 最初の文字のイタレータを返します（const）
	const_iterator begin() const { return const_iterator(0, this); }
	// 最初の文字のイタレータを返します
	iterator begin() { return iterator(0, this); }
	// 終端のイタレータを返します（const）
	const_iterator end() const { return const_iterator(npos, this); }
	// 終端のイタレータを返します
	iterator end() { iterator it(npos, this); return it; }

//#!rbegin()とrend()
	// 逆側から見て最初の文字のリバースイタレータを返します（const）
	const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	// 逆側から見て最初の文字のリバースイタレータを返します
	reverse_iterator rbegin() { return reverse_iterator(end()); }
	// 逆側から見て終端のリバースイタレータを返します（const）
	const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
	// 逆側から見て終端のリバースイタレータを返します
	reverse_iterator rend() { return reverse_iterator(begin()); }

//#!replace
	// pos文字目からlen文字分を、文字列strと置き換えます
	self_type& replace(size_type pos, size_type len, const self_type& s) {
		_Replace(s.c_str(), s.size(), pos, len);
		return *this;
	}
	// pos文字目からlen文字分を、文字列strのpos2文字目からlen2文字分と置き換えます
	self_type& replace(size_type pos, size_type len, const self_type& s, size_type pos2, size_type len2) {
		const size_type _size = s.size();
		if ( pos2==npos || _size<=pos2 ) {
			_Erase(pos, len);
		} else {
			_Replace(str.c_str()+pos2, detail::_min(len2, (_size-pos2)), pos, len);
		}
		return *this;
	}
	// pos文字目からlen文字分を、C言語文字列strと置き換えます
	self_type& replace(size_type pos, size_type len, const char_type* s) {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::replace(size_type pos, size_type len, const char_type* s)");
		}
		const size_type len2 = traits_type::length(s);
		_Replace(s, len2, pos, len);
		return *this;
	}
	// pos文字目からlen文字分を、len2個の文字chで置き換えます
	self_type& replace(size_type pos, size_type len, size_type len2, char_type ch) {
		_ReplaceChar(ch, len2, pos, len);
		return *this;
	}

	// start～endの区間を、文字列strで置き換えます
	self_type& replace(iterator start, iterator end, const self_type& s) {
		const size_type len = std::distance(first, last);
		_Replace(s.c_str(), s.size(), start.get(), len);
		return *this;
	}
	// start～endの区間を、C言語文字列strで置き換えます
	self_type& replace(iterator first, iterator last, const char_type* s) {
		const size_type len = std::distance(first, last);
		const size_type len2 = traits_type::length(s);
		_Replace(str.c_str(), len2, first.get(), len);

		return *this;
	}
	// start～endの区間を、len個の文字chで置き換えます
	self_type& replace(iterator first, iterator last, size_type len, char_type ch){
		const size_type len2 = traits_type::length(s);
		_ReplaceChar(ch, len, start.get(), len2);
		return *this;
	}
	// start～endの区間を、別コンテナのstart2～end2を示す文字列で置き換えます
	template<class InputIterator> self_type & replace(iterator first, iterator last, InputIterator first2, InputIterator last2){
		const size_type len = std::distance(first, last);
		const size_type len2 = std::distance(first2, last2);
		_Replace(first2, len2, first.get(), len);
		return *this;
	}

//#!reserve
	// len文字以上収納できるメモリを確保します
	void reserve(size_type len=0) {
		//size()+1<=capacity()なので比較は１つでOK
		_MakeBuffer(len);
		if ( len < capacity() ) {
		} else {	//	確保しなおし?
			pointer pBuf;
			size_type cap;
			myAlloc(len+1, pBuf, cap);

			pointer p = pBuf;
			_Copy(p, getBuffer(), size()+1);

			_Destroy(getBuffer(), size()+1);
			myFree(getBuffer());
			m_pszBuffer = pBuf;
			m_nCapacity = cap;
		}
	}

//#!resize
	// 文字列のサイズを変更します。長くなった部分は文字chで埋まります
	void resize(size_type len, char_type ch){
		if ( len<0 ) {
			_Clear();
			return;
		}
		else if ( len==size() ) return;

		_MakeBuffer(len);
		if ( len<size() ) {
			pointer p=m_pszBuffer+size()-len;
			_Copy(p,m_pszBuffer+size(), 1, m_pszBuffer+size()+1);
			_Destroy(m_pszBuffer+len+1, size()-len);
			m_nLength=len;
		} else {
			reserve(len);
			pointer p=m_pszBuffer+len;
			_CopyRev(p, m_pszBuffer+size(), 1, m_pszBuffer+size()+1);
			p=m_pszBuffer+size();
			_Fill(p, ch, len-size(), p+1);
			m_nLength=len;
		}
	}
	// 文字列のサイズを変更します
	void resize(size_type len){
		if ( len<0 ) {
			_Clear();
			return;
		}
		else if ( len==size() ) return;

		_MakeBuffer(len);
		if ( len<size() ) {
			pointer p=m_pszBuffer+size()-len;
			_Copy(p,m_pszBuffer+size(), 1, m_pszBuffer+size()+1);
			_Destroy(m_pszBuffer+len+1, size()-len);
			m_nLength=len;
		} else {
			resize(len, char_type());
		}
	}

//#!swap
	// ２つの文字列の中身を入れ替えます。（アドレスを入れ替えているので、アロケータも同時に入れ替えています）
	void swap(self_type& s) {
		std::swap(m_pszBuffer, s.m_pszBuffer);
		std::swap(m_nCapacity, s.m_nCapacity);
		std::swap(m_nLength, s.m_nLength);
		std::swap(m_Allocator, s.m_Allocator);
	}

//#!メンバ変数の習得
	size_type capacity() const { return m_nCapacity; }
	bool empty() const { return size()==0; }

	size_type	size() const {
		return m_nLength;
	}
	size_type	length() const {
		return size();
	}

protected:

//#!メンバ変数
	pointer			m_pszBuffer;
	size_type		m_nLength;
	//	nLength!=0 のとき、m_pszBufferは、
	//	sizeof(E) * (m_nLength+1)確保されている
	//				(↑+1なのは終端文字列があるので)
	size_type		m_nCapacity;	//	確保したサイズ(2^n単位で確保)
	allocator_type	m_Allocator;

	///	バッファを得る
	pointer getBuffer() const { return m_pszBuffer; }

//#!メモリ確保メソッド
	///	nSize : 確保するサイズ
	///	pBuf  : 確保したポインタ
	///	capa  : 実際に確保したサイズ(2^nでアラインされる)
	void myAlloc(size_type nSize,pointer&pBuf,size_type& capa) {
		//	2^nにアラインする
		size_type n = nSize;
		size_type c = 1;
		while ( n!=0 ) {
			n>>=1; c<<=1;
		}
		pBuf = m_Allocator.allocate(c,(pointer)0);//new E[c];
		capa = c;
	}

//#!メモリ解放メソッド
	///	myAllocで確保したメモリを解放する
	void myFree(pointer pBuf) {
		if ( pBuf!=NULL ) {
			m_Allocator.deallocate(pBuf, m_nCapacity);//delete [] pBuf;
		}
	}

protected:///////////////////////////////////////////////////////////
//インスタンス化・してるしてない領域へのコピー
	//最後のポインタins以降はインスタンス化していない。
	//template<class InputIterator> void _Copy(pointer& p,InputIterator start, InputIterator end, pointer ins=NULL){
	//	while(start!=end){
	//		if(p<ins)	*(p++)=*(start++);
	//		else		m_Allocator.construct(p++,*(start++));
	//	}
	//}
	template<class InputIterator> static void _NCopy(pointer& p,InputIterator start, size_type n){
		for(size_type i=0;i<n;i++){
			*(p++)=*(start++);
		}
	}
	template<class InputIterator> void _Copy(pointer& p,InputIterator start, size_type n, pointer ins=NULL){
		for(size_type i=0;i<n;i++){
			if(p<ins)	*(p++)=*(start++);
			else		m_Allocator.construct(p++,*(start++));
		}
	}
	template<class InputIterator> void _CopyRev(pointer& p,InputIterator start, size_type n, pointer ins=NULL){
		{for(size_type i=0;i<n;i++){
			p++;start++;
		}}
		pointer p2=p;
		{for(size_type i=0;i<n;i++){
			if(--p2<ins)	*(p2)=*(--start);
			else		m_Allocator.construct(p2,*(--start));
		}}
	}
	void _Fill(pointer& p,const_reference x, size_type n, pointer ins=NULL){
		for(size_type i=0;i<n;i++){
			if(p<ins)	*(p++)=x;
			else		m_Allocator.construct(p++,x);
		}
	}
//インスタンス化領域を解放
	void _Destroy(pointer s, size_type n){
		for(size_type i=0;i<n;++i){
			m_Allocator.destroy(s++);
		}
	}
//#! プライベートに隠蔽された本処理メソッド
	template<class InputIterator> void _Insert(InputIterator it, size_type len, size_type pos){
		if(len<=0)return;
		if(pos==npos || size()<pos)pos=size();

		_MakeBuffer(len);
		if(size()+len+1<=capacity()){
			pointer p=m_pszBuffer+pos+len;
			_CopyRev(p, m_pszBuffer+pos, size()-pos+1, m_pszBuffer+size()+1);
			p=m_pszBuffer+pos;
			_Copy(p, it, len, m_pszBuffer+size()+1);
		}
		else{
			pointer pBuf;
			size_type cap;
			myAlloc(size()+len+1,pBuf,cap);

			pointer p=pBuf;
			_Copy(p, m_pszBuffer, pos);
			_Copy(p, it, len);
			_Copy(p, m_pszBuffer+pos, size()-pos+1);

			_Destroy(m_pszBuffer, size()+1);
			myFree(m_pszBuffer);
			m_pszBuffer = pBuf;
			m_nCapacity = cap;
		}
		m_nLength+=len;
	}
	void _InsertChar(const_reference ch, size_type len, size_type pos){
		if(len<=0)return;
		if(pos==npos || size()<pos)pos=size();

		_MakeBuffer(len);
		if(size()+len+1<=capacity()){
			pointer p=m_pszBuffer+pos+len;
			_CopyRev(p, m_pszBuffer+pos, size()-pos+1, m_pszBuffer+size()+1);
			p=m_pszBuffer+pos;
			_Fill(p, ch, len, m_pszBuffer+size()+1);
		}
		else{
			pointer pBuf;
			size_type cap;
			myAlloc(size()+len+1,pBuf,cap);

			pointer p=pBuf;
			_Copy(p, m_pszBuffer, pos);
			_Fill(p, ch, len);
			_Copy(p, m_pszBuffer+pos, size()-pos+1);

			_Destroy(m_pszBuffer, size()+1);
			myFree(m_pszBuffer);
			m_pszBuffer = pBuf;
			m_nCapacity = cap;
		}
		m_nLength+=len;
	}
	template<class InputIterator> void _Replace(InputIterator it, size_type len, size_type pos, size_type len2){
		if(len<=0)return;
		if(pos==npos || size()<=pos){
			_Insert(it, len, pos);
			return;
		}
		if(len2==npos || size()<len2 || size()<len2+pos)len2=size()-pos;

		_MakeBuffer(len);
		if(len2<=len){
			if(size()+len-len2+1<=capacity()){
				if(len2!=len){
					pointer p=m_pszBuffer+pos+len;
					_CopyRev(p, m_pszBuffer+pos+len2, size()-pos-len2+1, m_pszBuffer+size()+1);
				}
				pointer p=m_pszBuffer+pos;
				_Copy(p, it, len, m_pszBuffer+size()+1);
			}
			else{
				pointer pBuf;
				size_type cap;
				myAlloc(size()+len-len2+1,pBuf,cap);

				pointer p=pBuf;
				_Copy(p, m_pszBuffer, pos);
				_Copy(p, it, len);
				_Copy(p, m_pszBuffer+pos+len2, size()-pos-len2+1);

				_Destroy(m_pszBuffer, size()+1);
				myFree(m_pszBuffer);
				m_pszBuffer = pBuf;
				m_nCapacity = cap;
			}
		}
		else{
			pointer p=m_pszBuffer+pos;
			_Copy(p, it, len, m_pszBuffer+size()+1);
			_Copy(p, m_pszBuffer+pos+len2, size()-pos-len2+1, m_pszBuffer+size()+1);
			_Destroy(m_pszBuffer+size()+len-len2+1, len2-len);
		}
		m_nLength+=len-len2;
	}

	void _ReplaceChar(const_reference ch, size_type len, size_type pos, size_type len2){
		if(len<=0)return;
		if(pos==npos || size()<=pos){
			_InsertChar(ch, len, pos);
			return;
		}
		if(len2==npos || size()<len2 || size()<len2+pos)len2=size()-pos;

		_MakeBuffer(len);
		if(len2<=len){
			if(size()+len-len2+1<=capacity()){
				pointer p;
				if(len2!=len){
					p=m_pszBuffer+pos+len;
					_CopyRev(p, m_pszBuffer+pos+len2, size()-pos-len2+1, m_pszBuffer+size()+1);
				}
				p=m_pszBuffer+pos;
				_Fill(p, ch, len, m_pszBuffer+size()+1);
			}
			else{
				pointer pBuf;
				size_type cap;
				myAlloc(size()+len-len2+1,pBuf,cap);

				pointer p=pBuf;
				_Copy(p, m_pszBuffer, pos);
				_Fill(p, ch, len);
				_Copy(p, m_pszBuffer+pos+len2, size()-pos-len2+1);

				_Destroy(m_pszBuffer, size()+1);
				myFree(m_pszBuffer);
				m_pszBuffer = pBuf;
				m_nCapacity = cap;
			}
		}
		else{
			pointer p=m_pszBuffer+pos;
			_Fill(p, ch, len, m_pszBuffer+size()+1);
			_Copy(p, m_pszBuffer+pos+len2, size()-pos-len2+1, m_pszBuffer+size()+1);
			_Destroy(m_pszBuffer+size()+len-len2+1, len2-len);
		}
		m_nLength+=len-len2;
	}

	void _Erase(size_type pos, size_type len){
		if(size()<=pos){
			return;
		}
		if(len==npos || size()<len || size()<len+pos){
			len=size()-pos;
			if(pos==0){
				_Clear();
				return;
			}
		}
		pointer p=m_pszBuffer+pos;
		_Copy(p, m_pszBuffer+pos+len, size()-pos-len+1, m_pszBuffer+size()+1);
		_Destroy(m_pszBuffer+size()-len+1, len);
		m_nLength-=len;
	}

	int _Compare(const_pointer str, size_type len, const_pointer str2, size_type len2) const {
		const int cmp = traits_type::compare(str, str2, detail::_min(len, len2));
		return cmp != 0 ? cmp : (len < len2 ? -1 : (len > len2 ? 1 : 0));
	}

	void _Clear() {
		if ( m_pszBuffer!=NULL ) {
			_Destroy(m_pszBuffer, size()+1);
			myFree(m_pszBuffer);
		}
		m_pszBuffer=NULL;
		m_nLength=0;
		m_nCapacity=0;
	}
	void _MakeBuffer(size_type len=0) {
		if ( m_pszBuffer==NULL ) {
			m_nLength = 0;
			myAlloc(len+1, m_pszBuffer, m_nCapacity);
			pointer p = m_pszBuffer;
			_Fill(p, char_type(), 1);
		}
	}

};	//	end of class basic_string<E,T,A>

//	オペレータ
template <class E,class T,class A>
basic_string<E,T,A>	operator+ (E* pBuf, const basic_string<E,T,A>&s2) {
	return basic_string<E,T,A>(pBuf) += s2;
}

template <class E,class T,class A>
basic_string<E,T,A>	operator+ (E ch, const basic_string<E,T,A>&s2) {
	return basic_string<E,T,A>(ch) += s2;
}

//*			// streamへの対応　間接版
template <class E,class T,class A>
std::basic_ostream<E,T> & operator<<(std::basic_ostream<E,T>& o, const basic_string<E,T,A>& s){
	std::basic_string<E,T,A> str(s.c_str(), s.size());
	o << str;
	return o;
}
template <class E,class T,class A>
std::basic_istream<E,T> & operator>>(std::basic_istream<E,T>& i, basic_string<E,T,A>& s){
	std::basic_string<E,T,A> str;
	i >> str;
	s.assign(str.begin(), str.end());
	return i;
}
template <class E,class T,class A>
std::basic_istream<E,T> & getline(std::basic_istream<E,T>& i, basic_string<E,T,A>& s, E delim){
	std::basic_string<E,T,A> str;
	getline(i, str, delim);
	s.assign(str.begin(), str.end());
	return i;
}
/*/			// streamへの対応　直接版
template <class E,class T,class A>
std::basic_ostream<E,T> & operator<<(std::basic_ostream<E,T>& o, const basic_string<E,T,A>& s){
	o.write(s.c_str(), (std::streamsize)s.size());
	return o;
}
template <class E,class T,class A>
std::basic_istream<E,T> & operator>>(std::basic_istream<E,T>& i, basic_string<E,T,A>& s){
	std::size_t w=i.width();
	std::ios_base::iostate st = std::ios_base::goodbit;
	s.erase();
	//s.reserve(w);
	for(T::int_type ch=i.rdbuf()->sgetc();--w || s.size()==0;ch=i.rdbuf()->snextc()){
		if(T::eq_int_type(T::eof(), ch)){
			st|=std::ios_base::eofbit;
			break;
		}
		if(std::use_facet<std::ctype<E> >(i.getloc()).is(std::ctype<E>::space, T::to_char_type(ch))){
			i.rdbuf()->sbumpc();
			break;
		}
		s.append(1, T::to_char_type(ch));
		if(s.max_size()<=s.size()){
			i.rdbuf()->sbumpc();
			break;
		}
	}
	if(s.size()==0)st|=std::ios_base::failbit;
	i.setstate(st);
	return i;
}
template <class E,class T,class A>
std::basic_istream<E,T> & getline(std::basic_istream<E,T>& i, basic_string<E,T,A>& s, E delim){
	std::size_t w=i.width();
	std::ios_base::iostate st = std::ios_base::goodbit;
	s.erase();
	//s.reserve(w);
	for(T::int_type ch=i.rdbuf()->sgetc();--w || s.size()==0;ch=i.rdbuf()->snextc()){
		if(T::eq_int_type(T::eof(), ch)){
			st|=std::ios_base::eofbit;
			break;
		}
		if(T::to_char_type(ch)==delim){
			i.rdbuf()->sbumpc();
			break;
		}
		s.append(1, T::to_char_type(ch));
		if(s.max_size()<=s.size()){
			i.rdbuf()->sbumpc();
			break;
		}
	}
	if(s.size()==0)st|=std::ios_base::failbit;
	i.setstate(st);
	return i;
}
//*/		// streamへの対応終わり。


template <class E,class T,class A>
std::basic_istream<E,T> & getline(std::basic_istream<E,T>& i, basic_string<E,T,A>& s){
	return getline(i, s, i.widen('\n'));
}

template<class E,class T, class A>
void swap(const basic_string<E,T,A> & lhs, const basic_string<E,T,A> & rhs){
	lhs.swap(rhs);
}

template<class E,class T,class A>
typename const basic_string<E,T,A>::size_type basic_string<E,T,A>::npos = (typename basic_string<E,T,A>::size_type)-1;
template<class E,class T,class A>
typename const basic_string<E,T,A>::char_type basic_string<E,T,A>::char_null = basic_string<E,T,A>::char_type();

typedef basic_string<char> string;

}	//	end of namespace YTLstring
}	//	end of namespace YTL
}	//	end of namespace yaneuraoGameSDK3rd

//	こんなしょーもないクラス作るのに丸一日かかったよヽ(`Д´)ノｳﾜｧｧｧｧｧﾝ

#endif
