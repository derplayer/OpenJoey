//  IRegex.h
//		boost::regex++�Ȃǂ̐��K�\�����C�u������DLL����Ăяo�����߂̃C���^�t�F�[�X��
//			programmed by coLun	'03/04/30 - '03/05/19

#ifndef __yanePlugin_IRegex_h__
#define __yanePlugin_IRegex_h__

namespace yaneuraoGameSDK3rd{
namespace Dll{

struct IJcodeConverter{
/**
�@�g�����F
	class CFJisConverter : IJcodeConverter {...};///����Jis�R���o�[�^�i������False�j
	class CFSjisConverter : IJcodeConverter {...};///����Sjis�R���o�[�^�i������False�j
	class CFEucConverter : IJcodeConverter {...};///����Euc�R���o�[�^�i������False�j

	void main()
	{
		IRegex *regex=new CRegex();
		regex->JSearch(CFSjisConverter(), "Say ���������� to BlackJack", "[��-��]*");
		cout << regex->GetJResults()->str() << endl;
		///�������������ƕ\�������B
		delete regex;
	}

�@JSearch�ȂǂɎg��ꂽ���Ƃ̃R���o�[�^�iIJcodeConverter�h���̃I�u�W�F�N�g�j�̒��g�͕s��ł��B�i�ė��p�͉\�ł��j
�@�Ȃ��A�X�̃I�u�W�F�N�g�̓X���b�h�Z�[�t�ł͂���܂���B�i�N���X�̓X���b�h�Z�[�t�Ȃ̂ŁA�X���b�h�ԂŕʁX�̃I�u�W�F�N�g�𐶐����Ďg����OK�ł��j
*/

	virtual ~IJcodeConverter(){}
	virtual basic_string<char> ConvertToString(const basic_string<wchar_t> & wstr, bool flag=false) = 0;
	virtual basic_string<wchar_t> ConvertToWString(const basic_string<char> & str, bool flag=false) = 0;
	virtual basic_string<char> GetString() = 0;
	virtual basic_string<wchar_t> GetWString() = 0;
	virtual size_t GetPos(size_t wpos) = 0;
	virtual size_t GetWPos(size_t pos) = 0;
};
template<class E>
class IRegexResults{
/**
�@�g�����F

	IRegex *regex=new CRegex();
	regex->Search("Say Hello to BlackJack", "\\w*");
	cout << regex->GetResults()->str() << endl;
	///��Hello�ƕ\�������B
	delete regex;
*/

public:
	virtual ~IRegexResults(){}
	virtual basic_string<E> str(size_t n=0) const =0;	//�q�b�g�����������Ԃ��Bn==0�̂Ƃ��S�́A1<=n�̂Ƃ��An�Ԗڂ̊��ʂ̒��g�ɂȂ�B
	virtual size_t position(size_t n=0) const =0;	//�����񂪃q�b�g�����ʒu��Ԃ��B
	virtual size_t length(size_t n=0) const =0;	//�q�b�g����������̒�����Ԃ��Bstr(n).length()�Ɠ����B
	virtual size_t size() const =0;	//n�̍ő吔+1
};

template<class E>
struct IRegexReplaceCallback{
/**
�@�g�����F
	class CHoge : public IRegexReplaceCallback<char> {
		virtual ~CHoge(){}
		virtual basic_string<E> ReplaceCallback(const IRegexResults<E> * results){
			return "["+results->str()+"]";
		}
	};

	IRegex *regex=new CRegex();
	string s="Say Hello to BlackJack";
	regex->Replace(s, "[A-Za-z]+", CHoge(), "g");///�������ňꎞ�I�u�W�F�N�g��n���Ă���B
	cout << s << endl;
	///��"[Say] [Hello] [to] [BlackJack]"�ƕ\�������B
	delete regex;

�@��̗�ł͈ꎞ�I�u�W�F�N�g��n���Ă��邪�A�ꎞ�I�u�W�F�N�g����Ȃ��Ă������̂ŁA
�@���炩���ߒl��^���Ă������I�u�W�F�N�g��Replace�Ɏg������A
�@���邢��Replace�ł̌Ăяo�����������̉�͏����Ɏg�����Ƃ��\�B
*/
	virtual ~IRegexReplaceCallback(){}
	virtual basic_string<E> ReplaceCallback(const IRegexResults<E> * results) = 0;
};
template<class E, class F>
class CRegexReplaceFunctionalCaller : IRegexReplaceCallback<E>{
/**
�@�g�����F
	class CHoge {
		virtual ~CHoge(){}
		basic_string<E> operator () (const IRegexResults<E> * results){
			return "["+results->str()+"]";
		}
	};

	IRegex *regex=new CRegex();
	string s="Say Hello to BlackJack";
	CHoge hoge;
	regex->Replace(s, "[A-Za-z]+", CRegexReplaceFunctionalCaller<char, CHoge>(hoge), "g");///��������template�𗘗p���Ĉꎞ�I�u�W�F�N�g�����A�����n���Ă���B
	cout << s << endl;
	///��"[Say] [Hello] [to] [BlackJack]"�ƕ\�������B
	delete regex;

�@functional�Ȃ��̂��I�u���[�g���Ă����template�N���X�ł��B
�@�⏕�N���X�Ȃ̂ŁA�g��������������g���Ă݂Ă��������B
*/
public:
	CRegexReplaceFunctionalCaller(F & func) : m_func(func) {}
	virtual ~CRegexReplaceFunctionalCaller(){}
	virtual basic_string<E> ReplaceCallback(const IRegexResults<E> * results)
		{	return m_func(results);	}
protected:
	F & m_func;
};

class IRegex{
/**
�@�g�����@-�@���̂P�i��ʓI�Ȑ��K�\���j�F
	CObjectCreater::GetObj()->LoadPlugIn("boost_regex.dll");	///��DLL�̓ǂݍ���
	IRegex *regex = (IRegex*)CObjectCreater::GetObj()->Create("boost::regex");	///��DLL���ŃI�u�W�F�N�g�𐶐�

	if(regex->Search("Say Hello to BlackJack", " [A-Za-z]+")){	///��DLL���̃��\�b�h�����R�ɌĂяo����
		cout << regex->GetResults()->str() << endl;	///��" Hello"�ƕ\�������B
	}

	delete regex;	///��DLL���Ő������ꂽ�I�u�W�F�N�g���A�����瑤��delete�ł���
	CObjectCreater::GetObj()->ReleasePlugIn("boost_regex.dll");	///���Ō��DLL��������Ă�����
*/
/**
�@�g�����@-�@���̂Q�i��ʓI�Ȑ��K�\���̂��̑��̋@�\�j�F
	CObjectCreater::GetObj()->LoadPlugIn("boost_regex.dll");	///��DLL�̓ǂݍ���
	IRegex *regex = (IRegex*)CObjectCreater::GetObj()->Create("boost::regex");	///��DLL���ŃI�u�W�F�N�g�𐶐�

	if(regex->Search("Say Hello to BlackJack", "[A-Z]+", "ig")){	///���I�v�V�����t���B[i���啶����������ʂȂ�][g����Ԃ��d�������Ƀq�b�g������̂����ׂ�]
		do{
			cout << regex->GetResults()->str() << endl;	///��1��ځF"Say"�@2��ځF"Hello"�@3��ځF"to"�@4��ځF"BlackJack"�@�ƕ\�������B
		}while(regex->Next());
	}

	string s="Say Hello to BlackJack";
	regex->Replace(s, "[A-Za-z]+", "\\0/", "g");
	cout << s << endl;	///��"Say/ Hello/ to/ BlackJack/"�ƕ\�������B

	delete regex;	///��DLL���Ő������ꂽ�I�u�W�F�N�g���A�����瑤��delete�ł���
	CObjectCreater::GetObj()->ReleasePlugIn("boost_regex.dll");	///���Ō��DLL��������Ă�����
*/
/**
�@�g�����@-�@���̂R�iSJIS�Ή��Ȏg�����j�iCFSjisConverter.h��ǂݍ��ނ��Ɓj�F
	CObjectCreater::GetObj()->LoadPlugIn("boost_regex.dll");	///��DLL�̓ǂݍ���
	IRegex *regex = (IRegex*)CObjectCreater::GetObj()->Create("boost::regex");	///��DLL���ŃI�u�W�F�N�g�𐶐�

	if(regex->JSearch(CFSjisConverter(), "�ray HELLO ���� Black��������", "[A-Za-z�`-�y��-��]+")){	///��DLL���̃��\�b�h�����R�ɌĂяo����	///��Search��JSearch�ɂȂ��Ă��邱�Ƃɒ���
		cout << regex->GetJResults()->str() << endl;	///��"�ray"�ƕ\�������	///��GetResults��GetJResults�ɂȂ��Ă��邱�Ƃɒ���
	}

	delete regex;	///��DLL���Ő������ꂽ�I�u�W�F�N�g���A�����瑤��delete�ł���
	CObjectCreater::GetObj()->ReleasePlugIn("boost_regex.dll");	///���Ō��DLL��������Ă�����
*/
public:
	virtual ~IRegex(){};
	//GetResults
	virtual const IRegexResults<char> * GetResults() const = 0;
	virtual const IRegexResults<wchar_t> * GetWResults() const = 0;
	virtual const IRegexResults<char> * GetJResults() const = 0;
	//Search
	/**
	�@�g�����F
		IRegex *regex = ...;
		if(regex->Search("Say Hello to BlackJack", "([A-Z][a-z]+) ([a-z]+) ([A-Z][A-Za-z]+)")){
			cout << regex->GetResults()->str() << endl;	///��"Hello to BlackJack"�@�ƕ\�������B	/// regex->GetResults()->str(0)�@�ł������B0�͏ȗ��ł���B
			cout << regex->GetResults()->str(1) << endl;	///��"Hello"�@�ƕ\�������B
			cout << regex->GetResults()->str(2) << endl;	///��"to"�@�ƕ\�������B
			cout << regex->GetResults()->str(3) << endl;	///��"BlackJack"�@�ƕ\�������B
		}
	*/
	/**
	�@�����F
		str 	��	���������镶����
		pos1	��	�J�n�ꏊ�i�O��������̍ŏ��j
		pos2	��	�I���ꏊ�istr.size()��������̍Ō�j
		pat 	��	�������鐳�K�\���p�^�[��
		opt 	��	�I�v�V����[i���啶����������ʂȂ�][g����Ԃ��d�������Ƀq�b�g������̂����ׂ�]�i������JSearch�̂Ƃ��A�S�p�A���t�@�x�b�g�ւ�i�͖����j�ig�I�v�V�����������ꍇ�́ANext()�ŏ��񂳂���j
		conv	��	���{�ꌟ���̂Ƃ��̕ϊ��N���X�BCFSjisConverter()�Ȃǂ��w�肵�Ă�����B
	�@�߂�l�F
		�����ɐ��������ꍇ��true���B���s�����ꍇ��false���߂�B
	*/
	virtual bool Search(const basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, const char * opt="") = 0;
	virtual bool Search(const basic_string<char> & str, const basic_string<char> & pat, const char * opt="") = 0;
	virtual bool WSearch(const basic_string<wchar_t> & str, size_t pos1, size_t pos2, const basic_string<wchar_t> & pat, const char * opt="") = 0;
	virtual bool WSearch(const basic_string<wchar_t> & str, const basic_string<wchar_t> & pat, const char * opt="") = 0;
	virtual bool JSearch(IJcodeConverter & conv, const basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, const char * opt="") = 0;
	virtual bool JSearch(IJcodeConverter & conv, const basic_string<char> & str, const basic_string<char> & pat, const char * opt="") = 0;
	//Next
	/**
	�@�g�����F
		IRegex *regex = ...;
		if(regex->Search("Say Hello to BlackJack", "[A-Za-z]+", "g")){	///��"g"�I�v�V����������
			do{
				cout << regex->GetResults()->str() << endl;	///��1��ځF"Say"�@2��ځF"Hello"�@3��ځF"to"�@4��ځF"BlackJack"�@�ƕ\�������B
			}while(regex->Next());	///��Next()��GetResults()�̒��g���܂킷
		}
	�@�߂�l�F
		���̌������ʂ�����ꍇ��true���B�����ꍇ��false���߂�B
	*/
	virtual bool Next() = 0;
	virtual bool WNext() = 0;
	virtual bool JNext() = 0;
	//Replace
	/**
	�@�g�����F
		IRegex *regex = ...;
		string s="Say Hello to BlackJack";
		regex->Replace(s, "[A-Za-z]+", "\\0/", "g");	//��"g"�I�v�V�����������ꍇ�́A�S�����ϊ������B���ĂȂ��ꍇ�͍ŏ��̂P�̂�
		cout << s << endl;	///��"Say/ Hello/ to/ BlackJack/"�ƕ\�������B
	*/
	/**
	�@�����F
		str 	��	���������镶����
		pos1	��	�J�n�ꏊ�i�O��������̍ŏ��j
		pos2	��	�I���ꏊ�istr.size()��������̍Ō�j
		pat 	��	�������鐳�K�\���p�^�[��
		opt 	��	�I�v�V����[i���啶����������ʂȂ�][g����Ԃ��d�������Ƀq�b�g������̂����ׂ�]�i������JSearch�̂Ƃ��A�S�p�A���t�@�x�b�g�ւ�i�͖����j�ig�I�v�V�����������ꍇ�́ANext()�ŏ��񂳂���j
		conv	��	���{�ꌟ���̂Ƃ��̕ϊ��N���X�BCFSjisConverter()�Ȃǂ��w�肵�Ă�����B
		clbk	��	IRegexReplaceCallback���p�����Ă���R�[���o�b�N�N���X�B�ڂ�����IRegexReplaceCallback�̐�����ǂނ��ƁB
		exp 	��	�u�����p�^�[���B�G�X�P�[�v�����i\�j�̂��Ƃɐ��������邱�ƂŁA�����Ńq�b�g����������̎Q�Ƃ��ł���B\0���S�́B\1��1�߂̊��ʁB�ȉ����B�i����""�ň͂��Ďw�肷��ꍇ�́A����̃G�X�P�[�v�������Ă��܂��̂�"\\0"�̂悤�Ɏw�肷��j
	�@�߂�l�F
		�����ɐ�����������Ԃ��B
	*/
	virtual size_t Replace(basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, IRegexReplaceCallback<char> & clbk, const char * opt="") = 0;
	virtual size_t Replace(basic_string<char> & str, const basic_string<char> & pat, IRegexReplaceCallback<char> & clbk, const char * opt="") = 0;
	virtual size_t Replace(basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, const basic_string<char> & exp, const char * opt="") = 0;
	virtual size_t Replace(basic_string<char> & str, const basic_string<char> & pat, const basic_string<char> & exp, const char * opt="") = 0;

	virtual size_t WReplace(basic_string<wchar_t> & str, size_t pos1, size_t pos2, const basic_string<wchar_t> & pat, IRegexReplaceCallback<wchar_t> & clbk, const char * opt="") = 0;
	virtual size_t WReplace(basic_string<wchar_t> & str, const basic_string<wchar_t> & pat, IRegexReplaceCallback<wchar_t> & clbk, const char * opt="") = 0;
	virtual size_t WReplace(basic_string<wchar_t> & str, size_t pos1, size_t pos2, const basic_string<wchar_t> & pat, const basic_string<wchar_t> & exp, const char * opt="") = 0;
	virtual size_t WReplace(basic_string<wchar_t> & str, const basic_string<wchar_t> & pat, const basic_string<wchar_t> & exp, const char * opt="") = 0;

	virtual size_t JReplace(IJcodeConverter & conv, basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, IRegexReplaceCallback<char> & clbk, const char * opt="") = 0;
	virtual size_t JReplace(IJcodeConverter & conv, basic_string<char> & str, const basic_string<char> & pat, IRegexReplaceCallback<char> & clbk, const char * opt="") = 0;
	virtual size_t JReplace(IJcodeConverter & conv, basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, const basic_string<char> & exp, const char * opt="") = 0;
	virtual size_t JReplace(IJcodeConverter & conv, basic_string<char> & str, const basic_string<char> & pat, const basic_string<char> & exp, const char * opt="") = 0;
};

}// end of namespace Dll
}// end of namespace yaneuraoGameSDK3rd

#endif
