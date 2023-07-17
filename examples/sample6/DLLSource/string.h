/*
	simple string class
		STL��string class�́AVC++6��7(.NET)�Ŏ������قȂ�̂�
		���O�ŗp�ӂ���B
		�iSTLPort���������s�ςƂ͌���Ȃ��̂Łj

	implemented by yaneurao(M.Isozaki) '03/04/02
	UNICODE�ł����܂������Ƃ����Ȃ��D�D�R(�L�D�M)�m

	�Ȃ񂩏t�x�݂̏h����Ċ����D�D

	���낢�둫��Ȃ������o���邩���m��Ȃ����ǁA�ǉ�������A
	��˂��炨�܂Ń��[�����Ă���[�����i�O�O�G
*/

#ifndef __YTLString_h__
#define __YTLString_h__

//#include "../YTL/exceptions.h"
#include <stdexcept>
#include <iterator>
//#include <iostream>

namespace YTL {
//	���ɒP���Ȏ��������Ǌ��ق��Ă���D�D(��_�G)�
template<class E,
class T = std::char_traits<E>,
class A = std::allocator<E> >
class basic_string {
/**
	STL�݊�string�N���X(���Ȃ肵��ڂ�)
	namespace��YTL�ɓ����Ă���Bstdafx.h��using���Ďg��
*/
public:
	typedef basic_string<E,T,A> _Myt;
	typedef T traits_type;
	typedef A allocator_type;
	typedef T::char_type char_type;
	typedef A::size_type size_type;
	typedef A::difference_type difference_type;
	typedef A::pointer pointer;
	typedef A::const_pointer const_pointer;
	typedef A::reference reference;
	typedef A::const_reference const_reference;
	typedef A::value_type value_type;
	static const size_type npos;
	static char_type nullstr; // '\0'���w���|�C���^

	struct iterator;
	struct const_iterator;
	struct reverse_iterator;
	struct const_reverse_iterator;

	struct iterator {
	private:
		iterator(const const_iterator & i){}//�֎~
		iterator(const const_reverse_iterator & i){}//�֎~
		iterator & operator = (const const_iterator & i){return *this;}//�֎~
		iterator & operator = (const const_reverse_iterator & i){return *this;}//�֎~
	public:
		/* iterator����`�����Ⴄ���[�R�i�L�[�M�j�m */
		iterator() : m_nPos(npos),m_pStr(NULL) {}
		iterator(const iterator & i) : m_nPos(i.m_nPos), m_pStr(i.m_pStr) {}
		iterator(size_type n,_Myt * pStr)
			: m_nPos(n),m_pStr(pStr) {	set(n);	}

		//	����I�y���[�^
		iterator & operator = (const iterator & i){
			m_nPos=i.m_nPos;
			m_pStr=i.m_pStr;
			return *this;
		}
		//iterator & operator = (size_type size){
		//	set(size);
		//	return *this;
		//}

		//	�A�N�Z�b�T
		size_type get() const { return m_nPos; }
		void set(size_type n) {
			m_nPos = n;
			if(length()<=m_nPos){
				m_nPos=npos;
			}
		}

		//	*
		reference operator* () const {
			if (m_pStr!=NULL) {
				return (*m_pStr)[get()];
			}
			return nullstr;
		}

		//	���Z�q
		iterator& operator += (size_type n){
			if (m_nPos!=npos){
				set(m_nPos+n);
			}
			return *this;
		}
		iterator& operator -= (size_type n){
			if (m_nPos==npos){
				m_nPos = length();
			}
			if (m_nPos < n) {
				m_nPos = 0;
			} else {
				set(m_nPos-n);
			}
			return *this;
		}
		void	inc(){
			if (m_nPos!=npos){
				m_nPos++;
				if (m_nPos>=length()) m_nPos = npos;
			}
		}
		void	dec(){
			if (m_nPos==npos){
				m_nPos = length()-1;
			} else {
				if (m_nPos!=0) m_nPos--;
			}
		}
		///	�����Z�̂��߂̃I�y���[�^
		iterator& operator++() { inc(); return (*this); }
		iterator operator++(int)
			{ iterator _Tmp = *this; inc(); return (_Tmp); }
		iterator& operator--() { dec(); return (*this); }
		iterator operator--(int)
			{ iterator _Tmp = *this; dec(); return (_Tmp); }
		iterator operator+(size_type _N) const
			{iterator _Tmp = *this; return (_Tmp += _N); }
		iterator operator-(size_type _N) const
			{iterator _Tmp = *this; return (_Tmp -= _N); }

		//	��r�I�y���[�^
		bool	operator == (const iterator& it) const {
			return (m_nPos == it.m_nPos) && (m_pStr == it.m_pStr);
		}
		bool	operator != (const iterator& it) const {
			return !(*this == it);
		}
		//	size_type�Ƃ̔�r���p�ӂ��Ȃ���string::npos�Ɣ�r�ł��Ȃ�
		bool	operator == (size_type size) const {
			return m_nPos == size;
		}
		bool	operator != (size_type size) const {
			return !(*this == size);
		}

		size_type length() const {
			return m_pStr!=0?m_pStr->length():0;
		}

	protected:
		size_type	m_nPos;	//	0�I���W���ȃ|�W�V�����ŕۑ�����Ƃ��悤
		_Myt * m_pStr; // �e�ւ̃|�C���^�������Ɖ����Z�̂Ƃ���end�̔��肪�ł���
	};

	struct const_iterator : public iterator{
	public:
		const_iterator() : iterator() {}
		const_iterator(const iterator & i) : iterator(i) {}
		const_iterator(size_type n,_Myt * pStr) : iterator(n, pStr) {}

		const_reference operator* () const {
			if (m_pStr!=NULL) {
				return (*m_pStr)[get()];
			}
			return nullstr;
		}
	};
	struct reverse_iterator : public iterator{
	private:
		reverse_iterator(const const_iterator & i){}//�֎~
		reverse_iterator(const const_reverse_iterator & i){}//�֎~
		reverse_iterator & operator = (const const_iterator & i){return *this;}//�֎~
		reverse_iterator & operator = (const const_reverse_iterator & i){return *this;}//�֎~
	public:
		reverse_iterator() : iterator() {}
		reverse_iterator(const iterator & i) : iterator(i) {}
		reverse_iterator(size_type n,_Myt * pStr) : iterator(n, pStr) {}

		//	*
		reference operator* () const {
			if (m_pStr!=NULL) {
				iterator i(*this);
				return *(--i);
			}
			return nullstr;
		}

		//	���Z�q
		reverse_iterator& operator += (size_type n){
			iterator::operator-=(n);
			return *this;
		}
		reverse_iterator& operator -= (size_type n){
			iterator::operator+=(n);
			return *this;
		}
		iterator& operator++() { iterator::operator--(); return (*this); }
		iterator operator++(int)
			{ return iterator::operator--((int)0); }
		iterator& operator--() { iterator::operator++(); return (*this); }
		iterator operator--(int)
			{ return iterator::operator++((int)0); }
		iterator operator+(size_type _N) const
			{ return iterator::operator-(_N); }
		iterator operator-(size_type _N) const
			{ return iterator::operator+(_N); }
	};

	struct const_reverse_iterator : public reverse_iterator{
	public:
		const_reverse_iterator() : reverse_iterator() {}
		const_reverse_iterator(const iterator & i) : reverse_iterator(i) {}
		const_reverse_iterator(size_type n,_Myt * pStr) : reverse_iterator(n, pStr) {}

		const_reference operator* () const {
			if (m_pStr!=NULL) {
				const_iterator i(*this);
				return *(--i);
			}
			return nullstr;
		}
	};

//�R���X�g���N�^�E�f�X�g���N�^
	//��̕���������R���X�g���N�^
	explicit basic_string(const A & a=A()):m_Allocator(a),m_pszBuffer(NULL),m_nLength(0),m_nCapacity(0) {}
	// psz����n�܂�n�������̕��������e�Ƃ��镶��������R���X�g���N�^
	basic_string(const_pointer psz, size_type len, const A & a=A())
		:m_Allocator(a),m_pszBuffer(NULL),m_nLength(0),m_nCapacity(0)
	{
		assign(psz, len);
	}
	// ������s����e�Ƃ��镶��������R���X�g���N�^
	basic_string(const _Myt & s, const A & a=A())
		:m_Allocator(a),m_pszBuffer(NULL),m_nLength(0),m_nCapacity(0)
	{
		assign(s);
	}
	// C���ꕶ����psz����e�Ƃ��镶��������R���X�g���N�^
	basic_string(const char_type * psz, const A & a=A())
		:m_Allocator(a),m_pszBuffer(NULL),m_nLength(0),m_nCapacity(0)
	{
		assign(psz);
	}
	// ����ch��len���ׂ�����������R���X�g���N�^
	basic_string(size_type len, char_type ch, const A & a=A())
		:m_Allocator(a),m_pszBuffer(NULL),m_nLength(0),m_nCapacity(0)
	{
		assign(len, ch);
	}
	// ����ch��1�����̕���������R���X�g���N�^
	basic_string(const char_type ch, const A & a=A())
		:m_Allocator(a),m_pszBuffer(NULL),m_nLength(0),m_nCapacity(0)
	{
		assign(&ch, 1);
	}
	template <class InIter> basic_string(InIter start, InIter end, const A & a=A())
		:m_Allocator(a),m_pszBuffer(NULL),m_nLength(0),m_nCapacity(0)
	{
		assign(start, end);
	}
	// �f�X�g���N�^
	~basic_string() {
		myFree(m_pszBuffer);
	}

//operator =
	_Myt & operator = (const _Myt & s){
		assign(s);
		return *this;
	}
	_Myt & operator = (const char_type * str){
		assign(str);
		return *this;
	}
	_Myt & operator = (char_type ch){
		assign(1, ch);
		return *this;
	}

//operator +=
	_Myt & operator += (const _Myt & s){
		append(s);
		return *this;
	}
	_Myt & operator += (const char_type * str){
		append(str);
		return *this;
	}
	_Myt & operator += (char_type ch){
		append(1, ch);
		return *this;
	}

//operator +
	_Myt operator + (const _Myt & s2) {
		_Myt s1;
		s1.reserve(size()+s2.size()+1);
		s1.append(*this);
		s1.append(s2);
		return s1;
	}

//��r���Z�q
	bool operator == (const _Myt & s) const {
		if (m_pszBuffer==NULL && s.m_pszBuffer==NULL) return true;
		if (m_nLength!=s.m_nLength) return false; // �K�v�����𖞂����Ă��Ȃ�
		//	�Е�����NULL�ł�������́��Ɋ܂܂��̂Ń`�F�b�N���Ȃ��ėǂ�
		//	�}���`�o�C�g�����m��Ȃ��̂ŃR���y�A�͎��O�ł��Ȃ��Ƃ����Ȃ�
		if (m_nLength==0) return true;

		if(::memcmp(m_pszBuffer, s.m_pszBuffer, m_nLength * sizeof(char_type))==0)return true;
		//const_pointer p1 = m_pszBuffer;
		//const_pointer p2 = s.m_pszBuffer;
		//size_type n=m_nLength;
		//while (*(p1++)==*(p2++)) {
		//	if (0==--n) return true;
		//}
		return false;
	}

	bool	operator != (const _Myt & s) const {
		return !(*this==s);
	}
	bool	operator < (const _Myt & s) const {
		//	���ł����������r�ł��Ȃ���list�Ƃ���`�ł��Ȃ��ă}�Y�[(߄t�)
		return this < &s; // �I�u�W�F�N�g�̃A�h���X��r�ł�����R�i�L�[�M�j�m
	}
	bool	operator > (const _Myt & s) const {
		return s < *this;
	}
	bool	operator <= (const _Myt & s) const {
		return !(*this > s);
	}
	bool	operator >= (const _Myt & s) const {
		return !(*this < s);
	}

//operator []
	reference operator[](size_type n) {
		return m_pszBuffer[n];
	}
	const_reference operator[] (size_type n) const {
		return m_pszBuffer[n];
	}

//at
	// string[n]�̗�O�X���[��
	reference at (size_type n){
		if (n<0 || n>size()) {
			throw std::out_of_range("Out of range error! in YTL::string::at(size_type n)");
		}
		return (*this)[n];
	}
	// string[n]�̗�O�X���[��(const)
	const_reference at (size_type n) const {
		if (n<0 || n>size()) {
			throw std::out_of_range("Out of range error! in YTL::string::at(size_type n)");
		}
		return (*this)[n];
	}

//append
	// s����n�܂�n�������̕�������Astring�̖����ɑ}��
	_Myt & append(const_pointer s, size_type len){
		if (0<len && s!=NULL){
			size_type nLength = size() + len;
			if (nLength+1 > capacity()) {	//	�m�ۂ��Ȃ���?
				pointer pBuf;
				myAlloc(nLength+1,pBuf,m_nCapacity);
				if (size()!=0){
					::memcpy(pBuf,c_str(),size() * sizeof(char_type));
				}
				::memcpy(pBuf+size(),s,len * sizeof(char_type));
				*(pBuf+size()+len) = 0; // �I�[������
				myFree(m_pszBuffer);
				m_pszBuffer = pBuf;
			} else {
				::memcpy(m_pszBuffer+size(),s,len * sizeof(char_type));
				*((pointer)(c_str()+size()+len)) = 0; // �I�[������
			}
			m_nLength = nLength;
		}
		return *this;
	}
	// ������s���Astring�̖����ɑ}��
	_Myt & append(const basic_string<E,T,A>&s){
		append(s.c_str(), s.size());
		return *this;
	}
	// ������s��indx�����ڂ���len���������Astring�̖����ɑ}��
	_Myt & append(const basic_string<E,T,A>&s, size_type indx, size_type len){
		basic_string<E,T,A> ss;
		ss.assign(s, indx, len);//ss=s.substr(indx, len);
		append(ss.c_str(), ss.size());
		return *this;
	}
	// C���ꕶ����s���Astring�̖����ɑ}��
	_Myt & append(const char_type * s){
		if(s==NULL)return *this;
		size_type len=0;
		while(s[len])len++;
		append(s, len);
		return *this;
	}
	// ����ch��len���ׂ����̂��Astring�̖����ɑ}��
	_Myt & append(size_type len, char_type ch){
		_Myt ss(len, ch);
		append(ss.c_str(), ss.size());
		return *this;
	}
	// start����end��O�܂ł̃C�^���[�^���w����������Astring�̖����ɑ}��
	template<class InIter> basic_string<E,T,A>& append(InIter start, InIter end){
		_Myt ss(start, end);
		append(ss.c_str(), ss.size());
		return *this;
	}

//assign
	// s����n�܂�n�������̕�������Astring�ɑ��
	_Myt & assign(const_pointer s, size_type len){
		myFree(m_pszBuffer); // �o�b�t�@�����
		if (len == 0 || s==NULL){
			//	�����Ȃ����H
			m_pszBuffer = NULL;
			m_nLength = 0;
			m_nCapacity = 0;
		} else {
			m_nLength = len;
			myAlloc(len+1,m_pszBuffer,m_nCapacity);
			::memcpy(m_pszBuffer,s,len * sizeof(char_type));
			m_pszBuffer[len]='\0'; // c_str�̂��߂ɏI�[��\0�Ŗ��߂�
		}
		return *this;
	}
	// ������s���Astring�ɑ��
	_Myt & assign(const basic_string<E,T,A>&s){
		//	s = s;�̂悤�Ȏ��ȑ���ɑ΂��ăZ�[�t�łȂ���΂Ȃ�Ȃ�
		if (this==&s) return *this; // �R�s�[����K�v�Ȃ��I
		assign(s.c_str(), s.size());
		return *this;
	}
	// ������s��indx�����ڂ���len���������Astring�ɑ��
	_Myt & assign(const basic_string<E,T,A>&s, size_type pos, size_type n){
		if (s.size()<pos){
			assign((char*)NULL, 0);
			return *this;
		}
		int n2;
		if (n == npos || s.size()<pos+n){
			n2 = size()-pos;
		} else{
			n2 = n;
		}
		assign(s.c_str()+pos, n2);
		return *this;
	}
	// C���ꕶ����s���Astring�ɑ��
	_Myt & assign(const char_type *s){
		if(s==NULL){
			assign(s, 0);
			return *this;
		}
		int len=0;
		while(s[len])len++;
		assign(s, len);
		return *this;
	}
	// ����ch��len���ׂ����̂��Astring�ɑ��
	_Myt & assign(size_type len, char_type ch){
		myFree(m_pszBuffer); // �o�b�t�@�����
		if (len == 0){
			//	�����Ȃ����H
			m_pszBuffer = NULL;
			m_nLength = 0;
			m_nCapacity = 0;
		} else {
			m_nLength = len;
			myAlloc(len+1,m_pszBuffer,m_nCapacity);
			for(size_type i=0;i<len;i++){
				m_pszBuffer[i]=ch;
			}
			m_pszBuffer[len]='\0';
		}
		return *this;
	}
	// start����end��O�܂ł̃C�^���[�^���w����������Astring�ɑ��
	template<class InIter> _Myt & assign(InIter start, InIter end){
		size_type len=0;
		{
			InIter it=start;
			while(it!=end){it++;len++;}
		}

		myFree(m_pszBuffer); // �o�b�t�@�����
		if (len == 0){
			//	�����Ȃ����H
			m_pszBuffer = NULL;
			m_nLength = 0;
			m_nCapacity = 0;
		} else {
			m_nLength = len;
			myAlloc(len+1,m_pszBuffer,m_nCapacity);
			InIter it=start;
			for(size_type i=0;i<len;i++){
				m_pszBuffer[i]=*(it++);
			}
			m_pszBuffer[len]='\0';
		}
		return *this;
	}

//compare
	//�����񓯎m�̔�r
	int compare(const _Myt & s) const{
		if((*this)==s)return 0;
		else if((*this)<s)return -1;
		return 1;
	}
	//�����񓯎m�̔�r���̂Q
	int compare(size_type indx, size_type len, const _Myt & s) const{
		_Myt ss(*this, indx, len);
		return ss.compare(s);
	}
	//�����񓯎m�̔�r���̂R
	int compare(size_type indx, size_type len, const basic_string<E,T,A> *s, size_type indx2, size_type len2) const{
		_Myt ss(*this, indx, len);
		return ss.compare(s->substr(indx2, len2));
	}
	//�����񓯎m�̔�r���̂S
	int compare(const char_type *s) const{
		_Myt ss(s);
		return compare(ss);
	}
	//�����񓯎m�̔�r���̂T
	int compare(size_type indx, size_type len, const_pointer s, size_type len2=npos) const{
		_Myt ss(*this, indx, len);
		_Myt ss2(s, len2);
		return ss.compare(ss2);
	}

//substr
	//pos�����ڂ���n�������̕�����𐶐����ĕԂ�
	basic_string substr(size_type pos = 0,size_type n = npos) const{
		_Myt s;
		s.assign(*this, pos, n);
		return s;
	}

//erase
	// first����last��O�܂ł̕������폜
	iterator erase(iterator first, iterator last){
		// first��begin()�@���@last��end() �̂Ƃ��A��������܂邲�Ə���
		size_type p1 = first.get();
		if (p1==npos) p1 = length();
		if (p1>length()) p1 = length();
		size_type p2 = last.get();
		if (p2==npos) p2 = length();
		if (p2>length()) p2 = length();
		int l = p2-p1;
		int ts = (length()-p2+1); // �]��������
		memmove(m_pszBuffer+p1,m_pszBuffer+p2,ts * sizeof(char_type));//memcpy�̓R�s�[���s��Ȃ̂�memmove���g��
		m_nLength -= l;
		if (p1>=length()) first.set(npos);
		return first;
	}
	// p0�����ڂ���n���������폜
	_Myt & erase(size_type p0 = 0, size_type n = npos){
		// p0��0�@���@n��npos �̂Ƃ��A��������܂邲�Ə���
		iterator it1(p0,this);
		iterator it2;
		if(n==npos)it2=iterator(npos,this);
		else it2=iterator(p0+n,this);
		erase(it1,it2);
		return *this;
	}
	// it�̎����������폜
	iterator erase(iterator it){
		iterator it2 = it;
		it2++;
		return erase(it,it2);
	}
    
//copy
	//�o�b�t�@s�ɁAstring��len�����ڈȍ~��indx���������R�s�[���A�R�s�[������������Ԃ��܂�
	size_type copy(pointer s, size_type len, size_type indx=0) const{
		int nStart = indx;
		if (length()<pos) {
			nStart = length();
		}
		int nEnd;
		if (len == npos || length()<nStart+len){
			nEnd = length();
		} else{
			nEnd = nStart + len;
		}
		size_type l = nEnd - nStart;
		_Myt s;
		if (0<l){
			::memcpy(s,c_str() + nStart,l*sizeof(char_type));
			return l;
		}
		return 0;
	}

//c_str()��data()
	//C���ꕶ�����Ԃ��܂��B�istring��'\0'���܂ޏꍇ�́A'\0'�̎�O�܂ł�C���ꕶ���Ƃ��Đ���ɔF������܂��j
	const char_type * c_str() const {
		return m_pszBuffer!=NULL?m_pszBuffer:&nullstr;
	}
	//�����f�[�^�̐擪�����������|�C���^��Ԃ��܂��B
	const_pointer data() const{
		return c_str();
	}

//find - �����B�Ώۂ����������Ƃ��A�������������ԍ��i�擪���O�j��Ԃ��܂���
	// s����n�܂�n�������̕�������Astring��pos�����ڈȍ~���猟��
	size_type find(const_pointer s, size_type pos, size_type n) const{
		if (size()==0) return npos;
		if (n==0)return pos;
		if (s==NULL || pos == npos) return npos;
		pointer buf=m_pszBuffer;//�����o�ϐ��𗎂Ƃ��č���������i�v���O�����͏���������Â炭�Ȃ�E�E�E�H�j
		for(size_type i=pos, ii=size();i<ii;i++){
			if (buf[i] == s[0]){
				int j=1;
				while(1){
					if (j==n) return i; // ��v
					if (ii<=i+j)break;
					if (buf[i+j]!=s[j]) break; // ��v���Ȃ��̂ŃX�L�b�v
					j++;
				}
			}
		}
		return npos;
	}
	// C���ꕶ����s���Astring��pos�����ڈȍ~���猟��
	size_type find(const char_type *s, size_type pos = 0) const{
		if(s==NULL)return find(s, pos, 0);
		int len=0;
		while(s[len])len++;
		return find(s, pos, len);
	}
	// ������str���Astring��pos�����ڈȍ~���猟��
	size_type find(const basic_string& str, size_type pos = 0) const{
		return find(str.c_str(), pos, str.size());
	}
	// ����c���Astring��pos�����ڈȍ~���猟��
	size_type find(char_type ch, size_type pos = 0) const{
		return find_first_of(ch, pos);
	}

//rfind - �t���������B�Ώۂ����������Ƃ��A�������������ԍ��i�擪���O�j��Ԃ��܂���
	// s����n�܂�n�������̕�������Astring��pos�����ڈȑO����t��������
	size_type rfind(const_pointer s, size_type pos, size_type n) const{
		if (size()==0) return npos;
		if (n==0)return pos;
		if (s==NULL || pos==0)return npos;
		if (pos == npos) pos = length();
		pointer buf=m_pszBuffer;//�����o�ϐ��𗎂Ƃ��č���������i�v���O�����͏���������Â炭�Ȃ�E�E�E�H�j
		for(size_type i=pos-1,ii=size();0<=i;i--){
			if (buf[i] == s[0]){
				int j=1;
				while(1){
					if (j==n) return i; // ��v
					if (ii<=i+j)break;
					if (buf[i+j]!=s[j]) break; // ��v���Ȃ��̂ŃX�L�b�v
					j++;
				}
			}
		}
		return npos;
	}
	// C���ꕶ����s���Astring��pos�����ڈȑO����t��������
	size_type rfind(const char_type *s, size_type pos = npos) const{
		if(s==NULL)return rfind(s, pos, 0);
		int len=0;
		while(s[len])len++;
		return rfind(s, pos, len);
	}
	// ������str���Astring��pos�����ڈȑO����t��������
	size_type rfind(const basic_string& str, size_type pos = npos) const{
		return rfind(str.c_str(),pos,str.size());
	}
	// ����c���Astring��pos�����ڈȑO����t��������
	size_type rfind(char_type ch, size_type pos = npos) const{
		return find_last_of(ch, pos);
	}

//find_first_of
	// s����n�܂�n�������̕�����Ɋ܂܂�邢���ꂩ�̕������Astring��indx�����ڈȍ~���猟��
	size_type find_first_of(const_pointer s, size_type indx, size_type len) const{
		if(size()==0 || len==0 || s==NULL || indx==npos) return npos;
		pointer buf=m_pszBuffer;//�����o�ϐ��𗎂Ƃ��č���������i�v���O�����͏���������Â炭�Ȃ�E�E�E�H�j
		for(size_type i=indx, ii=size();i<ii;i++){
			for(size_type j=0;j<len;j++){
				if(buf[i]==s[j]){
					return i; // ��v
				}
			}
		}
		return npos;
	}
	// ������s�Ɋ܂܂�邢���ꂩ�̕������Astring��indx�����ڈȍ~���猟��
	size_type find_first_of(const _Myt &s, size_type indx=0) const{
		return find_first_of(s.c_str(), indx, s.size());
	}
	// C���ꕶ����s�Ɋ܂܂�邢���ꂩ�̕������Astring��indx�����ڈȍ~���猟��
	size_type find_first_of(const char_type *s, size_type indx=0) const{
		if(s==NULL)return npos;
		size_type len=0;
		while(s[len])len++;
		return find_first_of(s, indx, len);
	}
	// ����c���Astring��indx�����ڈȍ~���猟��
	size_type find_first_of(char_type ch, size_type indx=0) const{
		for(size_type i=indx, ii=size();i<ii;i++){
			if(*(m_pszBuffer+i)==ch)return i;
		}
		return npos;
	}

//find_first_not_of
	// s����n�܂�n�������̕�����Ɋ܂܂�Ȃ������ꂩ�̕������Astring��indx�����ڈȍ~���猟��
	size_type find_first_not_of(const_pointer s, size_type indx, size_type len) const{
		if(size()==0 || indx==npos) return npos;
		if(len==0 || s==NULL)return indx;//����A���Ԃǂ����ɂ��邩����Ƃ���E�E�E�H���ۂ̎��������Ȃ��Ƃ킩��܂ւ�B
		pointer buf=m_pszBuffer;//�����o�ϐ��𗎂Ƃ��č���������i�v���O�����͏���������Â炭�Ȃ�E�E�E�H�j
		for(size_type i=indx, ii=size();i<ii;i++){
			bool flag=true;
			for(size_type j=0;j<len;j++){
				if(buf[i]==s[j]){
					flag=false;
				}
			}
			if(flag)return i; // �s��v
		}
		return npos;
	}
	// ������s�Ɋ܂܂�Ȃ������ꂩ�̕������Astring��indx�����ڈȍ~���猟��
	size_type find_first_not_of(const _Myt &s, size_type indx=0) const{
		return find_first_not_of(s.c_str(), indx, s.size());
	}
	// C���ꕶ����s�Ɋ܂܂�Ȃ������ꂩ�̕������Astring��indx�����ڈȍ~���猟��
	size_type find_first_not_of(const char_type *s, size_type indx=0) const{
		if(s==NULL)return 0;
		size_type len=0;
		while(s[len])len++;
		return find_first_not_of(s, indx, len);
	}
	// ����c�ȊO�̕������Astring��indx�����ڈȍ~���猟��
	size_type find_first_not_of(char_type ch, size_type indx=0) const{
		for(size_type i=indx, ii=size();i<ii;i++){
			if(*(m_pszBuffer+i)!=ch)return i;
		}
		return npos;
	}

//find_last_of
	// s����n�܂�n�������̕�����Ɋ܂܂�邢���ꂩ�̕������Astring��indx�����ڈȑO����t��������
	size_type find_last_of(const_pointer s, size_type indx, size_type len) const{
		if(size()==0 || len==0 || s==NULL || indx==0) return npos;
		if(indx == npos) indx = length();
		pointer buf=m_pszBuffer;//�����o�ϐ��𗎂Ƃ��č���������i�v���O�����͏���������Â炭�Ȃ�E�E�E�H�j
		for(size_type i=indx-1;0<=i;i--){
			for(size_type j=0;j<len;j++){
				if(buf[i]==s[j]){
					return i; // ��v
				}
			}
		}
		return npos;
	}
	// ������s�Ɋ܂܂�邢���ꂩ�̕������Astring��indx�����ڈȑO����t��������
	size_type find_last_of(const _Myt & s, size_type indx=npos) const{
		return find_last_of(s.c_str(), indx, s.size());
	}
	// C���ꕶ����s�Ɋ܂܂�邢���ꂩ�̕������Astring��indx�����ڈȑO����t��������
	size_type find_last_of(const char_type * s, size_type indx=npos) const{
		if(s==NULL)return npos;
		size_type len=0;
		while(s[len])len++;
		return find_last_of(s, indx, len);
	}
	// ����c���Astring��indx�����ڈȑO����t��������
	size_type find_last_of(char_type ch, size_type indx=npos) const{
		for(size_type i=indx-1;0<=i;i--){
			if(m_pszBuffer[i]==ch)return i;
		}
		return npos;
	}

//find_last_not_of
	// s����n�܂�n�������̕�����Ɋ܂܂�Ȃ������ꂩ�̕������Astring��indx�����ڈȑO����t��������
	size_type find_last_not_of(const_pointer s, size_type indx, size_type len) const{
		if(size()==0 || indx==0) return npos;
		if(len==0 || s==NULL)return (indx ? (indx-1) : npos);
		if(indx == npos) indx = length();
		pointer buf=m_pszBuffer;//�����o�ϐ��𗎂Ƃ��č���������i�v���O�����͏���������Â炭�Ȃ�E�E�E�H�j
		for(size_type i=indx-1;0<=i;i--){
			bool flag=true;
			for(size_type j=0;j<len;j++){
				if(buf[i]==s[j]){
					flag=false;
					break;
				}
			}
			if(flag)return i; // �s��v
		}
		return npos;
	}
	// ������s�Ɋ܂܂�Ȃ������ꂩ�̕������Astring��indx�����ڈȑO����t��������
	size_type find_last_not_of(const _Myt & s, size_type indx=npos) const{
		return find_last_not_of(s.c_str(), indx, s.size());
	}
	// C���ꕶ����s�Ɋ܂܂�Ȃ������ꂩ�̕������Astring��indx�����ڈȑO����t��������
	size_type find_last_not_of(const char_type *s, size_type indx=npos) const{
		if(s==NULL)return size()-1;
		size_type len=0;
		while(s[len])len++;
		return find_last_not_of(s, indx, len);
	}
	// ����c�ȊO�̕������Astring��indx�����ڈȑO����t��������
	size_type find_last_not_of(char_type ch, size_type indx=npos) const{
		for(size_type i=indx-1;0<=i;i--){
			if(m_pszBuffer[i]!=ch)return i;
		}
		return npos;
	}

//get_allocator
	// �A���P�[�^��Ԃ��܂�
	A get_allocator() const{
		return m_Allocator;
	}

//insert
	// indx�����ڂ�str�Ŏn�܂�len�����̕������}�����܂�
	_Myt & insert(size_type indx, const_pointer str, size_type len){
		if(len==0)return *this;

		size_type i=indx.get();
		if(i==npos || size()<i)i=size();
		size_type nLength = size() + len;
		if (nLength+1 > capacity()) {	//	�m�ۂ��Ȃ���?
			pointer pBuf;
			myAlloc(nLength+1,pBuf,m_nCapacity);
			if(m_pszBuffer==NULL){
				*pBuf=ch;
				*(pBuf+1)='\0';
			}
			else{
				::memcpy(pBuf, m_pszBuffer, i * sizeof(char_type));
				::memcpy(pBuf+i,str,len * sizeof(char_type));
				::memcpy(pBuf+i+len, m_pszBuffer, (size()-i+1) * sizeof(char_type));
			}
			myFree(m_pszBuffer);
			m_pszBuffer = pBuf;
-		} else {
			::memmove((void *)(m_pszBuffer+i+len),(void *)(m_pszBuffer+i),(size()-i+1) * sizeof(char_type));
			::memcpy(pBuf+i,str,len * sizeof(char_type));
		}
		m_nLength = nLength;
		return *this;
	}
	// indx�����ڂ�C���ꕶ����str��}�����܂�
	_Myt & insert(size_type indx, const char_type *str){
		if(s==NULL)return *this;
		size_type len=0;
		while(str[len])len++;
		insert(indx, str, len);
		return *this;
	}
	// indx�����ڂɕ�����str��}�����܂�
	_Myt & insert(size_type indx, const _Myt & str){
		insert(indx, str.c_str(), str.size());
		return *this;
	}
	// indx�����ڂ�str��index2�����ڂ���len�������̕������}�����܂�
	_Myt & insert(size_type indx, const _Myt & str, size_type indx2, size_type len){
		insert(indx, str.substr(indx2, len));
		return *this;
	}
	// indx�����ڂɕ���ch��len���ׂ��������}�����܂�
	_Myt & insert(size_type indx, size_type len, char_type ch){
		_Myt str(len, ch);
		insert(indx, str.c_str(), len);
		return *this;
	}
	// indx�̏ꏊ�ɕ���ch��len���ׂ��������}�����܂�
	void insert(iterator indx, size_type len, const_reference ch){
		_Myt str(len, ch);
		size_type i=indx.get();
		insert(i, str.c_str(), len);
	}
	// indx�̏ꏊ�ɕ���ch��}�����܂�
	iterator insert(iterator indx, const_reference ch){
		size_type i=indx.get();
		insert(i, &ch, 1);
		iterator it(i, this);
		return it;
	}
	// indx�̏ꏊ��start�`end�̎����������}�����܂�
	template<class InIter> void insert(iterator indx, InIter start, InIter end){
		_Myt str(start, end);
		size_type i=indx.get();
		insert(i, str.c_str(), len);
	}

//max_size
	//�ő�̕����̒����𓾂܂�
	size_type max_size() const{
		return m_Allocator.max_size();
	}

//begin()��end()
	// �ŏ��̕����̃C�^���[�^��Ԃ��܂��iconst�j
	const_iterator begin() const { iterator it(0,this); return it; }
	// �ŏ��̕����̃C�^���[�^��Ԃ��܂�
	iterator begin() { iterator it(0,this); return it; }
	// �I�[�̃C�^���[�^��Ԃ��܂��iconst�j
	const_iterator end() const { iterator it(npos,this); return it; }
	// �I�[�̃C�^���[�^��Ԃ��܂�
	iterator end() { iterator it(npos,this); return it; }

//rbegin()��rend()
	// �t�����猩�čŏ��̕����̃��o�[�X�C�^���[�^��Ԃ��܂��iconst�j
	const_reverse_iterator rbegin() const { iterator it(npos,this); return const_reverse_iterator(it); }
	// �t�����猩�čŏ��̕����̃��o�[�X�C�^���[�^��Ԃ��܂�
	reverse_iterator rbegin() { iterator it(npos,this); return reverse_iterator(it); }
	// �t�����猩�ďI�[�̃��o�[�X�C�^���[�^��Ԃ��܂��iconst�j
	const_reverse_iterator rend() const { iterator it(0,this); return const_reverse_iterator(it); }
	// �t�����猩�ďI�[�̃��o�[�X�C�^���[�^��Ԃ��܂�
	reverse_iterator rend() { iterator it(0,this); return reverse_iterator(it); }

//replace
	// indx�����ڂ���len���������A������str�ƒu�������܂�
	_Myt & replace(size_type indx, size_type len, const _Myt & str){
		erase(indx, len);
		insert(indx, str);
		return *this;
	}
	// indx�����ڂ���len���������A������str��indx2�����ڂ���len2�������ƒu�������܂�
	_Myt & replace(size_type indx, size_type len, const _Myt & str, size_type indx2, size_type len2){
		erase(indx, len);
		insert(indx, str, indx2, len2);
		return *this;
	}
	// indx�����ڂ���len���������AC���ꕶ����str�ƒu�������܂�
	_Myt & replace(size_type indx, size_type len, const char_type * str){
		erase(indx, len);
		insert(indx, str);
		return *this;
	}
	// indx�����ڂ���len���������Alen2�̕���ch�Œu�������܂�
	_Myt & replace(size_type indx, size_type len, size_type len2, char_type ch){
		erase(indx, len);
		insert(indx, len2, ch);
		return *this;
	}

	// start�`end�̋�Ԃ��A������str�Œu�������܂�
	_Myt & replace(iterator start, iterator end, const _Myt & str){
		erase(start, end);
		insert(start, str);
		return *this;
	}
	// start�`end�̋�Ԃ��AC���ꕶ����str�Œu�������܂�
	_Myt & replace(iterator start, iterator end, const char_type * str){
		erase(start, end);
		insert(start, str);
		return *this;
	}
	// start�`end�̋�Ԃ��Alen�̕���ch�Œu�������܂�
	_Myt & replace(iterator start, iterator end, size_type len, char_type ch){
		erase(start, end);
		insert(start, len, ch);
		return *this;
	}
	// start�`end�̋�Ԃ��A�ʃR���e�i��start2�`end2������������Œu�������܂�
	template<class InIter> _Myt & insert(iterator start, iterator end, InIter start2, InIter end2){
		erase(start, end);
		insert(start, start2, end2);
		return *this;
	}

//reserve
	// num�����ȏ���[�ł��郁�������m�ۂ��܂�
	void reserve(size_type num=0){
		size_type len=(num>size()) ? num : size();
		if (len > capacity()) {	//	�m�ۂ��Ȃ���?
			pointer pBuf;
			myAlloc(len+1,pBuf,m_nCapacity);
			if(size()!=0){
				::memcpy(pBuf, m_pszBuffer, (size()+1) * sizeof(char_type));
			}
			else{
				*pBuf='\0';
			}
			myFree(m_pszBuffer);
			m_pszBuffer = pBuf;
		}
	}

//resize
	// ������̃T�C�Y��ύX���܂��B�����Ȃ��������͕���ch�Ŗ��܂�܂�
	void resize(size_type len, char_type ch){
		if(len<0){
			m_nLength=0;
		}
		else if(len<size()){
			m_nLength=len;
		}
		else{
			append(len-size(), ch);
		}
	}
	// ������̃T�C�Y��ύX���܂�
	void resize(size_type len){
		resize(len, '\0');
	}

//swap
	// �Q�̕�����̒��g�����ւ��܂��B�i�A�h���X�����ւ��Ă���̂ŁA�A���P�[�^�������ɓ���ւ��Ă��܂��j
	void swap(_Myt & str){
		pointer tmp=m_pszBuffer;	m_pszBuffer=str.m_pszBuffer;	str.m_pszBuffer=tmp;
		size_type tmp2=m_nCapacity;	m_nCapacity=str.m_nCapacity;	str.m_nCapacity=tmp2;
		tmp2=m_nLength;	m_nLength=str.m_nLength;	str.m_nLength=tmp2;
		A tmp3=m_Allocator;	m_Allocator=str.m_Allocator;	str.m_Allocator=tmp3;
	}

//�����o�ϐ��̏K��
	size_type capacity() const { return m_nCapacity; }
	bool empty() const { return size()==0; }

	size_type	size() const {
		return m_nLength;
	}
	size_type	length() const {
		return size();
	}

protected:
//�����o�ϐ�
	pointer		m_pszBuffer;
	size_type	m_nLength;
	//	nLength!=0 �̂Ƃ��Am_pszBuffer�́A
	//	sizeof(E) * (m_nLength+1)�m�ۂ���Ă���
	//				(��+1�Ȃ̂͏I�[�����񂪂���̂�)
	size_type	m_nCapacity;	//	�m�ۂ����T�C�Y(2^n�P�ʂŊm��)
	A m_Allocator;

//�������m�ۃ��\�b�h
	void myAlloc(size_type nSize,pointer &pBuf,size_type& capa)
		/**
			nSize : �m�ۂ���T�C�Y
			pBuf  : �m�ۂ����|�C���^
			capa  : ���ۂɊm�ۂ����T�C�Y(2^n�ŃA���C�������)
		*/
	{
		//	2^n�ɃA���C������
		size_type n = nSize;
		size_type c = 1;
		while (n!=0){
			n>>=1; c<<=1;
		}
		pBuf = m_Allocator.allocate(c,(pointer)0);//new E[c];
		capa = c;
	}

//������������\�b�h
	void	myFree(pointer pBuf){
		//	myAlloc�Ŋm�ۂ������������������
		if (pBuf!=NULL){
			m_Allocator.deallocate((A::pointer)pBuf, m_nCapacity);//delete [] pBuf;
		}
	}

};  //  end of class basic_string<E,T,A>

//	�I�y���[�^
template <class E,class T,class A>
basic_string<E,T,A>	operator + (E* pBuf,const basic_string<E,T,A>&s2) {
	basic_string<E,T,A>	s1(pBuf);
	s1 += s2;
	return s1;
}

template <class E,class T,class A>
basic_string<E,T,A>	operator + (E ch,const basic_string<E,T,A>&s2) {
	basic_string<E,T,A>	s1(ch);
	s1 += s2;
	return s1;
}
template <class E,class T,class A>
std::basic_ostream<E,T> & operator<<(std::basic_ostream<E,T>& o, const basic_string<E,T,A>& s){
	o.write(s.c_str(), s.size());
	return o;
}

template<class E,class T,class A>
const basic_string<E,T,A>::size_type basic_string<E,T,A>::npos = -1;

template<class E,class T,class A>
basic_string<E,T,A>::char_type basic_string<E,T,A>::nullstr = 0;

typedef basic_string<char> string;

};	//	end of namespace YTL

//	����Ȃ���[���Ȃ��N���X���̂Ɋۈ������������R(`�D�L)�m�ܧ�����

#endif
