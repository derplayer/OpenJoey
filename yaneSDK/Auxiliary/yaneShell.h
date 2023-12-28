//
//	yaneShell.h :
//
//		シェルコマンド実行関係
//
//

#ifndef __yaneShell_h__
#define __yaneShell_h__


namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class CShell {
public:
	LRESULT	Execute(const string& strExeName,bool bWait=false);
	///　---ファイル実行
	///	外部コマンドを実行します。bWait==trueの場合は、そのコマンドの終了を待ちます。

	LRESULT	OpenFolder(const string& strPathName);
	///	---フォルダをexplorerで開く
	///	指定のフォルダを開きます。

	LRESULT SetWallPaper(const string& strFileName);
	///	指定のファイルを壁紙にします。yanePack/Ex/Dxで圧縮されている
	///	ファイルは不可です。

	LRESULT CopyAndSetWallPaper(const string& strFileName);
	///	指定のファイルをWindowsフォルダにコピーし、それを壁紙にします。
	///	yanePack/yanePackExで圧縮されているファイルでも可能です。

	string getModuleMutexName() const;
	///	起動Exe名から多重起動防止用のMutex名に変換して返す

	string getModuleName() const;
	///	起動モジュール名を返す(full path)
};

}}

#ifndef yaneSDK_GCC_Mode_NOWIN
#include <Shlobj.h>
#endif // yaneSDK_GCC_Mode_NOWIN

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {
class CFolderDialog {
/**
	フォルダ選択用ダイアログ(Windows標準のもの)
		ゲームでは使うことはあまりないだろうけれど..

		↓MSDNに、apartment threadingでCoInitializeしなはれと
		書いてあるんだが、どうしたもんか．．
	Remarks:
	SHBrowseForFolder. You can also use CoInitialize or OLEInitialize,
	which always use apartment threading.
*/
public:
	LRESULT GetOpenFolderName(string& strFolderName, const string& DefaultFolder=""
		,const string& Title = "フォルダを選択してください");
	/**
		↑これ呼ぶだけ。strFolderNameに選択されたフォルダが返ってくる
		DefaultFolderは、ディフォルトフォルダを渡すと良い。省略可能。
		返されるフォルダ名の末尾には　￥　マークはついていない
	*/

protected:
	static int CALLBACK SHBrowseProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
	void _SHFree(ITEMIDLIST* pidl);
};

class CFileDialog {
///	このクラスは作りかけ
public:
	LRESULT GetOpenFile(string& strFileName,const string& DefaultFolder=""
		,const string& Title = "ファイルを選択してください");
	/**
		↑これ呼ぶだけ。strFileNameに選択されたファイルがフルパスで返る
	*/

};

}}
//////////////////////////////////////////////////////////////////////////////

#ifndef yaneSDK_GCC_Mode_NOWIN
#include <winreg.h>
#endif // yaneSDK_GCC_Mode_NOWIN

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {
class CRegistry {
/**
	レジストリ操作用クラス

	使用例:
		CRegistry reg("Software\\yaneu");
			// HKEY_CURRENT_USER\\Software\\yaneu　フォルダ上になる
			//	↑↑このHKEY_CURRENT_USERはコンストラクタの第二引数で設定できる
		reg.CreateFolder("yaneu");
			//	↑そのなかにさらにyaneuというフォルダを作成
		reg.SetValue("yaneu","passwd","test");
			//	↑そのフォルダのなかにpasswdというキーでtestという文字列をストア
		reg.SetValue("yaneu","pass",1234);
			//	↑値をストア

		ret.GetValue("yaneu","pass",dw);
			//　↑値の取得。取得できなかったときは、返し値が非0なので、
			//	必ずそれをチェックすること！

		文字データは、GetValueで数字としては取得できない
		また、数字データは、GetValueで文字としては取得できない
		（ともにエラーリターンする）

		ret.DeleteFolder("yaneu");
			//	yaneuフォルダを削除。
			//	上の例ではHKEY_CURRENT_USER\\Software\\yaneu\\yaneu が削除される
			//	HKEY_CURRENT_USER\\Software\\yaneuは残る

		reg.DeleteFolder("");
			//	ルートフォルダを削除。
			//	上の例ではHKEY_CURRENT_USER\\Software\\yaneu が削除される

*/
public:

	LRESULT CreateFolder (const string& strSection);
	///	レジストリ上にフォルダを作成

	LRESULT DeleteFolder (const string& strSection);
	///	レジストリ上のフォルダを削除

	LRESULT DeleteKey	 (const string& strSection, const string& strKey);
	///	レジストリ上のキー（とそれに付随するデータ）を削除


	LRESULT	GetValue(const string& strSection,
			const string& strKey, string& strValue);
	///	文字データをレジストリから取得

	LRESULT	GetValue (const string& strSection,
			const string& strKey, DWORD& dwValue);
	///	数字データをレジストリから取得

	LRESULT	 GetValue (const string& strSection,
			const string& strKey, BYTE*pData,int& nSize);
	/**
		バイトデータをレジストリから取得
		sizeには、読み込みたいサイズを指定しておく。
		そして、sizeには、取得できたデータサイズが返る
		指定した値が、実際のデータサイズと異なる場合は読み込みに失敗する。
		そのとき、nSizeには、実際のデータサイズが入り、関数の返し値として2が返る
	*/

	LRESULT	 SetValue (const string& strSection,
			const string& strKey, const string& strValue);
	///	文字データをレジストリに保存

	LRESULT	 SetValue (const string& strSection,
			const string& strKey, DWORD dwValue);
	///	数字データをレジストリに保存

	LRESULT	 SetValue (const string& strSection,
			const string& strKey, BYTE*pData,int nSize);
	///	バイトデータをレジストリに保存

	CRegistry(const string& strPath, HKEY hkeyRoot = HKEY_CURRENT_USER);
	/**
		↑第２パラメータは
			HKEY_CLASSES_ROOT
			HKEY_CURRENT_CONFIG
			HKEY_CURRENT_USER
			HKEY_LOCAL_MACHINE
			HKEY_USERS
		から指定すべし
	*/

	virtual ~CRegistry();

protected:
	string	m_strPath;
	HKEY	m_hkeyRoot;
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
