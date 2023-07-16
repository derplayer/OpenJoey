// yaneJoyStick.h
//	 This is a JoyStick input wrapper.
//		programmed by yaneurao(M.Isozaki) '99/08/16

#ifndef __yaneJoyStick_h__
#define __yaneJoyStick_h__


#ifdef USE_JOYSTICK

#include "yaneKeyBase.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

class IJoyStick : public IKeyBase {
/// IJoyStick はIKeyBaseのJoyStick版
public:
	///	from CKeyBase
///	virtual LRESULT Input()=0; // 256バイト状態を読み込む
///	virtual bool	IsKeyPress(int key) const;
///	virtual bool	IsKeyPush(int key) const;
///	virtual const BYTE*	GetKeyData() const;

	virtual LRESULT	SelectDevice(int n)=0;
	///	デバイスのナンバーを指定(0-15)
	///	そのデバイスの初期化に失敗したら、LRESULTは非０を返す

	///	for JoyStick setting
	virtual LRESULT	SetButtonMax(int max)=0;
	virtual int		GetButtonMax() const=0;
	///	joy stickのボタン数の設定取得。
	///	ここで設定したボタン数まで認識する。default:最大6ボタン。
	/**
		このボタン数の設定は、SelectDeviceして成功したときのみ有効
		default : 6ボタン
	*/

	virtual bool	IsInit() const=0;
	/**
		デバイスの初期化に成功したか？
	*/
};

class CJoyStickNullDevice : public IJoyStick {
/**
	JoyStick用のNull Device
	class CJoyStick で使用される
*/
public:
	virtual LRESULT Input() { return 0; }
	virtual bool	IsKeyPress(int key) const{ return false;}
	virtual bool	IsKeyPush(int key) const{ return false; }
	virtual LRESULT	SelectDevice(int n) { return 0; }
	virtual LRESULT	SetButtonMax(int max) { return 0; }
	virtual int		GetButtonMax() const { return 0; }
	virtual bool	IsInit() const { return true; }
};

class CWIN32JoyStick : public IJoyStick	 {
///	Win32のjoy stick入力を行なうwrapper
public:
	virtual LRESULT Input(); // 256バイト状態を読み込む

	virtual LRESULT	SelectDevice(int n);
	///	デバイスのナンバーを指定(0-1)	default : 0
	///	そのデバイスの初期化に失敗したら、LRESULTは非０を返す

	///	for JoyStick setting
	virtual LRESULT	SetButtonMax(int max);
	virtual int		GetButtonMax()const		{ return m_nButtonMax; }
	/**
		このボタン数の設定は、SelectDeviceして成功したときのみ有効
		default : 6ボタン
	*/

	virtual bool	IsInit() const { return m_bSuccessInit;}
	/**
		デバイスの初期化に成功したか？
	*/

	CWIN32JoyStick();

protected:

	bool	m_bSuccessInit;				//	初期化に成功したのか
	UINT	m_uID;						//	JOYSTICK1 or JOYSTICK2
	int		m_nButtonMax;				//	ボタン最大数(ディフォルト2)

	UINT	m_uX1,m_uX2;				//	入力敷居値
	UINT	m_uY1,m_uY2;

	int		m_nSelect;					//	JOYSTICK selector
	
};

class CJoyStick : public IJoyStick {
/**

*/
public:
	///	from IKeyBase
	virtual LRESULT Input(); /// 256バイト状態を読み込む
	virtual bool	IsKeyPress(int key) const;
	virtual bool	IsKeyPush(int key) const;
	virtual const BYTE*	GetKeyData() const;
	/**
		方向入力は、IsPushKey/IsPressKeyの引数に0,1,2,3が上下左右。
		そのあと4～35が、ボタン1～32（最大）に対応する。
	*/

	virtual LRESULT	SelectDevice(int n);
	///	デバイスのナンバーを指定(0-15)
	///	そのデバイスの初期化に失敗したら、LRESULTはfalseを返す
	/**
		実は、DirectInputを用いなくともDirectX5のインストールされている
		環境ならばWin32APIで16本までJoyStickを認識できる。
	*/

	//	for JoyStick setting
	virtual LRESULT	SetButtonMax(int max);
	virtual int		GetButtonMax()const;
	/**
		検知する最大ボタン数を設定。0～32。default : 6 ボタン
		このボタン数の設定は、SelectDeviceして成功したときのみ有効
	*/

	virtual bool	IsInit() const;

	CJoyStick();

protected:
	smart_ptr<IJoyStick>	m_pJoyStick;
	smart_ptr<IJoyStick> GetJoyStick() const { return m_pJoyStick; }
	void	SetNullDevice();	//	JoyStickにNullDeviceを設定する
};

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd

#endif // #ifdef USE_JOYSTICK
#endif
