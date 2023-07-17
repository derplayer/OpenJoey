#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 これが実行される

	//	DirectInputは、マルチスレッド非対応なのだ。
	//	だから、このクラスのメンバにCKeyを持たせることはいまのところ不可なのだ。

	GetDraw()->SetDisplay(false,320,240);

	static volatile int nNo=0;
	nNo++;
	CDbg().Out("第"+CStringScanner::NumToString(nNo)+"スレッド生成されたぴょん！");

#ifdef FAST_PLANE_MODE
	CFastPlane dib;
#else
#ifdef PLANE_MODE
	CPlane dib;
#else
	CDIB32 dib;
#endif
#endif
	dib.Load("data/113s.jpg");

	CFPSTimer timer;
	timer.SetFPS(10);
	CFPSLayer l(&timer);
	l.SetPos(0,0);

	int nCount = 0;
	smart_ptr<CAppMainWindow> p[5];
	while(IsThreadValid()) {					//	これがValidの間、まわり続ける
		nCount ++;
		if (nCount==30 && nNo <=4){
			//	子供を産む＾＾；
			CDbg().Out("緑の野菜を食べて子供を産むんだぴょん！");
			
			//	生成したCAppBaseのポインタは、このThreadが責任を持って、この
			//	関数から抜けるまでに解体しなければならない。
			p[nNo].Add(new CAppMainWindow);
			p[nNo]->Run();

			//	上で定義したメインのウィンドゥを作成
		}

		GetDraw()->GetSecondary()->BltFast(&dib,0,0);
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
