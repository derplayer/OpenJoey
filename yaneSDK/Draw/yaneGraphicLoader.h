//
//	yaneGraphicLoader.h
//
//		BMP,JPEG,GIF,...や必要に応じてSusie-Plug inを自動的に読み込むクラス
//		programmed by yaneurao(M.Isozaki)	'00/05/15
//		reprogrammed by yaneurao(M.Isozaki) '00/06/21
//
//			SPI読み込み部分のサンプルソースをくださった、にっくるさんに感謝！
//
//		yga読み込みにも対応 '02/07/25
//

#ifndef __yaneGraphicLoader_h__
#define __yaneGraphicLoader_h__


#include <olectl.h>	//	IPicture

#include "../Auxiliary/yaneFile.h"

///////////////////////////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CGraphicLoader;
class CSurfaceInfo;

class IGraphicLoader {
public:
	virtual LRESULT LoadPicture(const CFile& file)=0;
	virtual LRESULT GetSize(LONG& x,LONG& y) const=0;
	virtual LRESULT IsYGA() const = 0;

//	virtual LRESULT Render(DWORD* p,LONG lPitch)=0;
	virtual LRESULT Render(CSurfaceInfo*pInfo)=0;
	virtual LRESULT	Render(HDC hdc)=0;
	virtual LRESULT	ReleasePicture()=0;
	virtual LRESULT	GetPalette(PALETTEENTRY pal[256])=0;
	virtual ~IGraphicLoader(){}

	//	うひゃー。かっちょええ～＾＾；
	static factory_permutation_i<IGraphicLoader,CGraphicLoader>* GetGraphicFactory() { return& m_factory; }
private:
	static factory_permutation_i<IGraphicLoader,CGraphicLoader> m_factory;
};

class CGraphicLoader : public IGraphicLoader {
public:
	virtual LRESULT LoadPicture(const CFile& file);		///	これで読み込む返し値が0ならば成功

	virtual LRESULT GetSize(LONG& x,LONG& y) const;		///	これで読み込んだ画像のサイズを取得

	virtual LRESULT IsYGA() const;
	///	読み込んだ画像が、αサーフェースなのかを返す

	virtual LRESULT Render(CSurfaceInfo*pInfo);
	///	CSurfaceInfoを渡して、そこに描画してもらう

	virtual LRESULT	Render(HDC hdc);					///	hdcを渡して、そこに描画してもらう
	virtual LRESULT	ReleasePicture();					///	これで解放する

	virtual LRESULT	GetPalette(PALETTEENTRY pal[256]);	///	パレットの取得

	static LRESULT	ClearPalette(BITMAPINFO* pBI);
	/**
		BITMAPINFOが8bppのbmpをあらわすならば、その後段にある
		RGBQUADを初期化する。
	*/

	/**
		SPI(susie plug-in)の検索pathの設定／取得
		defaultでは"spi"（フォルダ名）
		よって、jpgを読み込むときは、"spi/ifjpg.spi"という
		プラグインを探しに行く。
	*/
	static	void SetSPIPath(const string& strPath) { m_strSPIPath = strPath; }
	static	string GetSPIPath() { return m_strSPIPath; }

	//	PrototypeFactoryで明示的に作ってね＾＾；
	CGraphicLoader();
	virtual ~CGraphicLoader();

protected:

	virtual LRESULT LoadPictureByOwn1(const CFile& file);	//	bmp
	virtual LRESULT LoadPictureByOwn2(const CFile& file);	//	yga
	virtual LRESULT LoadPictureByOLE(const CFile& file);	//	jpeg,gif
	virtual LRESULT LoadPictureBySPI(const CFile& file);	//	using by SPI

	//	for LoadPictureByOwn
	BITMAPFILEHEADER*	m_lpBF;
	BITMAPINFOHEADER*	m_lpBI;
	void*				m_lpBits;

	//	for Load YGA Image By Own
	bool	m_bYGA;			//	読み込んだのはyga画像なのか？
	int		m_nYGA_Xsize;	//	yga画像のxサイズ
	int		m_nYGA_Ysize;	//	yga画像のyサイズ

	///	このrenderingは、内部的に呼び出すだけ
	virtual LRESULT InnerRenderBmp(CSurfaceInfo*pInfo);	// BMPのRender
	virtual LRESULT InnerRenderYga(CSurfaceInfo*pInfo);	// YGAのRender

///////////////////////////////////////////////////////////////////////////

	//	for OLELoadPicture
	IPicture*	m_lpiPicture;

	//	for SPI
///////////////////////////////////////////////////////////////////////////
typedef struct SpiPictureInfo {
	long left,top;					//	画像を展開する位置
	long width;						//	画像の幅(pixel)
	long height;					//	画像の高さ(pixel)
	WORD x_density;					//	画素の水平方向密度
	WORD y_density;					//	画素の垂直方向密度
	short colorDepth;				//	１画素当たりのbit数
	HLOCAL hInfo;					//	画像内のテキスト情報
} SpiPictureInfo, *LPSpiPictureInfo;

typedef	int (PASCAL *SpiFuncIsSupported)(LPSTR, DWORD);
typedef	int (PASCAL *SpiFuncGetPictureInfo)(LPSTR, long, unsigned int, SpiPictureInfo*);
typedef	int (PASCAL *SpiFuncGetPicture)(LPSTR, long, unsigned int, HLOCAL*, HLOCAL*, FARPROC, long);

///////////////////////////////////////////////////////////////////////////

	HMODULE		m_hDLL;
	SpiFuncIsSupported		m_pIsSupported;
	SpiFuncGetPictureInfo	m_pGetPictureInfo;
	SpiFuncGetPicture		m_pGetPicture;
	static string		m_strSPIPath;	//	SPIの検索パス　default:"SPI"
	struct CSPIPathInitializer {
		CSPIPathInitializer() { CGraphicLoader::m_strSPIPath = "SPI"; }
	};
	friend struct CSPIPathInitializer;
	static CSPIPathInitializer spi_initializer;
	///	↑SPIのpathを初期化するためのクラスコンストラクタ

	const CFile*		m_lpFile;
	const CFile*	GetFile() const { return m_lpFile; }

	HLOCAL		m_hBm;
	HLOCAL		m_hBmInfo;
	virtual void	FreeLocalMemory();
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
