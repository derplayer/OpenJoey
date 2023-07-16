#include "stdafx.h"

#include "yaneFindWindow.h"
#include "../Auxiliary/yaneFile.h"

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

LRESULT CFindWindow::Find(const string& strModuleName) {
	if (InitProcessFunction()!=0) return 1; // ���������s
	m_strModuleName = CFile::GetPureFileNameOf(strModuleName);
	return FindSameWindow();
}

LRESULT		CFindWindow::FindSameWindow( )
{
// �E�B���h�E�̗񋓗v��
	if ( !::EnumWindows( (WNDENUMPROC)CFindWindow::EnumWndProc, (LONG)this ) )
						// �����̃L���X�g�����Ȃ���VC++5�Œʂ�Ȃ��̂��O�O�G�G
		return 0;
	return 1;
}

int	CALLBACK	CFindWindow::EnumWndProc( HWND hWnd, LPARAM lParam ){
	return ((CFindWindow*)lParam) ->InnerEnumWndProc(hWnd);
}

int		CFindWindow::InnerEnumWndProc( HWND hWnd ){

// �C���X�^���X�ɑ΂���t�@�C�����̎擾
	CHAR	strFileName[_MAX_PATH];
	if (GetWindowModuleName( hWnd , strFileName, _MAX_PATH )!=0)
		return 1;
	//	WinNT�n�ł́A�t�@�C�������������Ȃ��B
	//	Win95/98�n�ł́A�p�X��������
	//	���ꂷ�邽�߂ɁA�����ł́A�t�@�C�����݂̂𒊏o����r����K�v������

	// �t�@�C���������������ۂ��̔���
	if ( m_strModuleName == CFile::GetPureFileNameOf(strFileName)) {

		// �ȑO�ɍ쐬�����E�B���h�E���A�N�e�B�u�ɂ���
		::ShowWindow( hWnd ,SW_SHOWNORMAL);
		::SetForegroundWindow( hWnd );

		return 0;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
//	�v���Z�X�̃X�i�b�v���Ƃ�
//		�������Ȃ��ƁA���m�ȃn���h�����������Ȃ�

LRESULT	CFindWindow::InitProcessFunction(void)
{
	HANDLE hKernel;
	hKernel = ::GetModuleHandle("KERNEL32.DLL");

	if ( hKernel ) {
		m_lpCreateToolhelp32Snapshot = (CREATESNAPSHOT)GetProcAddress(
			(HINSTANCE)hKernel, "CreateToolhelp32Snapshot");
		m_lpProcess32First =
			(PROCESSWALK)::GetProcAddress((HINSTANCE)hKernel, "Process32First");
		m_lpProcess32Next =
			(PROCESSWALK)::GetProcAddress((HINSTANCE)hKernel, "Process32Next");
	}
	if ( m_lpProcess32First && m_lpProcess32Next && m_lpCreateToolhelp32Snapshot )
		return 0;
	return 1;
}

LRESULT	CFindWindow::GetWindowModuleName( HWND hWnd , LPTSTR szFileName, DWORD dwSize )
{
	DWORD pid = 0;
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	LRESULT Ret = 1;

	::GetWindowThreadProcessId(hWnd, &pid);
	hProcessSnap = m_lpCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if ( hProcessSnap == (HANDLE)-1 )
		return 1;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if ( m_lpProcess32First(hProcessSnap, &pe32) ) {
		do {
			if ( pid == pe32.th32ProcessID ) {
				::lstrcpyn(szFileName, pe32.szExeFile, dwSize);
				Ret = 0;
				break;
			}
		} while ( m_lpProcess32Next(hProcessSnap, &pe32) );
	}
	::CloseHandle(hProcessSnap);
	return Ret;
}

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd
