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
	plane.Load("data/twinlove1.jpg");

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

		"DiagonalDiffusionBlt",CPlaneTransBlt::DiagonalDiffusionBlt,
		"DiffusionCongeriesBlt1",CPlaneTransBlt::DiffusionCongeriesBlt1,
		"DiffusionCongeriesBlt2",CPlaneTransBlt::DiffusionCongeriesBlt2,
		"DiffusionCongeriesBlt3",CPlaneTransBlt::DiffusionCongeriesBlt3,
		"SquashBlt",CPlaneTransBlt::SquashBlt,
		"ForwardRollBlt",CPlaneTransBlt::ForwardRollBlt,
		"RotationBlt1",CPlaneTransBlt::RotationBlt1,
		"RotationBlt2",CPlaneTransBlt::RotationBlt2,
		"RotationBlt3",CPlaneTransBlt::RotationBlt3,
		"RotationBlt4",CPlaneTransBlt::RotationBlt4,
		"EnterUpBlt1",CPlaneTransBlt::EnterUpBlt1,
		"EnterUpBlt2",CPlaneTransBlt::EnterUpBlt2,
		"CellGatherBlt1",CPlaneTransBlt::CellGatherBlt1,
		"CellGatherBlt2",CPlaneTransBlt::CellGatherBlt2,

		"MosaicBlt1",CPlaneTransBlt::MosaicBlt1,
		"FlushBlt1",CPlaneTransBlt::FlushBlt1,
		"SlitCurtainBlt1",CPlaneTransBlt::SlitCurtainBlt1,
		"SlitCurtainBlt2",CPlaneTransBlt::SlitCurtainBlt2,
		"SlitCurtainBlt3",CPlaneTransBlt::SlitCurtainBlt3,
		"SlitCurtainBlt4",CPlaneTransBlt::SlitCurtainBlt4,
		"SlitCurtainBlt5",CPlaneTransBlt::SlitCurtainBlt5,
		"SlitCurtainBlt6",CPlaneTransBlt::SlitCurtainBlt6,
		"SlitCurtainBlt7",CPlaneTransBlt::SlitCurtainBlt7,
		"SlitCurtainBlt8",CPlaneTransBlt::SlitCurtainBlt8,
		"TensileBlt1",CPlaneTransBlt::TensileBlt1,
		"TensileBlt2",CPlaneTransBlt::TensileBlt2,
		"TensileBlt3",CPlaneTransBlt::TensileBlt3,
		"TensileBlt4",CPlaneTransBlt::TensileBlt4,

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
		tf[nPat].FuncName(GetDraw()->GetSecondary(),&plane,215,0,nPhase);
		GetDraw()->OnDraw();
#else
		GetDraw()->GetSecondaryDIB()->Clear();
		tf[nPat].FuncName(GetDraw()->GetSecondaryDIB(),&plane,215,0,nPhase);
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
