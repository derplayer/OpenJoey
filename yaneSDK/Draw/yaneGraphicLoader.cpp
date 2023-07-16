//	yaneGraphicLoader.cpp

#include "stdafx.h"
#include "yaneGraphicLoader.h"
#include <ole2.h>				//	CreateStreamOnHGlobal
#include "yaneSurface.h"		//	CSurface::iterator
#include "yaneSurfaceBltter.h"	//	�T�[�t�F�[�X�̃s�N�Z���R���o�[�W�����Ɏg��
#include "../Auxiliary/yaneLZSS.h" // CLZSS(yga�摜�̓ǂݍ��݂Ɏg�p����)

namespace yaneuraoGameSDK3rd {
namespace Draw {

string CGraphicLoader::m_strSPIPath;
CGraphicLoader::CSPIPathInitializer CGraphicLoader::spi_initializer;
///	��SPI��path�����������邽�߂̃N���X�R���X�g���N�^

CGraphicLoader::CGraphicLoader(){
	m_lpBI			= NULL;				//	for LoadPictureByOwn
	m_lpBF			= NULL;
	m_lpBits		= NULL;

	m_lpiPicture	= NULL;				//	for OLELoadPicture

	//	yga�摜�̓ǂݍ��ݗp
	m_nYGA_Xsize = 0;
	m_nYGA_Ysize = 0;
	m_bYGA = false;

	m_hDLL			= NULL;				//	for SPILoader
	m_hBm			= NULL;
	m_hBmInfo		= NULL;

	m_lpFile		= NULL;

}

CGraphicLoader::~CGraphicLoader(){
	ReleasePicture();
}

factory_permutation_i<IGraphicLoader,CGraphicLoader> IGraphicLoader::m_factory;

LRESULT CGraphicLoader::LoadPicture(const CFile& file){
	ReleasePicture();
	m_lpFile= &file;
	if (LoadPictureByOwn1(file)==0) return 0;	//	Bitmap
	if (LoadPictureByOwn2(file)==0) return 0;	//	YGA
	if (LoadPictureByOLE(file)==0) return 0;	//	JPEG,Gif
	if (LoadPictureBySPI(file)==0) return 0;	//	using by SPI
	return 1;
}

LRESULT CGraphicLoader::LoadPictureByOwn1(const CFile& file){
	//	���͂Ńr�b�g�}�b�v����ǂݏo��
	//	�����I�ɂ�Png���ɂ��Ή����������悤�ȁA���������Ȃ��悤�ȁc
	//	�i�ǂȂ���˂�I�j

	// ����ŁA�t�@�C�����܂邲�Ɠǂݍ��߂�
	// ���Ƃ́Afilesize��fileadr�Ŋ撣��̂��B
	m_lpBF = (BITMAPFILEHEADER*)file.GetMemory();
	if (file.GetSize() < sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER) ||
			m_lpBF->bfType!=0x4D42) {
		// BM�Ƃ�������BMP�Ƃ��Ⴄ��łȂ�
		m_lpBF = NULL;
		return 1;
	}
	m_lpBI = (BITMAPINFOHEADER*)((BYTE*)m_lpBF + sizeof(BITMAPFILEHEADER));
	m_lpBits = (void*)((BYTE*)m_lpBF + m_lpBF->bfOffBits);

	return 0;
}

LRESULT CGraphicLoader::LoadPictureByOwn2(const CFile& file){
	//	yga�摜�̓ǂݍ���
	//	�g���q�Ŕ��ʂ��Ă����̂��ȁH�H

	//	yga�t�H�[�}�b�g(yaneurao graphic format with alpha)
	string suffix;
	suffix = CFile::GetSuffixOf(file.GetName());
	CFile::ToLower(suffix);
	if (suffix!="yga") return 1;

	//	yga�̓p���b�g���܂܂Ȃ�

	CYGAHeader& header = *(CYGAHeader*)file.GetMemory();
	if (header.dwIdentifier != 0x616779) {
		return 2;	// header��ID���ƍ����Ƃ���..
	}

	m_nYGA_Xsize = header.dwSizeX;
	m_nYGA_Ysize = header.dwSizeY;
	m_bYGA = true;

	return 0;
}

LRESULT CGraphicLoader::LoadPictureByOLE(const CFile& file){
	IStream* iStream;

/* UeSyu������F
CreateStreamOnHGlobal�̂P�Ԗڂ̈����i�f�[�^�̈ʒu�j�œn���|�C���^�̎����̈�
���Anew�ō����BYTE�̔z�񂾂�Win98�ł͓����Ȃ��悤�ɂȂ��Ă���݂����ł��B
�ŁAGlobalAlloc�ō�����̈�ɂ���Ƃ����Ɠ����悤�ɂȂ�܂����B
�����AWin2000�̏ꍇ�A�ǂ���ł������悤�ł��B
*/

	if(!SUCCEEDED(::CreateStreamOnHGlobal((HGLOBAL)file.GetMemory(),FALSE,&iStream))) {
		ReleasePicture();
		return 1;
	}
	if(!SUCCEEDED(::OleLoadPicture(iStream,file.GetSize(), TRUE, IID_IPicture, (LPVOID*)&m_lpiPicture))){
		iStream->Release();
		ReleasePicture();
		return 2;
	}
	iStream->Release();

	return 0;
}

LRESULT CGraphicLoader::LoadPictureBySPI(const CFile& file){
	//	�g���q�𒲂ׂ�
	string suffix;
	suffix = file.GetSuffix();
	string strDllName;
	strDllName = GetSPIPath() + "/if" + suffix + ".spi";
	CFile::MakeFullName(strDllName);

	m_hDLL = ::LoadLibrary(strDllName.c_str());
	if(m_hDLL==NULL) return 1;

	m_pIsSupported		= (SpiFuncIsSupported)		GetProcAddress(m_hDLL, "IsSupported");
	m_pGetPictureInfo	= (SpiFuncGetPictureInfo)	GetProcAddress(m_hDLL, "GetPictureInfo");
	m_pGetPicture		= (SpiFuncGetPicture)		GetProcAddress(m_hDLL, "GetPicture");

	//	����������t�@�C���͓ǂݍ��ނƃG���[�ɂȂ��Ă��܂��I
	if (file.GetSize() < 2048) return 1;

	if (!m_pIsSupported(NULL,(DWORD)file.GetMemory())) {
		ReleasePicture();	//	DLL�̉��
		return 1;
	}
	return 0;
}

LRESULT CGraphicLoader::IsYGA() const{
	///	�ǂݍ��񂾉摜���A���T�[�t�F�[�X�Ȃ̂���Ԃ�
	return m_bYGA;
}

LRESULT CGraphicLoader::GetSize(LONG& x,LONG& y) const {			//	����œǂݍ��񂾉摜�̃T�C�Y���擾
	if (m_lpBI!=NULL) {
		x = m_lpBI->biWidth;
		y = m_lpBI->biHeight;
		return 0;
	}
	if (m_bYGA){
		x = m_nYGA_Xsize;
		y = m_nYGA_Ysize;
		return 0;
	}
	if (m_lpiPicture!=NULL){
		m_lpiPicture->get_Width(&x);
		m_lpiPicture->get_Height(&y);

		//	�Ȃ���26.45724907�{���Ԃ��Ă���̂�:p
		//	������H26.45�����H('00/08/24)
		//	������Himetric��pixels�ɕϊ����Ă�񂾂�H('00/08/25)
		HDC hdc = ::GetDC(NULL);
		x = ::MulDiv(x,::GetDeviceCaps(hdc,LOGPIXELSX),2540);
		y = ::MulDiv(y,::GetDeviceCaps(hdc,LOGPIXELSY),2540);
		::ReleaseDC(NULL,hdc);

		return 0;
	}
	if (m_hDLL!=NULL){
		SpiPictureInfo info;
		m_pGetPictureInfo((LPSTR)GetFile()->GetMemory(),(LONG)GetFile()->GetSize(),1,&info);
		x = info.width;
		y = info.height;
		return 0;
	}
	return 1;
}

LRESULT CGraphicLoader::ReleasePicture(){
	m_lpBI			= NULL;				//	for LoadPictureByOwn
	m_lpBF			= NULL;
	m_lpBits		= NULL;

	if (m_lpiPicture!=NULL) {
		LRESULT lr = 0;
		if (m_lpiPicture->Release()!=S_OK) lr++;
		m_lpiPicture = NULL;
		return lr;
	}

	if (m_hDLL!=NULL){
		::FreeLibrary(m_hDLL);
		m_hDLL		= NULL;
	}
	FreeLocalMemory();

	//	yga�摜�̓ǂݍ��ݗp
	m_nYGA_Xsize = 0;
	m_nYGA_Ysize = 0;
	m_bYGA = false;

	m_lpFile = NULL;

	return 0;
}

void	CGraphicLoader::FreeLocalMemory(){
	if (m_hBm!=NULL) {
		::LocalFree(m_hBm);
		m_hBm		= NULL;
	}
	if (m_hBmInfo!=NULL){
		::LocalFree(m_hBmInfo);
		m_hBmInfo	= NULL;
	}
}

///////////////////////////////////////////////////////////////////////

//	bmp���O�œǂݍ��܂�Ƃ�����B�����A�𑜓x�؂�ւ���Ă��A
//	GetDC�́A�ŏ��N�������Ƃ���DC�Ԃ���邩��...
LRESULT CGraphicLoader::Render(CSurfaceInfo*pInfo){
///	CSurfaceInfo��n���āA�����ɕ`�悵�Ă��炤
	if (m_lpBI!=NULL)	return InnerRenderBmp(pInfo);
	if (m_bYGA)			return InnerRenderYga(pInfo);
	return -1; // ���O�œǂݍ���łւ��
}
///////////////////////////////////////////////////////////////////////

LRESULT CGraphicLoader::InnerRenderBmp(CSurfaceInfo* pInfo){
/*	
typedef struct tagBITMAPINFOHEADER {
	unsigned long		biSize;			// ���\���̂̃T�C�Y(�o�C�g)
	long				biWidth;		// �C���[�W�̕�			  [*1]
	long				biHeight;		// �C���[�W�̍���		  [*1]
	unsigned int		biPlanes;		// must be 1
	unsigned int		biBitCount;		// �F��					  [*2]
	unsigned long		biCompression;	// ���k�^�C�v			  [*3]
	unsigned long		biSizeImage;	// �C���[�W�̃T�C�Y		  [*3]
	long				biXPixPerMeter; // �����𑜓x			  [*4]
	long				biYPixPerMeter; // �����𑜓x			  [*4]
	unsigned long		biClrUsed;		// ���J���[�e�[�u�����Q��
	unsigned long		biClrImporant;	// ���J���[�e�[�u�����Q��
} BITMAPINFOHEADER;

�ERGBQUAD/RGBTRIPLE --------------------------------------------------
�@�ȉ��Ɏ����̂�RGBQUAD�ł���ARGBTRIPLE�ɂ����Ă͢rgbReserved����Ȃ�
�@�Ȃ邾���ł���B

typedef struct tagRGBQUAD {
	unsigned char		rgbBlue;		// �̋P�x
	unsigned char		rgbGreen;		// �΂̋P�x
	unsigned char		rgbRed;			// �Ԃ̋P�x
	unsigned char		rgbReserved;	// must be 0
} RGBQUAD;
*/

	//	�ςȈ��k�|���Ă���ւ���ȁH�R(`�D�L)�m
	if (m_lpBI->biCompression!=BI_RGB) {
		return 2;
	}

	int nBpp = m_lpBI->biBitCount;
	LONG sx,sy;
	GetSize(sx,sy);
	LONG align_sx = ((sx*nBpp)/8 + 3) & ~3;	//	�S�ŃA���C��

	switch (nBpp){
	case 1:	//	�O���[�X�P�[��
	case 4:	//	�P�U�F
	case 8:	//	�Q�T�U�F
			return 3;	//	��Ή����ׂ��B���߂�
	case 16:
/*
		{	///	RGB555�Ȃ��납�H
			//	������ƒx������iterator�ŏ����΂������肷��
			CSurfaceInfo::iterator it = pInfo->begin();
			for(int y=0;y<sy;y++){
				BYTE* pSrc = (BYTE*)((BYTE*)m_lpBits + align_sx * (sy-y-1));
				for(int x=0;x<sx;x++){
					WORD w = *(WORD*)pSrc;
					//	CFastPlaneRGB555��GetR,GetG,GetB���Q�l�ɂ��ׂ�
					it.SetRGB((BYTE)((w & 0x7c00) >> (10-3)),(BYTE)((w & 0x03e0) >> (5-3)),
						(BYTE)((w & 0x001f) << 3),255);
//					it.SetRGB((w & 0xf800) >> (11-3),(w & 0x07e0) >> (6-3),(w & 0x001f) << 3,255);
						///	��RGB565�̂Ƃ��́A������
					it++;	//	iterator�}���Z�[�R(�L���M)�m
					pSrc+=2;
				}
			}
*/
			//	��iterator�ŏ����Ƃ���ς��x����D�D
		{
			int nDstSurfaceType	= pInfo->GetSurfaceType();
			DWORD dwDstPitch	= pInfo->GetPitch();
			BYTE* pDstPtr		= (BYTE*)pInfo->GetPtr();
			for(int y=0;y<sy;++y){
				void* pSrc = (void*)(
					(BYTE*)((BYTE*)m_lpBits + align_sx * (sy-y-1)));
				void* pDst = (void*)(
					(BYTE*)(pDstPtr + y*dwDstPitch));
				CSurfaceBltter::Blt1Line(pSrc,5,pDst,nDstSurfaceType,sx);
				//	RGB555 -> dst surface...
			}
		}
		break;

	case 24://	�t���J���[
/**
		{
			//	������ƒx������iterator�ŏ����΂������肷��
			CSurfaceInfo::iterator it = pInfo->begin();
			for(int y=0;y<sy;y++){
				BYTE* pSrc = (BYTE*)((BYTE*)m_lpBits + align_sx * (sy-y-1));
//				DWORD* pDst = (DWORD*)((BYTE*)pDstSurface + lPitch * y);
				for(int x=0;x<sx;x++){
//					pDst[x] = *(DWORD*)pSrc & 0xffffff;	//	�ŏ�ʂ͖���
					DWORD dw = *(DWORD*)pSrc;
					it.SetRGB(255,(BYTE)((dw>>16)&0xff),(BYTE)((dw>>8)&0xff),(BYTE)(dw&0xff));
					it++;	//	iterator�}���Z�[�R(�L���M)�m
					pSrc+=3;
					//	�N��24bpp�̂Ƃ���RGBQUAD����Ă������́D�D
				}
			}
			break;
*/
		{
			int nDstSurfaceType	= pInfo->GetSurfaceType();
			DWORD dwDstPitch	= pInfo->GetPitch();
			BYTE* pDstPtr		= (BYTE*)pInfo->GetPtr();
			for(int y=0;y<sy;++y){
				void* pSrc = (void*)(
					(BYTE*)((BYTE*)m_lpBits + align_sx * (sy-y-1)));
				void* pDst = (void*)(
					(BYTE*)(pDstPtr + y*dwDstPitch));
				CSurfaceBltter::Blt1Line(pSrc,5,pDst,nDstSurfaceType,sx);
				//	RGB888 -> dst surface...
			}
		}
		break;

	default:
		return 3;	//	��Ή����ׂ�
	}

	return 0;
}

LRESULT CGraphicLoader::InnerRenderYga(CSurfaceInfo* pInfo){
	CYGAHeader& header = *(CYGAHeader*)GetFile()->GetMemory();

	LONG sx,sy;
	if (GetSize(sx,sy)!=0) return -1;

	BYTE* lpSrcPtr		= (BYTE*)GetFile()->GetMemory() + sizeof(CYGAHeader);
	DWORD dwSrcPitch	= sx * sizeof(CFastPlaneARGB8888);

	BYTE* lpDstPtr		= (BYTE*)pInfo->GetPtr();
	DWORD dwDstPitch	= pInfo->GetPitch();
	int nDstSurfaceType = pInfo->GetSurfaceType();

	if (!header.bCompress) {
		//	�񈳏k�]��
//		::CopyMemory(GetPtr(),lpSrc+sizeof(SYPGHeader),header.dwOriginalSize);
		for(int y=0;y<sy;++y){
			void* pSrc = (void*)(lpSrcPtr + y*dwSrcPitch);
			void* pDst = (void*)(lpDstPtr + y*dwDstPitch);
			CSurfaceBltter::Blt1Line(pSrc,12,pDst,nDstSurfaceType,sx);
			//	ARGB8888 -> dst surface...
		}
	} else {
		// ���k����Ă���̂ŉ𓀂��Ȃ���̓]��
		CLZSS lzss;
		if (nDstSurfaceType==12 &&
			//	�]����̃T�[�t�F�[�X��ARGB8888�Ȃ�΁A
			//	�������̂܂܂����ɓW�J���Ă��܂�
			((DWORD)pInfo->GetPitch() == dwSrcPitch)
			//	�T�[�t�F�[�X���]�����������Ƃ��K�v
		){
			BYTE* lpDst = (BYTE*)pInfo->GetPtr();
			if (lzss.Decode(lpSrcPtr,lpDst
				,header.dwOriginalSize,false)!=0) return 3;
			//	�Ȃ�ŉ𓀂Ɏ��s���邩�ȁD�D

			//	�s�N�Z���t�H�[�}�b�g�͈�v���Ă���̂ŃR���o�[�W�����͕s�v
		} else {
			BYTE* lpSrcBuf;
			if (lzss.Decode(lpSrcPtr,lpSrcBuf
				,header.dwOriginalSize,true)!=0) return 3;
			//	�Ȃ�ŉ𓀂Ɏ��s���邩�ȁD�D
			{
				for(int y=0;y<sy;++y){
					void* pSrc = (void*)(lpSrcBuf	+ y*dwSrcPitch);
					void* pDst = (void*)(lpDstPtr	+ y*dwDstPitch);
					CSurfaceBltter::Blt1Line(pSrc,12,pDst,nDstSurfaceType,sx);
					//	RGB888 -> dst surface...
				}
			}

		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////

LRESULT CGraphicLoader::Render(HDC hdc){
	LONG sx,sy;
	GetSize(sx,sy);

	if (m_lpBI!=NULL) {
		if (::SetDIBitsToDevice(hdc,0,0,m_lpBI->biWidth,m_lpBI->biHeight,0,0,0,m_lpBI->biHeight,
			m_lpBits,(BITMAPINFO*)m_lpBI,DIB_RGB_COLORS)==0) {
			return 1;	//	�Ȃ�ł�[...
		}
		return 0;
	}

	if (m_lpiPicture!=NULL) {
		LONG x,y;
		m_lpiPicture->get_Width(&x);
		m_lpiPicture->get_Height(&y);
		return m_lpiPicture->Render(hdc, 0, 0, sx, sy ,	 0,y,x,-y,NULL)==S_OK ? 0:1;
	}

	if (m_hDLL!=NULL) {
		FreeLocalMemory();
		if (m_pGetPicture((LPSTR)GetFile()->GetMemory(),(LONG)GetFile()->GetSize(),1,&m_hBmInfo, &m_hBm,NULL,NULL)!=0){
			ReleasePicture();
			return 1;
		}

		BITMAPINFO*	lpBitmap = (BITMAPINFO*)::LocalLock(m_hBmInfo);
		void*	lpBits		 = (void*)::LocalLock(m_hBm);
		if (::SetDIBitsToDevice(hdc,0,0,sx,sy,0,0,0,sy,lpBits,lpBitmap,DIB_RGB_COLORS)==0) {
			ReleasePicture();
			::LocalUnlock(m_hBmInfo);
			::LocalUnlock(m_hBm);
			FreeLocalMemory();
			return 1;
		}
		::LocalUnlock(m_hBmInfo);
		::LocalUnlock(m_hBm);
		FreeLocalMemory();
		return 0;
	}
	return 1;	//	not loaded
}


//////////////////////////////////////////////////////////////////////////////

LRESULT	CGraphicLoader::GetPalette(PALETTEENTRY pal[256]){

	BITMAPINFOHEADER* pBI = m_lpBI;

	if (pBI==NULL) return 1;				//	�ǂݍ���ł��Ȃ�

	if (pBI->biBitCount >8) return 2;	//	8bpp�̃O���t�B�b�N�ł͂Ȃ�

	// biBitCount == 1 or 4 or 8
	// ScreenColorMode2 == 8 (256Color)
	// �f�B�X�v���[���[�h��256�F���[�h���A�ǂݍ��ރr�b�g�}�b�v��
	// �p���b�g���g�p���Ă���ꍇ�A���O�Ńp���b�g���Č�����B

	RGBQUAD *lpRGBquad = (RGBQUAD*) ((BYTE*) pBI + sizeof(BITMAPINFOHEADER));

//	DWORD dwFlags;
	int num;
	// ����ŏꏊ�͂킩��������...
	switch (pBI->biBitCount){
	case 1: /* dwFlags = DDPCAPS_1BIT; */ num = 2;	 break;
	case 2: /* dwFlags = DDPCAPS_2BIT; */ num = 4;	 break;
	case 4: /* dwFlags = DDPCAPS_4BIT; */ num = 16;	 break;
	case 8: /* dwFlags = DDPCAPS_8BIT; */ num = 236; break; // 256�͊��ق���:p
	default: /* dwFlags = 0; */	num = 0; break; // ���������˂񂯂ǂ�
	} // �G���g���[���𐔂���...

	// �܂��A236�F�p���b�g�Ƃ����̂́A0�`235�܂ł��g�p����̂ł����āA
	// ������10�`245�͈̔͂ɃV�t�g���Ă��K�v������
		
	// RGBQUAD��PALETTEENTRY�Ƃł�R��B������ւ��I(��)	
	// ����ă����o�P�ʂł̃R�s�[���K�v

	// 0�`9,246�`255�́A�V�X�e���p���b�g(�ÓI�G���g��)
	int i;
	for(i=0;i<10;i++){
		pal[i].peFlags	= PC_EXPLICIT; // ���̃I�v�V�����ƂƂ��ɁA
		pal[i].peRed	= i; // �V�X�e���p���b�g�C���f�b�N�X���w�肷��
		pal[i].peGreen	= 0;
		pal[i].peBlue	= 0;

		// 0�`9,246�`255�̐F�̑��␫���l������΁A�����Z�ł͂Ȃ�xor����ق������S
		pal[i ^ 0xff].peFlags	= PC_EXPLICIT;
		pal[i ^ 0xff].peRed		= i ^ 0xff;
		pal[i ^ 0xff].peGreen	= 0;
		pal[i ^ 0xff].peBlue	= 0;
	}

	// ���[�U�[�p���b�g�̐ݒ�
	for(i=10;i<10+num;i++) {
		pal[i].peFlags = PC_NOCOLLAPSE | PC_RESERVED;
		// ���̃A�v���Ɏg���Ă͍��邵�A�ύX����Ă�����
		pal[i].peRed	= lpRGBquad[i-10].rgbRed; // �p���b�g�̃V�t�g
		pal[i].peGreen	= lpRGBquad[i-10].rgbGreen;
		pal[i].peBlue	= lpRGBquad[i-10].rgbBlue;
	}

	for(;i<246;i++){ // �ꉞ�A���߂Ƃ����H
		pal[i].peFlags	= 0; // �����w�肵�Ȃ�
		pal[i].peRed	= 0;
		pal[i].peGreen	= 0;
		pal[i].peBlue	= 0;
	}

	return 0;
}

LRESULT	CGraphicLoader::ClearPalette(BITMAPINFO* pInfo){
	if (pInfo==NULL) return 0;

	RGBQUAD* pal = (RGBQUAD*)((BYTE*)pInfo + sizeof(BITMAPINFO));

	// 0�`9,246�`255�́A�V�X�e���p���b�g(�ÓI�G���g��)
	int i;
	for(i=0;i<10;i++){
		pal[i].rgbRed	= i; // �V�X�e���p���b�g�C���f�b�N�X���w�肷��
		pal[i].rgbGreen	= 0;
		pal[i].rgbBlue	= 0;

		// 0�`9,246�`255�̐F�̑��␫���l������΁A�����Z�ł͂Ȃ�xor����ق������S
		pal[i ^ 0xff].rgbRed	= i ^ 0xff;
		pal[i ^ 0xff].rgbGreen	= 0;
		pal[i ^ 0xff].rgbBlue	= 0;
	}

	// ���[�U�[�p���b�g�̐ݒ�
	for(i=10;i<246;i++){ // �ꉞ�A���߂Ƃ����H
		pal[i].rgbRed	= 0;
		pal[i].rgbGreen	= 0;
		pal[i].rgbBlue	= 0;
	}

	return 0;
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
