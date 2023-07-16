// JoyStick Input Wrapper

#include "stdafx.h"

#ifdef USE_JOYSTICK

#include "yaneJoyStick.h"

//////////////////////////////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Input {

CJoyStick::CJoyStick(){
	SetNullDevice();
}

void	CJoyStick::SetNullDevice(){
	//	Null Deviceを突っ込んどく
	m_pJoyStick.Add(new CJoyStickNullDevice);
}

LRESULT CJoyStick::SetButtonMax(int max){
	return GetJoyStick()->SetButtonMax(max);
}

int	 CJoyStick::GetButtonMax() const {
	return GetJoyStick()->GetButtonMax();
}

LRESULT CJoyStick::SelectDevice(int j){
	//	規定外の数値ならばNull Deviceを突っ込んどく
	if (j<0) { SetNullDevice(); return 1; }

	m_pJoyStick.Add(new CWIN32JoyStick);
	if (GetJoyStick()->SelectDevice(j)!=0){
		return 1;
	}
	return 0;
}

LRESULT CJoyStick::Input(){
	return GetJoyStick()->Input();
}

bool CJoyStick::IsKeyPress(int key) const{
	return GetJoyStick()->IsKeyPress(key);
}
bool CJoyStick::IsKeyPush(int key) const{
	return GetJoyStick()->IsKeyPush(key);
}
const BYTE* CJoyStick::GetKeyData() const{
	return GetJoyStick()->GetKeyData();
}

bool	CJoyStick::IsInit() const {
	return GetJoyStick()->IsInit();
}

//////////////////////////////////////////////////////////////////////////////

CWIN32JoyStick::CWIN32JoyStick(){
	m_nButtonMax	=	6;					//	６ボタンがディフォルト
	m_nSelect		=	0;
	m_bSuccessInit	=	false;
}

LRESULT CWIN32JoyStick::SetButtonMax(int max) {
	// ボタンの数を変えるくらいならInitializeはいらない
	m_nButtonMax = max;
	Reset();
	return 0;
}

LRESULT CWIN32JoyStick::SelectDevice(int j) {
	m_nSelect = j;
	m_bSuccessInit	=	false;

	int n = joyGetNumDevs();
	if ( j < 0 || j>=n) return 1;	//	ジョイスティックあらへんやん...
	/**
		⇒これどうせ、DirectX5がインストールされている環境では16が返ってくる
	*/

	m_uID = JOYSTICKID1 + j;
	//	実は、この指定をしても動く！
	/*
	switch (m_nSelect){
	case 0: m_uID = JOYSTICKID1; break;
	case 1: m_uID = JOYSTICKID2; break;
	default: return 2;	//	規定外の選択
	}
	*/

	JOYCAPS joyCaps;
	if ( joyGetDevCaps(m_uID,&joyCaps,sizeof(JOYCAPS)) != JOYERR_NOERROR) {
		return 3;	//	初期化に失敗
	}

	//	センターはニュートラルだろうから、１：２に内分するところを閾値とする
	m_uX1	=	(joyCaps.wXmax	 +	joyCaps.wXmin*2)/3;
	m_uX2	=	(joyCaps.wXmax*2 +	joyCaps.wXmin  )/3;
	m_uY1	=	(joyCaps.wYmax	 +	joyCaps.wYmin*2)/3;
	m_uY2	=	(joyCaps.wYmax*2 +	joyCaps.wYmin  )/3;

	JOYINFO joyinfo;	//	実際に読み込めるのか確認しておく必要あり
	if (joyGetPos(m_uID,&joyinfo)!= JOYERR_NOERROR) return 4;

	m_bSuccessInit	=	true;
	Reset();	//	途中でのデバイス切り替えに対応するため一応、クリアする。

	return 0;
}

// ---------------------------------------------------------------------------

LRESULT CWIN32JoyStick::Input(){
	if (!m_bSuccessInit) return 1;	//	きみぃ、そもそも初期化失敗しとるがに..

	//	バッファのflip!
	FlipKeyBuffer(m_nKeyBufNo);

	//	ボタン４つ以下のときは、joyGetPosで十分...
	if (m_nButtonMax<=4) {
		UINT x,y,b;
		//	本来ならば、コールバック関数にしてメッセージハンドラを用意すべき...
		JOYINFO joyinfo;
		if (joyGetPos(m_uID,&joyinfo)!= JOYERR_NOERROR) return 2;
		x = WORD(joyinfo.wXpos);		//	X座標
		y = WORD(joyinfo.wYpos);		//	Y座標
		b = WORD(joyinfo.wButtons);		//	ボタン情報
		m_byKeyBuffer[m_nKeyBufNo][0] = y<m_uY1?1:0;	//	上
		m_byKeyBuffer[m_nKeyBufNo][1] = y>m_uY2?1:0;	//	下
		m_byKeyBuffer[m_nKeyBufNo][2] = x<m_uX1?1:0;	//	左
		m_byKeyBuffer[m_nKeyBufNo][3] = x>m_uX2?1:0;	//	右
		//	４番～３５（最大）番までが、各ボタン情報である！
		//	４番がボタン１，５番がボタン２．．以下３５番まで
		for(int i=0;i<m_nButtonMax;i++){
			m_byKeyBuffer[m_nKeyBufNo][i+4] = b & 1;
			b >>= 1;
		}
	} else {
		DWORD x,y,b;
		JOYINFOEX joyinfo;
		joyinfo.dwSize	= sizeof(JOYINFOEX);
		joyinfo.dwFlags	= JOY_RETURNALL;
		if (joyGetPosEx(m_uID,&joyinfo)!= JOYERR_NOERROR) return 2;
		x = WORD(joyinfo.dwXpos);		//	X座標
		y = WORD(joyinfo.dwYpos);		//	Y座標
		b =	 joyinfo.dwButtons;			//	ボタン情報
		m_byKeyBuffer[m_nKeyBufNo][0] = y<m_uY1?1:0;	//	上
		m_byKeyBuffer[m_nKeyBufNo][1] = y>m_uY2?1:0;	//	下
		m_byKeyBuffer[m_nKeyBufNo][2] = x<m_uX1?1:0;	//	左
		m_byKeyBuffer[m_nKeyBufNo][3] = x>m_uX2?1:0;	//	右
		for(int i=0;i<m_nButtonMax;i++){
			m_byKeyBuffer[m_nKeyBufNo][i+4] = (BYTE)(b & 1);
			b >>= 1;
		}
	}
	return 0;
}

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd

#endif // #ifdef USE_JOYSTICK
