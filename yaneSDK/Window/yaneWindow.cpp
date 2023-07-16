// yaneWindow.cpp
#include "stdafx.h"
#include "yaneWindow.h"
#include "yaneWinHook.h"
#include "../Auxiliary/yaneIMEWrapper.h"
#include "../AppFrame/yaneAppInitializer.h"
#include "../Auxiliary/yaneStream.h"
#include "../AppFrame/yaneAppManager.h"

namespace yaneuraoGameSDK3rd {
namespace Window {

bool CWindow::g_bFullScreen		= false; // �N������̓E�B���h�D���[�h�ł���I
CCriticalSection CWindow::m_cs;

//////////////////////////////////////////////////////////////////////////////

CWindow::CWindow(){
	m_dwFillColor	=	0;			//	�N���������f�B�t�H���g
	m_hWnd			=	NULL;
	m_bShowCursor	=	true;		//	�f�B�t�H���g�Ń}�E�X�J�[�\����\��
	m_bUseMouseLayer=	false;		//	�\�t�g�E�F�A�}�E�X�J�[�\��
	m_bUseMenu		=	false;		//	���j���[�̂����Ȃ���͂킩���O�O�G
	m_bMinimized	=	false;		//	�ŏ�������Ă��邩�H
	m_bResized		=	false;		//	ChangeWindowStyle��SetSize�̃t���O�B
	m_pWndProc		=	NULL;		//	hook����WndProc
}

CWindow::~CWindow(){
	GetHookList()->Del(this);		//	���ڃt�b�N����������
/*
	if (m_hWnd!=NULL) {
		::DestroyWindow(m_hWnd);
	//	������ȋ����ȕ��@��Destroy����̂́A�����߂����˂�O�O�G
	//	�{���ACAppBase����g���̂Ȃ�΁A���̃f�X�g���N�^�ł́A���ł�
	//	Window�͉�̂��ꂽ���Ƃ̂͂�����...
		m_hWnd	= NULL;
	}
*/
}

//////////////////////////////////////////////////////////////////////////////
/*
LRESULT CALLBACK CWindow::gDlgProc(HWND hDlgWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
        case WM_INITDIALOG:
            return FALSE;
        case WM_COMMAND:
            switch (LOWORD(wp)) {
                case IDOK:
					::EndDialog(hDlgWnd, IDOK);
                    break;
                case IDCANCEL:
					::EndDialog(hDlgWnd, IDCANCEL);
                    break;
                default:
                    return FALSE;
            }
        default:
            return FALSE;
    }
    return TRUE;
}
*/

LRESULT CWindow::Create(CWindowOption& opt,HWND hParent){
	CCriticalLock cl(GetCriticalSection());

	HINSTANCE hInst = CAppInitializer::GetInstance();
	m_bFullScreen = g_bFullScreen;	//	���݂̉�ʃ��[�h������I�ɕێ�
	m_opt	= opt;	//	�R�s�[���Ă���

	m_hWnd = NULL;

	LONG lChild = 0;

	//	�܂��C����������T�|�[�g
	if (m_opt.dialog!=NULL){
	//	dialog���炵���ŁH
		m_hWnd = ::CreateDialogParam(hInst,m_opt.dialog, hParent, (DLGPROC)gDlgProc,
			reinterpret_cast<LPARAM>(this));
			//	��this��LPARAM�ɓ����Ă���Ă���I
		goto window_setting;
	}

    // �܂��o�^����Ă��Ȃ��E�B���h�E�N���X���ŁA����Windows�W���N���X
    // �ȊO�̖��O�̏ꍇ��RegisterClassEx()���Ă��
    WNDCLASSEX wndclass;
    if( !::GetClassInfoEx(hInst, opt.classname.c_str(), &wndclass) &&
        !IsWindowsClassName(opt.classname)) {

        // �����܂�̃E�C���h�E�N���X��������
		wndclass.cbSize		= sizeof(WNDCLASSEX);
		wndclass.style		= 0;	//	�_�u���N���b�N���m����Ȃ灨CS_DBLCLKS;
		wndclass.lpfnWndProc= gWndProc;
		wndclass.cbClsExtra	= 0;
		wndclass.cbWndExtra	= 0;
		wndclass.hInstance	= hInst;

		// �Ƃ肠�����A"MAIN"�̃A�C�R���\��
		wndclass.hIcon		= LoadIcon(hInst,"MAIN");
		wndclass.hIconSm	= LoadIcon(hInst,"MAIN");

		wndclass.hCursor	= LoadCursor(NULL,IDC_ARROW);
		// �Ƃ肠�����A�}�E�X�J�[�\�����p�Ӂi�s�v�Ȃ�΁A���Ƃŏ����ׂ��I�j

		//	�Ƃ肠�����A�N������́A�������ɂ��Ƃ����Ɓc
		if (m_dwFillColor==0) {
			wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		} else {
			wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		}

		//	MENU�����œǂݍ���ł݂�
		wndclass.lpszMenuName  = "IDR_MENU";
		//	���̃��\�[�X���{���ɑ��݂���̂��́A���s���܂ł킩��Ȃ��̂�
		//	���m�ȃE�B���h�D�̃A�h�W���X�g���E�B���h�D������ł����o���Ȃ�

		{
			HRSRC hrsrc;
			hrsrc = ::FindResource(NULL,"IDR_MENU",RT_MENU);
			if (hrsrc!=NULL) { m_bUseMenu = true; }	//	���j���[����₪��ŁO�O�G
		}

		wndclass.lpszClassName = opt.classname.c_str();

		// �Ȃ�Ŏ��s���Ă񂾂�ˁH
		if (!::RegisterClassEx(&wndclass)) {
			Err.Out("CWindow::Create��RegisterClassEx���s");
			return 1;
		}
	}

	if (hParent!=NULL) {
		lChild	= WS_CHILDWINDOW;
	}

	if (m_bFullScreen){
		//	�E�C���h�E�̐����i�t���X�N���[�����j
		//	���̂Ƃ��́A��ʂ̃X�^�C���I�v�V�����͖�������

		int sx,sy;
		GetScreenSize(sx,sy);
		m_hWnd = ::CreateWindow(opt.classname.c_str(),opt.caption.c_str(),
						WS_POPUP|WS_VISIBLE	 /* |WS_SYSMENU|
						WS_MAXIMIZEBOX|WS_MINIMIZEBOX */ ,
						0, 0, sx, sy,
						hParent, NULL, hInst, NULL );
	} else {
		// �E�B���h�D���E���l���ɓ���Ȃ��Ă͂Ȃ�Ȃ�
		// �\���͈͂�640*480�Ȃ�΁A�������ׂ�Window�T�C�Y�́A����ȏ�ł���
		RECT r;
		InnerAdjustWindow(r,opt);
		m_hWnd = ::CreateWindow(opt.classname.c_str(),opt.caption.c_str(),
				// WS_EX_TOPMOST���w�肵�����񂾂��ǂȁ[
						lChild | opt.style,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						r.right-r.left,r.bottom-r.top,
						hParent,NULL,hInst,NULL);
	}

window_setting:;
	if (m_hWnd==NULL){
		Err.Out("CWindow::Create��CreateWindow�Ɏ��s�B");
		return 1;
	}

	//	�R�[���o�b�N���ꂽ�Ƃ��ɁA���ꂼ���WindowClass��dispatch�o����悤��
	//	GWL_USERDATA��this���B���Ă����B
	::SetWindowLong(m_hWnd,GWL_USERDATA,(LONG)this);

	//	��ʂ̃Z���^�[�ɐ�������悤�ɏC���O�O
	if ((!m_bFullScreen) && (m_opt.size_x!=0) && (m_opt.size_y!=0)
			&& m_opt.bCentering) {
		int	sx,sy;
		GetScreenSize(sx,sy);
		sx	 = (sx-m_opt.size_x) >> 1;
		sy	 = (sy-m_opt.size_y) >> 1;
		SetWindowPos(sx,sy);
	}

	if ((m_opt.size_x!=0) && (m_opt.size_y!=0)){
		::ShowWindow(m_hWnd, SW_SHOW);
	}

	::SetFocus(m_hWnd);

	//	�W����WindowClass�Ȃ�΁A�R�[���o�b�N�֐��Ƃ��āA�����
	//	��������ł��B
	if (IsWindowsClassName(opt.classname)){
		m_pWndProc = (WNDPROC)::SetWindowLong(m_hWnd,GWL_WNDPROC,(LONG)gWndProc);
		//	��������hook�́Aillegal��...
	} else {
		m_pWndProc = NULL;
	}

	//	�����̃E�B���h�D�Ȃ̂Œ��ڃt�b�N�ł���
	//	�i�Ƃ�����CAppManager�ɖ��o�^�̒i�K�Ȃ̂Œ��ڃt�b�N�����Ȃ��j
	GetHookList()->Add(this);			//	�t�b�N���J�n����

	return 0;
}

void CWindow::GetScreenSize(int &x,int &y){
	x = ::GetSystemMetrics(SM_CXSCREEN);
	y = ::GetSystemMetrics(SM_CYSCREEN);
}

LRESULT CWindow::SetWindowPos(int x,int y){
	if (m_hWnd==NULL) return 0;
	//	�E�B���h�E����������Ă�����ړ������Ȃ��Ă͂Ȃ�Ȃ��i�������E�B���h�D���[�h���j
	if (!m_bFullScreen){
		//	�����Ńg�b�v�I�[�_�[�ɂ��Ă����Ă����Ȃ��̂��낤...
		return !::SetWindowPos(m_hWnd,HWND_TOP,x,y,NULL,NULL,SWP_NOSIZE|SWP_NOZORDER);
	}
	return 0;
}

void CWindow::SetSize(int sx,int sy){
	if (m_opt.size_x != sx || m_opt.size_y != sy){
		m_opt.size_x = sx;
		m_opt.size_y = sy;
		m_bResized	= true;	//	�ύX���ꂽ�Ńt���O�O�O�G
	}
}

LRESULT CWindow::Resize(int sx,int sy){
	m_opt.size_x = sx;
	m_opt.size_y = sy;

	if (m_hWnd==NULL) return 0;

	if (!m_bFullScreen){
		//	�����[�h�Ȃ̂�...
		if ((m_opt.size_x!=0) && (m_opt.size_y!=0)){
			::ShowWindow(m_hWnd, SW_SHOW);	//	�O�̂��߂ɑ���O�O�G
		}
		RECT r;
		InnerAdjustWindow(r,m_opt);
		return !::SetWindowPos(m_hWnd,NULL,0,0,r.right-r.left,r.bottom-r.top,SWP_NOMOVE|SWP_NOZORDER);
	} else {
		//	�t���X�N���[���Ȃ�΁A���T�C�Y�͔�T�|�[�g
	}

	return 0;
}

void	CWindow::UseMouseLayer(bool bUse){
	if (bUse==m_bUseMouseLayer) return ;
	m_bUseMouseLayer = bUse;
	if (bUse) {
		//	�g���Ȃ�Ζ������ɏ���
		ShowCursor(false);
	} else {
		//	�g��Ȃ��̂ł���΁A�E�B���h�D���[�h(or DIBDraw)�Ȃ�΃}�E�X�J�[�\������
		if (!IsFullScreen()
/*
#ifdef USE_DIB32
			|| CAppManager::GetMyDIBDraw()!=NULL
			|| CAppManager::GetMyFastDraw()!=NULL
#endif
*/
		){
			ShowCursor(true);
		}
	}
}

void	CWindow::ChangeWindowStyle() {
	//	���݂̉�ʃ��[�h�ƁA�E�B���h�E�X�^�C�������v����ΕύX�̕K�v�Ȃ�
	if (m_bFullScreen==g_bFullScreen && !m_bResized) return ;

	m_bFullScreen	= g_bFullScreen;
	m_bResized		= false;

	// �قȂ�ꍇ�̓E�B���h�D�X�^�C�������݂̃��[�h�ɍ��킹�ĕύX
	if (!m_bFullScreen) {
		RECT r;
		int	sx,sy;
		GetScreenSize(sx,sy);
		r.left	 = (sx-m_opt.size_x) >> 1;
		r.top	 = (sy-m_opt.size_y) >> 1;
		r.right	 = r.left + m_opt.size_x;
		r.bottom = r.top  + m_opt.size_y;
		sx = r.left;	sy = r.top;
//		SetWindowPos(r.left,r.top);
		InnerAdjustWindow(r,m_opt);
		::SetWindowPos(m_hWnd,HWND_TOP,sx,sy,r.right-r.left,r.bottom-r.top,SWP_NOZORDER);

		//	�����́ASetWindowLong() �Ō��݂̏��(�ő剻�A�ŏ����A�ʏ�)
		//	��ݒ肷��(��� '02/03/16)
		UINT nState = 0;// �E�C���h�E�̏��
		if(::IsIconic(m_hWnd))nState = WS_MINIMIZE;
		else if(::IsZoomed(m_hWnd))nState = WS_MAXIMIZE;
		::SetWindowLong(m_hWnd,GWL_STYLE,m_opt.style | nState );
		//	WS_CAPTION����('01/04/24)�@����ρA����̓��[�U�[���w�肵�Ȃ���_��

		::SetMenu(m_hWnd,GetMenu(m_hWnd)); // ���j���[�̈�̍ĕ`��
		::ShowWindow(m_hWnd,SW_SHOW);
		CIMEWrapper::GetObj()->Show();
	} else {
		::SetWindowLong(m_hWnd, GWL_STYLE, WS_POPUP);
		::SetMenu(m_hWnd,GetMenu(m_hWnd)); // ���j���[�̈�̍ĕ`��
		::ShowWindow(m_hWnd,SW_SHOW);
		CIMEWrapper::GetObj()->Hide();
	}
}

//	�E�B���h�D�T�C�Y��adjust����
//	Win32::AdjustWindow��bug-fix�p
//		(C) Tia-Deen & yaneurao '00/08/01,'00/11/05
void	CWindow::InnerAdjustWindow(RECT&rc,CWindowOption&opt){
	::SetRect(&rc,0,0,opt.size_x,opt.size_y);
	LONG	lStyle = opt.style;
/*	//	����ρA���̃R�[�h��user���w�肷�ׂ� '01/11/14
	if (lStyle & WS_POPUP) {
		lStyle |= WS_CAPTION;
	}
*/
	bool	bMenu = false;
	if (lStyle & WS_SYSMENU){
		//	m_hWnd��NULL�̏�Ԃ�GetMenu���Ăяo���͕̂s���H
		if (m_bUseMenu || (m_hWnd!=NULL && ::GetMenu(m_hWnd)!=NULL)){
			bMenu = true;
		} else {
			bMenu = false;
			lStyle &= ~WS_SYSMENU;	//	SYSMENU�t���O���O��
		}
	//	lStyle |= WS_CAPTION;	//	�������A�L���v�V�����̎w��͍s�Ȃ��i�������������߁j
	}
	::AdjustWindowRect(&rc,lStyle,bMenu);
}

///////////////////////////////////////////////////////////////////////////////

bool	CWindow::IsShowCursor(){
	//	�n�[�h�E�F�A�}�E�X�J�[�\���̕\���^��\�����擾
	return m_bShowCursor;
}

void	CWindow::ShowCursor(bool bShow){
	//	�����CreateWindow�����X���b�h���������Ȃ��Ă͂Ȃ�Ȃ�

	//	�\�t�g�E�F�A�}�E�X�J�[�\���Ȃ�΋����I�t
	if (m_bUseMouseLayer) bShow = false;

	//	�n�[�h�E�F�A�}�E�X�J�[�\���̕\���^��\��
	if (m_bShowCursor==bShow) return ;

	m_bShowCursor = bShow;
	::ShowCursor(bShow);
}

bool CWindow::IsWindowsClassName(const string& strClassName){
	return (strClassName=="BUTTON") || (strClassName=="COMBOBOX") ||
		(strClassName=="EDIT") || (strClassName=="LISTBOX") ||
		(strClassName=="MDICLIENT") || (strClassName=="SCROLLBAR") ||
		(strClassName=="STATIC");
}

///////////////////////////////////////////////////////////////////////////////
//
//	���b�Z�[�W�̃g���b�v
//

//	�ʏ��Window�̏ꍇ
LRESULT CALLBACK CWindow::gWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){

	CWindow* win = (CWindow*)::GetWindowLong(hWnd,GWL_USERDATA);	//	������CWindow*���B���Ă�����
	/**
		���ł����E�B���h�D�ɑ΂��đ����Ă������b�Z�[�W���^����dispatch���邱�Ƃ����肤��..
	*/
	if (win!=NULL) {
		return win->m_HookPtrList.Dispatch(hWnd,uMsg,wParam,lParam,win->m_pWndProc);
	} else {
		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
}

//	�_�C�A���O�̏ꍇ
LRESULT CALLBACK CWindow::gDlgProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){
	CWindow* win = (CWindow*)::GetWindowLong(hWnd,GWL_USERDATA);	//	������CWindow*���B���Ă�����
	if (win==NULL) {
		win = reinterpret_cast<CWindow*>(lParam);	//	���Ⴀ�������H(Create�����u�ԁH)
	}
	if (win!=NULL) {
		return win->m_HookPtrList.DispatchDlg(hWnd,uMsg,wParam,lParam,win->m_pWndProc);
	} else {
		//	�_�C�A���O�̏ꍇ�́ADefWindowProc���Ăяo���Ă͂����Ȃ�
		//	(�����I�ɏ�������悤�����_�C�A���Oprocedure��)
		return 0;
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////////
//	�ŏ������ꂽ���𔻒肵�A�ێ����邽�߂̃t�b�N
LRESULT	CWindow::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){

	switch(uMsg){
	case WM_ACTIVATEAPP: {
		//	Windows2000�Ȃ�ΕK�����ł��邪�A
		//	Win95/98��WM_SIZE��SIZE_RESTORED�������ŗ��Ȃ����Ƃ���H
		UINT bActive = wParam;
		if (bActive) {
			m_bMinimized = false;	//	�ʏ��Ԃł���
		}
		break;
						 }
	case WM_SIZE : {
		switch (wParam) {
		case SIZE_MINIMIZED : m_bMinimized = true; break;		//	�ŏ�������Ă���
		case SIZE_RESTORED	: m_bMinimized = false; break;		//	�ʏ��Ԃł���
		//		���ʏ��Ԃɖ߂������ǂ����́A������WM_ACTIVATEAPP�ƂŌ���ׂ�
		}
		break;
						}


/*
	http://www.microsoft.com/JAPAN/support/kb/articles/JP214/6/55.HTM
	���F

	[SDK32] �A�v���P�[�V�����I����A�^�X�N �o�[�ɋ󔒂̃{�b�N�X���c�� 
	�ŏI�X�V��: 2001/01/30
	�����ԍ�: JP214655	

����
�A�v���P�[�V�����̏I����A�^�X�N �o�[���X�V���ꂸ�A�󔒂̃{�b�N�X (�E�B���h�E�̃^�C�g����A�C�R�����\������Ă��Ȃ�����) ���c���ꂽ�܂܂ɂȂ邱�Ƃ�����܂��B���̋󔒂̃{�b�N�X���N���b�N����ƁA�����Ă��܂��܂��B 

����
���̖��́A�A�v���P�[�V�������^�X�N �o�[�ɑ΂���\���Ɋ֌W����E�B���h�E�̃X�^�C����ύX�������߂ɋN����܂��B 

�Ⴆ�΁A�E�B���h�E�� WS_EX_TOOLWINDOWS �X�^�C�����w�肵�āA���̊g���X�^�C����ύX���A���̃E�B���h�E���N���[�Y����O�Ɋg���X�^�C���̃��Z�b�g�Ɏ��s�����ꍇ�A�^�X�N �o�[�ɋ󔒂̃{�^�����\������܂��B 

�������@
���̓���ɑ��������ꍇ�A���̖�����������ɂ́A�E�B���h�E�̃X�^�C�������Z�b�g���邩�A�A�v���P�[�V�����̃��C�� �E�B���h�E�̏����ŁAWM_CLOSE �܂��� WM_DESTROY �̒��ŁASW_HIDE ���p�����^�ɂ��� ShowWindow ���Ăяo���܂��B 
*/

/*
	�ǋL�B
		�������́A�E�B���h�D�̏�Ԃ�W���ɖ߂����ɏI������ƁA����
		���ۂ��N����悤���B
		�܂�A
		�E�B���h�E�̏�Ԃ��ő剻����W���ɖ߂��Ƃ��̃A�j���[�V������
		���ꂵ������SW_HIDE�ŁAhide���āA���̂��ƁA�E�B���h�D�X�^�C����
		�m�[�}���ɖ߂��Ηǂ��B
*/
	case WM_DESTROY : {
		::ShowWindow(m_hWnd, SW_HIDE);

///		�������ƁA�f�b�h���b�N�Ɋׂ邱�Ƃ�����悤���D�D
//		::SetWindowLong(m_hWnd, GWL_STYLE, WS_POPUP);
		//	���ň��̃h���C�o�̃o�O�ɂ��Ȃ��āA
		//	����𔽉f�����邽�߂�(ShowWindow��SetWindowPos����K�v������)
//		::SetWindowPos(hWnd, (HWND)HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
/*
		��SetWindowPos(hWnd, (HWND)HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		���Ă����Ƃ��A��������Z�b�g���Ȃ��ƁA��͂薼�����E�B���h�D���^�X�N�o�[��Ɏc�邱�Ƃ�����i�o�O���̂Q�j
*/

//		::ShowWindow(m_hWnd, SW_HIDE);
		}
		break;
	//	��L�̖��ɑ΂���΍�R�[�h
	case WM_NCDESTROY : {
		//	�E�B���h�D����̂��ꂽ�̂ŁA���̃t�F�C�Y�ŁA�E�B���h�D�n���h����
		//	���Z�b�g���Ă��K�v������B�i�������Ȃ��ƁA���̃N���X��
		//	�f�X�g���N�^�ŁA��d��destroy����Ă��܂��j
		m_hWnd = NULL;
		break;
		}
	} // end switch

	return 0;
}

//////////////////////////////////////////////////////////////////////////////

void	CDialogHelper::hook(IWindow*pWin){
	if (pWin!=NULL){
		pWin->GetHookList()->Add(GetPooler());
	}
}

void	CDialogHelper::unhook(IWindow*pWin){
	if (pWin!=NULL){
		pWin->GetHookList()->Del(GetPooler());
	}
}

string	CDialogHelper::GetText(int nEditControlID){
	HWND hWnd = GetHWnd();
	if (hWnd==NULL) return "";
	smart_ptr<BYTE> pBuf;
	int nLength = 16;	//	16����������J�n
	while (true) {
		pBuf.AddArray(nLength);
		int nResultLength = ::GetDlgItemText(hWnd,nEditControlID,(LPTSTR)pBuf.get(),pBuf.size());
		if (nResultLength+1 == nLength){
			//	�o�b�t�@��������Ȃ��Ǝv����
			nLength <<=1;
		} else {
			break;
		}
	}
	return string((LPCSTR)pBuf.get());
}

///	�X�^�e�B�b�N�e�L�X�g�R���g���[��etc..�ɑ΂��āA�����Ƀe�L�X�g�������ݒ肷��
LRESULT	CDialogHelper::SetText(int nEditControlID,const string& str){
	HWND hWnd = GetHWnd();
	if (hWnd==NULL) return 1;
	return ::SetDlgItemText(hWnd,nEditControlID,str.c_str())?0:1;
}

LRESULT CDialogHelper::SetCheck(int nEditControlID,int nCheck){
	HWND hWnd = GetHWnd();
	if (hWnd==NULL) return 1;
	HWND hDlgItem = ::GetDlgItem(hWnd,nEditControlID);
	if (hDlgItem==NULL) return 2;
	switch (nCheck){
	case 0 : nCheck = BST_UNCHECKED; break;
	case 1 : nCheck = BST_CHECKED; break;
	case 2 : nCheck = BST_INDETERMINATE; break;
	}
	return ::SendMessage(hDlgItem , BM_SETCHECK, nCheck , 0L)?0:3;
}

///	�{�^����Ԃ̎擾
LRESULT	CDialogHelper::GetCheck(int nEditControlID){
/*
	-1:���̃{�^���͑��݂��Ȃ�
	0:�{�^���̃`�F�b�N�̓I�t�ɂȂ��Ă��܂��B 
	1:�{�^���̃`�F�b�N�̓I���ɂȂ��Ă��܂��B 
	2:�{�^���̓O���C�\���i�s�m��j�̏�Ԃł��B�{�^�����ABS_3STATE �X�^�C���܂��� BS_AUTO3STATE �X�^�C�������Ƃ��ɂ����K�p����܂��B 
*/
	HWND hWnd = GetHWnd();
	if (hWnd==NULL) return -1;
//	HWND hDlgItem = ::GetDlgItem(hWnd,nEditControlID);
//	if (hDlgItem==NULL) return -1;
	UINT n = ::IsDlgButtonChecked(hWnd,nEditControlID);
	switch(n){
	case BST_UNCHECKED : return 0; // �{�^���̃`�F�b�N�̓I�t�ɂȂ��Ă��܂��B
	case BST_CHECKED : return 1; //�{�^���̃`�F�b�N�̓I���ɂȂ��Ă��܂��B 
	case BST_INDETERMINATE : return 2; // �{�^���̓O���C�\���i�s�m��j�̏�Ԃł��B�{�^�����ABS_3STATE �X�^�C���܂��� BS_AUTO3STATE �X�^�C�������Ƃ��ɂ����K�p����܂��B 
	}
	return -1;	//???
}

///////////////////////////////////////////////////////////////////////////

void	CMsgDlg::Out(const string& caption,const string& message){
/*
	// �t���X�N���[���Ńt���b�v�g���Ă邩���m��Ȃ��̂�
#ifdef USE_DirectDraw
	CDirectDraw* lpDraw = CAppManager::GetMyDirectDraw();
	if (lpDraw!=NULL){
		lpDraw->FlipToGDISurface();
	}
#endif
	CWindow* lpWindow = CAppManager::GetMyApp()->GetMyWindow();
	bool bShowCursor;
	if (lpWindow!=NULL) {
		bShowCursor = lpWindow->IsShowCursor();
		lpWindow->ShowCursor(true);
	}
*/
	::MessageBox(CAppManager::GetHWnd(),message.c_str(),caption.c_str()
		,MB_OK|MB_SYSTEMMODAL|MB_SETFOREGROUND);
/*
	if (lpWindow!=NULL) {
		lpWindow->ShowCursor(bShowCursor);
	}
*/
}

} // end of namespace Window
} // end of namespace yaneuraoGameSDK3rd
