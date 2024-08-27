#include "stdafx.h"
#include "CApp.h"

//	このdefineが有効だと、CDIB32で実装される
//	このdefineが無効だと、CPlaneで実装される
#define DIB_BASE

void	CApp::MainThread(void) {				 //	 これが実行される

	GetDraw()->SetDisplay(false);				//	Windowモード

	CFPSTimer t;
	t.SetFPS(30);

	CFPSLayer fps(&t);
	fps.SetPos(0,0);

#ifndef DIB_BASE
	CPlane plane;
#else
	GetDraw()->CreateSecondaryDIB();
	CDIB32 plane;
#endif
	plane.Load("data/poster.jpg");

	CTextLayer tl;
	tl.GetFont()->SetText("スペースキーを押すと次のパターンに行くにょ。リターンで前のに戻る");
	tl.SetPos(0,440);

	CTextLayer tl2;
	tl2.SetPos(200,460);

	//	関数
	struct transfunc {
		LPCSTR funcname;
		LRESULT (*FuncName)(CPlaneBase*,CPlaneBase*,int,int,int,int nTransMode=0,BYTE byFadeRate=255,LPRECT lpDstClipRect=NULL);
	};
	
	transfunc tf[] = {

		"MirrorBlt1",CPlaneTransBlt::MirrorBlt1,
		"MirrorBlt2",CPlaneTransBlt::MirrorBlt2,
		"MirrorBlt3",CPlaneTransBlt::MirrorBlt3,
		"MirrorBlt4",CPlaneTransBlt::MirrorBlt4,

		"CutInBlt1",CPlaneTransBlt::CutInBlt1,
		"CutInBlt2",CPlaneTransBlt::CutInBlt2,
		"CutInBlt3",CPlaneTransBlt::CutInBlt3,
		"CutInBlt4",CPlaneTransBlt::CutInBlt4,
		"CutInBlt5",CPlaneTransBlt::CutInBlt5,
		"CutInBlt6",CPlaneTransBlt::CutInBlt6,
		"CutInBlt7",CPlaneTransBlt::CutInBlt7,
		"CutInBlt8",CPlaneTransBlt::CutInBlt8,
		"CutInBlt9",CPlaneTransBlt::CutInBlt9,
		"CutInBlt10",CPlaneTransBlt::CutInBlt10,
		"CutInBlt11",CPlaneTransBlt::CutInBlt11,
		"CutInBlt12",CPlaneTransBlt::CutInBlt12,
		"CutInBlt13",CPlaneTransBlt::CutInBlt13,
		"CutInBlt14",CPlaneTransBlt::CutInBlt14,
		"CutInBlt15",CPlaneTransBlt::CutInBlt15,
		"CutInBlt16",CPlaneTransBlt::CutInBlt16,
		"CutInBlt17",CPlaneTransBlt::CutInBlt17,
		"CutInBlt18",CPlaneTransBlt::CutInBlt18,
		"CutInBlt19",CPlaneTransBlt::CutInBlt19,

		"WaveBlt1",CPlaneTransBlt::WaveBlt1,
		"WaveBlt2",CPlaneTransBlt::WaveBlt2,
		"WaveBlt3",CPlaneTransBlt::WaveBlt3,
		"WaveBlt4",CPlaneTransBlt::WaveBlt4,

		"CircleBlt1",CPlaneTransBlt::CircleBlt1,
		"CircleBlt2",CPlaneTransBlt::CircleBlt2,
		"CircleBlt3",CPlaneTransBlt::CircleBlt3,
		"CircleBlt4",CPlaneTransBlt::CircleBlt4,
		"CircleBlt5",CPlaneTransBlt::CircleBlt5,

		"RectBlt1",CPlaneTransBlt::RectBlt1,
		"RectBlt2",CPlaneTransBlt::RectBlt2,
		"RectBlt3",CPlaneTransBlt::RectBlt3,

		"BlindBlt1",CPlaneTransBlt::BlindBlt1,
		"BlindBlt2",CPlaneTransBlt::BlindBlt2,
		"BlindBlt3",CPlaneTransBlt::BlindBlt3,
		"BlindBlt4",CPlaneTransBlt::BlindBlt4,
		"BlindBlt5",CPlaneTransBlt::BlindBlt5,
		"BlindBlt6",CPlaneTransBlt::BlindBlt6,
		"BlindBlt7",CPlaneTransBlt::BlindBlt7,
		"BlindBlt8",CPlaneTransBlt::BlindBlt8,
		"BlindBlt9",CPlaneTransBlt::BlindBlt9,
		"BlindBlt10",CPlaneTransBlt::BlindBlt10,

		"WhorlBlt1",CPlaneTransBlt::WhorlBlt1,
		"WhorlBlt2",CPlaneTransBlt::WhorlBlt2,
		"WhorlBlt3",CPlaneTransBlt::WhorlBlt3,
		"WhorlBlt4",CPlaneTransBlt::WhorlBlt4,
		"WhorlBlt5",CPlaneTransBlt::WhorlBlt5,
		"WhorlBlt6",CPlaneTransBlt::WhorlBlt6,
		"WhorlBlt7",CPlaneTransBlt::WhorlBlt7,
		"WhorlBlt8",CPlaneTransBlt::WhorlBlt8,
		
		"BlendBlt1",CPlaneTransBlt::BlendBlt1,

		NULL,NULL,
	};

	int nPhase = 16;
	int	nV = 16;
	int nPat = 0;		//	パターンナンバー
	while(IsThreadValid()) {					//	これがValidの間、まわり続ける
		GetKey()->Input();
		if (GetKey()->IsVKeyPush(5)) {
			nPat++;
			if (tf[nPat].FuncName==NULL) nPat = 0;
			nPhase = 16; nV = 16;
		}
		if (GetKey()->IsVKeyPush(6)) {
			if (nPat) nPat--;
			nPhase = 16; nV = 16;
		}
		CHAR buf[256];
		::wsprintf(buf,"FuncName = %s , nPhase = %d",tf[nPat].funcname,nPhase);
		tl2.GetFont()->SetText(buf);

		//	ここではCPlaneに対してCPlaneTransBltの関数（トランジション関数）を
		//	呼び出しているが、CDIB32に対しても同じ関数が呼び出せる。
#ifndef DIB_BASE
		GetDraw()->Clear();
		tf[nPat].FuncName(GetDraw()->GetSecondary(),&plane,20,40,nPhase);
		GetDraw()->OnDraw();
#else
		GetDraw()->GetSecondaryDIB()->Clear();
		tf[nPat].FuncName(GetDraw()->GetSecondaryDIB(),&plane,20,40,nPhase);
		GetDraw()->OnDrawDIB();
#endif
		t.WaitFrame();
		if ((nPhase == 256) || (nPhase == 0)) ::Sleep(200);
		nPhase+=nV;
		if ((nPhase == 256) || (nPhase == 0)) nV = -nV;
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
