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
			{//Search�̃e�X�g
				p->Search("Say HELLO to Blackjack", "[A-Z]+", "i");
				CDbg().Out(p->GetResults()->str());
			}
			{//Replace�̃e�X�g
				string s="Say HELLO to Blackjack";
				p->Replace(s, "[A-Z]+", "\\0/", "ig");
				CDbg().Out(s);
			}
			{//JSearch�̃e�X�g
				p->JSearch(CFSjisConverter(), "�ray HELLO ���� Black��������", "[A-Z�`-�y��-��]+", "i");
				CDbg().Out(p->GetResults()->str());
			}
			{//JReplace�̃e�X�g
				string s="�ray HELLO ���� Black��������";
				p->JReplace(CFSjisConverter(), s, "[A-Z�`-�y��-��]+", "\\0/", "ig");
				CDbg().Out(s);
			}
			delete p;
		}
		else{
			CDbg().Out("error");
		}
		//	�g���I�������A���
		CObjectCreater::GetObj()->ReleasePlugIn("boost_regex.dll");
		CDbg().Out("end");
	}
};

//	���킸�ƒm�ꂽWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	{
		/*
		{	//	�G���[���O���t�@�C���ɏo�͂���̂�I
			CTextOutputStreamFile* p = new CTextOutputStreamFile;
			p->SetFileName("Error.txt");
			Err.SelectDevice(smart_ptr<ITextOutputStream>(p));
		}
		*/

		CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
		//	���K�������Ă�

		CSingleApp sapp;
		if (sapp.IsValid()) {
			CThreadManager::CreateThread(new CApp);
		}
		//	������CAppInitializer���X�R�[�v�A�E�g����̂����A���̂Ƃ���
		//	���ׂẴX���b�h�̏I����҂��ƂɂȂ�
	}
	return 0;
}
