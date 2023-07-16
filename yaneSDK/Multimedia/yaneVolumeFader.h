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
	オーディオのミキシング用クラスCAudioMixerを利用して、
	MIDI、CD、WAVEに対して擬似的なフェードイン／フェードアウトを実現する。
	CDに対するフェードイン等は、技術的にも稀である。

	class CAudioMixerも参照すること。

	フェードは、CIntervalTimerによるが、これはCAppBase::IsThreadValidを
	呼び出さないとCallBackされない。
	よって、CVolumeFaderのFadeOut/FadeInを呼び出したあとは、Sleepしては
	意味がない。CAppBase::IsThreadValidを呼び出す描画ループのなかで
	使うかである。

	フェードアウトは、その後、指定した秒数で、フェードアウトが完了するので
	そのあと、曲再生を停止させ、ResetVolumeを呼び出して、もとのヴォリューム
	に戻してやる必要がある。

	また、このクラスがヴォリュームをフェードさせていないときに、
	ヴォリューム変更が行なわれたときのことを考慮して、
	フェードを開始するときに、ヴォリュームをチェックして、その
	ヴォリュームをclass CAudioMixer の初期ヴォリュームとして設定する
	処理を行なっている。
*/
public:
	LRESULT ResetVolume(eAudioMix e);			///	初期ボリューム値に戻す

	////////////////////////////////////////////////////////////////////////
	/// dwTime[ms]後にボリュームのフェードは完了する
	LRESULT FadeOut(eAudioMix e,int dwTime);	///	フェードアウトを開始させる
	LRESULT FadeIn(eAudioMix e,int dwTime);		///	フェードインを開始させる
	bool	IsFade(eAudioMix e)const;			///	フェード中なのか？

	LRESULT FadeOut(eAudioMix e,int dwTime,
		const smart_ptr<function_callback>& fn);
	/**
		fadeout 終了時に、それを通知するためのコールバックを指定できる
	*/

	LRESULT FadeIn(eAudioMix e,int dwTime,
		const smart_ptr<function_callback>& fn);
	/**
		fadein 終了時に、それを通知するためのコールバックを指定できる
	*/

	void	StopFade();							///	フェードを停止

	CVolumeFader() : m_bTimer(false) {}
	virtual ~CVolumeFader() { Stop(); }
protected:
	class CVolumeFadeInfo {
	public:
		CTimer		m_Timer;		//	FadeのためのTimer
		DWORD		m_dwWholeTime;	//	Fadeを開始から終了させるまでの時間
		bool		m_bFadeIn;		//	FadeIn中なのか?
		bool		m_bFadeOut;		//	FadeOut中なのか?
		bool		m_bChanged;
	//	現在、このクラスがヴォリュームをいじっている状態か？
	//	⇒このフラグがfalseのときに、FadeIn/FadeOutが指定されれば、
	//	現在のヴォリュームをそのときのオリジナルヴォリュームとして
	//	反映させる

		smart_ptr<function_callback> m_fn;	//	Fade終了時のcallback

		CVolumeFadeInfo() {
			m_bFadeIn = m_bFadeOut = m_bChanged = false;
		}
	};

	CAudioMixer		m_AudioMixer;
	CVolumeFadeInfo	m_FadeInfo[4];

	void	TimerProc();	//	コールバックされる関数
	void	Start();		//	コールバック開始
	void	Stop();			//	コールバック終了
	bool	m_bTimer;		//	コールバックループ中
	bool	IsTimer() const { return m_bTimer; }

	bool	CheckCallBack(eAudioMix e);
	//	fadeの終了タイミングなので、必要あらばコールバックを行なう
	//	callbackを行なったときtrue
	LRESULT _FadeOut(eAudioMix e,int dwTime,bool bCheckCallBack);
	LRESULT _FadeIn(eAudioMix e,int dwTime,bool bCheckCallBack);
	//	bCheckCallBackの有りと無し（内部で使用）
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif
