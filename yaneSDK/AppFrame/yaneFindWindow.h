//
//	�Q�d�N���h�~���̃t�H�[�J�X�̈ړ�
//

#ifndef __yaneFindWindow_h__
#define __yaneFindWindow_h__

#ifndef yaneSDK_GCC_Mode_NOWIN
#include <tlhelp32.h>
#endif // yaneSDK_GCC_Mode_NOWIN

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

typedef BOOL (WINAPI *PROCESSWALK)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef HANDLE (WINAPI *CREATESNAPSHOT)(DWORD dwFlags, DWORD th32ProcessID);

class CFindWindow {
public:
	//	���s���W���[������^����ƁA���ۗ̕L�E�B���h�D�Ƀt�H�[�J�X���ڂ�
	LRESULT Find(const string& strModuleName);

protected:
	LRESULT	FindSameWindow();
	int		InnerEnumWndProc( HWND hWnd );
	static int	CALLBACK	EnumWndProc( HWND hWnd, LPARAM lParam );
	string	m_strModuleName;

	//	----- �v���Z�X�̃X�i�b�v���Ƃ� from MSDN sample
	LRESULT	InitProcessFunction(void);
	LRESULT	GetWindowModuleName( HWND hWnd , LPTSTR szFileName, DWORD dwSize );

	CREATESNAPSHOT	m_lpCreateToolhelp32Snapshot;
	PROCESSWALK		m_lpProcess32First;
	PROCESSWALK		m_lpProcess32Next;
};

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

#endif
