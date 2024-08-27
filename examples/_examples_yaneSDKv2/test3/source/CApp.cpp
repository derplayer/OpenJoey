#include "stdafx.h"
#include "CApp.h"
#include "CMonologueView.h"

	//	BGM Collection
SLOAD_CACHE BGM_LOADER_DEFINE[] = {
	"data/BGM31.mid"		,0,	//	0:BGM  opening
	NULL,
};

void	CApp::MainThread(void) {				 //	 これが実行される

	GetDraw()->SetDisplay(false);				//	Windowモード

	m_BGMLoader.Set(BGM_LOADER_DEFINE);			//	BGMのリストを定義
	
	CFPSTimer t;
	t.SetFPS(20);

	CMonologueView mv(this);
	mv.Load("data/opening.txt");
	while(IsThreadValid()) {					//	これがValidの間、まわり続ける
		if (mv.OnDraw()) break;
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
