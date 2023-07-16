#include "stdafx.h"
#include "yaneShell.h"
#include "yaneFile.h"
#include "../AppFrame/yaneAppInitializer.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

LRESULT	CShell::Execute(const string& strExeName,bool bWait){
	//�@�t�@�C�����s
	CHAR buf[512];
	::lstrcpy(buf,strExeName.c_str());

	STARTUPINFO startupInfo = { 0 } ;
	PROCESS_INFORMATION ProcessInfo;
	
	if (!::CreateProcess(NULL,buf,NULL,NULL,FALSE,
		CREATE_NO_WINDOW,NULL,NULL,&startupInfo,&ProcessInfo))
		//	CREATE_NO_WINDOW�́AMS-DOS�������Ȃ��I�v�V����
		//	�i�W���o�͂��E�������̂Łj
		return 1;	// unable open..

	if (bWait){
		::WaitForSingleObject(ProcessInfo.hProcess ,INFINITE);
	}

	return 0;
}

LRESULT	CShell::OpenFolder(const string& strPathName){
	//	�t�H���_��explorer�ŊJ��

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

	//	�V�X�e���t�H���_�ɃR�s�[
	string name;
	name =	CFile::GetWindowsDir() + file.GetPureFileName();
	if (file.WriteBack(name)!=0) return 1;
	return SetWallPaper(name);
}

LRESULT CShell::SetWallPaper(const string& strFileName){
	//	�ǎ���ݒ肷��

	//	�A�N�e�B�u�f�X�N�g�b�v�̂��߂ɁA���W�X�g�����ύX���Ȃ���΂Ȃ�Ȃ�

//	HKEY_CURRENT_USER/Desktop/WindowMetrics/WallPaper
//										   /WallpaperStyle/��0
//	�����̃Z�N�V�����̑��݂̗L���͒��ׂȂ��Ă͂Ȃ�Ȃ�

	return ::SystemParametersInfo(
		SPI_SETDESKWALLPAPER,		//	�ǎ��̕ύX
		0,
		(void*)strFileName.c_str(),
		SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE
		//	�ύX�͒ʒm�����ق����������낤..
	)?0:1;

}

string	CShell::getModuleName() const {
	CHAR szFileName[_MAX_PATH];
	::GetModuleFileName(CAppInitializer::GetInstance(),szFileName,_MAX_PATH);
	return string(szFileName);
}

string	CShell::getModuleMutexName() const {
	//	�N��Exe�����A���d�N���h�~��Mutex���Ƃ���
	CHAR szFileName[_MAX_PATH];
	::GetModuleFileName(CAppInitializer::GetInstance(),szFileName,_MAX_PATH);
	//	Mutex���ɂ�\�͎g���Ȃ��̂�+�ɒu��
	for(int i=0;i<_MAX_PATH;i++){
		if (szFileName[i]=='\\') szFileName[i] = '?';
		// ? ��Mutex���ɂ͎g���邪�t�@�C�����ɂ͏o�����Ȃ��̂łn�j
		else if (szFileName[i]=='\0') break;
	}
	return string(szFileName);
}

//////////////////////////////////////////////////////////////////////////////
//	CFolderDialog
//////////////////////////////////////////////////////////////////////////////

//#include <direct.h>
//#include <shellapi.h>

//	�t�H���_�I���_�C�A���O�̋N��
LRESULT CFolderDialog::GetOpenFolderName(string& Buffer,
		const string& DefaultFolder,const string& Title)
{
	BROWSEINFO bi = { NULL };
//	bi.hwndOwner = NULL;

	//	�R�[���o�b�N�֐����w��
	bi.lpfn	  = &CFolderDialog::SHBrowseProc;
	//	�f�t�H���g�őI�������Ă����t�H���_���w��
	bi.lParam = (LPARAM)DefaultFolder.c_str();
	//	�^�C�g���̎w��
	bi.lpszTitle = Title.c_str();	//"�t�H���_��I�����Ă�������";

	//	�t�H���_�_�C�A���O�̋N��
	ITEMIDLIST* pidl = ::SHBrowseForFolder( &bi );
	if( pidl )
	{
		//	�I�����ꂽ�t�H���_�����擾
		CHAR szSelectedFolder[MAX_PATH];
		::SHGetPathFromIDList( pidl, szSelectedFolder );
		_SHFree(pidl);
		if( (DWORD)lstrlen(szSelectedFolder) < MAX_PATH ){
			Buffer = szSelectedFolder;
		}else{
		//	����Ȃ̕Ԃ��Ƃ́A(�L��֥`)���ް݁@����..
			Buffer.erase();
			return 2; // �G���[�����R(`�D�L)�m
		}
		//	�t�H���_���I�����ꂽ
		return 0; // IDOK;
	}
	//	�t�H���_�͑I������Ȃ�����
	return 1; // IDCANCEL;
}

/////////////////////////////////////////////////////////////////////////////
//	SHBrowseForFolder()�p�R�[���o�b�N�֐�

int CALLBACK CFolderDialog::SHBrowseProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if( uMsg == BFFM_INITIALIZED && lpData )
	{
		//	�f�t�H���g�őI��������p�X�̎w��
		::SendMessage( hWnd, BFFM_SETSELECTION, TRUE, lpData);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//	�V�X�e�����m�ۂ���ITEMIDLIST���J�����Ȃ���΂Ȃ�Ȃ�

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
	//	Windows95/NT4.0�ł������ɂႢ����@�����̎��ʎq��
	//	WIN32_WINNT�̒l��0x0500�����̏ꍇ�A����`�ƂȂ�
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	ofn.nMaxFile = 1;
	ofn.lpstrTitle = Title.c_str();

	BOOL b = ::GetOpenFileName(&ofn);
	if (!b) {
		return -1; // ok�ȊO�N���b�N������?
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

	if  (dwType != REG_SZ){ // �f�[�^�^�C�v�������񂶂�Ȃ�
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
	if (dwType!=REG_DWORD) return 2; // ����^���Ⴄ

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

//	if (obj.isNull()) return -1; // �I�u�W�F�N�g����ւ񂩂���(��_�G)�
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
//	if (obj.isNull()) return -1; // �I�u�W�F�N�g����ւ񂩂���(��_�G)�
	//	�������Ŏ��O�Ŋm�ۂ��Ă������񂾂��D�D
	DWORD dwType;
	if (::RegQueryValueEx (hKey, strKey.c_str(), NULL, &dwType,
		pData/*(BYTE*)obj.get()*/, (DWORD*)&nSize) != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey);
		return 2;
	}

	::RegCloseKey(hKey);
	if (dwType!=REG_BINARY) return 3; // ����^���Ⴄ

	return 0;
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
