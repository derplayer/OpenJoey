
// yaneFastDraw.h
/**
	 This is a extended DirectDraw wrapper using the template library.
		programmed by yaneurao(M.Isozaki) '01/10/01-'01/10/06

		描画に関する、高速かつシンプルな実装。
		yaneuraoGameSDK 3rd styleを作るときは、
		きっと、CDirectDraw,CPlane,CDIBDrawを破棄して、
		このクラスにするだろう。

		それほど、今後、描画の根幹となるであろうクラス。

		メインは、メモリ上に確保したサーフェース。
		セカンダリも、メモリ上に確保したサーフェース。
		プライマリ（場合によってはセカンダリも）だけが、DirectDrawSurface。
		基本的には、そういう使いわけ。

		ただし、フルスクリーン時は、フリッピングサーフェースもサポート。
			⇒あまりメリットを感じないので、ノンサポートにしました＾＾；
		そのときは、VRAM上に確保されたプライマリが２つあって、
		セカンダリはメモリ上に確保されている、というソフト的な
		トリプルバッファリングを行なう。

		また、描画のための転送は、functorを駆使して行なう。
		最終的には、部分的に、Pentium/MMXのアセンブラによるコードを
		投入するかも知れない。

*/

#ifndef __yaneFastDraw_h__
#define __yaneFastDraw_h__


#ifdef USE_FastDraw

#include "yaneSurface.h"
#include "../Window/yaneWinHook.h"
#include "yaneLayer.h"
//#include "yaneSprite.h"
#include "yaneDirectDraw.h"
#include "yaneFastPlane.h"
#include "../AppFrame/yaneAppDraw.h"

//////////////////////////////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CFastDraw : public IWinHook , public IAppDraw {
public:

	///	---	IAppDrawのメンバ
	virtual int GetType() const { return 1; }

	//////////////////////////////////////////
	//	ディスプレイモードの変更

	LRESULT		SetDisplay(bool bFullScr=false,int nSizeX=0,int nSizeY=0,int nColorDepth=0);
	void		GetDisplay(bool&bFullScr,int &nSizeX,int &nSizeY,int &nColorDepth);
	void		GetSize(int &nSizeX,int &nSizeY);	// GetDisplayModeのx,yだけ得られる版
	bool		IsFullScreen();

	//	Begin～Endでディスプレイモードを変更する。
	void		BeginChangeDisplay();
	void		TestDisplayMode(int nSX,int nSY,bool bFullScr=false,int nColor=0);
	LRESULT		EndChangeDisplay();

	//////////////////////////////////////////

//	void	UseHardwareFlip(bool bEnable) { m_bUseFlip=true; }
	//	ハードウェアフリッピングを使うのか？
	//		ディフォルトではtrue
	//		もし、変更するならば、ディスプレイモードの変更に先行して、
	//		この関数を呼び出すこと。
	//	FastDrawにはFlip機構は速度的には、あまりアドバンテージが無い
	//	また、セカンダリをビデオメモリに配置してしまうと、
	//	それぞれの転送における、lock～unlockのコストが無視できない．．
	//	単に、描画時の、フリッカリング（ちらつき）の防止ぐらいの意味
	//	ただし、DirectDrawは、Windows GDIを無視して描画を行なうので
	//	メニュー等が正常に作動しなくなるので注意が必要

	void	UseDibSectionIn256(bool bEnable){}
		//	⇒　現状では、未サポート
	//	256色モードにおいて、RGB555(16bpp)を使うかどうか。
	//		ディフォルトではtrue
	//		もし、256色モードでは、パレットを使い、
	//		独自の半透明処理を行なうならばfalseにしておくこと。

	//////////////////////////////////////////
	//	プライマリとセカンダリの取得

	CFastPlane*		GetPrimary()		{ return &m_Primary; }
		//	↑　描画するためのプライマリサーフェース
/*
	CFastPlane*		GetPrimary2()		{ return &m_Primary2; }
		//	↑フルスクリーンで、ハードウェアフリッピングを行なうとき
		//		このサーフェースが裏画面になる
*/
	CFastPlane*		GetSecondary256()	{ return &m_Secondary256; }
		//	↑２５６色モードのときには、いったんセカンダリ(RGB555)から、
		//　このサーフェースに転送してから描画するのだ

	CFastPlane*		GetSecondary()		{ return &m_Secondary;}
		//	メモリ上に確保された、セカンダリサーフェース
		//		ディフォルトではDirectDrawSurfaceとなる。
		//	ただし、256色モードにおいては、RGB555のサーフェース！

	//////////////////////////////////////////
	//	Secondary->Primaryプレーンの転送
	
	virtual void	OnDraw(LPCRECT lpRect=NULL,bool bLayerCallBack=true);
	//	Secondary->Primaryへの転送

	//	上記転送のときに、関わってくるパラメータ
	void	SetOffset(int ox,int oy);	//	セカンダリの転送オフセット
	void	SetBrightness(int nBright); //	フェード比率(0-256)

	//////////////////////////////////////////
	//	レイヤの管理

	//	Layerのリストを返す
	ILayerList* GetLayerList() { return &m_LayerList; }
	ILayerList* GetAfterLayerList() { return &m_AfterLayerList; }
	ILayerList* GetHDCLayerList() { return &m_HDCLayerList; }

	//	↑レイヤとは、OnDrawのとき、すなわち、セカンダリからプライマリへ
	//	転送するときに、コールバックとして呼び出されるのだが、これは、
	//	セカンダリがHDCを取得できるサーフェース（DirectDrawSurface）
	//	であることがその前提となる。

	//////////////////////////////////////////
	/// ----	おまけ
	int	GetBpp(); // 現在の画面bppの取得

	///	これでDirectDrawPtrを取得する
	LPDIRECTDRAW	GetDDraw() { return m_DirectDrawBase.Get(); }

	/// このクラスが管理しているサーフェースへのポインタを取得
	list_chain<CFastPlane>* GetFastPlaneList() { return& m_listFastPlane; }

	//////////////////////////////////////////
	//	property..

	int		GetMenuHeight();
	//	メニューがついているならば、そのメニュー高さを返す

	//////////////////////////////////////////

	CFastDraw();
	virtual ~CFastDraw();

protected:
	CDirectDraw m_DirectDrawBase;

	//	画面モードの変更関連
	LRESULT		ChangeDisplay(bool bFullScr);	//	現在のディスプレイモードを反映させる
	bool	m_bFullScr;			//	フルスクリーンモードか？
	int		m_nScreenXSize;		//	画面サイズ
	int		m_nScreenYSize;
	int		m_nScreenColor;		//	画面bpp
	bool	m_bDisplayChanging; //	解像度変更中
	bool	m_bChangeDisplayMode;// ディスプレイモードを変更するか // 追加

	//	--- OnDrawで利用されるもの
	//		画面のフェード関連
	int		m_nBrightness;
	//		転送オフセット
	int		m_nSecondaryOffsetX;		//	セカンダリの転送オフセット量
	int		m_nSecondaryOffsetY;

	HWND	m_hWnd;				//	いちいちCAppInitializerから取得すると遅くなるので

	int		m_nMenu;			//	メニュー高さ
	//	このクラスのコンストラクタで計算して保持しておく

	//	for Layer management
	CLayerList	m_LayerList;
	CLayerList	m_AfterLayerList;
	CLayerList	m_HDCLayerList;

	//////////////////////////////////////////
	//	サーフェースのロストチェック
	//	（ユーザーは、明示的には、呼び出さない）
	void		CheckSurfaceLost();
	//	サーフェースのロストチェック（内部用）
	bool	m_bLostSurface;

/*
	//	フルスクリーン時にfilpping surfaceを使用するのか？
	//	default == true
	bool	m_bUseFlip;
	//	実際に、フリッピングサーフェースは作成されたのか？
	bool	m_bUseFlip2;
*/

	//	一時しのぎ
	void	RealizeBrightness(int nBrightness);

	///////////////////////////////////////////////////////

	list_chain<CFastPlane> m_listFastPlane;

	///////////////////////////////////////////////////////
	//	CFastPlaneのコンストラクタにはCFastDraw*が必要
	//	またCFastPlaneは、CFastDrawのlist_chainにaddしようとする
	//	よって、こいつらをCFastDrawのコンストラクタで初期化するときには、
	//	上のlist_chainが初期化済みでなくてはならない。
	
	//	プライマリサーフェースとセカンダリサーフェース
	CFastPlane	m_Primary;	//	
//	CFastPlane	m_Primary2;	//	Flippingするときのためのサーフェース
	CFastPlane	m_Secondary256;	//	256色モードの時の隠しセカンダリ
	CFastPlane	m_Secondary;
	CDirectDrawClipper m_WindowClipper;

	CFastDraw* GetThis() { return this; }

	//	サーフェースのロストを検出して、リストアをかける
	void	RestoreAll();


	//	ディスプレイ解像度を変更するときの排他lock用
	CCriticalSection m_cs;
	CCriticalSection* GetCriticalSection(){return &m_cs;}

	// overriden from CWinHook
	LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif // USE_FastDraw

#endif
