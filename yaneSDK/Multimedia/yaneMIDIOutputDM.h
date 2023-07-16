// yaneMIDIOutputDM.h
//
//		MIDI出力を、DirectMusicを使って行なう実装例。
//
//		DirectMusicを使うには、DirectX6以降が必要なので注意！
//

#ifndef __yaneMIDIOutputDM_h__
#define __yaneMIDIOutputDM_h__


#include "yaneSound.h"
#include "../auxiliary/yaneFile.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class ISoundFactory;
class CDirectMusic;
class CDirectSound;
class CMIDIOutputDM : public ISound {
public:
	CMIDIOutputDM(CDirectMusic* p);
	virtual ~CMIDIOutputDM();

	//	override from ISoundBase
	virtual LRESULT Open(const string& pFileName);
	virtual LRESULT Close();
	virtual LRESULT Play();
	virtual LRESULT Replay();
	virtual LRESULT Stop();
	virtual LRESULT Pause(){ return Stop(); }
	virtual bool IsPlay() const;
	virtual bool IsLoopPlay() const { return m_bLoopPlay; }
	virtual LRESULT SetLoopPlay(bool bLoop);
	virtual LONG	GetCurrentPos() const;
	virtual LRESULT	SetCurrentPos(LONG lPos);
	virtual LONG	GetLength() const;
	virtual LRESULT SetVolume(LONG volume); 
	virtual LONG	GetVolume() const;
	string	GetFileName() const { return m_File.GetName(); }
	virtual int	GetType() const { return 2; } // RTTIもどき

protected:
	LRESULT	InnerOpen();	//	DirectMusicをオープンする

	CFile	m_File;
	MUSIC_TIME m_mtPosition;	//	Pause時間
	int		m_bPaused;

	LRESULT	LoopPlay();	//	LoopPlay用
	bool	m_bLoopPlay;	//	Loop再生するのか？
	LONG	m_lLength;

	//	再生するセグメント
	IDirectMusicSegment* m_lpDMSegment;
	//	再生セグメントのステータス
	IDirectMusicSegmentState* m_lpDMSegmentState;

	//	コンストラクタで渡されたCDirectMusicの取得
	CDirectMusic* GetDirectMusic() const { return m_pDirectMusic; }
	CDirectMusic* m_pDirectMusic;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif
