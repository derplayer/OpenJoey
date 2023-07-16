#include "stdafx.h"
#include "yaneShell.h"
#include "yaneFile.h"
#include "../AppFrame/yaneAppInitializer.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

LRESULT	CShell::Execute(const string& strExeName,bool bWait){
	//　ファイル実行
	CHAR buf[512];
	::lstrcpy(buf,strExeName.c_str());

	STARTUPINFO startupInfo = { 0 } ;
	PROCESS_INFORMATION ProcessInfo;
	
	if (!::CreateProcess(NULL,buf,NULL,NULL,FALSE,
		CREATE_NO_WINDOW,NULL,NULL,&startupInfo,&ProcessInfo))
		//	CREATE_NO_WINDOWは、MS-DOS窓を作らないオプション
		//	（標準出力を殺したいので）
		return 1;	// unable open..

	if (bWait){
		::WaitForSingleObject(ProcessInfo.hProcess ,INFINITE);
	}

	return 0;
}

LRESULT	CShell::OpenFolder(const string& strPathName){
	//	フォルダをexplorerで開く

	CHAR buf[512];
	::lstrcpy(buf,"explorer ");
	::lstrcat(buf,strPathName.c_str());

	STARTUPINFO startupInfo = { 0 };
	PROCESS_INFORMATION ProcessInfo;

	if (!::CreateProcess(NULL,buf,NULL,NULL,FALSE,
		0,NULL,NULL,&startupInfo,&ProcessInfo))
		return 1;	// unable open..

	return 0;
}

LRESULT CShell::CopyAndSetWallPaper(const string& strFileName){
	CFile file;
	if (file.Read(strFileName)!=0) return 1;

	//	システムフォルダにコピー
	string name;
	name =	CFile::GetWindowsDir() + file.GetPureFileName();
	if (file.WriteBack(name)!=0) return 1;
	return SetWallPaper(name);
}

LRESULT CShell::SetWallPaper(const string& strFileName){
	//	壁紙を設定する

	//	アクティブデスクトップのために、レジストリも変更しなければならない

//	HKEY_CURRENT_USER/Desktop/WindowMetrics/WallPaper
//										   /WallpaperStyle/←0
//	↑このセクションの存在の有無は調べなくてはならない

	return ::SystemParametersInfo(
		SPI_SETDESKWALLPAPER,		//	壁紙の変更
		0,
		(void*)strFileName.c_str(),
		SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE
		//	変更は通知したほうがいいだろう..
	)?0:1;

}

string	CShell::getModuleName() const {
	CHAR szFileName[_MAX_PATH];
	::GetModuleFileName(CAppInitializer::GetInstance(),szFileName,_MAX_PATH);
	return string(szFileName);
}

string	CShell::getModuleMutexName() const {
	//	起動Exe名を、多重起動防止のMutex名とする
	CHAR szFileName[_MAX_PATH];
	::GetModuleFileName(CAppInitializer::GetInstance(),szFileName,_MAX_PATH);
	//	Mutex名には\は使えないので+に置換
	for(int i=0;i<_MAX_PATH;i++){
		if (szFileName[i]=='\\') szFileName[i] = '?';
		// ? はMutex名には使えるがファイル名には出現しないのでＯＫ
		else if (szFileName[i]=='\0') break;
	}
	return string(szFileName);
}

//////////////////////////////////////////////////////////////////////////////
//	CFolderDialog
//////////////////////////////////////////////////////////////////////////////

//#include <direct.h>
//#include <shellapi.h>

//	フォルダ選択ダイアログの起動
LRESULT CFolderDialog::GetOpenFolderName(string& Buffer,
		const string& DefaultFolder,const string& Title)
{
	BROWSEINFO bi = { NULL };
//	bi.hwndOwner = NULL;

	//	コールバック関数を指定
	bi.lpfn	  = &CFolderDialog::SHBrowseProc;
	//	デフォルトで選択させておくフォルダを指定
	bi.lParam = (LPARAM)DefaultFolder.c_str();
	//	タイトルの指定
	bi.lpszTitle = Title.c_str();	//"フォルダを選択してください";

	//	フォルダダイアログの起動
	ITEMIDLIST* pidl = ::SHBrowseForFolder( &bi );
	if( pidl )
	{
		//	選択されたフォルダ名を取得
		CHAR szSelectedFolder[MAX_PATH];
		::SHGetPathFromIDList( pidl, szSelectedFolder );
		_SHFree(pidl);
		if( (DWORD)lstrlen(szSelectedFolder) < MAX_PATH ){
			Buffer = szSelectedFolder;
		}else{
		//	こんなの返すとは、(´･ω･`)ｼｮﾎﾞｰﾝ　だな..
			Buffer.erase();
			return 2; // エラー扱いヽ(`Д´)ノ
		}
		//	フォルダが選択された
		return 0; // IDOK;
	}
	//	フォルダは選択されなかった
	return 1; // IDCANCEL;
}

/////////////////////////////////////////////////////////////////////////////
//	SHBrowseForFolder()用コールバック関数

int CALLBACK CFolderDialog::SHBrowseProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if( uMsg == BFFM_INITIALIZED && lpData )
	{
		//	デフォルトで選択させるパスの指定
		::SendMessage( hWnd, BFFM_SETSELECTION, TRUE, lpData);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//	システムが確保したITEMIDLISTを開放しなければならない

void CFolderDialog::_SHFree(ITEMIDLIST* pidl)
{
	IMalloc*  pMalloc;
	::SHGetMalloc( &pMalloc );
	if ( pMalloc )
	{
		pMalloc->Free( pidl );
		pMalloc->Release();
	}
}

/////////////////////////////////////////////////////////////////////////////
//	CFileDialog
/////////////////////////////////////////////////////////////////////////////
}}
#include <Commdlg.h>

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

LRESULT CFileDialog::GetOpenFile(string& strFileName,const string& DefaultFolder
	 ,const string& Title)
{
	
	OPENFILENAME ofn = { sizeof (OPENFILENAME) };
#if (_WIN32_WINNT >= 0x0500)
		ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400;
#endif
	//	Windows95/NT4.0でも動かにゃいかん　↑この識別子は
	//	WIN32_WINNTの値が0x0500未満の場合、未定義となる
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	ofn.nMaxFile = 1;
	ofn.lpstrTitle = Title.c_str();

	BOOL b = ::GetOpenFileName(&ofn);
	if (!b) {
		return -1; // ok以外クリックしたな?
	} else {
		strFileName = ofn.lpstrFile;
		return 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
//	CRegistry
/////////////////////////////////////////////////////////////////////////////
//#include <tchar.h>

CRegistry::CRegistry (const string& strPath, HKEY hkeyRoot) :
		m_strPath (strPath), m_hkeyRoot (hkeyRoot){}

CRegistry::~CRegistry (){}

///

LRESULT CRegistry::CreateFolder (const string& strSection)
{
	HKEY hKey;
	string strTemp;

	if (!strSection.empty ())
	{
		strTemp = string("\\") + strSection;
	}
	DWORD dwDisposition;
	string fullname;
	fullname = m_strPath + strTemp;
	if (::RegCreateKeyEx (
		m_hkeyRoot,
		fullname.c_str(),
		0, "", REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) !=
		ERROR_SUCCESS)
	{
		return 1;
	}

	::RegCloseKey (hKey);
	return 0;
}

LRESULT CRegistry::DeleteFolder (const string& strSection)
{
	HKEY hKey;

	if (::RegOpenKeyEx (
		m_hkeyRoot, m_strPath.c_str(), 0, KEY_ALL_ACCESS, &hKey) !=
		ERROR_SUCCESS)
	{
		return 1;
	}

	if (::RegDeleteKey (hKey, strSection.c_str()) != ERROR_SUCCESS)
	{
		::RegCloseKey (hKey);
		return 1;
	}

	::RegCloseKey (hKey);
	return 0;
}

LRESULT CRegistry::DeleteKey (const string& strSection,
							   const string& strKey)
{
	HKEY hKey;

	string strTemp;
	if (!strSection.empty ())
	{
		strTemp = string("\\") + strSection;
	}
	string fullname;
	fullname = m_strPath + strTemp;
	if (::RegOpenKeyEx (
		m_hkeyRoot, fullname.c_str() , 0, KEY_WRITE, &hKey) !=
		ERROR_SUCCESS)
	{
		return 1;
	}
	if (::RegDeleteValue (hKey, strKey.c_str()) != ERROR_SUCCESS)
	{
		::RegCloseKey (hKey);
		return 1;
	}

	::RegCloseKey (hKey);
	return 0;
}

///

LRESULT CRegistry::GetValue (const string& strSection,
		const string& strKey, string& strValue)
{
	HKEY  hKey;

	string fullname;
	fullname = m_strPath + '\\' + strSection;
	if (::RegOpenKeyEx (m_hkeyRoot,fullname.c_str() , 0, KEY_READ, &hKey)
		!= ERROR_SUCCESS)
	{
		return 1;
	}

	BYTE pbyBuf [256];
	DWORD dwSize = sizeof(pbyBuf);
	DWORD dwType;
	if (::RegQueryValueEx (hKey, strKey.c_str(), NULL, &dwType, pbyBuf, &dwSize)
		!= ERROR_SUCCESS)
	{
		::RegCloseKey(hKey);
		return 1;
	}

	::RegCloseKey(hKey);

	if  (dwType != REG_SZ){ // データタイプが文字列じゃない
		return 2;
	}

	strValue = (LPCSTR)&pbyBuf[0];
	return 0;
}

LRESULT CRegistry::GetValue (const string& strSection,
		const string& strKey, DWORD &dwValue)
{
	HKEY  hKey = 0;

	string fullname;
	fullname = m_strPath + '\\' + strSection;
	if (::RegOpenKeyEx (m_hkeyRoot, fullname.c_str() , 0, KEY_READ, &hKey)
		!= ERROR_SUCCESS)
	{
		return 1;
	}

	DWORD dwSize = sizeof(DWORD);
	DWORD dwType;
	if (::RegQueryValueEx (hKey, strKey.c_str(), NULL, &dwType,
		(BYTE*)&dwValue, &dwSize) != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey);
		return 1;
	}

	::RegCloseKey(hKey);
	if (dwType!=REG_DWORD) return 2; // これ型が違う

	return 0;
}

LRESULT CRegistry::SetValue (const string& strSection,
		const string& strKey, const string& strValue)
{
	HKEY hKey;

	string fullname;
	fullname = m_strPath + '\\' + strSection;
	if (::RegOpenKeyEx (m_hkeyRoot, fullname.c_str(), 0, KEY_WRITE, &hKey)
		!= ERROR_SUCCESS)
	{
		DWORD dwDisposition;
		if (::RegCreateKeyEx (m_hkeyRoot, fullname.c_str(), 0
			, "" , REG_OPTION_NON_VOLATILE,
			  KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) !=
			  ERROR_SUCCESS)
		{
			 return 1;
		}
	}

	if (::RegSetValueEx (hKey, strKey.c_str(), NULL, REG_SZ,
			(const BYTE*)strValue.c_str(), strValue.size()) !=
				ERROR_SUCCESS)
	{
		::RegCloseKey (hKey);
		return 1;
	}

	::RegCloseKey (hKey);
	return 0;
}

LRESULT CRegistry::SetValue (const string& strSection,
		const string& strKey, DWORD dwValue)
{
	HKEY hKey;

	string fullname;
	fullname = m_strPath + '\\' + strSection;
	if (::RegOpenKeyEx (m_hkeyRoot, fullname.c_str(), 0, KEY_WRITE, &hKey)
		!= ERROR_SUCCESS)
	{
		DWORD dwDisposition;
		if (::RegCreateKeyEx (m_hkeyRoot, fullname.c_str(), 0, "", REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) != ERROR_SUCCESS)
		{
			return 1;
		}
	}

	if (::RegSetValueEx (hKey, strKey.c_str(), NULL, REG_DWORD,
		(BYTE*)&dwValue, sizeof(DWORD)) != ERROR_SUCCESS)
	{
		::RegCloseKey (hKey);
		return 1;
	}

	::RegCloseKey (hKey);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

//template <class T>
LRESULT CRegistry::SetValue (const string& strSection,
		const string& strKey, BYTE*pData,int nSize)
{
	HKEY hKey;

	string fullname;
	fullname = m_strPath + '\\' + strSection;
	if (::RegOpenKeyEx (m_hkeyRoot, fullname.c_str(), 0, KEY_WRITE, &hKey)
		!= ERROR_SUCCESS)
	{
		DWORD dwDisposition;
		if (::RegCreateKeyEx (m_hkeyRoot, fullname.c_str(), 0, "", REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) != ERROR_SUCCESS)
		{
			return 1;
		}
	}

//	if (obj.isNull()) return -1; // オブジェクトあらへんかった(ρ_；)ﾉ
	if (::RegSetValueEx (hKey, strKey.c_str(), NULL, REG_BINARY,
		pData /*(BYTE*)obj.get()*/, nSize /*obj.size()*sizeof(BYTE)*/ ) != ERROR_SUCCESS)
	{
		::RegCloseKey (hKey);
		return 1;
	}

	::RegCloseKey (hKey);
	return 0;
}

//template <class T>
LRESULT CRegistry::GetValue (const string& strSection,
		const string& strKey, BYTE *pData,int &nSize)
{
	HKEY  hKey = 0;

	string fullname;
	fullname = m_strPath + '\\' + strSection;
	if (::RegOpenKeyEx (m_hkeyRoot, fullname.c_str() , 0, KEY_READ, &hKey)
		!= ERROR_SUCCESS)
	{
		return 1;
	}

//	DWORD dwSize = obj.size()*sizeof(BYTE);
//	if (obj.isNull()) return -1; // オブジェクトあらへんかった(ρ_；)ﾉ
	//	↑ここで自前で確保してもいいんだが．．
	DWORD dwType;
	if (::RegQueryValueEx (hKey, strKey.c_str(), NULL, &dwType,
		pData/*(BYTE*)obj.get()*/, (DWORD*)&nSize) != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey);
		return 2;
	}

	::RegCloseKey(hKey);
	if (dwType!=REG_BINARY) return 3; // これ型が違う

	return 0;
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
