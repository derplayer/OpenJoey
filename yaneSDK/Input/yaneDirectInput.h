// yaneDirectInput.h
//	 This is DirectInput wrapper.
//		programmed by yaneurao(M.Isozaki) '99/06/18
//		modified by yaneurao '00/02/29

#ifndef __yaneDirectInput_h__
#define __yaneDirectInput_h__


#include "../Auxiliary/yaneCOMBase.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

class CDirectInput {
/**
	DirectInput�̏������̂��߂̃N���X
	class CKeyInput , class CJoyStick �ɂĎg�p
*/
public:

	LPDIRECTINPUT Get() const { return m_lpDirectInput; }
	/**
		�R���X�g���N�^�Ŏ����I��DirectInput�̏����������݂�̂ŁA
		���̊֐��ŁAIDirectInput���擾����΂���ł����B
	*/

	int	GetStatus() const { return m_nStatus; }
	/**
		DirectInput�̏������󋵂ɂ��āA���U���g��Ԃ�
		0:ok(DirectX5�ȍ~) 1:ok(DirectX3�ȍ~)
		2:ok(DirectX3 on NT4.0)
		3,4,5:failure(DirectX3��������ĂȂ�)
	*/

	CDirectInput();
	virtual ~CDirectInput(){}

private:
	CCOMObject<LPDIRECTINPUT>	m_vDirectInput;
	CCOMObject<LPDIRECTINPUT>* GetDirectInput() { return& m_vDirectInput;}
	CLoadLibraryWrapper m_vLoadLibraryWrapper;
	CLoadLibraryWrapper* GetLib() { return& m_vLoadLibraryWrapper; }

	LPDIRECTINPUT m_lpDirectInput;
	int		m_nStatus;	//	���̕ϐ��̈Ӗ��ɂ��Ă�GetStatus���Q�Ƃ̂���
};

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd

#endif
