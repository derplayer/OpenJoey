#include "stdafx.h"
#include "CApp.h"


void	CApp::MainThread(void) {				 //	 これが実行される

	GetDraw()->SetDisplay(false);				//	Windowモード

	CFPSTimer t;
	t.SetFPS(30);

	CTextLayer text;
	text.GetFont()->SetBackColor(CLR_INVALID);
	text.GetFont()->SetText("左と中央のボタンはWindowsのボタンと同じような動作をします\n"
		"右のボタンはカーソルを重ねるとアクティブになるタイプのボタンです");
	text.SetPos(50,300);

	CDIB32Loader loader;
	loader.Set("data/botton_cfg.txt");	//	ボタンコンフィグ

	CGUIButton bt[3];
	{
		for(int i=0;i<3;++i){
			bt[i].SetMouse(GetMouse());
			CGUINormalButtonListener *p = new CGUINormalButtonListener;
			switch (i) {
			case 0:
				p->SetPlaneLoader(&loader,0);
				break;
			case 1:
				p->SetPlaneLoader(&loader,2);
				p->SetType(2);	//	反転ありのボタン
				break;
			case 2:
				p->SetPlaneLoader(&loader,0);
				p->SetType(9);	//	上に置くとActiveになるボタン
				break;
			}
			bt[i].SetEvent(smart_ptr<CGUIButtonEventListener>(p,true));
			bt[i].SetXY(100+i*150,200);
		}
	}

	while(IsThreadValid()) {					//	これがValidの間、まわり続ける
		GetKey()->Input();
		if (GetKey()->IsVKeyPush(0)) break;
		GetMouse()->Flush();					//	毎フレームこれでマウス情報ゲット
		{
			//	ボタンの描画
			for(int i=0;i<3;++i){
				bt[i].OnDraw(GetDraw());
			}
		}
		//	ボタンイベントのハンドリング例１
		{
			CGUIButtonEventListener* e = bt[0].GetEvent();
			if (((CGUINormalButtonListener*)e)->IsLClick()){
				//	ボタン０が押されたならば、ボタン１のマークを反転
				e = (CGUIButtonEventListener*)bt[1].GetEvent();
				CGUINormalButtonListener* p = (CGUINormalButtonListener*)e;
				p->SetReverse(true);
			}
		}
		//	ボタンイベントのハンドリング例２
		{
			if (bt[2].IsLClick()){	//	リスナを経由せずにボタンの状態をチェックできる！
				//	ボタン０が押されたならば、ボタン１のマークを正常に戻す
				CGUIButtonEventListener* e = (CGUIButtonEventListener*)bt[1].GetEvent();
				CGUINormalButtonListener* p = (CGUINormalButtonListener*)e;
				p->SetReverse(false);
			}
		}

		//	マウス入力の必要があるのならば、ここで行なえば、
		//	上のボタンイベントで吸収されたマウスメッセージはここに届かない。

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
