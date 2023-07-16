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

	// ������Hook�������T���č폜���Ă䂭�i�������肤��j
	CCriticalLock cl(GetCriticalSection());
		//	�폜���ɑ��̃v���Z�X��������Ă���Ǝ��ʂ���
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
		//	�폜���ɑ��̃v���Z�X��������Ă���Ǝ��ʂ���
	{
		m_HookPtrList.clear();
	}
}

//	message dispatcher
//	  CWindow��WndProc����Ăяo�����̂ŁA����CriticalSection�́A�e�E�B���h�D�ɂ�����݂��邱�ƂɂȂ�B
//	  �܂肱��CriticalSection�́A���b�Z�[�W���[�v�ƁA����ɑΉ����郁�C���X���b�h�Ԃł̑��ݔr���̂��߂̂��̂ł���B
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
				return l; // �Ԃ��l�Ƃ��ĂO�ȊO�����Ȃ�΃��b�Z�[�W�͏������ꂽ�ƌ��Ȃ�
			}
			it ++ ;
		}
	}

	if (pWndProc == NULL) {
		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	} else {
		//	hook�����E�B���h�D�֐����Ăяo��
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
				return l; // �Ԃ��l�Ƃ��ĂO�ȊO�����Ȃ�΃��b�Z�[�W�͏������ꂽ�ƌ��Ȃ�
			}
			it ++ ;
		}
	}
	return 0;
}

int		CMessagePooler::SetPoolData(UINT uMsg,WPARAM wParam,bool bWEnable,
		LPARAM lParam,bool bLEnable)
{
	//	�󂫂�T��
	for(int i=0;i<(int)m_aInfo.size();++i){
		if (m_aInfo[i].isEmpty()){
			//	�󂫂��������̂ŁA�������g����
			m_aInfo[i].setMsg(uMsg,wParam,bWEnable,lParam,bLEnable);
			return i;
		}
	}
	//	�󂫂��Ȃ��̂ŁA�V���Ɋm�ۂ��Ă������g����
	CInfo info;
	info.setMsg(uMsg,wParam,bWEnable,lParam,bLEnable);
	m_aInfo.push_back(info);
	return m_aInfo.size()-1;
}

void	CMessagePooler::ClearPoolData(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	//	�T��
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
