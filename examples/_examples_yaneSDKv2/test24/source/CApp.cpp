#include "stdafx.h"
#include "CApp.h"

#include "CScnEffect.h"

void	CApp::MainThread(void) {				 //	 これが実行される
	GetDraw()->SetDisplay(false);

	//	ソフトウェアマウスカーソル
	{
		smart_ptr<CPlaneBase> v(new CDIB32,true);
		v->Load("interface/common/mouse.yga");
		GetMouseLayer()->SetPlane(v,2,52);
	}

	//	これがシナリオを閲覧するための母体となるクラス
	CScenarioDraw scn;

	//	各種 plug-in
	scn.SetTextDraw(smart_ptr<CTextDrawBase>(new CTextDrawDIB32A,true));
	scn.SetBGLoader(smart_ptr<CPlaneLoaderBasePre>(new CDIB32Loader,true));
	scn.SetBGMLoader(smart_ptr<CBGMLoader>(new CBGMLoader,true));
	scn.SetSELoader(smart_ptr<CSELoader>(new CSELoader,true));
	scn.SetSCLoader(smart_ptr<CPlaneLoaderBasePre>(new CDIB32Loader,true));
	scn.SetFaceLoader(smart_ptr<CPlaneLoaderBasePre>(new CDIB32Loader,true));
	scn.SetNameLoader(smart_ptr<CPlaneLoaderBasePre>(new CDIB32Loader,true));
	scn.SetButtonLoader(smart_ptr<CPlaneLoaderBasePre>(new CDIB32Loader,true));
	//	effect factory
	scn.SetScenarioEffectFactory(smart_ptr<CScenarioEffectFactory>(new CMyScenarioEffectFactory,true));

	//	以下の２つは所有権は移さない
	scn.SetKey(GetKey());
	scn.SetMouse(GetMouse());

	//	設定ファイル
	scn.SetConfigFile("interface/talk1/layout_define.txt");

	//	BGMはストリーム再生だい！
	scn.GetBGMLoader()->UseStreamSound(true);

	//	FPSの設定
	CFPSTimer fps;
	fps.SetFPS(30);	//	テキトーにredrawしてなちゃい＾＾；

	//	scn.SetSkipFast(false);
	//	↑これを設定すれば、早送りボタンを無効にできる

	//	テストモード
	const bool bTest = false;
	int nScn = 520;
ReScan:;
	
	{	//	今、再生中のＳＥ等を停止させる
		scn.GetBGMLoader()->Stop();
		scn.GetSELoader()->ReleaseAll();
		scn.ReleaseCacheAll();
		scn.ResetEffect();
	}

	if (!bTest) {
		//	対象ファイル読み込み
		GetMouseLayer()->Enable(false);
		CFileDialog filedlg;
		CHAR buf[MAX_PATH];
		if (filedlg.GetOpenFile(buf)!=0) return ;
		scn.Open(buf);
		GetMouseLayer()->Enable(true);
	} else {
		CHAR buf[MAX_PATH];
		::wsprintf(buf,"scn/scn%.3d.html",nScn);
		CDbg().Out(buf);
		scn.Open(buf);
		nScn++;
	}

	while (IsThreadValid()){

		//	毎フレーム呼び出す処理リスト＾＾
		GetKey()->Input();
		GetMouse()->Flush();
		scn.GetSELoader()->OnPlay();

		if (GetKey()->IsVKeyPress(0)) break;	//	ESCキーで終了するよ
//		GetDraw()->Clear();						//	BGのクリア
		scn.OnDraw(GetDraw()->GetSecondary());	//	シナリオ画面の描画
		{
			HDC hdc = GetDraw()->GetSecondary()->GetDC();
			scn.OnDrawText(hdc);
			GetDraw()->GetSecondary()->ReleaseDC();
		}
		GetDraw()->OnDraw();			//	実画面へ転送
		if (scn.Input()) goto ReScan;	//	キー入力を受け付けてそれに応じて反応する
		//	※　OnDrawのあとで行なうこと
		fps.WaitFrame();				//	フレーム待ち
	}

//	int p = 123;

}

//	これがmain windowのためのクラス。
class CAppMainWindow : public CAppBase {	//	アプリケーションクラスから派生
	virtual void MainThread(void){			//	これがワーカースレッド
		CApp().Start();						//	CApp app; app.Start();の意味ね
	}
	//	キャプション変更しとかんとどやされる＾＾
	virtual LRESULT OnPreCreate(CWindowOption& opt){
		opt.caption		= "蒼き大地";
		opt.classname	= "YANESDKAPPLICATION";
		opt.size_x		= 640;
		opt.size_y		= 480;
		opt.style		= WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION;
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
