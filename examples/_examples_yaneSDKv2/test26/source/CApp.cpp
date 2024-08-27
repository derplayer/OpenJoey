#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 これが実行される

	GetDraw()->SetDisplay(false);

	CFastPlane plane;
	plane.Load("data/113b.jpg");

	//	plane.Load("data/skip.yga");
	//	↑ここでは行なっていないが、yga画像を読み込めば、
	//	自動的に内部でα付きサーフェースが生成される。
	//	しかもα付きサーフェースに対しても、α付き加色合成、α付き減色合成、
	//	α付きブレンド等をサポートしている。

	CFastPlane bg;
	bg.Load("data/276.jpg");

	CFPSTimer timer;
	timer.SetFPS(120);
	CFPSLayer l(&timer);
	l.SetPos(0,0);

	CTextFastPlane text;
	text.GetFont()->SetSize(40);

	CTextFastPlane text2;
	text2.GetFont()->SetSize(20);
	text2.GetFont()->SetColor(RGB(128,50,60));
	text2.GetFont()->SetText("画面モードを8bpp,15/16bpp,24bpp,32bppでも試してみてね！");
	text2.UpdateTextA();
	//	↑UpdateTextAを使えば、内部的に自動的にα付きサーフェースが生成される

	int nCount = 0;

	while(IsThreadValid()) {					//	これがValidの間、まわり続ける

		int nCountL = nCount & 0xff;
		int nCountH = nCount >> 8;
		if (nCountL==0){
			switch (nCountH) {
			case 0: text.GetFont()->SetText("定数倍飽和加算"); break;
			case 1: text.GetFont()->SetText("定数倍飽和減算"); break;
			case 2: text.GetFont()->SetText("半透明"); break;
			case 3: text.GetFont()->SetText("画面全体に対する飽和加算"); break;
			case 4: text.GetFont()->SetText("画面全体に対する飽和減算"); break;
			}
			//	文字も、アンチェリ付きで表示するのだ！
			text.UpdateTextA();
		}

		GetDraw()->GetSecondary()->BltFast(&bg,0,0);

			//	RGB555,565のときは、αに対して、4bitテーブルで掛け算するため、
			//	上位4bitのみしか有効では無い．．

		switch (nCountH) {
		case 0:
			GetDraw()->GetSecondary()->AddColorAlphaBlt(&plane,60+nCountL,0,nCountL);
			break;
		case 1:
			GetDraw()->GetSecondary()->SubColorAlphaBlt(&plane,60+nCountL,0,nCountL);
			break;
		case 2:
			GetDraw()->GetSecondary()->BlendBltFast(&plane,60+nCountL,0,nCountL);
			break;
		case 3:
			GetDraw()->GetSecondary()->AddColorFast(RGB(nCountL,nCountL,nCountL));
			break;
		case 4:
			GetDraw()->GetSecondary()->SubColorFast(RGB(nCountL,nCountL,nCountL));
			break;
		}

		GetDraw()->GetSecondary()->BltNaturalPos(&text,320,420,0);
		GetDraw()->GetSecondary()->BltNaturalPos(&text2,320,450,0);

		GetDraw()->OnDraw();

		nCount++; if (nCount == 256*5) nCount = 0;

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
