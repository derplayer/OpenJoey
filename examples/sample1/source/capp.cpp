#include "stdafx.h"

#include "capp.h"
#include "yaneFlagDebugWindow.h"

int* funcGetFlag(int n) {
	//	まずフラグが512あると思いねぇ！
	static int flag[512] = {
		1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
		12345,23456,34567,45678
	};
	if (n<0 || n>=512) return NULL;	//	範囲外の数字ならばNULL pointerを返す
	return &flag[n]; // そうでなければ、フラグへのポインタをint*として返す
}

string funcGetFlagExp(int n) {
	//	この関数は、フラグナンバーに対する説明を返す

	if (n<0 || n>=512) return "";	//	範囲外の数字ならば "" を返す
	switch(n){
	case 0:
	case 9:
	case 12: return "一日のエッチの回数";
	case 1: return "おっぱいの数";
	case 2: return "あゆみちんの陵辱回数";
	case 3:
	case 14: return "デートの回数";
	case 5: return "ツッコミポイント";
	case 25: return "まいにちのお小遣い";
	default: return "ほげほげ";
	}
}

void	CApp::MainThread() {

		GetDraw()->SetDisplay();

		{	
			///	フラグのデバッガの起動
			
			CFlagDebugWindow* flagdebug = new CFlagDebugWindow;
			
			//	数字ビットマップの設定
			flagdebug->SetDebugBmpFile("debug_nums.bmp");
			
			//	フラグ位置を返す関数の設定
			delegate<int*,int> func;
			func.set(&funcGetFlag);
			flagdebug->SetFlagDelegate(func);

			//	フラグの説明を返す関数の設定
			delegate<string,int> funcExp;
			funcExp.set(&funcGetFlagExp);
			flagdebug->SetFlagExplanationDelegate(funcExp);
			
			CThreadManager::CreateThread(flagdebug);
		}

		CFPSTimer timer;

		//	これをメインプリにする（終了するときに、他のウィンドゥをすべて閉じる）
		SetMainApp(true);

		CKey1 key;

		while (IsThreadValid()){

			GetDraw()->OnDraw();

			key.Input();
			if (key.IsKeyPush(0))
				break;
			timer.WaitFrame();
		}
}

//	これがmain windowのためのクラス。
class CAppMainWindow : public CAppBase {	//	アプリケーションクラスから派生
	virtual void MainThread(){			   //  これがワーカースレッド
		CApp().Start();
	}
};

//	言わずと知れたWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	{
		{
		CTextOutputStreamFile* p = new CTextOutputStreamFile;
		p->SetFileName("Error.txt");
		Err.SelectDevice(smart_ptr<ITextOutputStream>(p));
		}

		CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);

		//	必ず書いてね
		CSingleApp sapp;
		if (sapp.IsValid()) {
			CThreadManager::CreateThread(new CAppMainWindow);
//			CThreadManager::CreateThread(new CAppMainWindow);
			//	上で定義したメインのウィンドゥを作成
		}
		//	ここでCAppInitializerがスコープアウトするのだが、このときに
		//	すべてのスレッドの終了を待つことになる
	}
	return 0;
}
