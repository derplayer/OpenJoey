#include "stdafx.h"
#include "IRegex.h"
#include <boost/regex.hpp>
#include "CBoostRegex.h"

namespace yaneuraoGameSDK3rd{
namespace Dll{

namespace boost_regex_local{

class Searcher{
public:
	Searcher(CBoostRegex & c, size_t pos) : m_regex(c), m_pos(pos) {}
	bool operator()(const boost::match_results<basic_string<char>::const_iterator> & res){
		m_regex.push_results(res, m_pos);
		return true;
	}
protected:
	Searcher & operator = (const Searcher & s){}//代入演算子を生成できません・・・とかいう警告を出さなくするため。

	CBoostRegex & m_regex;
	size_t m_pos;
};
class WSearcher{
public:
	WSearcher(CBoostRegex & c, size_t pos) : m_regex(c), m_pos(pos){}
	bool operator()(const boost::match_results<basic_string<wchar_t>::const_iterator>& res){
		m_regex.push_wresults(res, m_pos);
		return true;
	}
protected:
	WSearcher & operator = (const WSearcher & s){}//代入演算子を生成できません・・・とかいう警告を出さなくするため。

	CBoostRegex & m_regex;
	size_t m_pos;
};
class JSearcher{
public:
	JSearcher(IJcodeConverter & conv, CBoostRegex & c, size_t pos) : m_conv(conv), m_regex(c), m_pos(pos) {}
	bool operator()(const boost::match_results<basic_string<wchar_t>::const_iterator>& res){
		m_regex.push_jresults(m_conv, res, m_pos);
		return true;
	}
protected:
	JSearcher & operator = (const JSearcher & s){}//代入演算子を生成できません・・・とかいう警告を出さなくするため。

	IJcodeConverter & m_conv;
	CBoostRegex & m_regex;
	size_t m_pos;
};

class Replacer : public IRegexReplaceCallback<char>{
public:
	Replacer(const basic_string<char> & exp) : m_exp(exp) {}
	virtual ~Replacer(){}
	virtual basic_string<char> ReplaceCallback(const IRegexResults<char> * results){
		typedef boost::regex_traits<char> traits;
		basic_string<char> str;
		for(basic_string<char>::size_type pos=0;pos<m_exp.size();++pos){
			if(traits::syntax_type(m_exp[pos])==traits::syntax_slash){
				++pos;
				if(m_exp.size()<=pos)break;
				if(traits::is_class(m_exp[pos], traits::char_class_digit)){
					size_t n=0;
					do{
						n=n*10+traits::toi(m_exp[pos]);
						++pos;
					}while(pos<m_exp.size() && traits::is_class(m_exp[pos], traits::char_class_digit));
					--pos;
					if(n<results->size()){
						str.append(results->str((int)n));
					}
				} else switch(traits::syntax_type(m_exp[pos])) {
					case traits::syntax_slash:	str.append(1, '\\');	break;
					case traits::syntax_n:	str.append(1, '\n');	break;
					case traits::syntax_t:	str.append(1, '\t');	break;
					case traits::syntax_r:	str.append(1, '\r');	break;
					case traits::syntax_f:	str.append(1, '\f');	break;
					case traits::syntax_a:	str.append(1, '\a');	break;
					case traits::syntax_b:	str.append(1, '\b');	break;
					case traits::syntax_e:	str.append(1, '\x1b');	break;
					default: str.append(1, m_exp[pos]);	break;
				}
			} else {
				str.append(1, m_exp[pos]);
			}
		}
		return str;
	}
protected:
	basic_string<char> m_exp;
};

class WReplacer : public IRegexReplaceCallback<wchar_t>{
public:
	WReplacer(const basic_string<wchar_t> & exp) : m_exp(exp) {}
	virtual ~WReplacer(){}
	virtual basic_string<wchar_t> ReplaceCallback(const IRegexResults<wchar_t> * results){
		typedef boost::regex_traits<wchar_t> traits;
		basic_string<wchar_t> str;
		for(basic_string<wchar_t>::size_type pos=0;pos<m_exp.size();++pos){
			if(traits::syntax_type(m_exp[pos])==traits::syntax_slash){
				++pos;
				if(m_exp.size()<=pos)break;
				if(traits::is_class(m_exp[pos], traits::char_class_digit)){
					size_t n=0;
					do{
						n=n*10+traits::toi(m_exp[pos]);
						++pos;
					}while(pos<m_exp.size() && traits::is_class(m_exp[pos], traits::char_class_digit));
					--pos;
					if(n<results->size()){
						str.append(results->str((int)n));
					}
				} else switch(traits::syntax_type(m_exp[pos])) {
					case traits::syntax_slash:	str.append(1, '\\');	break;
					case traits::syntax_n:	str.append(1, '\n');	break;
					case traits::syntax_t:	str.append(1, '\t');	break;
					case traits::syntax_r:	str.append(1, '\r');	break;
					case traits::syntax_f:	str.append(1, '\f');	break;
					case traits::syntax_a:	str.append(1, '\a');	break;
					case traits::syntax_b:	str.append(1, '\b');	break;
					case traits::syntax_e:	str.append(1, '\x1b');	break;
					default: str.append(1, m_exp[pos]);	break;
				}
			} else {
				str.append(1, m_exp[pos]);
			}
		}
		return str;
	}
protected:
	basic_string<wchar_t> m_exp;
};

class JReplacer : public IRegexReplaceCallback<char>{
public:
	JReplacer(IJcodeConverter & conv, const basic_string<char> & exp) : m_conv(conv), m_exp(conv.ConvertToWString(exp)) {}
	virtual ~JReplacer(){}
	virtual basic_string<char> ReplaceCallback(const IRegexResults<char> * results){
		typedef boost::regex_traits<wchar_t> traits;
		basic_string<char> str;
		for(basic_string<wchar_t>::size_type pos=0;pos<m_exp.size();++pos){
			if(traits::syntax_type(m_exp[pos])==traits::syntax_slash){
				++pos;
				if(m_exp.size()<=pos)break;
				if(traits::is_class(m_exp[pos], traits::char_class_digit)){
					size_t n=0;
					do{
						n=n*10+traits::toi(m_exp[pos]);
						++pos;
					}while(pos<m_exp.size() && traits::is_class(m_exp[pos], traits::char_class_digit));
					--pos;
					if(n<results->size()){
						str.append(results->str((int)n));
					}
				} else switch(traits::syntax_type(m_exp[pos])) {
					case traits::syntax_slash:	str.append(1, '\\');	break;
					case traits::syntax_n:	str.append(1, '\n');	break;
					case traits::syntax_t:	str.append(1, '\t');	break;
					case traits::syntax_r:	str.append(1, '\r');	break;
					case traits::syntax_f:	str.append(1, '\f');	break;
					case traits::syntax_a:	str.append(1, '\a');	break;
					case traits::syntax_b:	str.append(1, '\b');	break;
					case traits::syntax_e:	str.append(1, '\x1b');	break;
					default: str.append(m_conv.ConvertToString(basic_string<wchar_t>(1, m_exp[pos])));	break;
				}
			} else {
				str.append(m_conv.ConvertToString(basic_string<wchar_t>(1, m_exp[pos])));
			}
		}
		return str;
	}
protected:
	JReplacer & operator = (const JReplacer & s){}//代入演算子を生成できません・・・とかいう警告を出さなくするため。

	IJcodeConverter & m_conv;
	basic_string<wchar_t> m_exp;
};

static bool CheckOption(const char * opt, char key){
	if(opt==NULL)return false;
	char c;
	while(c=*(opt++)){
		if(c==key)return true;
	}
	return false;
}

} // end of namespace boost_regex_local

using namespace boost_regex_local;

//+[CRegexCache]
template <class E>
class CRegexCache{
public:
	typedef std::pair<int, basic_string<E> > pair_int_string;
	typedef smart_ptr<boost::reg_expression<E> > reg_exp_ptr;
	typedef std::map<pair_int_string, reg_exp_ptr> cache_map_type;
public:
	static void Release(){
		if(!m_static.isNull()){
			m_static->_Release();
			m_static=smart_ptr<CRegexCache>();
		}
	}
	void _Release(){
		m_cache.clear();
	}
	static const boost::reg_expression<E> & GetRegex(const basic_string<E> & pat, const char * opt){
		if(m_static.isNull()){
			m_static=smart_ptr<CRegexCache>(new CRegexCache());
		}
		return m_static->_GetRegex(pat, opt);
	}
	const boost::reg_expression<E> & _GetRegex(const basic_string<E> & pat, const char * opt){
		int flag=boost::regbase::normal;
		if(CheckOption(opt, 'i')){
			flag|=boost::regbase::icase;
		}
		pair<int, basic_string<E> > pp(flag, pat);
		CCriticalLock lock(&m_cs);
		std::map<std::pair<int, basic_string<E> >, smart_ptr<boost::reg_expression<E> > >::iterator i=m_cache.find(pp);
		if(i!=m_cache.end()){
			return *((*i).second);
		}
		std::pair<
			cache_map_type::iterator,
			bool
		> p = m_cache.insert(
			std::make_pair(
				pp,
				smart_ptr<boost::reg_expression<E> >(
					new boost::reg_expression<E>(
						pat.c_str(), pat.c_str()+pat.size(),
						flag
					)
				)
			)
		);
		return *((*(p.first)).second);
	}
protected:
	CCriticalSection m_cs;
	cache_map_type m_cache;
	static smart_ptr<CRegexCache> m_static;
};
//-[CRegexCache]
smart_ptr<CRegexCache<char> > CRegexCache<char>::m_static;
smart_ptr<CRegexCache<wchar_t> > CRegexCache<wchar_t>::m_static;

//+[CBoostRegex]
	//+[コンストラクタ・デストラクタ]
	CBoostRegex::CBoostRegex(){}
	CBoostRegex::~CBoostRegex(){}
	//-[コンストラクタ・デストラクタ]
	//+[GetResults]
	const IRegexResults<char> * CBoostRegex::GetResults() const
	{
		return & m_results;
	}
	const IRegexResults<wchar_t> * CBoostRegex::GetWResults() const
	{
		return & m_wresults;
	}
	const IRegexResults<char> * CBoostRegex::GetJResults() const
	{
		return GetResults();
	}
	//-[GetResults]
	//+[Next]
	bool CBoostRegex::Next()
	{
/*
　使い方：
	if(regex->Search("aaabb", "a*b", "g")){
		do{
			//...
		}while(regex->Next());
	}
*/
		if(m_results_list.empty())return false;
		m_results=m_results_list.front();
		m_results_list.pop_front();
		return true;
	}
	bool CBoostRegex::WNext()
	{
		if(m_wresults_list.empty())return false;
		m_wresults=m_wresults_list.front();
		m_wresults_list.pop_front();
		return true;
	}
	bool CBoostRegex::JNext()
	{
		return Next();
	}
	//-[Next]
	//+[PushResults]
	void CBoostRegex::push_results(const boost::match_results<basic_string<char>::const_iterator>& res, size_t pos)
	{
		m_results_list.push_back(CRegexResults<char>());
		m_results_list.back().set_results(res, pos);
	}
	void CBoostRegex::push_wresults(const boost::match_results<basic_string<wchar_t>::const_iterator>& res, size_t pos)
	{
		m_wresults_list.push_back(CRegexResults<wchar_t>());
		m_wresults_list.back().set_results(res, pos);
	}
	void CBoostRegex::push_jresults(IJcodeConverter & conv, const boost::match_results<basic_string<wchar_t>::const_iterator> & res, size_t pos)
	{
		m_results_list.push_back(CRegexResults<char>());

		int size=(int)res.size();
		m_results_list.back().set_size(size);
		for(int i=0;i<size;i++){
			m_results_list.back().set_str(i, conv.ConvertToString(res.str(i)));
			m_results_list.back().set_position(i, conv.GetPos(res.position(i))+pos);
		}
	}
	//-[PushResults]
	//+[SearchIt]
	bool CBoostRegex::SearchIt(const basic_string<char>::const_iterator & begin, const basic_string<char>::const_iterator & end, size_t pos, const basic_string<char> & pat, const char * opt)
	{
		m_results_list.clear();
		if (CheckOption(opt, 'g')) {
			//複数検索
			boost::regex_grep(Searcher(*this, pos), begin, end, CRegexCache<char>::GetRegex(pat, opt));
			return Next();
		} else {
			//一般的な検索っす！
			boost::match_results<basic_string<char>::const_iterator> results;
			bool ret=boost::regex_search(begin, end, results, CRegexCache<char>::GetRegex(pat, opt));
			if(ret){
				m_results.set_results(results, pos);
			}
			return ret;
		}
	}
	bool CBoostRegex::WSearchIt(const basic_string<wchar_t>::const_iterator & begin, const basic_string<wchar_t>::const_iterator & end, size_t pos, const basic_string<wchar_t> & pat, const char * opt)
	{
		m_wresults_list.clear();
		if (CheckOption(opt, 'g')) {
			//複数検索
			boost::regex_grep(WSearcher(*this, pos), begin, end, CRegexCache<wchar_t>::GetRegex(pat, opt));
			return WNext();
		} else {
			//一般的な検索っす！
			boost::match_results<basic_string<wchar_t>::const_iterator> results;
			bool ret=boost::regex_search(begin, end, results, CRegexCache<wchar_t>::GetRegex(pat, opt));
			if(ret){
				m_wresults.set_results(results, pos);
			}
			return ret;
		}
	}
	//-[SearchIt]
	//+[Search]
	//++[Search - String]
	bool CBoostRegex::Search(const basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, const char * opt)
	{
		size_t len=str.size();
		if(len<pos2)pos2=len;
		if(pos2<=pos1)return false;
		return SearchIt(str.begin()+pos1, str.begin()+pos2, pos1, pat, opt);
	}
	bool CBoostRegex::Search(const basic_string<char> & str, const basic_string<char> & pat, const char * opt)
	{
		return SearchIt(str.begin(), str.end(), 0, pat, opt);
	}
	//--[Search - String]

	//++[Search - WString]
	bool CBoostRegex::WSearch(const basic_string<wchar_t> & str, size_t pos1, size_t pos2, const basic_string<wchar_t> & pat, const char * opt)
	{
		size_t len=str.size();
		if(len<pos2)pos2=len;
		if(pos2<=pos1)return false;
		return WSearchIt(str.begin()+pos1, str.begin()+pos2, pos1, pat, opt);
	}
	bool CBoostRegex::WSearch(const basic_string<wchar_t> & str, const basic_string<wchar_t> & pat, const char * opt)
	{
		return WSearchIt(str.begin(), str.end(), 0, pat, opt);
	}
	//--[Search - WString]

	//++[Search - JString]
	bool CBoostRegex::JSearch(IJcodeConverter & conv, const basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, const char * opt)
	{
		size_t len=str.size();
		if(len<pos2)pos2=len;
		if(pos2<=pos1)return false;

		basic_string<wchar_t> wstr=conv.ConvertToWString(str, true);
		size_t wpos1=conv.GetWPos(pos1), wpos2=conv.GetWPos(pos2);

		m_results_list.clear();
		if (CheckOption(opt, 'g')) {
			//複数検索
			boost::regex_grep(JSearcher(conv, *this, pos1), basic_string<wchar_t>::const_iterator(wstr.begin()+wpos1), basic_string<wchar_t>::const_iterator(wstr.begin()+wpos2), CRegexCache<wchar_t>::GetRegex(conv.ConvertToWString(pat), opt));
			return JNext();
		} else {
			//一般的な検索っす！
			boost::match_results<basic_string<wchar_t>::const_iterator> results;
			bool ret=boost::regex_search(basic_string<wchar_t>::const_iterator(wstr.begin()+wpos1), basic_string<wchar_t>::const_iterator(wstr.begin()+wpos2), results, CRegexCache<wchar_t>::GetRegex(conv.ConvertToWString(pat), opt));
			if(ret){
				int size=(int)results.size();
				m_results.set_size(size);
				for(int i=0;i<size;i++){
					m_results.set_str(i, conv.ConvertToString(results.str(i)));
					m_results.set_position(i, conv.GetPos(results.position(i))+pos1);
				}
			}
			return ret;
		}
	}
	bool CBoostRegex::JSearch(IJcodeConverter & conv, const basic_string<char> & str, const basic_string<char> & pat, const char * opt){
		return JSearch(conv, str, 0, str.size(), pat, opt);
	}
	//--[Search - JString]
	//-[Search]

	//+[Replace]
	//++[Replace - String]
	size_t CBoostRegex::Replace(basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, IRegexReplaceCallback<char> & clbk, const char * opt)
	{
		int count=0;
		const basic_string<char> dmy_str=str;
		str.erase();
		size_t pos_=0;
		if(Search(dmy_str, pos1, pos2, pat, opt)){
			do{
				++count;
				size_t pos3=GetResults()->position();
				str.append(dmy_str, pos_, pos3-pos_);
				str.append(clbk.ReplaceCallback(GetResults()));
				pos_=pos3+GetResults()->length();
			}while(Next());
		}
		str.append(dmy_str, pos_, dmy_str.size()-pos_);
		return count;
	}
	size_t CBoostRegex::Replace(basic_string<char> & str, const basic_string<char> & pat, IRegexReplaceCallback<char> & clbk, const char * opt)
	{
		return Replace(str, 0, str.size(), pat, clbk, opt);
	}
	size_t CBoostRegex::Replace(basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, const basic_string<char> & exp, const char * opt)
	{
		Replacer rep(exp);
		return Replace(str, pos1, pos2, pat, rep, opt);
	}
	size_t CBoostRegex::Replace(basic_string<char> & str, const basic_string<char> & pat, const basic_string<char> & exp, const char * opt)
	{
		return Replace(str, 0, str.size(), pat, exp, opt);
	}
	//--[Replace - String]

	//++[Replace - WString]
	size_t CBoostRegex::WReplace(basic_string<wchar_t> & str, size_t pos1, size_t pos2, const basic_string<wchar_t> & pat, IRegexReplaceCallback<wchar_t> & clbk, const char * opt)
	{
		int count=0;
		const basic_string<wchar_t> dmy_str=str;
		str.erase();
		size_t pos_=0;
		if(WSearch(dmy_str, pos1, pos2, pat, opt)){
			do{
				++count;
				size_t pos3=GetWResults()->position();
				str.append(dmy_str, pos_, pos3-pos_);
				str.append(clbk.ReplaceCallback(GetWResults()));
				pos_=pos3+GetWResults()->length();
			}while(WNext());
		}
		str.append(dmy_str, pos_, dmy_str.size()-pos_);
		return count;
	}
	size_t CBoostRegex::WReplace(basic_string<wchar_t> & str, const basic_string<wchar_t> & pat, IRegexReplaceCallback<wchar_t> & clbk, const char * opt)
	{
		return WReplace(str, 0, str.size(), pat, clbk, opt);
	}
	size_t CBoostRegex::WReplace(basic_string<wchar_t> & str, size_t pos1, size_t pos2, const basic_string<wchar_t> & pat, const basic_string<wchar_t> & exp, const char * opt)
	{
		WReplacer rep(exp);
		return WReplace(str, pos1, pos2, pat, rep, opt);
	}
	size_t CBoostRegex::WReplace(basic_string<wchar_t> & str, const basic_string<wchar_t> & pat, const basic_string<wchar_t> & exp, const char * opt)
	{
		return WReplace(str, 0, str.size(), pat, exp, opt);
	}
	//--[Replace - WString]

	//++[Replace - JString]
	size_t CBoostRegex::JReplace(IJcodeConverter & conv, basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, IRegexReplaceCallback<char> & clbk, const char * opt)
	{
		int count=0;
		basic_string<wchar_t> wstr;
		size_t pos_=0;
		if(JSearch(conv, str, pos1, pos2, pat, opt)){
			do{
				++count;
				size_t pos3=GetJResults()->position();
				wstr.append(conv.GetWString(), pos_, conv.GetWPos(pos3)-pos_);
				wstr.append(conv.ConvertToWString(clbk.ReplaceCallback(GetJResults())));
				pos_=conv.GetWPos(pos3+GetJResults()->length());
			}while(JNext());
		}
		wstr.append(conv.GetWString(), pos_, conv.GetWPos(str.size())-pos_);
		str=conv.ConvertToString(wstr);
		return count;
	}
	size_t CBoostRegex::JReplace(IJcodeConverter & conv, basic_string<char> & str, const basic_string<char> & pat, IRegexReplaceCallback<char> & clbk, const char * opt)
	{
		return JReplace(conv, str, 0, str.size(), pat, clbk, opt);
	}
	size_t CBoostRegex::JReplace(IJcodeConverter & conv, basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, const basic_string<char> & exp, const char * opt)
	{
		JReplacer rep(conv, exp);
		return JReplace(conv, str, pos1, pos2, pat, rep, opt);
	}
	size_t CBoostRegex::JReplace(IJcodeConverter & conv, basic_string<char> & str, const basic_string<char> & pat, const basic_string<char> & exp, const char * opt)
	{
		return JReplace(conv, str, 0, str.size(), pat, exp, opt);
	}
	//--[Replace - JString]

	//-[Replace]
//-[CBoostRegex]

} // end of namespace yaneuraoGameSDK3rd
} // end of namespace Auxiliary

#ifdef COMPILE_YANE_PLUGIN_DLL
//DLLを作るときのみ。
class CRegexFactory : public factory<CBoostRegex>{
public:
	CRegexFactory(){}
	virtual ~CRegexFactory(){
		CRegexCache<char>::Release();
		CRegexCache<wchar_t>::Release();
	}
};

void	YaneRegistPlugIn(IObjectCreater*p){
	p->RegistClass("boost::regex",new CRegexFactory);
}
#endif
