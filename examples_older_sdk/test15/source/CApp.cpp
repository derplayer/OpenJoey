#include "stdafx.h"
#include "CApp.h"

class CMyRegionHook : public CRegionHook {
public:
	CMyRegionHook(CDIB32*lp):CRegionHook(lp) {}
protected:
	//	ドラッグ可能領域の設定
	virtual	bool MustBeDrag(const POINT& pt){
		if (pt.y<100) {
			CMsgDlg().Out("触ったらダメにょ！","顔は嫌！もっと下！");
			return false;
		} ef (pt.y<250) {
			CMsgDlg().Out("触ったらダメにょ！","そんなところ触んないでエッチ！もっと下！");
			return false;
		} ef (pt.y>300) {
			CMsgDlg().Out("触ったらダメにょ！","あなた変態！？もっと上！");
			return false;
		} ef (pt.x<130) {
			CMsgDlg().Out("触ったらダメにょ！","馬鹿！もっと右よ！わかんないの？");
			return false;
		} ef (pt.x>150) {
			CMsgDlg().Out("触ったらダメにょ！","馬鹿！もっと左よ、左！");
			return false;
		} else {
			return true;	//	ドラッグ可能
		}
	}
};

void	CApp::MainThread(void) {				 //	 これが実行される
	CDIB32 pp;
	pp.UseDIBSection(true);
	pp.Load("data/aoisora.bmp");
	CMyRegionHook rgn(&pp);	//	このリージョンにするのだ
	
	{	//	セカンダリバッファは、ビットマップサイズだけ用意する
		int sx,sy;
		pp.GetSize(sx,sy);
		GetDraw()->SetDisplay(false,sx,sy);		//	Windowモード
		//	ここで初めてウィンドゥを表示する
	}

	CFPSTimer fps;
	fps.SetFPS(5);	//	テキトーにredrawしてなちゃい＾＾；
	CKey key;
	CRootCounter r(128,255,16);
	r.SetReverse(true);
	r.SetInit(255);
	while (IsThreadValid()){
		key.Input();
		if (key.IsVKeyPress(0)) break;	//	ESCキーで終了するよ
		GetDraw()->Clear(DIB32RGB(255,255,255));
		r.Inc();
		GetDraw()->BlendBltFast(&pp,0,0,DIB32RGB(255-r,255-r,255-r),DIB32RGB(r,r,r));
		GetDraw()->OnDraw();
		fps.WaitFrame();
	}
}

//	これがmain windowのためのクラス。
class CAppMainWindow : public CAppBase {	//	アプリケーションクラスから派生
	virtual void MainThread(void){			//	これがワーカースレッド
		CApp().Start();						//	CApp app; app.Start();の意味ね
	}
	virtual LRESULT OnPreCreate(CWindowOption& opt){
		opt.caption = "蒼井空";
		opt.classname="蒼井空";
		//	最初はウインドゥを出さない
		opt.size_x	= 0;
		opt.size_y	= 0;
		opt.style	= WS_POPUP;
		return 0;
	}
};

//	言わずと知れたWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CAppInitializer::Init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	必ず書いてね
//	CSingleApp sapp;
//	if (sapp.IsValid()) {
		CAppMainWindow().Run();					//	上で定義したメインのウィンドゥを作成
//	}
	return 0;
}
