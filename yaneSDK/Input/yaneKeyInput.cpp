// DirectInput Wrapper

#include "stdafx.h"
#include "yaneKeyInput.h"
#include "../Auxiliary/yaneCOMBase.h"
#include "../AppFrame/yaneAppManager.h"
#include "../AppFrame/yaneAppInitializer.h"

// #include "../Window/yaneDebugWindow.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

CKeyInput::CKeyInput(bool bBackGround){

	m_lpDIKeyDev = NULL;
	m_nStatus = GetDirectInput()->GetStatus();
	if (m_nStatus>=3) return ; // 初期化失敗してるにょろ

	LPDIRECTINPUT lpDInput = GetDirectInput()->Get();

	//キーボードデバイスの作成
	if (lpDInput->CreateDevice(GUID_SysKeyboard
			,&m_lpDIKeyDev,NULL)!=DI_OK){
		Err.Out("CKeyInput::CKeyInputでDirectInputの初期化CreateDeviceに失敗");
		m_nStatus = 6;
		return ; // おかしいーがな...
	}
	//	キーボードフォーマットの設定
	if(GetDevice()->SetDataFormat(&c_dfDIKeyboard)!=DI_OK){
		Err.Out("CKeyInput::CKeyInputでDirectInputの初期化SetDataFormatに失敗");
		m_nStatus = 7;
		return ;
	}
	// WindowNTで動作させるには、ここは、
	// DISCL_FOREGROUND | DISCL_NONEXCLUSIVEでなくてはいけない。
	
//			CDbg().Out((int)CAppManager::GetHWnd());
	if (bBackGround){
		//	バックグラウンドでの入力を許可
		if (GetDevice()->SetCooperativeLevel( CAppManager::GetHWnd(),
			//	バックグラウンドの入力を許可する場合でも、ウィンドゥハンドルは指定すべき
			DISCL_BACKGROUND| DISCL_NONEXCLUSIVE)!=DI_OK){
			Err.Out("CKeyInput::CKeyInputでDirectInputの初期化SetCooperativeLevelに失敗");
			m_nStatus = 8;
			return ;
		}
	} else {
		//	バックグラウンドでの入力は不許可
		if (GetDevice()->SetCooperativeLevel( CAppManager::GetHWnd(),
			DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)!=DI_OK){
			Err.Out("CKeyInput::CKeyInputでDirectInputの初期化SetCooperativeLevelに失敗");
			m_nStatus = 8;
			return ;
		}
	}
	/**
		ここ、GetHWndではなく、NULL(デスクトップ)にすれば、
		デスクトップと関連付けられるので、そのウィンドゥ上に
		キーフォーカスが無くとも入力することが出来るようになる。
	*/

	///	ここを通過したということは、m_nStatus==0 or 1 or 2

	m_bDIKeyAcquire=false;	//	これ、hookされる前に設定しなくっちゃ
	CAppManager::Hook(this);//	フックを開始する。
}

// ---------------------------------------------------------------------------
CKeyInput::~CKeyInput(){
	if (m_nStatus <= 2){
		CAppManager::Unhook(this);			//	フックを解除する。
	}

	// 獲得していたデバイスの解放
	if(GetDevice()!=NULL){
		if (m_bDIKeyAcquire)
			GetDevice()->Unacquire();
		GetDevice()->Release();
	}

	//	COMの解放は、CCOMObjectが勝手にやってくれるはず．．
}

// ---------------------------------------------------------------------------

LRESULT CKeyInput::Input(){
	if (m_nStatus >= 3) {
		return 1;	//	初期化失敗しとる．．
	}

	//	バッファのflip!
	FlipKeyBuffer(m_nKeyBufNo);

	LRESULT hr;

	// AcquireされていないのにGetDeviceStateしたらおかしくなる
	if (!m_bDIKeyAcquire){
		// Acquireしとらんのか
		hr=m_lpDIKeyDev->Acquire();
		if (hr==DI_OK) {
			m_bDIKeyAcquire=true;
		} else {
			m_bDIKeyAcquire=false;
			// 何もなかったことにしよ:p
			//	バッファをクリアして戻る。（画面外にフォーカスが移ったときに押しっぱなし
			//	になるのを防ぐため）
			::ZeroMemory((LPVOID)&(m_byKeyBuffer[m_nKeyBufNo][0]),256);
			return 1;
		}
	}
	hr = GetDevice()->GetDeviceState(256,
			(LPVOID)&(m_byKeyBuffer[m_nKeyBufNo][0]));
	if (hr==DIERR_INPUTLOST){
	// アプリ切り替わる瞬間はしゃーないのやね	
		hr=GetDevice()->Acquire();
		if (hr!=DI_OK) {
			// 何もなかったことにしよ
			m_bDIKeyAcquire = false;	//	次回にまた獲得してね！:p
			//	バッファをクリアして戻る。（画面外にフォーカスが移ったときに押しっぱなし
			//	になるのを防ぐため）
			::ZeroMemory((LPVOID)&(m_byKeyBuffer[m_nKeyBufNo][0]),256);
			return -1; // 失敗の巻き
			// Err.Out("GetKeyStateでデバイスをLOSTしている");
		}
		//	一回はok
		hr = GetDevice()->GetDeviceState(256,
			(LPVOID)&(m_byKeyBuffer[m_nKeyBufNo][0]));
	}
	if (hr==DI_OK) {
		return 0;
	} else {
		//	バッファをクリアして戻る。（画面外にフォーカスが移ったときに押しっぱなし
		//	になるのを防ぐため）
		::ZeroMemory((LPVOID)&(m_byKeyBuffer[m_nKeyBufNo][0]),256);
		m_bDIKeyAcquire = false;	//	次回にまた獲得してね！:p
		return 1;
	}
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CKeyInput::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){

	// ALTとかゲームで使いたいんでキーメッセージは捨てるよん...
	if ( WM_KEYFIRST<=uMsg && uMsg<=WM_KEYLAST ) {
//		CDbg().Out(uMsg);
//		return 1;
	}

	switch(uMsg){
	case WM_ACTIVATEAPP: {
/*	//	まだそのウィンドゥがアクティブでないから、これやらんほうがええんか？
		UINT bActive = wParam;
		if(bActive) {
			//	メッセージポンプ側ではなく、ワーカースレッド側でリストアすべき...
			// Acquireしとらんのか？
			if (!m_bDIKeyAcquire) {
				m_bDIKeyAcquire=(m_lpDIKeyDev->Acquire()==DI_OK);
			}
		}
*/
						 } break;
	}
	return 0;	//	all right,sir...
}

//////////////////////////////////////////////////////////////////////////////
//	これ0x80と&とる必要があるんで、overrideしたほうが良い？

inline bool CKeyInput::IsKeyPress(int key) const {
	return	(m_byKeyBuffer[m_nKeyBufNo][key] & 0x80) !=0;
};

inline bool CKeyInput::IsKeyPush(int key) const {
	// 押された瞬間にtrueにする場合
	if (!(m_byKeyBuffer[  m_nKeyBufNo][key] & 0x80)) return false;
	if (  m_byKeyBuffer[1-m_nKeyBufNo][key] & 0x80) return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////////

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd
