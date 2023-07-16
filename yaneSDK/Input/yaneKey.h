//	yaneKey.h :
//		Integrated Key Input using CVirtualKey(sample)
//
//		programmed by yaneurao '00/02/29

#ifndef __yaneKey_h__
#define __yaneKey_h__


#include "yaneVirtualKey.h"		//	���z�L�[�Ǘ�
#include "yaneKeyInput.h"		//	�L�[����
#include "yaneJoyStick.h"		//	JoyStick����

//////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Input {

class CKey1 : public CVirtualKey {
/**

  class CKey1,class CKey2,class CKey3,class CKey4 �́A
  �L�[�f�o�C�X�o�^�ς݁A���z�L�[�ݒ�ς݂̃N���X�ł��B

	�j�����a���������@�{�@�i�����r������������
	������́A�㉺���E�{�Q�{�^���̃Q�[���p
	�W���C�X�e�B�b�N�͂P���ڑ�����Ă��Ȃ��Ƃ��͎����I�ɂQ���g���悤�ɂȂ�܂�

�O�F�d�r�b�L�[
�P�F�e���L�[�W�C���L�[�C�W���C�X�e�B�b�N��
�Q�F�e���L�[�Q�C���L�[�C�W���C�X�e�B�b�N��
�R�F�e���L�[�S�C���L�[�C�W���C�X�e�B�b�N��
�S�F�e���L�[�Q�C���L�[�C�W���C�X�e�B�b�N��
�T�F�X�y�[�X�L�[�C�W���C�X�e�B�b�N�@�{�^���P
�U�F�e���L�[Enter,���^�[���L�[�C���V�t�g�C�E�V�t�g�B�W���C�X�e�B�b�N�@�{�^���Q
	
*/
public:
	CKey1(bool bBackGround=false);
	/**
		bBackGround == true�Ȃ�΁A�E�B���h�D�Ƀt�H�[�J�X��
		�����Ƃ����͂��s�Ȃ�
	*/
private:
	CKeyInput		m_Key;
	CJoyStick		m_Joy;
};

class CKey2 : public CVirtualKey {
/**
  class CKey1,class CKey2,class CKey3,class CKey4 �́A
  �L�[�f�o�C�X�o�^�ς݁A���z�L�[�ݒ�ς݂̃N���X�ł��B

	�j�����a���������@�{�@�i�����r������������
	������́A�㉺���E�{�U�{�^���̃Q�[���p
	�W���C�X�e�B�b�N�͂P���ڑ�����Ă��Ȃ��Ƃ��͎����I�ɂQ���g���悤�ɂȂ�܂�

�O�F�d�r�b�L�[
�P�F�e���L�[�W�C���L�[�C�W���C�X�e�B�b�N��
�Q�F�e���L�[�Q�C���L�[�C�W���C�X�e�B�b�N��
�R�F�e���L�[�S�C���L�[�C�W���C�X�e�B�b�N��
�S�F�e���L�[�Q�C���L�[�C�W���C�X�e�B�b�N��
�T�F�X�y�[�X�L�[�C�y�L�[�C�W���C�X�e�B�b�N�@�{�^���P
�U�F�e���L�[Enter,���^�[���L�[�C�w�L�[�C�W���C�X�e�B�b�N�@�{�^���Q
�V�F�b�L�[�C�W���C�X�e�B�b�N�@�{�^���R
�W�F�`�L�[�C�W���C�X�e�B�b�N�@�{�^���S
�X�F�r�L�[�C�W���C�X�e�B�b�N�@�{�^���T
�P�O�F�c�L�[�C�W���C�X�e�B�b�N�@�{�^���U

*/
public:
	CKey2(bool bBackGround=false);
	/**
		bBackGround == true�Ȃ�΁A�E�B���h�D�Ƀt�H�[�J�X��
		�����Ƃ����͂��s�Ȃ�
	*/
private:
	CKeyInput		m_Key;
	CJoyStick		m_Joy;
};

//////////////////////////////////////////////////////

class CKey3 : public CVirtualKey {
/**
  class CKey1,class CKey2,class CKey3,class CKey4 �́A
  �L�[�f�o�C�X�o�^�ς݁A���z�L�[�ݒ�ς݂̃N���X�ł��B

	�j�����a���������̂�
	������́A�㉺���E�{�Q�{�^���̃Q�[���p

  ���̃N���X�́Aclass CKey1 ����W���C�X�e�B�b�N��
  ���蓖�Ă����������̂ł��B
*/
public:
	CKey3(bool bBackGround=false);
	/**
		bBackGround == true�Ȃ�΁A�E�B���h�D�Ƀt�H�[�J�X��
		�����Ƃ����͂��s�Ȃ�
	*/
private:
	CKeyInput	m_Key;
};

class CKey4 : public CVirtualKey {
/**
  class CKey1,class CKey2,class CKey3,class CKey4 �́A
  �L�[�f�o�C�X�o�^�ς݁A���z�L�[�ݒ�ς݂̃N���X�ł��B

	�j�����a���������̂�
	������́A�㉺���E�{�U�{�^���̃Q�[���p

  ���̃N���X�́Aclass CKey2 ����W���C�X�e�B�b�N��
  ���蓖�Ă����������̂ł��B
*/
public:
	CKey4(bool bBackGround=false);
	/**
		bBackGround == true�Ȃ�΁A�E�B���h�D�Ƀt�H�[�J�X��
		�����Ƃ����͂��s�Ȃ�
	*/
private:
	CKeyInput	m_Key;
};

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd

#endif
