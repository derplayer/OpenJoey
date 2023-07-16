#include "stdafx.h"
#include "yaneDebugWindow.h"
#include "../AppFrame/yaneAppManager.h"

namespace yaneuraoGameSDK3rd {
namespace Window {

LRESULT CDebugWindow::OnPreCreate(CWindowOption &opt){		//	override from CAppBase

	opt.caption		= "�ł΂����\���`";
	opt.classname	= "LISTBOX";
	opt.size_x		= 400;
	opt.size_y		= 200;
	opt.style		=	WS_VISIBLE | WS_CAPTION | LBS_NOINTEGRALHEIGHT | WS_VSCROLL
					| WS_SIZEBOX									//	�T�C�Y�͉�
					| WS_MAXIMIZEBOX | WS_MINIMIZEBOX| WS_SYSMENU;	//	���������ƁA�E�B���h�E�́~�󂪂��B
	opt.bCentering	= false;	//	�ǂ��ł��ǂ�

	m_bIdle = true; // Idle���[�h�ɂ���

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
	//	���ꕶ���񂪒����ꍇ�`�F�b�N����悤�ɂ����ق���������..

	int n = ::lstrlen(buf) - 1;
	if (buf[n] == '\n') buf[n] = '\0'; // ���s��ׂ�

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
	//	�ψ�����邩�`���b�₯��Ȃ��i�΁j
	CHAR	buf[512];
	::wvsprintf(buf,(LPSTR)*lpl,(LPSTR)(lpl+1) );
	Out(buf);
}


//////////////////////////////////////////////////////////////////////////////
//	�`�F�b�N����

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

	//	�ψ�����邩�`���b�₯��Ȃ��i�΁j
	CHAR	buf[512];
	::wvsprintf(buf,fmt,(LPSTR)(&fmt+1) );
	m_lpDebugWindow[m_nTarget]->Out(buf);
}

void	CDbg::CheckValid(){
//	CCriticalLock guard(GetCriticalSection());
	if (m_lpDebugWindow[m_nTarget]==NULL) {
		m_lpDebugWindow[m_nTarget] = new CDebugWindow;
		m_lpDebugWindow[m_nTarget]->Run();

		//	�I������Ƃ��ɁA�R�[���o�b�N�����Ă��炦��悤�ɗv������
		smart_ptr<function_callback> f(
			function_callback_v::Create((void (*)(int))Release,m_nTarget)
									//	cast���Ȃ��Ɓ�static�ȃ����o�֐��̃}�b�`���O�Ɏ��s���₪��
		);
		CCriticalLock cl(CAppManager::GetCriticalSection());
		CAppManager::GetCallBackList()->insert(f);
	} else {
	//	�����I�����Ă���̂Ȃ�΁A�ċN�������Ƃ��H
		if (!m_lpDebugWindow[m_nTarget]->IsThreadExecute()){
			m_lpDebugWindow[m_nTarget]->Run();
		}
	}
}

} // end of namespace Window
} // end of namespace yaneuraoGameSDK3rd
