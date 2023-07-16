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
				Err.Out("CAppManager::Add����̂ɋ󂫂�����");
				goto Exit;
			}
			it++;
		}

		{ // info�̕ϐ��X�R�[�v�𐧌����邽�߂� { }
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
				//	�����X���b�hID�̃`�����N�������āA�����ɒǉ�����
				(*it)->m_lpAppFrame = lpAppFrame;
				//	��̃X���b�h�ɑ΂��ĂQ��CAppFrame�̃C���X�^���X��������
				//	���݂��邱�Ƃ͑z�肵�Ă��Ȃ�
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
		//	��v����X���b�h�������Ƃ͂ǂ��䂤��������H
		Err.Out("CAppManager::Add����̂ɋ󂫂�����");
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
				//	�}���`�X���b�h��߂��̂ŁA�X���b�h�̑��݂ɂ�����炸
				//	������폜�o���ėǂ��B
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
				(*it)->m_lpAppFrame = NULL;	//	���ׂĂɑ΂��čs�Ȃ�
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
	//	�X���b�h�}�l�[�W���̗͂��؂����ƁD�D
	{
		//	�������[�X�ߒ��ŁA���X���b�h���ACAppManager::Unhook��
		//	�Ăяo���ƁA��d��CriticalLock��������
		CAppCallBackList::iterator it = GetCallBackList()->begin();
		while (it!=GetCallBackList()->end()){
			(*it)->run();	//	�R�[���o�b�N���čs��
			it++;
		}
	//	GetCriticalSection()->Leave();
	}
}

//	���@�V�X�e�����̂P:p
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

//	���@�V�X�e�����̂Q:p
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

//	���@�V�X�e�����̂R:p
bool	CAppManager::_IsFullScreen(void){
	return CWindow::IsFullScreen();
}

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd
