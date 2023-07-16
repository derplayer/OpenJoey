#include "stdafx.h"

#include "yaneFindWindow.h"
#include "../Auxiliary/yaneFile.h"

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

LRESULT CFindWindow::Find(const string& strModuleName) {
	if (InitProcessFunction()!=0) return 1; // 初期化失敗
	m_strModuleName = CFile::GetPureFileNameOf(strModuleName);
	return FindSameWindow();
}

LRESULT		CFindWindow::FindSameWindow( )
{
// ウィンドウの列挙要求
	if ( !::EnumWindows( (WNDENUMPROC)CFindWindow::EnumWndProc, (LONG)this ) )
						// ↑このキャストをしないとVC++5で通らないのだ＾＾；；
		return 0;
	return 1;
}

int	CALLBACK	CFindWindow::EnumWndProc( HWND hWnd, LPARAM lParam ){
	return ((CFindWindow*)lParam) ->InnerEnumWndProc(hWnd);
}

int		CFindWindow::InnerEnumWndProc( HWND hWnd ){

// インスタンスに対するファイル名の取得
	CHAR	strFileName[_MAX_PATH];
	if (GetWindowModuleName( hWnd , strFileName, _MAX_PATH )!=0)
		return 1;
	//	WinNT系では、ファイル名しか得られない。
	//	Win95/98系では、パスも得られる
	//	統一するために、ここでは、ファイル名のみを抽出し比較する必要がある

	// ファイル名が等しいか否かの判定
	if ( m_strModuleName == CFile::GetPureFileNameOf(strFileName)) {

		// 以前に作成したウィンドウをアクティブにする
		::ShowWindow( hWnd ,SW_SHOWNORMAL);
		::SetForegroundWindow( hWnd );

		return 0;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
//	プロセスのスナップをとる
//		こうしないと、正確なハンドル名が得られない

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
