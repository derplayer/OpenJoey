#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 これが実行される

	GetDraw()->SetDisplay(false);				//	Windowモード
	CFPSTimer t;
	t.SetFPS(30);

	CKey3 key;
	int nNum = CJoyStick::GetDirectInputJoyStickNum(); // ジョイスティックの本数
	if (nNum==0) return ; // JoyStick無いにょ！＾＾；

	auto_array<CJoyStick> aJoy(nNum);
	{
		for ( int i = 0; i < nNum ; i++){
			int j = (int)jsDIRECT_JOYSTICK1+i;
			aJoy[i].SelectDevice( (JoySelector)j );
			aJoy[i].SetButtonMax(32);
		}
	}
	CTextPlane tMode[2];
	tMode[0].GetFont()->SetSize(15);
	tMode[0].SetColorKey(RGB(255,255,255));
	tMode[0].GetFont()->SetText("ON");
	tMode[0].UpdateText();
	tMode[1].GetFont()->SetSize(15);
	tMode[1].SetColorKey(RGB(255,255,255));
	tMode[1].GetFont()->SetText("OFF");
	tMode[1].UpdateText();
	POINT	pos[] = { 
		40,0,
		40,40,
		20,20,
		60,20,
	};

	while(IsThreadValid()) {					//	これがValidの間、まわり続ける
		GetDraw()->Clear();
		key.Input();

		for ( int k = 0; k < nNum ; k++){
		aJoy[k].GetKeyState();
			for ( int i = 0 ; i < 4 ; i++ ){
				int nMode;
				if ( aJoy[k].IsKeyPress(i) )  nMode = 0;else nMode =1;
				GetDraw()->BltFast(&tMode[nMode],pos[i].x,pos[i].y+k*90);
			}
			for ( i = 0 ; i < 32 ; i++ ){
				int nMode;
				if ( aJoy[k].IsKeyPress(i+4) )  nMode = 0;else nMode =1;
				GetDraw()->BltFast(&tMode[nMode],i*30,60+k*90);
			}
		}
		if ( key.IsVKeyPress(0) ) break; 

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
