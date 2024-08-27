#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 これが実行される
	GetDraw()->SetDisplay(false);				//	Windowモード

#ifdef _USE_FastDraw
	CFastPlane pp;
	CFastPlane bg;
	CFastPlane light;
	pp.Load("data/Shi-20-01.bmp.yga");

	//	αチャンネル付きサーフェースを強制的に作成
	bg.SetYGAUse(true);
#else
	CDIB32 pp;
	CDIB32 pp2;
	CDIB32 bg;
	CDIB32 light;
	pp.Load("data/Shi-20-01.bmp.yga");
	pp.FadeAlpha(80*255/100);	//	半透明率90%
	{ // 同サイズのサーフェースを作成
		int sx,sy;
		pp.GetSize(sx,sy);
		pp2.CreateSurface(sx,sy);
	}
#endif

	bg.Load("data/bg.jpg");

	light.Load("data/light.jpg");

	CTextLayer tl;
	tl.GetFont()->SetText("立ちキャラの輪郭はαブレンドにより\nジャギが抑えられている（しかも同時に半透明！）\n"
		"そしてα値付き画像のリアルタイムフェード。\n"
		"ＢＧは、ユニバーサルトランジションによる\n"
		"レンズフレアエフェクト。");
	tl.GetFont()->SetSize(25);
	tl.SetPos(40,340);

	int x=0,y=103,nPhase=0;
	int nLight = 255;

	//	for fastplane debugging..
//	nPhase=1;

	CRootCounter r(0,255,8);
	r.SetInit(255);		//	初期値の設定
	r.SetReverse(true);	//	オートリバースカウンタ
	
	while (IsThreadValid()){
		switch (nPhase){
		case 0: {
			GetDraw()->GetSecondary()->Clear();
			bg.BltToAlpha(&light,nLight,nLight+128,0,255);	//	light効果
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&bg,0,0);
			nLight-=10; if (nLight<-128) nLight = 255;

			GetDraw()->GetSecondary()->BlendBltFastAlpha(&pp,x,y);
			r.Inc();
#ifdef _USE_FastDraw
			//	FastPlaneには、αプレーンに対するFadeしながらの転送がネイティブにサポートされている
			GetDraw()->GetSecondary()->FadeAlphaBlt(&pp,500-x,y,r);
#else
			pp2.FadeBltAlpha(&pp,r);
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&pp2,500-x,y);
#endif
			x++; if (x>200) { x=0; nPhase++; nLight=-128; r.Reset(); }
			break;
				}
		case 1: {
			GetDraw()->GetSecondary()->Clear();
			bg.BltToAlpha(&light,nLight,nLight+128,0,255);	//	light効果
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&bg,0,0);
			nLight+=10; if (nLight>255) nLight = -128;

			int sx,sy;
			pp.GetSize(sx,sy);
			SIZE s = { sx*2, sy*2 };
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&pp,x,y,NULL,&s);
			r.Inc();
#ifdef _USE_FastDraw
			GetDraw()->GetSecondary()->FadeAlphaBlt(&pp,400-x,y,r,NULL,&s);
#else
			pp2.FadeBltAlpha(&pp,r);
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&pp2,400-x,y,NULL,&s);
#endif
			x+=2; if (x>200) { x=-100; y=-100; nPhase++; nLight=255; r.Reset(); }
			break;
				}
		case 2: {
			GetDraw()->GetSecondary()->Clear();
			bg.BltToAlpha(&light,nLight,nLight+128,255,0);	//	light効果
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&bg,0,0);
			nLight-=10; if (nLight<-128) nLight = 255;

			int sx,sy;
			pp.GetSize(sx,sy);
			SIZE s = { sx*4, sy*4 };
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&pp,x,y,NULL,&s);
			r.Inc();
#ifdef _USE_FastDraw
			GetDraw()->GetSecondary()->FadeAlphaBlt(&pp,200-x,y,r,NULL,&s);
#else
			pp2.FadeBltAlpha(&pp,r);
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&pp2,200-x,y,NULL,&s);
#endif
			x+=2; if (x>100) { x=-300; y=-100; nPhase++; nLight=-128; r.Reset(); }
			break;
				}
		case 3: {
			GetDraw()->GetSecondary()->Clear();
			bg.BltToAlpha(&light,nLight,nLight+128,255,0);	//	light効果
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&bg,0,0);
			nLight+=10; if (nLight>255) nLight = -128;

			int sx,sy;
			pp.GetSize(sx,sy);
			SIZE s = { sx*8, sy*8 };
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&pp,x,y,NULL,&s);
			r.Inc();
#ifdef _USE_FastDraw
			GetDraw()->GetSecondary()->FadeAlphaBlt(&pp,-100-x,y,r,NULL,&s);
#else
			pp2.FadeBltAlpha(&pp,r);
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&pp2,-100-x,y,NULL,&s);
#endif
			x+=2; if (x>0) { x=0; y=103; nPhase=0; nLight=255; r.Reset(); }
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
