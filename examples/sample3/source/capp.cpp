#include "stdafx.h"

class CApp : public CThread {
public:
	void ThreadProc(){
		//　レジストリ設定～取得
		CRegistry reg("Software\\yaneu");

		string str;
		str = "てすてす";
		reg.SetValue("folder1","moji",str);
		str.erase(); // ←消してから↓をして正しく代入されているかをテスト
		if (reg.GetValue("folder1","moji",str)==0){
			CDbg().Out(str);
		}

		DWORD dw1 = 89383; // ←やくざ屋さん？
		reg.SetValue("folder1","dword",dw1);
		dw1 = 0;
		if (reg.GetValue("folder1","dword",dw1)==0){
			CDbg().Out(dw1);
		}

		//	ユーザー定義データの書き出し
		DWORD dw[3];
		dw[0] = 123;
		dw[1] = 234;
		dw[2] = 345;
		int nSize =3*sizeof(DWORD);
		reg.SetValue("folder1","dataex",(BYTE*)&dw,nSize);
		dw[0] = 0;
		dw[1] = 0;
		dw[2] = 0;
//		nSize = 4;	//	←実際のデータサイズと異なるとこの要求↓はエラーになる
		if (reg.GetValue("folder1","dataex",(BYTE*)&dw,nSize)==0){
			int n1 = dw[0];
			int n2 = dw[1];
			int n3 = dw[2];
			CDbg().Out("data = %d,%d,%d  size = %d",n1,n2,n3,nSize);
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
