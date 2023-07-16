//
//	yaneGraphicLoader.h
//
//		BMP,JPEG,GIF,...��K�v�ɉ�����Susie-Plug in�������I�ɓǂݍ��ރN���X
//		programmed by yaneurao(M.Isozaki)	'00/05/15
//		reprogrammed by yaneurao(M.Isozaki) '00/06/21
//
//			SPI�ǂݍ��ݕ����̃T���v���\�[�X�������������A�ɂ����邳��Ɋ��ӁI
//
//		yga�ǂݍ��݂ɂ��Ή� '02/07/25
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

	//	���Ђ�[�B�������傦���`�O�O�G
	static factory_permutation_i<IGraphicLoader,CGraphicLoader>* GetGraphicFactory() { return& m_factory; }
private:
	static factory_permutation_i<IGraphicLoader,CGraphicLoader> m_factory;
};

class CGraphicLoader : public IGraphicLoader {
public:
	virtual LRESULT LoadPicture(const CFile& file);		///	����œǂݍ��ޕԂ��l��0�Ȃ�ΐ���

	virtual LRESULT GetSize(LONG& x,LONG& y) const;		///	����œǂݍ��񂾉摜�̃T�C�Y���擾

	virtual LRESULT IsYGA() const;
	///	�ǂݍ��񂾉摜���A���T�[�t�F�[�X�Ȃ̂���Ԃ�

	virtual LRESULT Render(CSurfaceInfo*pInfo);
	///	CSurfaceInfo��n���āA�����ɕ`�悵�Ă��炤

	virtual LRESULT	Render(HDC hdc);					///	hdc��n���āA�����ɕ`�悵�Ă��炤
	virtual LRESULT	ReleasePicture();					///	����ŉ������

	virtual LRESULT	GetPalette(PALETTEENTRY pal[256]);	///	�p���b�g�̎擾

	static LRESULT	ClearPalette(BITMAPINFO* pBI);
	/**
		BITMAPINFO��8bpp��bmp������킷�Ȃ�΁A���̌�i�ɂ���
		RGBQUAD������������B
	*/

	/**
		SPI(susie plug-in)�̌���path�̐ݒ�^�擾
		default�ł�"spi"�i�t�H���_���j
		����āAjpg��ǂݍ��ނƂ��́A"spi/ifjpg.spi"�Ƃ���
		�v���O�C����T���ɍs���B
	*/
	static	void SetSPIPath(const string& strPath) { m_strSPIPath = strPath; }
	static	string GetSPIPath() { return m_strSPIPath; }

	//	PrototypeFactory�Ŗ����I�ɍ���ĂˁO�O�G
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
	bool	m_bYGA;			//	�ǂݍ��񂾂̂�yga�摜�Ȃ̂��H
	int		m_nYGA_Xsize;	//	yga�摜��x�T�C�Y
	int		m_nYGA_Ysize;	//	yga�摜��y�T�C�Y

	///	����rendering�́A�����I�ɌĂяo������
	virtual LRESULT InnerRenderBmp(CSurfaceInfo*pInfo);	// BMP��Render
	virtual LRESULT InnerRenderYga(CSurfaceInfo*pInfo);	// YGA��Render

///////////////////////////////////////////////////////////////////////////

	//	for OLELoadPicture
	IPicture*	m_lpiPicture;

	//	for SPI
///////////////////////////////////////////////////////////////////////////
typedef struct SpiPictureInfo {
	long left,top;					//	�摜��W�J����ʒu
	long width;						//	�摜�̕�(pixel)
	long height;					//	�摜�̍���(pixel)
	WORD x_density;					//	��f�̐����������x
	WORD y_density;					//	��f�̐����������x
	short colorDepth;				//	�P��f�������bit��
	HLOCAL hInfo;					//	�摜���̃e�L�X�g���
} SpiPictureInfo, *LPSpiPictureInfo;

typedef	int (PASCAL *SpiFuncIsSupported)(LPSTR, DWORD);
typedef	int (PASCAL *SpiFuncGetPictureInfo)(LPSTR, long, unsigned int, SpiPictureInfo*);
typedef	int (PASCAL *SpiFuncGetPicture)(LPSTR, long, unsigned int, HLOCAL*, HLOCAL*, FARPROC, long);

///////////////////////////////////////////////////////////////////////////

	HMODULE		m_hDLL;
	SpiFuncIsSupported		m_pIsSupported;
	SpiFuncGetPictureInfo	m_pGetPictureInfo;
	SpiFuncGetPicture		m_pGetPicture;
	static string		m_strSPIPath;	//	SPI�̌����p�X�@default:"SPI"
	struct CSPIPathInitializer {
		CSPIPathInitializer() { CGraphicLoader::m_strSPIPath = "SPI"; }
	};
	friend struct CSPIPathInitializer;
	static CSPIPathInitializer spi_initializer;
	///	��SPI��path�����������邽�߂̃N���X�R���X�g���N�^

	const CFile*		m_lpFile;
	const CFile*	GetFile() const { return m_lpFile; }

	HLOCAL		m_hBm;
	HLOCAL		m_hBmInfo;
	virtual void	FreeLocalMemory();
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
