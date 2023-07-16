/**
	simple string class
		STL��string class�́AVC++6��7(.NET)�Ŏ������قȂ�̂�
		���O�ŗp�ӂ���B
		�iSTLPort���������s�ςƂ͌���Ȃ��̂Łj

	implemented by yaneurao(M.Isozaki) '03/04/02
	implemented by coLun(Y.Imamura) '03/04/07 - '03/05/19

	'03/04/25�@���@substr�̕s�������
	'03/04/27�@���@rfind�̕s�������(�����������Ɏ��rfind)
	'03/05/01�@���@replace�̕s�������
	'03/05/12�@���@erase�̕s�������
	'03/05/12�@���@reverse_iterator�̕s�������
	'03/05/13�@���@rfind�̕s�������(������������Ɏ��rfind)
	'03/05/19�@���@iterator��const_iterator�̒�`�̌������iboost�Ή��j
	'03/06/20�@���@reverse_iterator��const_reverse_iterator�̌������iVC6�Ή��j

	UNICODE�ł����܂������Ƃ����Ȃ��D�D�R(�L�D�M)�m

	�Ȃ񂩏t�x�݂̏h����Ċ����D�D

	���낢�둫��Ȃ������o���邩���m��Ȃ����ǁA�ǉ�������A
	��˂��炨�܂Ń��[�����Ă���[�����i�O�O�G
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

//	�ʂ��( �L�́M)
struct null_pointer_exception : public std::exception
{
	null_pointer_exception(const char* str) : std::exception(str) {}
};
//	typedef�ɂ��Ƃ��܂����c
typedef std::out_of_range	index_out_of_bounds_exception;

//	VC6/VC7/STLPort�ŋ��ʂ̖��O���Ȃ��̂Ō��ǎ����Œ�`���Ȃ��Ƃ����Ȃ�
template<class T> inline
const T& _max(const T& _X, const T& _Y) { return (_X < _Y ? _Y : _X); }
template<class T> inline
const T& _min(const T& _X, const T& _Y) { return (_Y < _X ? _Y : _X); }

}	//	end of namespace detail

template <class E, class T = std::char_traits<E>, class A = YTLallocator::allocator<E> >
class basic_string
{/**
	STL�݊�string�N���X(���Ȃ肵��ڂ�)
	���ɒP���Ȏ��������Ǌ��ق��Ă���D�D(��_�G)�
*/
public:
//#!��`���i�C�^���[�^�Ȃǂ��j
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

		//	�A�N�Z�b�T
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

		//	���Z�q
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
		///	�����Z�̂��߂̃I�y���[�^
		self_type& operator++() { inc(); return (*this); }
		self_type operator++(int) { self_type i = *this; inc(); return i; }
		self_type& operator--() { dec(); return (*this); }
		self_type operator--(int) { self_type i = *this; dec(); return i; }
		self_type operator+(size_type _N) const { self_type i = *this; return i+=_N; }
		self_type operator-(size_type _N) const { self_type i = *this; return i-=_N; }
		difference_type operator-(const self_type & it) const { return m_nPos-it.m_nPos; }

		//	��r�I�y���[�^
		bool operator== (const self_type& it) const { return (m_nPos == it.m_nPos) && (m_pStr == it.m_pStr); }
		bool operator!= (const self_type& it) const { return !(*this == it); }
		//	size_type�Ƃ̔�r���p�ӂ��Ȃ���string::npos�Ɣ�r�ł��Ȃ�
		bool operator== (size_type size) const { if(size==npos)size=length(); return m_nPos == size; }
		bool operator!= (size_type size) const { return !(*this == size); }
		reference_type operator[] (difference_type n) const { return *((*this)+n); }

		void inc(){ set(m_nPos+1); }
		void dec(){ if(0<m_nPos)set(m_nPos-1); }
		size_type length() const { return m_pStr!=0?m_pStr->size():0; }
	protected:
		size_type		m_nPos;	//	0�I���W���ȃ|�W�V�����ŕۑ�����Ƃ��悤
		parent_type*	m_pStr; // �e�ւ̃|�C���^�������Ɖ����Z�̂Ƃ���end�̔��肪�ł���
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

		//	�A�N�Z�b�T
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

		//	���Z�q
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
		///	�����Z�̂��߂̃I�y���[�^
		self_type& operator++() { inc(); return (*this); }
		self_type operator++(int) { self_type i = *this; inc(); return i; }
		self_type& operator--() { dec(); return (*this); }
		self_type operator--(int) { self_type i = *this; dec(); return i; }
		self_type operator+(size_type _N) const { self_type i = *this; return i+=_N; }
		self_type operator-(size_type _N) const { self_type i = *this; return i-=_N; }
		difference_type operator-(const self_type& it) const { return m_nPos-it.m_nPos; }

		//	��r�I�y���[�^
		bool operator== (const self_type& it) const { return (m_nPos == it.m_nPos) && (m_pStr == it.m_pStr); }
		bool operator!= (const self_type& it) const { return !(*this == it); }
		//	size_type�Ƃ̔�r���p�ӂ��Ȃ���string::npos�Ɣ�r�ł��Ȃ�
		bool operator== (size_type size) const { if(size==npos)size=length(); return m_nPos == size; }
		bool operator!= (size_type size) const { return !(*this == size); }
		reference_type operator[] (difference_type n) const { return *((*this)+n); }

		void inc(){ set(m_nPos+1); }
		void dec(){ if(0<m_nPos)set(m_nPos-1); }
		size_type length() const { return m_pStr!=0?m_pStr->size():0; }
	protected:
		size_type		m_nPos;	//	0�I���W���ȃ|�W�V�����ŕۑ�����Ƃ��悤
		const parent_type*	m_pStr; // �e�ւ̃|�C���^�������Ɖ����Z�̂Ƃ���end�̔��肪�ł���
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

		//	�A�N�Z�b�T
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

		//	���Z�q
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
		///	�����Z�̂��߂̃I�y���[�^
		self_type& operator++() { inc(); return (*this); }
		self_type operator++(int) { self_type i = *this; inc(); return i; }
		self_type& operator--() { dec(); return (*this); }
		self_type operator--(int) { self_type i = *this; dec(); return i; }
		self_type operator+(size_type _N) const { self_type i = *this; return i+=_N; }
		self_type operator-(size_type _N) const { self_type i = *this; return i-=_N; }
		difference_type operator-(const self_type & it) const { return it.m_nPos-m_nPos; }

		//	��r�I�y���[�^
		bool operator== (const self_type& it) const { return (m_nPos == it.m_nPos) && (m_pStr == it.m_pStr); }
		bool operator!= (const self_type& it) const { return !(*this == it); }
		//	size_type�Ƃ̔�r���p�ӂ��Ȃ���string::npos�Ɣ�r�ł��Ȃ�
		bool operator== (size_type size) const { if(size==npos)size=length(); return m_nPos == size; }
		bool operator!= (size_type size) const { return !(*this == size); }
		reference_type operator[] (difference_type n) const { return *((*this)+n); }

		void dec(){ set(m_nPos+1); }
		void inc(){ if(0<m_nPos)set(m_nPos-1); }
		size_type length() const { return m_pStr!=0?m_pStr->size():0; }
	protected:
		size_type		m_nPos;	//	0�I���W���ȃ|�W�V�����ŕۑ�����Ƃ��悤
		parent_type*	m_pStr; // �e�ւ̃|�C���^�������Ɖ����Z�̂Ƃ���end�̔��肪�ł���
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

		//	�A�N�Z�b�T
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

		//	���Z�q
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
		///	�����Z�̂��߂̃I�y���[�^
		self_type& operator++() { inc(); return (*this); }
		self_type operator++(int) { self_type i = *this; inc(); return i; }
		self_type& operator--() { dec(); return (*this); }
		self_type operator--(int) { self_type i = *this; dec(); return i; }
		self_type operator+(size_type _N) const { self_type i = *this; return i+=_N; }
		self_type operator-(size_type _N) const { self_type i = *this; return i-=_N; }
		difference_type operator-(const self_type & it) const { return it.m_nPos-m_nPos; }

		//	��r�I�y���[�^
		bool operator== (const self_type& it) const { return (m_nPos == it.m_nPos) && (m_pStr == it.m_pStr); }
		bool operator!= (const self_type& it) const { return !(*this == it); }
		//	size_type�Ƃ̔�r���p�ӂ��Ȃ���string::npos�Ɣ�r�ł��Ȃ�
		bool operator== (size_type size) const { if(size==npos)size=length(); return m_nPos == size; }
		bool operator!= (size_type size) const { return !(*this == size); }
		reference_type operator[] (difference_type n) const { return *((*this)+n); }

		void dec(){ set(m_nPos+1); }
		void inc(){ if(0<m_nPos)set(m_nPos-1); }
		size_type length() const { return m_pStr!=0?m_pStr->size():0; }
	protected:
		size_type		m_nPos;	//	0�I���W���ȃ|�W�V�����ŕۑ�����Ƃ��悤
		const parent_type*	m_pStr; // �e�ւ̃|�C���^�������Ɖ����Z�̂Ƃ���end�̔��肪�ł���
	};


//#!�R���X�g���N�^�E�f�X�g���N�^
	///	��̕��������e�Ƃ��ď��������܂�
	explicit basic_string(const allocator_type& a = allocator_type())
		: m_Allocator(a), m_pszBuffer(NULL), m_nLength(0), m_nCapacity(0)
	{
	}

	///	s���w��������̂����A������len�������̕��������e�Ƃ��ď��������܂�
	basic_string(const char_type* s, size_type len, const allocator_type& a = allocator_type())
		: m_Allocator(a), m_pszBuffer(NULL), m_nLength(0), m_nCapacity(0)
	{
		assign(psz, len);
	}

	///	������s����e�Ƃ��ď��������܂�
	basic_string(const self_type& s)
		: m_Allocator(s.get_allocator()), m_pszBuffer(NULL), m_nLength(0), m_nCapacity(0)
	{
		assign(s);
	}
	///	\0�I�[������s����e�Ƃ��ď��������܂�
	basic_string(const char_type* s, const allocator_type& a = allocator_type())
		: m_Allocator(a), m_pszBuffer(NULL), m_nLength(0), m_nCapacity(0)
	{
		assign(s);
	}
	///	����ch��1���ׂ����������e�Ƃ��ď��������܂�
	basic_string(const char_type ch, const allocator_type& a = allocator_type())
		: m_Allocator(a), m_pszBuffer(NULL), m_nLength(0), m_nCapacity(0)
	{
		assign(1, ch);
	}
	///	����ch��len���ׂ����������e�Ƃ��ď��������܂�
	basic_string(size_type len, char_type ch, const allocator_type& a = allocator_type())
		: m_Allocator(a), m_pszBuffer(NULL), m_nLength(0), m_nCapacity(0)
	{
		assign(len, ch);
	}
	///	[first,last)�͈̔͂̕��������e�Ƃ��ď��������܂�
	template <class InputIterator>
	basic_string(InputIterator first, InputIterator last, const allocator_type& a = allocator_type())
		: m_Allocator(a), m_pszBuffer(NULL), m_nLength(0), m_nCapacity(0)
	{
		assign(first, last);
	}
	///	std::basic_string<E,T,A>�^�̃I�u�W�F�N�gs���ێ����镶�������e�Ƃ��ď��������܂�
	basic_string(const std::basic_string<E,T,A>& s)
		: m_Allocator(s.get_allocator()), m_pszBuffer(NULL), m_nLength(0), m_nCapacity(0)
	{
		assign(s.begin(), s.end());
	}
	///	�f�X�g���N�g���܂�(��virtual�֐�)
	~basic_string() {
		_Clear();
	}

//#!�W��STL��std::string�Ƃ̕ϊ�
	///	std::basic_string<E,T,A>�^�̃I�u�W�F�N�gs���ێ����镶�������e�Ƃ��ď��������܂� 
	self_type& operator= (const std::basic_string<E,T,A>& s) {
		assign(s.begin(), s.end());
		return *this;
	}
	///	�ێ����Ă��镶����̓��e��std::basic_string<E,T,A>�^�̃I�u�W�F�N�g�𐶐����ĕԂ��܂�
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

//#!��r���Z�q
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
	///	n�����ڂւ̎Q�Ƃ�Ԃ��܂�
	reference operator[] (size_type n) {
		pointer p = getBuffer();
		if ( p==NULL ) return *((pointer)&char_null);
		return *(p+n);
	}
	///	n�����ڂւ�const�Q�Ƃ�Ԃ��܂�
	const_reference operator[] (size_type n) const {
		pointer p = getBuffer();
		if ( p==NULL ) return char_null;
		return *(p+n);
	}

//#!at
	///	n�����ڂւ̎Q�Ƃ�Ԃ��܂�
	///	n���͈͊O�̏ꍇ��index_out_of_bounds_exception��O��throw���܂�
	reference at (size_type n) {
		if (n<0 || size()<n) {
			throw detail::index_out_of_bounds_exception("in YTL::YTLstring::basic_string::at(size_type n)");
		}
		return (*this)[n];
	}
	///	n�����ڂւ�const�Q�Ƃ�Ԃ��܂�
	///	n���͈͊O�̏ꍇ��index_out_of_bounds_exception��O��throw���܂�
	const_reference at (size_type n) const {
		if (n<0 || size()<n) {
			throw detail::index_out_of_bounds_exception("in YTL::YTLstring::basic_string::at(size_type n) const");
		}
		return (*this)[n];
	}

//#!append
	///	s���w��������̂����A������len�������̕�����𖖔��ɒǉ����܂�
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
	self_type& append(const char_type* s, size_type len) {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::append(const char_type* s, size_type len)");
		}
		_Insert(s, len, size());
		return *this;
	}
	///	������s�𖖔��ɒǉ����܂�
	self_type& append(const self_type& s) {
		if ( s.size()!=0 ) {
			_Insert(s.c_str(), s.size(), size());
		}
		return *this;
	}
	///	������s��pos�����ڂ���len�������𖖔��ɒǉ����܂�
	///	pos���͈͊O�̏ꍇ��index_out_of_bounds_exception��O��throw���܂�
	self_type& append(const self_type& s, size_type pos, size_type len) {
		if ( s.size()<pos ) {
			throw detail::index_out_of_bounds_exception("in YTL::YTLstring::basic_string::append(const self_type& s, size_type pos, size_type len)");
		}
		_Insert(s.c_str()+pos, detail::_min(len, s.size()-pos), size());
		return *this;
	}
	///	\0�I�[������s�𖖔��ɒǉ����܂�
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
	self_type& append(const char_type* s) {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::append(const char_type* s)");
		}
		const size_type len = traits_type::length(s);
		_Insert(s, len, size());
		return *this;
	}
	///	����ch��len���ׂ�������𖖔��ɒǉ����܂�
	self_type& append(size_type len, char_type ch) {
		_InsertChar(ch, len, size());
		return *this;
	}
	///	[first,last)�͈̔͂̕�����𖖔��ɒǉ����܂�
	template<class InputIterator>
	self_type& append(InputIterator first, InputIterator last) {
		const size_type len = std::distance(first, last);
		_Insert(first, len, size());
		return *this;
	}

//#!assign
	///	s���w��������̂����A������len�������̕���������蓖�Ă܂�
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
	///	������s�����蓖�Ă܂�
	self_type& assign(const self_type& s) {
		//	s = s;�̂悤�Ȏ��ȑ���ɑ΂��ăZ�[�t�łȂ���΂Ȃ�Ȃ�
		if ( this==&s ) return *this; // �R�s�[����K�v�Ȃ��I
		assign(s.c_str(), s.size());
		return *this;
	}
	///	������s��pos�����ڂ���len�����������蓖�Ă܂�
	///	pos���͈͊O�̏ꍇ��index_out_of_bounds_exception��O��throw���܂�
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
	///	������s�����蓖�Ă܂�
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
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
	///	����ch��len���ׂ�����������蓖�Ă܂�
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
	///	[first,last)�͈̔͂̕���������蓖�Ă܂�
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
	///	���g�̕�����ƁA������s���r���܂�
	///	����������0�A�����łȂ�����0�ȊO��Ԃ��܂�
	int compare(const self_type& s) const {
		return _Compare(c_str(), size(), s.c_str(), s.size());
	}
	///	���g��pos�����ڂ���len�������̕�����ƁA������s���r���܂�
	///	����������0�A�����łȂ�����0�ȊO��Ԃ��܂�
	///	pos���͈͊O�̏ꍇ��index_out_of_bounds_exception��O��throw���܂�
	int compare(size_type pos, size_type len, const self_type& s) const {
		if ( size()<=pos ) {
			//if ( s.size() ) return -1;
			//return 0;
			throw detail::index_out_of_bounds_exception("in YTL::YTLstring::basic_string::compare(size_type pos, size_type len, const self_type& s) const");
		}
		return _Compare(c_str()+pos, detail::_min(len, size() - pos), s.c_str(), s.size());
	}
	///	���g��pos�����ڂ���len�������̕�����ƁA������s��pos2�����ڂ���len2�������̕�������r���܂�
	///	����������0�A�����łȂ�����0�ȊO��Ԃ��܂�
	///	pos/pos2���͈͊O�̏ꍇ��index_out_of_bounds_exception��O��throw���܂�
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
	///	���g�̕�����ƁA������s���r���܂�
	///	����������0�A�����łȂ�����0�ȊO��Ԃ��܂�
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
	int compare(const char_type* s) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::compare(const char_type* s) const");
		}
		const size_type len = traits_type::length(s);
		return _Compare(c_str(), size(), s, len);
	}
	///	���g��pos�����ڂ���len�������̕�����ƁA������s��pos2�����ڈȍ~�̕�������r���܂�
	///	����������0�A�����łȂ�����0�ȊO��Ԃ��܂�
	///	pos���͈͊O�̏ꍇ��index_out_of_bounds_exception��O��throw���܂�
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
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
	///	pos�����ڂ���len�������̕�����𐶐����ĕԂ��܂�
	///	pos���͈͊O�̏ꍇ��index_out_of_bounds_exception��O��throw���܂�
	self_type substr(size_type pos = 0, size_type len = npos) const {
		if ( size()<=pos ) {
			throw detail::index_out_of_bounds_exception("in YTL::YTLstring::basic_string::substr(size_type pos = 0,size_type n = npos) const");
		}
		return self_type().assign(*this, pos, len);
	}

//#!erase
	///	[first,last)�͈̔͂̕������폜���܂�
	iterator erase(iterator first, iterator last) {
		// first��begin()�@���@last��end() �̂Ƃ��A��������܂邲�Ə���
		_Erase(first.get(), last.get() - first.get());
		return first;
	}
	///	pos�����ڂ���len���������폜���܂�
	self_type& erase(size_type pos = 0, size_type len = npos) {
		// pos��0�@���@len��npos �̂Ƃ��A��������܂邲�Ə���
		_Erase(pos, len);
		return *this;
	}
	///	it���w���������폜���܂�
	iterator erase(iterator it) {
		_Erase(it.get(), 1);
		return it;
	}

//#!copy
	///	�o�b�t�@s�ɁA���g��pos�����ڂ���len�������̕�������R�s�[���A�R�s�[������������Ԃ��܂�
	///	pos���͈͊O�̏ꍇ��index_out_of_bounds_exception��O��throw���܂�
	size_type copy(char_type* s, size_type len, size_type pos=0) const {
		if ( size()<=pos ) {
			throw detail::index_out_of_bounds_exception("in YTL::YTLstring::basic_string::copy(char_type* s, size_type len, size_type pos=0) const");
		}
		//if (len == npos || size()<len || size()<pos+len)len=size()-pos;	//	detail::_min(len, size()-pos)�ł������
		_NCopy(s, c_str()+pos, detail::_max(detail::_min(len, size()-pos), 0));
		return len;
	}

//#!c_str()��data()
	///	\0�I�[�������Ԃ��܂��B�istring��'\0'���܂ޏꍇ�́A'\0'�̎�O�܂ł�C���ꕶ���Ƃ��Đ���ɔF������܂��j
	const char_type* c_str() const {
		const char_type* p = getBuffer();
		return (p!=NULL) ? p : &char_null;	//	0�����̏ꍇ�ł��K���m�ۂ��Ă܂���ŁE�E�E
	}
	///	�����f�[�^�̐擪�����������|�C���^��Ԃ��܂��B
	const char_type* data() const {
		return c_str();
	}

//#!find
	///	pos�����ڂ��珇�����Ɍ������܂��i�ΏہF������s�̐擪����len�������j
	///	�Ώۂ����������Ƃ��A���̈ʒu���[���I���W���ŕԂ��܂�
	///	������Ȃ�����npos��Ԃ��܂�
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
	size_type find(const char_type* s, size_type pos, size_type len) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::find(const char_type* s, size_type pos, size_type len) const");
		}
		const size_type _size = size();
		if ( len==0 && pos<=_size ) return pos;		//	�͈͓��Ȃ�󕶎��͏�Ƀ}�b�`���Ȃ�����炵��
		if ( pos<_size && len<=_size-pos ) {
			const_pointer _first = getBuffer();
			const_pointer _last = _first + _size - len + 1;
			const_pointer _it = _first + pos;
			while ( _it != _last ) {	//	char_traits<E>::search�������đR��ׂ����Ǝv�����c
				if ( traits_type::eq(*_it, *s) ) {
					if ( 0==traits_type::compare(_it, s, len) ) { return (_it - _first); }
				}
				++_it;
			}
		}

		return npos;
	}
	///	pos�����ڈȍ~���������Ɍ������܂��i�ΏہF������s�S�́j
	///	�Ώۂ����������Ƃ��A���̈ʒu���[���I���W���ŕԂ��܂�
	///	������Ȃ�����npos��Ԃ��܂�
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
	size_type find(const char_type* s, size_type pos = 0) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::find(const char_type* s, size_type pos = 0) const");
		}
		const size_type len = traits_type::length(s);	//	������null�`�F�b�N���ĂȂ��c
		return find(s, pos, len);
	}
	///	pos�����ڈȍ~���������Ɍ������܂��i�ΏہF������s�S�́j
	///	�Ώۂ����������Ƃ��A���̈ʒu���[���I���W���ŕԂ��܂�
	///	������Ȃ�����npos��Ԃ��܂�
	size_type find(const basic_string& s, size_type pos = 0) const{
		return find(s.c_str(), pos, s.size());
	}
	///	pos�����ڈȍ~���������Ɍ������܂��i�ΏہF����ch�j
	///	�Ώۂ����������Ƃ��A���̈ʒu���[���I���W���ŕԂ��܂�
	///	������Ȃ�����npos��Ԃ��܂�
	size_type find(char_type ch, size_type pos = 0) const{
		return find_first_of(ch, pos);
	}

//#!rfind
	///	pos�����ڈȑO���t�����Ɍ������܂��i�ΏہF������s�̐擪����len�������j
	///	�Ώۂ����������Ƃ��A���̈ʒu���[���I���W���ŕԂ��܂�
	///	������Ȃ�����npos��Ԃ��܂�
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
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
	///	pos�����ڈȑO���t�����Ɍ������܂��i�ΏہF������s�S�́j
	///	�Ώۂ����������Ƃ��A���̈ʒu���[���I���W���ŕԂ��܂�
	///	������Ȃ�����npos��Ԃ��܂�
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
	size_type rfind(const char_type* s, size_type pos = npos) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::rfind(const char_type* s, size_type pos = npos) const");
		}
		const size_type len = traits_type::length(s);
		return rfind(s, pos, len);
	}
	///	pos�����ڈȑO���t�����Ɍ������܂��i�ΏہF������s�S�́j
	///	�Ώۂ����������Ƃ��A���̈ʒu���[���I���W���ŕԂ��܂�
	///	������Ȃ�����npos��Ԃ��܂�
	size_type rfind(const basic_string& s, size_type pos = npos) const {
		return rfind(str.c_str(),pos,str.size());
	}
	///	pos�����ڈȑO���t�����Ɍ������܂��i�ΏہF����ch�j
	///	�Ώۂ����������Ƃ��A���̈ʒu���[���I���W���ŕԂ��܂�
	///	������Ȃ�����npos��Ԃ��܂�
	size_type rfind(char_type ch, size_type pos = npos) const {
		return find_last_of(ch, pos);
	}

//#!find_first_of
	///	pos�����ڈȍ~���������Ɍ������܂��i�ΏہF������s�̐擪����len�������̕�����Ɋ܂܂�镶���̂����ꂩ�j
	///	�Ώۂ����������Ƃ��A���̈ʒu���[���I���W���ŕԂ��܂�
	///	������Ȃ�����npos��Ԃ��܂�
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
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
	///	pos�����ڈȍ~���������Ɍ������܂��i�ΏہF������s�Ɋ܂܂�镶���̂����ꂩ�j
	///	�Ώۂ����������Ƃ��A���̈ʒu���[���I���W���ŕԂ��܂�
	///	������Ȃ�����npos��Ԃ��܂�
	size_type find_first_of(const self_type& s, size_type pos=0) const {
		return find_first_of(s.c_str(), pos, s.size());
	}
	///	pos�����ڈȍ~���������Ɍ������܂��i�ΏہF������s�Ɋ܂܂�镶���̂����ꂩ�j
	///	�Ώۂ����������Ƃ��A���̈ʒu���[���I���W���ŕԂ��܂�
	///	������Ȃ�����npos��Ԃ��܂�
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
	size_type find_first_of(const char_type* s, size_type pos=0) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::find_first_of(const char_type* s, size_type pos=0) const");
		}
		const size_type len = traits_type::length(s);
		return find_first_of(s, pos, len);
	}
	///	pos�����ڈȍ~���������Ɍ������܂��i�ΏہF����ch�j
	///	�Ώۂ����������Ƃ��A���̈ʒu���[���I���W���ŕԂ��܂�
	///	������Ȃ�����npos��Ԃ��܂�
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
	///	pos�����ڈȍ~���������Ɍ������܂��i�ΏہF������s�̐擪����len�������̕�����Ɋ܂܂�Ȃ������j
	///	�Ώۂ����������Ƃ��A���̈ʒu���[���I���W���ŕԂ��܂�
	///	������Ȃ�����npos��Ԃ��܂�
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
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
	///	pos�����ڈȍ~���������Ɍ������܂��i�ΏہF������s�Ɋ܂܂�Ȃ������j
	///	�Ώۂ����������Ƃ��A���̈ʒu���[���I���W���ŕԂ��܂�
	///	������Ȃ�����npos��Ԃ��܂�
	size_type find_first_not_of(const self_type& s, size_type pos=0) const {
		return find_first_not_of(s.c_str(), pos, s.size());
	}
	///	pos�����ڈȍ~���������Ɍ������܂��i�ΏہF������s�Ɋ܂܂�Ȃ������j
	///	�Ώۂ����������Ƃ��A���̈ʒu���[���I���W���ŕԂ��܂�
	///	������Ȃ�����npos��Ԃ��܂�
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
	size_type find_first_not_of(const char_type* s, size_type pos=0) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::find_first_not_of(const char_type* s, size_type pos=0) const");
		}
		const size_type len = traits_type::length(s);
		return find_first_not_of(s, pos, len);
	}
	///	pos�����ڈȍ~���������Ɍ������܂��i�ΏہF����ch�ȊO�̕����j
	///	�Ώۂ����������Ƃ��A���̈ʒu���[���I���W���ŕԂ��܂�
	///	������Ȃ�����npos��Ԃ��܂�
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
	// s����n�܂�len�������̕�����Ɋ܂܂�邢���ꂩ�̕������Astring��pos�����ڈȑO����t��������
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
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
	// ������s�Ɋ܂܂�邢���ꂩ�̕������Astring��pos�����ڈȑO����t��������
	size_type find_last_of(const self_type& s, size_type pos=npos) const {
		return find_last_of(s.c_str(), pos, s.size());
	}
	// C���ꕶ����s�Ɋ܂܂�邢���ꂩ�̕������Astring��pos�����ڈȑO����t��������
	size_type find_last_of(const char_type* s, size_type pos=npos) const {
		if(s==NULL)return npos;
		const size_type len = traits_type::length(s);
		return find_last_of(s, pos, len);
	}
	// ����c���Astring��pos�����ڈȑO����t��������
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
	// s����n�܂�n�������̕�����Ɋ܂܂�Ȃ������ꂩ�̕������Astring��pos�����ڈȑO����t��������
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
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
	// ������s�Ɋ܂܂�Ȃ������ꂩ�̕������Astring��pos�����ڈȑO����t��������
	size_type find_last_not_of(const self_type & s, size_type pos=npos) const {
		return find_last_not_of(s.c_str(), pos, s.size());
	}
	// C���ꕶ����s�Ɋ܂܂�Ȃ������ꂩ�̕������Astring��pos�����ڈȑO����t��������
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
	size_type find_last_not_of(const char_type* s, size_type pos=npos) const {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::find_last_not_of(const char_type* s, size_type pos) const");
		}
		const size_type len = traits_type::length(s);
		return find_last_not_of(s, pos, len);
	}
	// ����c�ȊO�̕������Astring��pos�����ڈȑO����t��������
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
	// �A���P�[�^��Ԃ��܂�
	allocator_type get_allocator() const {
		return m_Allocator;
	}

//#!insert
	// pos�����ڂ�str�Ŏn�܂�len�����̕������}�����܂�
	self_type& insert(size_type pos, const char_type* s, size_type len) {
		_Insert(s, len, pos);
		return *this;
	}
	// pos�����ڂ�C���ꕶ����s��}�����܂�
	///	s��NULL�̏ꍇ��null_pointer_exception��O��throw���܂�
	self_type& insert(size_type pos, const char_type* s) {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::insert(size_type pos, const char_type *str)");
		}
		const size_type len = traits_type::length(s);
		_Insert(s, len, pos);
		return *this;
	}
	// pos�����ڂɕ�����str��}�����܂�
	self_type& insert(size_type pos, const self_type& s) {
		_Insert(s.c_str(), s.size(), pos);
		return *this;
	}
	// pos�����ڂ�str��index2�����ڂ���len�������̕������}�����܂�
	self_type& insert(size_type pos, const self_type& s, size_type pos2, size_type len) {
		const size_type _size = s.size();
		if ( pos2<_size() ) {
			_Insert(s.c_str()+pos2, detail::_min(len, _size-pos2), pos);
		}
		return *this;
	}
	// pos�����ڂɕ���ch��len���ׂ��������}�����܂�
	self_type& insert(size_type pos, size_type len, char_type ch) {
		_InsertChar(ch, len, pos);
		return *this;
	}
	// pos�̏ꏊ�ɕ���ch��len���ׂ��������}�����܂�
	void insert(iterator pos, size_type len, char_type ch) {
		_InsertChar(ch, len, pos.get());
		return *this;
	}
	// pos�̏ꏊ�ɕ���ch��}�����܂�
	iterator insert(iterator pos, char_type ch) {
		_InsertChar(ch, 1, pos.get());
		return pos;
	}
	// pos�̏ꏊ��start�`end�̎����������}�����܂�
	template<class InputIterator>
	void insert(iterator pos, InputIterator first, InputIterator last) {
		const size_type len = std::distance(first, last);
		_Insert(first, len, pos.get());
	}

//#!max_size
	///	�ő�̕����̒����𓾂܂�
	size_type max_size() const{
		return get_allocator().max_size();
	}

//#!begin()��end()
	// �ŏ��̕����̃C�^���[�^��Ԃ��܂��iconst�j
	const_iterator begin() const { return const_iterator(0, this); }
	// �ŏ��̕����̃C�^���[�^��Ԃ��܂�
	iterator begin() { return iterator(0, this); }
	// �I�[�̃C�^���[�^��Ԃ��܂��iconst�j
	const_iterator end() const { return const_iterator(npos, this); }
	// �I�[�̃C�^���[�^��Ԃ��܂�
	iterator end() { iterator it(npos, this); return it; }

//#!rbegin()��rend()
	// �t�����猩�čŏ��̕����̃��o�[�X�C�^���[�^��Ԃ��܂��iconst�j
	const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	// �t�����猩�čŏ��̕����̃��o�[�X�C�^���[�^��Ԃ��܂�
	reverse_iterator rbegin() { return reverse_iterator(end()); }
	// �t�����猩�ďI�[�̃��o�[�X�C�^���[�^��Ԃ��܂��iconst�j
	const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
	// �t�����猩�ďI�[�̃��o�[�X�C�^���[�^��Ԃ��܂�
	reverse_iterator rend() { return reverse_iterator(begin()); }

//#!replace
	// pos�����ڂ���len���������A������str�ƒu�������܂�
	self_type& replace(size_type pos, size_type len, const self_type& s) {
		_Replace(s.c_str(), s.size(), pos, len);
		return *this;
	}
	// pos�����ڂ���len���������A������str��pos2�����ڂ���len2�������ƒu�������܂�
	self_type& replace(size_type pos, size_type len, const self_type& s, size_type pos2, size_type len2) {
		const size_type _size = s.size();
		if ( pos2==npos || _size<=pos2 ) {
			_Erase(pos, len);
		} else {
			_Replace(str.c_str()+pos2, detail::_min(len2, (_size-pos2)), pos, len);
		}
		return *this;
	}
	// pos�����ڂ���len���������AC���ꕶ����str�ƒu�������܂�
	self_type& replace(size_type pos, size_type len, const char_type* s) {
		if ( s==NULL ) {
			throw detail::null_pointer_exception("in YTL::YTLstring::basic_string::replace(size_type pos, size_type len, const char_type* s)");
		}
		const size_type len2 = traits_type::length(s);
		_Replace(s, len2, pos, len);
		return *this;
	}
	// pos�����ڂ���len���������Alen2�̕���ch�Œu�������܂�
	self_type& replace(size_type pos, size_type len, size_type len2, char_type ch) {
		_ReplaceChar(ch, len2, pos, len);
		return *this;
	}

	// start�`end�̋�Ԃ��A������str�Œu�������܂�
	self_type& replace(iterator start, iterator end, const self_type& s) {
		const size_type len = std::distance(first, last);
		_Replace(s.c_str(), s.size(), start.get(), len);
		return *this;
	}
	// start�`end�̋�Ԃ��AC���ꕶ����str�Œu�������܂�
	self_type& replace(iterator first, iterator last, const char_type* s) {
		const size_type len = std::distance(first, last);
		const size_type len2 = traits_type::length(s);
		_Replace(str.c_str(), len2, first.get(), len);

		return *this;
	}
	// start�`end�̋�Ԃ��Alen�̕���ch�Œu�������܂�
	self_type& replace(iterator first, iterator last, size_type len, char_type ch){
		const size_type len2 = traits_type::length(s);
		_ReplaceChar(ch, len, start.get(), len2);
		return *this;
	}
	// start�`end�̋�Ԃ��A�ʃR���e�i��start2�`end2������������Œu�������܂�
	template<class InputIterator> self_type & replace(iterator first, iterator last, InputIterator first2, InputIterator last2){
		const size_type len = std::distance(first, last);
		const size_type len2 = std::distance(first2, last2);
		_Replace(first2, len2, first.get(), len);
		return *this;
	}

//#!reserve
	// len�����ȏ���[�ł��郁�������m�ۂ��܂�
	void reserve(size_type len=0) {
		//size()+1<=capacity()�Ȃ̂Ŕ�r�͂P��OK
		_MakeBuffer(len);
		if ( len < capacity() ) {
		} else {	//	�m�ۂ��Ȃ���?
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
	// ������̃T�C�Y��ύX���܂��B�����Ȃ��������͕���ch�Ŗ��܂�܂�
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
	// ������̃T�C�Y��ύX���܂�
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
	// �Q�̕�����̒��g�����ւ��܂��B�i�A�h���X�����ւ��Ă���̂ŁA�A���P�[�^�������ɓ���ւ��Ă��܂��j
	void swap(self_type& s) {
		std::swap(m_pszBuffer, s.m_pszBuffer);
		std::swap(m_nCapacity, s.m_nCapacity);
		std::swap(m_nLength, s.m_nLength);
		std::swap(m_Allocator, s.m_Allocator);
	}

//#!�����o�ϐ��̏K��
	size_type capacity() const { return m_nCapacity; }
	bool empty() const { return size()==0; }

	size_type	size() const {
		return m_nLength;
	}
	size_type	length() const {
		return size();
	}

protected:

//#!�����o�ϐ�
	pointer			m_pszBuffer;
	size_type		m_nLength;
	//	nLength!=0 �̂Ƃ��Am_pszBuffer�́A
	//	sizeof(E) * (m_nLength+1)�m�ۂ���Ă���
	//				(��+1�Ȃ̂͏I�[�����񂪂���̂�)
	size_type		m_nCapacity;	//	�m�ۂ����T�C�Y(2^n�P�ʂŊm��)
	allocator_type	m_Allocator;

	///	�o�b�t�@�𓾂�
	pointer getBuffer() const { return m_pszBuffer; }

//#!�������m�ۃ��\�b�h
	///	nSize : �m�ۂ���T�C�Y
	///	pBuf  : �m�ۂ����|�C���^
	///	capa  : ���ۂɊm�ۂ����T�C�Y(2^n�ŃA���C�������)
	void myAlloc(size_type nSize,pointer&pBuf,size_type& capa) {
		//	2^n�ɃA���C������
		size_type n = nSize;
		size_type c = 1;
		while ( n!=0 ) {
			n>>=1; c<<=1;
		}
		pBuf = m_Allocator.allocate(c,(pointer)0);//new E[c];
		capa = c;
	}

//#!������������\�b�h
	///	myAlloc�Ŋm�ۂ������������������
	void myFree(pointer pBuf) {
		if ( pBuf!=NULL ) {
			m_Allocator.deallocate(pBuf, m_nCapacity);//delete [] pBuf;
		}
	}

protected:///////////////////////////////////////////////////////////
//�C���X�^���X���E���Ă邵�ĂȂ��̈�ւ̃R�s�[
	//�Ō�̃|�C���^ins�ȍ~�̓C���X�^���X�����Ă��Ȃ��B
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
//�C���X�^���X���̈�����
	void _Destroy(pointer s, size_type n){
		for(size_type i=0;i<n;++i){
			m_Allocator.destroy(s++);
		}
	}
//#! �v���C�x�[�g�ɉB�����ꂽ�{�������\�b�h
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

//	�I�y���[�^
template <class E,class T,class A>
basic_string<E,T,A>	operator+ (E* pBuf, const basic_string<E,T,A>&s2) {
	return basic_string<E,T,A>(pBuf) += s2;
}

template <class E,class T,class A>
basic_string<E,T,A>	operator+ (E ch, const basic_string<E,T,A>&s2) {
	return basic_string<E,T,A>(ch) += s2;
}

//*			// stream�ւ̑Ή��@�Ԑڔ�
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
/*/			// stream�ւ̑Ή��@���ڔ�
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
//*/		// stream�ւ̑Ή��I���B


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

//	����Ȃ���[���Ȃ��N���X���̂Ɋۈ������������R(`�D�L)�m�ܧ�����

#endif
