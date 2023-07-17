#include "stdafx.h"
#include "CApp.h"

class CMyTextDraw {
public:
	CTextDrawBase* GetTextDraw() { return& m_TextDraw; }
	CDIB32* GetDIB() { return m_TextDraw.GetDIB(); }
	int	GetPat() { return m_nPat; } // 描画パターンを保持する

	CMyTextDraw(void) { Init(); }
	virtual ~CMyTextDraw() {}

	void Init() {
		m_File.Read("data/out.html");
		m_TextContext.SetTextPtr((LPSTR)m_File.GetMemory());
		m_TextContext.m_nWidth = 260;
		m_TextContext.m_nHInterval = 0;
		m_TextContext.SetBaseFontSize(25);
		m_TextContext.m_rgbColorBk = RGB(128,128,128);
		m_TextDraw.SetContext(m_TextContext);
		m_TextDraw.UpdateText();
		m_nPat = -1;
		CheckTag();	//	未知のタグチェック
	}

	void Update(){
		m_TextDraw.GoNextContext();
		m_TextDraw.UpdateText();
		CheckTag();	//	未知のタグチェック
	}

	void CheckTag(){
		//	CTextDrawのサポートしていないタグを拾い、
		//	その処理を行なう

		//	ここでは、実際に、アニメパターンを指定する<PAT n>タグ、
		//	ジャンプを実現する<Goto label名>タグ、
		//　ジャンプ先のラベルを定義する<JumpLabel label名>タグの
		//	読み込みを実装してみる。

		vector<LPSTR>::iterator it = (*m_TextDraw.GetTagList()).begin();
		while (it!=(*m_TextDraw.GetTagList()).end()){
			LPSTR lpsz = *it;
			//	<PAT n>タグか？
			if (CStringScanner::IsToken(lpsz,"PAT")){
				CStringScanner::GetNum(lpsz,m_nPat); // m_nPatに後続の数字を読み込む
			}ef(CStringScanner::IsToken(lpsz,"GOTO")){
			// <GOTO label>タグをサポートするには、こうやる
				{
					string label;
					label = CStringScanner::GetStr(lpsz,'>');	//	閉じタグまでのラベルを取得
					m_TextDraw.SetTextOffset(0);	//	テキスト先頭から、そのラベルを探す
					while (true) {
						if (m_TextDraw.UpdateText(false)!=0) break;	//	カラ読みして、jumplabelタグを探す
						vector<LPSTR>::iterator it = (*m_TextDraw.GetTagList()).begin();
						while (it!=(*m_TextDraw.GetTagList()).end()){
							LPSTR lpsz = *it;
							if (CStringScanner::IsToken(lpsz,"JUMPLABEL")){
								string label2;
								label2 = CStringScanner::GetStr(lpsz,'>');	//	閉じタグまでのラベルを取得
								if (label == label2) {
									//	一致したので、ここでリターンする
									*m_TextDraw.GetNextContext() = *m_TextDraw.GetContext();
									return ;
								}
							}
							it ++;
						}
						m_TextDraw.GoNextContext();
					}
					return ; // 終了
				}
			} else {
			//	それ以外のタグは無視
			}
			it++;
		}
	}

protected:
	CFile m_File;
	CTextDrawContext m_TextContext;
	CTextDrawDIB32A m_TextDraw;	//	アンチェリ付きバージョン
	int	m_nPat;		//	キャラクター描画パターンを保持する
};

void	CApp::MainThread(void) {				 //	 これが実行される
	GetDraw()->SetDisplay(false);

	CDIB32 dib[4];
	dib[0].Load("data/Mie_00.yga"); // 0:通常
	dib[1].Load("data/Mie_01.yga"); // 1:しょんぼり
	dib[2].Load("data/Mie_02.yga"); // 2:ぷんこ
	dib[3].Load("data/Mie_03.yga"); // 3:えへへ

//	CDIB32 bg;
//	bg.Load("data/logo.jpg");

	CMyTextDraw textdraw;

	CFPSTimer fps;
	fps.SetFPS(30);	//	テキトーにredrawしてなちゃい＾＾；
	CKey key;

	CRootCounter rct(0,255,4);
	rct.SetReverse(true);

	while (IsThreadValid()){
		key.Input();
		if (key.IsVKeyPress(0)) break;	//	ESCキーで終了するよ
		GetKey()->Input();
		if (GetKey()->IsVKeyPush(5)) {	//	スペースキーで読み進める
			textdraw.Update();
		}
		RECT rc;
		::SetRect(&rc,0,0,280,480);
		GetDraw()->Clear(RGB(255,255-rct,rct),&rc);
		rct.Inc();
		::SetRect(&rc,280,0,640,480);
		GetDraw()->Clear(RGB(0,0,0),&rc);
//		GetDraw()->BltFast(&bg,0,0);
		if (textdraw.GetPat()!=-1) {
			GetDraw()->BlendBltFastAlpha(&dib[textdraw.GetPat()],0,0);
		}
		{
			int sx,sy;
			textdraw.GetDIB()->GetSize(sx,sy);
			GetDraw()->BlendBltFastAlpha(textdraw.GetDIB(),340,(480-sy)/2);
		}
		GetDraw()->OnDraw();
		fps.WaitFrame();
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
