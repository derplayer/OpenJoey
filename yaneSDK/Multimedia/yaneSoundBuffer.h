// yaneSoundBuffer.h
//		programmed by ENRA		'02/03/23

#ifndef __yaneSoundBuffer_h__
#define __yaneSoundBuffer_h__


namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class CSoundParameter;
class ISoundBuffer {
/**
	SoundBufferを抽象化したインターフェースクラス
	class CDirectSoundBuffer, class CNullSoundBuffer はこいつから派生
*/
public:
	ISoundBuffer(){};
	virtual ~ISoundBuffer(){};

	virtual int GetType() const = 0;
	/**
		RTTIもどき。派生クラスのタイプを返す
		0 : class CNullSoundBuffer
		1 : class CDirectSoundBuffer
		2 : かんがえちゅう
	*/

	/// バッファを作成する
	/// [in] lpWFormat 　- WAVEFORMATEXへのポインタを指定する
	/// [in] dwLockBytes - バッファのサイズを指定する
	/// [in] bZeroClear　- バッファの中身をゼロクリアするかどうか
	/// [in] pParam　　　- CSoundParameterへのポインタを指定する
	virtual LRESULT Create(LPCWAVEFORMATEX lpWFormat, DWORD dwSize, bool bZeroClear=false, const CSoundParameter* pParam=NULL) = 0;

	/// 自分の複製を返す
	virtual smart_ptr<ISoundBuffer> Clone() = 0;

	/// バッファの現在の状態を得る
	/// 　0 - バッファが無い
	/// 　1 - バッファがロストしている
	/// 　2 - 停止中
	/// 　3 - 再生中
	virtual LRESULT GetStatus() const = 0;

	/// バッファのフォーマットをWAVEFORMATEXとして、設定・取得する
	virtual LRESULT SetFormat(LPCWAVEFORMATEX lpFormat) = 0;
	virtual LRESULT GetFormat(LPWAVEFORMATEX lpFormat) const = 0;

	/// バッファの再生ポジションを設定・取得する
	/// [in/out] dwPlayCursor　- 再生位置を指定する/再生位置が返る
	/// [out]　　dwWriteCursor - 書き込み位置が返る
	virtual LRESULT SetCurrentPosition(DWORD dwPlayCursor) = 0;
	virtual LRESULT GetCurrentPosition(DWORD& dwPlayCursor, DWORD& dwWriteCursor) const = 0;

	/// バッファのボリュームを設定・取得する
	/// [in/out] lVolume - ボリューム値(0～-10000)
	virtual LRESULT SetVolume(LONG lVolume) = 0;
	virtual LRESULT GetVolume(LONG& lVolume) const = 0;

	/// バッファのパンを設定・取得する
	/// [in/out] lPan - パン値(-100000～+10000)
	virtual LRESULT SetPan(LONG lPan) = 0;
	virtual LRESULT GetPan(LONG& lPan) const = 0;

	/// バッファの再生周波数を設定・取得する
	/// [in/out] dwFrequency - 周波数値(100～100000)
	virtual LRESULT SetFrequency(DWORD dwFrequency) = 0;
	virtual LRESULT GetFrequency(DWORD& dwFrequency) const = 0;

	/// バッファをLock・Unlockする
	/// [in]　dwWriteCursor 　- Lockする位置を指定する
	/// [in]　dwLockBytes 　　- Lockするサイズを指定する
	/// [out] lpLockedBuffer1 - Lockされたバッファへのポインタが返る
	/// [out] dwLockedBytes1　- Lockされたサイズが返る
	/// [out] lpLockedBuffer2 - Lock領域がバッファ領域をはみでた場合、バッファ先頭へのポインタが返る
	/// [out] dwLockedBytes2　- はみ出た分の領域のサイズが返る
	virtual LRESULT Lock(DWORD dwWriteCursor, DWORD dwLockBytes, BYTE** lplpLockedBuffer1, DWORD* lpdwLockedBytes1, BYTE** lplpLockedBuffer2=NULL, DWORD* lpdwLockedBytes2=NULL) = 0;
	virtual LRESULT Unlock(BYTE* lpLockedBuffer1, DWORD dwLockedBytes1, BYTE* lpLockedBuffer=NULL, DWORD dwLockedBytes2=0) = 0;

	/// バッファを再生する
	/// [in] bLoop - ループ再生する場合はtrueを指定する
	virtual LRESULT Play(bool bLoop = false) = 0;

	/// バッファを停止する
	virtual LRESULT Stop() = 0;

	/// バッファがロストしていたらリストアする
	virtual LRESULT Restore() = 0;
};

class CDirectSound;
class CDirectSoundBuffer : public ISoundBuffer {
/**
	DirectSoundに特化した class ISoundBuffer 派生クラス
	class CDirectSoundPrimaryBuffer 　と
	class CDirectSoundSeconderyBuffer は、こいつから派生

	[注意点]　DirectSoundは、ボリューム値に-1000を指定したとすると、
	　　　　　-10dBのボリューム値になる。
*/
public:
	CDirectSoundBuffer(CDirectSound* lpDirectSound);
	virtual ~CDirectSoundBuffer();

	/// override from class ISoundBuffer
	virtual int GetType() const { return 1; }
	virtual LRESULT Create(LPCWAVEFORMATEX lpWFormat, DWORD dwSize, bool bZeroClear=false, const CSoundParameter* pParam=NULL) = 0;
	virtual smart_ptr<ISoundBuffer> Clone() = 0;
	virtual LRESULT GetStatus() const;
	virtual LRESULT SetFormat(LPCWAVEFORMATEX lpFormat);
	virtual LRESULT GetFormat(LPWAVEFORMATEX lpFormat) const;
	virtual LRESULT SetCurrentPosition(DWORD dwPlayCursor);
	virtual LRESULT GetCurrentPosition(DWORD& dwPlayCursor, DWORD& dwWriteCursor) const;
	virtual LRESULT SetVolume(LONG lVolume);
	virtual LRESULT GetVolume(LONG& lVolume) const;
	virtual LRESULT SetPan(LONG lPan);
	virtual LRESULT GetPan(LONG& lPan) const;
	virtual LRESULT SetFrequency(DWORD dwFrequency);
	virtual LRESULT GetFrequency(DWORD& dwFrequency) const;
	virtual LRESULT Lock(DWORD dwWriteCursor, DWORD dwLockBytes, BYTE** lplpLockedBuffer1, DWORD* lpdwLockedBytes1, BYTE** lplpLockedBuffer2=NULL, DWORD* lpdwLockedBytes2=NULL);
	virtual LRESULT Unlock(BYTE* lpLockedBuffer, DWORD dwLockedBytes, BYTE* lpLockedBuffer2=NULL, DWORD dwLockedBytes2=0);
	virtual LRESULT Play(bool bLoop = false);
	virtual LRESULT Stop();
	virtual LRESULT Restore();

protected:
	/// LPDIRECTSOUNDBUFFERを返す
	LPDIRECTSOUNDBUFFER GetBuffer() const { return m_lpBuffer; }
	LPDIRECTSOUNDBUFFER m_lpBuffer;

	/// CDirectSoundを返す
	CDirectSound* GetDirectSound() const { return m_lpDirectSound; }
	CDirectSound* m_lpDirectSound;
};

class CDirectSoundPrimaryBuffer : public CDirectSoundBuffer {
/**
	DirectSoundBufferのPrimaryBufferのためのクラス
*/
public:
	CDirectSoundPrimaryBuffer(CDirectSound* lpDirectSound) : CDirectSoundBuffer(lpDirectSound) {};
	virtual ~CDirectSoundPrimaryBuffer() {};

	/// override from class ISoundBuffer
	/// 引数は全て無視される
	virtual LRESULT Create(LPCWAVEFORMATEX lpWFormat, DWORD dwSize, bool bZeroClear=false, const CSoundParameter* pParam=NULL);

	/// override from class ISoundBuffer
	virtual smart_ptr<ISoundBuffer> Clone() { return smart_ptr<ISoundBuffer>(); }
};

class CDirectSoundSeconderyBuffer : public CDirectSoundBuffer {
/**
	DirectSoundBufferのSeconderyBufferのためのクラス
*/
public:
	CDirectSoundSeconderyBuffer(CDirectSound* lpDirectSound) : CDirectSoundBuffer(lpDirectSound) {};
	virtual ~CDirectSoundSeconderyBuffer() {};

	/// override from class ISoundBuffer
	virtual LRESULT Create(LPCWAVEFORMATEX lpWFormat, DWORD dwSize, bool bZeroClear=false, const CSoundParameter* pParam=NULL);
	virtual smart_ptr<ISoundBuffer> Clone();
};

class CNullSoundBuffer : public ISoundBuffer {
/**
	class ISoundBuffer のNullDeviceクラス
*/
public:
	CNullSoundBuffer(){};
	virtual ~CNullSoundBuffer(){};

	/// override from class ISoundBuffer
	virtual int GetType() const { return 0; }
	virtual LRESULT Create(LPCWAVEFORMATEX lpWFormat, DWORD dwSize, bool bZeroClear=false, const CSoundParameter* pParam=NULL) { return -1; }
	virtual smart_ptr<ISoundBuffer> Clone() { return smart_ptr<ISoundBuffer>(new CNullSoundBuffer); }
	virtual LRESULT GetStatus() const { return -1; }
	virtual LRESULT SetFormat(LPCWAVEFORMATEX lpFormat) { return -1; }
	virtual LRESULT GetFormat(LPWAVEFORMATEX lpFormat) const { return -1; }
	virtual LRESULT SetCurrentPosition(DWORD dwPlayCursor) { return -1; }
	virtual LRESULT GetCurrentPosition(DWORD& dwPlayCursor, DWORD& dwWriteCursor) const { return -1; }
	virtual LRESULT SetVolume(LONG lVolume) { return -1; }
	virtual LRESULT GetVolume(LONG& lVolume) const { return -1; }
	virtual LRESULT SetPan(LONG lPan) { return -1; }
	virtual LRESULT GetPan(LONG& lPan) const { return -1; }
	virtual LRESULT SetFrequency(DWORD dwFrequency) { return -1; }
	virtual LRESULT GetFrequency(DWORD& dwFrequency) const { return -1; }
	virtual LRESULT Lock(DWORD dwWriteCursor, DWORD dwLockBytes, BYTE** lplpLockedBuffer1, DWORD* lpdwLockedBytes1, BYTE** lplpLockedBuffer2=NULL, DWORD* lpdwLockedBytes2=NULL) { return -1; }
	virtual LRESULT Unlock(BYTE* lpLockedBuffer1, DWORD dwLockedBytes1, BYTE* lpLockedBuffer2=NULL, DWORD dwLockedBytes2=0) { return -1; }
	virtual LRESULT Play(bool bLoop = false) { return -1; }
	virtual LRESULT Stop() { return -1; }
	virtual LRESULT Restore() { return -1; }
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneSoundBuffer_h__
