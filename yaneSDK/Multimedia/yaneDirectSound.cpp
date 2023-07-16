// DirectSound Wrapper

	//�@�T�E���h�h���C�o�[�𕡐��T�|�[�g����K�v������̂Ȃ�΁A
	//	�T�E���h����݂̊֐��́A���ׂĊ֐��|�C���^�Ƃ��Ĉ����ׂ�
	//	�����A�K���ɂ��āADirectSound�����邩�Ȃ��������Ȃ̂ŁA
	//	�����܂ł��Ȃ��B

#include "stdafx.h"
#include "yaneDirectSound.h"
#include "../Auxiliary/yaneCOMBase.h"		//	COM�̗��p�̂���
#include "../AppFrame/yaneAppManager.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

//////////////////////////////////////////////////////////////////////////////

// static member
bool CDirectSound::CanUseDirectSound(){
	//	DirectMusic���g������Ȃ̂��ǂ����𒲂ׂĕԂ�
	static bool bFirst = true;
	static bool bUse   = false;

	if ( bFirst ) {	//	�ŏ��̂P��̂ݒ��ׂĕԂ�
		bFirst = false;
		CCOMObject<LPDIRECTSOUND> obj;
		bUse = obj.CreateInstance(CLSID_DirectSound, IID_IDirectSound)==0;
	}
	return bUse;
}

//////////////////////////////////////////////////////////////////////////////

CDirectSound::CDirectSound()
{
	m_nStatus = 0;
	m_hWnd = CAppManager::GetHWnd();

	// Create the DirectSound object
	if ( GetObject()->CreateInstance(CLSID_DirectSound, IID_IDirectSound)!=0 ) {
		Err.Out("CDirectSound::CDirectSound() object�̍쐬�Ɏ��s");
		// SoundCARD�����݂��Ȃ��ꍇ�́A���̃G���[
		m_nStatus = 1;
		return ;
	}

	// ���̑�P�p�����[�^�̓T�E���h�J�[�h��GUID
	if ( Get()&&Get()->Initialize(NULL)==DS_OK ) {
	} else {
		Err.Out("CDirectSound::CDirectSound() Initialize�Ɏ��s");
		// COM�I�u�W�F�N�g����̂���
		GetObject()->Release();
		m_nStatus = 2;
		return ;
	}

	// Set the cooperation level for the DirectSound object
	if ( SetCooperativeLevel(DSSCL_NORMAL)!=DS_OK ) {
		Err.Out("CDirectSound::CDirectSound() SetCooperativeLevel�̎��s");
		// COM�I�u�W�F�N�g����̂���
		GetObject()->Release();
		m_nStatus = 3;
		return ;
	}
}

CDirectSound::~CDirectSound(){
	// �O�̂��߁A���Ƃ̋������x���ɖ߂�
	SetCooperativeLevel(DSSCL_NORMAL);
}

//////////////////////////////////////////////////////////////////////////////
LRESULT CDirectSound::SetCooperativeLevel(DWORD dwLevel)
{
	// �������Ɏ��s���Ă���G���[
	if ( m_nStatus!=0 ) {
		Err.Out("CDirectSound::SetCooperativeLevel(%d) DirectSound�̏������Ɏ��s���Ă���", dwLevel);
		return 1;
	}

	if ( Get()->SetCooperativeLevel(m_hWnd, dwLevel)!=DS_OK ) {
		Err.Out("CDirectSound::SetCooperativeLevel(%d) SetCooperativeLevel�̎��s", dwLevel);
		return 2;
	}
	
	return 0;
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
