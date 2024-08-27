#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����

	GetDraw()->SetDisplay(false);

	CTextFastPlane plane;
	plane.GetFont()->SetText("�T�E���h�Đ��b�g�t�I");
	plane.GetFont()->SetSize(40);
	plane.UpdateTextAA();

	CFPSTimer timer;
	timer.SetFPS(10);
	CFPSLayer l(&timer);
	l.SetPos(0,0);

	CStreamSound bgm;
	bgm.GetReaderFactory()->AddPlugin("plugin/OggVorbisPlugin.dll");
	bgm.Open("flower.ogg");
	bgm.Play();

	while(IsThreadValid()) {					//	���ꂪValid�̊ԁA�܂�葱����
		GetDraw()->GetSecondary()->Clear();
		GetDraw()->GetSecondary()->BltNatural(&plane,100,200);
		GetDraw()->OnDraw();

		timer.WaitFrame();
	}
}

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
