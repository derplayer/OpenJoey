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
	Wave�Đ��̂��߂� class ISound �h���N���X
	���ۂ�class CWaveSound��ʂ��ė��p����
	class CWaveStreamSound �ƈႢ�A�t�@�C�����ۂ��Ɠǂ�ōĐ�����
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

	/// �t�@�C����ǂݍ���
	LRESULT Load(const string& strFileName);

	/// �t�@�C�������
	LRESULT Release();

	/// �Đ����g���̎擾
	LRESULT SetFrequency(DWORD freq);

	/// �Đ����g���̐ݒ�
	LONG	GetFrequency() const;

	//////////////////////////////////////////////////////////////////////////

protected:
	smart_ptr<ISoundBuffer> m_vBuffer;			//	�Z�J���_���o�b�t�@
	smart_ptr<ISoundBuffer> GetBuffer() const { return m_vBuffer; }
	void SetBuffer(smart_ptr<ISoundBuffer> v) { m_vBuffer = v; }

	string	m_strFileName;						//	�t�@�C����
	int		m_bPaused;							//	�T�E���hpause�����H
	bool	m_bLoop;							//	���[�v���[�h�ōĐ�����̂��H
	DWORD	m_nAvgBytesPerSec;					//	�b�Ԃ̃f�[�^�o�C�g���i�Đ��ʒu�Z�o�p�j
	LONG	m_lLength;

	IWaveOutput*	GetOutput() const { return m_pOutput; }
	IWaveOutput*	m_pOutput;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneWaveStaticSound_h__
