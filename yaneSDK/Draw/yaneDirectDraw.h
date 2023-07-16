// yaneDirectDraw.h
//	 This is DirectDraw wrapper.

#ifndef __yaneDirectDraw_h__
#define __yaneDirectDraw_h__


#include "../Auxiliary/yaneCOMBase.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CDirectDraw {
/**
	���̃N���X�́ADirectDraw��COM�I�u�W�F�N�g��wrapper
*/
public:

	LPDIRECTDRAW Get() const { return m_vDirectDraw.get(); }
	/**
		�R���X�g���N�^�Ŏ����I��DirectDraw�̏����������݂�̂ŁA
		���̊֐��ŁAIDirectDraw���擾����΂���ł����B
	*/

	int	GetStatus() const { return m_nStatus; }
	/**
		DirectDraw�̏������󋵂ɂ��āA���U���g��Ԃ�
		0:ok
		1,2:DirectX3�������Ă��Ȃ�
		3:HEL�������Ɏ��s�BHAL�������ɂ͐����B��DirectDraw�͎g����
		4:HEL,HAL�̏������Ɏ��s�BDirectDraw�́A���̏�Ԃł͎g���Ȃ��B
	*/

	CDirectDraw(bool bEmulationOnly=false);
	/**
		bEmulationOnly == true�Ȃ��HEL�œ���
		2D��p�̃Q�[���Ȃ�΁AHEL�œ��������ق����ǂ�
	*/
	
	virtual ~CDirectDraw() {}

protected:
	CCOMObject<LPDIRECTDRAW>	m_vDirectDraw;
	CCOMObject<LPDIRECTDRAW>*	GetDirectDraw() { return& m_vDirectDraw;}
	int		m_nStatus;	//	���̕ϐ��̈Ӗ��ɂ��Ă�GetStatus���Q�Ƃ̂���
};

class CDirectDrawSurfaceManager {
/**
	�s�N�Z���t�H�[�}�b�g���ׂ�˂�I
*/
public:
	int GetSurfaceType(LPDIRECTDRAWSURFACE);
	///	������T�[�t�F�[�X�̃T�[�t�F�[�X�^�C�v�𒲂ׂ�
	/**
		0:	�T�[�t�F�[�X�̃^�C�v�̎擾�Ɏ��s
		1:	�T�[�t�F�[�X�̃^�C�v�̓��肪�o���Ȃ�����
			�i�ȉ��̈ȊO���Ǝv����)
		2:	8bpp
		3:	16(RGB565)
		4:	16(RGB555)
		5:	24(RGB888)
		6:	24(BGR888)
		7:	32(XRGB8888)
		8:	32(XBGR8888)

		10:	16+a4(ARGB4565)	//	���ۂ́A���ꂪ�Ԃ��Ă��邱�Ƃ͖���
		11:	16+a4(ARGB4555)	//	���ۂ́A���ꂪ�Ԃ��Ă��邱�Ƃ͖���
		12:	32(ARGB8888)	//	���ۂ́A���ꂪ�Ԃ��Ă��邱�Ƃ͖���
		13:	32(ABGR8888)	//	���ۂ́A���ꂪ�Ԃ��Ă��邱�Ƃ͖���
	*/
};

class CDirectDrawClipper {
public:
	LRESULT SetClipper(LPDIRECTDRAW lpDraw,LPDIRECTDRAWSURFACE lpPrimary,HWND hWnd);
	//	�������N���b�v����

	void	Release();						//	�N���b�p���������

	CDirectDrawClipper() : m_lpClipper(NULL) {}
	virtual ~CDirectDrawClipper() { Release();}

protected:
	LPDIRECTDRAWCLIPPER m_lpClipper;
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneDirectDraw_h__
