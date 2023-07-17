#include "stdafx.h"
#include "CApp.h"

class CTaskControlBase {
/**
	タスク制御のための基底クラス
*/
public:
	void	setPriority(int nPriority) { nPriority_ = nPriority;}
	int		getPriority() { return nPriority_; }
	/*
		プライオリティ（タスクの優先度）の設定／取得
		プライオリティの設定自体は、class CTaskController の側で行なうので
		この派生クラスが行なう必要はない。
	*/

	virtual void Task(void*) = 0;
	///	呼び出されるべきタスク。これを派生クラスでオーバーライドする
	///	この引数には、CTaskController::callTaskで渡したものがそのまま入ってくる

protected:
	int		nPriority_;
};

class CTaskController {
/**
	タスクのコントローラー
	CTaskControlBase*のlistを持っていて管理する
*/
public:
	typedef list<CTaskControlBase*>	tasklist;
	tasklist* getTaskList() { return& tasklist_; }

	///	すべてのタスクを呼び出す
	void	callTask(void*p){
		tasklist::iterator it = getTaskList()->begin();
		bKillTask_ = false;
		while (it!=getTaskList()->end()){
			(*it)->Task(p);
			if (bKillTask_) {
				//	このタスクは自爆しよった
				bKillTask_ = false;
				delete *it;
				it = getTaskList()->erase(it);
			} else {
				it++;
			}
		}
	}

	/**
		生成したタスクをタスクリストに登録する
		new したCTaskControlBase派生クラスを渡してチョ
		プライオリティは、タスクの優先度。0だと一番優先順位が高い。
		万が一に備えてマイナスの値を渡しても良いようにはなっている。
	*/
	void	addTask(CTaskControlBase*p,int nPriority)
	{
		p->setPriority(nPriority);
		tasklist::iterator it = getTaskList()->begin();
		while (it!=getTaskList()->end()){
			if ((*it)->getPriority() > nPriority) break;
			it++;
		}
		getTaskList()->insert(it,p);
	}

	/**
		優先度を指定して、そのタスクを消滅させる
		(ただし、自分で自分のタスクを削除することは出来ない)
	*/
	void	killTask(int nPriority){
		tasklist::iterator it = getTaskList()->begin();
		while (it!=getTaskList()->end()){
			int nMyPriority = (*it)->getPriority();
			if (nMyPriority > nPriority) break;
			// priorityに関して整順を仮定できるので超えていればそこでおしまい
			if (nMyPriority == nPriority) {
				delete *it;
				it = getTaskList()->erase(it);
			}
		}
	}

	/**
		優先度を指定してタスクを一括削除する
		このメソッドを呼び出すとき、自分のタスクが含まれてはいけない
	*/
	void	killTask(int nStartPriority,int nEndPriority){
		tasklist::iterator it = getTaskList()->begin();
		while (it!=getTaskList()->end()
			&& ((*it)->getPriority()<nStartPriority)) ++it;
		while (it!=getTaskList()->end()
			&& ((*it)->getPriority()<nEndPriority)) {
			delete *it;
			it = getTaskList()->erase(it);
		}
	}

	/**
		callTaskで呼び出されているタスクが
		自分自身を削除するためのメソッド
	*/
	void	killMe() { bKillTask_ = true; }

protected:
	tasklist tasklist_;
	bool	bKillTask_;
};

//	↑↑↑ここまでは汎用的なゲームタスクとゲームタスクコントローラー
//	適当にぶっこ抜いて使ってチョ
/////////////////////////////////////////////////////////////////
//	↓↓↓は、このゲーム専用のゲームタスクコントローラーとinfo

/**
	タスク優先順位としてはタスクごとに異なる値を用意する
*/
namespace taskPriority {
	enum TaskPriority__ { task1,task2,task3 };
}

struct CGameTaskInfo {
	CApp* getApp() { return pApp;}
	void  setApp(CApp*p){ pApp = p; }

	//	描画するコンテキストを取得
	CFastPlane* getDrawContext()
	{ return getApp()->GetDraw()->GetSecondary(); }

	CTaskController* getTaskController() { return pTaskController;}
	void setTaskController(CTaskController*tc) { pTaskController=tc;}

	bool bMove;	//	移動するのか
	bool bDraw;	//	描画するのか

protected:
	CTaskController* pTaskController;
	CApp*		pApp;	//	自分のアプリクラス
};

//	ユーザー定義タスク基底クラス
class CMyTask : public CTaskControlBase {
public:
	virtual void Task(void*p){
		CGameTaskInfo* pInfo = (CGameTaskInfo*)p;
		if (pInfo->bMove) OnMove(pInfo);
		if (pInfo->bDraw) OnDraw(pInfo);
	}
	virtual void OnMove(CGameTaskInfo* p){}
	virtual void OnDraw(CGameTaskInfo* p){}
	/*
		ユーザーはこのクラスを派生させ、
		OnMoveとOnDrawをオーバーライドして使う
	*/

	//	自分自身のタスクを消す(タスクコントローラーの力を借りて)
	virtual void killMe(CGameTaskInfo* p){
		p->getTaskController()->killMe();
	}

	//	ほか、タスクコントローラーへのアクセス等は
	//	このクラスで適当にwrapすると良い
};

struct CMyTask2 : public CMyTask {
	CMyTask2() : n_(0) {}
	virtual void OnMove(CGameTaskInfo* p){
		CDbg().Out("CMyTask2::OnMove");
		n_++;
		if (n_==4) {
			CDbg().Out("時間が来たのでTask2もさよならにゅ(´Д`)");
			killMe(p);
		}
	}
	int n_;
};

//	ユーザー定義タスク基底クラス
struct CMyTask1 : public CMyTask {
	CMyTask1() : n_(0) {}
	virtual void OnMove(CGameTaskInfo* p){
		CDbg().Out("CMyTask1::OnMove");
		n_ ++;
		if (n_==3) {
			CDbg().Out("Task1がTask2を生成するにょ○(≧∇≦)o");
			p->getTaskController()->addTask(new CMyTask2,taskPriority::task2);
		}
		if (n_==5) {
			CDbg().Out("時間が来たのでTask1はさよならにょ(´Д`)");
			killMe(p);
		}
	}
	int n_;
};

void	CApp::MainThread() {				 //	 これが実行される

	CTaskController tc;
	CGameTaskInfo info;
	info.setApp(this);
	info.setTaskController(&tc);
	//	第１タスクを起動
	tc.addTask(new CMyTask1,taskPriority::task1);
	int count=0;
	while (IsThreadValid()){
		if (count<10) { count++; CDbg().Out("--callTask--"); }
		info.bMove = true;
		info.bDraw = false;
		tc.callTask(&info);
		//	これでタスクを呼び出す
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
	CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	必ず書いてね
	CSingleApp sapp;
	if (sapp.IsValid()) {
		CThreadManager::CreateThread(new CAppMainWindow());					//	上で定義したメインのウィンドゥを作成
	}
	return 0;
}
