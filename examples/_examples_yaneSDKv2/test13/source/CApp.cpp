#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 これが実行される

	GetDraw()->SetDisplay(false);				//	Windowモード

	CFPSTimer t;
	t.SetFPS(30);

	CDIB32 bg;
	bg.Load("data/Talk.jpg");

	CFile file;
	file.Read("data/out.html");
	CTextDrawContext context;
	context.SetTextPtr((LPSTR)file.GetMemory());
	context.m_nWidth = 480;
	context.SetBaseFontSize(25);
	context.m_rgbColorBk = RGB(128,128,128);
//	CTextDrawDIB32 textdraw;
	CTextDrawDIB32A textdraw;	//	アンチェリ付きバージョン
	textdraw.SetContext(context);
	textdraw.UpdateText();

	while (IsThreadValid()){
		GetDraw()->Clear();
		GetDraw()->Blt(&bg,0,0);

//		GetDraw()->Blt(textdraw.GetDIB(),128,376);
		GetDraw()->BlendBltFastAlpha(textdraw.GetDIB(),128,376);
		
		t.WaitFrame();

		GetKey()->Input();
		if (GetKey()->IsVKeyPush(5)) {	//	読み進める
			textdraw.GoNextContext();
			textdraw.UpdateText();

			/*
			{	//	解析できなかったタグを表示してみる例
				for(int i=0;i<textdraw.GetTagList()->size();++i){
					CHAR buf[10];
					::CopyMemory(&buf,(void*)((*textdraw.GetTagList())[i]),9);
					buf[9] = '\0';
					CDbg db;
					db.Out("%s",buf);
				}
			}
			*/

		}
		if (GetKey()->IsVKeyPush(6)) {	//	先頭に戻る
			textdraw.SetTextOffset(0);
			textdraw.UpdateText();
		}
		GetDraw()->OnDraw();
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
