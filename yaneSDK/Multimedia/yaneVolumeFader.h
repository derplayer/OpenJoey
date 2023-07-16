//	yaneVolumeFader.h :
//		programmed by yaneurao '00/03/12
//

#ifndef __yaneVolumeFader_h__
#define __yaneVolumeFader_h__


#include "yaneAudioMixer.h"
#include "../Timer/yaneIntervalTimer.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class CVolumeFader {
/**
	�I�[�f�B�I�̃~�L�V���O�p�N���XCAudioMixer�𗘗p���āA
	MIDI�ACD�AWAVE�ɑ΂��ċ[���I�ȃt�F�[�h�C���^�t�F�[�h�A�E�g����������B
	CD�ɑ΂���t�F�[�h�C�����́A�Z�p�I�ɂ��H�ł���B

	class CAudioMixer���Q�Ƃ��邱�ƁB

	�t�F�[�h�́ACIntervalTimer�ɂ�邪�A�����CAppBase::IsThreadValid��
	�Ăяo���Ȃ���CallBack����Ȃ��B
	����āACVolumeFader��FadeOut/FadeIn���Ăяo�������Ƃ́ASleep���Ă�
	�Ӗ����Ȃ��BCAppBase::IsThreadValid���Ăяo���`�惋�[�v�̂Ȃ���
	�g�����ł���B

	�t�F�[�h�A�E�g�́A���̌�A�w�肵���b���ŁA�t�F�[�h�A�E�g����������̂�
	���̂��ƁA�ȍĐ����~�����AResetVolume���Ăяo���āA���Ƃ̃��H�����[��
	�ɖ߂��Ă��K�v������B

	�܂��A���̃N���X�����H�����[�����t�F�[�h�����Ă��Ȃ��Ƃ��ɁA
	���H�����[���ύX���s�Ȃ�ꂽ�Ƃ��̂��Ƃ��l�����āA
	�t�F�[�h���J�n����Ƃ��ɁA���H�����[�����`�F�b�N���āA����
	���H�����[����class CAudioMixer �̏������H�����[���Ƃ��Đݒ肷��
	�������s�Ȃ��Ă���B
*/
public:
	LRESULT ResetVolume(eAudioMix e);			///	�����{�����[���l�ɖ߂�

	////////////////////////////////////////////////////////////////////////
	/// dwTime[ms]��Ƀ{�����[���̃t�F�[�h�͊�������
	LRESULT FadeOut(eAudioMix e,int dwTime);	///	�t�F�[�h�A�E�g���J�n������
	LRESULT FadeIn(eAudioMix e,int dwTime);		///	�t�F�[�h�C�����J�n������
	bool	IsFade(eAudioMix e)const;			///	�t�F�[�h���Ȃ̂��H

	LRESULT FadeOut(eAudioMix e,int dwTime,
		const smart_ptr<function_callback>& fn);
	/**
		fadeout �I�����ɁA�����ʒm���邽�߂̃R�[���o�b�N���w��ł���
	*/

	LRESULT FadeIn(eAudioMix e,int dwTime,
		const smart_ptr<function_callback>& fn);
	/**
		fadein �I�����ɁA�����ʒm���邽�߂̃R�[���o�b�N���w��ł���
	*/

	void	StopFade();							///	�t�F�[�h���~

	CVolumeFader() : m_bTimer(false) {}
	virtual ~CVolumeFader() { Stop(); }
protected:
	class CVolumeFadeInfo {
	public:
		CTimer		m_Timer;		//	Fade�̂��߂�Timer
		DWORD		m_dwWholeTime;	//	Fade���J�n����I��������܂ł̎���
		bool		m_bFadeIn;		//	FadeIn���Ȃ̂�?
		bool		m_bFadeOut;		//	FadeOut���Ȃ̂�?
		bool		m_bChanged;
	//	���݁A���̃N���X�����H�����[�����������Ă����Ԃ��H
	//	�˂��̃t���O��false�̂Ƃ��ɁAFadeIn/FadeOut���w�肳���΁A
	//	���݂̃��H�����[�������̂Ƃ��̃I���W�i�����H�����[���Ƃ���
	//	���f������

		smart_ptr<function_callback> m_fn;	//	Fade�I������callback

		CVolumeFadeInfo() {
			m_bFadeIn = m_bFadeOut = m_bChanged = false;
		}
	};

	CAudioMixer		m_AudioMixer;
	CVolumeFadeInfo	m_FadeInfo[4];

	void	TimerProc();	//	�R�[���o�b�N�����֐�
	void	Start();		//	�R�[���o�b�N�J�n
	void	Stop();			//	�R�[���o�b�N�I��
	bool	m_bTimer;		//	�R�[���o�b�N���[�v��
	bool	IsTimer() const { return m_bTimer; }

	bool	CheckCallBack(eAudioMix e);
	//	fade�̏I���^�C�~���O�Ȃ̂ŁA�K�v����΃R�[���o�b�N���s�Ȃ�
	//	callback���s�Ȃ����Ƃ�true
	LRESULT _FadeOut(eAudioMix e,int dwTime,bool bCheckCallBack);
	LRESULT _FadeIn(eAudioMix e,int dwTime,bool bCheckCallBack);
	//	bCheckCallBack�̗L��Ɩ����i�����Ŏg�p�j
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif
