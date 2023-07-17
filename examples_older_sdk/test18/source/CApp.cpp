#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 これが実行される

	GetDraw()->SetDisplay(false);				//	Windowモード

	CFPSTimer t;
	t.SetFPS(30);

//	CFPSLayer fps(&t);
//	fps.SetPos(0,0);

#ifdef FAST_PLANE_MODE
	CFastPlane bg;
	bg.SetYGAUse(true);	//	強制でαサーフェースを作成
	CFastPlane yga;
#else
	CDIB32 bg;
	CDIB32 yga;
#endif
	bg.Load("data/grd.jpg");
	yga.Load("data/title.yga");

	CTextLayer tl;
	tl.GetFont()->SetText("stencil alpha with gradation layer");
	tl.GetFont()->SetSize(30);
	tl.SetPos(60,440);

	bool bAlpha = true;	//  stencil alpha or silhouette alpha
	bool bBG	= true;	//	grd.jog or CG_4_003.jpg

	CRootCounter r(0,500,4);
	r.SetReverse(true);

	while(IsThreadValid()) {					//	これがValidの間、まわり続ける
		GetKey()->Input();
		if (GetKey()->IsVKeyPush(0)) break;

#ifdef FAST_PLANE_MODE
		GetDraw()->GetSecondary()->SetFillColor(RGB(r/2,0,(511-r)/2));
		GetDraw()->GetSecondary()->Clear();
#else
		GetDraw()->GetSecondary()->Clear(DIB32RGB(r/2,0,(511-r)/2));
#endif

		//	こいつで、α値を温存したまま画像を転送する
		yga.BltFastWithoutAlpha(&bg,-r,-50);

		GetDraw()->GetSecondary()->BlendBltFastAlpha(&yga,100,60);
		r.Inc();
		if (r.IsLapAroundI()) {
			yga.FlushAlpha();
			if (bAlpha) {
				if (bBG) {
					tl.GetFont()->SetText("silhouette alpha with gradation layer");
				} else {
					tl.GetFont()->SetText("silhouette alpha with jpeg layer");
				}
			} else {
				if (bBG) {
					tl.GetFont()->SetText("stencil alpha with jpeg layer");
					bg.Load("data/CG_4_003.jpg");
				} else {
					tl.GetFont()->SetText("stencil alpha with gradation layer");
					bg.Load("data/grd.jpg");
				}
				bBG = !bBG;
			}
			bAlpha = !bAlpha;
		}
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
