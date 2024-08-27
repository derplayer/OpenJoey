#include "stdafx.h"
#include "CApp.h"

//	--------------   シーンのつかいかた   -------------------------

//	1.シーン名をenumする
enum SCENE {	//	シーン名
	SCENE1,SCENE2,SCENE3
};

//	2.ベースシーンを作る (CSceneとmediatorから多重継承する)
class CBaseScene : public CScene , public mediator<CApp> {
};	//	これで完成＾＾

//	3.シーンを作る ( ベースシーンから派生させる )
class CScene1 : public CBaseScene {
	//	初期化は、OnInitをオーバーロードして行なう。このなかでouterも使える。
	virtual void OnInit() {
		dib.Load("data/1.jpg");
		text.GetFont()->SetText("スペースキーでシーン２へジャンプ\nリターンキーでシーン１をスタックに積んでシーン３へジャンプ");
		text.SetPos(50,200);
		outer.GetKey()->ResetKey();
		//	↑キー入力はリセットしないと、キーを押されて飛んできたのなら二重入力になる
	}
	virtual void OnDraw(CPlaneBase*lp){
		//	※　outer識別子で、CAppクラスにアクセスできる！
		if (outer.GetKey()->IsVKeyPush(5)) {
			outer.GetTransiter()->Backup(lp);
			outer.GetTransiter()->BeginTransit(19);
			JumpScene(SCENE2);
			return ;
		}
		if (outer.GetKey()->IsVKeyPush(6)) {
			outer.GetTransiter()->Backup(lp);
			outer.GetTransiter()->BeginTransit(25);
			PushScene(SCENE1);	//	PushScene + JumpScene == CallScene
			JumpScene(SCENE3);
		}

		lp->ClearRect();
		lp->BltFast(&dib,0,0);
	}	
	CDIB32 dib;
	CTextLayer text;
};
class CScene2 : public CBaseScene {
	virtual void OnInit() {
		dib.Load("data/2.jpg");
		text.GetFont()->SetText("スペースキーでシーン１へジャンプ\nリターンキーでシーン３をコール");
		text.SetPos(50,200);
		outer.GetKey()->ResetKey();
		//	↑キー入力はリセットしないと、キーを押されて飛んできたのなら二重入力になる
	}
	virtual void OnDraw(CPlaneBase*lp){
		//	※　outer識別子で、CAppクラスにアクセスできる！
		if (outer.GetKey()->IsVKeyPush(5)) {
			outer.GetTransiter()->Backup(lp);
			outer.GetTransiter()->BeginTransit(21);
			JumpScene(SCENE1);
			return ;
		}
		if (outer.GetKey()->IsVKeyPush(6)) {
			outer.GetTransiter()->Backup(lp);
			outer.GetTransiter()->BeginTransit(22);
			CallScene(SCENE3);
			return ;
		}

		lp->ClearRect();
		lp->BltFast(&dib,0,0);
	}	
	CDIB32 dib;
	CTextLayer text;
};
class CScene3 : public CBaseScene {
	virtual void OnInit() {
		dib.Load("data/3.jpg");
		text.GetFont()->SetText("スペースキーでリターン\nリターンキーで終了");
		text.SetPos(50,200);
		outer.GetKey()->ResetKey();
		//	↑キー入力はリセットしないと、キーを押されて飛んできたのなら二重入力になる
	}
	virtual void OnDraw(CPlaneBase*lp){
		if (outer.GetKey()->IsVKeyPush(5)) {
			outer.GetTransiter()->Backup(lp);
			outer.GetTransiter()->BeginTransit(23);
			ReturnScene();
			return ;
		}
		if (outer.GetKey()->IsVKeyPush(6)) {
			ExitScene();
			return ;
		}

		lp->ClearRect();
		lp->BltFast(&dib,0,0);
	}	
	CDIB32 dib;
	CTextLayer text;
};

//	4.上のに対応するfactoryをつくる
class CMySceneFactory : public CSceneFactory,public mediator<CApp> {
public:
	CMySceneFactory(CApp* pv) : mediator<CApp>(pv){}
protected:
	virtual CScene* CreateScene(int nScene) {
		CBaseScene* lp;
		switch ((SCENE)nScene){
		case SCENE1: lp = new CScene1; break;
		case SCENE2: lp = new CScene2; break;
		case SCENE3: lp = new CScene3; break;
		default:	 lp = NULL;	// error..
		}
		lp->SetOutClass(&outer);
		return lp;
	}
};
/////////////////////////////////////////////////

void	CApp::MainThread(void) {				 //	 これが実行される

	GetDraw()->SetDisplay(false);				//	Windowモード

	CFPSTimer t;
	t.SetFPS(30);

	//	5.シーンコントローラーにSceneFactoryを渡してやる
	CSceneControl sc(smart_ptr<CSceneFactory>(new CMySceneFactory(this),true));
	sc.JumpScene(SCENE1);

	//	6.トランジッタを使うならば、トランジッタにCSceneControlを渡してやる
	GetTransiter()->SetSceneControl(&sc);

	while(IsThreadValid()) {					//	これがValidの間、まわり続ける
		GetKey()->Input();
		if (GetKey()->IsVKeyPush(0)) break;

		//	7.これでシーンコントローラーを調べ、返り値が非０ならループから抜けるようにする
	//	if (sc.OnDraw(GetDraw()->GetSecondary())) break;
		if (GetTransiter()->OnDraw(GetDraw()->GetSecondary())) break;

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
