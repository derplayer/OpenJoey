//
//	DirectDrawSurface+DIB wrapper
//


#ifndef __yaneFastPlane_h__
#define __yaneFastPlane_h__


#ifdef USE_FastDraw

#include "yaneSurface.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CFastDraw;

class CFastPlane : public ISurface {
/**
	DirectDrawによる２次元向き　描画クラス（サーフェース）
	class CFastDraw も参考にすること。
*/
public:

	///	---- ISurfaceのメンバ
	virtual int GetType() const { return 1; }
	virtual LRESULT Load(const string& strBitmapFileName);
//	virtual LRESULT Save(const string& strBitmapFileName,LPCRECT lpRect=NULL);
//	virtual LRESULT SaveYGA(const string& strBitmapFileName,LPCRECT lpRect=NULL,bool bCompress=true);
	virtual LRESULT	GetSize(int &x,int &y) const;
	virtual LRESULT SetSize(int x,int y){
		int sx,sy;
		GetSize(sx,sy);
		if (sx!=x || sy!=y) return CreateSurface(x,y);
		return 0;	//	resizeしとらん
	}
	virtual LRESULT	Release();

	/** ISurfaceのメンバで十分なので、オーバーライドはしていない
	virtual	LRESULT SetColorKey(ISurfaceRGB rgb);
	virtual LRESULT SetColorKeyPos(int x,int y);
	virtual ISurfaceRGB GetColorKey() const;

		プライマリ／セカンダリのディフォルトの抜き色設定については、
		保証外。プライマリに至っては、サーフェースがlockできない
		ビデオカードもあるので、いじらないのが吉。
	*/

	/**
	//	default動作で良いものはオーバーライドしていない
	//		fill color...
		virtual void		Clear(LPCRECT lpRect=NULL);
		virtual LRESULT		SetFillColor(ISurfaceRGB c);
		virtual ISurfaceRGB	GetFillColor() const;
	*/

	virtual smart_ptr<ISurface> clone();
	virtual LRESULT CreateSurfaceByType(int sx,int sy,int nType);

	/**
		その他、描画関連のメソッドは、ISurfaceのほうに
		かなりあるので、そちらも見てください。
	*/

	//////////////////////////////////////////////////////////////////////////

	//	各種設定
//	void	SetDirectDrawSurface(bool bEnable);	//	DirectDrawSurfaceにするのか
	LRESULT	SetSystemMemoryUse(bool bEnable);	//	システムメモリを使うのか
	//	↑これらは、途中変更は不可。
	//	次にCreateSurafeceもしくは、LoadBitmapしたときから有効。

	/*
		上の２つの設定は、
			プライマリ：true  - false
			セカンダリ：false - true   or	true  - true
			普通のサーフェース　　：false - true
		が標準的だと考えられる。
	*/

	//////////////////////////////////////////////////////////////////////////
	//	HDCを得て、直接書き書き:p

	HDC		GetDC();
		//	↑ただし、かならず取得できるとは限らない。
		//	必ず取得しなければならないのであれば、
		//	SetDirectDrawSurface(true)にしておくこと。
		//	取得できなかった場合、NULLが返る
	void	ReleaseDC();

	///	オーナードローサーフェースの生成
	///	サイズ指定でプレーン作成
	///	(sx,sy)サイズ  bAlpha==trueならば、αサーフェース
	virtual LRESULT CreateSurface(int sx,int sy,bool bAlpha=false);	

	//	プライマリサーフェースの生成
	LRESULT	CreatePrimary(bool& bUseFilp,int nSx=0,int nSy=0);
	//	セカンダリサーフェースの生成
	LRESULT CreateSecondary(CFastPlane*lpPrimary,bool& bUseFlip);

	//////////////////////////////////////////////////////////////////////////
	///		----	Effector

	///　α値を反転させる（αサーフェースに対してのみ有効）
	LRESULT		FlushAlpha(LPCRECT lpRect=NULL) { return 0; } // 未実装(todo)

	/////// todo

	//	α値を得る
	virtual int	GetPixelAlpha(int x,int y) { return 0; }
	//	ピクセルの色を得る

	//////////////////////////////////////////////////////////////////////////
	///	property...

	//	ビットマップが読み込まれていればtrue
	bool	IsLoaded() const { return !m_strBitmapFile.empty(); }
	//	読み込んでいるビットマップのファイル名を返す
	string	GetFileName() const;

	//	YGA画像なのかどうか
	virtual	bool	IsYGA() { return m_bYGA; }
	bool*	GetYGA() { return& m_bYGA; }

	void	SetYGAUse(bool bUse) { m_bYGAUse = bUse; }
	bool	GetYGAUse() { return m_bYGAUse; }
	//	これでtrueを設定すれば↑Loadするときや、CreateSurfaceするとき、
	//	強制的に常にα付きサーフェースを作成する。
	//	ディフォルトではfalse

	//	自動修復サーフェースにする(default:false)
	//		(これだとLostしない。ただし、32bpp->16bppのような変換によってビット深度が失われる)
	void	SetAutoRestore(bool bEnable) { m_bAutoRestore = bEnable; }
	bool	GetAutoRestore() { return m_bAutoRestore; }

	LPDIRECTDRAWSURFACE GetSurface();
	LPDIRECTDRAWPALETTE GetPalette();
	//	↑DirectDrawSurfaceとは限らないので、この２つは
	//		取得できるかどうかわからない．．
	//		取得できなかったときは、NULLが返る

	int		GetBpp() const {
		const int anType[] = { 0,0,8,16,16,24,24,32,32,0,16,16,32,32 };
		return anType[const_cast<CFastPlane*>(this)->GetSurfaceType()];
	}

	//	0～1(不明)の場合、DIB(16bpp)を作りにかかります。
	//	それ以外の場合、m_bMemorySurfaceがtrueであれば、メモリサーフェース
	//	さもなくば、DirectDrawSurfaceですから、m_lpSurfaceが有効ということに
	//	なります。

	//	不明の場合、セカンダリを作るときは、何が何でも
	//	こいつをプライマリへ転送してやる必要があるので、
	//	DIBSectionを作るべき？
	//	というか、こいつが不明ってどういうこと？？

	//////////////////////////////////////////////////////////////////////////

	///		FastDrawのmediator
	void	SetFastDraw(CFastDraw*p) { m_pFastDraw = p; }
	CFastDraw* GetFastDraw() const { return m_pFastDraw; }

	/**		ディフォルトで使用するFastDrawの設定/取得
		こちらは、
		1.CFastPlane::SetFastDraw　か
		2.CFastPlaneのコンストラクタ
		でCFastDraw*が渡されていないときに限り使用するディフォルトの
		CFastDrawの設定／取得。

		※　ThreadLocalで保持しています。

		CFastDrawのコンストラクタでSetDefaultFastDraw(this);とやっているので、
		1スレッドにCFastDrawがひとつしか存在しない場合は特に何もしなくてok

	*/
	static void	SetDefaultFastDraw(CFastDraw*p) { m_pDefaultFastDraw = p; }
	static CFastDraw* GetDefaultFastDraw() {
		return m_pDefaultFastDraw.isEmpty()?NULL:*m_pDefaultFastDraw.Get();
	}

	///	プライマリサーフェースはlockできないことがあるので特殊扱い
	bool	IsPrimarySurface () const { return m_bPrimary; }

	//////////////////////////////////////////////////////////////////////////

	CFastPlane(CFastDraw* pFastDraw=NULL);
	virtual ~CFastPlane();

	friend class CFastDraw;

protected:

	LPDIRECTDRAWSURFACE m_lpSurface;
	//	DirectDrawSurfaceのときは、そのポインタ
	LPDIRECTDRAWPALETTE m_lpPalette;
	//	DirectDrawPaletteのときは、そのポインタ

	//	CreateDIBSectionで作ったサーフェースの場合
	//	(不明ピクセルフォーマットに対してセカンダリのみ、これで有り得る)
	HBITMAP m_hBitmap;
	HDC		m_hDC;

	//	256色モードの時に読み込んだ画像は、そのときのHDCに依存するので
	//	RGB555に変更されたときは、読み直す必要がある
	bool	m_bLoad256;

	//	気持ち悪いので、参照カウンタは用意しない。

	//	プレーンのサイズ
	int		m_nSizeX;
	int		m_nSizeY;

	//	bitmapファイル名
	string	m_strBitmapFile;				// ファイル名保存しとかなくっちゃ
		//	ビットマップを読み込んでいないときは、ファイル名は空である

	//////////////////////////////////////////////////////////////////////////
	//	サーフェースの復帰処理
	//	リストアは内部的に呼び出される（ユーザーは通常、直接呼び出すことは無い）
	virtual LRESULT	Restore();
	//	リストア処理を書くには、この関数をオーバーライドする（通常、その必要は無い）

	virtual LRESULT OnDraw(){ return 0; }
	//	プレーンの復元のためこれが呼び出される（これをオーバーライドする）

	virtual LRESULT	InnerLoad(const string& strFileName);
	virtual LRESULT InnerCreateSurface(int sx,int sy,bool bYGA=false,bool bSecondary256=false);

	//	サイズ指定でプレーン作成
	LRESULT InnerCreateMySurface(int sx,int sy,int nSurfaceType,bool bClear=true);
	/**
		bClear == trueならば、画面を消す(Clearする)　false
		もし、直後にサーフェースにデータを読み込むことが
		わかっているのならばクリアする必要は無い
	*/

	LRESULT InnerRestoreSurface();		//	サーフェースのリストア処理（内部用）
	LRESULT	UpdateSurfaceInfo();		//	サーフェース情報を更新する（内部用）
	//	呼び出し元はGetSurface()が不正でないことを保証すべき

	//////////////////////////////////////////////////////////////////////////


	bool	m_bUseSystemMemory;			//	強制的にシステムメモリを使うオプション
//	bool	m_bUseDirectDrawSurface;	//	強制的にDirectDrawSurfaceを使うオプション

	ISurfaceRGB m_FillColor;			//	矩形クリアする色
	DWORD	m_dwFillColor;				//	その色を現在のサーフェースに変換したもの

	//////////////////////////////////////////////////////////////////////////

	//	サーフェース情報
	bool	m_bYGA;					//	YGA画像であるか
	bool	m_bYGAUse;				//	強制的にα付きサーフェースを作成する
	bool	m_bAutoRestore;			//	自動リストアするのか
	bool	m_bMySurface;			//	自分でnewしたSurfaceか？
	bool	m_bSecondary256;		//	256色用のセカンダリ
									//	（このサーフェースは256色モードでもRGB555ではなく256色）
	bool	m_bSecondary256DIB;		//	256色用のセカンダリ
									//	（このサーフェースは、RGB555かつDIBSection）
	bool	m_bOwnerDraw;
	//	オーナードロー(これにしておけばRestoreが呼び出されない。Primary,Secondaryはこれ)
	bool	m_bPrimary;				//	プライマリサーフェース(これはlock出来ないことがあるので特殊)

	//////////////////////////////////////////////////////////////////////////

	//	全プレーンへのインスタンスのチェイン
	//	このチェインにはPrimary,Secondaryも含むので注意が必要
//	static set<CFastPlane*>	m_lpPlaneList;

	//	内部YGA画像の読み込み用
	LRESULT InnerLoadYGA(const string& strFilename);

protected:
//	static DWORD DDColorMatch(LPDIRECTDRAWSURFACE pdds, COLORREF rgb); // 同じ色を探す
//	static DWORD DDGetPixel(LPDIRECTDRAWSURFACE pdds,int x,int y); // 特定の点の色を調べる
	//	↑このサーフェースはDirectDrawSurfaceとは限らないので、これでは使いものにならない！ヽ(`Д´)ノ

//	static void	InnerGetBpp();	//	bpp managerのリセット
//	static int	GetBpp();		//	bppの取得

	CFastDraw*	m_pFastDraw;
	static ThreadLocal<CFastDraw*>	m_pDefaultFastDraw;

	// FastDrawは、SetFastDrawで設定されたものを返す。設定されていなければディフォルトのものを返す
	CFastDraw* GetMyFastDraw() const {
		if (m_pFastDraw!=NULL) return m_pFastDraw;
		return GetDefaultFastDraw();
	}

	bool m_bNowRestoring;	//	リストア中かのフラグ
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif	// USE_FastDraw

#endif	// ifdef __yaneFastDraw_h__

