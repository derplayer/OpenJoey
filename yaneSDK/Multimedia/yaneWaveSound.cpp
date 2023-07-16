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
	// デフォルトでfalse
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

	// 情報を取得
	string filename = GetSound()->GetFileName();
	bool IsLoopPlay = GetSound()->IsLoopPlay();
	LONG lVol = GetSound()->GetVolume();
//	LONG lPos = GetSound()->GetCurrentPos();
	bool IsPlay = GetSound()->IsPlay();

	// もう用済み
	GetSound()->Close();

	// どちらかを生成
	if (m_bStreamPlay) {
		m_vSound.Add(new CWaveStreamSound(GetOutput()));
	} else {
		m_vSound.Add(new CWaveStaticSound(GetOutput()));
	}

	// 情報を設定
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
	//	pauseするときは、次は続きから再生するわけで目をつぶる？
	StopFade();
	return GetSound()->Pause();
}

}}

///----------	以下の関数は、Fade処理のために、細工が必要
#include "../Timer/yaneIntervalTimer.h"
#include "../AppFrame/yaneAppManager.h"
#include "../AppFrame/yaneAppBase.h"
//	インターバルタイマを用いて、フェードさせながらの停止

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

void	CWaveSound::StopFade(){
	//	Fadeさせているのならば、それを破棄する
	if (IsFadeStop()){
		CAppManager::GetMyApp()->GetIntervalTimer()->DeleteCallBack(this);
		SetFade(false);
		InnerSetVolume(m_lOriginalVolume);
	}
}

LRESULT CWaveSound::Stop() {
	if (IsFadeStop()) { return 0; } // 既に停止命令がきとるやん？

#ifndef __SoundFadeStop__

	m_lOriginalVolume = GetVolume();
	InnerStop();

#else
	//	fadeさせながら停止させる実装

	m_lOriginalVolume = GetVolume();
	/*
		DSBVOLUME_MAX（減衰なし） : 0
		DSBVOLUME_MIX（無音） : -10000
	*/

	//	10ms後ごとにVolumeをfade
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
			///	10ms×n ごとにvolumeを下げる
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
	lr |= InnerSetVolume(m_lOriginalVolume);	//	ヴォリューム戻す
	return lr;
}

LRESULT CWaveSound::SetVolume(LONG volume) {
	if (IsFadeStop()){
		//	Fade終わってからにしてんか！
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
