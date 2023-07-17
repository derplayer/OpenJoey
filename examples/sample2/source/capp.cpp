#include "stdafx.h"
#include "capp.h"

void	CApp::MainThread() {
	GetDraw()->SetDisplay();

	CKey1 key;
	CFPSTimer timer;
	timer.SetFPS(15);

	//	これをメインプリにする（終了するときに、他のウィンドゥをすべて閉じる）
	SetMainApp(true);

	CPlane bgplane;
	bgplane->Load("AA076_640480.jpg");

	CPlane charaplane;
	charaplane->Load("理緒菜640_480.yga");

	CRootCounter nFade(0, 255, 8);
	CRootCounter nPhase(0, 6, 1);

	CTextFastPlane* pText = new CTextFastPlane;
	pText->GetFont()->SetText("スペースキーを押すと次のフェーズに");
	pText->GetFont()->SetSize(30);
	pText->UpdateTextAA();
	CPlane text(pText);

	while (IsThreadValid()){
		ISurface* pSecondary = GetDraw()->GetSecondary();

		pSecondary->Clear();
		//	必ずBGを画面全体に対して転送するならばクリアはしなくとも良い

		//	フェーズごとに異なる描画
		switch (nPhase){
		case 0: {
			pSecondary->BltFast(bgplane,0,0);
			//	抜き色無効転送はBltFastを用いる
			pSecondary->BltNatural(charaplane,0,0);
			//	抜き色有効転送とyga画像(α情報つきの画像)の転送は
			//	BltNaturalを用いるのがわかりやすい
			break;
				}
		case 1: {
			pSecondary->BltNatural(charaplane,0,0);
			// キャラを先に表示させて、
			pSecondary->BlendBltFast(bgplane,0,0,255-nFade);
			//	そのあとにBGを減衰させて描画！
			nFade++;
			break;
				}
		case 2: {
			pSecondary->BltFast(bgplane,0,0);
			//	抜き色無効転送はBltFastを用いる
			int sx,sy;
			charaplane->GetSize(sx,sy);	//	サーフェースサイズの取得
			//	画面中心をベースポイントとして、転送してみる
			SIZE dstsize = { sx*nFade>>8 , sy * nFade>>8 };
			pSecondary->BltNatural(charaplane,sx/2,sy/2,nFade/2+128,&dstsize,NULL,NULL,4);
			//	この場合、最後のパラメータで指定している4というのは、画像中心を起点として
			//	座標を指定するもの
			nFade++;
			break;
				}
		case 3: {
			pSecondary->BltFast(bgplane,0,0);
			int sx,sy;
			charaplane->GetSize(sx,sy);	//	サーフェースサイズの取得
			SIZE dstsize = { sx*nFade>>8 , sy * nFade>>8 };
			pSecondary->AddColorBltFast(charaplane,sx/2,sy/2,&dstsize,NULL,NULL,4);
			//	↑αサーフェースからのAddColor(加色合成)
			nFade++;
			break;
				}
		case 4: {
			pSecondary->BltFast(bgplane,0,0);
			int sx,sy;
			charaplane->GetSize(sx,sy);	//	サーフェースサイズの取得
			SIZE dstsize = { sx*nFade>>8 , sy * nFade>>8 };
			pSecondary->SubColorBltFast(charaplane,sx/2,sy/2,&dstsize,NULL,NULL,4);
			//	↑αサーフェースからのSubColor(加色合成)
			nFade++;
			break;
				}
		case 5: {
			pSecondary->BltFast(bgplane,0,0);
			int sx,sy;
			charaplane->GetSize(sx,sy);	//	サーフェースサイズの取得
			SIZE dstsize = { sx*nFade>>8 , sy * nFade>>8 };
			pSecondary->AddColorBltFastFade(charaplane,sx/2,sy/2,nFade,&dstsize,NULL,NULL,4);
			//	↑αサーフェースからのAddColor(加色合成)+Fade(減衰指定)
			nFade++;
			break;
				}
		case 6: {
			pSecondary->BltFast(bgplane,0,0);
			int sx,sy;
			charaplane->GetSize(sx,sy);	//	サーフェースサイズの取得
			SIZE dstsize = { sx*nFade>>8 , sy * nFade>>8 };
			pSecondary->SubColorBltFastFade(charaplane,sx/2,sy/2,nFade,&dstsize,NULL,NULL,4);
			//	↑αサーフェースからのSubColor(加色合成)+Fade(減衰指定)
			nFade++;
			break;
				}
		}

		pSecondary->BltNatural(text,20,400);

		GetDraw()->OnDraw();

		key.Input();
		if (key.IsKeyPush(0))	//	ESCキーで終了
			break;
		if (key.IsKeyPush(5)) {	//	SPACEキーを押すとフェーズがインクリメント
			nPhase++;
			nFade.Reset();
		}

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
		//*
		{	//	エラーログをファイルに出力するのら！
			CTextOutputStreamFile* p = new CTextOutputStreamFile;
			p->SetFileName("Error.txt");
			Err.SelectDevice(smart_ptr<ITextOutputStream>(p));
		}
		//*/

		CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
		//	↑必ず書いてね

		CSingleApp sapp;
		if (sapp.IsValid()) {
			CThreadManager::CreateThread(new CAppMainWindow);
			//	上で定義したメインのウィンドゥを作成
//			CThreadManager::CreateThread(new CAppMainWindow);
//			↑複数書くと、複数ウィンドゥが生成されるのだ

		}
		//	ここでCAppInitializerがスコープアウトするのだが、このときに
		//	すべてのスレッドの終了を待つことになる
	}
	return 0;
}
