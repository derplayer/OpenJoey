#include "stdafx.h"
#include "CApp.h"
#include "CScnConvert.h"

void	CApp::MainThread(void) {				 //	 これが実行される
	GetDraw()->SetDisplay(false);				//	Windowモード

	CFPSTimer t;
	t.SetFPS(10);

	int c0 = 0 , c1 = 0;
	int c = (((~c0 & c1) << 1) + ((~c0 ^ c1) & 0x7bde));
	//c = (((~c0 & c1) << 1) + ((~c0 ^ c1) & 0x7bde));

	SetMainApp(true);

	CDir dir;
	dir.SetPath("org");
	dir.SetFindFile("*.txt");

	CTextFastPlane text(GetDraw());
	text.GetFont()->SetText("原稿コンバータ　version 1.01");
	text.GetFont()->SetSize(20);
	text.UpdateTextAA();

	CDbg().Out("変換処理を開始します");

	{
		CFile file;
		file.Delete("html/covertlog.txt");
	}

	while (IsThreadValid()){

		GetDraw()->GetSecondary()->BltNatural(&text,0,0);
		GetDraw()->OnDraw();

		string src_filename;
		if (dir.FindFile(src_filename)!=0) break;
		CDbg().Out(src_filename + "を変換中..");

		CScnConvert scn;
		if (scn.Convert(src_filename,"html")==0){
			CDbg().Out(src_filename + "の変換終了");
		} else {
			CDbg().Out(src_filename + "の変換失敗");
		}
		t.WaitFrame();
	}

	CDbg().Out("すべての変換処理が終わりました");
	CDbg().Out("ウィンドゥの右上の×印で終了させてください");
	
	//	ウィンドゥの右上の×印で閉じてやー
	while (IsThreadValid()) {
		GetDraw()->GetSecondary()->BltNatural(&text,0,0);
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
