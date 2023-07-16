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
	DIB(Device Independent Bitmap�F�f�o�C�X�Ɉˑ����Ȃ��r�b�g�}�b�v)��
	�Ǘ����܂��B
*/
public:
	///	�摜�̓ǂݍ���
	virtual LRESULT Load(const string& strFileName,int nBpp=24);
	/**
		Load�̓t�@�C������T�[�t�F�[�X�ɓǂݍ��ށB
		Save�̓T�[�t�F�[�X���t�@�C���ɏ������݂܂��B
�@		�ǂݍ��݂�CGraphicLoader�N���X���g�p����̂ŁA
		BMP,GIF,JPEG,SPI���̗��p���\�ɂȂ�܂��B
		nBpp���w�肷��ƁA����bpp(bits per pixel)�œǂݍ��݂܂��B
		�������݂�24bpp�̃r�b�g�}�b�v�ɂȂ�܂��B
		lpRect�̓t�@�C���ɕۑ��������T�[�t�F�[�X�̋�`�̈���w�肵�܂��B
		�w�肪�Ȃ���ΑS��ɂȂ�܂��B

		�g�p��j
	�@�@::SetPixel(dib.GetDC(),0,0,RGB(100,200,150));
			// (0,0)�̍��W��RGB(100,200,150)�̓_��ł�

		�Q�Ɓ��@::GetPixel,COLORREF,GetRValue,GetGValue,GetBValue

	*/

	virtual LRESULT Blt(HDC hSrcDC,LPCRECT lpRect,int nBpp=24);
	/**
		hDC�Ŏw�肳���A�����`�̈�lpRect������CDIBitmap�ɓǂݍ��ށB
		���̂��ƁASave���邱�Ƃɂ���āA�r�b�g�}�b�v�ɕۑ����Ă�邱�Ƃ�
		�o����B�܂�HDC�����l���ł���΁A���̊֐���Save�Ńr�b�g�}�b�v��
		�ۑ����邱�Ƃ��o����B
		nBpp���w�肷��ƁA����bpp(bits per pixel)�ŃT�[�t�F�[�X���쐬����
		�ǂݍ��݂܂��B���x�͒x���ł��B
	*/

	virtual LRESULT Save(const string& strFileName,LPCRECT lpRect=NULL);
	/**
		�T�[�t�F�[�X�̃t�@�C���ւ̕ۑ�
		�w�肵����`��ۑ����܂��BlpRect==NULL�Ȃ�΁A�T�[�t�F�[�X�S��B
	*/

	virtual LRESULT CreateSurface(int nSizeX,int nSizeY,int nBpp=24,bool bAlpha=false);
	/**
		�T�[�t�F�[�X���w��T�C�Y�A�w���BPP(BitParPixel)�ō쐬���܂��B
		�T�[�t�F�[�X�̓N���A����܂���B

		bAlpha==true�ɂ��Ă����ƁAGetSurfaceInfo�Ŏ擾�ł���
		SurfaceType�����T�[�t�F�[�X�̔ԍ��ɂȂ��ĕԂ��Ă���̂�
		(�������AnBpp==32���w�肵�Ȃ���΂Ȃ�Ȃ�)
		���̂��ƁACSurfaceInfo��Bltter��Alpha�T�[�t�F�[�X�̂��Ƃ�
		�]�����邱�Ƃ��o����
	*/

	virtual LRESULT Clear(LPRECT pRect=NULL);
	/**
		�T�[�t�F�[�X�̃N���A
	*/

	virtual LRESULT	Release();
	/**
		�T�[�t�F�[�X��������܂��B�f�X�g���N�^�ŉ�������̂ŁA
		�����I�Ɏw�肷��K�v�͂���܂���B
	*/

	//	LoadBitmap/CreateSurface�ȍ~�A����
	virtual HDC		GetDC() { return m_hDC; }
	virtual HBITMAP GetHBITMAP() { return m_hBitmap; }
	/**
		HDC�AHBITMAP��Ԃ��܂��BUseDIBSection(true)�Ƃ��Ă������ꍇ��
		�idefault�ł�false�j�ALoadBitmap/CreateSurface�ȍ~�A���ł�
		HDC,HBITMAP�̎擾�͉\�ł��B
		�����p���ăs�N�Z�����������Ɨǂ��ł��B
	*/

	virtual LRESULT	GetSize(LONG& x,LONG& y);
	/**
		�T�[�t�F�[�X�̃T�C�Y���擾���܂��B
	*/

	virtual BYTE*	GetPtr() { return (BYTE*)m_lpdwSrc; }
	/**
		Load/CreateSurface�Ŋm�ۂ��ꂽ�C���[�W�ւ̃|�C���^�������܂��B
		(CreateDIBSection�œ���ꂽ����)
		DIB�Ȃ̂ŁA�C���[�W�̊e���C���̐擪�́ADWORD�ŃA���C������Ă���̂�
		���ӂ��K�v�B�������㉺�͓]�u���Ă��܂���B
	*/

	virtual CSurfaceInfo* GetSurfaceInfo();
	/**
		class CSurfaceInfo ��������΁A������Bltter��p����
		�ǂ�ȓ]���ł��\�ɂȂ�
	*/

	/**
		�T�C�Y�擾�̂��߂�property
	*/
	ULONG GetWidth() const { return m_nSizeX; }
	ULONG GetHeight() const { return m_nSizeY; }

	CDIBitmap();
	virtual ~CDIBitmap();

protected:
	DWORD*	m_lpdwSrc;		//	�\�[�X�o�b�t�@
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
