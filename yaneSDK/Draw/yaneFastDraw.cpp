//
//	DirectDrawSurface and MemorySurface Wrapper
//

#include "stdafx.h"

#ifdef USE_FastDraw

#include "yaneDirectDraw.h"
#include "yaneFastDraw.h"
#include "yaneFastPlane.h"
#include "yaneGTL.h"
#include "../Window/yaneWindow.h"
#include "../AppFrame/yaneAppManager.h"	//	GetHWnd
#include "../AppFrame/yaneAppBase.h"

//////////////////////////////////////////////////////////////////////////////
//	CFastDraw : 画面解像度を切り替えるなど
//////////////////////////////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Draw {

CFastDraw::CFastDraw() :
	m_DirectDrawBase(true),	//	DirectDraw emulation only
	//	何と、DirectDrawはEmulation Onlyのほうが速い！	
	m_Primary(GetThis()),
	m_Secondary256(GetThis()),
	m_Secondary(GetThis())
{
	CFastPlane::SetDefaultFastDraw(this);

//	m_nScreenXSize	= 640;
//	m_nScreenYSize	= 480;
	//	これは、生成されているウィンドゥサイズに準ずるほうがいいんちがう？
	IWindow* pWin = CAppManager::GetMyWindow();
	CWindowOption* opt;
	if (pWin!=NULL){
		opt = pWin->GetWindowOption();
	} else {
		opt = NULL;
	}
	if (opt==NULL){
		m_nScreenXSize	= 640;
		m_nScreenYSize	= 480;
	} else {
		m_nScreenXSize = opt->size_x;
		m_nScreenYSize = opt->size_y;
	}

	m_nScreenColor	= 16;
	m_bFullScr		= false;

	//	FullScr時のフリップの使用。使えるものならば使ったほうが良い
//	m_bUseFlip		= false;

	{
	RECT rc,rc2;
	::SetRect(&rc,0,0,0,0);
	::SetRect(&rc2,0,0,0,0);
	::AdjustWindowRect(&rc,WS_CAPTION | WS_SYSMENU,true);
	::AdjustWindowRect(&rc2,WS_CAPTION ,false);
	//	フルスクリーン時のメニューサイズを正確に算出するため、メニュー高さを調べる(c)yaneurao'00/12/15
	//	（なんとメニュー高さはＯＳごとに異なる＾＾； ）
	m_nMenu = (rc.bottom-rc.top) - (rc2.bottom-rc2.top);
	}

	m_nSecondaryOffsetX = 0;
	m_nSecondaryOffsetY = 0;

	m_nBrightness	= 256;
	m_bLostSurface	= false; 

	m_hWnd	= CAppManager::GetHWnd();
	CAppManager::Add(this);
	CAppManager::Hook(this);		//	フックを開始する

	//	プライマリはハードウェアメモリ上に確保
	GetPrimary()->SetSystemMemoryUse(false);
//	GetPrimary2()->SetSystemMemoryUse(false);
	//	そこ以外は、すべてシステムメモリ上に確保する
	//	CFastPlaneは、ディフォルトでシステムメモリなので、これでＯＫ

	//	セカンダリに関しては、自動リストアサーフェースとしておく
	//	（どうせ、OnDrawはオーバーライドされていないので）
//	GetPrimary2()->SetAutoRestore(true);
	GetSecondary()->SetAutoRestore(true);

	// yaneGTL.hで使用する、ブレンドテーブルを初期化
	CFastPlaneBlendTable::InitTable();	
}

CFastDraw::~CFastDraw(){
	CAppManager::Unhook(this);		//	フックを解除する。
	CAppManager::Del(this);
}

//////////////////////////////////////////////////////////////////////////////

int		CFastDraw::GetMenuHeight(){
//	メニューがついているならば、そのメニュー高さを返す
//	（ついていなければ０）
	if (::GetMenu(m_hWnd)!=NULL) return m_nMenu;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

void CFastDraw::BeginChangeDisplay(){
	GetCriticalSection()->Enter();	//	排他ロックしておこう

	m_bDisplayChanging = true;
	m_bChangeDisplayMode = false;
	if (GetDDraw()==NULL) return ;

	// FullScreenModeでFlip用にアタッチしてたら、解放する必要なしだが...
	//	セカンダリ、クリッパ、プライマリの解放
 
	m_Secondary256.Release();
//	m_Secondary.Release();
	//	↑このセカンダリは、システムメモリ上に作成された
	//	仮想セカンダリサーフェースなので解放する必要が無い
	//	（むしろ、解放しないことによって、AutoRestoreの対象としたい）

	m_WindowClipper.Release();
//	m_Primary2.Release();
	m_Primary.Release();

	if (m_bFullScr) {	//	もしフルスクリーンモードならばそれを戻す必要がある
		if (GetDDraw()->SetCooperativeLevel(m_hWnd,DDSCL_NORMAL)){
			Err.Out("CFastDraw::BeginChangeDisplayでSetCooperativeLevelに失敗..");
		}
		// ウィンドゥモードにするときは、これを行なう必要あり
		if (GetDDraw()->RestoreDisplayMode()){
			Err.Out("CFastDraw::BeginChangeDisplayでRestoreDisplayModeに失敗..");
		}
	}
}

void CFastDraw::TestDisplayMode(int nSX,int nSY,bool bFullScr,int nColor){
	if (!m_bDisplayChanging) return ; // already changing!!
	if (GetDDraw()==NULL) return ;

	// ウインドウモードでディスプレイモードを変更しないか
	m_bChangeDisplayMode |= (m_bFullScr!=bFullScr) || m_bFullScr;// 追加
	if (m_bChangeDisplayMode) CAppManager::GetMyWindow()->SetResized(true);

	//	画面モードを反映させて...
	m_bFullScr	= bFullScr;

	if (!bFullScr) {
		// Windowモードの指定があるならば仕方がない。
		if (GetDDraw()->SetCooperativeLevel(m_hWnd,DDSCL_NORMAL)){
			Err.Out("CFastDraw::TestDisplayModeのSetCooperativeLevelにDDSC_NORMALで失敗");
			return ; // これであかんかったらはっきり言ってシャレならんのやけど...
		}
	} else {
		if (GetDDraw()->SetCooperativeLevel(m_hWnd,DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN)!=DD_OK){
			Err.Out("CFastDraw::TestDisplayModeのSetCooperativeLevelにDDSC_EXCLUSIVEで失敗");
			return ; // 他のアプリ使ってんちゃうん？
		}
		if (GetDDraw()->SetDisplayMode(nSX,nSY,nColor)!=DD_OK){
			Err.Out("CFastDraw::TestDisplayModeのSetDisplayModeに失敗");
			return ; // 非対応け？
		}
	}

	//	m_bUseFlip	: フリップを使用するのか？（ユーザーの意思	）
	//	m_bUseFlip2 : フリップが実際にハードウェア的に使えて、
	//				そして使うときにtrue

//	m_bUseFlip2 = m_bUseFlip;

	bool bUseFilp = false;
	//	プライマリサーフェースの作成
	if (m_Primary.CreatePrimary(bUseFilp,nSX,nSY)!=0) {
		Err.Out("CFastDraw::TestDisplayModeでCreatePrimaryに失敗");
		return ;
	}

	//	ウィンドゥモードならばクリッパを仕掛ける
	m_WindowClipper.Release();
	if (!bFullScr){
		if (m_WindowClipper.SetClipper(GetDDraw(),m_Primary.GetSurface(),m_hWnd)!=0) {
			Err.Out("CFastDraw::TestDisplayModeでClipに失敗");
			return ;
		}
	}

/*
	if (m_bUseFlip2){
		if (m_Primary2.CreatePrimary2(&m_Primary,m_bUseFlip2)!=0){
			//	これ失敗してたら、フリップ使用しない、とする
			m_bUseFlip2 = false;
		}
	}
*/

	bool b256 = m_Primary.GetSurfaceType()==2;
	if (b256) {
		m_Secondary.m_bSecondary256DIB = true;
		//	RGB555のDIBにしてもらうで！
	}

	//	セカンダリサーフェースの作成
	if (m_Secondary.CreateSecondary(&m_Primary,bUseFilp)!=0) {
		Err.Out("CFastDraw::TestDisplayModeでCreateSecondaryに失敗");
		return ;
	}

/*
	// 256でないならばパレットの設定は無意味
	if (b256) InstallPalette();
*/

	//	もし、フルスクリーンモードの256色でかつ、フリップを使用するならば、
	//	Secondary256は不要なのだが．．．
	if (b256) {
		//	256変換用サーフェース
		m_Secondary256.InnerCreateSurface(nSX,nSY,false,true);
	}

	//	ここでウィンドゥモードを反映しておく
	CAppManager::GetMyWindow()->ChangeScreen(bFullScr);
	CAppManager::GetMyWindow()->ChangeWindowStyle();

	m_bDisplayChanging	= false;	//	画面モードの変更に成功！
	m_nScreenColor	= nColor;
	m_nScreenXSize	= nSX;
	m_nScreenYSize	= nSY;
	return ;
}

LRESULT CFastDraw::EndChangeDisplay(){
	struct tmpclass {
		tmpclass(CCriticalSection*c):c_(c){}
		~tmpclass() { c_->Leave();}
		CCriticalSection*c_;
	};
	tmpclass t(GetCriticalSection());
	//	抜けるときにBeginChangeDisplayでかけた排他ロックを解除しよう

	if (GetDDraw()==NULL) return 1;
	
	if (m_bDisplayChanging) { // Changeされていない！！
		Err.Out("CFastDraw::BeginChangeDisplay～EndChangeDisplayに失敗");
		return 1;
	}

	// ウインドウモードで、ディスプレイモードが変わってないなら待たない
	if(m_bChangeDisplayMode) {// 追加
		::Sleep(500);	//	ビデオモードの変更の余波を受ける可能性がある
	}

/*
	// パレットの再構築
	m_bBrightnessChanged = true;
*/

	// 自前で描画するならば関係ないのだが...
	CAppManager::GetMyWindow()->ShowCursor(!m_bFullScr);
	// ウィンドゥサイズの変更
	CAppManager::GetMyWindow()->SetSize(m_nScreenXSize,m_nScreenYSize);
	// ウィンドゥスタイルの変更

	CAppManager::GetMyWindow()->ChangeScreen(m_bFullScr);
	//	自分の所属ウィンドゥには告知してやる。
	//	（本当は、すべてのウィンドウにブロードキャストする必要がある）
	CAppManager::GetMyWindow()->ChangeWindowStyle();

	// プレーンの再構築（もし消失していれば）
	RestoreAll();

	//	（パレットのリアライズ後に）
	// yaneGTL.hで使用する、RGB555⇒256の変換テーブルの初期化
	//	(ただし、256色モード以外のときは、何もせずに戻ってくる)
	CFastPlaneBlendTable::OnChangePalette();	

	return 0;
}

/////// 使用例:p) ////////////////////////////////////////////////////////////

LRESULT CFastDraw::ChangeDisplay(bool bFullScr){
	if (GetDDraw()==NULL) return 1; //	だめだこりゃ...
	BeginChangeDisplay();
		if (bFullScr) {
			TestDisplayMode(m_nScreenXSize,m_nScreenYSize,true,m_nScreenColor);
		}
		TestDisplayMode(m_nScreenXSize,m_nScreenYSize); // ウィンドゥモード
	if (EndChangeDisplay()) return 2;	//	ディスプレイモード変更に失敗
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//	ディスプレイモードの設定→変更

LRESULT		CFastDraw::SetDisplay(bool bFullScr,int nSizeX,int nSizeY,int nColorDepth){
	if (nSizeX!=0) {
		m_nScreenXSize	=	nSizeX;
	}
	if (nSizeY!=0) {
		m_nScreenYSize	=	nSizeY;
	}
	if (nColorDepth!=0) {
		m_nScreenColor	=	nColorDepth;
	} else {
		m_nScreenColor	=	16; // fixed '01/04/08
	}
	ChangeDisplay(bFullScr);
	//	このパラメータだけはじかに渡す
	//	（m_bFullScrは現在の画面モードを保持しているので）
	return 0;
}

void		CFastDraw::GetDisplay(bool&bFullScr,int &nSizeX,int &nSizeY,int &nColorDepth){
	bFullScr		=	m_bFullScr;
	nSizeX			=	m_nScreenXSize;
	nSizeY			=	m_nScreenYSize;
	nColorDepth		=	m_nScreenColor;
}

void		CFastDraw::GetSize(int &nSizeX,int &nSizeY){	// GetDisplayModeのx,yだけ得られる版
	nSizeX			=	m_nScreenXSize;
	nSizeY			=	m_nScreenYSize;
}
	
bool	CFastDraw::IsFullScreen(){
	return		m_bFullScr;
}

void	CFastDraw::CheckSurfaceLost(){
	LPDIRECTDRAWSURFACE lpSurface = GetPrimary()->GetSurface();
	//	システムメモリ上のサーフェースはLostしないのである＾＾；
	if (lpSurface==NULL) return ;
	if (lpSurface->IsLost()){
		ChangeDisplay(m_bFullScr);
		//	プライマリサーフェースの再構築から行なうべき
	}
}

/*
int		CFastDraw::GetBpp(){	// 現在のBppの取得
	return GetPrimary()->GetBpp();
}
*/

void	CFastDraw::SetBrightness(int nBright){
	if (m_nBrightness == nBright) return ;
	m_nBrightness = nBright;
}

//////////////////////////////////////////////////////////////////////////////

void	CFastDraw::SetOffset(int ox,int oy){
	//	セカンダリの転送オフセット
	m_nSecondaryOffsetX =	ox;
	m_nSecondaryOffsetY =	oy;
}

void	CFastDraw::OnDraw(LPCRECT lpRect,bool bLayerCallBack){

	//	最小化されているならば描画せずにリターン
	if (CAppManager::GetMyWindow()->IsMinimized()) return ;

	//	MenuのPopup処理
	int nMenu = 0;
	//	FullScrで、メニューありで、マウスが画面上のメニュー領域にさしかかったのならば...
	if (IsFullScreen() && ::GetActiveWindow()==m_hWnd) {
	//						↑さらにメインウインドウがアクティブ(ダイアログなどが呼び出されてない)
		POINT point;
		::GetCursorPos(&point);
		if (point.y<m_nMenu) {
			if (::GetMenu(m_hWnd)!=NULL){
//				↑このメニューサイズを正確に算出するため、メニュー高さを調べる(c)yaneurao'00/12/15
				nMenu = m_nMenu;
				//	Paintしなおすぜー
//				::SetMenu(m_hWnd,GetMenu(m_hWnd));	//	メニュー領域の再描画
				::DrawMenuBar(m_hWnd);
			}
		}
	}
	
	//	もし、サーフェースが消失していれば...
	if (m_bLostSurface) {
		//	スレッドがもう破棄されるのならばリストアは無効...
		if (!CAppManager::GetMyApp()->IsThreadValid()) return ;
		CheckSurfaceLost();
		//	このフラグは、メッセージポンプ側で
		//	リストアイベントに応じて設定される
		m_bLostSurface = false;
		return ;
	}
	
	LPDIRECTDRAWSURFACE lpPrimary = GetPrimary()->GetSurface();
	if (lpPrimary==NULL) return ;

	//////////////////////////////////////////////////////////
	//	Layer callback
	if ( bLayerCallBack ){
		//	Layerをセカンダリに描画してもらう＾＾
		m_LayerList.OnDraw(GetSecondary());
		
		//	HDCLayerをセカンダリに描画してもらう＾＾
		if (!m_HDCLayerList.IsEmpty()){
			HDC hdc = GetSecondary()->GetDC();
			if (hdc!=NULL){
				m_HDCLayerList.OnDraw(hdc);
				GetSecondary()->ReleaseDC();
			}
//			Invalidate();	//	書き書きしたんで、汚しフラグ立てる
		}
		
		//	AfterLayerをセカンダリに描画してもらう＾＾
		m_AfterLayerList.OnDraw(GetSecondary());

		//	ブライトネスに応じて、セカンダリを減衰
		RealizeBrightness(m_nBrightness);
	}
	//////////////////////////////////////////////////////////
	
	//	プライマリが汚れていなければリターン
	//	if (!m_bDirty) return ;
	//	DirtyRect管理は行なっていない
	//	どうせFullScreenでFlipを使うと最小更新差分では管理できない
	
	LPDIRECTDRAWSURFACE lpSecondary;
	CFastPlane* lpSecondarySurface;
	if (m_Primary.GetSurfaceType()==2){	//	8bppか...
		//	Secondaryは、実はRGB555なので転送してやる必要あり
		GetSecondary256()->BltFast(GetSecondary(),0,0);
		lpSecondary = GetSecondary256()->GetSurface();
		lpSecondarySurface = GetSecondary256();
	} else {
		lpSecondary = GetSecondary()->GetSurface();
		lpSecondarySurface = GetSecondary();
	}
	if (lpSecondary==NULL) return ;

	RECT r; // source rect
	::SetRect(&r,0,0,m_nScreenXSize,m_nScreenYSize);
	
	if (m_nSecondaryOffsetX || m_nSecondaryOffsetY) {
		//	転送先オフセットがかかっているのか？
		RECT r2; // distination rect
		if ( lpRect == NULL ){	// 部分更新か？
			::SetRect(&r2,0,0,m_nScreenXSize,m_nScreenYSize);	//	同一サイズと仮定して良い
			//	a clipping algorithm
			//		between the same size rect (C) yaneurao'1999
			if (m_nSecondaryOffsetX>0) {
				r.right		-= m_nSecondaryOffsetX;
				r2.left		+= m_nSecondaryOffsetX;
			} else {
				r.left		-= m_nSecondaryOffsetX;
				r2.right	+= m_nSecondaryOffsetX;
			}
			if (m_nSecondaryOffsetY>0) {
				r.bottom	-= m_nSecondaryOffsetY;
				r2.top		+= m_nSecondaryOffsetY;
			} else {
				r.top		-= m_nSecondaryOffsetY;
				r2.bottom	+= m_nSecondaryOffsetY;
			}
			
			if (CWindow::IsFullScreen()){
				// フルスクリーンでも、Bltなのら！
				DDBLTFX fx;
				ZERO(fx);	//	一応初期化
				fx.dwSize = sizeof(fx);
				fx.dwFillColor = lpSecondarySurface->GetFillColor();
				RECT br;
				::SetRect(&br,0,nMenu,m_nScreenXSize,r2.top);					//	上領域
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				//	main画像
				lpPrimary->BltFast(r2.left,r2.top,lpSecondary,&r,DDBLTFAST_WAIT);
				::SetRect(&br,0,r2.top,r2.left,r2.bottom);				//	左領域
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				::SetRect(&br,r2.right,r2.top,m_nScreenXSize,r2.bottom);	//	右領域
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				::SetRect(&br,0,r2.bottom,m_nScreenXSize,m_nScreenYSize);	//	下領域
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				
			} else {
				// DirectDrawClipperを利用するにはBltにしなくてはならない。
				// しかし、なんちゅー制限や。いい加減にして欲しい>Clipper君！
				POINT cp;
				cp.x = cp.y = 0;
				::ClientToScreen(m_hWnd,&cp); // クライアントウィンドゥの矩形内左上！
				
				DDBLTFX fx;
				ZERO(fx);	//	一応初期化
				fx.dwSize = sizeof(fx);
				fx.dwFillColor = lpSecondarySurface->GetFillColor();
				
				RECT br;
				::SetRect(&br,0,0,m_nScreenXSize,r2.top);					//	上領域
				::OffsetRect(&br,cp.x,cp.y);
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				
				//	main画像
				br = r2;
				::OffsetRect(&br,cp.x,cp.y);
				lpPrimary->Blt(&br,lpSecondary,&r,DDBLT_WAIT,NULL);
				
				::SetRect(&br,0,r2.top,r2.left,r2.bottom);					//	左領域
				::OffsetRect(&br,cp.x,cp.y);
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				::SetRect(&br,r2.right,r2.top,m_nScreenXSize,r2.bottom);	//	右領域
				::OffsetRect(&br,cp.x,cp.y);
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				::SetRect(&br,0,r2.bottom,m_nScreenXSize,m_nScreenYSize);	//	下領域
				::OffsetRect(&br,cp.x,cp.y);
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
			}
		}else{
			// 部分更新
				r = *lpRect;	// source rest;
				r2 = *lpRect;	// dest	 rest;
			if (CWindow::IsFullScreen()){
				// from CPlane DRAW_CLIPPER
				RECT& sr = r;			
				int x = m_nSecondaryOffsetX;
				int y = m_nSecondaryOffsetY;
				{
					RECT clip;
					::SetRect(&clip,0,0,m_nScreenXSize,m_nScreenYSize);
					int x2,y2;													
					x2 = x + sr.right - sr.left; /* x + Width  */				
					y2 = y + sr.bottom - sr.top; /* y + Height */				
					if (x2<clip.left || y2<clip.top								
						|| x>=clip.right || y>=clip.bottom) return;	 /* 画面外 */ 
					int t;														
					t = clip.left-x;										
					if (t>0)	{ sr.left	+= t;	x = clip.left;	}		
					t = clip.top -y;										
					if (t>0)	{ sr.top	+= t;	y = clip.top;	}		
					t = x2-clip.right;										
					if (t>0)	{ sr.right	-= t;	x2= clip.right; }		
					t = y2-clip.bottom;										
					if (t>0)	{ sr.bottom -= t;	y2= clip.bottom;}		
					if (sr.right<=sr.left || sr.bottom<=sr.top) return; // invalid rect
				}
				lpPrimary->BltFast(x,y,lpSecondary,&r,DDBLTFAST_WAIT);
			}else{
				// オフセット転送の部分更新なので、ずれた黒背景は描画しない
				// DirectDrawClipperを利用するにはBltにしなくてはならない。
				// しかし、なんちゅー制限や。いい加減にして欲しい>Clipper君！
				POINT cp;
				cp.x = cp.y = 0;
				::ClientToScreen(m_hWnd,&cp); // クライアントウィンドゥの矩形内左上！
				::OffsetRect(&r2,m_nSecondaryOffsetX,m_nSecondaryOffsetY);
				::OffsetRect(&r2,cp.x,cp.y); 
				lpPrimary->Blt(&r2,lpSecondary,&r,DDBLT_WAIT,NULL);
			}
			
		}
		
	} else {
		//	転送オフセットが指定されていないフツーの場合
		
		if (CWindow::IsFullScreen()){
			if (/*m_bUseFlip2*/ false && lpRect == NULL ){
				lpPrimary->Flip(NULL,DDFLIP_WAIT);
			} else {
				if ( lpRect != NULL ){
					r = *lpRect;
				}
				//	こいつ、メニュー表示対応！(yane'02/01/13)
				if ( nMenu && nMenu > r.top ) {
					r.top = nMenu;
				}
				lpPrimary->BltFast(r.left,r.top,lpSecondary,&r,DDBLTFAST_WAIT);
			}
		} else {
			// DirectDrawClipperを利用するにはBltにしなくてはならない。
			// しかし、なんちゅー制限や。いい加減にして欲しい>Clipper君！
			POINT cp;
			cp.x = cp.y = 0;
			::ClientToScreen(m_hWnd,&cp); // クライアントウィンドゥの矩形内左上！
			if ( lpRect != NULL ) {
				r = *lpRect;
			}
			RECT sr = r;
			::OffsetRect(&sr,cp.x,cp.y);
			lpPrimary->Blt(&sr,lpSecondary,&r,DDBLT_WAIT,NULL);
		}
	}
	
//	m_bDirty = false;
}

//////////////////////////////////////////////////////////////////////////////
void	CFastDraw::RealizeBrightness(int nBrightness){
	if (m_nBrightness==256) return ;
	//	これだけでええんか＾＾；　得した＾＾；
	GetSecondary()->FadeEffect(nBrightness);
}

//////////////////////////////////////////////////////////////////////////////

void	CFastDraw::RestoreAll(){ // 全プレーンのリロード
	GetFastPlaneList()->for_each(&CFastPlane::Restore);
	return ;
}

//////////////////////////////////////////////////////////////////////////////
LRESULT		CFastDraw::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){

	switch(uMsg){
	case WM_PAINT : {
		//	dirty rectが発生
//		m_bDirty = true;
//		if (!CWindow::IsFullScreen()) OnDraw();	//	一応、再描画してまう。
//		OnDraw();
		/*
			これ外していたのだが、もしコメントを外すとしても
			WM_PAINTの処理中にはGetDCは使えないので注意が必要。
			（GetDCで取得したデバイスコンテキストに描画するとWM_PAINTが発生するから）
			WM_PAINTの処理中にWM_PAINTが発生するんで、アプリケーションはWM_PAINTの
			処理しかできなくなる！
		*/
		return 0;	//	DefWindowProcを呼び出しに行く。
					//	これを呼び出しておかないと再度WM_PAINTが送られてくる
					}

	case WM_ACTIVATEAPP: {
		UINT bActive = wParam;
		if(bActive) {
			//	メッセージポンプ側ではなく、ワーカースレッド側でリストアすべき...
			m_bLostSurface = true;
		}
		return 0;
						 }

	case WM_PALETTECHANGED: {
		//	（パレットのリアライズ後に）
		// yaneGTL.hで使用する、RGB555⇒256の変換テーブルの初期化
		CFastPlaneBlendTable::OnChangePalette();	
		return 0;
							}

	} // end switch


	return 0;
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif // USE_FastDraw
