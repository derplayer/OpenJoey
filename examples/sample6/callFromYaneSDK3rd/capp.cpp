#include "stdafx.h"

//	１．DLLからexportするクラスのインターフェースを読み込む
//	(これは、別ファイルにヘッダとして存在すれば良い)
class IPlugInTest {
public:
	virtual int Calc(int n)=0;
	virtual ~IPlugInTest(){}
};

class CApp : public CThread {
  virtual void ThreadProc(){

		CObjectCreater::GetObj()->LoadPlugIn("sample.dll");
		IPlugInTest* p = (IPlugInTest*)CObjectCreater::GetObj()->Create("CPlugInTest");
		if (p!=NULL){
			int total = p->Calc(100);
			// 1から100までの合計を計算する(DLL側のクラスを呼び出す)
			delete p;
			CDbg().Out(total);
		}
		//	使い終わったら、解放
		CObjectCreater::GetObj()->ReleasePlugIn("sample.dll");
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
