// yaneSoundParameter.h
//		programmed by ENRA		 '02/03/28

#ifndef __yaneSoundParameter_h__
#define __yaneSoundParameter_h__


namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class ISoundStreamFactory;
class CSoundParameter
/**
	�O���[�o���t�H�[�J�X�A�O���v���O�C���N���X�Ȃǂ̎w���ێ�����N���X
	�ύX�́A���̃N���X�����L���Ă���S�Ă�ISound�C���X�^���X�ɉe������
*/
{
public:
	CSoundParameter();
	virtual ~CSoundParameter(){};

	/**
		�T�E���h�o�b�t�@���A�O���[�o���t�H�[�J�X�����̂��ݒ�E�擾
		�����true�ɂ��Ă����΁A�t�H�[�J�X�����̃X���b�h����������
		�E�B���h�D���痣��Ă��T�E���h�͕�����������
		�������A�T�E���h�o�b�t�@�𐶐��������Ȃ���Δ��f����Ȃ�
		�f�t�H���g�ł�false
	*/
	virtual void SetGlobalFocus(bool b) { m_bGlobalFocus = b; }
	virtual	bool IsGlobalFocus() const { return m_bGlobalFocus; }

	/**
		�T�E���h�o�b�t�@���A�p���R���g���[���@�\�����̂��ݒ�E�擾
		�f�t�H���g�ł�false
	*/
	virtual void SetPanControl(bool b) { m_bPanControl = b; }
	virtual	bool IsPanControl() const { return m_bPanControl; }

	/**
		�T�E���h�o�b�t�@���A���g���R���g���[���@�\�����̂��ݒ�E�擾
		�f�t�H���g�ł�false
	*/
	virtual void SetFrequencyControl(bool b) { m_bFrequencyControl = b; }
	virtual	bool IsFrequencyControl() const { return m_bFrequencyControl; }

	/**
		ISoundStreamFactory�h���N���X�̐ݒ�E�擾
		�O���v���O�C���Ȃǂ̎w��͂��̃N���X�ɑ΂��čs��
		�f�t�H���g�ł�CSoundStreamFactory
	*/
	virtual void SetStreamFactory(const smart_ptr<ISoundStreamFactory>& p) { m_pStreamFactory = p; }
	virtual smart_ptr<ISoundStreamFactory> GetStreamFactory() const { return m_pStreamFactory; }

protected:
	// �T�E���h�o�b�t�@���O���[�o���t�H�[�J�X�����̂�
	bool	m_bGlobalFocus;
	// �T�E���h�o�b�t�@���p���R���g���[���@�\�����̂�
	bool	m_bPanControl;
	// �T�E���h�o�b�t�@�����g���R���g���[���@�\�����̂�
	bool	m_bFrequencyControl;
	// ISoundStream��factory
	smart_ptr<ISoundStreamFactory> m_pStreamFactory;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneSoundParameter_h__
