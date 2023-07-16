//
//	Device Independent Bitmap
//

#ifndef __yaneDIBitmap_h__
#define __yaneDIBitmap_h__


namespace yaneuraoGameSDK3rd {
namespace Draw {

class CSurfaceInfo;

class IDIBitmap {
public:
	virtual LRESULT Load(const string& strFileName,int nBpp=24)=0;
	virtual LRESULT Blt(HDC hSrcDC,LPCRECT lpRect,int nBpp=24)=0;
	virtual LRESULT Save(const string& strFileName,LPCRECT lpRect=NULL)=0;
	virtual LRESULT CreateSurface(int nSizeX,int nSizeY,int nBpp=24,bool bAlpha=false)=0;
	virtual LRESULT Clear(LPRECT pRect=NULL)=0;
	virtual LRESULT	Release()=0;

	virtual HDC		GetDC()=0;
	virtual HBITMAP GetHBITMAP()=0;
	virtual LRESULT	GetSize(LONG& x,LONG& y)=0;
	virtual BYTE*	GetPtr()=0;

	virtual CSurfaceInfo* GetSurfaceInfo()=0;

	virtual ~IDIBitmap(){}
};

class CDIBitmap : public IDIBitmap {
/**
	DIB(Device Independent Bitmap：デバイスに依存しないビットマップ)を
	管理します。
*/
public:
	///	画像の読み込み
	virtual LRESULT Load(const string& strFileName,int nBpp=24);
	/**
		Loadはファイルからサーフェースに読み込む。
		Saveはサーフェースをファイルに書き込みます。
　		読み込みはCGraphicLoaderクラスを使用するので、
		BMP,GIF,JPEG,SPI等の利用が可能になります。
		nBppを指定すると、そのbpp(bits per pixel)で読み込みます。
		書き込みは24bppのビットマップになります。
		lpRectはファイルに保存したいサーフェースの矩形領域を指定します。
		指定がなければ全域になります。

		使用例）
	　　::SetPixel(dib.GetDC(),0,0,RGB(100,200,150));
			// (0,0)の座標にRGB(100,200,150)の点を打つ

		参照→　::GetPixel,COLORREF,GetRValue,GetGValue,GetBValue

	*/

	virtual LRESULT Blt(HDC hSrcDC,LPCRECT lpRect,int nBpp=24);
	/**
		hDCで指定される、ある矩形領域lpRectをこのCDIBitmapに読み込む。
		このあと、Saveすることによって、ビットマップに保存してやることが
		出来る。つまりHDCさえ獲得できれば、この関数とSaveでビットマップに
		保存することが出来る。
		nBppを指定すると、そのbpp(bits per pixel)でサーフェースを作成して
		読み込みます。速度は遅いです。
	*/

	virtual LRESULT Save(const string& strFileName,LPCRECT lpRect=NULL);
	/**
		サーフェースのファイルへの保存
		指定した矩形を保存します。lpRect==NULLならば、サーフェース全域。
	*/

	virtual LRESULT CreateSurface(int nSizeX,int nSizeY,int nBpp=24,bool bAlpha=false);
	/**
		サーフェースを指定サイズ、指定のBPP(BitParPixel)で作成します。
		サーフェースはクリアされません。

		bAlpha==trueにしておくと、GetSurfaceInfoで取得できる
		SurfaceTypeがαサーフェースの番号になって返ってくるので
		(ただし、nBpp==32を指定しなければならない)
		そのあと、CSurfaceInfoのBltterでAlphaサーフェースのごとく
		転送することが出来る
	*/

	virtual LRESULT Clear(LPRECT pRect=NULL);
	/**
		サーフェースのクリア
	*/

	virtual LRESULT	Release();
	/**
		サーフェースを解放します。デストラクタで解放されるので、
		明示的に指定する必要はありません。
	*/

	//	LoadBitmap/CreateSurface以降、いつで
	virtual HDC		GetDC() { return m_hDC; }
	virtual HBITMAP GetHBITMAP() { return m_hBitmap; }
	/**
		HDC、HBITMAPを返します。UseDIBSection(true)としておいた場合は
		（defaultではfalse）、LoadBitmap/CreateSurface以降、いつでも
		HDC,HBITMAPの取得は可能です。
		これを用いてピクセル操作をすると良いです。
	*/

	virtual LRESULT	GetSize(LONG& x,LONG& y);
	/**
		サーフェースのサイズを取得します。
	*/

	virtual BYTE*	GetPtr() { return (BYTE*)m_lpdwSrc; }
	/**
		Load/CreateSurfaceで確保されたイメージへのポインタが得られます。
		(CreateDIBSectionで得られたもの)
		DIBなので、イメージの各ラインの先頭は、DWORDでアラインされているので
		注意が必要。ただし上下は転置していません。
	*/

	virtual CSurfaceInfo* GetSurfaceInfo();
	/**
		class CSurfaceInfo さえあれば、こいつのBltterを用いて
		どんな転送でも可能になる
	*/

	/**
		サイズ取得のためのproperty
	*/
	ULONG GetWidth() const { return m_nSizeX; }
	ULONG GetHeight() const { return m_nSizeY; }

	CDIBitmap();
	virtual ~CDIBitmap();

protected:
	DWORD*	m_lpdwSrc;		//	ソースバッファ
	LONG	m_nSizeX;
	LONG	m_nSizeY;
	int		m_nBpp;
	HBITMAP m_hBitmap;
	HDC		m_hDC;

	smart_ptr<CSurfaceInfo> m_vSurfaceInfo;
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
