// yaneWaveSound.h
//		programmed by ENRA		 '02/03/25

#ifndef __yaneWaveSound_h__
#define __yaneWaveSound_h__


#include "yaneSound.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class IWaveOutput;
class CWaveSound : public IWaveSound {
/**
	class CWaveStaticSound と class CWaveStreamSound を
	シームレスに扱うためのproxyクラス

	Stop後、サウンドのバグ対策のために、50msかけてゆっくりフェードさせながら
	停止させる。(CAppIntervalTimerを利用)
*/
public:
	CWaveSound(IWaveOutput* p);
	virtual ~CWaveSound();

	/// ストリーム再生するかを設定・取得
	/// 設定した瞬間に即座にインスタンスのすり替えが行われる
	void	SetStreamPlay(bool b);
	bool	IsStreamPlay() { return m_bStreamPlay; }

	/// overriden from class ISound
	virtual LRESULT Open(const string& strFileName);
	virtual LRESULT Close();
	virtual LRESULT Play();
	virtual LRESULT Replay();
	virtual LRESULT Stop();
	virtual LRESULT Pause();
	virtual bool	IsPlay() const;
	virtual LRESULT	SetLoopPlay(bool bLoop);
	virtual bool	IsLoopPlay() const;
	virtual LONG	GetLength() const;
	virtual LRESULT SetCurrentPos(LONG lPos);
	virtual LONG	GetCurrentPos() const;
	virtual LRESULT SetVolume(LONG volume);
	virtual LONG	GetVolume() const;
	virtual string	GetFileName() const;
	int	GetType() const { return 5; }

	/// override from class IWaveSound
	virtual LRESULT Restore();

protected:
	smart_ptr<IWaveSound>	GetSound() const { return m_vSound; }
	smart_ptr<IWaveSound>	m_vSound;

	IWaveOutput*	GetOutput() const { return m_pOutput; }
	IWaveOutput*	m_pOutput;

	// ----	セカンダリのバグ対策のため50ms遊ばせてから停止させる ------

	//	「FadeさせながらのStop中か」のフラグとそれを取得する関数／設定する関数
	bool	m_bFadeStop;
	bool	IsFadeStop() const { return m_bFadeStop; }
	void	SetFade(bool b) { m_bFadeStop = b; } // 
	
	LONG	m_lOriginalVolume;	//	Fadeさせる前の初期ヴォリューム
	//	(Fade完了後、このヴォリュームに戻す)
	void	StopFade();		//	Fadeさせているのならば、それを停止させる

	LRESULT InnerStop();
			//	Stopの実処理
	LRESULT InnerSetVolume(LONG lVolume);
			//	Fade中にコールバックされるSetVolume
	// ----------------------------------------------------------------

	bool	m_bStreamPlay;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneWaveSound_h__
