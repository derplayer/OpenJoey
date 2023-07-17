#include "stdafx.h"
#include "CApp.h"

//	--------------   シーンのつかいかた   -------------------------

//	1.シーン名をenumする
enum SCENE {	//	シーン名
	SCENE1,SCENE2,SCENE3,
	SCENE_ISEND, // 終了しますか？シーン
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
		text.SetPos(50,50);
		text2.GetFont()->SetText("右上の×ボタンでウィンドゥを閉じてみてください");
		text2.SetPos(50,130);
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
	CTextLayer text2;
};
class CScene2 : public CBaseScene {
	virtual void OnInit() {
		dib.Load("data/2.jpg");
		text.GetFont()->SetText("スペースキーでシーン１へジャンプ\nリターンキーでシーン３をコール");
		text.SetPos(50,50);
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
		text.SetPos(50,50);
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

//	YesNoの確認シーン
class CSceneYesNo : public CBaseScene {
public:
	virtual void OnInit();
	virtual void OnDraw(CPlaneBase*lp);

	CPlaneLoaderBasePre* GetPlaneLoader() { return &m_vPlaneLoader;}

//	void	SetExitFlag(bool b) { m_bOverwrite = !b; }

	CSceneYesNo() { /* m_bOverwrite = true;*/ }

private:
	CDIB32Loader m_vPlaneLoader;
	CGUIButton	 m_vButton[2];	//	ボタンが２つ
	int			 m_nButton;		//	ボタン押された情報
	CRootCounter m_nButtonCount;//　フェードカウンタ
	smart_ptr<CPlaneBase> m_vPlane; // ＢＧ
//	bool		m_bOverwrite;	//	上書きしますか？のシーンか？
//	int			m_nType;		//	シーンのタイプ
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
		case SCENE_ISEND: lp = new CSceneYesNo; break;
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
		GetMouse()->Flush();
		if (GetKey()->IsVKeyPush(0)) break;

		//	7.これでシーンコントローラーを調べ、返り値が非０ならループから抜けるようにする
	//	if (sc.OnDraw(GetDraw()->GetSecondary())) break;

		//	WM_CLOSEでせっつかれてんのか？＾＾
		if (m_bWindowClosing) {
			//	終了しますかシーンとちゃうなら、終了しますかシーンに飛ばす
			if (sc.GetSceneNo()!=SCENE_ISEND){
				GetTransiter()->Backup(GetDraw()->GetSecondary(),3);
				sc.CallSceneFast(SCENE_ISEND);
				//	トランジッターは停止させる
				GetTransiter()->StopTransit();
			}
			m_bWindowClosing = false;
		}

		if (GetTransiter()->OnDraw(GetDraw()->GetSecondary())) break;

		GetDraw()->OnDraw();
		t.WaitFrame();
	}
}

LRESULT	CApp::OnPreClose(void){
	m_bWindowClosing = true;
	//	１を返してCloseを阻止
	return 1;	
}

//	これがmain windowのためのクラス。
class CAppMainWindow : public CAppBase {	//	アプリケーションクラスから派生
public:
	CAppMainWindow() {
		m_lpApp = NULL;
	}
protected:
	virtual void MainThread(void){			//	これがワーカースレッド
		m_lpApp = new CApp;		//	m_lpAppは、この間でしか生きていてはマズイ
		m_lpApp->Start();
		DELETE_SAFE(m_lpApp);
	}
	virtual LRESULT OnPreClose(){
		//	WM_CLOSEハンドラ
		if (m_lpApp!=NULL)
			return m_lpApp->OnPreClose();
		return 0;
	}
	CApp*	m_lpApp;
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

/////////////////////////////////////////////////////////
//	選択シーン from 蒼き大地＾＾；
/////////////////////////////////////////////////////////

void CSceneYesNo::OnInit(){

	outer.GetKey()->ResetKey();

	//	トランジッターに記憶されたプレーンのコピー
	m_vPlane = outer.GetTransiter()->GetPlane(3);
	outer.GetTransiter()->ReleasePlane(3); // コピーしたので消して良い

	if (/*!m_bOverwrite*/true) {
		//	終了しますかボックス
		((CDIB32*)(CPlaneBase*)m_vPlane)->SubColorFast(PlaneRGB(172,172,172));
		//	トランジションよりも優先するシーン
		SetPrecedentScene(true);
//		m_nType = 0;
	} else {
//		m_nType = GetSceneParam()[0]+1;
	}

	//	PlaneLoaderに読み込み
	m_vPlaneLoader.Set("data/yesno/list.txt");

	//	ボタンの設定
	{
		for(int i=0;i<2;++i){
			m_vButton[i].SetMouse(outer.GetMouse());
			CGUINormalButtonListener *p = new CGUINormalButtonListener;
			p->SetPlaneLoader(&m_vPlaneLoader,i*3);
			p->SetType(2);	//	反転ありのボタン
			m_vButton[i].SetEvent(smart_ptr<CGUIButtonEventListener>(p,true));
			m_vButton[i].SetXY(216+i*120,240);
		}
	}

	m_nButton = 0;
	m_nButtonCount.Set(0,16);
}

void CSceneYesNo::OnDraw(CPlaneBase*lpPlane){
	//	lpPlane->ClearRect();	//	このシーンはサブシーンなのでＢＧはクリアしない
	lpPlane->BltFast(m_vPlane,0,0);

	//	中央にセンタリングして表示
	{
		int n;
		switch (/*m_nType*/ 0){
		case 0: n = 6; break; // 終了しますか？
		case 1: n = 7; break; // 上書きしますか？Yes/No
		case 2: n = 8; break; // 選ぶんか？
		default : WARNING(true,"パラメータちゃんと渡せ～");
		}
		lpPlane->BltNaturalPos(m_vPlaneLoader.GetPlaneBase(n),320,240,0);
	}

	//	ボタンの表示
	{
		for(int i=0;i<2;++i){
			m_vButton[i].OnDraw(lpPlane);
		}
	}

	//	ボタン押されたんか？
	{
		for(int i=0;i<2;++i){
			if (m_nButton==0 && m_vButton[i].IsLClick()) { // Yes
				m_nButton = i+1;
				CGUIButtonEventListener* e	= m_vButton[i].GetEvent();
				CGUINormalButtonListener* p	= (CGUINormalButtonListener*)e;
				p->SetType(32);
				p->SetImageOffset(2);
			}
		}
	}

	//	ボタン押されてから、ちょっと待ってから消える
	if (m_nButton!=0) {
		m_nButtonCount++;
		if (m_nButtonCount.IsLapAround()){
			//	もし終了しますかダイアログだったならば
			if (/*!m_bOverwrite*/true) {
				if (m_nButton == 1) { // Yes?
					outer.Close();	//	Windowを閉じるように要求
				}
			} else {
				GetSceneParam()[0] = 2-m_nButton;
			}
			outer.GetMouse()->ResetButton();
			//	トランジッターを再開させる
			outer.GetTransiter()->RestartTransit();
			ReturnScene(); return ;
		}
	}
}
