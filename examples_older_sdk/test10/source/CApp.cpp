#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����

	GetDraw()->SetDisplay(false);				//	Window���[�h

	CFPSTimer t;
	t.SetFPS(30);

	int nDisplayPlane=0;
	CDIB32 plane;
	plane.Load("data/aoi.jpg");

	CTextLayer tl;
	tl.GetFont()->SetText("Cell Automaton is simple,but powerful!!");
	tl.GetFont()->SetSize(20);
	tl.GetFont()->SetColor(RGB(200,100,100));
	
	CRootCounter nPhase(256),nPat(4);
	while(IsThreadValid()) {					//	���ꂪValid�̊ԁA�܂�葱����
		tl.SetPos(30+nPhase,250);

		if (nPhase == 0){	//	reset
			GetDraw()->BltFast(&plane,0,0);
		}
		GetDraw()->OnDraw();
		CDIB32* lpDraw = GetDraw()->GetSecondary();

		switch (nPat){
		case 0 : CCellAutomaton::UpFade   (lpDraw); break;
		case 1 : CCellAutomaton::DownFade (lpDraw); break;
		case 2 : CCellAutomaton::RightFade(lpDraw); break;
		case 3 : CCellAutomaton::LeftFade (lpDraw); break;
		}
		t.WaitFrame();

		nPhase.Inc();
		if (nPhase.IsLapAround()) nPat.Inc();
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
