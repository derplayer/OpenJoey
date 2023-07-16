//
//	yaneSurface.h
//		�T�[�t�F�[�X���N���X
//
//	yaneSurface1.cpp �c ��{����
//	yaneSurface2.cpp �c �ʏ�Blt�n����
//	yaneSurface3.cpp �c �ʏ�Effect�n����
//	yaneSurface4.cpp �c ���[�t�B���O�n����

#ifndef __yaneSurface_h__
#define __yaneSurface_h__


/////////////////////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Draw {

class ISurfaceLocker {
/**
	Surface��Lock����@�\���|�����[�t�B�b�N�ɒ񋟂���

	class CSurfaceLockerGuard ���Q�Ƃ̂��ƁB
*/
public:
	/**
		�����̊֐��͎��ۂ�const�ł͖������A�]�����T�[�t�F�[�X��lock�`unlock����̂�
		�]�����T�[�t�F�[�X�����������Ă���킯�ł͂Ȃ��̂ŁA�T�[�t�F�[�X��const����
		�ۂĂĂ���ƍl����B����āALock�`Unlock��const�ƍl���邱�Ƃɂ���B
	*/
	virtual LRESULT Lock() const = 0;
	virtual LRESULT Unlock() const = 0;
	virtual ~ISurfaceLocker(){}
};

class INullSurfaceLocker : public ISurfaceLocker {
///	class ISurfaceLocker �� Null Device
public:
	virtual LRESULT Lock() const { return 0; }
	virtual LRESULT Unlock() const { return 0; }
};

class IDisableSurfaceLocker : public ISurfaceLocker {
///	class ISurfaceLocker �� Disable Device
/**
	Lock���s�\�i�ł��邩���m��Ȃ��jDirectDrawSurface��
	�v���C�}���T�[�t�F�[�X���́A���������蓖�ĂĂ����D�D�Ƃ��H
*/
public:
	virtual LRESULT Lock() const { return 1; }
	virtual LRESULT Unlock() const { return 1; }
};

class CSurfaceLockerGuard {
/**
	�f�X�g���N�^��Unlock���Ăяo�������̃I�u�W�F�N�g
	CSurfaceInfo::GetPixel �Ŏ��ۂɎg���Ă���̂ŁA�������
	�Q�Ƃ̂��ƁB
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

///	ISurfaceRGB��DWORD������[���Ƃ�
typedef DWORD ISurfaceRGB;

class ISurface;
class CSurfaceInfo : public ISurfaceLocker {
/**
	Lock�����Ƃ��̃T�[�t�F�[�X����ێ����邽�߂̍\����
	�ꉞ�Aclass CSurfaceLockerGuard ���g����悤�ɁA
	���̃N���X�́AISurfaceLocker����h�������Ă����B
*/
public:

	////////////////////////////////////////////////////
	//	�������֐�
	////////////////////////////////////////////////////
	void	Init(void* lpSurfacePtr,LONG lPitch,const SIZE &size
		,int nSurfaceType = 0) {
		m_lpSurface = lpSurfacePtr; m_lPitch = lPitch; m_size = size,
		m_bInit = true, m_nSurfaceType = nSurfaceType;
	}
	/**
		�����ŗ^������̂́A�ȉ��̂��́B�i���Ƃł��ʂɐݒ�ł���j
		����ɁAlock�`unlock�ɍۂ��āA����ɏ������K�v�Ȃ��
		SetLocker�ŁA�����ݒ肷�邱�ƁB
	
		lpSurfacePtr	:	�T�[�t�F�[�X�̃C���[�W�ւ̃|�C���^
			�T�[�t�F�[�X�̃C���[�W�̍���̍��W�̃������A�h���X
		lPitch			:	�P���C���̃o�C�g��
		size			:	Surface�̃T�C�Y
		nSurfaceType	:	�T�[�t�F�[�X�̎��

		0:	�������iOnChangeSurface���܂��Ăяo����Ă��Ȃ�)
		1:	�s���i�ȉ��̈ȊO�j

		//	�ʏ�̃T�[�t�F�[�X
		2:	8bpp
		3:	16(RGB565)
		4:	16(RGB555)
		5:	24(RGB888)
		6:	24(BGR888)
		7:	32(XRGB8888)
		8:	32(XBGR8888)

		//	���T�[�t�F�[�X
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

	///	�T�[�t�F�[�X�̍���̃|�C���^�̐ݒ�^�擾
	void*	GetPtr() const { return m_lpSurface; }
	void	SetPtr(void*lpSurface){ m_lpSurface=lpSurface; }

	///	�T�[�t�F�[�X�̃��X�^�s�b�`
	LONG	GetPitch() const { return m_lPitch; }
	void	SetPitch(LONG lPitch) { m_lPitch = lPitch; }

	///	�T�[�t�F�[�X�̍L����������`�ݒ�^�擾
	SIZE	GetSize() const { return m_size; }
	void	SetSize(const SIZE& size) { m_size = size; }

	///	�T�[�t�F�[�X�̂P�s�N�Z���̃o�C�g��
	int		GetPixelSize() const;

	///	�r�f�I���������DirectDrawSurface��Lock���Ƃ�񂩁H
	///	�i�V�X�e�����������DirectDrawSurface�Ȃ�΁A
	///	����𖳎����āA���ړǂݏ������Ă܂��I�j
	bool	IsLocked() const { return m_bLock; }
		///	lock����Ă��邩�H
	void	SetLock(bool bLock) const {
		CSurfaceInfo* pThis = const_cast<CSurfaceInfo*>(this);
		pThis->m_bLock = bLock;
	}
		///	lock����Ă��邩�̏���ݒ肷��

	///	Lock�`Unlock����
	///	(�Q�d���b�N����ƁACRuntimeException������)
	/**
		�����̊֐��͎��ۂ�const�ł͖������A�]�����T�[�t�F�[�X��lock�`unlock����̂�
		�]�����T�[�t�F�[�X�����������Ă���킯�ł͂Ȃ��̂ŁA�T�[�t�F�[�X��const����
		�ۂĂĂ���ƍl����B����āALock�`Unlock��const�ƍl���邱�Ƃɂ���B
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
		�^����ꂽ��`���A���̃T�[�t�F�[�X�ŃN���b�v���āA�c���`��Ԃ�
		lpRect==NULL�Ȃ�΂��̃T�[�t�F�[�X�S��
	*/
	RECT	GetClipRect(const LPCRECT lpRect) const;

	///	SurfaceType���擾
	int		GetSurfaceType() const { return m_nSurfaceType; }
	void	SetSurfaceType(int n) { m_nSurfaceType = n; }

	///	Surface��Locker��Set/Reset
	smart_ptr<ISurfaceLocker> GetLocker() const { return m_vLocker; }
	void SetLocker(const smart_ptr<ISurfaceLocker>& locker) { m_vLocker = locker; }

	///	�J���[�L�[�̐ݒ�^�擾(���̃T�[�t�F�[�X�p�Ƀs�N�Z���t�H�[�}�b�g��ϊ���������)
	void	SetColorKey(ISurfaceRGB dw){ m_rgbColorKey = dw; }
	ISurfaceRGB	GetColorKey() const { return m_rgbColorKey; }

	///	FillColor�̐ݒ�^�擾(Clear���Ăяo�����Ƃ��͂��̐F�ŃN���A�����)
	void	SetFillColor(ISurfaceRGB dw) { m_rgbFillColor = dw; }
	ISurfaceRGB	GetFillColor() const { return m_rgbFillColor; }

	virtual LRESULT Save(const string& strBitmapFileName,LPCRECT lpRect=NULL);
		///	�摜�̃t�@�C���ւ̏����o��(lpRect�͏����o���̈�BNULL�Ȃ�ΑS��)

	virtual LRESULT SaveYGA(const string& strBitmapFileName,LPCRECT lpRect=NULL,bool bCompress=true);
		///	YGA�摜�`���ł̃t�@�C�������o��(lpRect�͏����o���̈�BNULL�Ȃ�ΑS��)
		///	bCompress==true�Ȃ��CLZSS�ň��k���ĕۑ��B�f�B�t�H���g��true

	CSurfaceInfo(){
		m_bInit			= false; // ����������Ă܂ւ�Init()���Ă���͂�
		m_bLock			= false;
		m_lpSurface		= NULL;
		m_nSurfaceType	=	0;
		SetLocker(smart_ptr<ISurfaceLocker>(new INullSurfaceLocker));
		//	null locker��default��
		m_rgbColorKey	=	0;
		m_rgbFillColor	=	0;
	}

	///	��ʓI��Bltter & Effector

	///	--- �ėp�s�N�Z������(�x��)
	LRESULT GetPixel(int x,int y,ISurfaceRGB&rgba) const;
	/**
		rgba�ŕԂ�B�񃿃T�[�t�F�[�X�̏ꍇ�Argba��a��0���Ԃ�B
		�T�[�t�F�[�X�͈͊O�̂Ƃ��́A��0���Ԃ�
		Lock�`Unlock���s�Ȃ��B
	*/

	LRESULT	GetMatchColor(ISurfaceRGB rgb,DWORD& dw) const;
	/**
		����F���A���̃T�[�t�F�[�X�łǂ��\������邩��Ԃ�
		�񃿃T�[�t�F�[�X�̏ꍇ�Argb�̃��l�͖��������
	*/

	enum EBltType {
	///	�]���̎��(�T�[�t�F�[�X����T�[�t�F�[�X�ւ̓]���ɑ΂��ėL��)
	/**
		�����K���ɂ��ẮAISurface::Blt�`�̂Ƃ�����Q�Ƃ��邱�ƁB
	*/
		eSurfaceBltFast	,			///	�ʏ�Blt�B�����F����
		eSurfaceBlt		,			///	�ʏ�Blt�B�����F�L��
		eSurfaceBltAddColorFast,	///	���F�����B�����F�����B�\�[�X���T�[�t�F�[�X�B
		eSurfaceBltSubColorFast,	///	���F�����B�����F�����B�\�[�X���T�[�t�F�[�X�B
		eSurfaceBltAddColor,		///	���F�����B�����F�L��
		eSurfaceBltSubColor,		///	���F�����B�����F�L��

		///	�ȉ��̂S�͒ǉ��p�����[�^pAdditionalParameter[0]�Ń�(fade��)���w��(0-255)
		eSurfaceBltAddColorFastFade,	///	�~�����Ă�����F�����B�����F�����B�\�[�X���T�[�t�F�[�X�B
		eSurfaceBltSubColorFastFade,	///	�~�����Ă��猸�F�����B�����F�����B�\�[�X���T�[�t�F�[�X�B
		eSurfaceBltAddColorFade,		///	�~�����Ă�����F�����B�����F�L��
		eSurfaceBltSubColorFade,		///	�~�����Ă��猸�F�����B�����F�L��

		eSurfaceBltAlphaFast,			///	���T�[�t�F�[�X���A�񃿁^���T�[�t�F�[�X�֓]��

		///	�ȉ��̂R�͒ǉ��p�����[�^pAdditionalParameter[0]�Ń�(fade��)���w��(0-255)
		eSurfaceBltFastFade,			///	�~�����Ă���BltFast
		eSurfaceBltFade,				///	�~�����Ă���Blt
		eSurfaceBltAlphaFade,			///	�~�����Ă���BltAlpha(�]���������T�[�t�F�[�X��)
		/// ������3�͓]����ƃu�����h�ł͂Ȃ��B�]����͖������āA�����I�ɓ]�����~��������
		///	�g�����Ƃ͂Ȃ��Ǝv���̂Ŗ�����

		///	�ȉ��̂Q�͒ǉ��p�����[�^pAdditionalParameter[0]�Ń�(fade��)���w��(0-255)
		eSurfaceBlendBltFast,			///	�ʏ�u�����h�B�����F����
		eSurfaceBlendBlt	,			///	�ʏ�u�����h�B�����F�L��

		///	�ȉ��̂Q�͒ǉ��p�����[�^pAdditionalParameter[0],[1]�Ń�1,��2(fade��)���w��(0-255)
		eSurfaceBlendConstBltFast,		///	�ʏ�u�����h�B�u�����h���Œ�B�����F����
		eSurfaceBlendConstBlt	,		///	�ʏ�u�����h�B�u�����h���Œ�B�����F�L��

	};

	enum EEffectType {
	///�@�G�t�F�N�g�̎��(���̃T�[�t�F�[�X�ɑ΂��ėL��)
		///	���̃T�[�t�F�[�X�Őݒ肵�Ă���fillcolor(ISurfaceRGB)�ŃN���A
		eSurfaceFill	,		///	�h��Ԃ�

		///	pAdditionalParameter[0]�ŁA�t�F�[�h���[�g���w��(0-255)
		///	255�Ȃ��100%�̈Ӗ��ŁA��ʂɕω��Ȃ�
		eSurfaceFade	,		///	�ʏ�Fade

		///	pAdditionalParameter[0]�ŉ��F�^���F����ISurfaceRGB���w��
		eSurfaceAddColor,		///	���F����
		eSurfaceSubColor,		///	���F����

		eSurfaceFlush,			///	��ʂ��t���b�V���i���]������j

		/// pAdditionalParameter[0]�ŗʎq�����x�����w�肷��
		eSurfaceMosaic,			///	���U�C�N

	};

	struct CBltInfo {
	LPCPOINT	pDstPoint;		///	�]������W(pDstPoint==NULL�Ȃ�΍���)
	LPCSIZE		pDstSize;		///	�]����T�C�Y(pSize==NULL�Ȃ�Γ��{)
	LPCRECT		pSrcRect;		///	�]�������W(pSrcPoint==NULL�Ȃ�΃T�[�t�F�[�X�S��)
	LPCRECT		pDstClip;		///	�]����̃N���b�v
									///	(���͈̔͂���͂ݏo��͈͂ɂ͕`�悳��Ȃ�)
	int			nBasePoint;		// �x�[�X�ʒu
	/**
		�ipSrcPoint�Ŏw�肵�Ă���̂́A�]�����́A�ǂ̓_�Ȃ̂��H�j
		0:���� 1:�^�� 2:�E��
		3:�^�� 4:���� 5:�^�E
		6:���� 7:�^�� 8:�E��
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
		LPCPOINT lpSrcPoint;	///	�]�������W��
		LPCPOINT lpDstPoint;	///	�]������W��
		LPCRECT lpClipRect;		///	�]����N���b�v��`
		bool bContinual;		///	���E���ڑ�����邩�̂悤�ɓ]������̂�
			/// (�ׂ荇�킹�Ƀ��[�t�B���O�]���������̌q���ڂ𒲐�)
		int nAngle;				///	point�̐��Bangel���B�R�Ȃ�΂R�p�`�B

		CMorphInfo(){} // �������̏ꍇ�́A���̂���Init���Ăяo���ď��������邱�ƁB
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
	//	invoke - execution �����̂��߂�helper

	static smart_ptr<void> getWrappedPtr(
		CSurfaceInfo::EBltType type, CSurfaceInfo::CBltInfo*& info, DWORD*& pAdditionalParameter);
	//	�^����ꂽ�|�C���^�̒��g��deep copy���āA����smart_ptr<void>��wrap���ĕԂ�
	//	�����āA�^����ꂽ�|�C���^�̊e�v�f(�|�C���^�܂�)���A
	//	����deep copy���ꂽ�ق��̗v�f���w���悤�ɂ��ĕԂ�

	static smart_ptr<void> getWrappedPtr(
	CSurfaceInfo::EBltType type, CSurfaceInfo::CMorphInfo*& info, DWORD*& pAdditionalParameter);
	//	�^����ꂽ�|�C���^�̒��g��deep copy���āA����smart_ptr<void>��wrap���ĕԂ�
	//	�����āA�^����ꂽ�|�C���^�̊e�v�f(�|�C���^�܂�)���A
	//	����deep copy���ꂽ�ق��̗v�f���w���悤�ɂ��ĕԂ�

	static smart_ptr<void> getWrappedPtr(
	CSurfaceInfo::EEffectType type, LPCRECT& prc, DWORD*& pAdditionalParameter);
	//	�^����ꂽ�|�C���^�̒��g��deep copy���āA����smart_ptr<void>��wrap���ĕԂ�
	//	�����āA�^����ꂽ�|�C���^�̊e�v�f(�|�C���^�܂�)���A
	//	����deep copy���ꂽ�ق��̗v�f���w���悤�ɂ��ĕԂ�

	//------------------------------------------------------------------------

	///	��]�g��k���p��Morph�̍\���̏������p
	///	CMorphInfo�\���̂��A��Q�p�����[�^�ȉ��œn�������Ɋ�Â��ď���������
	LRESULT InitRotateParam(CMorphInfo& info,const CSurfaceInfo* lpSrc, int x,int y, int nAngle,int nRate,int nType, LPCRECT lpSrcRect, LPCRECT lpClipDstRect,
		RECT& rcSrc,POINT aSrcPoint[4],POINT aDstPoint[4]);

	/**
		ISurface�̓����̊֐��̉������֐�

		����ŁABlt��Effect���`���Ă���̂ŁA�h���N���X��
		�n�[�h�E�F�A�A�N�Z�����[�^�ɂ���ē]������̂łȂ���΁A
		�����ɈϏ����邾���œ]������������B
	*/
	LRESULT GeneralBlt(EBltType type,const CSurfaceInfo*pSrc,const CBltInfo* pInfo,DWORD*pAdditionalParameter=NULL);
	LRESULT GeneralEffect(EEffectType type,LPCRECT prc=NULL,DWORD*pAdditionalParameter=NULL);
	LRESULT GeneralMorph(EBltType type,const CSurfaceInfo*pSrc,const CMorphInfo* pInfo,DWORD*pAdditionalParameter=NULL);

	///----- surface�p��iterator --------------------------------------------
	class iterator {
	/**
		�T�[�t�F�[�X��iterator�̎g������
		CSurfaceInfo::iterator it = GetSurfaceInfo()->begin();
		while (it!=GetSurfaceInfo()->end()){
			it.SetRGB(r,g,b,a);
			it++;
		}
		���̋@�\�ɂ��A�ǂ�ȃT�[�t�F�[�X�ɑ΂��Ă������悤��
		�s�N�Z���𑀍�ł���i�x�����ǁj
	*/
	public:
		iterator(const CSurfaceInfo*pInfo,int x=0,int y=0)
			: m_pInfo(pInfo),m_nX(x),m_nY(y) {}

		/**
			RGB�̐ݒ�^�擾
				Lock�`Unlock�͍s�Ȃ�Ȃ��̂ł��̊O����Lock�`Unlock���s�Ȃ�����
		*/
		void	SetRGB(BYTE r,BYTE g,BYTE b,BYTE a=0);
		void	SetRGB(ISurfaceRGB rgb){
			SetRGB((BYTE)((rgb>>16)&0xff),(BYTE)((rgb>>8)&0xff),(BYTE)(rgb&0xff),(BYTE)((rgb>>24)&0xff));
		}

		ISurfaceRGB	GetRGB() const;
		///	�񃿃T�[�t�F�[�X�ɑ΂��ẮA���l��0���Z�b�g�����

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

		///	��r�̂��߂̃I�y���[�^
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

		///	�T�[�t�F�[�X�̃|�C���^�̐ݒ�^�擾
		void SetSurfaceInfo(const CSurfaceInfo*pInfo) { m_pInfo=pInfo; }
		const CSurfaceInfo* GetSurfaceInfo() const { return m_pInfo; }

		///	���ݎw�肵�Ă�����W�̐ݒ�^�擾
		void	SetXY(int x,int y) { m_nX = x; m_nY = y; }
		void	GetXY(int&x,int&y) { x = m_nX; y = m_nY; }

	protected:
		const CSurfaceInfo* m_pInfo;	//	iterate���Ă���T�[�t�F�[�X
		int			m_nX,m_nY;	//	�w���Ă���ꏊ
	};
	iterator begin() const { return iterator(this,0,0); }
	iterator end() const { return iterator(this,0,GetSize().cy); }

private:
	bool	m_bInit;			//	�������ς݂��H
	void*	m_lpSurface;		//	���̃T�[�t�F�[�X�̍���̃s�N�Z���̃|�C���^
	LONG	m_lPitch;			//	���̃T�[�t�F�[�X�̃s�b�`
								//	�i���̃��X�^�[�܂ł̃o�C�g���j
	SIZE	m_size;				//	���̃T�[�t�F�[�X�̃T�C�Y
	bool	m_bLock;			//	���b�N�����H
	int		m_nSurfaceType;		//	�T�[�t�F�[�X�̎�ނ������ɕێ����Ă�����

	///	(�ȉ��́A���̃T�[�t�F�[�X�p�Ƀs�N�Z���t�H�[�}�b�g��ϊ���������)
	ISurfaceRGB	m_rgbColorKey;		//	���̃T�[�t�F�[�X�̃J���[�L�[
	ISurfaceRGB	m_rgbFillColor;		//	���̃T�[�t�F�[�X�̓h��Ԃ����Ɏg���F

	smart_ptr<ISurfaceLocker> m_vLocker;	//	surface��lock���邽�߂̃I�u�W�F�N�g

};

class ISurface {
/**
	Surface�̊��N���X�B
	��	class ISurfaceDefault , class CFastPlane ���Q�l�̂��ƁB
*/
public:
	///	typedef DWORD ISurfaceRGB;
	///	���̃T�[�t�F�[�X�ł́A���ʂ��āA����RGB�l�Ŏw�肷��

	static inline ISurfaceRGB makeRGB(DWORD r,DWORD g,DWORD b,DWORD a=0)
		#ifdef USE_EXCEPTION
			throw(CRuntimeException)
		#endif
	/**
		IPlane�Ŏg�����߂�RGB�s�N�Z���l�̐���
		�F	������́AARGB8888(DIB32�Ǝd�l�͓���)�̐���
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
		IPlane�Ŏg�����߂�RGB�s�N�Z���l�̕���
		�F	ARGB8888(DIB32�Ǝd�l�͓���)�Ɠ���
	*/
	{
		a = (BYTE)((rgb >> 24) & 0xff);
		r = (BYTE)((rgb >> 16) & 0xff);
		g = (BYTE)((rgb >>	8) & 0xff);
		b = (BYTE)((rgb		 ) & 0xff);
	}

	virtual int GetType() const = 0;
	/**
		�T�[�t�F�[�X�̃^�C�v��Ԃ�(RTTI���ǂ�)
		0:	Null Device
		1:	CFastPlane
		2:	CDIB32
	*/

	//////////////////////////////////////////////////////////////////////
	///	�ėpBltter
	//////////////////////////////////////////////////////////////////////

	virtual LRESULT GeneralBlt(CSurfaceInfo::EBltType type,const CSurfaceInfo*pSrc,const CSurfaceInfo::CBltInfo* pInfo,DWORD*pAdditionalParameter=NULL)
	///	�ĉ����ꂽBltter
	/**
		type : �]�����[�h(�����F�]���ł��邾�Ƃ��A���F�����ł��邾�Ƃ�)
		pSrc : �]������CSurfaceInfo
		pInfo: �]���p�����[�^
		pAdditionalParameter : �ǉ��ŕK�v�ƂȂ�p�����[�^�ւ̃|�C���^
	*/
	{
		///	�Ϗ����邾���ŗǂ��Ǝv���
		return GetSurfaceInfo()->GeneralBlt(type,pSrc,pInfo,pAdditionalParameter);
	}

	//////////////////////////////////////////////////////////////////////////
	/// ---- ��{�I��Bltter�͎��O�ɗp�ӂ��Ă���
	//////////////////////////////////////////////////////////////////////////

	/**
		Bltter�̖����K���F

		��j
			MorphBltAlphaFastFade
			(1)	  (2) (3) (4) (5)

			BlendConstBltFast
			(6)	 (7)  (1) (4)

			(1)...Morph...���[�t�B���O�n�ł��邱�Ƃ��Ӗ�����
			(2)...Blt.....�]���֐��ł��邱�Ƃ��Ӗ�����
			(3)...Alpha...�]������alpha�T�[�t�F�[�X�ł��邱�Ƃ��Ӗ�����
					���ꂪ���Ă��Ȃ��ꍇ�Aalpha�T�[�t�F�[�X��
					�T�|�[�g���Ă��邩�ǂ����́A���̃��\�b�h�ɂ��
			(4)...Fast....�����F�͖�������邱�Ƃ��Ӗ�����
			(5)...Fade....�]���̂Ƃ��Ƀt�F�[�h�����w��ł��邱�Ƃ��Ӗ�����
				�t�F�[�h���i�������j�́ABYTE(0-255)�Ŏw�肵�A
				255���w�肷���100%(��������),0���w�肷���0%
				(���̂Ƃ��A�]�����͍��Ƃ��Ĉ�����)
			(6)...Blend...�u�����h�n�]���ł��邱�Ƃ��Ӗ�����B
					�]����̃s�N�Z���ƃu�����h����i�������킹��j
					���Ƃ���64(=25%�Ȃ��Ȃ�255�~25%=64)���w�肷��΁A
					�]����25%�Ɠ]����75%�̔䗦�Ńu�����h�����B
			(7)...Const...�u�����h�̔䗦���Œ�ł��邱�Ƃ��Ӗ�����
					�u�����h�䗦�́ABYTE(0-255)���ӂ��Ƃ�B
					���̂ӂ������P�C���Q�Ƃ���΁A
					�]����@���@�]�����~���P�{�]����~���Q
				�ƂȂ�B�������A���P�{���Q���Q�T�T�łȂ��Ă͂Ȃ�Ȃ�
	*/

	LRESULT Blt(const ISurface*pSrc,int x=0,int y=0,LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
	/**
		�����F�L��]��
		x,y		 : �]������W
		pDstSize : �]����T�C�Y(NULL�Ȃ�Γ��{)
		pSrcRect : �]������`
		pDstClip : �]����N���b�s���O��`(���͈̔͂ɂ͕`�悳��Ȃ�)
		nBasePoint:�]�����ʒu

		//	class CSurfaceInfo::CBltInfo ���Q�Ƃ̂��ƁB
  */
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBlt,pSrc->GetConstSurfaceInfo(),&info);
	}

	///	�����F�����]��
	LRESULT BltFast(const ISurface*pSrc,int x=0,int y=0,LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
	///	�p�����[�^�̈Ӗ��́ABlt�Ɠ���
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltFast,pSrc->GetConstSurfaceInfo(),&info);
	}

	///	���F����(�����F�Ȃ�)
	///	�p�����[�^�̈Ӗ��́ABlt�Ɠ���
	///	�\�[�X���T�[�t�F�[�X�B
	LRESULT AddColorBltFast(const ISurface*pSrc,int x=0,int y=0,LPCSIZE pDstSize=NULL,
		LPCRECT	 pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltAddColorFast,pSrc->GetConstSurfaceInfo(),&info);
	}
	///	���F����(�����F����)
	///	�p�����[�^�̈Ӗ��́ABlt�Ɠ���
	LRESULT AddColorBlt(const ISurface*pSrc,int x=0,int y=0,LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltAddColor,pSrc->GetConstSurfaceInfo(),&info);
	}
	///	���F����(�����F�Ȃ�)
	///	�p�����[�^�̈Ӗ��́ABlt�Ɠ���
	///	�\�[�X���T�[�t�F�[�X�B
	LRESULT SubColorBltFast(const ISurface*pSrc,int x=0,int y=0,LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltSubColorFast,pSrc->GetConstSurfaceInfo(),&info);
	}
	///	���F����(�����F����)
	///	�p�����[�^�̈Ӗ��́ABlt�Ɠ���
	LRESULT SubColorBlt(const ISurface*pSrc,int x=0,int y=0,LPCSIZE pDstSize=NULL,
		LPCRECT	 pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltSubColor,pSrc->GetConstSurfaceInfo(),&info);
	}

	/// -------- �ȉ��A���t���̉��F�����B
	///	���@alpha��DWORD�œn���Ă��邪�ABYTE(0-255)�ˁB

	///	�~�����F����(�����F�Ȃ�)
	///	�p�����[�^�̈Ӗ��́ABlt�Ɠ���
	///	�\�[�X���T�[�t�F�[�X�B
	LRESULT AddColorBltFastFade(const ISurface*pSrc,int x=0,int y=0,DWORD nAlpha=255,
		LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltAddColorFastFade,pSrc->GetConstSurfaceInfo(),&info,&nAlpha);
	}
	///	�~�����F����(�����F����)
	///	�p�����[�^�̈Ӗ��́ABlt�Ɠ���
	LRESULT AddColorBltFade(const ISurface*pSrc,int x=0,int y=0,DWORD nAlpha=255,
		LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltAddColorFade,pSrc->GetConstSurfaceInfo(),&info,&nAlpha);
	}
	///	�~�����F����(�����F�Ȃ�)
	///	�p�����[�^�̈Ӗ��́ABlt�Ɠ���
	///	�\�[�X���T�[�t�F�[�X�B
	LRESULT SubColorBltFastFade(const ISurface*pSrc,int x=0,int y=0,DWORD nAlpha=255,
		LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltSubColorFastFade,pSrc->GetConstSurfaceInfo(),&info,&nAlpha);
	}
	///	�~�����F����(�����F����)
	///	�p�����[�^�̈Ӗ��́ABlt�Ɠ���
	LRESULT SubColorBltFade(const ISurface*pSrc,int x=0,int y=0,DWORD nAlpha=255,
		LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltSubColorFade,pSrc->GetConstSurfaceInfo(),&info,&nAlpha);
	}

	///	���T�[�t�F�[�X����̉摜�̓]���n
	///	�p�����[�^�̈Ӗ��́ABlt�Ɠ���
	LRESULT BltAlphaFast(const ISurface*pSrc,int x=0,int y=0,LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBltAlphaFast,pSrc->GetConstSurfaceInfo(),&info);
	}

	///	�p�����[�^�̈Ӗ��́ABlt�Ɠ���
	///	BlendBltFast��BltFastFade�͓�������
	LRESULT BlendBltFast(const ISurface*pSrc,int x=0,int y=0,DWORD nAlpha=255,
		LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBlendBltFast,pSrc->GetConstSurfaceInfo(),&info,&nAlpha);
	}
	///	�p�����[�^�̈Ӗ��́ABlt�Ɠ���
	///	BlendBlt��BltFade�͓�������
	LRESULT BlendBlt(const ISurface*pSrc,int x=0,int y=0,DWORD nAlpha=255,
		LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBlendBlt,pSrc->GetConstSurfaceInfo(),&info,&nAlpha);
	}
	///	�p�����[�^�̈Ӗ��́ABlt�Ɠ���
	LRESULT BlendConstBltFast(const ISurface*pSrc,int x=0,int y=0,DWORD nAlpha1=255,DWORD nAlpha2=255,
		LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		DWORD anAlpha[] = { nAlpha1,nAlpha2 };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBlendConstBltFast,pSrc->GetConstSurfaceInfo(),&info,&anAlpha[0]);
	}
	///	�p�����[�^�̈Ӗ��́ABlt�Ɠ���
	LRESULT BlendConstBlt(const ISurface*pSrc,int x=0,int y=0,DWORD nAlpha1=255,DWORD nAlpha2=255,
		LPCSIZE pDstSize=NULL,
		LPCRECT pSrcRect=NULL,LPCRECT	pDstClip=NULL,int nBasePoint=0){
		POINT DstPoint = { x , y };
		DWORD anAlpha[] = { nAlpha1,nAlpha2 };
		CSurfaceInfo::CBltInfo info(&DstPoint,pDstSize,pSrcRect,pDstClip,nBasePoint);
		return GeneralBlt(CSurfaceInfo::eSurfaceBlendConstBlt,pSrc->GetConstSurfaceInfo(),&info,&anAlpha[0]);
	}

	/////////////////////////////////////////////////////////////
	///	�⏕Bltter

	///	���R�ȓ]��(Blt,yga�摜�Ȃ��BltAlphaFast)
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
	///	--- Surface�̃G�t�F�N�^

	virtual LRESULT GeneralEffect(CSurfaceInfo::EEffectType type,LPCRECT prc=NULL,DWORD*pAdditionalParameter=NULL)
	///	�ĉ����ꂽEffector
	/**
		type : �]�����[�h(�ʏ�Fade�]���ł��邾�Ƃ��A���F�����ł��邾�Ƃ�)
		prc	 : �G�t�F�N�g���������`�͈�(NULL�Ȃ�ΑS��)
		pAdditionalParameter : �ǉ��ŕK�v�ƂȂ�p�����[�^�ւ̃|�C���^
			type : eEffectFade �Ȃ�΁A[0]�� Fade��(BYTE:0-255)
			type : eAddColor,eSubColor �Ȃ��[0]�͂��̒萔(ISurfaceRGB)
	*/
	{
		return GetSurfaceInfo()->GeneralEffect(type,prc,pAdditionalParameter);
	}

	//////////////////////////////////////////////////////////////////////////
	/// ---- ��{�I��effecter�͎��O�ɗp�ӂ��Ă���
	///	�����́A���̃T�[�t�F�[�X�ɑ΂��ăG�t�F�N�g��������
	///	LPCRECT prc�́ANULL�Ȃ�΃T�[�t�F�[�X�S��
	//////////////////////////////////////////////////////////////////////////

	///	�t�F�[�h alpha�̓t�F�[�h��(0-255)
	virtual LRESULT FadeEffect(BYTE alpha,LPCRECT prc=NULL){
		DWORD dw = alpha; // �����n���炵�����ǁA�܂�������
		return GeneralEffect(CSurfaceInfo::eSurfaceFade,prc,&dw);
	}

	///	���F����
	virtual LRESULT AddColorFast(ISurfaceRGB c,LPCRECT prc=NULL){
		return GeneralEffect(CSurfaceInfo::eSurfaceAddColor,prc,(DWORD*)&c);

	}

	///	���F����
	virtual LRESULT SubColorFast(ISurfaceRGB c,LPCRECT prc=NULL){
		return GeneralEffect(CSurfaceInfo::eSurfaceSubColor,prc,(DWORD*)&c);
	}

	///	���U�C�N
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
	///	�ĉ����ꂽBltter
	/**
		type : �]�����[�h(�����F�]���ł��邾�Ƃ��A���F�����ł��邾�Ƃ�)
		pSrc : �]������CSurfaceInfo
		pInfo: �]���p�����[�^
		pAdditionalParameter : �ǉ��ŕK�v�ƂȂ�p�����[�^�ւ̃|�C���^
	*/
	{
		///	�Ϗ����邾���ŗǂ��Ǝv���
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
	///	--- Surface�Ƃ��āA�펯�I�ɕK�v�Ȃ���

	///	--- �����F�̐ݒ�^�擾
	virtual	LRESULT SetColorKey(ISurfaceRGB rgb);
	virtual ISurfaceRGB GetColorKey() const;

	virtual LRESULT SetColorKeyPos(int x,int y);
	///�@(x,y)�̓_�𓧉߃L�[�ɐݒ肷��

	virtual LRESULT SetColorKeyRGB(int r,int g,int b){ return SetColorKey(makeRGB(r,g,b)); }
	///	RGB�Ŕ����F���w�肷��Brgb�͂��ꂼ��0�`255

	///	--- �T�[�t�F�[�X�̃T�C�Y�ݒ�^�擾
	virtual LRESULT GetSize(int& x,int& y) const =0;
	///		�������̃T�[�t�F�[�X�Ȃ��GetSize�́A-1��Ԃ��i���̂Ƃ�x==y==0�j
	virtual LRESULT SetSize(int x,int y)=0;
	/**
		SetSize�́A�������݃T�C�Y�ƈႤ�̂ł���΁A���炽��Create�����
	*/

	///		�T�[�t�F�[�X�̎�ނ�Ԃ�
	virtual int GetSurfaceType() const
	{ return GetConstSurfaceInfo()->GetSurfaceType(); }

	///		���T�[�t�F�[�X���ǂ�����Ԃ�
	bool	IsAlpha() const { return GetSurfaceType()>=10; }

	///////////////////////////////////////////////////////
	///	�T�[�t�F�[�X�ɑ΂��ăr�b�g�}�b�v�f�[�^��ǂݍ��݂�
	///	�T�|�[�g����⏕���\�b�h
	/**
		���̃T�C�Y�̎w��^�C�v�̃T�[�t�F�[�X���쐬����
		(��ɁA�����I�ɗ��p����)
		(sx,sy) : �T�[�t�F�[�X�̃T�C�Y
		�����Ŏw�肷��nType�́A�쐬����T�[�t�F�[�X�̃^�C�v(CSurfaceInfo�Ɠ���)
		�i�����ō쐬���ꂽ�T�[�t�F�[�X�́A�N���A����Ȃ��j
	*/
	virtual LRESULT CreateSurfaceByType(int sx,int sy,int nType)=0;

	/**
		�w�肳�ꂽnType�ŃT�[�t�F�[�X���A��L��CreateSurfaceByType�ō쐬���A
		�r�b�g�}�b�v�f�[�^��ǂݍ��ށB(YGA�Ή�)
		YGA�摜�̂Ƃ��́A����nType�Ŏw�肳�ꂽ���̂����T�[�t�F�[�X�Ŗ����Ȃ�΁A		�����I�ɂ���ɑΉ����郿�T�[�t�F�[�X�ɂ���
	*/
	virtual LRESULT LoadByType(const string& strBitmapName,int nType);

	///	virtual void ClearMSB();
	/*
		�r�f�I�J�[�h�̃o�O�΍�ōŏ�ʃo�C�g��ׂ�
		(HDC�o�R��XRGB8888,XBGR8888�ɓǂݍ��񂾂Ƃ��ɁA�N���A����K�v������)
		�ˁ@���O�œǂݍ��ނ��Ƃɂ����B�ǂݍ��߂Ȃ��Ƃ��́A24bpp��DIB���쐬����
		��������ǂݍ��ށB����āA��L�̃o�O�̉e���͎󂯂Ȃ��̂ł��̃��\�b�h��
		�p�~����
	*/

	///////////////////////////////////////////////////////

	virtual LRESULT		Clear(LPCRECT lpRect=NULL);			///	��`�N���A
	virtual void		SetFillColor(ISurfaceRGB c);		///	Clear����F���w�肷��(Default==RGB(0,0,0))
	virtual ISurfaceRGB	GetFillColor() const;				///	Clear����F�̎擾
		///	�˂������A���t���T�[�t�F�[�X�ɑ΂���Clear�́A
		///		�� == 0�ɂȂ邱�Ƃ��ۏ؂������̂Ƃ���
	/**
		�����ŁA�ݒ肳�ꂽFillColor�́A�ēx�ݒ肳���܂ŗL���B
		�f�B�t�H���g��RGB(0,0,0) ���Ȃ킿���ł���B
	*/

	///////////////////////////////////////////////////////

	///�@--- �r�b�g�}�b�v�֘A
	virtual LRESULT Load(const string& strBitmapFileName){
		///	�摜�̓ǂݍ���
		return 0;
	}
	virtual LRESULT Save(const string& strBitmapFileName,LPCRECT lpRect=NULL){
		///	�摜�̃t�@�C���ւ̏����o��(lpRect�͏����o���̈�BNULL�Ȃ�ΑS��)
		return GetSurfaceInfo()->Save(strBitmapFileName,lpRect);
	}
	virtual LRESULT SaveYGA(const string& strBitmapFileName,LPCRECT lpRect=NULL,bool bCompress=true){
		///	YGA�摜�`���ł̃t�@�C�������o��(lpRect�͏����o���̈�BNULL�Ȃ�ΑS��)
		///	bCompress==true�Ȃ��CLZSS�ň��k���ĕۑ��B�f�B�t�H���g��true
		return GetSurfaceInfo()->SaveYGA(strBitmapFileName,lpRect,bCompress);
	}

	virtual LRESULT	Release(){ return 0; }
	///	�ǂݍ��񂾉摜�̉��

	//////////////////////////////////////////////////////////////////////////
	///	�T�[�t�F�[�X���
	///	(���ꂳ������΁A�����擾���ē]�����邱�Ƃ��o����)

		///	���g��ύX���Ȃ��o�[�W����
	virtual const CSurfaceInfo* GetConstSurfaceInfo() const
	{ return m_vSurfaceInfo.get(); }

		///	���g��ύX����o�[�W����
	virtual CSurfaceInfo* GetSurfaceInfo()
	{ return m_vSurfaceInfo.get(); }

	//////////////////////////////////////////////////////////////////////////

	virtual smart_ptr<ISurface> clone()=0;
	/**
		�����^�̃I�u�W�F�N�g�𐶐����ĕԂ��B
		�������A�T�[�t�F�[�X�̓��e�̓R�s�[�����킯�ł͂Ȃ��B
	*/

	ISurface();
	virtual ~ISurface() {}	//	merely place holder

protected:
	///	�T�[�t�F�[�X�����������\����
	smart_ptr<CSurfaceInfo>	m_vSurfaceInfo;
	smart_ptr<ISurfaceLocker> m_vSurfacelocker;

	// ---------------------- ���߃L�[�֘A ----------------------------
	//	default(ResetColorKey�̓���)�ł�m_bUsePosColorKey==true
	//		m_nColorKeyX=m_nColoyKeyY=0
	//	���Ȃ킿�A����̃h�b�g�������F
	void	ResetColorKey();
	//	�J���[�L�[�̃��Z�b�g(�T�[�t�F�[�X�̍쐬���ɂ�����Ăяo���Ɨǂ�)
	bool	m_bUsePosColorKey;			//�@true:�ʒu�w��^��ColorKey
										//	false:�F�w��^��ColorKey
	int		m_nColorKeyX,m_nColorKeyY;	//�@�ʒu�w��^�@���߃J���[�L�[
	ISurfaceRGB m_rgbColorKey;			//	�J���[�L�[�̐F
	/*
		�������A�ʒu�w��^�̏ꍇ�A���̃T�[�t�F�[�X�ł͐F�[�x���s�����Ă���
		�{���̐F�Ƃ͌���Ȃ��̂ŁA���̂��Ɖ�ʂ̐F�[�x��[�������Ƃ��ɁA
		���̈ʒu�ɂ���s�N�Z���͈قȂ�F�ɂȂ��Ă��܂��\��������B������
		�ʒu�w��^�J���[�L�[�̏ꍇ�ɂ��Ȃ��āA�T�[�t�F�[�X�̐F�[�x���ύX��
		�Ȃ����Ƃ��ɁA�ēx�A�ȉ���UpdateColorKey���\�b�h���Ăяo���Ȃ��Ă�
		�Ȃ�Ȃ��킯�ł���B
	*/
	LRESULT	UpdateColorKey();	//	���A�p�i�����I�Ɏg�p�j


	///	����T�[�t�F�[�X�ɑΉ�����YGA�T�[�t�F�[�X��Ԃ�
	int	GetYGASurfaceType(int nSurfaceType) const;

};	//	�Ƃ肠�����A���ꂾ������Ώ\��������O�O�G

class ISurfaceFactory {
/*
	ISurface��factory
*/
public:
	virtual smart_ptr<ISurface> CreateInstance() = 0;
	virtual ~ISurfaceFactory(){}
};

/////////////////////////////////////////////////////////////////////////////

class CSurfaceNullDevice : public ISurface {
/**
	class IPlane ��NullDevice
	�v����ɁA�������Ȃ��Bvirtual proxy(smart_ptr<ISurface>)����g���Ƃ��̂��߁B
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
///		YGA�̃w�b�_�[
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
