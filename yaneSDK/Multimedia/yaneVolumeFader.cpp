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
	m_AudioMixer.SetVolumeOrg(e);		//	初期ボリューム値に戻す
	return 0;
}

bool	CVolumeFader::CheckCallBack(eAudioMix e){
//	必要あらばコールバックを行なう
//	callbackを行なったときにtrue

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
	//	フェードアウトを開始させる

	CVolumeFadeInfo& info = m_FadeInfo[(int)e];

	info.m_Timer.Reset();
	info.m_dwWholeTime	= dwTime;
	info.m_bFadeIn		= false;
	info.m_bFadeOut		= true;

	//	現在のヴォリューム量をリストアしておく必要がある
	//	(ヴォリュームコントロールが変更された可能性があるので)
	if (!info.m_bChanged) { m_AudioMixer.RestoreVolume(e); }

	//	インターバルタイマーを発動させる＾＾
	if (!IsTimer()) {
		Start();
	}	//	これくらいで十分やろ？
	info.m_bChanged		= true;
	TimerProc();
	//	いますぐ１回目を呼び出さないと、この呼び出し直後に
	//	再生している場合、そいつの音量が更新前の値で出力される
	return 0;
}

LRESULT CVolumeFader::FadeIn(eAudioMix e,int dwTime){
	return _FadeIn(e,dwTime,true);
}

LRESULT CVolumeFader::_FadeIn(eAudioMix e,int dwTime,bool bCheckCallBack){
	if (bCheckCallBack){
		CheckCallBack(e);
	}

	//	フェードインを開始させる
	CVolumeFadeInfo& info = m_FadeInfo[(int)e];
	info.m_Timer.Reset();
	info.m_dwWholeTime	= dwTime;
	info.m_bFadeIn		= true;
	info.m_bFadeOut		= false;

	//	現在のヴォリューム量をリストアしておく必要がある
	//	(ヴォリュームコントロールが変更された可能性があるので)
	if (!info.m_bChanged) { m_AudioMixer.RestoreVolume(e); }

	//	インターバルタイマーを発動させる＾＾
	if (!IsTimer()) {
		Start();
	}	//	これくらいで十分やろ？
	info.m_bChanged = true;
	TimerProc();
	//	いますぐ１回目を呼び出さないと、この呼び出し直後に
	//	再生している場合、そいつの音量が更新前の値で出力される
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
				//	変更終了⇒次のfadeのときには、
				//	現在のヴォリュームを初期ヴォリュームとして保存
			}
			m_AudioMixer.SetVolumeRel((eAudioMix)i,d);
		} else if (info.m_bFadeOut){
			double d = (double)info.m_Timer.Get() / info.m_dwWholeTime;
			if (d>1) {
				d=1;
				if (CheckCallBack((eAudioMix)i)){
					d = 0;	//	コールバックを行なったので初期ヴォリュームに戻す
					info.m_bChanged = false;
					//	この関数を抜けたあと、この値は初期ヴォリュームとみなして良い
				}
				info.m_bFadeOut = false;
			}
			d = 1-d;
			m_AudioMixer.SetVolumeRel((eAudioMix)i,d);
		} else {
			nReset++;
		}
	}
	//	すべてのフェードが終わっているか？
	if (nReset==4) Stop();	//	タイマーの停止
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

//	コールバックの開始
void	CVolumeFader::Start(){
	if (m_bTimer) return ; // すでにタイマまわっとる！
	m_bTimer = true;
	smart_ptr<function_callback> fn(
		function_callback_v::Create(&CVolumeFader::TimerProc,this)
	);
	//	コールバックの登録
	CAppManager::GetMyApp()->GetIntervalTimer()->RegistCallBack(
		this,	//	コールバック登録されているものを
				//	削除するための識別子
		20,		//	一回目のコールバックは20ms後
		20,		//	20msごとに
		0,		//	endlessコールバック
		fn
	);
}

void	CVolumeFader::Stop(){
	//	コールバックの削除
	if (IsTimer()){
		CAppManager::GetMyApp()->GetIntervalTimer()->DeleteCallBack(this);
		m_bTimer = false;
	}
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
