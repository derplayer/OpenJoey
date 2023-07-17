#include "stdafx.h"
#include "CApp.h"
#include "Resource/Resource.h"

//////////////////////////////////////////////////////////////////////////////
//	メニュー判定の実装

void	CMyMenu::SetDisplayMode( int nBpp )
{
	//	0はウィンドゥモード
	//	16,24,32は画面bpp
	
	WARNING( nBpp != 0 && nBpp != 16 && nBpp != 24 && nBpp != 32,
			 "CMyMenu::SetDisplayModeは、範囲外のディスプレイモードです" );

//	画面モード設定
	bool bFullScr = false;
	if ( nBpp != 0 )
		bFullScr = true;

	//	万全を期してBeginChangeDisplay～EndChangeDisplayで変更する
	{
		GetApp()->GetDraw()->BeginChangeDisplay();
		switch (nBpp) {
		case 0: goto WIN_MODE;
		case 16: goto FULL_16;
		case 24: goto FULL_24;
		case 32: goto FULL_32;
		default: WARNING(true,"未サポートの画面モード");
		}
		FULL_16: GetApp()->GetDraw()->TestDisplayMode( 640,480, true,16 );
		FULL_24: GetApp()->GetDraw()->TestDisplayMode( 640,480, true,24 );
		FULL_32: GetApp()->GetDraw()->TestDisplayMode( 640,480, true,32 );
		WIN_MODE: GetApp()->GetDraw()->TestDisplayMode( 640,480 );
		GetApp()->GetDraw()->EndChangeDisplay();
	}

// 設定による更新 (設定に失敗した場合、Getしたbppを配列に渡す)
	bFullScr = GetApp()->GetDraw()->IsFullScreen();
	nBpp	 = GetApp()->GetDraw()->GetBpp();
	if ( !bFullScr ) { nBpp = 0; }
	*GetApp()->GetDisplayMode() = nBpp;							//	システム配列を更新

// 指定したディスプレイモードのメニュー上のチェック
	CheckDisplay( nBpp );
} // SetDisplayMode


//////////////////////////////////////////
//	CheckDisplay
//	ディスプレイモードに対するチェック
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
// メッセージのコールバック
//////////////////////////////////////////
LRESULT CMyMenu::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		// メニューの処理
		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				// 終了だった時
				case IDM_END:
					if ( ::MessageBox( CAppInitializer::GetHWnd(), "ほたる荘を終了しますか？", "終了確認",
									   MB_YESNO | MB_DEFBUTTON2 | MB_TOPMOST ) == IDYES )
					{
						GetApp()->InvalidateThread();			//	メインループを止める
					}
					break;
				// ウィンドウモードにする時
				case IDM_WinMode:
					SetDisplayMode( 0 );
					break;
				// フルスクリーン16bitにする時
				case IDM_HighColor:
					SetDisplayMode( 16 );
					break;
				// フルスクリーン24bitにする時
				case IDM_FullColor24:
					SetDisplayMode( 24 );
					break;
				// フルスクリーン32bitにする時
				case IDM_FullColor32:
					SetDisplayMode( 32 );
					break;
				// バージョン
				case IDM_Version:
					LPSTR szVersion = "　　Ｈａｐｐｙ　ほたる荘　　　　　\n\n　　Ｖｅｒｓｉｏｎ １．０．０　　　\n\n\n　　　　　　　　　　（Ｃ）ＷＡＦＦＬＥ　　";
					::MessageBox( CAppInitializer::GetHWnd(), szVersion, "バージョン", MB_OK | MB_TOPMOST );
					break;
			}
			break;
	}

	return 0;
} // WndProc

//////////////////////////////////////////////////////////////////////////////

//	メインスレッド
void	CApp::MainThread(void) {				 //	 これが実行される
	GetDraw()->SetDisplay(false);

	*GetMyApp()->GetWaitIfMinimized()=true;		//	最小化されているときは、スレッドを停止する

	CDIB32 bg;
	bg.Load("data/bg.jpg");

	CFPSTimer fps;
	CKey key;

	*GetMyApp()->GetWaitIfMinimized() = true;

	while (IsThreadValid()){
		key.Input();
		if (key.IsVKeyPress(0)) break;	//	ESCキーで終了するよ
		GetDraw()->Blt(&bg,0,0);
		GetDraw()->OnDraw();
		fps.WaitFrame();
	}
}

CApp::CApp(void) : m_Menu(this) {}	//	これでwarning出すなよー > VC++

//	これがmain windowのためのクラス。
class CAppMainWindow : public CAppBase {	//	アプリケーションクラスから派生
	virtual void MainThread(void){			//	これがワーカースレッド
		CApp().Start();						//	CApp app; app.Start();の意味ね
	}
	//	キャプション変更しとかんとどやされる＾＾
	virtual LRESULT OnPreCreate(CWindowOption& opt){
		opt.caption		= "Happy ほたる荘";
		opt.classname	= "YANESDKAPPLICATION";
		opt.size_x		= 640;
		opt.size_y		= 480;
		opt.style		= WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION;
		return 0;
	}
};

//	言わずと知れたWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CAppInitializer::Init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	必ず書いてね
	CSingleApp sapp;
	if (sapp.IsValid()) {
		CAppMainWindow().Run();					//	上で定義したメインのウィンドゥを作成
	}
	return 0;
}
