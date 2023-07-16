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
	class CWaveStaticSound �� class CWaveStreamSound ��
	�V�[�����X�Ɉ������߂�proxy�N���X

	Stop��A�T�E���h�̃o�O�΍�̂��߂ɁA50ms�����Ă������t�F�[�h�����Ȃ���
	��~������B(CAppIntervalTimer�𗘗p)
*/
public:
	CWaveSound(IWaveOutput* p);
	virtual ~CWaveSound();

	/// �X�g���[���Đ����邩��ݒ�E�擾
	/// �ݒ肵���u�Ԃɑ����ɃC���X�^���X�̂���ւ����s����
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

	// ----	�Z�J���_���̃o�O�΍�̂���50ms�V�΂��Ă����~������ ------

	//	�uFade�����Ȃ����Stop�����v�̃t���O�Ƃ�����擾����֐��^�ݒ肷��֐�
	bool	m_bFadeStop;
	bool	IsFadeStop() const { return m_bFadeStop; }
	void	SetFade(bool b) { m_bFadeStop = b; } // 
	
	LONG	m_lOriginalVolume;	//	Fade������O�̏������H�����[��
	//	(Fade������A���̃��H�����[���ɖ߂�)
	void	StopFade();		//	Fade�����Ă���̂Ȃ�΁A������~������

	LRESULT InnerStop();
			//	Stop�̎�����
	LRESULT InnerSetVolume(LONG lVolume);
			//	Fade���ɃR�[���o�b�N�����SetVolume
	// ----------------------------------------------------------------

	bool	m_bStreamPlay;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneWaveSound_h__
