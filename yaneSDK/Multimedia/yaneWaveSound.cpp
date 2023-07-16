#include "stdafx.h"

#include "yaneWaveOutput.h"
#include "yaneSoundParameter.h"
#include "yaneWaveStaticSound.h"
#include "yaneWaveStreamSound.h"

#include "yaneWaveSound.h"
//#include "../Window/yaneDebugWindow.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

CWaveSound::CWaveSound(IWaveOutput* p) : m_pOutput(p)
{
	// �f�t�H���g��false
	m_bStreamPlay = false;
	if (m_bStreamPlay) {
		m_vSound.Add(new CWaveStreamSound(GetOutput()));
	} else {
		m_vSound.Add(new CWaveStaticSound(GetOutput()));
	}
	m_bFadeStop	= false;
}

CWaveSound::~CWaveSound()
{
}

void CWaveSound::SetStreamPlay(bool b)
{
	if (m_bStreamPlay==b) return;
	m_bStreamPlay = b;

	// �����擾
	string filename = GetSound()->GetFileName();
	bool IsLoopPlay = GetSound()->IsLoopPlay();
	LONG lVol = GetSound()->GetVolume();
//	LONG lPos = GetSound()->GetCurrentPos();
	bool IsPlay = GetSound()->IsPlay();

	// �����p�ς�
	GetSound()->Close();

	// �ǂ��炩�𐶐�
	if (m_bStreamPlay) {
		m_vSound.Add(new CWaveStreamSound(GetOutput()));
	} else {
		m_vSound.Add(new CWaveStaticSound(GetOutput()));
	}

	// ����ݒ�
	GetSound()->SetLoopPlay(IsLoopPlay);
	GetSound()->Open(filename);
//	GetSound()->SetCurrentPos(lPos);
	if (IsPlay) GetSound()->Play();
	GetSound()->SetVolume(lVol);
}

////////////////////////////////////////////////////////////////////////

LRESULT CWaveSound::Open(const string& strFileName) {
	StopFade();
	return GetSound()->Open(strFileName);
}

LRESULT CWaveSound::Close()	{ StopFade(); return GetSound()->Close(); }
LRESULT CWaveSound::Play() { StopFade(); return GetSound()->Play(); }
LRESULT CWaveSound::Replay() { StopFade(); return GetSound()->Replay(); }

bool	CWaveSound::IsPlay() const { return GetSound()->IsPlay(); }
LRESULT	CWaveSound::SetLoopPlay(bool bLoop) { return GetSound()->SetLoopPlay(bLoop); }
bool	CWaveSound::IsLoopPlay() const { return GetSound()->IsLoopPlay(); }

LONG	CWaveSound::GetLength() const { return GetSound()->GetLength(); }
LRESULT CWaveSound::SetCurrentPos(LONG lPos) { StopFade(); return GetSound()->SetCurrentPos(lPos); }

LONG	CWaveSound::GetCurrentPos() const { return GetSound()->GetCurrentPos(); }

string	CWaveSound::GetFileName() const { return GetSound()->GetFileName(); }

LRESULT CWaveSound::Restore() { return GetSound()->Restore(); }

LRESULT CWaveSound::Pause() {
	//	pause����Ƃ��́A���͑�������Đ�����킯�Ŗڂ��Ԃ�H
	StopFade();
	return GetSound()->Pause();
}

}}

///----------	�ȉ��̊֐��́AFade�����̂��߂ɁA�׍H���K�v
#include "../Timer/yaneIntervalTimer.h"
#include "../AppFrame/yaneAppManager.h"
#include "../AppFrame/yaneAppBase.h"
//	�C���^�[�o���^�C�}��p���āA�t�F�[�h�����Ȃ���̒�~

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

void	CWaveSound::StopFade(){
	//	Fade�����Ă���̂Ȃ�΁A�����j������
	if (IsFadeStop()){
		CAppManager::GetMyApp()->GetIntervalTimer()->DeleteCallBack(this);
		SetFade(false);
		InnerSetVolume(m_lOriginalVolume);
	}
}

LRESULT CWaveSound::Stop() {
	if (IsFadeStop()) { return 0; } // ���ɒ�~���߂����Ƃ���H

#ifndef __SoundFadeStop__

	m_lOriginalVolume = GetVolume();
	InnerStop();

#else
	//	fade�����Ȃ����~���������

	m_lOriginalVolume = GetVolume();
	/*
		DSBVOLUME_MAX�i�����Ȃ��j : 0
		DSBVOLUME_MIX�i�����j : -10000
	*/

	//	10ms�ゲ�Ƃ�Volume��fade
	LONG lVol = m_lOriginalVolume;
	int t=0;
	for(;lVol>-10000;t++){
		lVol -= 1000;
		if (lVol < -10000) lVol = -10000;
		if (t==0){
			InnerSetVolume(lVol);
		} else {
			smart_ptr<function_callback> fn(
				function_callback_r<LRESULT>::Create(
					&CWaveSound::InnerSetVolume,this,lVol
				)
			);
			CAppManager::GetMyApp()->GetIntervalTimer()->RegistCallBack(
				this,t*10,0,1,fn);
			///	10ms�~n ���Ƃ�volume��������
		}
	}
	{
		smart_ptr<function_callback> fn(
			function_callback_r<LRESULT>::Create(
				&CWaveSound::InnerStop,this
			)
		);
		CAppManager::GetMyApp()->GetIntervalTimer()->RegistCallBack(
			this,t*10,0,1,fn);
	}
	SetFade(true);

#endif
	return 0;
}

LRESULT CWaveSound::InnerStop() {
	LRESULT lr = GetSound()->Stop();
	lr |= InnerSetVolume(m_lOriginalVolume);	//	���H�����[���߂�
	return lr;
}

LRESULT CWaveSound::SetVolume(LONG volume) {
	if (IsFadeStop()){
		//	Fade�I����Ă���ɂ��Ă񂩁I
		m_lOriginalVolume = volume;
		return 0;
	}
	return GetSound()->SetVolume(volume);
}

LRESULT CWaveSound::InnerSetVolume(LONG volume) {
	return GetSound()->SetVolume(volume);
}

LONG	CWaveSound::GetVolume() const {
	if (IsFadeStop()){
		return m_lOriginalVolume;
	}
	return GetSound()->GetVolume();
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
