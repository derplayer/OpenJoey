//
//	yaneShell.h :
//
//		�V�F���R�}���h���s�֌W
//
//

#ifndef __yaneShell_h__
#define __yaneShell_h__


namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class CShell {
public:
	LRESULT	Execute(const string& strExeName,bool bWait=false);
	///�@---�t�@�C�����s
	///	�O���R�}���h�����s���܂��BbWait==true�̏ꍇ�́A���̃R�}���h�̏I����҂��܂��B

	LRESULT	OpenFolder(const string& strPathName);
	///	---�t�H���_��explorer�ŊJ��
	///	�w��̃t�H���_���J���܂��B

	LRESULT SetWallPaper(const string& strFileName);
	///	�w��̃t�@�C����ǎ��ɂ��܂��ByanePack/Ex/Dx�ň��k����Ă���
	///	�t�@�C���͕s�ł��B

	LRESULT CopyAndSetWallPaper(const string& strFileName);
	///	�w��̃t�@�C����Windows�t�H���_�ɃR�s�[���A�����ǎ��ɂ��܂��B
	///	yanePack/yanePackEx�ň��k����Ă���t�@�C���ł��\�ł��B

	string getModuleMutexName() const;
	///	�N��Exe�����瑽�d�N���h�~�p��Mutex���ɕϊ����ĕԂ�

	string getModuleName() const;
	///	�N�����W���[������Ԃ�(full path)
};

}}

#ifndef yaneSDK_GCC_Mode_NOWIN
#include <Shlobj.h>
#endif // yaneSDK_GCC_Mode_NOWIN

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {
class CFolderDialog {
/**
	�t�H���_�I��p�_�C�A���O(Windows�W���̂���)
		�Q�[���ł͎g�����Ƃ͂��܂�Ȃ����낤�����..

		��MSDN�ɁAapartment threading��CoInitialize���Ȃ͂��
		�����Ă���񂾂��A�ǂ��������񂩁D�D
	Remarks:
	SHBrowseForFolder. You can also use CoInitialize or OLEInitialize,
	which always use apartment threading.
*/
public:
	LRESULT GetOpenFolderName(string& strFolderName, const string& DefaultFolder=""
		,const string& Title = "�t�H���_��I�����Ă�������");
	/**
		������ĂԂ����BstrFolderName�ɑI�����ꂽ�t�H���_���Ԃ��Ă���
		DefaultFolder�́A�f�B�t�H���g�t�H���_��n���Ɨǂ��B�ȗ��\�B
		�Ԃ����t�H���_���̖����ɂ́@���@�}�[�N�͂��Ă��Ȃ�
	*/

protected:
	static int CALLBACK SHBrowseProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
	void _SHFree(ITEMIDLIST* pidl);
};

class CFileDialog {
///	���̃N���X�͍�肩��
public:
	LRESULT GetOpenFile(string& strFileName,const string& DefaultFolder=""
		,const string& Title = "�t�@�C����I�����Ă�������");
	/**
		������ĂԂ����BstrFileName�ɑI�����ꂽ�t�@�C�����t���p�X�ŕԂ�
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
	���W�X�g������p�N���X

	�g�p��:
		CRegistry reg("Software\\yaneu");
			// HKEY_CURRENT_USER\\Software\\yaneu�@�t�H���_��ɂȂ�
			//	��������HKEY_CURRENT_USER�̓R���X�g���N�^�̑������Őݒ�ł���
		reg.CreateFolder("yaneu");
			//	�����̂Ȃ��ɂ����yaneu�Ƃ����t�H���_���쐬
		reg.SetValue("yaneu","passwd","test");
			//	�����̃t�H���_�̂Ȃ���passwd�Ƃ����L�[��test�Ƃ�����������X�g�A
		reg.SetValue("yaneu","pass",1234);
			//	���l���X�g�A

		ret.GetValue("yaneu","pass",dw);
			//�@���l�̎擾�B�擾�ł��Ȃ������Ƃ��́A�Ԃ��l����0�Ȃ̂ŁA
			//	�K��������`�F�b�N���邱�ƁI

		�����f�[�^�́AGetValue�Ő����Ƃ��Ă͎擾�ł��Ȃ�
		�܂��A�����f�[�^�́AGetValue�ŕ����Ƃ��Ă͎擾�ł��Ȃ�
		�i�Ƃ��ɃG���[���^�[������j

		ret.DeleteFolder("yaneu");
			//	yaneu�t�H���_���폜�B
			//	��̗�ł�HKEY_CURRENT_USER\\Software\\yaneu\\yaneu ���폜�����
			//	HKEY_CURRENT_USER\\Software\\yaneu�͎c��

		reg.DeleteFolder("");
			//	���[�g�t�H���_���폜�B
			//	��̗�ł�HKEY_CURRENT_USER\\Software\\yaneu ���폜�����

*/
public:

	LRESULT CreateFolder (const string& strSection);
	///	���W�X�g����Ƀt�H���_���쐬

	LRESULT DeleteFolder (const string& strSection);
	///	���W�X�g����̃t�H���_���폜

	LRESULT DeleteKey	 (const string& strSection, const string& strKey);
	///	���W�X�g����̃L�[�i�Ƃ���ɕt������f�[�^�j���폜


	LRESULT	GetValue(const string& strSection,
			const string& strKey, string& strValue);
	///	�����f�[�^�����W�X�g������擾

	LRESULT	GetValue (const string& strSection,
			const string& strKey, DWORD& dwValue);
	///	�����f�[�^�����W�X�g������擾

	LRESULT	 GetValue (const string& strSection,
			const string& strKey, BYTE*pData,int& nSize);
	/**
		�o�C�g�f�[�^�����W�X�g������擾
		size�ɂ́A�ǂݍ��݂����T�C�Y���w�肵�Ă����B
		�����āAsize�ɂ́A�擾�ł����f�[�^�T�C�Y���Ԃ�
		�w�肵���l���A���ۂ̃f�[�^�T�C�Y�ƈقȂ�ꍇ�͓ǂݍ��݂Ɏ��s����B
		���̂Ƃ��AnSize�ɂ́A���ۂ̃f�[�^�T�C�Y������A�֐��̕Ԃ��l�Ƃ���2���Ԃ�
	*/

	LRESULT	 SetValue (const string& strSection,
			const string& strKey, const string& strValue);
	///	�����f�[�^�����W�X�g���ɕۑ�

	LRESULT	 SetValue (const string& strSection,
			const string& strKey, DWORD dwValue);
	///	�����f�[�^�����W�X�g���ɕۑ�

	LRESULT	 SetValue (const string& strSection,
			const string& strKey, BYTE*pData,int nSize);
	///	�o�C�g�f�[�^�����W�X�g���ɕۑ�

	CRegistry(const string& strPath, HKEY hkeyRoot = HKEY_CURRENT_USER);
	/**
		����Q�p�����[�^��
			HKEY_CLASSES_ROOT
			HKEY_CURRENT_CONFIG
			HKEY_CURRENT_USER
			HKEY_LOCAL_MACHINE
			HKEY_USERS
		����w�肷�ׂ�
	*/

	virtual ~CRegistry();

protected:
	string	m_strPath;
	HKEY	m_hkeyRoot;
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
