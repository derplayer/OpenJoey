#include "stdafx.h"
#include "yaneWinHook.h"

namespace yaneuraoGameSDK3rd {
namespace Window {

void CWinHookList::Add(IWinHook* hook)
{
	CCriticalLock cl(GetCriticalSection());
	{
		m_HookPtrList.push_back(hook);
	}
}

void CWinHookList::Del(IWinHook* hook){

	// 自分がHookしたやつを探して削除してゆく（複数ありうる）
	CCriticalLock cl(GetCriticalSection());
		//	削除中に他のプロセスが介入してくると死ぬため
	{
		for(hooklist::iterator it=m_HookPtrList.begin();it!=m_HookPtrList.end();) {
			if (*it==hook) {
				it = m_HookPtrList.erase(it);
			} else {
				it ++;
			}
		}
	}
}

void CWinHookList::Clear(){

	CCriticalLock cl(GetCriticalSection());
		//	削除中に他のプロセスが介入してくると死ぬため
	{
		m_HookPtrList.clear();
	}
}

//	message dispatcher
//	  CWindowのWndProcから呼び出されるので、このCriticalSectionは、各ウィンドゥにつき一つ存在することになる。
//	  つまりこのCriticalSectionは、メッセージループと、それに対応するメインスレッド間での相互排他のためのものである。
LRESULT CWinHookList::Dispatch(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,WNDPROC pWndProc){

	{
		CCriticalLock cl(GetCriticalSection());
		hooklist::iterator it=m_HookPtrList.begin();
		while (it!=m_HookPtrList.end()) {
			LRESULT l;

//			Err.Out("%.8x %.8x %.8x",(DWORD)this,(DWORD)*it,((*it)->WndProc));
//			DWORD d = reinterpret_cast<DWORD>((*it)->WndProc);

			l = (*it)->WndProc(hWnd,uMsg,wParam,lParam);
			if (l) {
				return l; // 返し値として０以外を持つならばメッセージは処理されたと見なす
			}
			it ++ ;
		}
	}

	if (pWndProc == NULL) {
		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	} else {
		//	hookしたウィンドゥ関数を呼び出す
		return ::CallWindowProc(pWndProc,hWnd,uMsg,wParam,lParam);
	}
}

LRESULT CWinHookList::DispatchDlg(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,WNDPROC pWndProc){
	{
		CCriticalLock cl(GetCriticalSection());
		hooklist::iterator it=m_HookPtrList.begin();
		while (it!=m_HookPtrList.end()) {
			LRESULT l;
			l = (*it)->WndProc(hWnd,uMsg,wParam,lParam);
			if (l) {
				return l; // 返し値として０以外を持つならばメッセージは処理されたと見なす
			}
			it ++ ;
		}
	}
	return 0;
}

int		CMessagePooler::SetPoolData(UINT uMsg,WPARAM wParam,bool bWEnable,
		LPARAM lParam,bool bLEnable)
{
	//	空きを探す
	for(int i=0;i<(int)m_aInfo.size();++i){
		if (m_aInfo[i].isEmpty()){
			//	空きがあったので、こいつを使おう
			m_aInfo[i].setMsg(uMsg,wParam,bWEnable,lParam,bLEnable);
			return i;
		}
	}
	//	空きがないので、新たに確保してそいつを使おう
	CInfo info;
	info.setMsg(uMsg,wParam,bWEnable,lParam,bLEnable);
	m_aInfo.push_back(info);
	return m_aInfo.size()-1;
}

void	CMessagePooler::ClearPoolData(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	//	探す
	for(int i=0;i<(int)m_aInfo.size();++i){
		if (m_aInfo[i].isMsg(uMsg,wParam,lParam)){
			m_aInfo[i].init();
		}
	}
}

LRESULT CMessagePooler::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	for(int i=0;i<(int)m_aInfo.size();++i){
		if (m_aInfo[i].isMsg(uMsg,wParam,lParam)){
			m_aInfo[i].pool(wParam,lParam);
		}
	}
	return 0;
}

} // end of namespace Window
} // end of namespace yaneuraoGameSDK3rd
