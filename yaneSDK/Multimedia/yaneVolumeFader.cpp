#include "stdafx.h"
#include "yaneVolumeFader.h"
#include "../AppFrame/yaneAppManager.h"
#include "../AppFrame/yaneAppBase.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

LRESULT CVolumeFader::ResetVolume(eAudioMix e){
	CheckCallBack(e);

	CVolumeFadeInfo& info = m_FadeInfo[(int)e];
	info.m_bFadeIn		= false;
	info.m_bFadeOut		= false;
	info.m_bChanged		= false;
	m_AudioMixer.SetVolumeOrg(e);		//	�����{�����[���l�ɖ߂�
	return 0;
}

bool	CVolumeFader::CheckCallBack(eAudioMix e){
//	�K�v����΃R�[���o�b�N���s�Ȃ�
//	callback���s�Ȃ����Ƃ���true

	CVolumeFadeInfo& info = m_FadeInfo[(int)e];
	if ((info.m_bFadeIn || info.m_bFadeOut) && !info.m_fn.isNull()){
		info.m_fn->run();
		info.m_fn.Delete();
		return true;
	}
	return false;
}

LRESULT CVolumeFader::FadeOut(eAudioMix e,int dwTime){
	return _FadeOut(e,dwTime,true);
}

LRESULT CVolumeFader::_FadeOut(eAudioMix e,int dwTime,bool bCheckCallBack){
	if (bCheckCallBack){
		CheckCallBack(e);
	}
	//	�t�F�[�h�A�E�g���J�n������

	CVolumeFadeInfo& info = m_FadeInfo[(int)e];

	info.m_Timer.Reset();
	info.m_dwWholeTime	= dwTime;
	info.m_bFadeIn		= false;
	info.m_bFadeOut		= true;

	//	���݂̃��H�����[���ʂ����X�g�A���Ă����K�v������
	//	(���H�����[���R���g���[�����ύX���ꂽ�\��������̂�)
	if (!info.m_bChanged) { m_AudioMixer.RestoreVolume(e); }

	//	�C���^�[�o���^�C�}�[�𔭓�������O�O
	if (!IsTimer()) {
		Start();
	}	//	���ꂭ�炢�ŏ\�����H
	info.m_bChanged		= true;
	TimerProc();
	//	���܂����P��ڂ��Ăяo���Ȃ��ƁA���̌Ăяo�������
	//	�Đ����Ă���ꍇ�A�����̉��ʂ��X�V�O�̒l�ŏo�͂����
	return 0;
}

LRESULT CVolumeFader::FadeIn(eAudioMix e,int dwTime){
	return _FadeIn(e,dwTime,true);
}

LRESULT CVolumeFader::_FadeIn(eAudioMix e,int dwTime,bool bCheckCallBack){
	if (bCheckCallBack){
		CheckCallBack(e);
	}

	//	�t�F�[�h�C�����J�n������
	CVolumeFadeInfo& info = m_FadeInfo[(int)e];
	info.m_Timer.Reset();
	info.m_dwWholeTime	= dwTime;
	info.m_bFadeIn		= true;
	info.m_bFadeOut		= false;

	//	���݂̃��H�����[���ʂ����X�g�A���Ă����K�v������
	//	(���H�����[���R���g���[�����ύX���ꂽ�\��������̂�)
	if (!info.m_bChanged) { m_AudioMixer.RestoreVolume(e); }

	//	�C���^�[�o���^�C�}�[�𔭓�������O�O
	if (!IsTimer()) {
		Start();
	}	//	���ꂭ�炢�ŏ\�����H
	info.m_bChanged = true;
	TimerProc();
	//	���܂����P��ڂ��Ăяo���Ȃ��ƁA���̌Ăяo�������
	//	�Đ����Ă���ꍇ�A�����̉��ʂ��X�V�O�̒l�ŏo�͂����
	return 0;
}

void	CVolumeFader::StopFade(){
	Stop();
	for(int i=0;i<4;i++){
		CheckCallBack((eAudioMix)i);
		m_FadeInfo[i].m_bFadeIn	 = false;
		m_FadeInfo[i].m_bFadeOut = false;
	}
}

void	CVolumeFader::TimerProc(){
	if (!m_AudioMixer.IsSuccessInit()) return ;

	int nReset = 0;
	for (int i=0;i<4;i++){
		CVolumeFadeInfo& info = m_FadeInfo[i];
		if (info.m_bFadeIn) {
			double d = (double)info.m_Timer.Get() / info.m_dwWholeTime;
			if (d>1) {
				d=1;
				CheckCallBack((eAudioMix)i);
				info.m_bFadeIn = false;

				info.m_bChanged = false;
				//	�ύX�I���ˎ���fade�̂Ƃ��ɂ́A
				//	���݂̃��H�����[�����������H�����[���Ƃ��ĕۑ�
			}
			m_AudioMixer.SetVolumeRel((eAudioMix)i,d);
		} else if (info.m_bFadeOut){
			double d = (double)info.m_Timer.Get() / info.m_dwWholeTime;
			if (d>1) {
				d=1;
				if (CheckCallBack((eAudioMix)i)){
					d = 0;	//	�R�[���o�b�N���s�Ȃ����̂ŏ������H�����[���ɖ߂�
					info.m_bChanged = false;
					//	���̊֐��𔲂������ƁA���̒l�͏������H�����[���Ƃ݂Ȃ��ėǂ�
				}
				info.m_bFadeOut = false;
			}
			d = 1-d;
			m_AudioMixer.SetVolumeRel((eAudioMix)i,d);
		} else {
			nReset++;
		}
	}
	//	���ׂẴt�F�[�h���I����Ă��邩�H
	if (nReset==4) Stop();	//	�^�C�}�[�̒�~
}

bool CVolumeFader::IsFade(eAudioMix e) const {
	return m_FadeInfo[(int)e].m_bFadeIn || m_FadeInfo[(int)e].m_bFadeOut;
}

LRESULT CVolumeFader::FadeOut(eAudioMix e,int dwTime,
		const smart_ptr<function_callback>& fn){
	CheckCallBack(e);
	m_FadeInfo[(int)e].m_fn = fn;
	return _FadeOut(e,dwTime,false);
}

LRESULT CVolumeFader::FadeIn(eAudioMix e,int dwTime,
		const smart_ptr<function_callback>& fn){
	CheckCallBack(e);
	m_FadeInfo[(int)e].m_fn = fn;
	return _FadeIn(e,dwTime,false);
}

//////////////////////////////////////////////////////////////////////////////

//	�R�[���o�b�N�̊J�n
void	CVolumeFader::Start(){
	if (m_bTimer) return ; // ���łɃ^�C�}�܂���Ƃ�I
	m_bTimer = true;
	smart_ptr<function_callback> fn(
		function_callback_v::Create(&CVolumeFader::TimerProc,this)
	);
	//	�R�[���o�b�N�̓o�^
	CAppManager::GetMyApp()->GetIntervalTimer()->RegistCallBack(
		this,	//	�R�[���o�b�N�o�^����Ă�����̂�
				//	�폜���邽�߂̎��ʎq
		20,		//	���ڂ̃R�[���o�b�N��20ms��
		20,		//	20ms���Ƃ�
		0,		//	endless�R�[���o�b�N
		fn
	);
}

void	CVolumeFader::Stop(){
	//	�R�[���o�b�N�̍폜
	if (IsTimer()){
		CAppManager::GetMyApp()->GetIntervalTimer()->DeleteCallBack(this);
		m_bTimer = false;
	}
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
