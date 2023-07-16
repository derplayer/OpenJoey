// yaneDirectSound.h
//	 This is DirectSound wrapper.
//		programmed by yaneurao(M.Isozaki) '99/06/18
//				modified by yaneurao	  '00/03/02
//				modified by ENRA		  '02/03/22

#ifndef __yaneDirectSound_h__
#define __yaneDirectSound_h__


#include "../Auxiliary/yaneCOMBase.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class CDirectSound
{
/**
	DirectSound�̏������̂��߂̃N���X
*/
public:
	CDirectSound();
	virtual ~CDirectSound();

	static bool CanUseDirectSound();
	/**
		DirectSound���g������Ȃ̂��ǂ����𒲂ׂĕԂ�
		���ׂ�͍̂ŏ��̂P��ڂ̌Ăяo���݂̂Ȃ̂ŉ��x�Ăяo���Ă��ǂ��B
	*/

	/// DirectSoundObject���擾����
	LPDIRECTSOUND Get() { return GetObject()->get(); }

	int	GetStatus() const { return m_nStatus; }
	/**
		DirectSound�̏������󋵂ɂ��āA���U���g��Ԃ�
		0: success
		1: failure - �T�E���h�J�[�h����������
		2: failure - ���������o���Ȃ�����
		3: failure - �������x�����ݒ�o���Ȃ�����
	*/

	LRESULT SetCooperativeLevel(DWORD dwLevel);
	/**
		DirectSound�̋������x����ݒ肷��
		DSSCL_NORMAL 
		DSSCL_PRIORITY
		DSSCL_EXCLUSIVE �̂����ꂩ��ݒ肷��
	*/

protected:
	// DirectSoundObject�̎擾
	CCOMObject<LPDIRECTSOUND>*	GetObject() { return &m_vDirectSound; }
	CCOMObject<LPDIRECTSOUND>	m_vDirectSound;
	// �ۗL���Ă���E�B���h�E�n���h��
	HWND	m_hWnd;
	// GetStatus�֐����Ԃ���������
	int		m_nStatus;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif
