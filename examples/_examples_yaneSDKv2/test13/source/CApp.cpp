#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����

	GetDraw()->SetDisplay(false);				//	Window���[�h

	CFPSTimer t;
	t.SetFPS(30);

	CDIB32 bg;
	bg.Load("data/Talk.jpg");

	CFile file;
	file.Read("data/out.html");
	CTextDrawContext context;
	context.SetTextPtr((LPSTR)file.GetMemory());
	context.m_nWidth = 480;
	context.SetBaseFontSize(25);
	context.m_rgbColorBk = RGB(128,128,128);
//	CTextDrawDIB32 textdraw;
	CTextDrawDIB32A textdraw;	//	�A���`�F���t���o�[�W����
	textdraw.SetContext(context);
	textdraw.UpdateText();

	while (IsThreadValid()){
		GetDraw()->Clear();
		GetDraw()->Blt(&bg,0,0);

//		GetDraw()->Blt(textdraw.GetDIB(),128,376);
		GetDraw()->BlendBltFastAlpha(textdraw.GetDIB(),128,376);
		
		t.WaitFrame();

		GetKey()->Input();
		if (GetKey()->IsVKeyPush(5)) {	//	�ǂݐi�߂�
			textdraw.GoNextContext();
			textdraw.UpdateText();

			/*
			{	//	��͂ł��Ȃ������^�O��\�����Ă݂��
				for(int i=0;i<textdraw.GetTagList()->size();++i){
					CHAR buf[10];
					::CopyMemory(&buf,(void*)((*textdraw.GetTagList())[i]),9);
					buf[9] = '\0';
					CDbg db;
					db.Out("%s",buf);
				}
			}
			*/

		}
		if (GetKey()->IsVKeyPush(6)) {	//	�擪�ɖ߂�
			textdraw.SetTextOffset(0);
			textdraw.UpdateText();
		}
		GetDraw()->OnDraw();
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
	CAppInitializer::Init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	�K�������Ă�
	CSingleApp sapp;
	if (sapp.IsValid()) {
		CAppMainWindow().Run();					//	��Œ�`�������C���̃E�B���h�D���쐬
	}
	return 0;
}
