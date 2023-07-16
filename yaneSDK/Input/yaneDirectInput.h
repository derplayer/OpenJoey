// yaneDirectInput.h
//	 This is DirectInput wrapper.
//		programmed by yaneurao(M.Isozaki) '99/06/18
//		modified by yaneurao '00/02/29

#ifndef __yaneDirectInput_h__
#define __yaneDirectInput_h__


#include "../Auxiliary/yaneCOMBase.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

class CDirectInput {
/**
	DirectInputの初期化のためのクラス
	class CKeyInput , class CJoyStick にて使用
*/
public:

	LPDIRECTINPUT Get() const { return m_lpDirectInput; }
	/**
		コンストラクタで自動的にDirectInputの初期化を試みるので、
		この関数で、IDirectInputを取得すればそれでｏｋ。
	*/

	int	GetStatus() const { return m_nStatus; }
	/**
		DirectInputの初期化状況について、リザルトを返す
		0:ok(DirectX5以降) 1:ok(DirectX3以降)
		2:ok(DirectX3 on NT4.0)
		3,4,5:failure(DirectX3すら入ってない)
	*/

	CDirectInput();
	virtual ~CDirectInput(){}

private:
	CCOMObject<LPDIRECTINPUT>	m_vDirectInput;
	CCOMObject<LPDIRECTINPUT>* GetDirectInput() { return& m_vDirectInput;}
	CLoadLibraryWrapper m_vLoadLibraryWrapper;
	CLoadLibraryWrapper* GetLib() { return& m_vLoadLibraryWrapper; }

	LPDIRECTINPUT m_lpDirectInput;
	int		m_nStatus;	//	この変数の意味についてはGetStatusを参照のこと
};

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd

#endif
