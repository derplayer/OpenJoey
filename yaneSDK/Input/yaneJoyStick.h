// yaneJoyStick.h
//	 This is a JoyStick input wrapper.
//		programmed by yaneurao(M.Isozaki) '99/08/16

#ifndef __yaneJoyStick_h__
#define __yaneJoyStick_h__


#ifdef USE_JOYSTICK

#include "yaneKeyBase.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

class IJoyStick : public IKeyBase {
/// IJoyStick ��IKeyBase��JoyStick��
public:
	///	from CKeyBase
///	virtual LRESULT Input()=0; // 256�o�C�g��Ԃ�ǂݍ���
///	virtual bool	IsKeyPress(int key) const;
///	virtual bool	IsKeyPush(int key) const;
///	virtual const BYTE*	GetKeyData() const;

	virtual LRESULT	SelectDevice(int n)=0;
	///	�f�o�C�X�̃i���o�[���w��(0-15)
	///	���̃f�o�C�X�̏������Ɏ��s������ALRESULT�͔�O��Ԃ�

	///	for JoyStick setting
	virtual LRESULT	SetButtonMax(int max)=0;
	virtual int		GetButtonMax() const=0;
	///	joy stick�̃{�^�����̐ݒ�擾�B
	///	�����Őݒ肵���{�^�����܂ŔF������Bdefault:�ő�6�{�^���B
	/**
		���̃{�^�����̐ݒ�́ASelectDevice���Đ��������Ƃ��̂ݗL��
		default : 6�{�^��
	*/

	virtual bool	IsInit() const=0;
	/**
		�f�o�C�X�̏������ɐ����������H
	*/
};

class CJoyStickNullDevice : public IJoyStick {
/**
	JoyStick�p��Null Device
	class CJoyStick �Ŏg�p�����
*/
public:
	virtual LRESULT Input() { return 0; }
	virtual bool	IsKeyPress(int key) const{ return false;}
	virtual bool	IsKeyPush(int key) const{ return false; }
	virtual LRESULT	SelectDevice(int n) { return 0; }
	virtual LRESULT	SetButtonMax(int max) { return 0; }
	virtual int		GetButtonMax() const { return 0; }
	virtual bool	IsInit() const { return true; }
};

class CWIN32JoyStick : public IJoyStick	 {
///	Win32��joy stick���͂��s�Ȃ�wrapper
public:
	virtual LRESULT Input(); // 256�o�C�g��Ԃ�ǂݍ���

	virtual LRESULT	SelectDevice(int n);
	///	�f�o�C�X�̃i���o�[���w��(0-1)	default : 0
	///	���̃f�o�C�X�̏������Ɏ��s������ALRESULT�͔�O��Ԃ�

	///	for JoyStick setting
	virtual LRESULT	SetButtonMax(int max);
	virtual int		GetButtonMax()const		{ return m_nButtonMax; }
	/**
		���̃{�^�����̐ݒ�́ASelectDevice���Đ��������Ƃ��̂ݗL��
		default : 6�{�^��
	*/

	virtual bool	IsInit() const { return m_bSuccessInit;}
	/**
		�f�o�C�X�̏������ɐ����������H
	*/

	CWIN32JoyStick();

protected:

	bool	m_bSuccessInit;				//	�������ɐ��������̂�
	UINT	m_uID;						//	JOYSTICK1 or JOYSTICK2
	int		m_nButtonMax;				//	�{�^���ő吔(�f�B�t�H���g2)

	UINT	m_uX1,m_uX2;				//	���͕~���l
	UINT	m_uY1,m_uY2;

	int		m_nSelect;					//	JOYSTICK selector
	
};

class CJoyStick : public IJoyStick {
/**

*/
public:
	///	from IKeyBase
	virtual LRESULT Input(); /// 256�o�C�g��Ԃ�ǂݍ���
	virtual bool	IsKeyPress(int key) const;
	virtual bool	IsKeyPush(int key) const;
	virtual const BYTE*	GetKeyData() const;
	/**
		�������͂́AIsPushKey/IsPressKey�̈�����0,1,2,3���㉺���E�B
		���̂���4�`35���A�{�^��1�`32�i�ő�j�ɑΉ�����B
	*/

	virtual LRESULT	SelectDevice(int n);
	///	�f�o�C�X�̃i���o�[���w��(0-15)
	///	���̃f�o�C�X�̏������Ɏ��s������ALRESULT��false��Ԃ�
	/**
		���́ADirectInput��p���Ȃ��Ƃ�DirectX5�̃C���X�g�[������Ă���
		���Ȃ��Win32API��16�{�܂�JoyStick��F���ł���B
	*/

	//	for JoyStick setting
	virtual LRESULT	SetButtonMax(int max);
	virtual int		GetButtonMax()const;
	/**
		���m����ő�{�^������ݒ�B0�`32�Bdefault : 6 �{�^��
		���̃{�^�����̐ݒ�́ASelectDevice���Đ��������Ƃ��̂ݗL��
	*/

	virtual bool	IsInit() const;

	CJoyStick();

protected:
	smart_ptr<IJoyStick>	m_pJoyStick;
	smart_ptr<IJoyStick> GetJoyStick() const { return m_pJoyStick; }
	void	SetNullDevice();	//	JoyStick��NullDevice��ݒ肷��
};

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd

#endif // #ifdef USE_JOYSTICK
#endif
