#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 これが実行される
	GetDraw()->SetDisplay(false);				//	Windowモード

	CDIB32 pp;
	pp.Load("data/Shi-20-01.bmp.yga");

	CDIB32 bg;
	bg.Load("data/bg.jpg");

	CTextLayer tl;
	tl.GetFont()->SetText("立ちキャラの輪郭において\nαブレンドによりジャギが抑えられていることに注目");
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
