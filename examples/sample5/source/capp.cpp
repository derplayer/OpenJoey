#include "stdafx.h"
#include "CApp.h"
#include "CScnConvert.h"

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����
	GetDraw()->SetDisplay(false);				//	Window���[�h

	CFPSTimer t;
	t.SetFPS(10);

	int c0 = 0 , c1 = 0;
	int c = (((~c0 & c1) << 1) + ((~c0 ^ c1) & 0x7bde));
	//c = (((~c0 & c1) << 1) + ((~c0 ^ c1) & 0x7bde));

	SetMainApp(true);

	CDir dir;
	dir.SetPath("org");
	dir.SetFindFile("*.txt");

	CTextFastPlane text(GetDraw());
	text.GetFont()->SetText("���e�R���o�[�^�@version 1.01");
	text.GetFont()->SetSize(20);
	text.UpdateTextAA();

	CDbg().Out("�ϊ��������J�n���܂�");

	{
		CFile file;
		file.Delete("html/covertlog.txt");
	}

	while (IsThreadValid()){

		GetDraw()->GetSecondary()->BltNatural(&text,0,0);
		GetDraw()->OnDraw();

		string src_filename;
		if (dir.FindFile(src_filename)!=0) break;
		CDbg().Out(src_filename + "��ϊ���..");

		CScnConvert scn;
		if (scn.Convert(src_filename,"html")==0){
			CDbg().Out(src_filename + "�̕ϊ��I��");
		} else {
			CDbg().Out(src_filename + "�̕ϊ����s");
		}
		t.WaitFrame();
	}

	CDbg().Out("���ׂĂ̕ϊ��������I���܂���");
	CDbg().Out("�E�B���h�D�̉E��́~��ŏI�������Ă�������");
	
	//	�E�B���h�D�̉E��́~��ŕ��Ă�[
	while (IsThreadValid()) {
		GetDraw()->GetSecondary()->BltNatural(&text,0,0);
		GetDraw()->OnDraw();
		t.WaitFrame();
	}
}

//	���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
	virtual void MainThread(void){			//	���ꂪ���[�J�[�X���b�h
		CApp().Start();						//	CApp app; app.Start();�̈Ӗ���
	}
};

//	���킸�ƒm�ꂽWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	�K�������Ă�
	CSingleApp sapp;
	if (sapp.IsValid()) {
		CThreadManager::CreateThread(new CAppMainWindow());					//	��Œ�`�������C���̃E�B���h�D���쐬
	}
	return 0;
}
