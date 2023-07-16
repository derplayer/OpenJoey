//
// yaneSound.h
//
//		MIDI,WAVE�o�͊��N���X
//

#ifndef __yaneSound_h__
#define __yaneSound_h__


namespace yaneuraoGameSDK3rd {
namespace Multimedia {

/////////////////////////////////////////////////////////////////////////////
class ISound {
/**
	MIDI,WAVE�Đ��N���X�̃C���^�[�t�F�[�X
*/
public:
	virtual LRESULT Open(const string& strFileName) = 0;
	/// �t�@�C���̓ǂݍ���

	virtual LRESULT Close() = 0;
	///	�t�@�C�������

	virtual LRESULT Play()	= 0;
	///	�Đ�����

	virtual LRESULT Pause() = 0;
	///	�ꎞ��~

	virtual LRESULT Replay() = 0;
	/// �ꎞ��~���Ă������̂��ĊJ

	virtual LRESULT Stop()	= 0;
	/// ��~

	virtual bool IsPlay() const	 = 0;
	/// �Đ����Ȃ��true

	virtual LRESULT SetLoopPlay(bool bLoop) = 0;
	/// ���[�v�Đ����[�h�ɂ���i�I����A�擪�ɖ߂��čĐ�����j

	virtual bool IsLoopPlay() const = 0;
	/// SetLoopPlay�Ń��[�v�Đ����[�h�ɐݒ肳��Ă���Ȃ��true

	virtual LRESULT	SetCurrentPos(LONG lPos) = 0;
	virtual LONG	GetCurrentPos() const = 0;
	/// ���݂̍Đ��|�W�V������ݒ�^�擾�i[ms]�P�ʁj
	///	�擾�́A�G���[�̂Ƃ��̓}�C�i�X�̒l���Ԃ�

	virtual LONG	GetLength() const = 0;
	///	�Ȃ̑S�̒������擾����B([ms]�P��)

	virtual LRESULT SetVolume(LONG volume)=0;
	virtual LONG	GetVolume() const =0;
	/**
		���H�����[����ݒ�^�擾����B(DSBVOLUME_MIN�`DSBVOLUME_MAX�̊Ԃ̒l��
		�w�肷��BDSBVOLUME_MIN���w�肵���Ƃ��́A�������)�@�������A
		���ׂẴf�o�C�X�ŁA���̉��ʒ������T�|�[�g���Ă���Ƃ͌���Ȃ��B
		���Ƃ��΁ACMIDIOutputDM�̓T�|�[�g���Ă��邪�ACMIDIOutputMCI�́A
		������T�|�[�g���Ă��Ȃ��B
	*/

	virtual int	GetType() const = 0;
	/**
		RTTI���ǂ��B�h���N���X�̃^�C�v��Ԃ�
		0 : NullDevice
		1 : CMIDIOutputMCI
		2 : CMIDIOutputDM
		3 : CWaveStaticSound
		4 : CWaveStreamSound
		5 : CWaveSound
	*/

	virtual string	GetFileName() const = 0;
	/// �ǂݍ���ł���t�@�C������Ԃ�

	virtual LRESULT ConvertToMealTime(LONG nPos,int&nHour,int&nMin,int&nSec,int&nMS);
	virtual LRESULT ConvertFromMealTime(LONG&nPos,int nHour,int nMin,int nSec,int nMS);
	/**
		GetCurrentPos�œ���ꂽnPos [ms] ���A(nHour,nMin,nSec,nMs)���Ȃ킿�A
		�����b�тƂ�ϊ�����B�тƂ����̂́A�����Ȃ�Ȃ��P�ʂ��Ǝv���܂���
		1/1000�i�b�j==1[ms]�ł��B�O�҂�[ms]�ˎ����b�сC��҂́A
		�����b�с�[ms]�ł��B
	*/

	virtual ~ISound() {}
};

class ISoundRestorable : public ISound {
/**
	Restore���\�Ȏ������� class ISound �݊��C���^�[�t�F�[�X
*/
public:
	virtual LRESULT Restore() = 0;
	///	�t�@�C�����ēǂݍ���

	virtual ~ISoundRestorable() {}
};

class IWaveSound : public ISoundRestorable {
/**
	WAVE�Đ��N���X�̃C���^�[�t�F�[�X
*/
public:
	virtual LRESULT Restore() = 0;	// �Ē�`���Ȃ���list_chain::for_each���R���p�C���o���Ȃ�^^;
	///	�t�@�C�����ēǂݍ���

	virtual ~IWaveSound() {}
// �܂������o�͂Ȃ�^^;
};

class CSoundNullDevice : public ISound {
/**
	���Ɍ���NULL DEVICE
	MIDI�̕t���Ă��Ȃ����ł́A����ł��A�b�v�L���X�g���Ďg���Ηǂ�
*/
public:
	virtual LRESULT Open(const string& sFileName) { return 0; }
	virtual LRESULT Close()	 { return 0; }
	virtual LRESULT Play()	 { return 0; }
	virtual LRESULT Replay() { return 0; }
	virtual LRESULT Stop()	 { return 0; }
	virtual LRESULT Pause()	 { return 0; }
	virtual bool IsPlay() const { return false; }
	virtual bool IsLoopPlay() const { return m_bLoop; }
	virtual LRESULT SetLoopPlay(bool bLoop) { m_bLoop=bLoop; return 0; }
	virtual LONG	GetCurrentPos() const { return -1; }
	virtual LRESULT	SetCurrentPos(LONG lPos) { return -1;}
	virtual LONG	GetLength() const { return -1;}
	virtual LRESULT SetVolume(LONG volume){ return -1;}
	virtual LONG	GetVolume() const { return DSBVOLUME_MIN;}
	virtual string	GetFileName() const { return ""; }
	virtual int	GetType() const { return 0; }

	CSoundNullDevice() : m_bLoop(false) {}

protected:
	bool m_bLoop;
};

class CProxySound : public ISound {
/**
	ISound��proxy object
*/
public:
	virtual LRESULT Open(const string& pFileName) { return GetSound()->Open(pFileName); }
	virtual LRESULT Close() { return GetSound()->Close();  }
	virtual LRESULT Play()	{ return GetSound()->Play();   }
	virtual LRESULT Replay(){ return GetSound()->Replay(); }
	virtual LRESULT Stop()	{ return GetSound()->Stop();   }
	virtual LRESULT Pause() { return GetSound()->Pause();  }
	virtual bool IsPlay() const	{ return GetSound()->IsPlay(); }
	virtual bool IsLoopPlay() const { return GetSound()->IsLoopPlay(); }// �ǉ�
	virtual LRESULT SetLoopPlay(bool bLoop) { return GetSound()->SetLoopPlay(bLoop); }
	virtual LONG	GetCurrentPos() const { return GetSound()->GetCurrentPos(); }
	virtual LONG	GetLength() const { return GetSound()->GetLength();}
	virtual LRESULT SetCurrentPos(LONG lPos){ return GetSound()->SetCurrentPos(lPos);}
	virtual LRESULT SetVolume(LONG volume){ return GetSound()->SetVolume(volume);}
	virtual LONG	GetVolume() const { return GetSound()->GetVolume();}
	virtual string	GetFileName() const { return GetSound()->GetFileName(); }
	virtual int	GetType() const { return GetSound()->GetType(); }

	CProxySound::CProxySound() : m_lpSound(new CSoundNullDevice) {}

	void	SelectDevice(const smart_ptr<ISound>& p) { m_lpSound = p;}
	/**
		���̃N���X���̂́Aproxy�p�^�[���ɂ���������Ă���̂�
		����proxy�I�u�W�F�N�g�������ւ���
	*/
protected:
	smart_ptr<ISound>	m_lpSound;
	smart_ptr<ISound>	GetSound() const { return m_lpSound; }
};

/////////////////////////////////////////////////////////////////////////////

class ISoundFactory;
class CSound {
/**
	smart_ptr<ISound> �Ƃ������������͖̂ʓ|�ɂ܂�Ȃ��̂ŁD�D

	��
		ISurface�ɑ΂���CPlane�Ɠ����B
		ISound�ɑ΂��Ă�CSound�B
*/
public:

	LRESULT Load(const string& strFileName);
	///	�����̃t�@�C��(wav,mp3,mid,ogg..)��ǂݍ���

	///	�|�C���^�̂ӂ�����邽�߂̎d�|��
	ISound& operator*() const  {return *get(); }
	ISound* operator->() const {return get();  }

	///	ISurface�ւ̈Öق̕ϊ�
	operator ISound* () const { return get(); }

	/**
		Null Check
			load�Ɏ��s����΁A���̃N���X�̕����Ă���|�C���^smart_ptr<ISound>��
			Null�ɂȂ�̂ŁA��������o����B
	*/
	bool	isNull() const { return p_.isNull(); }

	ISound* get() const { return p_.get(); }

	static void SetFactory(const smart_ptr<ISoundFactory>&pFactory)
		{ pFactory_ = pFactory; }
	static smart_ptr<ISoundFactory> GetFactory()
		{ return pFactory_; }

	CSound(){}

	//CSound(const smart_ptr<ISoundFactory>& pFactory);
	///	sound��factory��n���΁A�����p���Đ������ĕێ�����

	CSound(const smart_ptr<ISound>& sound):	p_(sound) {}
	///	copy constructor

	CSound(ISound* sound):p_(sound) {}
	///�@������́A���L����CSound���ێ�����̂ŁA
	///		CSound sound(new CMIDISound);
	///	�̂悤�ɂ��Ďg�����ƁI

protected:
	smart_ptr<ISound> p_; // p_;�@�����ċ����Ă�́H

	static ThreadLocal<smart_ptr<ISoundFactory> > pFactory_;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif
