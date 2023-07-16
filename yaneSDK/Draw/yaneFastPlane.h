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
	DirectDraw�ɂ��Q���������@�`��N���X�i�T�[�t�F�[�X�j
	class CFastDraw ���Q�l�ɂ��邱�ƁB
*/
public:

	///	---- ISurface�̃����o
	virtual int GetType() const { return 1; }
	virtual LRESULT Load(const string& strBitmapFileName);
//	virtual LRESULT Save(const string& strBitmapFileName,LPCRECT lpRect=NULL);
//	virtual LRESULT SaveYGA(const string& strBitmapFileName,LPCRECT lpRect=NULL,bool bCompress=true);
	virtual LRESULT	GetSize(int &x,int &y) const;
	virtual LRESULT SetSize(int x,int y){
		int sx,sy;
		GetSize(sx,sy);
		if (sx!=x || sy!=y) return CreateSurface(x,y);
		return 0;	//	resize���Ƃ��
	}
	virtual LRESULT	Release();

	/** ISurface�̃����o�ŏ\���Ȃ̂ŁA�I�[�o�[���C�h�͂��Ă��Ȃ�
	virtual	LRESULT SetColorKey(ISurfaceRGB rgb);
	virtual LRESULT SetColorKeyPos(int x,int y);
	virtual ISurfaceRGB GetColorKey() const;

		�v���C�}���^�Z�J���_���̃f�B�t�H���g�̔����F�ݒ�ɂ��ẮA
		�ۏ؊O�B�v���C�}���Ɏ����ẮA�T�[�t�F�[�X��lock�ł��Ȃ�
		�r�f�I�J�[�h������̂ŁA������Ȃ��̂��g�B
	*/

	/**
	//	default����ŗǂ����̂̓I�[�o�[���C�h���Ă��Ȃ�
	//		fill color...
		virtual void		Clear(LPCRECT lpRect=NULL);
		virtual LRESULT		SetFillColor(ISurfaceRGB c);
		virtual ISurfaceRGB	GetFillColor() const;
	*/

	virtual smart_ptr<ISurface> clone();
	virtual LRESULT CreateSurfaceByType(int sx,int sy,int nType);

	/**
		���̑��A�`��֘A�̃��\�b�h�́AISurface�̂ق���
		���Ȃ肠��̂ŁA����������Ă��������B
	*/

	//////////////////////////////////////////////////////////////////////////

	//	�e��ݒ�
//	void	SetDirectDrawSurface(bool bEnable);	//	DirectDrawSurface�ɂ���̂�
	LRESULT	SetSystemMemoryUse(bool bEnable);	//	�V�X�e�����������g���̂�
	//	�������́A�r���ύX�͕s�B
	//	����CreateSurafece�������́ALoadBitmap�����Ƃ�����L���B

	/*
		��̂Q�̐ݒ�́A
			�v���C�}���Ftrue  - false
			�Z�J���_���Ffalse - true   or	true  - true
			���ʂ̃T�[�t�F�[�X�@�@�Ffalse - true
		���W���I���ƍl������B
	*/

	//////////////////////////////////////////////////////////////////////////
	//	HDC�𓾂āA���ڏ�������:p

	HDC		GetDC();
		//	���������A���Ȃ炸�擾�ł���Ƃ͌���Ȃ��B
		//	�K���擾���Ȃ���΂Ȃ�Ȃ��̂ł���΁A
		//	SetDirectDrawSurface(true)�ɂ��Ă������ƁB
		//	�擾�ł��Ȃ������ꍇ�ANULL���Ԃ�
	void	ReleaseDC();

	///	�I�[�i�[�h���[�T�[�t�F�[�X�̐���
	///	�T�C�Y�w��Ńv���[���쐬
	///	(sx,sy)�T�C�Y  bAlpha==true�Ȃ�΁A���T�[�t�F�[�X
	virtual LRESULT CreateSurface(int sx,int sy,bool bAlpha=false);	

	//	�v���C�}���T�[�t�F�[�X�̐���
	LRESULT	CreatePrimary(bool& bUseFilp,int nSx=0,int nSy=0);
	//	�Z�J���_���T�[�t�F�[�X�̐���
	LRESULT CreateSecondary(CFastPlane*lpPrimary,bool& bUseFlip);

	//////////////////////////////////////////////////////////////////////////
	///		----	Effector

	///�@���l�𔽓]������i���T�[�t�F�[�X�ɑ΂��Ă̂ݗL���j
	LRESULT		FlushAlpha(LPCRECT lpRect=NULL) { return 0; } // ������(todo)

	/////// todo

	//	���l�𓾂�
	virtual int	GetPixelAlpha(int x,int y) { return 0; }
	//	�s�N�Z���̐F�𓾂�

	//////////////////////////////////////////////////////////////////////////
	///	property...

	//	�r�b�g�}�b�v���ǂݍ��܂�Ă����true
	bool	IsLoaded() const { return !m_strBitmapFile.empty(); }
	//	�ǂݍ���ł���r�b�g�}�b�v�̃t�@�C������Ԃ�
	string	GetFileName() const;

	//	YGA�摜�Ȃ̂��ǂ���
	virtual	bool	IsYGA() { return m_bYGA; }
	bool*	GetYGA() { return& m_bYGA; }

	void	SetYGAUse(bool bUse) { m_bYGAUse = bUse; }
	bool	GetYGAUse() { return m_bYGAUse; }
	//	�����true��ݒ肷��΁�Load����Ƃ���ACreateSurface����Ƃ��A
	//	�����I�ɏ�Ƀ��t���T�[�t�F�[�X���쐬����B
	//	�f�B�t�H���g�ł�false

	//	�����C���T�[�t�F�[�X�ɂ���(default:false)
	//		(���ꂾ��Lost���Ȃ��B�������A32bpp->16bpp�̂悤�ȕϊ��ɂ���ăr�b�g�[�x��������)
	void	SetAutoRestore(bool bEnable) { m_bAutoRestore = bEnable; }
	bool	GetAutoRestore() { return m_bAutoRestore; }

	LPDIRECTDRAWSURFACE GetSurface();
	LPDIRECTDRAWPALETTE GetPalette();
	//	��DirectDrawSurface�Ƃ͌���Ȃ��̂ŁA���̂Q��
	//		�擾�ł��邩�ǂ����킩��Ȃ��D�D
	//		�擾�ł��Ȃ������Ƃ��́ANULL���Ԃ�

	int		GetBpp() const {
		const int anType[] = { 0,0,8,16,16,24,24,32,32,0,16,16,32,32 };
		return anType[const_cast<CFastPlane*>(this)->GetSurfaceType()];
	}

	//	0�`1(�s��)�̏ꍇ�ADIB(16bpp)�����ɂ�����܂��B
	//	����ȊO�̏ꍇ�Am_bMemorySurface��true�ł���΁A�������T�[�t�F�[�X
	//	�����Ȃ��΁ADirectDrawSurface�ł�����Am_lpSurface���L���Ƃ������Ƃ�
	//	�Ȃ�܂��B

	//	�s���̏ꍇ�A�Z�J���_�������Ƃ��́A�������ł�
	//	�������v���C�}���֓]�����Ă��K�v������̂ŁA
	//	DIBSection�����ׂ��H
	//	�Ƃ������A�������s�����Ăǂ��������ƁH�H

	//////////////////////////////////////////////////////////////////////////

	///		FastDraw��mediator
	void	SetFastDraw(CFastDraw*p) { m_pFastDraw = p; }
	CFastDraw* GetFastDraw() const { return m_pFastDraw; }

	/**		�f�B�t�H���g�Ŏg�p����FastDraw�̐ݒ�/�擾
		������́A
		1.CFastPlane::SetFastDraw�@��
		2.CFastPlane�̃R���X�g���N�^
		��CFastDraw*���n����Ă��Ȃ��Ƃ��Ɍ���g�p����f�B�t�H���g��
		CFastDraw�̐ݒ�^�擾�B

		���@ThreadLocal�ŕێ����Ă��܂��B

		CFastDraw�̃R���X�g���N�^��SetDefaultFastDraw(this);�Ƃ���Ă���̂ŁA
		1�X���b�h��CFastDraw���ЂƂ������݂��Ȃ��ꍇ�͓��ɉ������Ȃ���ok

	*/
	static void	SetDefaultFastDraw(CFastDraw*p) { m_pDefaultFastDraw = p; }
	static CFastDraw* GetDefaultFastDraw() {
		return m_pDefaultFastDraw.isEmpty()?NULL:*m_pDefaultFastDraw.Get();
	}

	///	�v���C�}���T�[�t�F�[�X��lock�ł��Ȃ����Ƃ�����̂œ��ꈵ��
	bool	IsPrimarySurface () const { return m_bPrimary; }

	//////////////////////////////////////////////////////////////////////////

	CFastPlane(CFastDraw* pFastDraw=NULL);
	virtual ~CFastPlane();

	friend class CFastDraw;

protected:

	LPDIRECTDRAWSURFACE m_lpSurface;
	//	DirectDrawSurface�̂Ƃ��́A���̃|�C���^
	LPDIRECTDRAWPALETTE m_lpPalette;
	//	DirectDrawPalette�̂Ƃ��́A���̃|�C���^

	//	CreateDIBSection�ō�����T�[�t�F�[�X�̏ꍇ
	//	(�s���s�N�Z���t�H�[�}�b�g�ɑ΂��ăZ�J���_���̂݁A����ŗL�蓾��)
	HBITMAP m_hBitmap;
	HDC		m_hDC;

	//	256�F���[�h�̎��ɓǂݍ��񂾉摜�́A���̂Ƃ���HDC�Ɉˑ�����̂�
	//	RGB555�ɕύX���ꂽ�Ƃ��́A�ǂݒ����K�v������
	bool	m_bLoad256;

	//	�C���������̂ŁA�Q�ƃJ�E���^�͗p�ӂ��Ȃ��B

	//	�v���[���̃T�C�Y
	int		m_nSizeX;
	int		m_nSizeY;

	//	bitmap�t�@�C����
	string	m_strBitmapFile;				// �t�@�C�����ۑ����Ƃ��Ȃ�������
		//	�r�b�g�}�b�v��ǂݍ���ł��Ȃ��Ƃ��́A�t�@�C�����͋�ł���

	//////////////////////////////////////////////////////////////////////////
	//	�T�[�t�F�[�X�̕��A����
	//	���X�g�A�͓����I�ɌĂяo�����i���[�U�[�͒ʏ�A���ڌĂяo�����Ƃ͖����j
	virtual LRESULT	Restore();
	//	���X�g�A�����������ɂ́A���̊֐����I�[�o�[���C�h����i�ʏ�A���̕K�v�͖����j

	virtual LRESULT OnDraw(){ return 0; }
	//	�v���[���̕����̂��߂��ꂪ�Ăяo�����i������I�[�o�[���C�h����j

	virtual LRESULT	InnerLoad(const string& strFileName);
	virtual LRESULT InnerCreateSurface(int sx,int sy,bool bYGA=false,bool bSecondary256=false);

	//	�T�C�Y�w��Ńv���[���쐬
	LRESULT InnerCreateMySurface(int sx,int sy,int nSurfaceType,bool bClear=true);
	/**
		bClear == true�Ȃ�΁A��ʂ�����(Clear����)�@false
		�����A����ɃT�[�t�F�[�X�Ƀf�[�^��ǂݍ��ނ��Ƃ�
		�킩���Ă���̂Ȃ�΃N���A����K�v�͖���
	*/

	LRESULT InnerRestoreSurface();		//	�T�[�t�F�[�X�̃��X�g�A�����i�����p�j
	LRESULT	UpdateSurfaceInfo();		//	�T�[�t�F�[�X�����X�V����i�����p�j
	//	�Ăяo������GetSurface()���s���łȂ����Ƃ�ۏ؂��ׂ�

	//////////////////////////////////////////////////////////////////////////


	bool	m_bUseSystemMemory;			//	�����I�ɃV�X�e�����������g���I�v�V����
//	bool	m_bUseDirectDrawSurface;	//	�����I��DirectDrawSurface���g���I�v�V����

	ISurfaceRGB m_FillColor;			//	��`�N���A����F
	DWORD	m_dwFillColor;				//	���̐F�����݂̃T�[�t�F�[�X�ɕϊ���������

	//////////////////////////////////////////////////////////////////////////

	//	�T�[�t�F�[�X���
	bool	m_bYGA;					//	YGA�摜�ł��邩
	bool	m_bYGAUse;				//	�����I�Ƀ��t���T�[�t�F�[�X���쐬����
	bool	m_bAutoRestore;			//	�������X�g�A����̂�
	bool	m_bMySurface;			//	������new����Surface���H
	bool	m_bSecondary256;		//	256�F�p�̃Z�J���_��
									//	�i���̃T�[�t�F�[�X��256�F���[�h�ł�RGB555�ł͂Ȃ�256�F�j
	bool	m_bSecondary256DIB;		//	256�F�p�̃Z�J���_��
									//	�i���̃T�[�t�F�[�X�́ARGB555����DIBSection�j
	bool	m_bOwnerDraw;
	//	�I�[�i�[�h���[(����ɂ��Ă�����Restore���Ăяo����Ȃ��BPrimary,Secondary�͂���)
	bool	m_bPrimary;				//	�v���C�}���T�[�t�F�[�X(�����lock�o���Ȃ����Ƃ�����̂œ���)

	//////////////////////////////////////////////////////////////////////////

	//	�S�v���[���ւ̃C���X�^���X�̃`�F�C��
	//	���̃`�F�C���ɂ�Primary,Secondary���܂ނ̂Œ��ӂ��K�v
//	static set<CFastPlane*>	m_lpPlaneList;

	//	����YGA�摜�̓ǂݍ��ݗp
	LRESULT InnerLoadYGA(const string& strFilename);

protected:
//	static DWORD DDColorMatch(LPDIRECTDRAWSURFACE pdds, COLORREF rgb); // �����F��T��
//	static DWORD DDGetPixel(LPDIRECTDRAWSURFACE pdds,int x,int y); // ����̓_�̐F�𒲂ׂ�
	//	�����̃T�[�t�F�[�X��DirectDrawSurface�Ƃ͌���Ȃ��̂ŁA����ł͎g�����̂ɂȂ�Ȃ��I�R(`�D�L)�m

//	static void	InnerGetBpp();	//	bpp manager�̃��Z�b�g
//	static int	GetBpp();		//	bpp�̎擾

	CFastDraw*	m_pFastDraw;
	static ThreadLocal<CFastDraw*>	m_pDefaultFastDraw;

	// FastDraw�́ASetFastDraw�Őݒ肳�ꂽ���̂�Ԃ��B�ݒ肳��Ă��Ȃ���΃f�B�t�H���g�̂��̂�Ԃ�
	CFastDraw* GetMyFastDraw() const {
		if (m_pFastDraw!=NULL) return m_pFastDraw;
		return GetDefaultFastDraw();
	}

	bool m_bNowRestoring;	//	���X�g�A�����̃t���O
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif	// USE_FastDraw

#endif	// ifdef __yaneFastDraw_h__

