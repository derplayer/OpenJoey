#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 これが実行される

	GetDraw()->SetDisplay(false);				//	Windowモード

	CFPSTimer t;
	t.SetFPS(30);

	CDIB32 dib;
	dib.Load("data/sor_11.jpg");

	CRootCounter r(0,300,4);
	while(IsThreadValid()) {					//	これがValidの間、まわり続ける
		GetKey()->Input();
		if (GetKey()->IsVKeyPush(0)) break;

		DWORD dwRGB = DIB32RGB(116,104,106);
		int nLevel = r;
		if (nLevel>256) nLevel = 256;
		CDIB32Effect::Blt(GetDraw()->GetSecondary(),&dib
			,CDIB32Sepia(nLevel,dwRGB),0,0);
		r++;

		GetDraw()->OnDraw();
		t.WaitFrame();
	}
}

//	これがmain windowのためのクラス。
class CAppMainWindow : public CAppBase {	//	アプリケーションクラスから派生
	virtual void MainThread(void){			//	これがワーカースレッド
		CApp().Start();						//	CApp app; app.Start();の意味ね
	}
};

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
