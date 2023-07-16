#include "stdafx.h"
#include "yaneDebugWindow.h"
#include "../AppFrame/yaneAppManager.h"

namespace yaneuraoGameSDK3rd {
namespace Window {

LRESULT CDebugWindow::OnPreCreate(CWindowOption &opt){		//	override from CAppBase

	opt.caption		= "でばっぐ表示～";
	opt.classname	= "LISTBOX";
	opt.size_x		= 400;
	opt.size_y		= 200;
	opt.style		=	WS_VISIBLE | WS_CAPTION | LBS_NOINTEGRALHEIGHT | WS_VSCROLL
					| WS_SIZEBOX									//	サイズは可変
					| WS_MAXIMIZEBOX | WS_MINIMIZEBOX| WS_SYSMENU;	//	これを入れると、ウィンドウの×印がつく。
	opt.bCentering	= false;	//	どこでも良い

	m_bIdle = true; // Idleモードにする

	return 0;
}

void	CDebugWindow::Clear(){
	::SendMessage(GetHWnd(),LB_RESETCONTENT,0,0L);
}

void __cdecl CDebugWindow::Out( LPSTR fmt, ... ) {
	HWND hWnd = GetHWnd();
	if (hWnd == NULL) return ;
	
	CHAR	buf[512];
	::wvsprintf(buf,fmt,(LPSTR)(&fmt+1) );
	//	これ文字列が長い場合チェックするようにしたほうがいいな..

	int n = ::lstrlen(buf) - 1;
	if (buf[n] == '\n') buf[n] = '\0'; // 改行を潰す

	::SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR) buf);
	UINT sel = (UINT)::SendMessage(hWnd,LB_GETCOUNT,0,0L);
	if(sel!=LB_ERR) {
		::SendMessage(hWnd,LB_SETCURSEL,sel-1,0L);
	}
}

void	CDebugWindow::Out(const string& str){
	Out((LPSTR)str.c_str());
}

void	CDebugWindow::Out(int i){
	CHAR buf[16];
	::wsprintf(buf,"%d",i);
	Out(buf);
}

void	CDebugWindow::Out(LONG*lpl){
	//	可変引数取るか～世話やけんなぁ（笑）
	CHAR	buf[512];
	::wvsprintf(buf,(LPSTR)*lpl,(LPSTR)(lpl+1) );
	Out(buf);
}


//////////////////////////////////////////////////////////////////////////////
//	チェックする

CDebugWindow* CDbg::m_lpDebugWindow[10];
int	CDbg::m_nTarget =0;
CCriticalSection CDbg::m_cs;

void	CDbg::Release(int nTarget){
	if (m_lpDebugWindow[nTarget]!=NULL){
		delete m_lpDebugWindow[nTarget];
	}
}

void	CDbg::Out(int i)				{
	CCriticalLock guard(GetCriticalSection());
	CheckValid(); GetWindow()->Out(i);
}

void	CDbg::Out(const string&s)		{
	CCriticalLock guard(GetCriticalSection());
	CheckValid(); GetWindow()->Out(s);
}

void	CDbg::Out(LONG*lpl)			{
	CCriticalLock guard(GetCriticalSection());
	CheckValid(); GetWindow()->Out(lpl);
}

void __cdecl CDbg::Out( LPSTR fmt, ... ) {
	CCriticalLock guard(GetCriticalSection());
	CheckValid();

	//	可変引数取るか～世話やけんなぁ（笑）
	CHAR	buf[512];
	::wvsprintf(buf,fmt,(LPSTR)(&fmt+1) );
	m_lpDebugWindow[m_nTarget]->Out(buf);
}

void	CDbg::CheckValid(){
//	CCriticalLock guard(GetCriticalSection());
	if (m_lpDebugWindow[m_nTarget]==NULL) {
		m_lpDebugWindow[m_nTarget] = new CDebugWindow;
		m_lpDebugWindow[m_nTarget]->Run();

		//	終了するときに、コールバックをしてもらえるように要請する
		smart_ptr<function_callback> f(
			function_callback_v::Create((void (*)(int))Release,m_nTarget)
									//	castしないと↑staticなメンバ関数のマッチングに失敗しやがる
		);
		CCriticalLock cl(CAppManager::GetCriticalSection());
		CAppManager::GetCallBackList()->insert(f);
	} else {
	//	もし終了しているのならば、再起動させとく？
		if (!m_lpDebugWindow[m_nTarget]->IsThreadExecute()){
			m_lpDebugWindow[m_nTarget]->Run();
		}
	}
}

} // end of namespace Window
} // end of namespace yaneuraoGameSDK3rd
