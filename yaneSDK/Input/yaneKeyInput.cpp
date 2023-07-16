// DirectInput Wrapper

#include "stdafx.h"
#include "yaneKeyInput.h"
#include "../Auxiliary/yaneCOMBase.h"
#include "../AppFrame/yaneAppManager.h"
#include "../AppFrame/yaneAppInitializer.h"

// #include "../Window/yaneDebugWindow.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

CKeyInput::CKeyInput(bool bBackGround){

	m_lpDIKeyDev = NULL;
	m_nStatus = GetDirectInput()->GetStatus();
	if (m_nStatus>=3) return ; // ���������s���Ă�ɂ��

	LPDIRECTINPUT lpDInput = GetDirectInput()->Get();

	//�L�[�{�[�h�f�o�C�X�̍쐬
	if (lpDInput->CreateDevice(GUID_SysKeyboard
			,&m_lpDIKeyDev,NULL)!=DI_OK){
		Err.Out("CKeyInput::CKeyInput��DirectInput�̏�����CreateDevice�Ɏ��s");
		m_nStatus = 6;
		return ; // ���������[����...
	}
	//	�L�[�{�[�h�t�H�[�}�b�g�̐ݒ�
	if(GetDevice()->SetDataFormat(&c_dfDIKeyboard)!=DI_OK){
		Err.Out("CKeyInput::CKeyInput��DirectInput�̏�����SetDataFormat�Ɏ��s");
		m_nStatus = 7;
		return ;
	}
	// WindowNT�œ��삳����ɂ́A�����́A
	// DISCL_FOREGROUND | DISCL_NONEXCLUSIVE�łȂ��Ă͂����Ȃ��B
	
//			CDbg().Out((int)CAppManager::GetHWnd());
	if (bBackGround){
		//	�o�b�N�O���E���h�ł̓��͂�����
		if (GetDevice()->SetCooperativeLevel( CAppManager::GetHWnd(),
			//	�o�b�N�O���E���h�̓��͂�������ꍇ�ł��A�E�B���h�D�n���h���͎w�肷�ׂ�
			DISCL_BACKGROUND| DISCL_NONEXCLUSIVE)!=DI_OK){
			Err.Out("CKeyInput::CKeyInput��DirectInput�̏�����SetCooperativeLevel�Ɏ��s");
			m_nStatus = 8;
			return ;
		}
	} else {
		//	�o�b�N�O���E���h�ł̓��͕͂s����
		if (GetDevice()->SetCooperativeLevel( CAppManager::GetHWnd(),
			DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)!=DI_OK){
			Err.Out("CKeyInput::CKeyInput��DirectInput�̏�����SetCooperativeLevel�Ɏ��s");
			m_nStatus = 8;
			return ;
		}
	}
	/**
		�����AGetHWnd�ł͂Ȃ��ANULL(�f�X�N�g�b�v)�ɂ���΁A
		�f�X�N�g�b�v�Ɗ֘A�t������̂ŁA���̃E�B���h�D���
		�L�[�t�H�[�J�X�������Ƃ����͂��邱�Ƃ��o����悤�ɂȂ�B
	*/

	///	������ʉ߂����Ƃ������Ƃ́Am_nStatus==0 or 1 or 2

	m_bDIKeyAcquire=false;	//	����Ahook�����O�ɐݒ肵�Ȃ�������
	CAppManager::Hook(this);//	�t�b�N���J�n����B
}

// ---------------------------------------------------------------------------
CKeyInput::~CKeyInput(){
	if (m_nStatus <= 2){
		CAppManager::Unhook(this);			//	�t�b�N����������B
	}

	// �l�����Ă����f�o�C�X�̉��
	if(GetDevice()!=NULL){
		if (m_bDIKeyAcquire)
			GetDevice()->Unacquire();
		GetDevice()->Release();
	}

	//	COM�̉���́ACCOMObject������ɂ���Ă����͂��D�D
}

// ---------------------------------------------------------------------------

LRESULT CKeyInput::Input(){
	if (m_nStatus >= 3) {
		return 1;	//	���������s���Ƃ�D�D
	}

	//	�o�b�t�@��flip!
	FlipKeyBuffer(m_nKeyBufNo);

	LRESULT hr;

	// Acquire����Ă��Ȃ��̂�GetDeviceState�����炨�������Ȃ�
	if (!m_bDIKeyAcquire){
		// Acquire���Ƃ��̂�
		hr=m_lpDIKeyDev->Acquire();
		if (hr==DI_OK) {
			m_bDIKeyAcquire=true;
		} else {
			m_bDIKeyAcquire=false;
			// �����Ȃ��������Ƃɂ���:p
			//	�o�b�t�@���N���A���Ė߂�B�i��ʊO�Ƀt�H�[�J�X���ڂ����Ƃ��ɉ������ςȂ�
			//	�ɂȂ�̂�h�����߁j
			::ZeroMemory((LPVOID)&(m_byKeyBuffer[m_nKeyBufNo][0]),256);
			return 1;
		}
	}
	hr = GetDevice()->GetDeviceState(256,
			(LPVOID)&(m_byKeyBuffer[m_nKeyBufNo][0]));
	if (hr==DIERR_INPUTLOST){
	// �A�v���؂�ւ��u�Ԃ͂���[�Ȃ��̂��	
		hr=GetDevice()->Acquire();
		if (hr!=DI_OK) {
			// �����Ȃ��������Ƃɂ���
			m_bDIKeyAcquire = false;	//	����ɂ܂��l�����ĂˁI:p
			//	�o�b�t�@���N���A���Ė߂�B�i��ʊO�Ƀt�H�[�J�X���ڂ����Ƃ��ɉ������ςȂ�
			//	�ɂȂ�̂�h�����߁j
			::ZeroMemory((LPVOID)&(m_byKeyBuffer[m_nKeyBufNo][0]),256);
			return -1; // ���s�̊���
			// Err.Out("GetKeyState�Ńf�o�C�X��LOST���Ă���");
		}
		//	����ok
		hr = GetDevice()->GetDeviceState(256,
			(LPVOID)&(m_byKeyBuffer[m_nKeyBufNo][0]));
	}
	if (hr==DI_OK) {
		return 0;
	} else {
		//	�o�b�t�@���N���A���Ė߂�B�i��ʊO�Ƀt�H�[�J�X���ڂ����Ƃ��ɉ������ςȂ�
		//	�ɂȂ�̂�h�����߁j
		::ZeroMemory((LPVOID)&(m_byKeyBuffer[m_nKeyBufNo][0]),256);
		m_bDIKeyAcquire = false;	//	����ɂ܂��l�����ĂˁI:p
		return 1;
	}
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CKeyInput::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){

	// ALT�Ƃ��Q�[���Ŏg��������ŃL�[���b�Z�[�W�͎̂Ă���...
	if ( WM_KEYFIRST<=uMsg && uMsg<=WM_KEYLAST ) {
//		CDbg().Out(uMsg);
//		return 1;
	}

	switch(uMsg){
	case WM_ACTIVATEAPP: {
/*	//	�܂����̃E�B���h�D���A�N�e�B�u�łȂ�����A�������ق��������񂩁H
		UINT bActive = wParam;
		if(bActive) {
			//	���b�Z�[�W�|���v���ł͂Ȃ��A���[�J�[�X���b�h���Ń��X�g�A���ׂ�...
			// Acquire���Ƃ��̂��H
			if (!m_bDIKeyAcquire) {
				m_bDIKeyAcquire=(m_lpDIKeyDev->Acquire()==DI_OK);
			}
		}
*/
						 } break;
	}
	return 0;	//	all right,sir...
}

//////////////////////////////////////////////////////////////////////////////
//	����0x80��&�Ƃ�K�v�������ŁAoverride�����ق����ǂ��H

inline bool CKeyInput::IsKeyPress(int key) const {
	return	(m_byKeyBuffer[m_nKeyBufNo][key] & 0x80) !=0;
};

inline bool CKeyInput::IsKeyPush(int key) const {
	// �����ꂽ�u�Ԃ�true�ɂ���ꍇ
	if (!(m_byKeyBuffer[  m_nKeyBufNo][key] & 0x80)) return false;
	if (  m_byKeyBuffer[1-m_nKeyBufNo][key] & 0x80) return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////////

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd
