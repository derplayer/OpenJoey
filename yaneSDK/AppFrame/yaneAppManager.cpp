#include "stdafx.h"
#include "yaneAppManager.h"
#include "yaneAppBase.h"
#include "../Window/yaneWindow.h"

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

singleton<CAppManager> CAppManager::m_obj;

//////////////////////////////////////////////////////////////////////////////
void CAppManager::_Inc(){
	CCriticalLock cl(_GetCriticalSection());
	m_nRef++;
}

void CAppManager::_Dec(){
	CCriticalLock cl(_GetCriticalSection());
	m_nRef--;
}

int CAppManager::_GetRef(){
	int n;
	CCriticalLock cl(_GetCriticalSection());
	n = m_nRef;
	return n;
}
//////////////////////////////////////////////////////////////////////////////

IWindow*	CAppManager::GetMyWindow(){
	IAppBase* pApp = GetMyApp();
	if (pApp==NULL) return NULL;
	return pApp->GetMyWindow();
}

//////////////////////////////////////////////////////////////////////////////

void CAppManager::_Add(IAppBase*lpAppBase){

	DWORD	dwThreadID = ::GetCurrentThreadId();

	CCriticalLock cl(_GetCriticalSection());
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if ((*it)->m_lpAppBase == lpAppBase) {
				if ((*it)->m_dwThreadID == NULL)
					{ (*it)->m_dwThreadID = dwThreadID; goto Exit; }
				Err.Out("CAppManager::Addするのに空きが無い");
				goto Exit;
			}
			it++;
		}

		{ // infoの変数スコープを制限するための { }
			CAppManageInfo* info = new CAppManageInfo;
			info->m_lpAppBase	=	lpAppBase;	
			info->m_lpAppFrame	=	NULL;
			info->m_lpAppDraw	=	NULL;
			info->m_dwThreadID	=	dwThreadID;
			m_alpInfo.insert(info);
		}

Exit:;
	}

}

void CAppManager::_Add(IAppFrame*lpAppFrame){

	DWORD	dwThreadID = ::GetCurrentThreadId();
	CCriticalLock cl(_GetCriticalSection());
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if (((*it)->m_dwThreadID == dwThreadID)) {
				//	同じスレッドIDのチャンクを見つけて、そこに追加する
				(*it)->m_lpAppFrame = lpAppFrame;
				//	一つのスレッドに対して２つのCAppFrameのインスタンスが同時に
				//	存在することは想定していない
				break;
			}
			it++;
		}
	}
}

////////////////////////////////////////////////////////////

void CAppManager::_Add(IAppDraw* lpDraw){

	DWORD	dwThreadID = ::GetCurrentThreadId();

	CCriticalLock cl(_GetCriticalSection());
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if ((*it)->m_dwThreadID == dwThreadID)
				{ (*it)->m_lpAppDraw = lpDraw; goto Exit; }
			it++;
		}
		//	一致するスレッドが無いとはどうゆうこっちゃ？
		Err.Out("CAppManager::Addするのに空きが無い");
Exit:;
	}

}

void CAppManager::_Del(IAppDraw*lpDraw) {

	DWORD	dwThreadID = ::GetCurrentThreadId();

	CCriticalLock cl(_GetCriticalSection());
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if (((*it)->m_dwThreadID == dwThreadID)
				&& ((*it)->m_lpAppDraw == lpDraw)){
				(*it)->m_lpAppDraw = NULL;
			}
			it++;
		}
	}
}

IAppDraw* CAppManager::_GetMyDraw(){
	CAppManageList::iterator it = m_alpInfo.begin();
	DWORD dwThreadID = ::GetCurrentThreadId();
	IAppDraw* lpDraw = NULL;

	CCriticalLock cl(_GetCriticalSection());
	{
		while (it!=m_alpInfo.end()) {
			if ((*it)->m_dwThreadID == dwThreadID ) {
				lpDraw = (*it)->m_lpAppDraw;
				break;
			}
			it++;
		}
	}
	return lpDraw;		//	return null if it was not found...
}

////////////////////////////////////////////////////////////

void CAppManager::_Del(IAppBase*app) {

	DWORD	dwThreadID = ::GetCurrentThreadId();

	CCriticalLock cl(_GetCriticalSection());
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if ((*it)->m_lpAppBase == app) {
				if ((*it)->m_dwThreadID == dwThreadID)
					{ (*it)->m_dwThreadID = NULL; }
				//	マルチスレッドやめたので、スレッドの存在にかかわらず
				//	これを削除出来て良い。
				it = m_alpInfo.erase(it);
			} else {
				it++;
			}
		}
	}
}

void CAppManager::_Del(IAppFrame*lpAppFrame){

//	DWORD	dwThreadID = ::GetCurrentThreadId();

	CCriticalLock cl(_GetCriticalSection());
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if ((*it)->m_lpAppFrame == lpAppFrame) {
				(*it)->m_lpAppFrame = NULL;	//	すべてに対して行なう
			}
			it++;
		}
	}
}

IAppBase* CAppManager::_GetMyApp(){
	IAppBase* app = NULL;

	CCriticalLock cl(_GetCriticalSection());
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		DWORD dwThreadID = ::GetCurrentThreadId();
		while (it!=m_alpInfo.end()) {
			if ((*it)->m_dwThreadID == dwThreadID ) {
				app = (*it)->m_lpAppBase;
				break;
			}
			it++;
		}
	}
	return app;		//	return null if it was not found...
}

IAppFrame* CAppManager::_GetMyFrame(){
	IAppFrame* app = NULL;

	CCriticalLock cl(_GetCriticalSection());
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		DWORD dwThreadID = ::GetCurrentThreadId();
		while (it!=m_alpInfo.end()) {
			if ((*it)->m_dwThreadID == dwThreadID) {
				app = (*it)->m_lpAppFrame;
				break;
			}
			it++;
		}
	}

	return app;		//	return null if it was not found...
}

int	CAppManager::_GetAppInstanceNum(){
	int n;

	CCriticalLock cl(_GetCriticalSection());
	{
		n = m_alpInfo.size();
	}

	return n;
}

void CAppManager::_StopAllThread(){
	if (CThreadManager::GetObj()->StopAllThread())
	//	スレッドマネージャの力を借りよっと．．
	{
		//	↑リリース過程で、他スレッドが、CAppManager::Unhookを
		//	呼び出すと、二重にCriticalLockがかかる
		CAppCallBackList::iterator it = GetCallBackList()->begin();
		while (it!=GetCallBackList()->end()){
			(*it)->run();	//	コールバックして行く
			it++;
		}
	//	GetCriticalSection()->Leave();
	}
}

//	魔法システムその１:p
void	CAppManager::_Hook(IWinHook*p){
	CCriticalLock cl(_GetCriticalSection());
	{
		IAppBase* pApp = GetMyApp();
		if (pApp==NULL) return ;
		IWindow* pWin = pApp->GetMyWindow();
		if (pWin==NULL) return ;
		pWin->GetHookList()->Add(p);
	}
}

void	CAppManager::_Unhook(IWinHook*p){
	CCriticalLock cl(_GetCriticalSection());
	{
		IAppBase* pApp = GetMyApp();
		if (pApp==NULL) return ;
		IWindow* pWin = pApp->GetMyWindow();
		if (pWin==NULL) return ;
		pWin->GetHookList()->Del(p);
	}
}

//	魔法システムその２:p
HWND	CAppManager::_GetHWnd() {
	HWND hWnd;
	CCriticalLock cl(_GetCriticalSection());
	{
		IAppBase* lp = CAppManager::GetMyApp();
		if (lp==NULL) {
			hWnd = NULL;
		} else {
			hWnd = lp->GetHWnd();
		}
	}
	return hWnd;
}

//	魔法システムその３:p
bool	CAppManager::_IsFullScreen(void){
	return CWindow::IsFullScreen();
}

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd
