// JoyStick Input Wrapper

#include "stdafx.h"

#ifdef USE_JOYSTICK

#include "yaneJoyStick.h"

//////////////////////////////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Input {

CJoyStick::CJoyStick(){
	SetNullDevice();
}

void	CJoyStick::SetNullDevice(){
	//	Null Device��˂�����ǂ�
	m_pJoyStick.Add(new CJoyStickNullDevice);
}

LRESULT CJoyStick::SetButtonMax(int max){
	return GetJoyStick()->SetButtonMax(max);
}

int	 CJoyStick::GetButtonMax() const {
	return GetJoyStick()->GetButtonMax();
}

LRESULT CJoyStick::SelectDevice(int j){
	//	�K��O�̐��l�Ȃ��Null Device��˂�����ǂ�
	if (j<0) { SetNullDevice(); return 1; }

	m_pJoyStick.Add(new CWIN32JoyStick);
	if (GetJoyStick()->SelectDevice(j)!=0){
		return 1;
	}
	return 0;
}

LRESULT CJoyStick::Input(){
	return GetJoyStick()->Input();
}

bool CJoyStick::IsKeyPress(int key) const{
	return GetJoyStick()->IsKeyPress(key);
}
bool CJoyStick::IsKeyPush(int key) const{
	return GetJoyStick()->IsKeyPush(key);
}
const BYTE* CJoyStick::GetKeyData() const{
	return GetJoyStick()->GetKeyData();
}

bool	CJoyStick::IsInit() const {
	return GetJoyStick()->IsInit();
}

//////////////////////////////////////////////////////////////////////////////

CWIN32JoyStick::CWIN32JoyStick(){
	m_nButtonMax	=	6;					//	�U�{�^�����f�B�t�H���g
	m_nSelect		=	0;
	m_bSuccessInit	=	false;
}

LRESULT CWIN32JoyStick::SetButtonMax(int max) {
	// �{�^���̐���ς��邭�炢�Ȃ�Initialize�͂���Ȃ�
	m_nButtonMax = max;
	Reset();
	return 0;
}

LRESULT CWIN32JoyStick::SelectDevice(int j) {
	m_nSelect = j;
	m_bSuccessInit	=	false;

	int n = joyGetNumDevs();
	if ( j < 0 || j>=n) return 1;	//	�W���C�X�e�B�b�N����ւ���...
	/**
		�˂���ǂ����ADirectX5���C���X�g�[������Ă�����ł�16���Ԃ��Ă���
	*/

	m_uID = JOYSTICKID1 + j;
	//	���́A���̎w������Ă������I
	/*
	switch (m_nSelect){
	case 0: m_uID = JOYSTICKID1; break;
	case 1: m_uID = JOYSTICKID2; break;
	default: return 2;	//	�K��O�̑I��
	}
	*/

	JOYCAPS joyCaps;
	if ( joyGetDevCaps(m_uID,&joyCaps,sizeof(JOYCAPS)) != JOYERR_NOERROR) {
		return 3;	//	�������Ɏ��s
	}

	//	�Z���^�[�̓j���[�g�������낤����A�P�F�Q�ɓ�������Ƃ����臒l�Ƃ���
	m_uX1	=	(joyCaps.wXmax	 +	joyCaps.wXmin*2)/3;
	m_uX2	=	(joyCaps.wXmax*2 +	joyCaps.wXmin  )/3;
	m_uY1	=	(joyCaps.wYmax	 +	joyCaps.wYmin*2)/3;
	m_uY2	=	(joyCaps.wYmax*2 +	joyCaps.wYmin  )/3;

	JOYINFO joyinfo;	//	���ۂɓǂݍ��߂�̂��m�F���Ă����K�v����
	if (joyGetPos(m_uID,&joyinfo)!= JOYERR_NOERROR) return 4;

	m_bSuccessInit	=	true;
	Reset();	//	�r���ł̃f�o�C�X�؂�ւ��ɑΉ����邽�߈ꉞ�A�N���A����B

	return 0;
}

// ---------------------------------------------------------------------------

LRESULT CWIN32JoyStick::Input(){
	if (!m_bSuccessInit) return 1;	//	���݂��A�����������������s���Ƃ邪��..

	//	�o�b�t�@��flip!
	FlipKeyBuffer(m_nKeyBufNo);

	//	�{�^���S�ȉ��̂Ƃ��́AjoyGetPos�ŏ\��...
	if (m_nButtonMax<=4) {
		UINT x,y,b;
		//	�{���Ȃ�΁A�R�[���o�b�N�֐��ɂ��ă��b�Z�[�W�n���h����p�ӂ��ׂ�...
		JOYINFO joyinfo;
		if (joyGetPos(m_uID,&joyinfo)!= JOYERR_NOERROR) return 2;
		x = WORD(joyinfo.wXpos);		//	X���W
		y = WORD(joyinfo.wYpos);		//	Y���W
		b = WORD(joyinfo.wButtons);		//	�{�^�����
		m_byKeyBuffer[m_nKeyBufNo][0] = y<m_uY1?1:0;	//	��
		m_byKeyBuffer[m_nKeyBufNo][1] = y>m_uY2?1:0;	//	��
		m_byKeyBuffer[m_nKeyBufNo][2] = x<m_uX1?1:0;	//	��
		m_byKeyBuffer[m_nKeyBufNo][3] = x>m_uX2?1:0;	//	�E
		//	�S�ԁ`�R�T�i�ő�j�Ԃ܂ł��A�e�{�^�����ł���I
		//	�S�Ԃ��{�^���P�C�T�Ԃ��{�^���Q�D�D�ȉ��R�T�Ԃ܂�
		for(int i=0;i<m_nButtonMax;i++){
			m_byKeyBuffer[m_nKeyBufNo][i+4] = b & 1;
			b >>= 1;
		}
	} else {
		DWORD x,y,b;
		JOYINFOEX joyinfo;
		joyinfo.dwSize	= sizeof(JOYINFOEX);
		joyinfo.dwFlags	= JOY_RETURNALL;
		if (joyGetPosEx(m_uID,&joyinfo)!= JOYERR_NOERROR) return 2;
		x = WORD(joyinfo.dwXpos);		//	X���W
		y = WORD(joyinfo.dwYpos);		//	Y���W
		b =	 joyinfo.dwButtons;			//	�{�^�����
		m_byKeyBuffer[m_nKeyBufNo][0] = y<m_uY1?1:0;	//	��
		m_byKeyBuffer[m_nKeyBufNo][1] = y>m_uY2?1:0;	//	��
		m_byKeyBuffer[m_nKeyBufNo][2] = x<m_uX1?1:0;	//	��
		m_byKeyBuffer[m_nKeyBufNo][3] = x>m_uX2?1:0;	//	�E
		for(int i=0;i<m_nButtonMax;i++){
			m_byKeyBuffer[m_nKeyBufNo][i+4] = (BYTE)(b & 1);
			b >>= 1;
		}
	}
	return 0;
}

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd

#endif // #ifdef USE_JOYSTICK
