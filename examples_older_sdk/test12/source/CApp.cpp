#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����
	GetDraw()->SetDisplay(false);				//	Window���[�h

	CDIB32 pp;
	pp.Load("data/Shi-20-01.bmp.yga");

	CDIB32 bg;
	bg.Load("data/bg.jpg");

	CTextLayer tl;
	tl.GetFont()->SetText("�����L�����̗֊s�ɂ�����\n���u�����h�ɂ��W���M���}�����Ă��邱�Ƃɒ���");
	tl.GetFont()->SetSize(25);
	tl.SetPos(0,0);

	int x=0,y=103+26,nPhase=0;
	while (IsThreadValid()){
		GetDraw()->BltFast(&bg,0,0);
		switch (nPhase){
		case 0: {
			GetDraw()->BlendBltFastAlpha(&pp,x,y);
			x++; if (x>200) { x=0; nPhase++; }
			break;
				}
		case 1: {
			int sx,sy;
			pp.GetSize(sx,sy);
			SIZE s = { sx*2, sy*2 };
			GetDraw()->BlendBltFastAlpha(&pp,x,y,NULL,&s);
			x++; if (x>200) { x=-100; y=-100; nPhase++; }
			break;
				}
		case 2: {
			int sx,sy;
			pp.GetSize(sx,sy);
			SIZE s = { sx*4, sy*4 };
			GetDraw()->BlendBltFastAlpha(&pp,x,y,NULL,&s);
			x++; if (x>100) { x=-300; y=-100; nPhase++; }
			break;
				}
		case 3: {
			int sx,sy;
			pp.GetSize(sx,sy);
			SIZE s = { sx*8, sy*8 };
			GetDraw()->BlendBltFastAlpha(&pp,x,y,NULL,&s);
			x++; if (x>0) { x=0; y=103; nPhase=0; }
			break;
				}
		}
		GetDraw()->OnDraw();
		GetKey()->Input();
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
