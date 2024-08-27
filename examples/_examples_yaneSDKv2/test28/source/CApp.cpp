#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 これが実行される

	GetDraw()->SetDisplay(false);

	CTextFastPlane plane;
	plane.GetFont()->SetText("サウンド再生ＣＨＵ！");
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

	while(IsThreadValid()) {					//	これがValidの間、まわり続ける
		GetDraw()->GetSecondary()->Clear();
		GetDraw()->GetSecondary()->BltNatural(&plane,100,200);
		GetDraw()->OnDraw();

		timer.WaitFrame();
	}
}

//	言わずと知れたWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CAppInitializer::Init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	必ず書いてね
	CSingleApp sapp;
	if (sapp.IsValid()) {
		CAppMainWindow().Run();					//	上で定義したメインのウィンドゥを作成
	}
	return 0;
}
