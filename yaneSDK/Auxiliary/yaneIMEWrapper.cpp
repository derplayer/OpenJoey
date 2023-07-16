#include "stdafx.h"
#include "yaneImeWrapper.h"
#include "../Window/yaneHWndManager.h"
#include "winnls32.h"	//	Win3.1‘ã‚ÌˆâYOOG

//	2001/07/31 sohei
//		http://www.hcn.zaq.ne.jp/no-ji/reseach/980715.htm
//		‚ğQl‚É‚³‚¹‚Ä’¸‚«‚Ü‚µ‚½
//

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

singleton<CIMEWrapper> CIMEWrapper::m_vIMEWrapper;

void	CIMEWrapper::Show(){
	if (m_nStat==1) return ;
	m_nStat = 1;
	HWND hWnd = CHWndManager::GetObj()->GetHWnd();
	//	¦@NULL‚É‘Î‚µ‚ÄA‚±‚ÌAPI‚ğŒÄ‚Ño‚·‚ÆIME‚ª—‚¿‚©‚Ë‚È‚¢
	if (hWnd!=NULL) {
		::WINNLSEnableIME(hWnd,TRUE);		 // ‚h‚l‚d‚ğŒ»‚·
	}
}

void	CIMEWrapper::Hide() {
	if (m_nStat==2) return ;
	m_nStat = 2;
	HWND hWnd = CHWndManager::GetObj()->GetHWnd();
	//	¦@NULL‚É‘Î‚µ‚ÄA‚±‚ÌAPI‚ğŒÄ‚Ño‚·‚ÆIME‚ª—‚¿‚©‚Ë‚È‚¢
	if (hWnd!=NULL) {
		::WINNLSEnableIME(hWnd,FALSE);	 // ‚h‚l‚d‚ğÁ‚·
	}
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
