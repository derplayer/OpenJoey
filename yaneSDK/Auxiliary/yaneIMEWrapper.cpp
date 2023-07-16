#include "stdafx.h"
#include "yaneImeWrapper.h"
#include "../Window/yaneHWndManager.h"
#include "winnls32.h"	//	Win3.1時代の遺産＾＾；

//	2001/07/31 sohei
//		http://www.hcn.zaq.ne.jp/no-ji/reseach/980715.htm
//		を参考にさせて頂きました
//

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

singleton<CIMEWrapper> CIMEWrapper::m_vIMEWrapper;

void	CIMEWrapper::Show(){
	if (m_nStat==1) return ;
	m_nStat = 1;
	HWND hWnd = CHWndManager::GetObj()->GetHWnd();
	//	※　NULLに対して、このAPIを呼び出すとIMEが落ちかねない
	if (hWnd!=NULL) {
		::WINNLSEnableIME(hWnd,TRUE);		 // ＩＭＥを現す
	}
}

void	CIMEWrapper::Hide() {
	if (m_nStat==2) return ;
	m_nStat = 2;
	HWND hWnd = CHWndManager::GetObj()->GetHWnd();
	//	※　NULLに対して、このAPIを呼び出すとIMEが落ちかねない
	if (hWnd!=NULL) {
		::WINNLSEnableIME(hWnd,FALSE);	 // ＩＭＥを消す
	}
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
