// yaneDirectSound.h
//	 This is DirectSound wrapper.
//		programmed by yaneurao(M.Isozaki) '99/06/18
//				modified by yaneurao	  '00/03/02
//				modified by ENRA		  '02/03/22

#ifndef __yaneDirectSound_h__
#define __yaneDirectSound_h__


#include "../Auxiliary/yaneCOMBase.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class CDirectSound
{
/**
	DirectSoundの初期化のためのクラス
*/
public:
	CDirectSound();
	virtual ~CDirectSound();

	static bool CanUseDirectSound();
	/**
		DirectSoundが使える環境なのかどうかを調べて返す
		調べるのは最初の１回目の呼び出しのみなので何度呼び出しても良い。
	*/

	/// DirectSoundObjectを取得する
	LPDIRECTSOUND Get() { return GetObject()->get(); }

	int	GetStatus() const { return m_nStatus; }
	/**
		DirectSoundの初期化状況について、リザルトを返す
		0: success
		1: failure - サウンドカードが無かった
		2: failure - 初期化が出来なかった
		3: failure - 強調レベルが設定出来なかった
	*/

	LRESULT SetCooperativeLevel(DWORD dwLevel);
	/**
		DirectSoundの協調レベルを設定する
		DSSCL_NORMAL 
		DSSCL_PRIORITY
		DSSCL_EXCLUSIVE のいずれかを設定する
	*/

protected:
	// DirectSoundObjectの取得
	CCOMObject<LPDIRECTSOUND>*	GetObject() { return &m_vDirectSound; }
	CCOMObject<LPDIRECTSOUND>	m_vDirectSound;
	// 保有しているウィンドウハンドル
	HWND	m_hWnd;
	// GetStatus関数が返す初期化状況
	int		m_nStatus;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif
