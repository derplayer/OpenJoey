/*
	�T�E���h�N���X�����p�N���X
*/

#ifndef __yaneSoundFactory_h__
#define __yaneSoundFactory_h__


#include "../Auxiliary/yaneStringMap.h"
#include "../AppFrame/yaneObjectCreater.h"

#include "../Multimedia/yaneDirectSound.h"
#include "../Multimedia/yaneDirectMusic.h"
#include "../Multimedia/yaneWaveOutput.h"
//#include "../Multimedia/yaneDirectSound.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

//class ISound;
//class CSoundParameter;
//class CDirectSound;
//class CDirectMusic;
//class IWaveOutput;
//class CWaveOutputDirectSound;

class ISoundFactory {
/**
	class ISound �h���N���X��factory�̃C���^�[�t�F�[�X
*/
public:
	virtual	smart_ptr<ISound>	Create(const string& filename)=0;
	virtual	smart_ptr<ISound>	Create(const string& filename,bool bStreamPlay)=0;
	virtual smart_ptr<ISound>	CreateSound()=0;
	virtual smart_ptr<ISound>	CreateMIDI()=0;
	virtual list_chain<ISound>*	GetSoundList()=0;
	virtual void			IncRefDirectSound()=0;
	virtual void			DecRefDirectSound()=0;
	virtual CDirectSound*	GetDirectSound()=0;
	virtual void			IncRefDirectMusic()=0;
	virtual void			DecRefDirectMusic()=0;
	virtual CDirectMusic*	GetDirectMusic()=0;
	virtual void			IncRefWaveOutput()=0;
	virtual void			DecRefWaveOutput()=0;
	virtual IWaveOutput*	GetWaveOutput()=0;

	//	property
	virtual void	SelectMIDIFactory(int nFirst,int nSecond) = 0;
	virtual smart_ptr<CSoundParameter> GetSoundParameter() const = 0;
	virtual void	SetSoundParameter(const smart_ptr<CSoundParameter>& p) = 0;
	virtual void	SetStreamPlay(bool b) = 0;
	virtual bool	IsStreamPlay() const = 0;

	virtual ~ISoundFactory() {}
};

class CSoundFactory : public ISoundFactory {
/**
	class ISound �h���N���X��factory

	[���ӓ_]�@�������ꂽISound�h���N���X������ɁA���̃N���X����̂�����
	�@�@�@�@�@��O����������B�����𓯈�N���X�̃����o�Ɏ��ꍇ�́A
	�@�@�@�@�@CSoundFactory���ɐ錾����K�v������B

	���̃N���X�̃R���X�g���N�^�ŁA
	class CSound::SetFactory�ŁA���̃N���X�̃C���X�^���X��o�^����

	����āA�ȍ~�ACSound�Ƃ��ăT�E���h���Đ����邱�Ƃ��ł���

*/
public:
	//	--------- SoundBase�h���N���X��factory
	///	1.SoundBuffer�i�T�E���h�Đ��N���X�j�𐶐�����
	virtual smart_ptr<ISound>	CreateSound();

	///	2.MIDI�o�͗p�̃N���X(CMIDIOutputDM��CMIDIOutputMCI)�𐶐�����
	virtual smart_ptr<ISound>	CreateMIDI();
	/**
		3.�g���q�ɉ����āACreateSound,CreateMIDI���s��
		��҂�Create�́Astream�Đ��ɂ��邩�ǂ������w�肷��B
		(���̃I�v�V�����w�肪���Őݒ肵���I�v�V�������D�悳���)
		
		���̕Ԃ��l��smart_ptr��null�ł��邱�Ƃ͂Ȃ��B
		(���Ȃ��Ƃ�CSoundNullDevice���Ԃ�)
	*/
	virtual smart_ptr<ISound>	Create(const string& filename);
	virtual	smart_ptr<ISound>	Create(const string& filename,bool bStreamPlay);

	///	��L��factory�Ő��������C���X�^���X��list_chain���l������
	list_chain<ISound>*	GetSoundList() { return& m_listSound; }

	//	--------- property -------------------

	/**
		CSoundParameter�̐ݒ�E�擾
		class ISoundStream �h���̃v���O�C���N���X�̎w��ƁA
		�T�E���h�o�b�t�@�̃O���[�o���t�H�[�J�X�̎w��́A
		�����ɑ΂��čs��
	*/
	virtual smart_ptr<CSoundParameter> GetSoundParameter() const { return m_pSoundParameter; }
	virtual void SetSoundParameter(const smart_ptr<CSoundParameter>& p) { m_pSoundParameter = p; }

	/**
		IncRefDirectSound�������ƂɁAGetDirectSound��
		CDirectSound*���擾���邱�Ƃ��\�B
		�g���I�������DecRefDirectSound���ׂ��B
	*/
	void			IncRefDirectSound();
	void			DecRefDirectSound();
	CDirectSound*	GetDirectSound();

	/**
		IncRefDirectSound�������ƂɁAGetDirectMusic��
		CDirectMusic*���擾���邱�Ƃ��\�B
		�g���I�������DecRefDirectMusic���ׂ��B
	*/
	void			IncRefDirectMusic();
	void			DecRefDirectMusic();
	CDirectMusic*	GetDirectMusic();

	/**
		IncRefWaveOutput�������ƂɁAGetWaveOutput��
		IWaveOutput*���擾���邱�Ƃ��\�B
		�g���I�������DecRefWaveOutput���ׂ��B
	*/
	void			IncRefWaveOutput();
	void			DecRefWaveOutput();
	IWaveOutput*	GetWaveOutput();

	/**
		MIDI�̍Đ�����factory��؂�ւ���
		�@�@nFirst�@: �ŏ��ɑI�΂��factory
		�@�@nSecond : nFirst���g�p�ł��Ȃ��Ƃ��ɑI�΂��factory
		�l�̈Ӗ���
		�@�@0 : CSoundNullDevice(�Đ����Ȃ�)
		�@�@1 : CMIDIOutputMCI(MCI�ɂ��Đ�)
		�@�@2 : CMIDIOutputDM (DirectMusic�ɂ��Đ�)
		�f�B�t�H���g�ł�
		�@�@nFirst == 2	, nSecond == 1
		���Ȃ킿�ADirectMusic�ɂ��Đ����\�Ȋ��Ȃ�΁A�����p���A
		���ꂪ�s�Ȋ��Ȃ�΁AMCI�ɂ���čĐ�����B
		�i�T�E���h�Đ����s�\�Ȋ��ł���΁ACreateMIDI�ł�
		�@CSoundNullDevice�̃C���X�^���X���Ԃ�j
	*/
	virtual void SelectMIDIFactory(int nFirst,int nSecond)
	{
		m_nMIDIFactoryFirst =nFirst;
		m_nMIDIFactorySecond=nSecond;
	}

	/// �X�g���[���Đ����邩��ݒ�E�擾
	/// ����CreateSound�Ő��������ISound�h���N���X�ɔ��f�����
	void	SetStreamPlay(bool b) { m_bStreamPlay = b; }
	bool	IsStreamPlay() const { return m_bStreamPlay; }

	CSoundFactory();
	virtual ~CSoundFactory();

protected:
	ref_creater<CDirectSound>	m_vDirectSound;
	ref_creater_and_mediator<CDirectMusic, CDirectSound>	m_vDirectMusic;
	ref_creater_and_mediator<CWaveOutputDirectSound, CDirectSound>	m_vWaveOutputDS;
	//	�K�v�ɂȂ����Ƃ��ɂ̂ݐ��������

	list_chain<ISound>	m_listSound;
	//	���̃N���X��factory�Ő��������C���X�^���X��list_chain

	int		m_nMIDIFactoryFirst;
	int		m_nMIDIFactorySecond;

	smart_ptr<ISound>	InnerCreate(int nDevice);
	//	ISound��Factory..
	//	nDevice ==	0 : class CSoundNullDevice
	//				1 : class CMIDIOutputMCI
	//				2 : class CMIDIOutputDM
	//				3 : class CWaveSound
	//				4 : �����I�ɔ�StreamSound
	//				5 : �����I��StreamSound

	// CSoundParameter��ێ�����
	smart_ptr<CSoundParameter> m_pSoundParameter;

	// �X�g���[���Đ����邩(default:false)
	bool m_bStreamPlay;

private:
	CSoundFactory* GetMyClass() { return this; }
	//�@�R���X�g���N�^�̃x�[�X�����������o�̂Ȃ���
	//�@this���g���ƌx�����o�āAᛂɏ��̂ŁD�D

	bool	InnerDeleteChain(ISound* p);
	//�@���̃N���X�̎���������ISound��chain
	//�@���Ȃ킿�Alist_chain<ISound>����
	//�@�I�u�W�F�N�g��̎��ɃR�[���o�b�N�����B
	//�@���̊֐��́Achain����p���O���̂Ɏg����
	//�@���X�g��p���������true��Ԃ�

	void	InnerDeleteChainForWO(ISound* p);
	//�@InnerDeleteChain()�{DecRefWaveOutput()

	void	InnerDeleteChainForDM(ISound* p);
	//�@InnerDeleteChain()�{DecRefDirectMusic()
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneSoundFactory_h__
