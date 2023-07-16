//	yaneAudioMix.h : �E�F�[�u�I�[�f�B�I�~�L�T
//
//		programmed by yaneurao '00/02/28
//

#ifndef __yaneAudioMixer_h__
#define __yaneAudioMixer_h__


namespace yaneuraoGameSDK3rd {
namespace Multimedia {

enum eAudioMix {
AUDIO_MIX_MASTER,	// �}�X�^�[���H�����[��
AUDIO_MIX_WAVE,		// WAVE���H�����[��
AUDIO_MIX_MIDI,		// MIDI�o�̓��H�����[��
AUDIO_MIX_CD		// CD�Đ����H�����[��
};

class IAudioMixer {
public:
	virtual LRESULT SetVolumeRel(eAudioMix eDevice,double dRelVolume)=0;
	virtual LRESULT SetVolumeAbs(eAudioMix eDevice,const vector<DWORD>& adwVol)=0;
	virtual LRESULT SetVolumeOrg(eAudioMix eDevice)=0;
	virtual LRESULT GetVolumeRel(eAudioMix eDevice,double &dRelVolume)const=0;
	virtual LRESULT GetVolumeAbs(eAudioMix eDevice,vector<DWORD>& adwVol)const=0;
	virtual LRESULT	GetVolumeOrg(eAudioMix eDevice,vector<DWORD>& adwVol)const=0;
	virtual	LRESULT	RestoreVolume(eAudioMix eDevice)=0;

	virtual ~IAudioMixer(){}
};

class CAudioMixer : public IAudioMixer {
/**
	�I�[�f�B�I�̃~�L�V���O�p�BMIDI�ACD�AWAVE�̃~�L�V���O�𒼐ڕύX����B
	�~�L�V���O���R���g���[������̂ŁA���̋@�킩�璼�ڃX�s�[�J�[��
	�o�͂��Ă���ꍇ�͖��Ӗ��ł���B

	class CVolumeFader ���Q�Ƃ��邱�ƁB

	���΃��H�����[���́A�����l�iCAudioMixer�̃R���X�g���N�g��
	�����������_�ł̒l�j����̑��Ύw��ƂȂ�܂��B
	�����l�Ȃ��1(100%)���̂Q�{�Ȃ��2(200%)�A�O�Ȃ�Ζ����ʁB
*/
public:
	virtual LRESULT SetVolumeRel(eAudioMix eDevice,double dRelVolume);
	///	���΃{�����[���ݒ�

	virtual LRESULT SetVolumeAbs(eAudioMix eDevice,const vector<DWORD>& adwVol);
	///	��΃{�����[���ݒ�

	virtual LRESULT SetVolumeOrg(eAudioMix eDevice);
	///	�����{�����[���ɐݒ�

	virtual LRESULT GetVolumeRel(eAudioMix eDevice,double &dRelVolume)const;
	///	���΃{�����[���擾

	virtual LRESULT GetVolumeAbs(eAudioMix eDevice,vector<DWORD>& adwVol)const;
	///	��΃{�����[���擾

	virtual LRESULT	GetVolumeOrg(eAudioMix eDevice,vector<DWORD>& adwVol)const;
	///	�����{�����[���擾

	virtual	LRESULT	RestoreVolume(eAudioMix eDevice);
	///	���݂̃{�����[���ݒ���A�u�����{�����[���ݒ�v�ɕۑ�����

	bool	IsSuccessInit() const { return m_bSuccessInit; }
	///	�������ɐ����������H

	CAudioMixer();
	virtual ~CAudioMixer();

protected:
	DWORD	m_dwVol;				//	���݂̃{�����[���ݒ�
	LONG	m_lPan;					//	���݂̃p���ݒ�

private:
	struct CAudioInfo {
		bool			m_bAudioComponent;		//	�R���|�[�l���g���T�|�[�g����Ă��邩�ۂ��̃t���O
		vector<DWORD>	m_dwOriginalVol;		//	���̃{�����[���ݒ�(�`�����l��)
		CAudioInfo() :m_bAudioComponent(false) {}
	};
	CAudioInfo	m_vAudioInfo[4];
	CAudioInfo* GetAudioInfo() { return& m_vAudioInfo[0]; }

	HMIXER		m_hMixer;			//	�~�L�T�f�o�C�X�̃n���h��

	UINT		m_MixerID;			//	�L���ȃ~�L�T�f�o�C�X�̎��ʎq
	
	MIXERCAPS	m_MixerCaps;		//	�~�L�T�f�o�C�X���\����
	
	MIXERLINE	m_MixerLine[4];		//	4��AudioMix�ɑ΂���Line(���Ԃ�enum��)
	//	�I�[�f�B�I���C����Ԃƃ��g���b�N�X��񂪓���\����
	
	MIXERCONTROL		m_MixerControl;
	//	�I�[�f�B�I���C���R���g���[���̃X�e�[�^�X�ƃ��g���b�N�X��񂪓���\����

	MIXERLINECONTROLS	m_MixerLineControl;
	//	�I�[�f�B�I���C���̃R���g���[����񂪓���\����

	MIXERCONTROLDETAILS	m_MCD;
	//	�~�L�T�R���g���[���̏�ԏ��\����

	smart_ptr<MIXERCONTROLDETAILS_UNSIGNED>	m_MCD_UnSigned;
	//	�~�L�T�R���g���[���̃v���p�e�B���擾�A�ݒ�ł���\����

protected:
	LRESULT	Open();
	LRESULT Close();

	LRESULT	GetMixerLineControl( int iLineID );		//	Detail�̎擾�����˂�B���̊֐��ɈϏ�����
	LRESULT GetMixerLineControl(MIXERLINE& ml);		//	�����炪�{���O�O�G
	LRESULT	SetMixerControlDetails( int iLineID, bool bMono );

private:
	LRESULT	GetMixerDev();
	void	ErrAudioMixer( MMRESULT AMErr, LPCSTR strBuf );
	bool	m_bSuccessInit;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif
