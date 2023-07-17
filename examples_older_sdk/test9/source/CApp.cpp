#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 これが実行される

	GetDraw()->SetDisplay(false);				//	Windowモード

	CFPSTimer t;
	t.SetFPS(30);

	int nDisplayPlane=0;
	CDIB32 plane[3];

	//	関数
	struct transfunc {
		LPCSTR funcname;
		LRESULT (*FuncName)(CPlaneBase*,CPlaneBase*,int,int,int,int nTransMode=0,BYTE byFadeRate=255,LPRECT lpDstClipRect=NULL);
	};

	transfunc tf[] = {

//		"MirrorBlt1",CPlaneTransBlt::MirrorBlt1,
//		"MirrorBlt2",CPlaneTransBlt::MirrorBlt2,
//		"MirrorBlt3",CPlaneTransBlt::MirrorBlt3,
//		"MirrorBlt4",CPlaneTransBlt::MirrorBlt4,

		"WaveBlt1",CPlaneTransBlt::WaveBlt1,
		"WaveBlt2",CPlaneTransBlt::WaveBlt2,
		"WaveBlt3",CPlaneTransBlt::WaveBlt3,
		"WaveBlt4",CPlaneTransBlt::WaveBlt4,

		"CircleBlt1",CPlaneTransBlt::CircleBlt1,
		"CircleBlt2",CPlaneTransBlt::CircleBlt2,
		"CircleBlt3",CPlaneTransBlt::CircleBlt3,
		"CircleBlt4",CPlaneTransBlt::CircleBlt4,
		"CircleBlt5",CPlaneTransBlt::CircleBlt5,

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

//		"MosaicBlt1",CPlaneTransBlt::MosaicBlt1,
//		"FlushBlt1",CPlaneTransBlt::FlushBlt1,

		"DiagonalDiffusionBlt",CPlaneTransBlt::DiagonalDiffusionBlt,
		"DiffusionCongeriesBlt1",CPlaneTransBlt::DiffusionCongeriesBlt1,
		"DiffusionCongeriesBlt2",CPlaneTransBlt::DiffusionCongeriesBlt2,
		"DiffusionCongeriesBlt3",CPlaneTransBlt::DiffusionCongeriesBlt3,
		"SquashBlt",CPlaneTransBlt::SquashBlt,
		"ForwardRollBlt",CPlaneTransBlt::ForwardRollBlt,
		"RotationBlt1",CPlaneTransBlt::RotationBlt1,
		"RotationBlt2",CPlaneTransBlt::RotationBlt2,
		"RotationBlt3",CPlaneTransBlt::RotationBlt3,
//		"RotationBlt4",CPlaneTransBlt::RotationBlt4,
		"EnterUpBlt1",CPlaneTransBlt::EnterUpBlt1,
		"EnterUpBlt2",CPlaneTransBlt::EnterUpBlt2,
		"CellGatherBlt1",CPlaneTransBlt::CellGatherBlt1,
		"CellGatherBlt2",CPlaneTransBlt::CellGatherBlt2,

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

	CDir dir;
//	dir.SetFindFile("*.bmp");
	dir.SetFindFile("*.*");
	dir.SetPath("data");

	{	//	次のファイルを獲得する
		string filename;
		while (dir.FindFile(filename)) ;	//	最後まで行ったらまた最初から＾＾；
		plane[nDisplayPlane].Load(filename);
	}

	int	nV = 16;
	int nPhase = -nV;
	int nPat = 0;		//	パターンナンバー
	while(IsThreadValid()) {					//	これがValidの間、まわり続ける
		nPhase+=nV;
		GetKey()->Input();
		if (GetKey()->IsVKeyPush(6)) break;

		CDIB32* lpDIB =	GetDraw()->GetSecondary();
		if (plane[1-nDisplayPlane].IsLoad()){
			lpDIB->BltFast(&plane[1-nDisplayPlane],0,0);
		}
		tf[nPat].FuncName(lpDIB,&plane[nDisplayPlane],0,0,nPhase);
		GetDraw()->OnDraw();

		t.WaitFrame();
		if (nPhase == 256) {
			::Sleep(200);
			nPat++; nPhase=0;
			if (tf[nPat].funcname==NULL) nPat = 0;
			nDisplayPlane = 1-nDisplayPlane;
			
			{	//	次のファイルを獲得する
				string filename;
				while (dir.FindFile(filename)) ;	//	最後まで行ったらまた最初から＾＾；
				plane[nDisplayPlane].Load(filename);
			}

		}
	}
}

//	これがmain windowのためのクラス。
class CAppMainWindow : public CAppBase {	//	アプリケーションクラスから派生
	virtual void MainThread(void){			//	これがワーカースレッド
		CApp().Start();						//	CApp app; app.Start();の意味ね
	}
	virtual LRESULT OnPreCreate(CWindowOption& opt){
		opt.caption		= "画像 Viewer";
		opt.classname	= "YANEAPPLICATION";
		opt.size_x		= 640;
		opt.size_y		= 480;
		opt.style		= WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
		return 0;
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
