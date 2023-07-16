
// yaneFastDraw.h
/**
	 This is a extended DirectDraw wrapper using the template library.
		programmed by yaneurao(M.Isozaki) '01/10/01-'01/10/06

		�`��Ɋւ���A�������V���v���Ȏ����B
		yaneuraoGameSDK 3rd style�����Ƃ��́A
		�����ƁACDirectDraw,CPlane,CDIBDraw��j�����āA
		���̃N���X�ɂ��邾�낤�B

		����قǁA����A�`��̍����ƂȂ�ł��낤�N���X�B

		���C���́A��������Ɋm�ۂ����T�[�t�F�[�X�B
		�Z�J���_�����A��������Ɋm�ۂ����T�[�t�F�[�X�B
		�v���C�}���i�ꍇ�ɂ���Ă̓Z�J���_�����j�������ADirectDrawSurface�B
		��{�I�ɂ́A���������g���킯�B

		�������A�t���X�N���[�����́A�t���b�s���O�T�[�t�F�[�X���T�|�[�g�B
			�˂��܂胁���b�g�������Ȃ��̂ŁA�m���T�|�[�g�ɂ��܂����O�O�G
		���̂Ƃ��́AVRAM��Ɋm�ۂ��ꂽ�v���C�}�����Q�����āA
		�Z�J���_���̓�������Ɋm�ۂ���Ă���A�Ƃ����\�t�g�I��
		�g���v���o�b�t�@�����O���s�Ȃ��B

		�܂��A�`��̂��߂̓]���́Afunctor����g���čs�Ȃ��B
		�ŏI�I�ɂ́A�����I�ɁAPentium/MMX�̃A�Z���u���ɂ��R�[�h��
		�������邩���m��Ȃ��B

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

	///	---	IAppDraw�̃����o
	virtual int GetType() const { return 1; }

	//////////////////////////////////////////
	//	�f�B�X�v���C���[�h�̕ύX

	LRESULT		SetDisplay(bool bFullScr=false,int nSizeX=0,int nSizeY=0,int nColorDepth=0);
	void		GetDisplay(bool&bFullScr,int &nSizeX,int &nSizeY,int &nColorDepth);
	void		GetSize(int &nSizeX,int &nSizeY);	// GetDisplayMode��x,y�����������
	bool		IsFullScreen();

	//	Begin�`End�Ńf�B�X�v���C���[�h��ύX����B
	void		BeginChangeDisplay();
	void		TestDisplayMode(int nSX,int nSY,bool bFullScr=false,int nColor=0);
	LRESULT		EndChangeDisplay();

	//////////////////////////////////////////

//	void	UseHardwareFlip(bool bEnable) { m_bUseFlip=true; }
	//	�n�[�h�E�F�A�t���b�s���O���g���̂��H
	//		�f�B�t�H���g�ł�true
	//		�����A�ύX����Ȃ�΁A�f�B�X�v���C���[�h�̕ύX�ɐ�s���āA
	//		���̊֐����Ăяo�����ƁB
	//	FastDraw�ɂ�Flip�@�\�͑��x�I�ɂ́A���܂�A�h�o���e�[�W������
	//	�܂��A�Z�J���_�����r�f�I�������ɔz�u���Ă��܂��ƁA
	//	���ꂼ��̓]���ɂ�����Alock�`unlock�̃R�X�g�������ł��Ȃ��D�D
	//	�P�ɁA�`�掞�́A�t���b�J�����O�i������j�̖h�~���炢�̈Ӗ�
	//	�������ADirectDraw�́AWindows GDI�𖳎����ĕ`����s�Ȃ��̂�
	//	���j���[��������ɍ쓮���Ȃ��Ȃ�̂Œ��ӂ��K�v

	void	UseDibSectionIn256(bool bEnable){}
		//	�ˁ@����ł́A���T�|�[�g
	//	256�F���[�h�ɂ����āARGB555(16bpp)���g�����ǂ����B
	//		�f�B�t�H���g�ł�true
	//		�����A256�F���[�h�ł́A�p���b�g���g���A
	//		�Ǝ��̔������������s�Ȃ��Ȃ��false�ɂ��Ă������ƁB

	//////////////////////////////////////////
	//	�v���C�}���ƃZ�J���_���̎擾

	CFastPlane*		GetPrimary()		{ return &m_Primary; }
		//	���@�`�悷�邽�߂̃v���C�}���T�[�t�F�[�X
/*
	CFastPlane*		GetPrimary2()		{ return &m_Primary2; }
		//	���t���X�N���[���ŁA�n�[�h�E�F�A�t���b�s���O���s�Ȃ��Ƃ�
		//		���̃T�[�t�F�[�X������ʂɂȂ�
*/
	CFastPlane*		GetSecondary256()	{ return &m_Secondary256; }
		//	���Q�T�U�F���[�h�̂Ƃ��ɂ́A��������Z�J���_��(RGB555)����A
		//�@���̃T�[�t�F�[�X�ɓ]�����Ă���`�悷��̂�

	CFastPlane*		GetSecondary()		{ return &m_Secondary;}
		//	��������Ɋm�ۂ��ꂽ�A�Z�J���_���T�[�t�F�[�X
		//		�f�B�t�H���g�ł�DirectDrawSurface�ƂȂ�B
		//	�������A256�F���[�h�ɂ����ẮARGB555�̃T�[�t�F�[�X�I

	//////////////////////////////////////////
	//	Secondary->Primary�v���[���̓]��
	
	virtual void	OnDraw(LPCRECT lpRect=NULL,bool bLayerCallBack=true);
	//	Secondary->Primary�ւ̓]��

	//	��L�]���̂Ƃ��ɁA�ւ���Ă���p�����[�^
	void	SetOffset(int ox,int oy);	//	�Z�J���_���̓]���I�t�Z�b�g
	void	SetBrightness(int nBright); //	�t�F�[�h�䗦(0-256)

	//////////////////////////////////////////
	//	���C���̊Ǘ�

	//	Layer�̃��X�g��Ԃ�
	ILayerList* GetLayerList() { return &m_LayerList; }
	ILayerList* GetAfterLayerList() { return &m_AfterLayerList; }
	ILayerList* GetHDCLayerList() { return &m_HDCLayerList; }

	//	�����C���Ƃ́AOnDraw�̂Ƃ��A���Ȃ킿�A�Z�J���_������v���C�}����
	//	�]������Ƃ��ɁA�R�[���o�b�N�Ƃ��ČĂяo�����̂����A����́A
	//	�Z�J���_����HDC���擾�ł���T�[�t�F�[�X�iDirectDrawSurface�j
	//	�ł��邱�Ƃ����̑O��ƂȂ�B

	//////////////////////////////////////////
	/// ----	���܂�
	int	GetBpp(); // ���݂̉��bpp�̎擾

	///	�����DirectDrawPtr���擾����
	LPDIRECTDRAW	GetDDraw() { return m_DirectDrawBase.Get(); }

	/// ���̃N���X���Ǘ����Ă���T�[�t�F�[�X�ւ̃|�C���^���擾
	list_chain<CFastPlane>* GetFastPlaneList() { return& m_listFastPlane; }

	//////////////////////////////////////////
	//	property..

	int		GetMenuHeight();
	//	���j���[�����Ă���Ȃ�΁A���̃��j���[������Ԃ�

	//////////////////////////////////////////

	CFastDraw();
	virtual ~CFastDraw();

protected:
	CDirectDraw m_DirectDrawBase;

	//	��ʃ��[�h�̕ύX�֘A
	LRESULT		ChangeDisplay(bool bFullScr);	//	���݂̃f�B�X�v���C���[�h�𔽉f������
	bool	m_bFullScr;			//	�t���X�N���[�����[�h���H
	int		m_nScreenXSize;		//	��ʃT�C�Y
	int		m_nScreenYSize;
	int		m_nScreenColor;		//	���bpp
	bool	m_bDisplayChanging; //	�𑜓x�ύX��
	bool	m_bChangeDisplayMode;// �f�B�X�v���C���[�h��ύX���邩 // �ǉ�

	//	--- OnDraw�ŗ��p��������
	//		��ʂ̃t�F�[�h�֘A
	int		m_nBrightness;
	//		�]���I�t�Z�b�g
	int		m_nSecondaryOffsetX;		//	�Z�J���_���̓]���I�t�Z�b�g��
	int		m_nSecondaryOffsetY;

	HWND	m_hWnd;				//	��������CAppInitializer����擾����ƒx���Ȃ�̂�

	int		m_nMenu;			//	���j���[����
	//	���̃N���X�̃R���X�g���N�^�Ōv�Z���ĕێ����Ă���

	//	for Layer management
	CLayerList	m_LayerList;
	CLayerList	m_AfterLayerList;
	CLayerList	m_HDCLayerList;

	//////////////////////////////////////////
	//	�T�[�t�F�[�X�̃��X�g�`�F�b�N
	//	�i���[�U�[�́A�����I�ɂ́A�Ăяo���Ȃ��j
	void		CheckSurfaceLost();
	//	�T�[�t�F�[�X�̃��X�g�`�F�b�N�i�����p�j
	bool	m_bLostSurface;

/*
	//	�t���X�N���[������filpping surface���g�p����̂��H
	//	default == true
	bool	m_bUseFlip;
	//	���ۂɁA�t���b�s���O�T�[�t�F�[�X�͍쐬���ꂽ�̂��H
	bool	m_bUseFlip2;
*/

	//	�ꎞ���̂�
	void	RealizeBrightness(int nBrightness);

	///////////////////////////////////////////////////////

	list_chain<CFastPlane> m_listFastPlane;

	///////////////////////////////////////////////////////
	//	CFastPlane�̃R���X�g���N�^�ɂ�CFastDraw*���K�v
	//	�܂�CFastPlane�́ACFastDraw��list_chain��add���悤�Ƃ���
	//	����āA�������CFastDraw�̃R���X�g���N�^�ŏ���������Ƃ��ɂ́A
	//	���list_chain���������ς݂łȂ��Ă͂Ȃ�Ȃ��B
	
	//	�v���C�}���T�[�t�F�[�X�ƃZ�J���_���T�[�t�F�[�X
	CFastPlane	m_Primary;	//	
//	CFastPlane	m_Primary2;	//	Flipping����Ƃ��̂��߂̃T�[�t�F�[�X
	CFastPlane	m_Secondary256;	//	256�F���[�h�̎��̉B���Z�J���_��
	CFastPlane	m_Secondary;
	CDirectDrawClipper m_WindowClipper;

	CFastDraw* GetThis() { return this; }

	//	�T�[�t�F�[�X�̃��X�g�����o���āA���X�g�A��������
	void	RestoreAll();


	//	�f�B�X�v���C�𑜓x��ύX����Ƃ��̔r��lock�p
	CCriticalSection m_cs;
	CCriticalSection* GetCriticalSection(){return &m_cs;}

	// overriden from CWinHook
	LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif // USE_FastDraw

#endif
