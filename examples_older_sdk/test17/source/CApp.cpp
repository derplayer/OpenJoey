#include "stdafx.h"
#include "CApp.h"
#include "Resource/Resource.h"

//////////////////////////////////////////////////////////////////////////////
//	���j���[����̎���

void	CMyMenu::SetDisplayMode( int nBpp )
{
	//	0�̓E�B���h�D���[�h
	//	16,24,32�͉��bpp
	
	WARNING( nBpp != 0 && nBpp != 16 && nBpp != 24 && nBpp != 32,
			 "CMyMenu::SetDisplayMode�́A�͈͊O�̃f�B�X�v���C���[�h�ł�" );

//	��ʃ��[�h�ݒ�
	bool bFullScr = false;
	if ( nBpp != 0 )
		bFullScr = true;

	//	���S��������BeginChangeDisplay�`EndChangeDisplay�ŕύX����
	{
		GetApp()->GetDraw()->BeginChangeDisplay();
		switch (nBpp) {
		case 0: goto WIN_MODE;
		case 16: goto FULL_16;
		case 24: goto FULL_24;
		case 32: goto FULL_32;
		default: WARNING(true,"���T�|�[�g�̉�ʃ��[�h");
		}
		FULL_16: GetApp()->GetDraw()->TestDisplayMode( 640,480, true,16 );
		FULL_24: GetApp()->GetDraw()->TestDisplayMode( 640,480, true,24 );
		FULL_32: GetApp()->GetDraw()->TestDisplayMode( 640,480, true,32 );
		WIN_MODE: GetApp()->GetDraw()->TestDisplayMode( 640,480 );
		GetApp()->GetDraw()->EndChangeDisplay();
	}

// �ݒ�ɂ��X�V (�ݒ�Ɏ��s�����ꍇ�AGet����bpp��z��ɓn��)
	bFullScr = GetApp()->GetDraw()->IsFullScreen();
	nBpp	 = GetApp()->GetDraw()->GetBpp();
	if ( !bFullScr ) { nBpp = 0; }
	*GetApp()->GetDisplayMode() = nBpp;							//	�V�X�e���z����X�V

// �w�肵���f�B�X�v���C���[�h�̃��j���[��̃`�F�b�N
	CheckDisplay( nBpp );
} // SetDisplayMode


//////////////////////////////////////////
//	CheckDisplay
//	�f�B�X�v���C���[�h�ɑ΂���`�F�b�N
//////////////////////////////////////////
void	CMyMenu::CheckDisplay( int nDisplay )
{
	HMENU	hMenu;
	int		nID[4] = { IDM_WinMode, IDM_HighColor, IDM_FullColor24, IDM_FullColor32	};
	int		nNo[4] = { 0,16,24,32 };
	
	for ( int i = 0 ; i < 4 ; i++ )
		if ( nNo[i] == nDisplay )
			nDisplay = i;

	hMenu = ::GetMenu( CAppInitializer::GetHWnd() );
	for ( i = 0 ; i < 4 ; i++ )
	{
		if ( i == nDisplay )
			::CheckMenuItem( hMenu, nID[i], MF_BYCOMMAND | MFS_CHECKED );
		else
			::CheckMenuItem( hMenu, nID[i], MF_BYCOMMAND | MFS_UNCHECKED );
	}
}// CheckDisplay

//////////////////////////////////////////
//	WndProc
// ���b�Z�[�W�̃R�[���o�b�N
//////////////////////////////////////////
LRESULT CMyMenu::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		// ���j���[�̏���
		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				// �I����������
				case IDM_END:
					if ( ::MessageBox( CAppInitializer::GetHWnd(), "�ق��鑑���I�����܂����H", "�I���m�F",
									   MB_YESNO | MB_DEFBUTTON2 | MB_TOPMOST ) == IDYES )
					{
						GetApp()->InvalidateThread();			//	���C�����[�v���~�߂�
					}
					break;
				// �E�B���h�E���[�h�ɂ��鎞
				case IDM_WinMode:
					SetDisplayMode( 0 );
					break;
				// �t���X�N���[��16bit�ɂ��鎞
				case IDM_HighColor:
					SetDisplayMode( 16 );
					break;
				// �t���X�N���[��24bit�ɂ��鎞
				case IDM_FullColor24:
					SetDisplayMode( 24 );
					break;
				// �t���X�N���[��32bit�ɂ��鎞
				case IDM_FullColor32:
					SetDisplayMode( 32 );
					break;
				// �o�[�W����
				case IDM_Version:
					LPSTR szVersion = "�@�@�g���������@�ق��鑑�@�@�@�@�@\n\n�@�@�u������������ �P�D�O�D�O�@�@�@\n\n\n�@�@�@�@�@�@�@�@�@�@�i�b�j�v�`�e�e�k�d�@�@";
					::MessageBox( CAppInitializer::GetHWnd(), szVersion, "�o�[�W����", MB_OK | MB_TOPMOST );
					break;
			}
			break;
	}

	return 0;
} // WndProc

//////////////////////////////////////////////////////////////////////////////

//	���C���X���b�h
void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����
	GetDraw()->SetDisplay(false);

	*GetMyApp()->GetWaitIfMinimized()=true;		//	�ŏ�������Ă���Ƃ��́A�X���b�h���~����

	CDIB32 bg;
	bg.Load("data/bg.jpg");

	CFPSTimer fps;
	CKey key;

	*GetMyApp()->GetWaitIfMinimized() = true;

	while (IsThreadValid()){
		key.Input();
		if (key.IsVKeyPress(0)) break;	//	ESC�L�[�ŏI�������
		GetDraw()->Blt(&bg,0,0);
		GetDraw()->OnDraw();
		fps.WaitFrame();
	}
}

CApp::CApp(void) : m_Menu(this) {}	//	�����warning�o���Ȃ�[ > VC++

//	���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
	virtual void MainThread(void){			//	���ꂪ���[�J�[�X���b�h
		CApp().Start();						//	CApp app; app.Start();�̈Ӗ���
	}
	//	�L���v�V�����ύX���Ƃ���Ƃǂ₳���O�O
	virtual LRESULT OnPreCreate(CWindowOption& opt){
		opt.caption		= "Happy �ق��鑑";
		opt.classname	= "YANESDKAPPLICATION";
		opt.size_x		= 640;
		opt.size_y		= 480;
		opt.style		= WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION;
		return 0;
	}
};

//	���킸�ƒm�ꂽWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CAppInitializer::Init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	�K�������Ă�
	CSingleApp sapp;
	if (sapp.IsValid()) {
		CAppMainWindow().Run();					//	��Œ�`�������C���̃E�B���h�D���쐬
	}
	return 0;
}
