#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 これが実行される

	GetDraw()->SetDisplay(false);				//	Windowモード

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
	while(IsThreadValid()) {					//	これがValidの間、まわり続ける
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
