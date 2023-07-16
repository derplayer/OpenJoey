// yaneWaveStaticSound.h
//		programmed by yaneurao(M.Isozaki) '99/07/07
//		reprogrammed by yaneurao '00/03/04

#ifndef __yaneWaveStaticSound_h__
#define __yaneWaveStaticSound_h__


#include "yaneSound.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class IWaveOutput;
class ISoundBuffer;
class CWaveStaticSound : public IWaveSound {
/**
	Wave再生のための class ISound 派生クラス
	実際はclass CWaveSoundを通じて利用する
	class CWaveStreamSound と違い、ファイルを丸ごと読んで再生する
*/
public:
	CWaveStaticSound(IWaveOutput* p);
	virtual ~CWaveStaticSound();

	/// override from class ISound
	LRESULT Open(const string& strFileName) { return Load(strFileName); }
	LRESULT Close()	{ return Release(); }
	LRESULT Play();
	LRESULT Replay();
	LRESULT Stop();
	LRESULT Pause();
	bool	IsPlay() const;
	LRESULT	SetLoopPlay(bool bLoop);
	bool	IsLoopPlay() const { return m_bLoop; }
	LONG	GetLength() const { return m_lLength; }
	LRESULT SetCurrentPos(LONG lPos);
	LONG	GetCurrentPos() const;
	LRESULT SetVolume(LONG volume);
	LONG	GetVolume() const;
	string	GetFileName() const { return m_strFileName; }
	int	GetType() const { return 3; }

	/// override from class IWaveSound
	LRESULT Restore();

	/// ファイルを読み込む
	LRESULT Load(const string& strFileName);

	/// ファイルを閉じる
	LRESULT Release();

	/// 再生周波数の取得
	LRESULT SetFrequency(DWORD freq);

	/// 再生周波数の設定
	LONG	GetFrequency() const;

	//////////////////////////////////////////////////////////////////////////

protected:
	smart_ptr<ISoundBuffer> m_vBuffer;			//	セカンダリバッファ
	smart_ptr<ISoundBuffer> GetBuffer() const { return m_vBuffer; }
	void SetBuffer(smart_ptr<ISoundBuffer> v) { m_vBuffer = v; }

	string	m_strFileName;						//	ファイル名
	int		m_bPaused;							//	サウンドpause中か？
	bool	m_bLoop;							//	ループモードで再生するのか？
	DWORD	m_nAvgBytesPerSec;					//	秒間のデータバイト数（再生位置算出用）
	LONG	m_lLength;

	IWaveOutput*	GetOutput() const { return m_pOutput; }
	IWaveOutput*	m_pOutput;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneWaveStaticSound_h__
