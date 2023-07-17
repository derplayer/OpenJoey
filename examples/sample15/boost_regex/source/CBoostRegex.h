#ifndef __yanePlugin_boost_regex_h__
#define __yanePlugin_boost_regex_h__

#include "IRegex.h"

/**

�@���̃v���O�����́A�{��ObjectCreater��ʂ���DLL����Ăяo���Ďg���܂��B
�@����ǂ�DLL��ʂ����Ɏg�������ꍇ�́A���̃w�b�_���C���N�������g���Ďg���Ă��������B�܂����̎��A���̂��ƂɋC�����Ă��������B
�@�@�E�@CBoostRegex.cpp�ƁAboost��regex�t�H���_��src.cpp�̂Q���v���W�F�N�g�ɉ�����K�v������܂��B
�@�@�E�@src.cpp�́A�v���R���p�C���ς݃w�b�_���g�p���Ȃ��ݒ�ŃR���p�C�����Ă��������B

�@DLL��ʂ����Ɏg���ꍇ�ACBoostRegex��boost::regex���g���������N���X�ɂȂ�܂��B

DLL��ʂ����Ɏg���g�����F
�@���̂P�F//DLL��ʂ��ꍇ�ƌ݊�
	IRegex * regex=new CBoostRegex();
	regex->Search(...);
	delete regex;
�@���̂Q�F//�������߁`��i�������DLL��ʂ��ꍇ�ƌ݊��j
	smart_ptr<IRegex> regex=new CBoostRegex();
	regex->Search(...);
�@���̂R�F//�ȒP�Ɏg���ꍇ
	CBoostRegex regex;
	regex.Search(...);
�@���̂S�F//�i��W���jIRegex�^�ŊȒP�Ɏg���ꍇ�B��regex�̃X�R�[�v���I���܂ŁA�ꎞ�I�u�W�F�N�g�͐�����iconst�Q�ƈȊO�ւ̑���͔�W���炵���j
	IRegex & regex=CBoostRegex();
	regex.Search(...);
*/

namespace yaneuraoGameSDK3rd{
namespace Dll{

//+[CRegexResults]
template <class E>
class CRegexResults : public IRegexResults<E>{
/**
�@�g�����F
	CBoostRegex��GetResults���\�b�h�̖߂�l�Ƃ��Ďg���B
	IRegexResults*�Ƃ��Ĉ������ƂɂȂ�̂ŁA�ڂ�����IRegexResults�̐������Q�ƁB
*/
public:
	void set_results(const boost::match_results<basic_string<E>::const_iterator> & results, size_t pos){
		size_t size=results.size();
		set_size(size);
		for(size_t i=0;i<size;i++){
			set_str(i, results.str((int)i));//�Ȃ�ł�������(int)��
			set_position(i, results.position((unsigned int)i)+pos);//��������(unsigned int)�ɂȂ邩�͓�E�E�Eboost�ɕ����Ă��������G�G
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

	//+[virtual���\�b�h]
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
	//-[virtual���\�b�h]
protected:
	size_t m_size;
	vector<basic_string<E> > m_str;
	vector<size_t> m_position;
};
//-[CRegexResults]

class CBoostRegex : public IRegex{
/**
�@�g�����F
	IRegex�Ƃ��Ĉ����Ă��������B
	�ڂ���������IRegex�Q�ƁB
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
	//+[�R���X�g���N�^�E�f�X�g���N�^]
	CBoostRegex();
	virtual ~CBoostRegex();
	//-[�R���X�g���N�^�E�f�X�g���N�^]
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

	//+[���̑�]
	void push_results(const boost::match_results<basic_string<char>::const_iterator> & res, size_t pos);
	void push_wresults(const boost::match_results<basic_string<wchar_t>::const_iterator> & res, size_t pos);
	void push_jresults(IJcodeConverter & conv, const boost::match_results<basic_string<wchar_t>::const_iterator> & res, size_t pos);
	//-[���̑�]
};

} // end of namespace yaneuraoGameSDK3rd
} // end of namespace Auxiliary

#endif
