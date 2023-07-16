#include "stdafx.h"
#include "yaneImeWrapper.h"
#include "../Window/yaneHWndManager.h"
#include "winnls32.h"	//	Win3.1����̈�Y�O�O�G

//	2001/07/31 sohei
//		http://www.hcn.zaq.ne.jp/no-ji/reseach/980715.htm
//		���Q�l�ɂ����Ē����܂���
//

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

singleton<CIMEWrapper> CIMEWrapper::m_vIMEWrapper;

void	CIMEWrapper::Show(){
	if (m_nStat==1) return ;
	m_nStat = 1;
	HWND hWnd = CHWndManager::GetObj()->GetHWnd();
	//	���@NULL�ɑ΂��āA����API���Ăяo����IME���������˂Ȃ�
	if (hWnd!=NULL) {
		::WINNLSEnableIME(hWnd,TRUE);		 // �h�l�d������
	}
}

void	CIMEWrapper::Hide() {
	if (m_nStat==2) return ;
	m_nStat = 2;
	HWND hWnd = CHWndManager::GetObj()->GetHWnd();
	//	���@NULL�ɑ΂��āA����API���Ăяo����IME���������˂Ȃ�
	if (hWnd!=NULL) {
		::WINNLSEnableIME(hWnd,FALSE);	 // �h�l�d������
	}
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
