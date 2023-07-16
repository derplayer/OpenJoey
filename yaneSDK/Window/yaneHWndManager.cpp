#include "stdafx.h"
#include "yaneHWndManager.h"

namespace yaneuraoGameSDK3rd {
namespace Window {

//	static object
singleton<CHWndManager> CHWndManager::m_vHWndManager;

HWND	CHWndManager::GetHWnd(){
	/// ���̃X���b�h�ɑΉ�����HWND�̎擾
	if (m_hWnd.isEmpty()){	//	�v�f����ւ���H
		m_hWnd = 0;
		//	���̏u�ԁA���X���b�h������������₾�Ȃ�...
		//	�ł��A�E�B���h�D������ɂ������̊֐���
		//	�Ăяo���Ȃ��Ǝv���̂�NULL�ł��邱�Ǝ��́A
		//	���肦�Ȃ��C������..
	}
	return m_hWnd;
}

void	CHWndManager::SetHWnd(HWND hWnd){
	///	���̃X���b�h�ɑΉ�����HWND�̐ݒ�
	m_hWnd = hWnd;	//	ThreadLocal�Ȃ̂ŁA���ꂾ���ŗǂ�
}

} // end of namespace Window
} // end of namespace yaneuraoGameSDK3rd
