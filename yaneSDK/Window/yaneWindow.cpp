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

bool CWindow::g_bFullScreen		= false; // 起動直後はウィンドゥモードでしょ！
CCriticalSection CWindow::m_cs;

//////////////////////////////////////////////////////////////////////////////

CWindow::CWindow(){
	m_dwFillColor	=	0;			//	起動時黒がディフォルト
	m_hWnd			=	NULL;
	m_bShowCursor	=	true;		//	ディフォルトでマウスカーソルを表示
	m_bUseMouseLayer=	false;		//	ソフトウェアマウスカーソル
	m_bUseMenu		=	false;		//	メニューのあるやなしやはわからん＾＾；
	m_bMinimized	=	false;		//	最小化されているか？
	m_bResized		=	false;		//	ChangeWindowStyle⇔SetSizeのフラグ。
	m_pWndProc		=	NULL;		//	hookしたWndProc
}

CWindow::~CWindow(){
	GetHookList()->Del(this);		//	直接フックを解除する
/*
	if (m_hWnd!=NULL) {
		::DestroyWindow(m_hWnd);
	//	↑こんな強引な方法でDestroyするのは、お勧めしかねる＾＾；
	//	本来、CAppBaseから使うのならば、このデストラクタでは、すでに
	//	Windowは解体されたあとのはずだが...
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
	m_bFullScreen = g_bFullScreen;	//	現在の画面モードを内部的に保持
	m_opt	= opt;	//	コピーしておく

	m_hWnd = NULL;

	LONG lChild = 0;

	//	また気が向いたらサポート
	if (m_opt.dialog!=NULL){
	//	dialog作るらしいで？
		m_hWnd = ::CreateDialogParam(hInst,m_opt.dialog, hParent, (DLGPROC)gDlgProc,
			reinterpret_cast<LPARAM>(this));
			//	↑thisがLPARAMに入ってやってくる！
		goto window_setting;
	}

    // まだ登録されていないウィンドウクラス名で、かつWindows標準クラス
    // 以外の名前の場合はRegisterClassEx()してやる
    WNDCLASSEX wndclass;
    if( !::GetClassInfoEx(hInst, opt.classname.c_str(), &wndclass) &&
        !IsWindowsClassName(opt.classname)) {

        // おきまりのウインドウクラス生成処理
		wndclass.cbSize		= sizeof(WNDCLASSEX);
		wndclass.style		= 0;	//	ダブルクリック感知するなら→CS_DBLCLKS;
		wndclass.lpfnWndProc= gWndProc;
		wndclass.cbClsExtra	= 0;
		wndclass.cbWndExtra	= 0;
		wndclass.hInstance	= hInst;

		// とりあえず、"MAIN"のアイコン表示
		wndclass.hIcon		= LoadIcon(hInst,"MAIN");
		wndclass.hIconSm	= LoadIcon(hInst,"MAIN");

		wndclass.hCursor	= LoadCursor(NULL,IDC_ARROW);
		// とりあえず、マウスカーソルも用意（不要ならば、あとで消すべし！）

		//	とりあえず、起動直後は、白か黒にしとこっと…
		if (m_dwFillColor==0) {
			wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		} else {
			wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		}

		//	MENUも仮で読み込んでみる
		wndclass.lpszMenuName  = "IDR_MENU";
		//	このリソースが本当に存在するのかは、実行時までわからないので
		//	正確なウィンドゥのアドジャストがウィンドゥ生成後でしか出来ない

		{
			HRSRC hrsrc;
			hrsrc = ::FindResource(NULL,"IDR_MENU",RT_MENU);
			if (hrsrc!=NULL) { m_bUseMenu = true; }	//	メニューありやがんで＾＾；
		}

		wndclass.lpszClassName = opt.classname.c_str();

		// なんで失敗してんだろね？
		if (!::RegisterClassEx(&wndclass)) {
			Err.Out("CWindow::CreateでRegisterClassEx失敗");
			return 1;
		}
	}

	if (hParent!=NULL) {
		lChild	= WS_CHILDWINDOW;
	}

	if (m_bFullScreen){
		//	ウインドウの生成（フルスクリーン時）
		//	このときは、画面のスタイルオプションは無視する

		int sx,sy;
		GetScreenSize(sx,sy);
		m_hWnd = ::CreateWindow(opt.classname.c_str(),opt.caption.c_str(),
						WS_POPUP|WS_VISIBLE	 /* |WS_SYSMENU|
						WS_MAXIMIZEBOX|WS_MINIMIZEBOX */ ,
						0, 0, sx, sy,
						hParent, NULL, hInst, NULL );
	} else {
		// ウィンドゥ境界を考慮に入れなくてはならない
		// 表示範囲が640*480ならば、生成すべきWindowサイズは、それ以上である
		RECT r;
		InnerAdjustWindow(r,opt);
		m_hWnd = ::CreateWindow(opt.classname.c_str(),opt.caption.c_str(),
				// WS_EX_TOPMOSTを指定したいんだけどなー
						lChild | opt.style,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						r.right-r.left,r.bottom-r.top,
						hParent,NULL,hInst,NULL);
	}

window_setting:;
	if (m_hWnd==NULL){
		Err.Out("CWindow::CreateでCreateWindowに失敗。");
		return 1;
	}

	//	コールバックされたときに、それぞれのWindowClassにdispatch出来るように
	//	GWL_USERDATAにthisを隠しておく。
	::SetWindowLong(m_hWnd,GWL_USERDATA,(LONG)this);

	//	画面のセンターに生成するように修正＾＾
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

	//	標準のWindowClassならば、コールバック関数として、これを
	//	書き込んでやる。
	if (IsWindowsClassName(opt.classname)){
		m_pWndProc = (WNDPROC)::SetWindowLong(m_hWnd,GWL_WNDPROC,(LONG)gWndProc);
		//	こういうhookは、illegalか...
	} else {
		m_pWndProc = NULL;
	}

	//	自分のウィンドゥなので直接フックできる
	//	（というかCAppManagerに未登録の段階なので直接フックしかない）
	GetHookList()->Add(this);			//	フックを開始する

	return 0;
}

void CWindow::GetScreenSize(int &x,int &y){
	x = ::GetSystemMetrics(SM_CXSCREEN);
	y = ::GetSystemMetrics(SM_CYSCREEN);
}

LRESULT CWindow::SetWindowPos(int x,int y){
	if (m_hWnd==NULL) return 0;
	//	ウィンドウが生成されていたら移動させなくてはならない（ただしウィンドゥモード時）
	if (!m_bFullScreen){
		//	ここでトップオーダーにしておいても問題ないのだろう...
		return !::SetWindowPos(m_hWnd,HWND_TOP,x,y,NULL,NULL,SWP_NOSIZE|SWP_NOZORDER);
	}
	return 0;
}

void CWindow::SetSize(int sx,int sy){
	if (m_opt.size_x != sx || m_opt.size_y != sy){
		m_opt.size_x = sx;
		m_opt.size_y = sy;
		m_bResized	= true;	//	変更されたでフラグ＾＾；
	}
}

LRESULT CWindow::Resize(int sx,int sy){
	m_opt.size_x = sx;
	m_opt.size_y = sy;

	if (m_hWnd==NULL) return 0;

	if (!m_bFullScreen){
		//	窓モードなので...
		if ((m_opt.size_x!=0) && (m_opt.size_y!=0)){
			::ShowWindow(m_hWnd, SW_SHOW);	//	念のために送る＾＾；
		}
		RECT r;
		InnerAdjustWindow(r,m_opt);
		return !::SetWindowPos(m_hWnd,NULL,0,0,r.right-r.left,r.bottom-r.top,SWP_NOMOVE|SWP_NOZORDER);
	} else {
		//	フルスクリーンならば、リサイズは非サポート
	}

	return 0;
}

void	CWindow::UseMouseLayer(bool bUse){
	if (bUse==m_bUseMouseLayer) return ;
	m_bUseMouseLayer = bUse;
	if (bUse) {
		//	使うならば無条件に消す
		ShowCursor(false);
	} else {
		//	使わないのであれば、ウィンドゥモード(or DIBDraw)ならばマウスカーソル復活
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
	//	現在の画面モードと、ウィンドウスタイルが合致すれば変更の必要なし
	if (m_bFullScreen==g_bFullScreen && !m_bResized) return ;

	m_bFullScreen	= g_bFullScreen;
	m_bResized		= false;

	// 異なる場合はウィンドゥスタイルを現在のモードに合わせて変更
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

		//	ここの、SetWindowLong() で現在の状態(最大化、最小化、通常)
		//	を設定する(れむ '02/03/16)
		UINT nState = 0;// ウインドウの状態
		if(::IsIconic(m_hWnd))nState = WS_MINIMIZE;
		else if(::IsZoomed(m_hWnd))nState = WS_MAXIMIZE;
		::SetWindowLong(m_hWnd,GWL_STYLE,m_opt.style | nState );
		//	WS_CAPTION消す('01/04/24)　やっぱ、これはユーザーが指定しなきゃダメ

		::SetMenu(m_hWnd,GetMenu(m_hWnd)); // メニュー領域の再描画
		::ShowWindow(m_hWnd,SW_SHOW);
		CIMEWrapper::GetObj()->Show();
	} else {
		::SetWindowLong(m_hWnd, GWL_STYLE, WS_POPUP);
		::SetMenu(m_hWnd,GetMenu(m_hWnd)); // メニュー領域の再描画
		::ShowWindow(m_hWnd,SW_SHOW);
		CIMEWrapper::GetObj()->Hide();
	}
}

//	ウィンドゥサイズのadjust処理
//	Win32::AdjustWindowのbug-fix用
//		(C) Tia-Deen & yaneurao '00/08/01,'00/11/05
void	CWindow::InnerAdjustWindow(RECT&rc,CWindowOption&opt){
	::SetRect(&rc,0,0,opt.size_x,opt.size_y);
	LONG	lStyle = opt.style;
/*	//	やっぱ、このコードはuserが指定すべき '01/11/14
	if (lStyle & WS_POPUP) {
		lStyle |= WS_CAPTION;
	}
*/
	bool	bMenu = false;
	if (lStyle & WS_SYSMENU){
		//	m_hWndがNULLの状態でGetMenuを呼び出すのは不正？
		if (m_bUseMenu || (m_hWnd!=NULL && ::GetMenu(m_hWnd)!=NULL)){
			bMenu = true;
		} else {
			bMenu = false;
			lStyle &= ~WS_SYSMENU;	//	SYSMENUフラグを外す
		}
	//	lStyle |= WS_CAPTION;	//	しかし、キャプションの指定は行なう（高さが狂うため）
	}
	::AdjustWindowRect(&rc,lStyle,bMenu);
}

///////////////////////////////////////////////////////////////////////////////

bool	CWindow::IsShowCursor(){
	//	ハードウェアマウスカーソルの表示／非表示を取得
	return m_bShowCursor;
}

void	CWindow::ShowCursor(bool bShow){
	//	これはCreateWindowしたスレッドが処理しなくてはならない

	//	ソフトウェアマウスカーソルならば強制オフ
	if (m_bUseMouseLayer) bShow = false;

	//	ハードウェアマウスカーソルの表示／非表示
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
//	メッセージのトラップ
//

//	通常のWindowの場合
LRESULT CALLBACK CWindow::gWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){

	CWindow* win = (CWindow*)::GetWindowLong(hWnd,GWL_USERDATA);	//	ここにCWindow*を隠しておいた
	/**
		消滅したウィンドゥに対して送られてきたメッセージを運悪くdispatchすることがありうる..
	*/
	if (win!=NULL) {
		return win->m_HookPtrList.Dispatch(hWnd,uMsg,wParam,lParam,win->m_pWndProc);
	} else {
		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
}

//	ダイアログの場合
LRESULT CALLBACK CWindow::gDlgProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){
	CWindow* win = (CWindow*)::GetWindowLong(hWnd,GWL_USERDATA);	//	ここにCWindow*を隠しておいた
	if (win==NULL) {
		win = reinterpret_cast<CWindow*>(lParam);	//	じゃあここか？(Createした瞬間？)
	}
	if (win!=NULL) {
		return win->m_HookPtrList.DispatchDlg(hWnd,uMsg,wParam,lParam,win->m_pWndProc);
	} else {
		//	ダイアログの場合は、DefWindowProcを呼び出してはいけない
		//	(内部的に処理するようだ＞ダイアログprocedureが)
		return 0;
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////////
//	最小化されたかを判定し、保持するためのフック
LRESULT	CWindow::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){

	switch(uMsg){
	case WM_ACTIVATEAPP: {
		//	Windows2000ならば必ず飛んでくるが、
		//	Win95/98はWM_SIZEのSIZE_RESTOREDしか飛んで来ないことあり？
		UINT bActive = wParam;
		if (bActive) {
			m_bMinimized = false;	//	通常状態である
		}
		break;
						 }
	case WM_SIZE : {
		switch (wParam) {
		case SIZE_MINIMIZED : m_bMinimized = true; break;		//	最小化されている
		case SIZE_RESTORED	: m_bMinimized = false; break;		//	通常状態である
		//		↑通常状態に戻したかどうかは、こことWM_ACTIVATEAPPとで見るべき
		}
		break;
						}


/*
	http://www.microsoft.com/JAPAN/support/kb/articles/JP214/6/55.HTM
	より：

	[SDK32] アプリケーション終了後、タスク バーに空白のボックスが残る 
	最終更新日: 2001/01/30
	文書番号: JP214655	

現象
アプリケーションの終了後、タスク バーが更新されず、空白のボックス (ウィンドウのタイトルやアイコンが表示されていないもの) が残されたままになることがあります。その空白のボックスをクリックすると、消えてしまいます。 

原因
この問題は、アプリケーションがタスク バーに対する表示に関係するウィンドウのスタイルを変更したために起こります。 

例えば、ウィンドウが WS_EX_TOOLWINDOWS スタイルを指定して、その拡張スタイルを変更し、そのウィンドウをクローズする前に拡張スタイルのリセットに失敗した場合、タスク バーに空白のボタンが表示されます。 

解決方法
この動作に遭遇した場合、この問題を解決するには、ウィンドウのスタイルをリセットするか、アプリケーションのメイン ウィンドウの処理で、WM_CLOSE または WM_DESTROY の中で、SW_HIDE をパラメタにして ShowWindow を呼び出します。 
*/

/*
	追記。
		正しくは、ウィンドゥの状態を標準に戻さずに終了すると、この
		現象が起きるようだ。
		つまり、
		ウィンドウの状態を最大化から標準に戻すときのアニメーションが
		見苦しいためSW_HIDEで、hideして、そのあと、ウィンドゥスタイルを
		ノーマルに戻せば良い。
*/
	case WM_DESTROY : {
		::ShowWindow(m_hWnd, SW_HIDE);

///		↓をやると、デッドロックに陥ることがあるようだ．．
//		::SetWindowLong(m_hWnd, GWL_STYLE, WS_POPUP);
		//	↑最悪のドライバのバグにそなえて、
		//	これを反映させるために(ShowWindowかSetWindowPosする必要がある)
//		::SetWindowPos(hWnd, (HWND)HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
/*
		↑SetWindowPos(hWnd, (HWND)HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		していたとき、これをリセットしないと、やはり名無しウィンドゥがタスクバー上に残ることがある（バグその２）
*/

//		::ShowWindow(m_hWnd, SW_HIDE);
		}
		break;
	//	上記の問題に対する対策コード
	case WM_NCDESTROY : {
		//	ウィンドゥが解体されたので、このフェイズで、ウィンドゥハンドルを
		//	リセットしてやる必要がある。（そうしないと、このクラスの
		//	デストラクタで、二重にdestroyされてしまう）
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
	int nLength = 16;	//	16文字分から開始
	while (true) {
		pBuf.AddArray(nLength);
		int nResultLength = ::GetDlgItemText(hWnd,nEditControlID,(LPTSTR)pBuf.get(),pBuf.size());
		if (nResultLength+1 == nLength){
			//	バッファ長が足りないと思われる
			nLength <<=1;
		} else {
			break;
		}
	}
	return string((LPCSTR)pBuf.get());
}

///	スタティックテキストコントロールetc..に対して、そこにテキスト文字列を設定する
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

///	ボタン状態の取得
LRESULT	CDialogHelper::GetCheck(int nEditControlID){
/*
	-1:そのボタンは存在しない
	0:ボタンのチェックはオフになっています。 
	1:ボタンのチェックはオンになっています。 
	2:ボタンはグレイ表示（不確定）の状態です。ボタンが、BS_3STATE スタイルまたは BS_AUTO3STATE スタイルを持つときにだけ適用されます。 
*/
	HWND hWnd = GetHWnd();
	if (hWnd==NULL) return -1;
//	HWND hDlgItem = ::GetDlgItem(hWnd,nEditControlID);
//	if (hDlgItem==NULL) return -1;
	UINT n = ::IsDlgButtonChecked(hWnd,nEditControlID);
	switch(n){
	case BST_UNCHECKED : return 0; // ボタンのチェックはオフになっています。
	case BST_CHECKED : return 1; //ボタンのチェックはオンになっています。 
	case BST_INDETERMINATE : return 2; // ボタンはグレイ表示（不確定）の状態です。ボタンが、BS_3STATE スタイルまたは BS_AUTO3STATE スタイルを持つときにだけ適用されます。 
	}
	return -1;	//???
}

///////////////////////////////////////////////////////////////////////////

void	CMsgDlg::Out(const string& caption,const string& message){
/*
	// フルスクリーンでフリップ使ってるかも知れないので
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
