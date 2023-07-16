// yaneMIDIOutputDM.h
//
//		MIDI�o�͂��ADirectMusic���g���čs�Ȃ�������B
//
//		DirectMusic���g���ɂ́ADirectX6�ȍ~���K�v�Ȃ̂Œ��ӁI
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
	virtual int	GetType() const { return 2; } // RTTI���ǂ�

protected:
	LRESULT	InnerOpen();	//	DirectMusic���I�[�v������

	CFile	m_File;
	MUSIC_TIME m_mtPosition;	//	Pause����
	int		m_bPaused;

	LRESULT	LoopPlay();	//	LoopPlay�p
	bool	m_bLoopPlay;	//	Loop�Đ�����̂��H
	LONG	m_lLength;

	//	�Đ�����Z�O�����g
	IDirectMusicSegment* m_lpDMSegment;
	//	�Đ��Z�O�����g�̃X�e�[�^�X
	IDirectMusicSegmentState* m_lpDMSegmentState;

	//	�R���X�g���N�^�œn���ꂽCDirectMusic�̎擾
	CDirectMusic* GetDirectMusic() const { return m_pDirectMusic; }
	CDirectMusic* m_pDirectMusic;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif
