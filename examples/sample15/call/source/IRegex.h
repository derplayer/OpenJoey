//  IRegex.h
//		boost::regex++などの正規表現ライブラリをDLLから呼び出すためのインタフェース等
//			programmed by coLun	'03/04/30 - '03/05/19

#ifndef __yanePlugin_IRegex_h__
#define __yanePlugin_IRegex_h__

namespace yaneuraoGameSDK3rd{
namespace Dll{

struct IJcodeConverter{
/**
　使い方：
	class CFJisConverter : IJcodeConverter {...};///えせJisコンバータ（えせ＝False）
	class CFSjisConverter : IJcodeConverter {...};///えせSjisコンバータ（えせ＝False）
	class CFEucConverter : IJcodeConverter {...};///えせEucコンバータ（えせ＝False）

	void main()
	{
		IRegex *regex=new CRegex();
		regex->JSearch(CFSjisConverter(), "Say ｈｅｌｌｏ to BlackJack", "[ａ-ｚ]*");
		cout << regex->GetJResults()->str() << endl;
		///↑ｈｅｌｌｏと表示される。
		delete regex;
	}

　JSearchなどに使われたあとのコンバータ（IJcodeConverter派生のオブジェクト）の中身は不定です。（再利用は可能です）
　なお、個々のオブジェクトはスレッドセーフではありません。（クラスはスレッドセーフなので、スレッド間で別々のオブジェクトを生成して使えばOKです）
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
　使い方：

	IRegex *regex=new CRegex();
	regex->Search("Say Hello to BlackJack", "\\w*");
	cout << regex->GetResults()->str() << endl;
	///↑Helloと表示される。
	delete regex;
*/

public:
	virtual ~IRegexResults(){}
	virtual basic_string<E> str(size_t n=0) const =0;	//ヒットした文字列を返す。n==0のとき全体、1<=nのとき、n番目の括弧の中身になる。
	virtual size_t position(size_t n=0) const =0;	//文字列がヒットした位置を返す。
	virtual size_t length(size_t n=0) const =0;	//ヒットした文字列の長さを返す。str(n).length()と同じ。
	virtual size_t size() const =0;	//nの最大数+1
};

template<class E>
struct IRegexReplaceCallback{
/**
　使い方：
	class CHoge : public IRegexReplaceCallback<char> {
		virtual ~CHoge(){}
		virtual basic_string<E> ReplaceCallback(const IRegexResults<E> * results){
			return "["+results->str()+"]";
		}
	};

	IRegex *regex=new CRegex();
	string s="Say Hello to BlackJack";
	regex->Replace(s, "[A-Za-z]+", CHoge(), "g");///←ここで一時オブジェクトを渡している。
	cout << s << endl;
	///↑"[Say] [Hello] [to] [BlackJack]"と表示される。
	delete regex;

　上の例では一時オブジェクトを渡しているが、一時オブジェクトじゃなくてもいいので、
　あらかじめ値を与えておいたオブジェクトをReplaceに使ったり、
　あるいはReplaceでの呼び出しをもろもろの解析処理に使うことも可能。
*/
	virtual ~IRegexReplaceCallback(){}
	virtual basic_string<E> ReplaceCallback(const IRegexResults<E> * results) = 0;
};
template<class E, class F>
class CRegexReplaceFunctionalCaller : IRegexReplaceCallback<E>{
/**
　使い方：
	class CHoge {
		virtual ~CHoge(){}
		basic_string<E> operator () (const IRegexResults<E> * results){
			return "["+results->str()+"]";
		}
	};

	IRegex *regex=new CRegex();
	string s="Say Hello to BlackJack";
	CHoge hoge;
	regex->Replace(s, "[A-Za-z]+", CRegexReplaceFunctionalCaller<char, CHoge>(hoge), "g");///←ここでtemplateを利用して一時オブジェクトを作り、それを渡している。
	cout << s << endl;
	///↑"[Say] [Hello] [to] [BlackJack]"と表示される。
	delete regex;

　functionalなものをオブラートしてくれるtemplateクラスです。
　補助クラスなので、使えそうだったら使ってみてください。
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
　使い方　-　その１（一般的な正規表現）：
	CObjectCreater::GetObj()->LoadPlugIn("boost_regex.dll");	///←DLLの読み込み
	IRegex *regex = (IRegex*)CObjectCreater::GetObj()->Create("boost::regex");	///←DLL側でオブジェクトを生成

	if(regex->Search("Say Hello to BlackJack", " [A-Za-z]+")){	///←DLL側のメソッドも自由に呼び出せる
		cout << regex->GetResults()->str() << endl;	///←" Hello"と表示される。
	}

	delete regex;	///←DLL側で生成されたオブジェクトも、こちら側でdeleteできる
	CObjectCreater::GetObj()->ReleasePlugIn("boost_regex.dll");	///←最後にDLLを解放してあげる
*/
/**
　使い方　-　その２（一般的な正規表現のその他の機能）：
	CObjectCreater::GetObj()->LoadPlugIn("boost_regex.dll");	///←DLLの読み込み
	IRegex *regex = (IRegex*)CObjectCreater::GetObj()->Create("boost::regex");	///←DLL側でオブジェクトを生成

	if(regex->Search("Say Hello to BlackJack", "[A-Z]+", "ig")){	///←オプション付き。[i→大文字小文字区別なし][g→区間が重複せずにヒットするものをすべて]
		do{
			cout << regex->GetResults()->str() << endl;	///←1回目："Say"　2回目："Hello"　3回目："to"　4回目："BlackJack"　と表示される。
		}while(regex->Next());
	}

	string s="Say Hello to BlackJack";
	regex->Replace(s, "[A-Za-z]+", "\\0/", "g");
	cout << s << endl;	///←"Say/ Hello/ to/ BlackJack/"と表示される。

	delete regex;	///←DLL側で生成されたオブジェクトも、こちら側でdeleteできる
	CObjectCreater::GetObj()->ReleasePlugIn("boost_regex.dll");	///←最後にDLLを解放してあげる
*/
/**
　使い方　-　その３（SJIS対応な使い方）（CFSjisConverter.hを読み込むこと）：
	CObjectCreater::GetObj()->LoadPlugIn("boost_regex.dll");	///←DLLの読み込み
	IRegex *regex = (IRegex*)CObjectCreater::GetObj()->Create("boost::regex");	///←DLL側でオブジェクトを生成

	if(regex->JSearch(CFSjisConverter(), "Ｓay HELLO ｔｏ Blackｊａｃｋ", "[A-Za-zＡ-Ｚａ-ｚ]+")){	///←DLL側のメソッドも自由に呼び出せる	///←SearchがJSearchになっていることに注意
		cout << regex->GetJResults()->str() << endl;	///←"Ｓay"と表示される	///←GetResultsがGetJResultsになっていることに注意
	}

	delete regex;	///←DLL側で生成されたオブジェクトも、こちら側でdeleteできる
	CObjectCreater::GetObj()->ReleasePlugIn("boost_regex.dll");	///←最後にDLLを解放してあげる
*/
public:
	virtual ~IRegex(){};
	//GetResults
	virtual const IRegexResults<char> * GetResults() const = 0;
	virtual const IRegexResults<wchar_t> * GetWResults() const = 0;
	virtual const IRegexResults<char> * GetJResults() const = 0;
	//Search
	/**
	　使い方：
		IRegex *regex = ...;
		if(regex->Search("Say Hello to BlackJack", "([A-Z][a-z]+) ([a-z]+) ([A-Z][A-Za-z]+)")){
			cout << regex->GetResults()->str() << endl;	///←"Hello to BlackJack"　と表示される。	/// regex->GetResults()->str(0)　でも同じ。0は省略できる。
			cout << regex->GetResults()->str(1) << endl;	///←"Hello"　と表示される。
			cout << regex->GetResults()->str(2) << endl;	///←"to"　と表示される。
			cout << regex->GetResults()->str(3) << endl;	///←"BlackJack"　と表示される。
		}
	*/
	/**
	　引数：
		str 	→	検索させる文字列
		pos1	→	開始場所（０＝文字列の最初）
		pos2	→	終了場所（str.size()＝文字列の最後）
		pat 	→	検索する正規表現パターン
		opt 	→	オプション[i→大文字小文字区別なし][g→区間が重複せずにヒットするものをすべて]（ただしJSearchのとき、全角アルファベットへのiは無効）（gオプションをつけた場合は、Next()で巡回させる）
		conv	→	日本語検索のときの変換クラス。CFSjisConverter()などを指定してあげる。
	　戻り値：
		検索に成功した場合はtrueが。失敗した場合はfalseが戻る。
	*/
	virtual bool Search(const basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, const char * opt="") = 0;
	virtual bool Search(const basic_string<char> & str, const basic_string<char> & pat, const char * opt="") = 0;
	virtual bool WSearch(const basic_string<wchar_t> & str, size_t pos1, size_t pos2, const basic_string<wchar_t> & pat, const char * opt="") = 0;
	virtual bool WSearch(const basic_string<wchar_t> & str, const basic_string<wchar_t> & pat, const char * opt="") = 0;
	virtual bool JSearch(IJcodeConverter & conv, const basic_string<char> & str, size_t pos1, size_t pos2, const basic_string<char> & pat, const char * opt="") = 0;
	virtual bool JSearch(IJcodeConverter & conv, const basic_string<char> & str, const basic_string<char> & pat, const char * opt="") = 0;
	//Next
	/**
	　使い方：
		IRegex *regex = ...;
		if(regex->Search("Say Hello to BlackJack", "[A-Za-z]+", "g")){	///←"g"オプションをつける
			do{
				cout << regex->GetResults()->str() << endl;	///←1回目："Say"　2回目："Hello"　3回目："to"　4回目："BlackJack"　と表示される。
			}while(regex->Next());	///←Next()でGetResults()の中身をまわす
		}
	　戻り値：
		次の検索結果がある場合はtrueが。無い場合はfalseが戻る。
	*/
	virtual bool Next() = 0;
	virtual bool WNext() = 0;
	virtual bool JNext() = 0;
	//Replace
	/**
	　使い方：
		IRegex *regex = ...;
		string s="Say Hello to BlackJack";
		regex->Replace(s, "[A-Za-z]+", "\\0/", "g");	//←"g"オプションをつけた場合は、全部が変換される。つけてない場合は最初の１つのみ
		cout << s << endl;	///←"Say/ Hello/ to/ BlackJack/"と表示される。
	*/
	/**
	　引数：
		str 	→	検索させる文字列
		pos1	→	開始場所（０＝文字列の最初）
		pos2	→	終了場所（str.size()＝文字列の最後）
		pat 	→	検索する正規表現パターン
		opt 	→	オプション[i→大文字小文字区別なし][g→区間が重複せずにヒットするものをすべて]（ただしJSearchのとき、全角アルファベットへのiは無効）（gオプションをつけた場合は、Next()で巡回させる）
		conv	→	日本語検索のときの変換クラス。CFSjisConverter()などを指定してあげる。
		clbk	→	IRegexReplaceCallbackを継承しているコールバッククラス。詳しくはIRegexReplaceCallbackの説明を読むこと。
		exp 	→	置換えパターン。エスケープ文字（\）のあとに数字をつけることで、検索でヒットした文字列の参照ができる。\0→全体。\1→1つめの括弧。以下略。（直接""で囲って指定する場合は、言語のエスケープが働いてしまうので"\\0"のように指定する）
	　戻り値：
		検索に成功した数を返す。
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
