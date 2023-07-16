//
//	yaneSurface.h
//		サーフェース基底クラス
//
//	yaneSurface1.cpp … 基本実装
//	yaneSurface2.cpp … 通常Blt系実装
//	yaneSurface3.cpp … 通常Effect系実装
//	yaneSurface4.cpp … モーフィング系実装

#ifndef __yaneSurface_h__
#define __yaneSurface_h__


/////////////////////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Draw {

class ISurfaceLocker {
/**
	SurfaceをLockする機構をポリモーフィックに提供する

	class CSurfaceLockerGuard も参照のこと。
*/
public:
	/**
		これらの関数は実際はconstでは無いが、転送元サーフェースをlock～unlockするのは
		転送元サーフェースを書き換えているわけではないので、サーフェースのconst性は
		保てていると考える。よって、Lock～Unlockはconstと考えることにする。
	*/
	virtual LRESULT Lock() const = 0;
	virtual LRESULT Unlock() const = 0;
	virtual ~ISurfaceLocker(){}
};

class INullSurfaceLocker : public ISurfaceLocker {
///	class ISurfaceLocker の Null Device
public:
	virtual LRESULT Lock() const { return 0; }
	virtual LRESULT Unlock() const { return 0; }
};

class IDisableSurfaceLocker : public ISurfaceLocker {
///	class ISurfaceLocker の Disable Device
/**
	Lockが不可能（であるかも知れない）DirectDrawSurfaceの
	プライマリサーフェース等は、こいつを割り当てておく．．とか？
*/
public:
	virtual LRESULT Lock() const { return 1; }
	virtual LRESULT Unlock() const { return 1; }
};

class CSurfaceLockerGuard {
/**
	デストラクタでUnlockを呼び出すだけのオブジェクト
	CSurfaceInfo::GetPixel で実際に使っているので、そちらも
	参照のこと。
*/
public:
	CSurfaceLockerGuard(const ISurfaceLocker*pLocker)
		: m_pSurfaceLocker(pLocker) {}
	CSurfaceLockerGuard(const smart_ptr<ISurfaceLocker>& vLocker)
		: m_pSurfaceLocker(vLocker.get()) {}
	~CSurfaceLockerGuard(){ m_pSurfaceLocker->Unlock(); }
private:
	const ISurfaceLocker* m_pSurfaceLocker;
};

/////////////////////////////////////////////////////////////////////

///	ISurfaceRGBはDWORDっちゅーことで
typedef DWORD ISurfaceRGB;

class ISurface;
class CSurfaceInfo : public ISurfaceLocker {
/**
	Lockしたときのサーフェース情報を保持するための構造体
	一応、class CSurfaceLockerGuard が使えるように、
	このクラスは、ISurfaceLockerから派生させておく。
*/
public:

	////////////////////////////////////////////////////
	//	初期化関数
	////////////////////////////////////////////////////
	void	Init(void* lpSurfacePtr,LONG lPitch,const SIZE &size
		,int nSurfaceType = 0) {
		m_lpSurface = lpSurfacePtr; m_lPitch = lPitch; m_size = size,
		m_bInit = true, m_nSurfaceType = nSurfaceType;
	}
	/**
		ここで与えるものは、以下のもの。（あとでも個別に設定できる）
		さらに、lock～unlockに際して、さらに処理が必要ならば
		SetLockerで、それを設定すること。
	
		lpSurfacePtr	:	サーフェースのイメージへのポインタ
			サーフェースのイメージの左上の座標のメモリアドレス
		lPitch			:	１ラインのバイト数
		size			:	Surfaceのサイズ
		nSurfaceType	:	サーフェースの種類

		0:	未調査（OnChangeSurfaceがまだ呼び出されていない)
		1:	不明（以下の以外）

		//	通常のサーフェース
		2:	8bpp
		3:	16(RGB565)
		4:	16(RGB555)
		5:	24(RGB888)
		6:	24(BGR888)
		7:	32(XRGB8888)
		8:	32(XBGR8888)

		//	αサーフェース
		10:	16+a4(ARGB4565)
		11:	16+a4(ARGB4555)
		12:	32(ARGB8888)
		13:	32(ABGR8888)
	*/

	bool	IsInit() const { return m_bInit; }
	void	SetInit(bool b) { m_bInit = b; }

	////////////////////////////////////////////////////
	//	property...
	////////////////////////////////////////////////////

	///	サーフェースの左上のポインタの設定／取得
	void*	GetPtr() const { return m_lpSurface; }
	void	SetPtr(void*lpSurface){ m_lpSurface=lpSurface; }

	///	サーフェースのラスタピッチ
	LONG	GetPitch() const { return m_lPitch; }
	void	SetPitch(LONG lPitch) { m_lPitch = lPitch; }

	///	サーフェースの広さを示す矩形設定／取得
	SIZE	GetSize() const { return m_size; }
	void	SetSize(const SIZE& size) { m_size = size; }

	///	サーフェースの１ピクセルのバイト数
	int		GetPixelSize() const;

	///	ビデオメモリ上のDirectDrawSurfaceをLockしとるんか？
	///	（システムメモリ上のDirectDrawSurfaceならば、
	///	これを無視して、直接読み書きしてまう！）
	bool	IsLocked() const { return m_bLock; }
		///	lockされているか？
	void	SetLock(bool bLock) const {
		CSurfaceInfo* pThis = const_cast<CSurfaceInfo*>(this);
		pThis->m_bLock = bLock;
	}
		///	lockされているかの情報を設定する

	///	Lock～Unlockする
	///	(２重ロックすると、CRuntimeExceptionが発生)
	/**
		これらの関数は実際はconstでは無いが、転送元サーフェースをlock～unlockするのは
		転送元サーフェースを書き換えているわけではないので、サーフェースのconst性は
		保てていると考える。よって、Lock～Unlockはconstと考えることにする。
	*/
	virtual LRESULT Lock() const
		#ifdef USE_EXCEPTION
			throw(CRuntimeException)
		#endif
			;
	virtual LRESULT Unlock() const
		#ifdef USE_EXCEPTION
			throw(CRuntimeException)
		#endif
			;

	/**
		与えられた矩形を、このサーフェースでクリップして、残る矩形を返す
		lpRect==NULLならばこのサーフェース全域
	*/
	RECT	GetClipRect(const LPCRECT lpRect) const;

	///	SurfaceTypeを取得
	int		GetSurfaceType() const { return m_nSurfaceType; }
	void	SetSurfaceType(int n) { m_nSurfaceType = n; }

	///	SurfaceのLockerのSet/Reset
	smart_ptr<ISurfaceLocker> GetLocker() const { return m_vLocker; }
	void SetLocker(const smart_ptr<ISurfaceLocker>& locker) { m_vLocker = locker; }

	///	カラーキーの設定／取得(このサーフェース用にピクセルフォーマットを変換したもの)
	void	SetColorKey(ISurfaceRGB dw){ m_rgbColorKey = dw; }
	ISurfaceRGB	GetColorKey() const { return m_rgbColorKey; }

	///	FillColorの設定／取得(Clearを呼び出したときはこの色でクリアされる)
	void	SetFillColor(ISurfaceRGB dw) { m_rgbFillColor = dw; }
	ISurfaceRGB	GetFillColor() const { return m_rgbFillColor; }

	virtual LRESULT Save(const string& strBitmapFileName,LPCRECT lpRect=NULL);
		///	画像のファイルへの書き出し(lpRectは書き出し領域。NULLならば全域)

	virtual LRESULT SaveYGA(const string& strBitmapFileName,LPCRECT lpRect=NULL,bool bCompress=true);
		///	YGA画像形式でのファイル書き出し(lpRectは書き出し領域。NULLならば全域)
		///	bCompress==trueならばCLZSSで圧縮して保存。ディフォルトでtrue

	CSurfaceInfo(){
		m_bInit			= false; // 初期化されてまへんInit()してくらはい
		m_bLock			= false;
		m_lpSurface		= NULL;
		m_nSurfaceType	=	0;
		SetLocker(smart_ptr<ISurfaceLocker>(new INullSurfaceLocker));
		//	null lockerをdefaultで
		m_rgbColorKey	=	0;
		m_rgbFillColor	=	0;
	}

	///	一般的なBltter & Effector

	///	--- 汎用ピクセル操作(遅い)
	LRESULT GetPixel(int x,int y,ISurfaceRGB&rgba) const;
	/**
		rgbaで返る。非αサーフェースの場合、rgbaのaは0が返る。
		サーフェース範囲外のときは、非0が返る
		Lock～Unlockも行なう。
	*/

	LRESULT	GetMatchColor(ISurfaceRGB rgb,DWORD& dw) const;
	/**
		ある色が、そのサーフェースでどう表現されるかを返す
		非αサーフェースの場合、rgbのα値は無視される
	*/

	enum EBltType {
	///	転送の種類(サーフェースからサーフェースへの転送に対して有効)
	/**
		命名規則については、ISurface::Blt～のところを参照すること。
	*/
		eSurfaceBltFast	,			///	通常Blt。抜き色無し
		eSurfaceBlt		,			///	通常Blt。抜き色有り
		eSurfaceBltAddColorFast,	///	加色合成。抜き色無し。ソースαサーフェース可。
		eSurfaceBltSubColorFast,	///	減色合成。抜き色無し。ソースαサーフェース可。
		eSurfaceBltAddColor,		///	加色合成。抜き色有り
		eSurfaceBltSubColor,		///	減色合成。抜き色有り

		///	以下の４つは追加パラメータpAdditionalParameter[0]でα(fade率)を指定(0-255)
		eSurfaceBltAddColorFastFade,	///	×αしてから加色合成。抜き色無し。ソースαサーフェース可。
		eSurfaceBltSubColorFastFade,	///	×αしてから減色合成。抜き色無し。ソースαサーフェース可。
		eSurfaceBltAddColorFade,		///	×αしてから加色合成。抜き色有り
		eSurfaceBltSubColorFade,		///	×αしてから減色合成。抜き色有り

		eSurfaceBltAlphaFast,			///	αサーフェースを、非α／αサーフェースへ転送

		///	以下の３つは追加パラメータpAdditionalParameter[0]でα(fade率)を指定(0-255)
		eSurfaceBltFastFade,			///	×αしてからBltFast
		eSurfaceBltFade,				///	×αしてからBlt
		eSurfaceBltAlphaFade,			///	×αしてからBltAlpha(転送元がαサーフェースね)
		/// ↑この3つは転送先とブレンドではない。転送先は無視して、強制的に転送元×αを書く
		///	使うことはないと思うので未実装

		///	以下の２つは追加パラメータpAdditionalParameter[0]でα(fade率)を指定(0-255)
		eSurfaceBlendBltFast,			///	通常ブレンド。抜き色無し
		eSurfaceBlendBlt	,			///	通常ブレンド。抜き色有り

		///	以下の２つは追加パラメータpAdditionalParameter[0],[1]でα1,α2(fade率)を指定(0-255)
		eSurfaceBlendConstBltFast,		///	通常ブレンド。ブレンド率固定。抜き色無し
		eSurfaceBlendConstBlt	,		///	通常ブレンド。ブレンド率固定。抜き色有り

	};

	enum EEffectType {
	///　エフェクトの種類(そのサーフェースに対して有効)
		///	このサーフェースで設定しているfillcolor(ISurfaceRGB)でクリア
		eSurfaceFill	,		///	塗りつぶし

		///	pAdditionalParameter[0]で、フェードレートを指定(0-255)
		///	255ならば100%の意味で、画面に変化なし
		eSurfaceFade	,		///	通常Fade

		///	pAdditionalParameter[0]で加色／減色するISurfaceRGBを指定
		eSurfaceAddColor,		///	加色合成
		eSurfaceSubColor,		///	減色合成

		eSurfaceFlush,			///	画面をフラッシュ（反転させる）

		/// pAdditionalParameter[0]で量子化レベルを指定する
		eSurfaceMosaic,			///	モザイク

	};

	struct CBltInfo {
	LPCPOINT	pDstPoint;		///	転送先座標(pDstPoint==NULLならば左上)
	LPCSIZE		pDstSize;		///	転送先サイズ(pSize==NULLならば等倍)
	LPCRECT		pSrcRect;		///	転送元座標(pSrcPoint==NULLならばサーフェース全域)
	LPCRECT		pDstClip;		///	転送先のクリップ
									///	(この範囲からはみ出る範囲には描画されない)
	int			nBasePoint;		// ベース位置
	/**
		（pSrcPointで指定しているのは、転送元の、どの点なのか？）
		0:左上 1:真上 2:右上
		3:真左 4:中央 5:真右
		6:左下 7:真下 8:右下
	*/
	CBltInfo(
		LPCPOINT	_pDstPoint=NULL,
		LPCSIZE		_pDstSize=NULL,
		LPCRECT		_pSrcRect=NULL,
		LPCRECT		_pDstClip=NULL,
		int			_nBasePoint=0
		) : pDstPoint(_pDstPoint),pDstSize(_pDstSize),pSrcRect(_pSrcRect),
			pDstClip(_pDstClip),nBasePoint(_nBasePoint){}
	};

	struct CMorphInfo {
		LPCPOINT lpSrcPoint;	///	転送元座標列
		LPCPOINT lpDstPoint;	///	転送先座標列
		LPCRECT lpClipRect;		///	転送先クリップ矩形
		bool bContinual;		///	境界が接続されるかのように転送するのか
			/// (隣り合わせにモーフィング転送した時の繋ぎ目を調整)
		int nAngle;				///	pointの数。angel数。３ならば３角形。

		CMorphInfo(){} // こっちの場合は、このあとInitを呼び出して初期化すること。
		CMorphInfo(
			LPCPOINT _lpSrcPoint,
			LPCPOINT _lpDstPoint,
			LPCRECT _lpClipRect,
			bool _bContinual,
			int _nAngle
		) : lpSrcPoint(_lpSrcPoint),lpDstPoint(_lpDstPoint),
			lpClipRect(_lpClipRect),bContinual(_bContinual),nAngle(_nAngle){}

		void Init(LPCPOINT _lpSrcPoint,LPCPOINT _lpDstPoint,LPCRECT _lpClipRect,bool _bContinual,int _nAngle){
			lpSrcPoint=_lpSrcPoint; lpDstPoint=_lpDstPoint;
			lpClipRect=_lpClipRect; bContinual=_bContinual; nAngle=_nAngle;
		}

	};

	//------------------------------------------------------------------------
	//	invoke - execution 分離のためのhelper

	static smart_ptr<void> getWrappedPtr(
		CSurfaceInfo::EBltType type, CSurfaceInfo::CBltInfo*& info, DWORD*& pAdditionalParameter);
	//	与えられたポインタの中身をdeep copyして、そのsmart_ptr<void>にwrapして返す
	//	そして、与えられたポインタの各要素(ポインタ含む)が、
	//	そのdeep copyされたほうの要素を指すようにして返す

	static smart_ptr<void> getWrappedPtr(
	CSurfaceInfo::EBltType type, CSurfaceInfo::CMorphInfo*& info, DWORD*& pAdditionalParameter);
	//	与えられたポインタの中身をdeep copyして、そのsmart_ptr<void>にwrapして返す
	//	そして、与えられたポインタの各要素(ポインタ含む)が、
	//	そのdeep copyされたほうの要素を指すようにして返す

	static smart_ptr<void> getWrappedPtr(
	CSurfaceInfo::EEffectType type, LPCRECT& prc, DWORD*& pAdditionalParameter);
	//	与えられたポインタの中身をdeep copyして、そのsmart_ptr<void>にwrapして返す
	//	そして、与えられたポインタの各要素(ポインタ含む)が、
	//	そのdeep copyされたほうの要素を指すようにして返す

	//------------------------------------------------------------------------

	///	回転拡大縮小用のMorphの構造体初期化用
	///	CMorphInfo構造体を、第２パラメータ以下で渡される情報に基づいて初期化する
	LRESULT InitRotateParam(CMorphInfo& info,const CSurfaceInfo* lpSrc, int x,int y, int nAngle,int nRate,int nType, LPCRECT lpSrcRect, LPCRECT lpClipDstRect,
		RECT& rcSrc,POINT aSrcPoint[4],POINT aDstPoint[4]);

	/**
		ISurfaceの同名の関数の下請け関数

		これで、BltとEffectを定義してあるので、派生クラスで
		ハードウェアアクセラレータによって転送するのでなければ、
		こいつに委譲するだけで転送が完了する。
	*/
	LRESULT GeneralBlt(EBltType type,const CSurfaceInfo*pSrc,const CBltInfo* pInfo,DWORD*pAdditionalParameter=NULL);
	LRESULT GeneralEffect(EEffectType type,LPCRECT prc=NULL,DWORD*pAdditionalParameter=NULL);
	LRESULT GeneralMorph(EBltType type,const CSurfaceInfo*pSrc,const CMorphInfo* pInfo,DWORD*pAdditionalParameter=NULL);

	///----- surface用のiterator --------------------------------------------
	class iterator {
	/**
		サーフェースのiteratorの使いかた
		CSurfaceInfo::iterator it = GetSurfaceInfo()->begin();
		while (it!=GetSurfaceInfo()->end()){
			it.SetRGB(r,g,b,a);
			it++;
		}
		この機構により、どんなサーフェースに対しても同じように
		ピクセルを操作できる（遅いけど）
	*/
	public:
		iterator(const CSurfaceInfo*pInfo,int x=0,int y=0)
			: m_pInfo(pInfo),m_nX(x),m_nY(y) {}

		/**
			RGBの設定／取得
				Lock～Unlockは行なわないのでこの外部でLock～Unlockを行なうこと
		*/
		void	SetRGB(BYTE r,BYTE g,BYTE b,BYTE a=0);
		void	SetRGB(ISurfaceRGB rgb){
			SetRGB((BYTE)((rgb>>16)&0xff),(BYTE)((rgb>>8)&0xff),(BYTE)(rgb&0xff),(BYTE)((rgb>>24)&0xff));
		}

		ISurfaceRGB	GetRGB() const;
		///	非αサーフェースに対しては、α値は0がセットされる

		void inc() { m_nX++; if (m_nX >= GetSurfaceInfo()->GetSize().cx) { m_nX=0; m_nY++; }}
		void dec() { m_nX++; if (m_nX<0) { m_nX=GetSurfaceInfo()->GetSize().cx-1; m_nY--; }}

		iterator& operator++() { inc(); return (*this); }
		iterator operator++(int)
		{ iterator _Tmp = *this; inc(); return (_Tmp); }
		iterator& operator--() { dec(); return (*this); }
		iterator operator--(int)
		{ iterator _Tmp = *this; dec(); return (_Tmp); }
		iterator& operator+=(int _N)
		{ m_nX += _N; int cx = m_pInfo->GetSize().cx;
		  m_nY += m_nX%cx; m_nX %= cx; return (*this); }
		iterator& operator-=(int _N)
		{ m_nX -= _N; if (m_nX<0) { int cx = m_pInfo->GetSize().cx;
			int sy = (-m_nX) % cx; m_nY-=sy; m_nX = (m_nX + sy*cx) % cx; }
			return (*this); }
		iterator operator+(int _N) const
		{iterator _Tmp = *this; return (_Tmp += _N); }
		iterator operator-(int _N) const
		{iterator _Tmp = *this; return (_Tmp -= _N); }

		///	比較のためのオペレータ
		bool operator==(const iterator & _X) const
		{ return m_pInfo == _X.m_pInfo && m_nX == _X.m_nX && m_nY == _X.m_nY; }
		bool operator!=(const iterator& _X) const
			{return (!(*this == _X)); }
		bool operator<(const iterator& _X) const
		{return (m_nY < _X.m_nY ||	(m_nY == _X.m_nY && m_nX < _X.m_nX)); }
		bool operator>(const iterator& _X) const
		{return (_X < *this); }
		bool operator<=(const iterator& _X) const
		{return (!(_X < *this)); }
		bool operator>=(const iterator& _X) const
		{return (!(*this < _X)); }

		/// ----	property...

		///	サーフェースのポインタの設定／取得
		void SetSurfaceInfo(const CSurfaceInfo*pInfo) { m_pInfo=pInfo; }
		const CSurfaceInfo* GetSurfaceInfo() const { return m_pInfo; }

		///	現在指定している座標の設定／取得
		void	SetXY(int x,int y) { m_nX = x; m_nY = y; }
		void	GetXY(int&x,int&y) { x = m_nX; y = m_nY; }

	protected:
		const CSurfaceInfo* m_pInfo;	//	iterateしているサーフェース
		int			m_nX,m_nY;	//	指している場所
	};
	iterator begin() const { return iterator(this,0,0); }
	iterator end() const { return iterator(this,0,GetSize().cy); }

private:
	bool	m_bInit;			//	初期化済みか？
	void*	m_lpSurface;		//	このサーフェースの左上のピクセルのポインタ
	LONG	m_lPitch;			//	このサーフェースのピッチ
								//	（次のラスターまでのバイト数）
	SIZE	m_size;				//	このサーフェースのサイズ
	bool	m_bLock;			//	ロック中か？
	int		m_nSurfaceType;		//	サーフェースの種類もここに保持しておこう

	///	(以下は、このサーフェース用にピクセルフォーマットを変換したもの)
	ISurfaceRGB	m_rgbColorKey;		//	このサーフェースのカラーキー
	ISurfaceRGB	m_rgbFillColor;		//	このサーフェースの塗りつぶし時に使う色

	smart_ptr<ISurfaceLocker> m_vLocker;	//	surfaceをlockするためのオブジェクト

};

class ISurface {
/**
	Surfaceの基底クラス。
	⇒	class ISurfaceDefault , class CFastPlane も参考のこと。
*/
public:
	///	typedef DWORD ISurfaceRGB;
	///	このサーフェースでは、共通して、このRGB値で指定する

	static inline ISurfaceRGB makeRGB(DWORD r,DWORD g,DWORD b,DWORD a=0)
		#ifdef USE_EXCEPTION
			throw(CRuntimeException)
		#endif
	/**
		IPlaneで使うためのRGBピクセル値の生成
		：	↑これは、ARGB8888(DIB32と仕様は同じ)の生成
	*/
	{
#ifdef _DEBUG
		if (r>=256 || g>=256 || b>=256 || a>=256){
		#ifdef USE_EXCEPTION
			throw CRuntimeException();
		#else
			r = g = b = a = 0;
		#endif
		}
#endif
		return (a<<24) | (r<<16) | (g<<8) | b ;
	}

	static inline void	getRGB(ISurfaceRGB rgb,BYTE& a,BYTE& r,BYTE& g,BYTE& b)
	/**
		IPlaneで使うためのRGBピクセル値の分解
		：	ARGB8888(DIB32と仕様は同じ)と同じ
	*/
	{
		a = (BYTE)((rgb >> 24) & 0xff);
		r = (BYTE)((rgb >> 16) & 0xff);
		g = (BYTE)((rgb >>	8) & 0xff);
		b = (BYTE)((rgb		 ) & 0xff);
	}

	virtual int GetType() const = 0;
	/**
		サーフェースのタイプを返す(RTTIもどき)
		0:	Null Device
		1:	CFastPlane
		2:	CDIB32
	*/

	//////////////////////////////////////////////////////////////////////
	///	汎用Bltter
	//////////////////////////////////////////////////////////////////////

	virtual LRESULT GeneralBlt(CSurfaceInfo::EBltType type,const CSurfaceInfo*pSrc,const CSurfaceInfo::CBltInfo* pInfo,DWORD*pAdditionalParameter=NULL)
	///	汎化されたBltter
	/**
		type : 転送モード(抜き色転送であるだとか、加色合成であるだとか)
		pSrc : 転送元のCSurfaceInfo
		pInfo: 転送パラメータ
		pAdditionalParameter : 追加で必要となるパラメータへのポインタ
	*/
	{
		///	委譲するだけで良いと思われ
		return GetSurfaceInfo()->GeneralBlt(type,pSrc,pInfo,pAdditionalParameter);
	}

	//////////////////////////////////////////////////////////////////////////
	/// ---- 基本的なBltterは事前に用意しておく
	//////////////////////////////////////////////////////////////////////////

	/**
		Bltterの命名規則：

		例）
			MorphBltAlphaFastFade
			(1)	  (2) (3) (4) (5)

			BlendConstBltFast
			(6)	 (7)  (1) (4)

			(1)...Morph...モーフィング系であることを意味する
			(2)...Blt.....転送関数であることを意味する
			(3)...Alpha...転送元がalphaサーフェースであることを意味する
					これがついていない場合、alphaサーフェースを
					サポートしているかどうかは、そのメソッドによる
			(4)...Fast....抜き色は無視されることを意味する
			(5)...Fade....転送のときにフェード率を指定できることを意味する
				フェード率（減衰率）は、BYTE(0-255)で指定し、
				255を指定すれば100%(減衰無し),0を指定すれば0%
				(このとき、転送元は黒として扱われる)
			(6)...Blend...ブレンド系転送であることを意味する。
					転送先のピクセルとブレンドする（混ぜ合わせる）
					たとえば64(=25%なぜなら255×25%=64)を指定すれば、
					転送元25%と転送先75%の比率でブレンドされる。
			(7)...Const...ブレンドの比率が固定であることを意味する
					ブレンド比率は、BYTE(0-255)をふたつとる。
					そのふたつをα１，α２とすれば、
					転送先　＝　転送元×α１＋転送先×α２
				となる。ただし、α１＋α２≦２５５でなくてはならない
	*/

	LRESULT Blt(const ISurface*pSrc,int x=0,int y=0,LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
	/**
		抜き色有り転送
		x,y		 : 転送先座標
		pDstSize : 転送先サイズ(NULLならば等倍)
		pSrcRect : 転送元矩形
		pDstClip : 転送先クリッピング矩形(この範囲には描画されない)
		nBasePoint:転送先基準位置

		//	class CSurfaceInfo::CBltInfo も参照のこと。
  */
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBlt,pSrc->GetConstSurfaceInfo(),&info);
	}

	///	抜き色無し転送
	LRESULT BltFast(const ISurface*pSrc,int x=0,int y=0,LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
	///	パラメータの意味は、Bltと同じ
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltFast,pSrc->GetConstSurfaceInfo(),&info);
	}

	///	加色合成(抜き色なし)
	///	パラメータの意味は、Bltと同じ
	///	ソースαサーフェース可。
	LRESULT AddColorBltFast(const ISurface*pSrc,int x=0,int y=0,LPCSIZE pDstSize=NULL,
		LPCRECT	 pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltAddColorFast,pSrc->GetConstSurfaceInfo(),&info);
	}
	///	加色合成(抜き色あり)
	///	パラメータの意味は、Bltと同じ
	LRESULT AddColorBlt(const ISurface*pSrc,int x=0,int y=0,LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltAddColor,pSrc->GetConstSurfaceInfo(),&info);
	}
	///	減色合成(抜き色なし)
	///	パラメータの意味は、Bltと同じ
	///	ソースαサーフェース可。
	LRESULT SubColorBltFast(const ISurface*pSrc,int x=0,int y=0,LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltSubColorFast,pSrc->GetConstSurfaceInfo(),&info);
	}
	///	減色合成(抜き色あり)
	///	パラメータの意味は、Bltと同じ
	LRESULT SubColorBlt(const ISurface*pSrc,int x=0,int y=0,LPCSIZE pDstSize=NULL,
		LPCRECT	 pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltSubColor,pSrc->GetConstSurfaceInfo(),&info);
	}

	/// -------- 以下、α付きの加色合成。
	///	☆　alphaはDWORDで渡しているが、BYTE(0-255)ね。

	///	×α加色合成(抜き色なし)
	///	パラメータの意味は、Bltと同じ
	///	ソースαサーフェース可。
	LRESULT AddColorBltFastFade(const ISurface*pSrc,int x=0,int y=0,DWORD nAlpha=255,
		LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltAddColorFastFade,pSrc->GetConstSurfaceInfo(),&info,&nAlpha);
	}
	///	×α加色合成(抜き色あり)
	///	パラメータの意味は、Bltと同じ
	LRESULT AddColorBltFade(const ISurface*pSrc,int x=0,int y=0,DWORD nAlpha=255,
		LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltAddColorFade,pSrc->GetConstSurfaceInfo(),&info,&nAlpha);
	}
	///	×α減色合成(抜き色なし)
	///	パラメータの意味は、Bltと同じ
	///	ソースαサーフェース可。
	LRESULT SubColorBltFastFade(const ISurface*pSrc,int x=0,int y=0,DWORD nAlpha=255,
		LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltSubColorFastFade,pSrc->GetConstSurfaceInfo(),&info,&nAlpha);
	}
	///	×α減色合成(抜き色あり)
	///	パラメータの意味は、Bltと同じ
	LRESULT SubColorBltFade(const ISurface*pSrc,int x=0,int y=0,DWORD nAlpha=255,
		LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltSubColorFade,pSrc->GetConstSurfaceInfo(),&info,&nAlpha);
	}

	///	αサーフェースからの画像の転送系
	///	パラメータの意味は、Bltと同じ
	LRESULT BltAlphaFast(const ISurface*pSrc,int x=0,int y=0,LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltAlphaFast,pSrc->GetConstSurfaceInfo(),&info);
	}

	///	パラメータの意味は、Bltと同じ
	///	BlendBltFastとBltFastFadeは同じ効果
	LRESULT BlendBltFast(const ISurface*pSrc,int x=0,int y=0,DWORD nAlpha=255,
		LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBlendBltFast,pSrc->GetConstSurfaceInfo(),&info,&nAlpha);
	}
	///	パラメータの意味は、Bltと同じ
	///	BlendBltとBltFadeは同じ効果
	LRESULT BlendBlt(const ISurface*pSrc,int x=0,int y=0,DWORD nAlpha=255,
		LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBlendBlt,pSrc->GetConstSurfaceInfo(),&info,&nAlpha);
	}
	///	パラメータの意味は、Bltと同じ
	LRESULT BlendConstBltFast(const ISurface*pSrc,int x=0,int y=0,DWORD nAlpha1=255,DWORD nAlpha2=255,
		LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		DWORD anAlpha[] = { nAlpha1,nAlpha2 };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBlendConstBltFast,pSrc->GetConstSurfaceInfo(),&info,&anAlpha[0]);
	}
	///	パラメータの意味は、Bltと同じ
	LRESULT BlendConstBlt(const ISurface*pSrc,int x=0,int y=0,DWORD nAlpha1=255,DWORD nAlpha2=255,
		LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		DWORD anAlpha[] = { nAlpha1,nAlpha2 };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBlendConstBlt,pSrc->GetConstSurfaceInfo(),&info,&anAlpha[0]);
	}

	/////////////////////////////////////////////////////////////
	///	補助Bltter

	///	自然な転送(Blt,yga画像ならばBltAlphaFast)
	LRESULT BltNatural(const ISurface* pSrc,int x,int y,LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0) {
		if (!pSrc->IsAlpha()) {
			return Blt(pSrc,x,y,pDstSize,pSrcRect,pDstClip,nBasePoint);
		} else {
			return BltAlphaFast(pSrc,x,y,pDstSize,pSrcRect,pDstClip,nBasePoint);
		}
	}
	LRESULT BltNatural(const ISurface* pSrc,int x,int y,BYTE nFadeRate,LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0) {
		if (nFadeRate==255) {
			return BltNatural(pSrc,x,y,pDstSize,pSrcRect,pDstClip,nBasePoint);
		}
		if (!pSrc->IsAlpha()) {
			return BlendBlt(pSrc,x,y,nFadeRate,pDstSize,pSrcRect,pDstClip,nBasePoint);
		} else {
			return BlendBltFast(pSrc,x,y,nFadeRate,pDstSize,pSrcRect,pDstClip,nBasePoint);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	///	--- Surfaceのエフェクタ

	virtual LRESULT GeneralEffect(CSurfaceInfo::EEffectType type,LPCRECT prc=NULL,DWORD*pAdditionalParameter=NULL)
	///	汎化されたEffector
	/**
		type : 転送モード(通常Fade転送であるだとか、加色合成であるだとか)
		prc	 : エフェクトをかける矩形範囲(NULLならば全域)
		pAdditionalParameter : 追加で必要となるパラメータへのポインタ
			type : eEffectFade ならば、[0]は Fade率(BYTE:0-255)
			type : eAddColor,eSubColor ならば[0]はその定数(ISurfaceRGB)
	*/
	{
		return GetSurfaceInfo()->GeneralEffect(type,prc,pAdditionalParameter);
	}

	//////////////////////////////////////////////////////////////////////////
	/// ---- 基本的なeffecterは事前に用意しておく
	///	これらは、そのサーフェースに対してエフェクトをかける
	///	LPCRECT prcは、NULLならばサーフェース全域
	//////////////////////////////////////////////////////////////////////////

	///	フェード alphaはフェード率(0-255)
	virtual LRESULT FadeEffect(BYTE alpha,LPCRECT prc=NULL){
		DWORD dw = alpha; // 少し馬鹿らしいけど、まあいいや
		return GeneralEffect(CSurfaceInfo::eSurfaceFade,prc,&dw);
	}

	///	加色合成
	virtual LRESULT AddColorFast(ISurfaceRGB c,LPCRECT prc=NULL){
		return GeneralEffect(CSurfaceInfo::eSurfaceAddColor,prc,(DWORD*)&c);

	}

	///	減色合成
	virtual LRESULT SubColorFast(ISurfaceRGB c,LPCRECT prc=NULL){
		return GeneralEffect(CSurfaceInfo::eSurfaceSubColor,prc,(DWORD*)&c);
	}

	///	モザイク
	virtual LRESULT MosaicEffect(DWORD nLevel,LPCRECT prc=NULL){
		return GeneralEffect(CSurfaceInfo::eSurfaceMosaic,prc,(DWORD*)&nLevel);
	}

	///	flush
	virtual LRESULT FlushEffect(LPCRECT prc=NULL){
		return GeneralEffect(CSurfaceInfo::eSurfaceFlush,prc);
	}

	//////////////////////////////////////////////////////////////////////////
	///	Rotate

	virtual LRESULT GeneralMorph(CSurfaceInfo::EBltType type,const CSurfaceInfo*pSrc,const CSurfaceInfo::CMorphInfo* pInfo,DWORD*pAdditionalParameter=NULL)
	///	汎化されたBltter
	/**
		type : 転送モード(抜き色転送であるだとか、加色合成であるだとか)
		pSrc : 転送元のCSurfaceInfo
		pInfo: 転送パラメータ
		pAdditionalParameter : 追加で必要となるパラメータへのポインタ
	*/
	{
		///	委譲するだけで良いと思われ
		return GetSurfaceInfo()->GeneralMorph(type,pSrc,pInfo,pAdditionalParameter);
	}

	LRESULT RotateBlt(const ISurface* lpSrc, int x,int y, int nAngle=0,int nRate=1<<16,int nType=0, LPCRECT lpSrcRect=NULL, LPCRECT lpClipDstRect=NULL){
		CSurfaceInfo::CMorphInfo info;
		RECT rcSrc;
		POINT aSrcPoint[4];
		POINT aDstPoint[4];
		if (GetSurfaceInfo()->InitRotateParam(info,lpSrc->GetConstSurfaceInfo(),x,y,nAngle,nRate,nType,lpSrcRect,lpClipDstRect,rcSrc,aSrcPoint,aDstPoint)!=0) return 1;
		return GeneralMorph(CSurfaceInfo::eSurfaceBlt,lpSrc->GetConstSurfaceInfo(), &info);
	}
	LRESULT RotateBltFast(const ISurface* lpSrc, int x,int y, int nAngle=0,int nRate=1<<16,int nType=0, LPCRECT lpSrcRect=NULL, LPCRECT lpClipDstRect=NULL){
		CSurfaceInfo::CMorphInfo info;
		RECT rcSrc;
		POINT aSrcPoint[4];
		POINT aDstPoint[4];
		if (GetSurfaceInfo()->InitRotateParam(info,lpSrc->GetConstSurfaceInfo(),x,y,nAngle,nRate,nType,lpSrcRect,lpClipDstRect,rcSrc,aSrcPoint,aDstPoint)!=0) return 1;
		return GeneralMorph(CSurfaceInfo::eSurfaceBltFast,lpSrc->GetConstSurfaceInfo(), &info);
	}
	LRESULT RotateAlphaBltFast(const ISurface* lpSrc, int x,int y, int nAngle=0,int nRate=1<<16,int nType=0, LPCRECT lpSrcRect=NULL, LPCRECT lpClipDstRect=NULL){
		CSurfaceInfo::CMorphInfo info;
		RECT rcSrc;
		POINT aSrcPoint[4];
		POINT aDstPoint[4];
		if (GetSurfaceInfo()->InitRotateParam(info,lpSrc->GetConstSurfaceInfo(),x,y,nAngle,nRate,nType,lpSrcRect,lpClipDstRect,rcSrc,aSrcPoint,aDstPoint)!=0) return 1;
		return GeneralMorph(CSurfaceInfo::eSurfaceBltAlphaFast,lpSrc->GetConstSurfaceInfo(), &info);
	}

	//////////////////////////////////////////////////////////////////////////
	///	--- Surfaceとして、常識的に必要なもの

	///	--- 抜き色の設定／取得
	virtual	LRESULT SetColorKey(ISurfaceRGB rgb);
	virtual ISurfaceRGB GetColorKey() const;

	virtual LRESULT SetColorKeyPos(int x,int y);
	///　(x,y)の点を透過キーに設定する

	virtual LRESULT SetColorKeyRGB(int r,int g,int b){ return SetColorKey(makeRGB(r,g,b)); }
	///	RGBで抜き色を指定する。rgbはそれぞれ0～255

	///	--- サーフェースのサイズ設定／取得
	virtual LRESULT GetSize(int& x,int& y) const =0;
	///		未生成のサーフェースならばGetSizeは、-1を返す（そのときx==y==0）
	virtual LRESULT SetSize(int x,int y)=0;
	/**
		SetSizeは、もし現在サイズと違うのであれば、あらたにCreateされる
	*/

	///		サーフェースの種類を返す
	virtual int GetSurfaceType() const
	{ return GetConstSurfaceInfo()->GetSurfaceType(); }

	///		αサーフェースかどうかを返す
	bool	IsAlpha() const { return GetSurfaceType()>=10; }

	///////////////////////////////////////////////////////
	///	サーフェースに対してビットマップデータを読み込みを
	///	サポートする補助メソッド
	/**
		このサイズの指定タイプのサーフェースを作成する
		(主に、内部的に利用する)
		(sx,sy) : サーフェースのサイズ
		ここで指定するnTypeは、作成するサーフェースのタイプ(CSurfaceInfoと同じ)
		（ここで作成されたサーフェースは、クリアされない）
	*/
	virtual LRESULT CreateSurfaceByType(int sx,int sy,int nType)=0;

	/**
		指定されたnTypeでサーフェースを、上記のCreateSurfaceByTypeで作成し、
		ビットマップデータを読み込む。(YGA対応)
		YGA画像のときは、もしnTypeで指定されたものがαサーフェースで無いならば、		自動的にそれに対応するαサーフェースにする
	*/
	virtual LRESULT LoadByType(const string& strBitmapName,int nType);

	///	virtual void ClearMSB();
	/*
		ビデオカードのバグ対策で最上位バイトを潰す
		(HDC経由でXRGB8888,XBGR8888に読み込んだときに、クリアする必要がある)
		⇒　自前で読み込むことにした。読み込めないときは、24bppのDIBを作成して
		そこから読み込む。よって、上記のバグの影響は受けないのでこのメソッドは
		廃止する
	*/

	///////////////////////////////////////////////////////

	virtual LRESULT		Clear(LPCRECT lpRect=NULL);			///	矩形クリア
	virtual void		SetFillColor(ISurfaceRGB c);		///	Clearする色を指定する(Default==RGB(0,0,0))
	virtual ISurfaceRGB	GetFillColor() const;				///	Clearする色の取得
		///	⇒ただし、α付きサーフェースに対するClearは、
		///		α == 0になることが保証されるものとする
	/**
		ここで、設定されたFillColorは、再度設定されるまで有効。
		ディフォルトはRGB(0,0,0) すなわち黒である。
	*/

	///////////////////////////////////////////////////////

	///　--- ビットマップ関連
	virtual LRESULT Load(const string& strBitmapFileName){
		///	画像の読み込み
		return 0;
	}
	virtual LRESULT Save(const string& strBitmapFileName,LPCRECT lpRect=NULL){
		///	画像のファイルへの書き出し(lpRectは書き出し領域。NULLならば全域)
		return GetSurfaceInfo()->Save(strBitmapFileName,lpRect);
	}
	virtual LRESULT SaveYGA(const string& strBitmapFileName,LPCRECT lpRect=NULL,bool bCompress=true){
		///	YGA画像形式でのファイル書き出し(lpRectは書き出し領域。NULLならば全域)
		///	bCompress==trueならばCLZSSで圧縮して保存。ディフォルトでtrue
		return GetSurfaceInfo()->SaveYGA(strBitmapFileName,lpRect,bCompress);
	}

	virtual LRESULT	Release(){ return 0; }
	///	読み込んだ画像の解放

	//////////////////////////////////////////////////////////////////////////
	///	サーフェース情報
	///	(これさえあれば、情報を取得して転送することが出来る)

		///	中身を変更しないバージョン
	virtual const CSurfaceInfo* GetConstSurfaceInfo() const
	{ return m_vSurfaceInfo.get(); }

		///	中身を変更するバージョン
	virtual CSurfaceInfo* GetSurfaceInfo()
	{ return m_vSurfaceInfo.get(); }

	//////////////////////////////////////////////////////////////////////////

	virtual smart_ptr<ISurface> clone()=0;
	/**
		同じ型のオブジェクトを生成して返す。
		ただし、サーフェースの内容はコピーされるわけではない。
	*/

	ISurface();
	virtual ~ISurface() {}	//	merely place holder

protected:
	///	サーフェース情報を持った構造体
	smart_ptr<CSurfaceInfo>	m_vSurfaceInfo;
	smart_ptr<ISurfaceLocker> m_vSurfacelocker;

	// ---------------------- 透過キー関連 ----------------------------
	//	default(ResetColorKeyの動作)ではm_bUsePosColorKey==true
	//		m_nColorKeyX=m_nColoyKeyY=0
	//	すなわち、左上のドットが抜き色
	void	ResetColorKey();
	//	カラーキーのリセット(サーフェースの作成時にこれを呼び出すと良い)
	bool	m_bUsePosColorKey;			//　true:位置指定型のColorKey
										//	false:色指定型のColorKey
	int		m_nColorKeyX,m_nColorKeyY;	//　位置指定型　透過カラーキー
	ISurfaceRGB m_rgbColorKey;			//	カラーキーの色
	/*
		ただし、位置指定型の場合、このサーフェースでは色深度が不足していて
		本来の色とは限らないので、このあと画面の色深度を深くしたときに、
		その位置にあるピクセルは異なる色になってしまう可能性がある。そこで
		位置指定型カラーキーの場合にそなえて、サーフェースの色深度が変更に
		なったときに、再度、以下のUpdateColorKeyメソッドを呼び出さなくては
		ならないわけである。
	*/
	LRESULT	UpdateColorKey();	//	復帰用（内部的に使用）


	///	あるサーフェースに対応するYGAサーフェースを返す
	int	GetYGASurfaceType(int nSurfaceType) const;

};	//	とりあえず、それだけあれば十分っしょ＾＾；

class ISurfaceFactory {
/*
	ISurfaceのfactory
*/
public:
	virtual smart_ptr<ISurface> CreateInstance() = 0;
	virtual ~ISurfaceFactory(){}
};

/////////////////////////////////////////////////////////////////////////////

class CSurfaceNullDevice : public ISurface {
/**
	class IPlane のNullDevice
	要するに、何もしない。virtual proxy(smart_ptr<ISurface>)から使うときのため。
*/
public:
	virtual LRESULT GeneralBlt(CSurfaceInfo::EBltType type,const CSurfaceInfo* pSrc,
		const CSurfaceInfo::CBltInfo* lpDst,DWORD*pAdditionalParameter=NULL) { return 0; }
	virtual LRESULT GeneralEffect(CSurfaceInfo::EEffectType type,LPCRECT prc=NULL,
		DWORD*pAdditionalParameter=NULL) { return 0; }
	virtual int GetType() const { return 0; }
	virtual LRESULT GetSize(int& x,int& y) const { return 0; }
	virtual LRESULT SetSize(int x,int y) { return 0; }
	virtual	LRESULT SetColorKey(ISurfaceRGB rgb) { return 0; }
	virtual ISurfaceRGB GetColorKey() const { return 0; }
	virtual LRESULT SetColorKeyPos(int x,int y){ return 0; }
	virtual smart_ptr<ISurface> clone() { return smart_ptr<ISurface>(new CSurfaceNullDevice);}
	virtual LRESULT		Clear(LPCRECT lpRect=NULL){ return 0;}
	virtual void		SetFillColor(ISurfaceRGB c) {}
	virtual ISurfaceRGB	GetFillColor() const { return 0;}
	virtual LRESULT CreateSurfaceByType(int sx,int sy,int nType) { return 0; }
};

/////////////////////////////////////////////////////////////////////////////

struct CYGAHeader {
///		YGAのヘッダー
	DWORD dwIdentifier;
	DWORD dwSizeX;
	DWORD dwSizeY;
	DWORD bCompress;
	DWORD dwOriginalSize;
	DWORD dwCompressSize;
	CYGAHeader() { dwIdentifier = 0x616779; /* "yga" */ }
};

/////////////////////////////////////////////////////////////////////////////

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
