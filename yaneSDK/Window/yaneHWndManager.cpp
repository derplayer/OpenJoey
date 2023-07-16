#include "stdafx.h"
#include "yaneHWndManager.h"

namespace yaneuraoGameSDK3rd {
namespace Window {

//	static object
singleton<CHWndManager> CHWndManager::m_vHWndManager;

HWND	CHWndManager::GetHWnd(){
	/// そのスレッドに対応するHWNDの取得
	if (m_hWnd.isEmpty()){	//	要素あらへんやん？
		m_hWnd = 0;
		//	この瞬間、他スレッドが生成したらやだなぁ...
		//	でも、ウィンドゥ生成後にしかこの関数を
		//	呼び出さないと思うのでNULLであること自体、
		//	ありえない気もする..
	}
	return m_hWnd;
}

void	CHWndManager::SetHWnd(HWND hWnd){
	///	そのスレッドに対応するHWNDの設定
	m_hWnd = hWnd;	//	ThreadLocalなので、これだけで良い
}

} // end of namespace Window
} // end of namespace yaneuraoGameSDK3rd
