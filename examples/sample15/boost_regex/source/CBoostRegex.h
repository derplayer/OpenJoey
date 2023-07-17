#ifndef __yanePlugin_boost_regex_h__
#define __yanePlugin_boost_regex_h__

#include "IRegex.h"

/**

　このプログラムは、本来ObjectCreaterを通じてDLLから呼び出して使います。
　けれどもDLLを通さずに使いたい場合は、このヘッダをインクリメントして使ってください。またこの時、次のことに気をつけてください。
　　・　CBoostRegex.cppと、boostのregexフォルダのsrc.cppの２つをプロジェクトに加える必要があります。
　　・　src.cppは、プリコンパイル済みヘッダを使用しない設定でコンパイルしてください。

　DLLを通さずに使う場合、CBoostRegexがboost::regexを使った実装クラスになります。

DLLを通さずに使う使い方：
　その１：//DLLを通す場合と互換
	IRegex * regex=new CBoostRegex();
	regex->Search(...);
	delete regex;
　その２：//おすすめ～♪（こちらもDLLを通す場合と互換）
	smart_ptr<IRegex> regex=new CBoostRegex();
	regex->Search(...);
　その３：//簡単に使う場合
	CBoostRegex regex;
	regex.Search(...);
　その４：//（非標準）IRegex型で簡単に使う場合。↓regexのスコープが終わるまで、一時オブジェクトは生きる（const参照以外への代入は非標準らしい）
	IRegex & regex=CBoostRegex();
	regex.Search(...);
*/

namespace yaneuraoGameSDK3rd{
namespace Dll{

//+[CRegexResults]
template <class E>
class CRegexResults : public IRegexResults<E>{
/**
　使い方：
	CBoostRegexのGetResultsメソッドの戻り値として使う。
	IRegexResults*として扱うことになるので、詳しくはIRegexResultsの説明を参照。
*/
public:
	void set_results(const boost::match_results<basic_string<E>::const_iterator> & results, size_t pos){
		size_t size=results.size();
		set_size(size);
		for(size_t i=0;i<size;i++){
			set_str(i, results.str((int)i));//なんでこっちが(int)で
			set_position(i, results.position((unsigned int)i)+pos);//こっちが(unsigned int)になるかは謎・・・boostに聞いてください；；
		}
	}
	void set_size(size_t s)
	{
		m_size=s;
		m_str.resize(s);
		m_position.resize(s);
	}
	void set_str(size_t n, const basic_string<E> & str){
		m_str[n]=str;
	}
	void set_position(size_t n, size_t pos){
		m_position[n]=(int)pos;
	}

	//+[virtualメソッド]
	virtual basic_string<E> str(size_t n=0) const
	{
		return m_str[n];
	}
	virtual size_t position(size_t n=0) const
	{
		return m_position[n];
	}
	virtual size_t length(size_t n=0) const
	{
		return m_str[n].size();
	}
	virtual size_t size() const
	{
		return m_size;
	}
	//-[virtualメソッド]
protected:
	size_t m_size;
	vector<basic_string<E> > m_str;
	vector<size_t> m_position;
};
//-[CRegexResults]

class CBoostRegex : public IRegex{
/**
　使い方：
	IRegexとして扱ってください。
	詳しい説明はIRegex参照。
*/
public:
	typedef std::list<CRegexResults<char> > results_list_type;
	typedef std::list<CRegexResults<wchar_t> > wresults_list_type;
protected:
	CRegexResults<char> m_results;
	CRegexResults<wchar_t> m_wresults;
	results_list_type m_results_list;
	wresults_list_type m_wresults_list;
public:
	//+[コンストラクタ・デストラクタ]
	CBoostRegex();
	virtual ~CBoostRegex();
	//-[コンストラクタ・デストラクタ]
	//+[GetResults]
	virtual const IRegexResults<char> * GetResults() const;
	virtual const IRegexResults<wchar_t> * GetWResults() const;
	virtual const IRegexResults<char> * GetJResults() const;
	//-[GetResults]
	//+[SearchIt]
	bool SearchIt(const basic_string<char>::const_iterator & begin, const basic_string<char>::const_iterator & end, size_t pos, const basic_string<char> & pat, const char * opt="");
	bool WSearchIt(const basic_string<wchar_t>::const_iterator & begin, const basic_string<wchar_t>::const_iterator & end, size_t pos, const basic_string<wchar_t> & pat, const char * opt="");
	//-[SearchIt]
	//+[Search]
	virtual bool Search(const basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, const char * opt="");
	virtual bool Search(const basic_string<char> & str, const basic_string<char> & pat, const char * opt="");
	virtual bool WSearch(const basic_string<wchar_t> & str, size_t pos1, size_t pos2, const basic_string<wchar_t> & pat, const char * opt="");
	virtual bool WSearch(const basic_string<wchar_t> & str, const basic_string<wchar_t> & pat, const char * opt="");
	virtual bool JSearch(IJcodeConverter & conv, const basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, const char * opt="");
	virtual bool JSearch(IJcodeConverter & conv, const basic_string<char> & str, const basic_string<char> & pat, const char * opt="");
	//-[Search]
	//+[Replace]
	virtual size_t Replace(basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, IRegexReplaceCallback<char> & clbk, const char * opt="");
	virtual size_t Replace(basic_string<char> & str, const basic_string<char> & pat, IRegexReplaceCallback<char> & clbk, const char * opt="");
	virtual size_t Replace(basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, const basic_string<char> & exp, const char * opt="");
	virtual size_t Replace(basic_string<char> & str, const basic_string<char> & pat, const basic_string<char> & exp, const char * opt="");

	virtual size_t WReplace(basic_string<wchar_t> & str, size_t pos1, size_t pos2, const basic_string<wchar_t> & pat, IRegexReplaceCallback<wchar_t> & clbk, const char * opt="");
	virtual size_t WReplace(basic_string<wchar_t> & str, const basic_string<wchar_t> & pat, IRegexReplaceCallback<wchar_t> & clbk, const char * opt="");
	virtual size_t WReplace(basic_string<wchar_t> & str, size_t pos1, size_t pos2, const basic_string<wchar_t> & pat, const basic_string<wchar_t> & exp, const char * opt="");
	virtual size_t WReplace(basic_string<wchar_t> & str, const basic_string<wchar_t> & pat, const basic_string<wchar_t> & exp, const char * opt="");

	virtual size_t JReplace(IJcodeConverter & conv, basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, IRegexReplaceCallback<char> & clbk, const char * opt="");
	virtual size_t JReplace(IJcodeConverter & conv, basic_string<char> & str, const basic_string<char> & pat, IRegexReplaceCallback<char> & clbk, const char * opt="");
	virtual size_t JReplace(IJcodeConverter & conv, basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, const basic_string<char> & exp, const char * opt="");
	virtual size_t JReplace(IJcodeConverter & conv, basic_string<char> & str, const basic_string<char> & pat, const basic_string<char> & exp, const char * opt="");
	//-[Replace]
	//+[Next]
	virtual bool Next();
	virtual bool WNext();
	virtual bool JNext();
	//-[Next]

	//+[その他]
	void push_results(const boost::match_results<basic_string<char>::const_iterator> & res, size_t pos);
	void push_wresults(const boost::match_results<basic_string<wchar_t>::const_iterator> & res, size_t pos);
	void push_jresults(IJcodeConverter & conv, const boost::match_results<basic_string<wchar_t>::const_iterator> & res, size_t pos);
	//-[その他]
};

} // end of namespace yaneuraoGameSDK3rd
} // end of namespace Auxiliary

#endif
