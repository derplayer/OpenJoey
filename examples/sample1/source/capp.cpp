#include "stdafx.h"

#include "capp.h"
#include "yaneFlagDebugWindow.h"

int* funcGetFlag(int n) {
	//	�܂��t���O��512����Ǝv���˂��I
	static int flag[512] = {
		1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
		12345,23456,34567,45678
	};
	if (n<0 || n>=512) return NULL;	//	�͈͊O�̐����Ȃ��NULL pointer��Ԃ�
	return &flag[n]; // �����łȂ���΁A�t���O�ւ̃|�C���^��int*�Ƃ��ĕԂ�
}

string funcGetFlagExp(int n) {
	//	���̊֐��́A�t���O�i���o�[�ɑ΂��������Ԃ�

	if (n<0 || n>=512) return "";	//	�͈͊O�̐����Ȃ�� "" ��Ԃ�
	switch(n){
	case 0:
	case 9:
	case 12: return "����̃G�b�`�̉�";
	case 1: return "�����ς��̐�";
	case 2: return "����݂���̗ːJ��";
	case 3:
	case 14: return "�f�[�g�̉�";
	case 5: return "�c�b�R�~�|�C���g";
	case 25: return "�܂��ɂ��̂�������";
	default: return "�ق��ق�";
	}
}

void	CApp::MainThread() {

		GetDraw()->SetDisplay();

		{	
			///	�t���O�̃f�o�b�K�̋N��
			
			CFlagDebugWindow* flagdebug = new CFlagDebugWindow;
			
			//	�����r�b�g�}�b�v�̐ݒ�
			flagdebug->SetDebugBmpFile("debug_nums.bmp");
			
			//	�t���O�ʒu��Ԃ��֐��̐ݒ�
			delegate<int*,int> func;
			func.set(&funcGetFlag);
			flagdebug->SetFlagDelegate(func);

			//	�t���O�̐�����Ԃ��֐��̐ݒ�
			delegate<string,int> funcExp;
			funcExp.set(&funcGetFlagExp);
			flagdebug->SetFlagExplanationDelegate(funcExp);
			
			CThreadManager::CreateThread(flagdebug);
		}

		CFPSTimer timer;

		//	��������C���v���ɂ���i�I������Ƃ��ɁA���̃E�B���h�D�����ׂĕ���j
		SetMainApp(true);

		CKey1 key;

		while (IsThreadValid()){

			GetDraw()->OnDraw();

			key.Input();
			if (key.IsKeyPush(0))
				break;
			timer.WaitFrame();
		}
}

//	���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
	virtual void MainThread(){			   //  ���ꂪ���[�J�[�X���b�h
		CApp().Start();
	}
};

//	���킸�ƒm�ꂽWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	{
		{
		CTextOutputStreamFile* p = new CTextOutputStreamFile;
		p->SetFileName("Error.txt");
		Err.SelectDevice(smart_ptr<ITextOutputStream>(p));
		}

		CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);

		//	�K�������Ă�
		CSingleApp sapp;
		if (sapp.IsValid()) {
			CThreadManager::CreateThread(new CAppMainWindow);
//			CThreadManager::CreateThread(new CAppMainWindow);
			//	��Œ�`�������C���̃E�B���h�D���쐬
		}
		//	������CAppInitializer���X�R�[�v�A�E�g����̂����A���̂Ƃ���
		//	���ׂẴX���b�h�̏I����҂��ƂɂȂ�
	}
	return 0;
}
