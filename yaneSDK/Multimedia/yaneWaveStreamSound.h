// yaneWaveStreamSound.h
//		programmed by ENRA		 '02/03/25

#ifndef __yaneWaveStreamSound_h__
#define __yaneWaveStreamSound_h__


#include "yaneSound.h"
#include "../Thread/yaneThread.h"
#include "../Thread/yaneCriticalSection.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class IWaveOutput;
class ISoundBuffer;
class ISoundStream;
class CWaveStreamSound : public IWaveSound, public CThread {
/**
	Wave再生のための class ISound 派生クラス
	実際はclass CWaveSoundを通じて利用する
	class CWaveStaticSound と違い、ファイルを展開しながら再生する
*/
public:
	CWaveStreamSound(IWaveOutput* p);
	virtual ~CWaveStreamSound();

	///	overriden from class ISound
	LRESULT Open(const string& strFileName) { return Load(strFileName); }
	LRESULT Close()	{ return Release(); }
	LRESULT Play();
	LRESULT Replay();
	LRESULT Stop();
	LRESULT Pause();
	bool	IsPlay() const;
	LRESULT	SetLoopPlay(bool bLoop);
	bool	IsLoopPlay() const;
	LONG	GetLength() const;
	LRESULT SetCurrentPos(LONG lPos);
	LONG	GetCurrentPos() const;
	LRESULT SetVolume(LONG volume);
	LONG	GetVolume() const;
	string	GetFileName() const { return m_strFileName; }
	int	GetType() const { return 4; }

	///	overriden from class IWaveSound
	LRESULT Restore();

	///	ファイルを読み込む
	LRESULT Load(const string& strFileName);
	///	ファイルを閉じる
	LRESULT Release();
	///	再生周波数の設定・取得
	LRESULT SetFrequency(DWORD freq);
	DWORD	GetFrequency() const;

	//////////////////////////////////////////////////////////////////////////

protected:
	//	バッファのByteサイズを得る
	inline DWORD	GetBufferSize() const { return eBufferLength * m_nAvgBytesPerSec; }

	//	Stop()の下請け
	LRESULT InnerStop();

	//	Secondary Bufferを初期化
	LRESULT InitBuffer();
	//	Secondary Bufferにデータを流し込む
	LRESULT GetNextSoundData();

	//	Accessor
	inline IWaveOutput*				GetOutput() const { return m_pOutput; }
	inline ICriticalSection*		GetCS() const { return const_cast<CCriticalSection*>(&m_vCS); }
	inline smart_ptr<ISoundStream>	GetStream() const { return m_vStream; }
	inline smart_ptr<ISoundBuffer>	GetBuffer() const { return m_vBuffer; }
	inline void	SetStream(const smart_ptr<ISoundStream>& p) { m_vStream = p; }
	inline void	SetBuffer(smart_ptr<ISoundBuffer> p) { m_vBuffer = p; }

	//	overriden from class CThread
	void	ThreadProc();

private:
	//	Secondary Buffer
	smart_ptr<ISoundBuffer>	m_vBuffer;	
	//	Sound Stream
	smart_ptr<ISoundStream>	m_vStream;
	//	Critical Section Object
	CCriticalSection		m_vCS;
	//	Wave Output Device
	IWaveOutput*			m_pOutput;

	volatile DWORD	m_dwPlayPosition;	//	ファイル中の再生位置をByteで示す

	enum	{ eBufferLength = 3 };	//	セカンダリバッファの秒数

	DWORD	m_dwPrevWriteCursor;//	前回の書込カーソル
	DWORD	m_dwNextWriteCursor;//	次回の書込カーソル
	DWORD	m_dwWriteDiffer;	//	前回の書き込めなかったサイズ
	DWORD	m_dwPrevPlayCursor;	//	前回の再生カーソル

	string	m_strFileName;		//	ファイル名
	int		m_bPaused;			//	サウンドpause中か？
	volatile bool	m_bLoop;	//	ループモードで再生するのか？
	DWORD	m_nAvgBytesPerSec;	//	１秒間のデータバイト数（再生位置算出用）
	DWORD	m_dwLength;			//	曲の長さ[単位: Byte]
	LONG	m_lVolume;			//	Volume
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneWaveStreamSound_h__
