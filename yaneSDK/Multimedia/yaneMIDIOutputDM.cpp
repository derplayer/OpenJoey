
#include "stdafx.h"

#include "yaneMIDIOutputDM.h"
#include "yaneDirectMusic.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

#define sign(val) (val>0?1:(val<0?-1:0))	//	符号を返す * yane

///////////////////////////////////////////////////////////////////////////////

CMIDIOutputDM::CMIDIOutputDM(CDirectMusic* p) : m_pDirectMusic(p)
{
	m_lpDMSegment = NULL;
	m_lpDMSegmentState = NULL;
	m_bPaused = 0;
	m_bLoopPlay = false;
	m_mtPosition = 0;
}

CMIDIOutputDM::~CMIDIOutputDM()
{
	Close();
}

////////////////
//	オープン
///////////////
LRESULT CMIDIOutputDM::Open(const string& sFileName)
{
	if (!CDirectMusic::CanUseDirectMusic()) return -1;

	Close();

	//	元のファイルを読み込み
	if(m_File.Read(sFileName)!=0)	{
		Err.Out("CMIDIOutputDM::Openで読み込むファイルが無い %s", sFileName.c_str());
		return 1;
	}

	DMUS_OBJECTDESC desc;
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.pbMemData = (BYTE*)m_File.GetMemory(); // (BYTE*)m_alpMIDICache;
	desc.llMemLength = m_File.GetSize();
	desc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_MEMORY | DMUS_OBJ_LOADED;

	HRESULT hr;
	hr = GetDirectMusic()->GetDirectMusicLoader()->get()->GetObject(
			&desc,IID_IDirectMusicSegment,(void**)&m_lpDMSegment);
	if (FAILED(hr)){
		m_lpDMSegment = NULL;
		return 2;
	}
	hr = m_lpDMSegment->SetParam(GUID_StandardMIDIFile,0xffffffff,0,0,
		(void*)GetDirectMusic()->GetDirectMusicPerformance()->get());
	if (FAILED(hr)) return 3;
	hr = m_lpDMSegment->SetParam(GUID_Download,0xffffffff,0,0,
		(void*)GetDirectMusic()->GetDirectMusicPerformance()->get());
	if (FAILED(hr)) return 4;

	return 0;
}

//////////
//	クローズ
//////////
LRESULT CMIDIOutputDM::Close()
{
	m_File.Close();	//	delete temporary file
	Stop();	// stopしないとCloseできないよーん

	RELEASE_SAFE(m_lpDMSegmentState);

	if (m_lpDMSegment!=NULL) {
		//	参照の解放
		//	GetDirectMusic()->GetDirectMusicLoader()->ReleaseObject((IDirectMusicObject*)m_lpDMSegment);
		//	これが無いとバグる＾＾；
		GetDirectMusic()->GetDirectMusicLoader()->get()->ClearCache(CLSID_DirectMusicSegment);

		//	DLSのアンロード
		m_lpDMSegment->SetParam(GUID_Unload,0xffffffff,0,0,(void*)GetDirectMusic()->GetDirectMusicPerformance()->get());
		m_lpDMSegment->Release();
	}

	return 0;
}

//////////
//	演奏処理
/////////

LRESULT CMIDIOutputDM::Play()
{
	if (!CDirectMusic::CanUseDirectMusic()) return -1;
	if (m_lpDMSegment==NULL) return -2;

	m_bPaused = 0;

	Stop();

	//	再生開始
	if (!m_bLoopPlay) {
		m_lpDMSegment->SetRepeats(0);	//	繰り返さない
	} else {
		m_lpDMSegment->SetRepeats((DWORD)-1);	//	回数∞
	}
	m_mtPosition = 0;
	m_lpDMSegment->SetStartPoint(m_mtPosition);

	HRESULT hr = GetDirectMusic()->GetDirectMusicPerformance()->get()->PlaySegment(
		m_lpDMSegment,DMUS_SEGF_DEFAULT,0,&m_lpDMSegmentState);

	if (FAILED(hr)) return 1;

	return 0;
}

LRESULT CMIDIOutputDM::Replay()
{
	if (!CDirectMusic::CanUseDirectMusic()) return -1;
	if (m_lpDMSegment==NULL) return -2;

	if (m_bPaused==0) return 0;		//	pauseしてへんて！
	if (--m_bPaused!=0) return 0;	//	参照カウント方式

	//	なぜか再生中なので何もせずに帰る
	if (IsPlay()) return 0;

	Stop();

	//	再生再開
	m_lpDMSegment->SetStartPoint(m_mtPosition);	//	m_mtPositionは前回Pauseした位置
	if (!m_bLoopPlay) {
		m_lpDMSegment->SetRepeats(0);	//	繰り返さない
	} else {
		m_lpDMSegment->SetRepeats((DWORD)-1);	//	回数∞
	}
	HRESULT hr = GetDirectMusic()->GetDirectMusicPerformance()->
		get()->PlaySegment(
			m_lpDMSegment,DMUS_SEGF_DEFAULT,0,&m_lpDMSegmentState);

	if (FAILED(hr)) return 1;

	m_bPaused = 0;
	return 0;
}

LRESULT CMIDIOutputDM::Stop()
{
	if (!CDirectMusic::CanUseDirectMusic()) return -1;

	m_bPaused += sign(m_bPaused);	//	必殺技:p
	if (!IsPlay()) return 0;			//	すでに停止している

	//	再生中のをpauseしたならば
	m_bPaused = 1;

	//	現在の状態読み出して停止
	if (m_lpDMSegment!=NULL && m_lpDMSegmentState!=NULL) {
		GetDirectMusic()->GetDirectMusicPerformance()->
			get()->Stop(m_lpDMSegment,m_lpDMSegmentState,0,0);
		if (FAILED(m_lpDMSegmentState->GetSeek(&m_mtPosition))) {	//	m_mtPositionは前回Pauseした位置
			m_mtPosition = 0;
		}
		RELEASE_SAFE(m_lpDMSegmentState);
	}

	return 0;
}

LONG	CMIDIOutputDM::GetCurrentPos() const {
	if (!CDirectMusic::CanUseDirectMusic()) return -1;

	if (m_lpDMSegmentState==NULL) return -2;

	// 現在の再生ポジションを得る
	MUSIC_TIME mt;
	if (FAILED(m_lpDMSegmentState->GetSeek(&mt))) return -3;
	//	↑ここで得たのは、テンポによる相対時刻なので↓で変換してやる必要がある

	REFERENCE_TIME rt;
	if (FAILED(GetDirectMusic()->GetDirectMusicPerformance()->get()->MusicToReferenceTime(
		mt,&rt))) return -4;
	REFERENCE_TIME rt2;
	if (FAILED(GetDirectMusic()->GetDirectMusicPerformance()->get()->MusicToReferenceTime(
		0,&rt2))) return -4;
	rt-=rt2;	//	恐ろしいことに、ベースからの値ではないので
				//	このような補正項が必要になるDirectMusicのバグのような気がしなくもない。

	return (LONG)(rt/10000);	//	これで[ms]単位（REFERENCE_TIMEは分解能ありすぎ！（笑））
}

LRESULT CMIDIOutputDM::SetCurrentPos(LONG lPos){
	HRESULT hr;
	if (!CDirectMusic::CanUseDirectMusic()) return -1;
	if ( m_lpDMSegment == NULL ) return -1;
	MUSIC_TIME mt;
	LONG lLen = GetLength();
	hr = m_lpDMSegment->GetLength(&mt);
	if (lLen==0) return -2; // なんで０やねん．．

//	なんでだめなんだろう…
//	REFERENCE_TIME rt = lPos*10000;
//	hr = GetDirectMusic()->GetDirectMusicPerformance()->ReferenceToMusicTime(rt,&mt);
	Pause();
	//m_mtPosition;
	m_mtPosition = MUSIC_TIME(lPos * mt / lLen);
	Replay();

	return 0;
}

LRESULT CMIDIOutputDM::SetVolume(LONG volume){
	if (!CDirectMusic::CanUseDirectMusic()) return -1;
	LONG v = volume;
	GetDirectMusic()->GetDirectMusicPerformance()->get()->SetGlobalParam(GUID_PerfMasterVolume,&v,sizeof(v));
	return 0;
}


LONG	CMIDIOutputDM::GetVolume() const
{
	if (!CDirectMusic::CanUseDirectMusic()) return -1;
	LONG v;
	GetDirectMusic()->GetDirectMusicPerformance()->get()->GetGlobalParam(GUID_PerfMasterVolume,(LPVOID)&v,sizeof(v));
	return v;
}

LRESULT CMIDIOutputDM::SetLoopPlay(bool bLoop){
	m_bLoopPlay = bLoop;
	return 0;
}

bool CMIDIOutputDM::IsPlay() const {
	if (!CDirectMusic::CanUseDirectMusic()) return false;
	if (m_lpDMSegmentState==NULL) return false;
	return GetDirectMusic()->GetDirectMusicPerformance()->get()->IsPlaying(m_lpDMSegment,m_lpDMSegmentState)==S_OK;
}

LRESULT	CMIDIOutputDM::LoopPlay(){
	if (m_bLoopPlay) {
		return Play();
	} else {
		return Stop();
	}
}

LONG	CMIDIOutputDM::GetLength() const {
	if (!CDirectMusic::CanUseDirectMusic()) return -1;

	if (m_lpDMSegment==NULL) return -2;

	MUSIC_TIME mt;
	if ( FAILED(m_lpDMSegment->GetLength(&mt))) return -3;

	REFERENCE_TIME rt;
	if (FAILED(
		GetDirectMusic()->GetDirectMusicPerformance()->get()->MusicToReferenceTime(mt,&rt)
		)) return -4;
	REFERENCE_TIME rt2;
	if (FAILED(GetDirectMusic()->GetDirectMusicPerformance()->get()->MusicToReferenceTime(
		0,&rt2))) return -4;
	rt-=rt2;
	return (LONG)(rt/10000);
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
