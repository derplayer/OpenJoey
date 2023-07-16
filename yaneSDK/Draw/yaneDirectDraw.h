// yaneDirectDraw.h
//	 This is DirectDraw wrapper.

#ifndef __yaneDirectDraw_h__
#define __yaneDirectDraw_h__


#include "../Auxiliary/yaneCOMBase.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CDirectDraw {
/**
	このクラスは、DirectDrawのCOMオブジェクトのwrapper
*/
public:

	LPDIRECTDRAW Get() const { return m_vDirectDraw.get(); }
	/**
		コンストラクタで自動的にDirectDrawの初期化を試みるので、
		この関数で、IDirectDrawを取得すればそれでｏｋ。
	*/

	int	GetStatus() const { return m_nStatus; }
	/**
		DirectDrawの初期化状況について、リザルトを返す
		0:ok
		1,2:DirectX3が入っていない
		3:HEL初期化に失敗。HAL初期化には成功。←DirectDrawは使える
		4:HEL,HALの初期化に失敗。DirectDrawは、この状態では使えない。
	*/

	CDirectDraw(bool bEmulationOnly=false);
	/**
		bEmulationOnly == trueならばHELで動く
		2D専用のゲームならば、HELで動かしたほうが良い
	*/
	
	virtual ~CDirectDraw() {}

protected:
	CCOMObject<LPDIRECTDRAW>	m_vDirectDraw;
	CCOMObject<LPDIRECTDRAW>*	GetDirectDraw() { return& m_vDirectDraw;}
	int		m_nStatus;	//	この変数の意味についてはGetStatusを参照のこと
};

class CDirectDrawSurfaceManager {
/**
	ピクセルフォーマット調べるねん！
*/
public:
	int GetSurfaceType(LPDIRECTDRAWSURFACE);
	///	↑特定サーフェースのサーフェースタイプを調べる
	/**
		0:	サーフェースのタイプの取得に失敗
		1:	サーフェースのタイプの特定が出来なかった
			（以下の以外だと思われる)
		2:	8bpp
		3:	16(RGB565)
		4:	16(RGB555)
		5:	24(RGB888)
		6:	24(BGR888)
		7:	32(XRGB8888)
		8:	32(XBGR8888)

		10:	16+a4(ARGB4565)	//	実際は、これが返ってくることは無い
		11:	16+a4(ARGB4555)	//	実際は、これが返ってくることは無い
		12:	32(ARGB8888)	//	実際は、これが返ってくることは無い
		13:	32(ABGR8888)	//	実際は、これが返ってくることは無い
	*/
};

class CDirectDrawClipper {
public:
	LRESULT SetClipper(LPDIRECTDRAW lpDraw,LPDIRECTDRAWSURFACE lpPrimary,HWND hWnd);
	//	こいつをクリップする

	void	Release();						//	クリッパを解放する

	CDirectDrawClipper() : m_lpClipper(NULL) {}
	virtual ~CDirectDrawClipper() { Release();}

protected:
	LPDIRECTDRAWCLIPPER m_lpClipper;
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneDirectDraw_h__
