
//
//	Audio Mixer :
//		�������珑�������������I
//			programmed by yaneurao (M.Isozaki) '02/03/22
//

#include "stdafx.h"
#include "yaneAudioMixer.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

CAudioMixer::CAudioMixer()
{
	m_hMixer = NULL;				//	�~�L�T�f�o�C�X�̃n���h��
	m_MixerID = NULL;				//	�L���ȃ~�L�T�f�o�C�X�̎��ʎq
	m_dwVol = 0;					//	���̃{�����[���ݒ�(Abs��)
	m_lPan = 0;						//	���̃p���ݒ�

	Open();
}

CAudioMixer::~CAudioMixer(){
	Close();
}

//////////////////////////////////////////////////////////////////////////////

LRESULT		CAudioMixer::Open( )
{
	m_bSuccessInit = false;

	if ( Close()!=0 ) return 1;

	if ( ::mixerGetNumDevs() == 0 ) {
		Err.Out( "CAudioMixer::Open �~�L�T�f�o�C�X���� 0���A�G���[���������܂���" );
		return 2;
	}

	// �~�L�T�f�o�C�X���I�[�v������
	MMRESULT	res;
	res = ::mixerOpen( &m_hMixer, m_MixerID, 0, 0, MIXER_OBJECTF_MIXER );
	if ( res != MMSYSERR_NOERROR ) {
		ErrAudioMixer( res, "�~�L�T�f�o�C�X���I�[�v���ł��܂���ł���\n" );		
		return 3;
	}

	//	�~�L�T�f�o�C�X���擾����
	if ( GetMixerDev() != 0 ) return 4;

	//	���̃{�����[���𓾂�
	for ( int i = 0 ; i < 4 ; i++ ) {
		// ����̃~�L�T�R���g���[���𓾂�
		if ( GetMixerLineControl( i ) != 0 ) continue;
		// ���̃{�����|����ۑ�����
		int nSize = m_MCD_UnSigned.size();
		if (nSize!=0){
			GetAudioInfo()[i].m_dwOriginalVol.resize(nSize);
			for (int j=0;j<nSize;j++){
				GetAudioInfo()[i].m_dwOriginalVol[j] = m_MCD_UnSigned[j].dwValue;//	���̃{�����[���ݒ�(�`�����l��1)
			}
		} else {
			GetAudioInfo()[i].m_dwOriginalVol.clear();
		}
	}

	//	���̎��_�ŏ���������
	m_bSuccessInit = true;
	return 0;
}


////////////////////////////////////
//	SetVolumeRel
//	�{�����[���̐ݒ�(����)
////////////////////////////////////
LRESULT		CAudioMixer::SetVolumeRel( eAudioMix eDevice, double dRelVolume)
{
	if (!IsSuccessInit()) return -1; // �������Ɏ��s���Ă���

	//	���̃{�����[���ɑ΂��鉹�ʂ̐ݒ�
	vector<DWORD>	nowVol;

	GetVolumeOrg( eDevice, nowVol );	
	//	���̃{�����[�����擾����

	for(int i=0;i<(int)nowVol.size();i++){
		nowVol[i] = (DWORD)(nowVol[i] * dRelVolume);
	}

	// �{�����[���̐ݒ�(�Ϗ�����)
	return SetVolumeAbs( eDevice, nowVol);
}


///////////////////////////////////////////////////////////////
//	SetVolumeAbs
//	�{�����[���̐ݒ�(���)
///////////////////////////////////////////////////////////////
LRESULT		CAudioMixer::SetVolumeAbs( eAudioMix eDevice, const vector<DWORD>& adwVol)
{
	if (!IsSuccessInit()) return -1; // �������Ɏ��s���Ă���

	// ���݂̃R���g���[���̎擾
	if (GetMixerLineControl( eDevice )!=0) return -1;

	for ( DWORD i = 0 ; i < m_MixerLine[eDevice].cChannels ; i++ ){
		DWORD dwVolume;
		if (i < adwVol.size()) {
			dwVolume = adwVol[i];
			if ( dwVolume > 65535 ) dwVolume = 65535;
//			if ( dwVolume < 0 ) dwVolume = 0;	//	�������Ȃ̂ł��肦�Ȃ�
		} else {
			dwVolume = 0;
		}
		m_MCD_UnSigned[i].dwValue = dwVolume;
	}

	// �{�����[���̍X�V
	SetMixerControlDetails( eDevice, false );

	return 0;
}


//////////////////////////////////////////
//	GetVolumeRel
//	�{�����[���̎擾(����)
//////////////////////////////////////////
LRESULT		CAudioMixer::GetVolumeRel(eAudioMix eDevice,double &dRelVolume) const {
	if (!IsSuccessInit()) return -1; // �������Ɏ��s���Ă���

	// ���݂̃{�����[���l�̎擾
	vector<DWORD>	nowVol;

	if ( GetVolumeAbs( eDevice, nowVol) != 0 ) return 1;

	//	���̃{�����[���ɑ΂��鉹�ʂ̐ݒ�
	vector<DWORD>	orgVol;
	if (GetVolumeOrg( eDevice, orgVol )!=0 ) return 1;

	//	����vol���v�ƁA���݂�vol���v�̔䗦�Ōv�Z����
	DWORD dwNow=0;
	{for(int i=0;i<(int)nowVol.size();i++){ dwNow += nowVol[i]; }}
	DWORD dwOrg=0;
	{for(int i=0;i<(int)orgVol.size();i++){ dwOrg += orgVol[i]; }}
	
	if (dwOrg!=0) {	//	���̔��肢����..
		dRelVolume = (dwNow)/(double)(dwOrg);
	} else {
		if (dwNow==0){
			dRelVolume = 0;
		} else {
			dRelVolume = 0xffff;	// infinity..
		}
	}
	return 0;
}


//////////////////////////////////////////
//	GetVolumeAbs
//	�{�����[���̎擾(���)
//////////////////////////////////////////
LRESULT		CAudioMixer::GetVolumeAbs(eAudioMix eDevice,vector<DWORD> &dwVolume) const
{
	if (!IsSuccessInit()) return -1; // �������Ɏ��s���Ă���

	if (const_cast<CAudioMixer*>(this)->GetMixerLineControl( eDevice )!=0) return -1;

	int nSize = m_MCD_UnSigned.size();
	if (nSize!=0) {
		dwVolume.resize(nSize);
		for(int i=0;i<nSize;i++){
			dwVolume[i] = m_MCD_UnSigned.get(i)->dwValue;		// ���݂̉��ʂ𓾂�
		}
	} else {
		dwVolume.clear();
	}

	return 0;
}

////////////////////////////////////////
//	GetVolumeOrg
//	���̃{�����[�����擾����
////////////////////////////////////////
LRESULT	CAudioMixer::GetVolumeOrg( eAudioMix eDevice, vector<DWORD> &vol) const
{
	if (!IsSuccessInit()) return -1; // �������Ɏ��s���Ă���

	vol = const_cast<CAudioMixer*>(this)->GetAudioInfo()[eDevice].m_dwOriginalVol;
	return 0;
}

////////////////////////////////////////
//	SetVolumeOrg
//	���̃{�����[���ɐݒ肷��
////////////////////////////////////////
LRESULT	CAudioMixer::SetVolumeOrg(eAudioMix eDevice)
{
	if (!IsSuccessInit()) return -1; // �������Ɏ��s���Ă���

	int nLineID = (int)eDevice;
	if ( m_hMixer == NULL ) return 1;
	if (GetMixerLineControl(nLineID)!=0) return 2;

	int nSize = m_MCD_UnSigned.size();
	for(int i=0;i<nSize;i++){
		if ((int)GetAudioInfo()[nLineID].m_dwOriginalVol.size()>i){
			m_MCD_UnSigned[i].dwValue = GetAudioInfo()[nLineID].m_dwOriginalVol[i];
		}
	}
	return SetMixerControlDetails(nLineID,false);
}

////////////////////////////////
//	Close
//	close audio mixer(and retrive volume)
////////////////////////////////
LRESULT		CAudioMixer::Close( ){

	LRESULT lr = 0;

	if ( m_hMixer != NULL ){
		for ( int i = 0 ; i < 4 ; i++ )
			SetVolumeOrg((eAudioMix)i);	//	���̃{�����[���l�ɖ߂�

		MMRESULT	res = ::mixerClose ( m_hMixer );								//	�~�L�T�f�o�C�X�����
		if ( res != MMSYSERR_NOERROR ) {
			ErrAudioMixer( res, "�~�L�T�f�o�C�X������܂���ł���\n" );
			lr = 1;
		}
	}

	//	�~�L�T�R���g���[���̃v���p�e�B���擾�A�ݒ�ł���\���̂̃��������
	m_MCD_UnSigned.Delete();
	return lr;
}

//////////////////////////////////////////////
//	GetMixerLineControl
//	�I�[�f�B�I���C���̃R���g���[�����𓾂�
//////////////////////////////////////////////
LRESULT	CAudioMixer::GetMixerLineControl( int iLineID ){

	// �~�L�T�f�o�C�X�̃n���h����\�ߓ��Ă��邩�ۂ��̔���
	if ( m_hMixer == NULL ) {
		Err.Out( "�~�L�T�f�o�C�X�̃n���h��������܂���ł���" );
		return 1;
	}

	// iLineID���T�|�[�g���Ă��邩�ۂ��̔���
	if (! GetAudioInfo()[iLineID].m_bAudioComponent ) {
		Err.Out("%d��ID�̃I�[�f�B�I �R���|�[�l���g�́A���̃T�E���h�{�[�h�ɂ̓T�|�[�g����Ă��܂���ł���",iLineID);
		return 1;
	}

	return GetMixerLineControl(m_MixerLine[iLineID]);
}

LRESULT CAudioMixer::GetMixerLineControl(MIXERLINE& ml){

	if (ml.cControls<=0) return 1; // ����������炾�߂�

	// �~�L�T�R���g���[�������ɓ���ϐ��Ƀ��������m�ۂ���
	smart_ptr<MIXERCONTROL> pMc;
	pMc.AddArray(ml.cControls);

	// �I�[�f�B�I���C���R���g���[�����̏���������
	MIXERLINECONTROLS&	mlc =  m_MixerLineControl;
	mlc.cbStruct	= sizeof (MIXERLINECONTROLS);
	mlc.dwLineID	= ml.dwLineID;
	mlc.cControls	= ml.cControls;
	mlc.cbmxctrl	= sizeof (MIXERCONTROL);
	mlc.pamxctrl	= pMc.get();

	// �I�[�f�B�I���C���R���g���[�����𓾂�
	MMRESULT	res;
	res = ::mixerGetLineControls( (HMIXEROBJ)m_hMixer, &mlc, MIXER_GETLINECONTROLSF_ALL );
	if ( res != MMSYSERR_NOERROR) {
		ErrAudioMixer( res, "�I�[�f�B�I���C���̃R���g���[�����𓾂��܂���ł���\n" );
		return 1;
	}

	bool	bResult = false;
	for (DWORD i = 0 ; i < mlc.cControls ; i++ ){
		if ( pMc[i].dwControlType == MIXERCONTROL_CONTROLTYPE_VOLUME ){

			//	�~�L�T�R���g���[����ԏ��\���̂𓾂�
			//	�~�L�T�R���g���[���̃v���p�e�B���擾�A�ݒ�ł���\���̂̃������m��
			DWORD channels = ml.cChannels;
			if (channels == 0) {
//				Err.Out("%d��ID�̃I�[�f�B�I �R���|�[�l���g�̓`�����l������0�ł���",i);
				continue;
			}

/**
			m_MCD_UnSigned.AddArray(max(channels,2));
			//	channel����1�ł�2�Ƃ��Ĉ���Ȃ��ƁA���ʕۑ��̕������߂�ǂ��O�O�G
*/

			m_MCD_UnSigned.AddArray(channels);

			// �~�L�T�R���g���[����ԏ��\���̂̏���������
			m_MCD.cbStruct		=	sizeof (MIXERCONTROLDETAILS);
			m_MCD.dwControlID	=	pMc[i].dwControlID;
			m_MCD.cChannels		=	channels;
			m_MCD.cMultipleItems=	pMc[i].cMultipleItems;
			m_MCD.cbDetails		=	sizeof (MIXERCONTROLDETAILS_UNSIGNED);
			m_MCD.paDetails		=	m_MCD_UnSigned.get();

			// �~�L�T�R���g���[����ԏ��\���̂̎擾
			res = ::mixerGetControlDetails( (HMIXEROBJ)m_hMixer, &m_MCD, MIXER_GETCONTROLDETAILSF_VALUE );
			if ( res != MMSYSERR_NOERROR ) {
//				ErrAudioMixer( res, "�~�L�T�R���g���[����ԏ��\���̂̎擾���ł��܂���ł���\n" );
				continue;
			}

			//	�悤�₭�R�s�[
			m_MixerControl = pMc[i];
			bResult = true;
			break;
		}
	}

	// ���s���Ă�����
	if ( !bResult ) {
		Err.Out( "MixerControl�\���̂Ɋ֘A�t������I�[�f�B�I���C���R���g���[��������܂���ł���" );
		return 1;
	}

	return 0;
} // GetMixerControlDetails


///////////////////////////////////////////////
//	SetMixerControlDetails
//	�~�L�T�R���g���[����ԏ��\���̂𓾂�
///////////////////////////////////////////////
LRESULT	CAudioMixer::SetMixerControlDetails( int iLineID, bool bMono )
{
	// �~�L�T�f�o�C�X�̃n���h����\�ߓ��Ă��邩�ۂ�
	if ( m_hMixer == NULL ) {
		Err.Out( "�~�L�T�f�o�C�X�̃n���h��������܂���ł���" );
		return 1;
	}
	// iLineID���T�|�[�g���Ă��邩�ۂ��̔���
	if ( !GetAudioInfo()[iLineID].m_bAudioComponent) {
		Err.Out( iLineID + "��ID�̃I�[�f�B�I �R���|�[�l���g�́A���̃T�E���h�{�[�h�ɂ̓T�|�[�g����Ă��܂���ł���" );
		return 1;
	}

	if (m_MCD_UnSigned.size()==0) {
		//	�~�L�T�[������H(;�L�E�ցE`)���ް�
		return 1;
	}

	// �~�L�T�R���g���[����ԏ��\���̂̏���������
	m_MCD.cbStruct		= sizeof (MIXERCONTROLDETAILS);
	m_MCD.dwControlID	= m_MixerControl.dwControlID;
	if ( bMono ) {
		m_MCD.cChannels		=	1;
	} else {
		m_MCD.cChannels		=	m_MixerLine[iLineID].cChannels;
	}
	m_MCD.cMultipleItems	=	m_MixerControl.cMultipleItems;
	m_MCD.cbDetails			=	sizeof (MIXERCONTROLDETAILS_UNSIGNED);
	m_MCD.paDetails			=	m_MCD_UnSigned.get();

	// �~�L�T�R���g���[����ԏ��\���̂̎擾
	MMRESULT	res;
	res = ::mixerSetControlDetails( (HMIXEROBJ)m_hMixer, &m_MCD, MIXER_GETCONTROLDETAILSF_VALUE );
	if ( res != MMSYSERR_NOERROR ){
		ErrAudioMixer( res, "�~�L�T�R���g���[����ԏ��\���̂̎擾���ł��܂���ł���\n" );
		return 1;
	}

	return 0;
} // SetMixerControlDetails


/////////////////////////////////////////// Private /////////////////////////////////////////
//////////////////////////////
//private for Open
//	GetMixerDev
//	�~�L�T�f�o�C�X���擾����
//////////////////////////////
LRESULT	CAudioMixer::GetMixerDev( )
{
	// �~�L�T�f�o�C�X�̏��𓾂�
	MMRESULT	res;
	res = ::mixerGetDevCaps( (UINT)m_MixerID, &m_MixerCaps, sizeof (m_MixerCaps) );
	if ( res != MMSYSERR_NOERROR ) {
		ErrAudioMixer( res, "�~�L�T�f�o�C�X���𓾂��܂���ł���\n" );
		return 1;
	}

	for (DWORD i = 0 ; i < m_MixerCaps.cDestinations ; i++ )
	{
		// �~�L�T�f�o�C�X�̃f�B�X�e�B�l�\�V�����A�\�[�X���𓾂�
		MIXERLINE	ml = { sizeof(ml) };		//	�I�[�f�B�I���C����Ԃƃ��g���b�N�X��񂪓���\����
		// �~�L�T�f�o�C�X�̃f�B�X�e�B�l�\�V�������𓾂�
		ml.dwDestination = i;

		res = ::mixerGetLineInfo( (HMIXEROBJ)m_hMixer, &ml, MIXER_GETLINEINFOF_DESTINATION );
		if ( res != MMSYSERR_NOERROR ) {
			ErrAudioMixer( res, "�~�L�T�f�o�C�X�̃f�B�X�e�B�l�\�V�������𓾂��܂���ł���\n" );
			return 1;
		}
		//	Get�����f�B�X�e�B�l�[�V�������A�I�[�f�B�I�o�͂ɑ΂���R���|�[�l���g���ۂ���
		//	���肷��(�w�b�h�z��������ɓ���)
		if ( ml.dwComponentType == MIXERLINE_COMPONENTTYPE_DST_SPEAKERS ) {

			//	bug-fixed '00/09/13
			//	�����́A���C���o�͂ł���̂��𒲂ׂāA���C���o�͂ł���Ȃ�΁A������擾����
			if (GetMixerLineControl(ml)!=0) continue;	//	����ȓz�͎��i�Ȃ��I�O�O�G

			m_MixerLine[0] = ml;
			GetAudioInfo()[0].m_bAudioComponent = true;
			///	���ꂪ�}�X�^�[���H�����[���ɈႢ����

			DWORD	iDstConnect = ml.cConnections;
			// �~�L�T�f�o�C�X�̃\�[�X���𓾂�
			for (DWORD j = 0 ; j < iDstConnect ; j++ )
			{
				ml.cbStruct = sizeof (ml);
				ml.dwDestination = i;
				ml.dwSource = j;
				res = ::mixerGetLineInfo( (HMIXEROBJ)m_hMixer, &ml, MIXER_GETLINEINFOF_SOURCE );
				if ( res != MMSYSERR_NOERROR ) {
					ErrAudioMixer( res, "�~�L�T�f�o�C�X�̃\�[�X���𓾂��܂���ł���\n" );
					return 1;
				}
				int nLineNo=0;
				switch (ml.dwComponentType) {
				case MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT:
					nLineNo = 1; break;
				case MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER:
					nLineNo = 2; break;
				case MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC:
					nLineNo = 3; break;
				}

				//	�ŏ��Ɏ擾�����Ƃ��납��㏑���͏o���Ȃ��悤�ɂ��� '00/09/13				//	�ˁ@�{���́A
				//	�����̃T�E���h�J�[�h���T�|�[�g���Ȃ���΂Ȃ�Ȃ��̂ł́D�D

				if (nLineNo!=0 && !GetAudioInfo()[nLineNo].m_bAudioComponent
					&& GetMixerLineControl(ml)==0) {
					m_MixerLine[nLineNo] = ml;
					GetAudioInfo()[nLineNo].m_bAudioComponent = true;
				}
			}
		}
	}
	return 0;
} // GetMixerDev

LRESULT	CAudioMixer::RestoreVolume(eAudioMix eDevice){
	vector<DWORD> adwVol;
	if (GetVolumeAbs(eDevice,adwVol)!=0){
		return 1;
	}
	GetAudioInfo()[eDevice].m_dwOriginalVol = adwVol;
	//	�������H�����[���ʂƂ��ăR�s�[�I
	return 0;
}

/////////////////////////////////////
//	ErrAudioMixer
//	�I�[�f�B�I�~�L�T�̃G���[����
/////////////////////////////////////
void	CAudioMixer::ErrAudioMixer( MMRESULT AMErr, LPCSTR strBuf )
{
	switch ( AMErr )
	{
		case MMSYSERR_NOERROR:
			Err.Out( "CAudioMixer::����HNOERROR���Ԃ��Ă܂���" );
			break;
		case MMSYSERR_INVALHANDLE:
			Err.Out( "CAudioMixer::�w�肵���f�o�C�X�n���h�����s���ł�" );
			break;
		case MIXERR_INVALCONTROL:
			Err.Out( "CAudioMixer::�R���g���[���̎Q�Ƃ��s���ł�" );
			break;
		case MMSYSERR_BADDEVICEID:
			Err.Out( "CAudioMixer::�f�o�C�X���ʎq���s���܂��͗L���͈͊O�ł���" );
			break;
		case MMSYSERR_INVALFLAG:
			Err.Out( "CAudioMixer::�t���O�̎w�肪�s���ł�" );
			break;
		case MMSYSERR_INVALPARAM:
			Err.Out( "CAudioMixer::�A�h���X���s���ł�" );
			break;
		case MMSYSERR_NODRIVER:
			Err.Out( "CAudioMixer::�f�o�C�X�h���C�o�����݂��܂���ł���" );
			break;
		case MMSYSERR_ALLOCATED:
			Err.Out( "CAudioMixer::�w�肵�����\�[�X�́A�ő勖�e���̃N���C�A���g�ɂ��łɊ��蓖�Ă��Ă��܂���" );
			break;
		case MMSYSERR_NOMEM:
			Err.Out( "CAudioMixer::�K�v�ȃ����������蓖�Ă邱�Ƃ��ł��܂���ł���" );
			break;

		default:
			Err.Out( "CAudioMixer::�����s���̃G���[���N����܂���" );
			break;
	}
	Err.Out( strBuf );
} // ErrAudioMixer

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
