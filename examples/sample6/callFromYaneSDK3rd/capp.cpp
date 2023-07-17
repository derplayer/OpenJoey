#include "stdafx.h"

//	�P�DDLL����export����N���X�̃C���^�[�t�F�[�X��ǂݍ���
//	(����́A�ʃt�@�C���Ƀw�b�_�Ƃ��đ��݂���Ηǂ�)
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
			// 1����100�܂ł̍��v���v�Z����(DLL���̃N���X���Ăяo��)
			delete p;
			CDbg().Out(total);
		}
		//	�g���I�������A���
		CObjectCreater::GetObj()->ReleasePlugIn("sample.dll");
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
