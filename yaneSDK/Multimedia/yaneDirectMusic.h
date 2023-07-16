// yaneDirectInput.h
//	 This is DirectInput wrapper.
//		programmed by yaneurao(M.Isozaki) '99/06/18
//		modified by yaneurao '00/02/29

#ifndef __yaneDirectMusic_h__
#define __yaneDirectMusic_h__


#include "../Auxiliary/yaneCOMBase.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class CDirectSound;
class CDirectMusic {
/**
	DirectMusic�̏������̂��߂̃N���X
	class CMIDIOutputDM �ɂĎg�p
*/
public:
	static bool CanUseDirectMusic();
	/**
		DirectMusic���g������Ȃ̂��ǂ����𒲂ׂĕԂ�
		���ׂ�͍̂ŏ��̂P��ڂ̌Ăяo���݂̂Ȃ̂ŉ��x�Ăяo���Ă��ǂ��B
	*/

	/**
		�R���X�g���N�^�Ŏ����I��DirectMusic�̏����������݂�̂ŁA
		�ȉ��̊֐��ŁA�K�v�Ȃ��̂��擾����΂���ł����B
	*/
	CCOMObject<IDirectMusicPerformance*>* GetDirectMusicPerformance()
		{ return& m_vDirectMusicPerformance;}
	//	IDirectMusicPerformace���ADirectMusic�̕��

	CCOMObject<IDirectMusicLoader*>* GetDirectMusicLoader()
		{ return& m_vDirectMusicLoader;}
	//	���ۂ̍Đ���IDirectMusicLoader��IDirectMusicSegment��
	//	�쐬���āA�������S���B

	int	GetStatus() const { return m_nStatus; }
	/**
		DirectInput�̏������󋵂ɂ��āA���U���g��Ԃ�
		0:ok(DirectX6�ȍ~)
		1,2,3:failure(DirectX6�������ĂȂ� or
			DirectMusic�������܂��̎��s)
	*/

	CDirectMusic(CDirectSound* p=NULL);
	/**
		�֘A����DirectSound�I�u�W�F�N�g���w�肵�Ă��΁A
		����DirectMusic�́A����DirectSound�̃v���C�}���𗘗p����
		�Đ����邱�Ƃ��o����B������w�肵�Ȃ���΁A�f�B�t�H���g�ł���
		22kHz�ōĐ����邱�ƂɂȂ�B
	*/

	virtual ~CDirectMusic();

protected:
	LPDIRECTSOUND m_pDirectSound;	//	CDirectSound�o�R�Ŏ擾�������
	LPDIRECTSOUND GetDirectSound() const { return m_pDirectSound; }

	CCOMObject<IDirectMusicPerformance*>	m_vDirectMusicPerformance;
	IDirectMusic* m_lpDMusic;
	//	IDirectMusic�́A�P��DirectSound�ƌ������邽�߂̂���
	CCOMObject<IDirectMusicLoader*>	m_vDirectMusicLoader;
	int		m_nStatus;	//	���̕ϐ��̈Ӗ��ɂ��Ă�GetStatus���Q�Ƃ̂���

};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif
