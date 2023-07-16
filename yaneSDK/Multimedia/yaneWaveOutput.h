// yaneWaveOutput.h
//		programmed by ENRA		 '02/03/28

#ifndef __yaneWaveOutput_h__
#define __yaneWaveOutput_h__


#include "../Window/yaneWinHook.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class ISound;
class IWaveSound;
class ISoundBuffer;
class ISoundStreamFactory;
class CDirectSound;
class CSoundParameter;

class IWaveOutput {
/**
	Wave�o�͂𒊏ۉ������N���X
	class IWaveSound �h���N���X�̊Ǘ�
	class ISoundBuffer �� class ISoundStreamFactory �̒�
	���󂯎���

	[�g�p���@]
	CWaveOutputDirectSound ds;�@��IWaveOutput�h���N���X
	CWaveSound sound(&ds);
	sound.Play();
*/
public:
	IWaveOutput();
	virtual ~IWaveOutput(){};

	virtual int	GetType() const = 0;
	/**
		RTTI���ǂ��B�h���N���X�̃^�C�v��Ԃ�
		0 : class CWaveNullOutput
		1 : class CWaveOutputDirectSound
		2 : ���񂪂����イ
	*/

	/// class ISoundBuffer �h���N���X�̐���
	virtual smart_ptr<ISoundBuffer> CreateBuffer()=0;

	/// class CSoundParameter �̐ݒ�E�擾
	virtual smart_ptr<CSoundParameter> GetSoundParameter() const { return m_pSoundParameter; }
	virtual void SetSoundParameter(const smart_ptr<CSoundParameter>& p) { m_pSoundParameter = p; }

	/// �Ǘ����X�g�̎擾
	/// class IWaveSound �h���N���X�̑S�C���X�^���X�ɑ΂��Ă̑���͂���ōs��
	virtual list_chain<IWaveSound>* GetSoundList() { return &m_listSound; }

	/// �Ǘ����X�g�ւ̒ǉ��E�폜
	/// class IWaveSound �h���N���X�̃R���X�g���N�^�E�f�X�g���N�^�ŌĂяo�����̂�
	/// ���[�U�������I�ɌĂяo���K�v�͂Ȃ�
	virtual LRESULT AddSound(IWaveSound* p);
	virtual LRESULT DelSound(IWaveSound* p);

protected:
	// class IWaveSound �h���N���X�̃��X�g
	list_chain<IWaveSound> m_listSound;
	// class CSoundParameter ��ێ�����
	smart_ptr<CSoundParameter> m_pSoundParameter;
};

class CWaveNullOutput : public IWaveOutput {
/**
	class IWaveOutput ��NullDevice�N���X
*/
public:
	CWaveNullOutput(){};
	virtual ~CWaveNullOutput(){};

	/// override from class IWaveOutput
	virtual int	GetType() const { return 0; }
	virtual smart_ptr<ISoundBuffer> CreateBuffer();
};

class CWaveOutputDirectSound : public IWaveOutput, public IWinHook {
/**
	DirectSound�ɓ������� class IWaveOutput �h���N���X
	�o�b�t�@�̃��X�g���A�Ǘ����Ă���S�Ă� class IWaveSound �h���N���X��
	Restore�֐��������I�ɌĂт���
*/
public:
	/// CDirectSound�ւ̃|�C���^�����ip==NULL���Ƒ�������j
	/// �ȗ��\�ɂ��Ȃ���ref_creater_and_mediator�̃R���p�C�����o���Ȃ�
	CWaveOutputDirectSound(CDirectSound* p=NULL);
	virtual ~CWaveOutputDirectSound();

	/// �v���C�}���T�E���h�o�b�t�@�̃t�H�[�}�b�g��ύX����
	/// [in] nFrequency - 11kHz(0) or 22kHz(1) or 33kHz(2) or 44kHz(3)
	/// [in] nBit�@�@�@ - 8bit(0) or 16bit(1)
	/// [in] bStereo�@�@- true�Ȃ�X�e���I
	virtual LRESULT ChangePrimaryFormat(int nFrequency, int nBit, bool bStereo);

	/// �Z�J���_���T�E���h�o�b�t�@�𐶐�����
	/// �쐬�ł��Ȃ������Ƃ���CNullSoundBuffer�𐶐�����
	virtual smart_ptr<ISoundBuffer> CreateSecondery();

	/// override from class IWaveOutput
	virtual int	GetType() const { return 1; }
	virtual smart_ptr<ISoundBuffer> CreateBuffer() { return CreateSecondery(); }

protected:
	// �o�b�t�@�̃��X�g�Ď��p
	LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wparam,LPARAM lparam);
	// CDirectSound�ւ̃|�C���^
	CDirectSound* m_pDirectSound;
	CDirectSound* GetDirectSound() const { return m_pDirectSound; }
	// �v���C�}���T�E���h�o�b�t�@
	smart_ptr<ISoundBuffer> m_pPrimary;
	// �v���C�}���T�E���h�o�b�t�@�̎��
	int	m_nPrimaryType;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneWaveOutput_h__
