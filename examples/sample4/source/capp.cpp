#include "stdafx.h"

class CApp : public CThread {
  virtual void ThreadProc(){

	CFPSTimer timer;
	timer.SetFPS(10);

	CKey1 key(true);	//	バックグラウンド動作

	while (IsThreadValid()){

		key.Input();
		if (key.IsKeyPush(0))
			break;
		//	スペースキー押した？
		if (key.IsKeyPush(5)){
			//	画面サイズを取得
			int sx = ::GetSystemMetrics(SM_CXSCREEN);
			int sy = ::GetSystemMetrics(SM_CYSCREEN);
			CDIBitmap bmp;
			bmp.CreateSurface(sx,sy,24);

			HDC hdc1 = bmp.GetDC();
			if (hdc1!=NULL){
				HDC hdc2 = ::GetDC(NULL); // デスクトップのDCを取得
				if (hdc2!=NULL){
					::BitBlt(hdc1,0,0,sx,sy,hdc2,0,0,SRCCOPY);
					::ReleaseDC(NULL,hdc2);
				}
			}

			/*	//	通常のセカンダリサーフェースから転送する場合
			bmp.GetSurfaceInfo()->GeneralBlt(CSurfaceInfo::eSurfaceBltFast,
				GetDraw()->GetSecondary()->GetSurfaceInfo(),&CSurfaceInfo::CBltInfo(),0);
			*/
			bmp.Save("画面.bmp");
		}
		timer.WaitFrame();
	}
  }
};

//	言わずと知れたWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	{
		/*
		{	//	エラーログをファイルに出力するのら！
			CTextOutputStreamFile* p = new CTextOutputStreamFile;
			p->SetFileName("Error.txt");
			Err.SelectDevice(smart_ptr<ITextOutputStream>(p));
		}
		*/

		CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
		//	↑必ず書いてね

		CSingleApp sapp;
		if (sapp.IsValid()) {
			CThreadManager::CreateThread(new CApp);
		}
		//	ここでCAppInitializerがスコープアウトするのだが、このときに
		//	すべてのスレッドの終了を待つことになる
	}
	return 0;
}
