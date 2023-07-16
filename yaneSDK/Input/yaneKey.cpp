#include "stdafx.h"
#include "yaneKey.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

CKey1::CKey1(bool bBackGround) : m_Key(bBackGround) {

	//	キーデバイスの登録

	AddDevice(smart_ptr<IKeyBase> (&m_Key,false));	//	Keyboard
	AddDevice(smart_ptr<IKeyBase> (&m_Joy,false));	//	Joystick

	//	デバイス１があかんなら２で＞JoyStick
	if (m_Joy.SelectDevice(0)!=0){
		m_Joy.SelectDevice(1);
	}
	m_Joy.SetButtonMax(2);	//	2 button

	//	仮想キーの追加

	//	0	:	Escape
	AddKey(0,0,DIK_ESCAPE);

	//	1	:	Up
	AddKey(1,0,DIK_NUMPAD8);
	AddKey(1,0,DIK_UP);
	AddKey(1,1,0);

	//	2	:	Down
	AddKey(2,0,DIK_NUMPAD2);
	AddKey(2,0,DIK_DOWN);
	AddKey(2,1,1);

	//	3	:	Left
	AddKey(3,0,DIK_NUMPAD4);
	AddKey(3,0,DIK_LEFT);
	AddKey(3,1,2);

	//	4	:	Right
	AddKey(4,0,DIK_NUMPAD6);
	AddKey(4,0,DIK_RIGHT);
	AddKey(4,1,3);

	//	5	:	Space
	AddKey(5,0,DIK_SPACE);
	AddKey(5,1,4);

	//	6	:	Return
	AddKey(6,0,DIK_RETURN);
	AddKey(6,0,DIK_NUMPADENTER);
	AddKey(6,0,DIK_LSHIFT);
	AddKey(6,0,DIK_RSHIFT);
	AddKey(6,1,5);

}

CKey2::CKey2(bool bBackGround) : m_Key(bBackGround) {

	//	キーデバイスの登録

	AddDevice(smart_ptr<IKeyBase> (&m_Key,false));	//	Keyboard
	AddDevice(smart_ptr<IKeyBase> (&m_Joy,false));	//	Joystick

	//	デバイス１があかんなら２で＞JoyStick
	if (m_Joy.SelectDevice(0)!=0){
		m_Joy.SelectDevice(1);
	}
	m_Joy.SetButtonMax(6);	//	6 button

	//	仮想キーの追加

	//	0	:	Escape
	AddKey(0,0,DIK_ESCAPE);

	//	1	:	Up
	AddKey(1,0,DIK_NUMPAD8);
	AddKey(1,0,DIK_UP);
	AddKey(1,1,0);

	//	2	:	Down
	AddKey(2,0,DIK_NUMPAD2);
	AddKey(2,0,DIK_DOWN);
	AddKey(2,1,1);

	//	3	:	Left
	AddKey(3,0,DIK_NUMPAD4);
	AddKey(3,0,DIK_LEFT);
	AddKey(3,1,2);

	//	4	:	Right
	AddKey(4,0,DIK_NUMPAD6);
	AddKey(4,0,DIK_RIGHT);
	AddKey(4,1,3);

	//	5	:	Button A
	AddKey(5,0,DIK_SPACE);
	AddKey(5,0,DIK_Z);
	AddKey(5,1,4);

	//	6	:	Button B
	AddKey(6,0,DIK_RETURN);
	AddKey(6,0,DIK_X);
	AddKey(6,0,DIK_NUMPADENTER);
	AddKey(6,1,5);

	//	7	:	Button C
	AddKey(7,0,DIK_C);
	AddKey(7,1,6);

	//	8	:	Button C
	AddKey(8,0,DIK_A);
	AddKey(8,1,7);

	//	9	:	Button C
	AddKey(9,0,DIK_S);
	AddKey(9,1,8);

	//	10	:	Button C
	AddKey(10,0,DIK_D);
	AddKey(10,1,9);

}

CKey3::CKey3(bool bBackGround) : m_Key(bBackGround) {

	//	キーデバイスの登録

	AddDevice(smart_ptr<IKeyBase> (&m_Key,false));	//	Keyboard

	//	仮想キーの追加

	//	0	:	Escape
	AddKey(0,0,DIK_ESCAPE);

	//	1	:	Up
	AddKey(1,0,DIK_NUMPAD8);
	AddKey(1,0,DIK_UP);

	//	2	:	Down
	AddKey(2,0,DIK_NUMPAD2);
	AddKey(2,0,DIK_DOWN);

	//	3	:	Left
	AddKey(3,0,DIK_NUMPAD4);
	AddKey(3,0,DIK_LEFT);

	//	4	:	Right
	AddKey(4,0,DIK_NUMPAD6);
	AddKey(4,0,DIK_RIGHT);

	//	5	:	Space
	AddKey(5,0,DIK_SPACE);

	//	6	:	Return
	AddKey(6,0,DIK_RETURN);
	AddKey(6,0,DIK_NUMPADENTER);
}

CKey4::CKey4(bool bBackGround) : m_Key(bBackGround) {

	//	キーデバイスの登録

	AddDevice(smart_ptr<IKeyBase> (&m_Key,false));	//	Keyboard

	//	仮想キーの追加

	//	0	:	Escape
	AddKey(0,0,DIK_ESCAPE);

	//	1	:	Up
	AddKey(1,0,DIK_NUMPAD8);
	AddKey(1,0,DIK_UP);

	//	2	:	Down
	AddKey(2,0,DIK_NUMPAD2);
	AddKey(2,0,DIK_DOWN);

	//	3	:	Left
	AddKey(3,0,DIK_NUMPAD4);
	AddKey(3,0,DIK_LEFT);

	//	4	:	Right
	AddKey(4,0,DIK_NUMPAD6);
	AddKey(4,0,DIK_RIGHT);

	//	5	:	Button A
	AddKey(5,0,DIK_SPACE);
	AddKey(5,0,DIK_Z);

	//	6	:	Button B
	AddKey(6,0,DIK_RETURN);
	AddKey(6,0,DIK_X);
	AddKey(6,0,DIK_NUMPADENTER);

	//	7	:	Button C
	AddKey(7,0,DIK_C);

	//	8	:	Button C
	AddKey(8,0,DIK_A);

	//	9	:	Button C
	AddKey(9,0,DIK_S);

	//	10	:	Button C
	AddKey(10,0,DIK_D);
}

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd
