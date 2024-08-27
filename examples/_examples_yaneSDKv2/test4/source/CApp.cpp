#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����

	GetDraw()->SetDisplay(false);				//	Window���[�h

	CFPSTimer t;
	t.SetFPS(30);

	CSpriteEx sprite[2];						//	�X�v���C�g
	CSpriteChara spchara;						//	�X�v���C�g��`�t�@�C���̓ǂݍ��݃N���X
	spchara.Load("data/ars.txt");				//	�X�v���C�g��`�t�@�C���̓ǂݍ���
	sprite[0].SetSprite(spchara.GetSprite());	//	�X�v���C�g�̐ݒ�
	sprite[1].SetSprite(spchara.GetSprite());	//	�X�v���C�g�̐ݒ�

	//
	//	����CSpriteChara�̓ǂݍ��ݓ������������������΁ACSpriteLoader���g���B
	//	�܂��ACSpriteEx�́ACGameObjectBase�h���N���X�Ɏ�������Ɨǂ��B
	//
	//


	CTextLayer tl;
	tl.GetFont()->SetText("�X�y�[�X�L�[�������Ǝ��̃p�^�[���ɍs���ɂ�B");
	tl.SetPos(150,440);

	while(IsThreadValid()) {					//	���ꂪValid�̊ԁA�܂�葱����
		GetKey()->Input();
		if (GetKey()->IsVKeyPush(5)) {
			int nDirection = sprite[0].GetDirection();
			nDirection ++;
			if (nDirection==33) nDirection = 0;
			sprite[0].SetDirection(nDirection);
			sprite[1].SetDirection(32-nDirection);
		}
		GetDraw()->Clear();
		GetDraw()->Blt(&sprite[0],200,200);
		GetDraw()->Blt(&sprite[1],300,200);
		GetDraw()->OnDraw();
		t.WaitFrame();
	}
}

//	���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
	virtual void MainThread(void){			//	���ꂪ���[�J�[�X���b�h
		CApp().Start();						//	CApp app; app.Start();�̈Ӗ���
	}
};

//	���킸�ƒm�ꂽWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CAppInitializer::Init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	�K�������Ă�
	CSingleApp sapp;
	if (sapp.IsValid()) {
		CAppMainWindow().Run();					//	��Œ�`�������C���̃E�B���h�D���쐬
	}
	return 0;
}
