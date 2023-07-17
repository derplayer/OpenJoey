#include "stdafx.h"
#include "IRegex.h"
#include "CFSjisConverter.h"
#include "CFEucConverter.h"
#include "CFJisConverter.h"
#include "jcode_pl.hpp"

class IPlugInTest {
public:
	virtual int Calc(int n)=0;
	virtual ~IPlugInTest(){}
};

class CApp : public CThread {
  virtual void ThreadProc(){
		CDbg().Out("start");

		CDbg().Out(CObjectCreater::GetObj()->LoadPlugIn("boost_regex.dll"));
		IRegex* p = (IRegex*)CObjectCreater::GetObj()->Create("boost::regex");
		if (p!=NULL){
			{//Searchのテスト
				p->Search("Say HELLO to Blackjack", "[A-Z]+", "i");
				CDbg().Out(p->GetResults()->str());
			}
			{//Replaceのテスト
				string s="Say HELLO to Blackjack";
				p->Replace(s, "[A-Z]+", "\\0/", "ig");
				CDbg().Out(s);
			}
			{//JSearchのテスト
				p->JSearch(CFSjisConverter(), "Ｓay HELLO ｔｏ Blackｊａｃｋ", "[A-ZＡ-Ｚａ-ｚ]+", "i");
				CDbg().Out(p->GetResults()->str());
			}
			{//JReplaceのテスト
				string s="Ｓay HELLO ｔｏ Blackｊａｃｋ";
				p->JReplace(CFSjisConverter(), s, "[A-ZＡ-Ｚａ-ｚ]+", "\\0/", "ig");
				CDbg().Out(s);
			}
			delete p;
		}
		else{
			CDbg().Out("error");
		}
		//	使い終わったら、解放
		CObjectCreater::GetObj()->ReleasePlugIn("boost_regex.dll");
		CDbg().Out("end");
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
