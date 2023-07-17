#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 これが実行される

	GetDraw()->SetDisplay(false);				//	Windowモード

	CFPSTimer t;
	t.SetFPS(30);

	CSpriteEx sprite[2];						//	スプライト
	CSpriteChara spchara;						//	スプライト定義ファイルの読み込みクラス
	spchara.Load("data/ars.txt");				//	スプライト定義ファイルの読み込み
	sprite[0].setMySprite(spchara.getMySprite());	//	スプライトの設定
	sprite[1].setMySprite(spchara.getMySprite());	//	スプライトの設定

	//
	//	このCSpriteCharaの読み込み動作を自動化したければ、
	//	CSpriteLoaderを使う。
	//
	
	CTextLayer tl;
	tl.GetFont()->SetText("スペースキーを押すと次のパターンに行くにょ。\n"
		"Enterキーを押すと描画プライオリティを変更するにょ");
	tl.SetPos(150,440);
	GetDraw()->GetHDCLayerList()->Add(&tl);

	CSurfaceCache surface_cache; // 描画プライオリティを持つSurface
	int nPriorityPhase = 0;

	while(IsThreadValid()) {					//	これがValidの間、まわり続ける
		GetKey()->Input();
		if (GetKey()->IsKeyPush(5)) {
			int nDirection = sprite[0].getDirection();
			nDirection ++;
			if (nDirection==33) nDirection = 0;
			sprite[0].setDirection(nDirection);
			sprite[1].setDirection(32-nDirection);
		}
		if (GetKey()->IsKeyPush(6)) {
			//	enterキーを押すと、プライオリティを変更
			nPriorityPhase = (++nPriorityPhase) & 3;
		}
		GetDraw()->GetSecondary()->Clear();

		//	キャラクタを重ねて描画
//		sprite[0].Blt(GetDraw()->GetSecondary(),280,200);
//		sprite[1].Blt(GetDraw()->GetSecondary(),300,200);

		//	setPriorityで設定された数値が低い順番に描画される
		surface_cache.setPriority((nPriorityPhase+0) & 3);
		sprite[0].Blt(&surface_cache,280,200);
		surface_cache.setPriority((nPriorityPhase+1) & 3);
		sprite[1].Blt(&surface_cache,300,200);
		surface_cache.setPriority((nPriorityPhase+3) & 3);
		sprite[0].Blt(&surface_cache,280,240);
		surface_cache.setPriority((nPriorityPhase+2) & 3);
		sprite[1].Blt(&surface_cache,300,240);
					//	↑いったんここに描画しておく。

		//	描画要求をflushする(このとき描画される)
		surface_cache.flush(GetDraw()->GetSecondary());
	
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
	CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	必ず書いてね
	CSingleApp sapp;
	if (sapp.IsValid()) {
		CThreadManager::CreateThread(new CAppMainWindow());					//	上で定義したメインのウィンドゥを作成
	}
	return 0;
}
