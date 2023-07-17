/*
	simple string class
		STLのstring classは、VC++6と7(.NET)で実装が異なるので
		自前で用意する。
		（STLPortも実装が不変とは限らないので）

	implemented by yaneurao(M.Isozaki) '03/04/02
	UNICODEでもうまく動くといいなぁ．．ヽ(´Д｀)ノ

	なんか春休みの宿題って感じ．．

	いろいろ足りないメンバあるかも知れないけど、追加したら、
	やねうらおまでメールしてちょーだい（＾＾；
*/

#ifndef __YTLString_h__
#define __YTLString_h__

//#include "../YTL/exceptions.h"
#include <stdexcept>
#include <iterator>
//#include <iostream>

namespace YTL {
//	非常に単純な実装だけど勘弁してちょ．．(ρ_；)ﾉ
template<class E,
class T = std::char_traits<E>,
class A = std::allocator<E> >
class basic_string {
/**
	STL互換stringクラス(かなりしょぼめ)
	namespaceはYTLに入っている。stdafx.hでusingして使う
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
	static char_type nullstr; // '\0'を指すポインタ

	struct iterator;
	struct const_iterator;
	struct reverse_iterator;
	struct const_reverse_iterator;

	struct iterator {
	private:
		iterator(const const_iterator & i){}//禁止
		iterator(const const_reverse_iterator & i){}//禁止
		iterator & operator = (const const_iterator & i){return *this;}//禁止
		iterator & operator = (const const_reverse_iterator & i){return *this;}//禁止
	public:
		/* iteratorも定義しちゃうぞーヽ（´ー｀）ノ */
		iterator() : m_nPos(npos),m_pStr(NULL) {}
		iterator(const iterator & i) : m_nPos(i.m_nPos), m_pStr(i.m_pStr) {}
		iterator(size_type n,_Myt * pStr)
			: m_nPos(n),m_pStr(pStr) {	set(n);	}

		//	代入オペレータ
		iterator & operator = (const iterator & i){
			m_nPos=i.m_nPos;
			m_pStr=i.m_pStr;
			return *this;
		}
		//iterator & operator = (size_type size){
		//	set(size);
		//	return *this;
		//}

		//	アクセッサ
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

		//	演算子
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
		///	加減算のためのオペレータ
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

		//	比較オペレータ
		bool	operator == (const iterator& it) const {
			return (m_nPos == it.m_nPos) && (m_pStr == it.m_pStr);
		}
		bool	operator != (const iterator& it) const {
			return !(*this == it);
		}
		//	size_typeとの比較も用意しないとstring::nposと比較できない
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
		size_type	m_nPos;	//	0オリジンなポジションで保存するとしよう
		_Myt * m_pStr; // 親へのポインタが無いと加減算のときにendの判定がでけん
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
		reverse_iterator(const const_iterator & i){}//禁止
		reverse_iterator(const const_reverse_iterator & i){}//禁止
		reverse_iterator & operator = (const const_iterator & i){return *this;}//禁止
		reverse_iterator & operator = (const const_reverse_iterator & i){return *this;}//禁止
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

		//	演算子
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

//コンストラクタ・デストラクタ
	//空の文字列を作るコンストラクタ
	explicit basic_string(const A & a=A()):m_Allocator(a),m_pszBuffer(NULL),m_nLength(0),m_nCapacity(0) {}
	// pszから始まるn文字分の文字列を内容とする文字列を作るコンストラクタ
	basic_string(const_pointer psz, size_type len, const A & a=A())
		:m_Allocator(a),m_pszBuffer(NULL),m_nLength(0),m_nCapacity(0)
	{
		assign(psz, len);
	}
	// 文字列sを内容とする文字列を作るコンストラクタ
	basic_string(const _Myt & s, const A & a=A())
		:m_Allocator(a),m_pszBuffer(NULL),m_nLength(0),m_nCapacity(0)
	{
		assign(s);
	}
	// C言語文字列pszを内容とする文字列を作るコンストラクタ
	basic_string(const char_type * psz, const A & a=A())
		:m_Allocator(a),m_pszBuffer(NULL),m_nLength(0),m_nCapacity(0)
	{
		assign(psz);
	}
	// 文字chをlen個並べた文字列を作るコンストラクタ
	basic_string(size_type len, char_type ch, const A & a=A())
		:m_Allocator(a),m_pszBuffer(NULL),m_nLength(0),m_nCapacity(0)
	{
		assign(len, ch);
	}
	// 文字chを1個だけの文字列を作るコンストラクタ
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
	// デストラクタ
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

//比較演算子
	bool operator == (const _Myt & s) const {
		if (m_pszBuffer==NULL && s.m_pszBuffer==NULL) return true;
		if (m_nLength!=s.m_nLength) return false; // 必要条件を満たしていない
		//	片方だけNULLである条件は↑に含まれるのでチェックしなくて良い
		//	マルチバイトかも知れないのでコンペアは自前でやらないといけない
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
		//	何でもいいから比較できないとlistとか定義できなくてマズー(ﾟдﾟ)
		return this < &s; // オブジェクトのアドレス比較でいいやヽ（´ー｀）ノ
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
	// string[n]の例外スルー版
	reference at (size_type n){
		if (n<0 || n>size()) {
			throw std::out_of_range("Out of range error! in YTL::string::at(size_type n)");
		}
		return (*this)[n];
	}
	// string[n]の例外スルー版(const)
	const_reference at (size_type n) const {
		if (n<0 || n>size()) {
			throw std::out_of_range("Out of range error! in YTL::string::at(size_type n)");
		}
		return (*this)[n];
	}

//append
	// sから始まるn文字分の文字列を、stringの末尾に挿入
	_Myt & append(const_pointer s, size_type len){
		if (0<len && s!=NULL){
			size_type nLength = size() + len;
			if (nLength+1 > capacity()) {	//	確保しなおし?
				pointer pBuf;
				myAlloc(nLength+1,pBuf,m_nCapacity);
				if (size()!=0){
					::memcpy(pBuf,c_str(),size() * sizeof(char_type));
				}
				::memcpy(pBuf+size(),s,len * sizeof(char_type));
				*(pBuf+size()+len) = 0; // 終端文字列
				myFree(m_pszBuffer);
				m_pszBuffer = pBuf;
			} else {
				::memcpy(m_pszBuffer+size(),s,len * sizeof(char_type));
				*((pointer)(c_str()+size()+len)) = 0; // 終端文字列
			}
			m_nLength = nLength;
		}
		return *this;
	}
	// 文字列sを、stringの末尾に挿入
	_Myt & append(const basic_string<E,T,A>&s){
		append(s.c_str(), s.size());
		return *this;
	}
	// 文字列sのindx文字目からlen文字分を、stringの末尾に挿入
	_Myt & append(const basic_string<E,T,A>&s, size_type indx, size_type len){
		basic_string<E,T,A> ss;
		ss.assign(s, indx, len);//ss=s.substr(indx, len);
		append(ss.c_str(), ss.size());
		return *this;
	}
	// C言語文字列sを、stringの末尾に挿入
	_Myt & append(const char_type * s){
		if(s==NULL)return *this;
		size_type len=0;
		while(s[len])len++;
		append(s, len);
		return *this;
	}
	// 文字chをlen個並べたものを、stringの末尾に挿入
	_Myt & append(size_type len, char_type ch){
		_Myt ss(len, ch);
		append(ss.c_str(), ss.size());
		return *this;
	}
	// startからend手前までのイタレータが指す文字列を、stringの末尾に挿入
	template<class InIter> basic_string<E,T,A>& append(InIter start, InIter end){
		_Myt ss(start, end);
		append(ss.c_str(), ss.size());
		return *this;
	}

//assign
	// sから始まるn文字分の文字列を、stringに代入
	_Myt & assign(const_pointer s, size_type len){
		myFree(m_pszBuffer); // バッファを解放
		if (len == 0 || s==NULL){
			//	文字ないやん？
			m_pszBuffer = NULL;
			m_nLength = 0;
			m_nCapacity = 0;
		} else {
			m_nLength = len;
			myAlloc(len+1,m_pszBuffer,m_nCapacity);
			::memcpy(m_pszBuffer,s,len * sizeof(char_type));
			m_pszBuffer[len]='\0'; // c_strのために終端は\0で埋める
		}
		return *this;
	}
	// 文字列sを、stringに代入
	_Myt & assign(const basic_string<E,T,A>&s){
		//	s = s;のような自己代入に対してセーフでなければならない
		if (this==&s) return *this; // コピーする必要なし！
		assign(s.c_str(), s.size());
		return *this;
	}
	// 文字列sのindx文字目からlen文字分を、stringに代入
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
	// C言語文字列sを、stringに代入
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
	// 文字chをlen個並べたものを、stringに代入
	_Myt & assign(size_type len, char_type ch){
		myFree(m_pszBuffer); // バッファを解放
		if (len == 0){
			//	文字ないやん？
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
	// startからend手前までのイタレータが指す文字列を、stringに代入
	template<class InIter> _Myt & assign(InIter start, InIter end){
		size_type len=0;
		{
			InIter it=start;
			while(it!=end){it++;len++;}
		}

		myFree(m_pszBuffer); // バッファを解放
		if (len == 0){
			//	文字ないやん？
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
	//文字列同士の比較
	int compare(const _Myt & s) const{
		if((*this)==s)return 0;
		else if((*this)<s)return -1;
		return 1;
	}
	//文字列同士の比較その２
	int compare(size_type indx, size_type len, const _Myt & s) const{
		_Myt ss(*this, indx, len);
		return ss.compare(s);
	}
	//文字列同士の比較その３
	int compare(size_type indx, size_type len, const basic_string<E,T,A> *s, size_type indx2, size_type len2) const{
		_Myt ss(*this, indx, len);
		return ss.compare(s->substr(indx2, len2));
	}
	//文字列同士の比較その４
	int compare(const char_type *s) const{
		_Myt ss(s);
		return compare(ss);
	}
	//文字列同士の比較その５
	int compare(size_type indx, size_type len, const_pointer s, size_type len2=npos) const{
		_Myt ss(*this, indx, len);
		_Myt ss2(s, len2);
		return ss.compare(ss2);
	}

//substr
	//pos文字目からn文字分の文字列を生成して返す
	basic_string substr(size_type pos = 0,size_type n = npos) const{
		_Myt s;
		s.assign(*this, pos, n);
		return s;
	}

//erase
	// firstからlast手前までの文字を削除
	iterator erase(iterator first, iterator last){
		// first←begin()　＆　last←end() のとき、文字列をまるごと消す
		size_type p1 = first.get();
		if (p1==npos) p1 = length();
		if (p1>length()) p1 = length();
		size_type p2 = last.get();
		if (p2==npos) p2 = length();
		if (p2>length()) p2 = length();
		int l = p2-p1;
		int ts = (length()-p2+1); // 転送文字数
		memmove(m_pszBuffer+p1,m_pszBuffer+p2,ts * sizeof(char_type));//memcpyはコピー順不定なのでmemmoveを使う
		m_nLength -= l;
		if (p1>=length()) first.set(npos);
		return first;
	}
	// p0文字目からn文字分を削除
	_Myt & erase(size_type p0 = 0, size_type n = npos){
		// p0←0　＆　n←npos のとき、文字列をまるごと消す
		iterator it1(p0,this);
		iterator it2;
		if(n==npos)it2=iterator(npos,this);
		else it2=iterator(p0+n,this);
		erase(it1,it2);
		return *this;
	}
	// itの示す文字を削除
	iterator erase(iterator it){
		iterator it2 = it;
		it2++;
		return erase(it,it2);
	}
    
//copy
	//バッファsに、stringのlen文字目以降のindx文字分をコピーし、コピーした文字数を返します
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

//c_str()とdata()
	//C言語文字列を返します。（stringが'\0'を含む場合は、'\0'の手前までがC言語文字として正常に認識されます）
	const char_type * c_str() const {
		return m_pszBuffer!=NULL?m_pszBuffer:&nullstr;
	}
	//内部データの先頭文字を示すポインタを返します。
	const_pointer data() const{
		return c_str();
	}

//find - 検索。対象が見つかったとき、見つかった文字番号（先頭が０）を返しまする
	// sから始まるn文字分の文字列を、stringのpos文字目以降から検索
	size_type find(const_pointer s, size_type pos, size_type n) const{
		if (size()==0) return npos;
		if (n==0)return pos;
		if (s==NULL || pos == npos) return npos;
		pointer buf=m_pszBuffer;//メンバ変数を落として高速化する（プログラムは少し分かりづらくなる・・・？）
		for(size_type i=pos, ii=size();i<ii;i++){
			if (buf[i] == s[0]){
				int j=1;
				while(1){
					if (j==n) return i; // 一致
					if (ii<=i+j)break;
					if (buf[i+j]!=s[j]) break; // 一致しないのでスキップ
					j++;
				}
			}
		}
		return npos;
	}
	// C言語文字列sを、stringのpos文字目以降から検索
	size_type find(const char_type *s, size_type pos = 0) const{
		if(s==NULL)return find(s, pos, 0);
		int len=0;
		while(s[len])len++;
		return find(s, pos, len);
	}
	// 文字列strを、stringのpos文字目以降から検索
	size_type find(const basic_string& str, size_type pos = 0) const{
		return find(str.c_str(), pos, str.size());
	}
	// 文字cを、stringのpos文字目以降から検索
	size_type find(char_type ch, size_type pos = 0) const{
		return find_first_of(ch, pos);
	}

//rfind - 逆方向検索。対象が見つかったとき、見つかった文字番号（先頭が０）を返しまする
	// sから始まるn文字分の文字列を、stringのpos文字目以前から逆方向検索
	size_type rfind(const_pointer s, size_type pos, size_type n) const{
		if (size()==0) return npos;
		if (n==0)return pos;
		if (s==NULL || pos==0)return npos;
		if (pos == npos) pos = length();
		pointer buf=m_pszBuffer;//メンバ変数を落として高速化する（プログラムは少し分かりづらくなる・・・？）
		for(size_type i=pos-1,ii=size();0<=i;i--){
			if (buf[i] == s[0]){
				int j=1;
				while(1){
					if (j==n) return i; // 一致
					if (ii<=i+j)break;
					if (buf[i+j]!=s[j]) break; // 一致しないのでスキップ
					j++;
				}
			}
		}
		return npos;
	}
	// C言語文字列sを、stringのpos文字目以前から逆方向検索
	size_type rfind(const char_type *s, size_type pos = npos) const{
		if(s==NULL)return rfind(s, pos, 0);
		int len=0;
		while(s[len])len++;
		return rfind(s, pos, len);
	}
	// 文字列strを、stringのpos文字目以前から逆方向検索
	size_type rfind(const basic_string& str, size_type pos = npos) const{
		return rfind(str.c_str(),pos,str.size());
	}
	// 文字cを、stringのpos文字目以前から逆方向検索
	size_type rfind(char_type ch, size_type pos = npos) const{
		return find_last_of(ch, pos);
	}

//find_first_of
	// sから始まるn文字分の文字列に含まれるいずれかの文字を、stringのindx文字目以降から検索
	size_type find_first_of(const_pointer s, size_type indx, size_type len) const{
		if(size()==0 || len==0 || s==NULL || indx==npos) return npos;
		pointer buf=m_pszBuffer;//メンバ変数を落として高速化する（プログラムは少し分かりづらくなる・・・？）
		for(size_type i=indx, ii=size();i<ii;i++){
			for(size_type j=0;j<len;j++){
				if(buf[i]==s[j]){
					return i; // 一致
				}
			}
		}
		return npos;
	}
	// 文字列sに含まれるいずれかの文字を、stringのindx文字目以降から検索
	size_type find_first_of(const _Myt &s, size_type indx=0) const{
		return find_first_of(s.c_str(), indx, s.size());
	}
	// C言語文字列sに含まれるいずれかの文字を、stringのindx文字目以降から検索
	size_type find_first_of(const char_type *s, size_type indx=0) const{
		if(s==NULL)return npos;
		size_type len=0;
		while(s[len])len++;
		return find_first_of(s, indx, len);
	}
	// 文字cを、stringのindx文字目以降から検索
	size_type find_first_of(char_type ch, size_type indx=0) const{
		for(size_type i=indx, ii=size();i<ii;i++){
			if(*(m_pszBuffer+i)==ch)return i;
		}
		return npos;
	}

//find_first_not_of
	// sから始まるn文字分の文字列に含まれないいずれかの文字を、stringのindx文字目以降から検索
	size_type find_first_not_of(const_pointer s, size_type indx, size_type len) const{
		if(size()==0 || indx==npos) return npos;
		if(len==0 || s==NULL)return indx;//これ、順番どっちにするか難しいところ・・・？実際の実装を見ないとわかりまへん。
		pointer buf=m_pszBuffer;//メンバ変数を落として高速化する（プログラムは少し分かりづらくなる・・・？）
		for(size_type i=indx, ii=size();i<ii;i++){
			bool flag=true;
			for(size_type j=0;j<len;j++){
				if(buf[i]==s[j]){
					flag=false;
				}
			}
			if(flag)return i; // 不一致
		}
		return npos;
	}
	// 文字列sに含まれないいずれかの文字を、stringのindx文字目以降から検索
	size_type find_first_not_of(const _Myt &s, size_type indx=0) const{
		return find_first_not_of(s.c_str(), indx, s.size());
	}
	// C言語文字列sに含まれないいずれかの文字を、stringのindx文字目以降から検索
	size_type find_first_not_of(const char_type *s, size_type indx=0) const{
		if(s==NULL)return 0;
		size_type len=0;
		while(s[len])len++;
		return find_first_not_of(s, indx, len);
	}
	// 文字c以外の文字を、stringのindx文字目以降から検索
	size_type find_first_not_of(char_type ch, size_type indx=0) const{
		for(size_type i=indx, ii=size();i<ii;i++){
			if(*(m_pszBuffer+i)!=ch)return i;
		}
		return npos;
	}

//find_last_of
	// sから始まるn文字分の文字列に含まれるいずれかの文字を、stringのindx文字目以前から逆方向検索
	size_type find_last_of(const_pointer s, size_type indx, size_type len) const{
		if(size()==0 || len==0 || s==NULL || indx==0) return npos;
		if(indx == npos) indx = length();
		pointer buf=m_pszBuffer;//メンバ変数を落として高速化する（プログラムは少し分かりづらくなる・・・？）
		for(size_type i=indx-1;0<=i;i--){
			for(size_type j=0;j<len;j++){
				if(buf[i]==s[j]){
					return i; // 一致
				}
			}
		}
		return npos;
	}
	// 文字列sに含まれるいずれかの文字を、stringのindx文字目以前から逆方向検索
	size_type find_last_of(const _Myt & s, size_type indx=npos) const{
		return find_last_of(s.c_str(), indx, s.size());
	}
	// C言語文字列sに含まれるいずれかの文字を、stringのindx文字目以前から逆方向検索
	size_type find_last_of(const char_type * s, size_type indx=npos) const{
		if(s==NULL)return npos;
		size_type len=0;
		while(s[len])len++;
		return find_last_of(s, indx, len);
	}
	// 文字cを、stringのindx文字目以前から逆方向検索
	size_type find_last_of(char_type ch, size_type indx=npos) const{
		for(size_type i=indx-1;0<=i;i--){
			if(m_pszBuffer[i]==ch)return i;
		}
		return npos;
	}

//find_last_not_of
	// sから始まるn文字分の文字列に含まれないいずれかの文字を、stringのindx文字目以前から逆方向検索
	size_type find_last_not_of(const_pointer s, size_type indx, size_type len) const{
		if(size()==0 || indx==0) return npos;
		if(len==0 || s==NULL)return (indx ? (indx-1) : npos);
		if(indx == npos) indx = length();
		pointer buf=m_pszBuffer;//メンバ変数を落として高速化する（プログラムは少し分かりづらくなる・・・？）
		for(size_type i=indx-1;0<=i;i--){
			bool flag=true;
			for(size_type j=0;j<len;j++){
				if(buf[i]==s[j]){
					flag=false;
					break;
				}
			}
			if(flag)return i; // 不一致
		}
		return npos;
	}
	// 文字列sに含まれないいずれかの文字を、stringのindx文字目以前から逆方向検索
	size_type find_last_not_of(const _Myt & s, size_type indx=npos) const{
		return find_last_not_of(s.c_str(), indx, s.size());
	}
	// C言語文字列sに含まれないいずれかの文字を、stringのindx文字目以前から逆方向検索
	size_type find_last_not_of(const char_type *s, size_type indx=npos) const{
		if(s==NULL)return size()-1;
		size_type len=0;
		while(s[len])len++;
		return find_last_not_of(s, indx, len);
	}
	// 文字c以外の文字を、stringのindx文字目以前から逆方向検索
	size_type find_last_not_of(char_type ch, size_type indx=npos) const{
		for(size_type i=indx-1;0<=i;i--){
			if(m_pszBuffer[i]!=ch)return i;
		}
		return npos;
	}

//get_allocator
	// アロケータを返します
	A get_allocator() const{
		return m_Allocator;
	}

//insert
	// indx文字目にstrで始まるlen文字の文字列を挿入します
	_Myt & insert(size_type indx, const_pointer str, size_type len){
		if(len==0)return *this;

		size_type i=indx.get();
		if(i==npos || size()<i)i=size();
		size_type nLength = size() + len;
		if (nLength+1 > capacity()) {	//	確保しなおし?
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
	// indx文字目にC言語文字列strを挿入します
	_Myt & insert(size_type indx, const char_type *str){
		if(s==NULL)return *this;
		size_type len=0;
		while(str[len])len++;
		insert(indx, str, len);
		return *this;
	}
	// indx文字目に文字列strを挿入します
	_Myt & insert(size_type indx, const _Myt & str){
		insert(indx, str.c_str(), str.size());
		return *this;
	}
	// indx文字目にstrのindex2文字目からlen文字分の文字列を挿入します
	_Myt & insert(size_type indx, const _Myt & str, size_type indx2, size_type len){
		insert(indx, str.substr(indx2, len));
		return *this;
	}
	// indx文字目に文字chをlen個並べた文字列を挿入します
	_Myt & insert(size_type indx, size_type len, char_type ch){
		_Myt str(len, ch);
		insert(indx, str.c_str(), len);
		return *this;
	}
	// indxの場所に文字chをlen個並べた文字列を挿入します
	void insert(iterator indx, size_type len, const_reference ch){
		_Myt str(len, ch);
		size_type i=indx.get();
		insert(i, str.c_str(), len);
	}
	// indxの場所に文字chを挿入します
	iterator insert(iterator indx, const_reference ch){
		size_type i=indx.get();
		insert(i, &ch, 1);
		iterator it(i, this);
		return it;
	}
	// indxの場所にstart～endの示す文字列を挿入します
	template<class InIter> void insert(iterator indx, InIter start, InIter end){
		_Myt str(start, end);
		size_type i=indx.get();
		insert(i, str.c_str(), len);
	}

//max_size
	//最大の文字の長さを得ます
	size_type max_size() const{
		return m_Allocator.max_size();
	}

//begin()とend()
	// 最初の文字のイタレータを返します（const）
	const_iterator begin() const { iterator it(0,this); return it; }
	// 最初の文字のイタレータを返します
	iterator begin() { iterator it(0,this); return it; }
	// 終端のイタレータを返します（const）
	const_iterator end() const { iterator it(npos,this); return it; }
	// 終端のイタレータを返します
	iterator end() { iterator it(npos,this); return it; }

//rbegin()とrend()
	// 逆側から見て最初の文字のリバースイタレータを返します（const）
	const_reverse_iterator rbegin() const { iterator it(npos,this); return const_reverse_iterator(it); }
	// 逆側から見て最初の文字のリバースイタレータを返します
	reverse_iterator rbegin() { iterator it(npos,this); return reverse_iterator(it); }
	// 逆側から見て終端のリバースイタレータを返します（const）
	const_reverse_iterator rend() const { iterator it(0,this); return const_reverse_iterator(it); }
	// 逆側から見て終端のリバースイタレータを返します
	reverse_iterator rend() { iterator it(0,this); return reverse_iterator(it); }

//replace
	// indx文字目からlen文字分を、文字列strと置き換えます
	_Myt & replace(size_type indx, size_type len, const _Myt & str){
		erase(indx, len);
		insert(indx, str);
		return *this;
	}
	// indx文字目からlen文字分を、文字列strのindx2文字目からlen2文字分と置き換えます
	_Myt & replace(size_type indx, size_type len, const _Myt & str, size_type indx2, size_type len2){
		erase(indx, len);
		insert(indx, str, indx2, len2);
		return *this;
	}
	// indx文字目からlen文字分を、C言語文字列strと置き換えます
	_Myt & replace(size_type indx, size_type len, const char_type * str){
		erase(indx, len);
		insert(indx, str);
		return *this;
	}
	// indx文字目からlen文字分を、len2個の文字chで置き換えます
	_Myt & replace(size_type indx, size_type len, size_type len2, char_type ch){
		erase(indx, len);
		insert(indx, len2, ch);
		return *this;
	}

	// start～endの区間を、文字列strで置き換えます
	_Myt & replace(iterator start, iterator end, const _Myt & str){
		erase(start, end);
		insert(start, str);
		return *this;
	}
	// start～endの区間を、C言語文字列strで置き換えます
	_Myt & replace(iterator start, iterator end, const char_type * str){
		erase(start, end);
		insert(start, str);
		return *this;
	}
	// start～endの区間を、len個の文字chで置き換えます
	_Myt & replace(iterator start, iterator end, size_type len, char_type ch){
		erase(start, end);
		insert(start, len, ch);
		return *this;
	}
	// start～endの区間を、別コンテナのstart2～end2を示す文字列で置き換えます
	template<class InIter> _Myt & insert(iterator start, iterator end, InIter start2, InIter end2){
		erase(start, end);
		insert(start, start2, end2);
		return *this;
	}

//reserve
	// num文字以上収納できるメモリを確保します
	void reserve(size_type num=0){
		size_type len=(num>size()) ? num : size();
		if (len > capacity()) {	//	確保しなおし?
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
	// 文字列のサイズを変更します。長くなった部分は文字chで埋まります
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
	// 文字列のサイズを変更します
	void resize(size_type len){
		resize(len, '\0');
	}

//swap
	// ２つの文字列の中身を入れ替えます。（アドレスを入れ替えているので、アロケータも同時に入れ替えています）
	void swap(_Myt & str){
		pointer tmp=m_pszBuffer;	m_pszBuffer=str.m_pszBuffer;	str.m_pszBuffer=tmp;
		size_type tmp2=m_nCapacity;	m_nCapacity=str.m_nCapacity;	str.m_nCapacity=tmp2;
		tmp2=m_nLength;	m_nLength=str.m_nLength;	str.m_nLength=tmp2;
		A tmp3=m_Allocator;	m_Allocator=str.m_Allocator;	str.m_Allocator=tmp3;
	}

//メンバ変数の習得
	size_type capacity() const { return m_nCapacity; }
	bool empty() const { return size()==0; }

	size_type	size() const {
		return m_nLength;
	}
	size_type	length() const {
		return size();
	}

protected:
//メンバ変数
	pointer		m_pszBuffer;
	size_type	m_nLength;
	//	nLength!=0 のとき、m_pszBufferは、
	//	sizeof(E) * (m_nLength+1)確保されている
	//				(↑+1なのは終端文字列があるので)
	size_type	m_nCapacity;	//	確保したサイズ(2^n単位で確保)
	A m_Allocator;

//メモリ確保メソッド
	void myAlloc(size_type nSize,pointer &pBuf,size_type& capa)
		/**
			nSize : 確保するサイズ
			pBuf  : 確保したポインタ
			capa  : 実際に確保したサイズ(2^nでアラインされる)
		*/
	{
		//	2^nにアラインする
		size_type n = nSize;
		size_type c = 1;
		while (n!=0){
			n>>=1; c<<=1;
		}
		pBuf = m_Allocator.allocate(c,(pointer)0);//new E[c];
		capa = c;
	}

//メモリ解放メソッド
	void	myFree(pointer pBuf){
		//	myAllocで確保したメモリを解放する
		if (pBuf!=NULL){
			m_Allocator.deallocate((A::pointer)pBuf, m_nCapacity);//delete [] pBuf;
		}
	}

};  //  end of class basic_string<E,T,A>

//	オペレータ
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

//	こんなしょーもないクラス作るのに丸一日かかったよヽ(`Д´)ノｳﾜｧｧｧｧｧﾝ

#endif
