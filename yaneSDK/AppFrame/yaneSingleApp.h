#ifndef __yaneSingleApp_h__
#define __yaneSingleApp_h__

#include "../Thread/yaneMutex.h"

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

class CSingleApp {
/**
	��d�N���h�~�Ɏg���܂��B

	bool IsValid();
	//�@���Ɏ����Ɠ����A�v�����N�����Ă��Ȃ��������H

	��d�N�����Ă����false�ɂȂ�̂ŁA���̏ꍇ�́A
	�����ɂ��̃v���O�������I�������Ă��������B
	�i���̏ꍇ�A������Ƀt�H�[�J�X���ڂ�܂��j
	���̏����ɂ� class CFindWindow �𗘗p���Ă��܂��B
	class CFindWindow �́A���ݎ��s����Ă��郂�W���[������
	�������W���[�����̃v���Z�X��T���āA���̃E�B���h�D��
	�A�N�e�B�u�ɂ���N���X�ł��B

	CFindWindow�ł̃��W���[�����̔�r�́A�t���p�X�ł͂Ȃ��A���s�t�@�C�����݂̂�
	�s�Ȃ��邽�߁A���t�H���_�œ����̎��s�t�@�C�����̂��̂��N�����Ă���
	�ꍇ�́A���ꂪActive�ɂȂ邱�Ƃ�����܂�

���@�g�p��

// ���킸�ƒm�ꂽWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
�@�@CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
�@�@CSingleApp sapp;
�@�@if (sapp.IsValid()) {
		//	�Q�[���̏���
�@�@}
�@�@return 0;
}

*/
public:
	bool	IsValid(const string& strMutexName = "");
	///�@���Ɏ����Ɠ����A�v�����N�����Ă��Ȃ��������H
	/**
		��d�N���̔���́A�u����̃t�H���_�̓��ꖼ�̎��s�t�@�C���Ȃ��
		����Ƃ݂Ȃ��v�Ƃ��������ɂȂ��Ă��܂��B�����A���ꂪ�܂�����΁A
		���̃A�v���P�[�V�����ŗL��Mutex�����w�肷�邱�Ƃ�
		����āA�r�����邱�Ƃ��o���܂��B
	*/

protected:
	CMutex	m_oMutex;
	bool	m_bValid;
};

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

#endif
