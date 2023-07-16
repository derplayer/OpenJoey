// yaneKeyBase.h
//	 This is a based wrapper of input.
//		programmed by yaneurao(M.Isozaki) '99/11/13

#ifndef __yaneKeyBase_h__
#define __yaneKeyBase_h__


namespace yaneuraoGameSDK3rd {
namespace Input {

class IKeyBase {
/**
	�L�[���͂̂��߂̊��N���X�B
	DirectInput,JoyStick,MIDI Input���̓��͌n�ɑ΂���
	���ʂ̃C���^�[�t�F�[�X��^���邽�߂Ɏg���B
*/
public:
	virtual LRESULT	Input() = 0;		// must be override...
	/// ��Ԏ擾�i��������s�����u�ԁA�L�[�̃��A���^�C�������擾����j

	/**
		�ȉ��̂R�̊֐��ŁA�L�[������s�Ȃ��B
		GetKeyState�ɂ���ē���ꂽ���𒲂ׂĕԂ������Ȃ̂ŁA
		GetKeyState��Y��Ȃ��悤�ɒ��ӁB
	*/
	virtual bool	IsKeyPress(int key) const;
	/// ����L�[�����݉�����Ă��邩

	virtual bool	IsKeyPush(int key) const;
	/// ����L�[���O���GetKeyState�̂Ƃ����牟���ꂽ���j

	virtual const BYTE*	GetKeyData() const;
	/// �擾�����L�[���(BYTE m_byKeyBuffer[256])�ւ̃|�C���^�𓾂�)
	///	���ڃ|�C���^�𓾂Ă��肮�肷��l�̂��߂ɁO�O�G

	IKeyBase();
	virtual ~IKeyBase(){}
protected:
	virtual void	Reset();
	/**
		�o�b�t�@�̒��g���N���A����BJoyStick�̃{�^������ύX�����Ƃ��Ȃ�
		�����ꂽ�܂܂̏�ԂŎc��̂�h�~����
	*/

	// buffer��flip! (C)yaneurao
//	void	FlipKeyBuffer(int& var) { var = 1 - var;}
	void	FlipKeyBuffer(int& var) { var ^= 1;} // ���ʁA�������D�D(;�L�D`)

	int		m_nKeyBufNo;			//	���ƕ\��flip���āA�������Ƃ�̂ɗ��p
	BYTE	m_byKeyBuffer[2][256];	//	key buffer
};

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd

#endif
