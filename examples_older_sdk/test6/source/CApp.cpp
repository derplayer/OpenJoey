#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 これが実行される

	GetDraw()->SetDisplay(false);				//	Windowモード
	GetDraw()->CreateSecondaryDIB();

	CFPSTimer t;
	t.SetFPS(30);

	CFPSLayer fps(&t);
	fps.SetPos(0,0);

	const int nMax = 30;

	CDIB32 plane[nMax];
	plane[nMax-1].Load("bg.jpg");
	int i;
	for(i=0;i<nMax-1;i++){
		int sx,sy;
		plane[nMax-1].GetSize(sx,sy);
		plane[i].CreateSurface(sx,sy);
		plane[i].BltFast(&plane[nMax-1],0,0);
		if (i!=nMax-2)	//	最後のワンループは抜ける
			plane[nMax-1].ShadeOff();
	}

	//	CTextDIB32を使う例：
	CTextDIB32 plane2[2];
	LPCTSTR alpsz[] = {
		"毎日がパンの耳にょ\nパンの耳うまいにゅ",
		"今日はパンの耳に砂糖にょ\nパンの耳最高にゅ",
		"昨日はパンの耳にバターにょ\nパンの耳おいしいにゅ",
		"明日は揚げパンの耳に砂糖にょ\nパンの耳お前も食え",
		"明日後日はハチミツ＆パンの耳にょ\nパンの耳いかすにゅ",
		"今年いっぱいはパンの耳で乗り切るにょ\nパンの耳三昧にゅ",
		"ゲマ～",
		NULL,NULL,
	};

	int n = 0 , nv = 1;
	int nt = 0;
	while(IsThreadValid()) {					//	これがValidの間、まわり続ける
		GetKey()->Input();

		GetDraw()->GetSecondaryDIB()->Clear();

		//	WAFFLEロゴ
		GetDraw()->GetSecondaryDIB()->Blt(&plane[n],200,100);

		if (n==0){
			for(i=0;i<2;i++){
				plane2[i].GetFont()->SetSize(30);
				plane2[i].GetFont()->SetQuality(2);
				plane2[i].GetFont()->SetText(alpsz[nt]);
				plane2[i].UpdateText();
			}
			if (alpsz[nt+1]==NULL) nt=0; else nt++;
		}
		
		//	文字パネル
		if (nv>0) {
			GetDraw()->GetSecondaryDIB()->Blt(&plane2[1],100+n,120+n);	//	影を先に描かなきゃ
		}
		GetDraw()->GetSecondaryDIB()->Blt(&plane2[0],100,120);
		plane2[1].ShadeOff();	//	ぼかし
		n+=nv;
		if (n==nMax-1 || n==0) nv = -nv;
		if (n==0) {
			plane2[1].BltFast(&plane2[0],0,0);
		}

		GetDraw()->OnDrawDIB();
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
