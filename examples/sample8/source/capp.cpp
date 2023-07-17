#include "stdafx.h"
#include "resource.h"
#include "capp.h"
void	CApp::MainThread() {
/**
	スライダーと、ドロップダウンリストボックスは未実装
	ごめんにょ

*/

	CFPSTimer timer;
	timer.SetFPS(10);

	//	これをメインプリにする（終了するときに、他のウィンドゥをすべて閉じる）
	SetMainApp(true);

	CDialogHelper dialog(GetMyApp()->GetMyWindow());
	int nButtonID = dialog.HookButtonOnClick(IDC_BUTTON1);

	string s;
	CDIBitmap dib;
	dib.Load("1s.jpg");

	CDialogHelper::CListBoxInfo listbox[2];
	dialog.Attach(listbox[0],IDC_LIST1,true); // ひとつはリストボックス
	dialog.Attach(listbox[1],IDC_COMBO1,false); // ひとつはコンボボックス

	listbox[0].AddString("こんにち");			// 0
	listbox[0].AddString("おっす！おら悟空！");	// 1
	listbox[0].AddString("シェーーー！！！");		// 2
	listbox[0].AddString("わくちん",1);			// ←1なので、
		//	1の直前、すなわち「おっす！おら悟空」の直前の行に挿入される
//	listbox[0].SetCurSel(2);

	listbox[1].AddString("りすといち");
	listbox[1].AddString("りすとにー");

	while (IsThreadValid()){
		// ---	ダイアログをいじくりまわす

		//	エディットボックスの文字列取得
		string t = dialog.GetText(IDC_EDIT1);
		int r1 = dialog.GetCheck(IDC_RADIO1);
		int r2 = dialog.GetCheck(IDC_RADIO2);
		int r3 = dialog.GetCheck(IDC_CHECK1);
		dialog.SetText(IDC_EDIT2,"ラジオボタンの状態⇒"+CStringScanner::NumToString(r1)
			+":"+CStringScanner::NumToString(r2)
			+"\r\nリストボックスの状態"+CStringScanner::NumToString(listbox[0].GetCurSel())
			+"\r\nコンボボックスの状態"+CStringScanner::NumToString(listbox[1].GetCurSel())
			+"\r\n"+t+s);
		dialog.SetText(IDC_STATIC1,"チェックボタンの状態⇒"+CStringScanner::NumToString(r3));
		if (dialog.GetPoolInfo(nButtonID)->isPool()){
		//	ボタンおされとる！
			dialog.GetPoolInfo(nButtonID)->reset();
			//	押し下げ情報のクリア
			s += "爆発";
		}
		//	ピクチャーコントロールに画像を描画
		{
			HWND h = dialog.GetHWnd(IDC_STATIC2);
			HDC dc = ::GetDC(h);
			::BitBlt(dc,0,0,100,75,dib.GetDC(),0,0,SRCCOPY);
			::ReleaseDC(h,dc);
		}

		timer.WaitFrame();
	}
}

//	これがmain windowのためのクラス。
class CAppMainWindow : public CAppBase {	//	アプリケーションクラスから派生
	virtual void MainThread(){			   //  これがワーカースレッド
		CApp().Start();
	}
	virtual LRESULT OnPreCreate(CWindowOption &opt){
		opt.dialog = MAKEINTRESOURCE(IDD_DIALOG1);	//	ダイアログなのだ！
		return 0;
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
			CThreadManager::CreateThread(new CAppMainWindow);
		}
		//	ここでCAppInitializerがスコープアウトするのだが、このときに
		//	すべてのスレッドの終了を待つことになる
	}
	return 0;
}
