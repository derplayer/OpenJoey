#include "stdafx.h"
#include "CApp.h"
#include "CMonologueView.h"

	//	BGM Collection
SLOAD_CACHE BGM_LOADER_DEFINE[] = {
	"data/BGM31.mid"		,0,	//	0:BGM  opening
	NULL,
};

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����

	GetDraw()->SetDisplay(false);				//	Window���[�h

	m_BGMLoader.Set(BGM_LOADER_DEFINE);			//	BGM�̃��X�g���`
	
	CFPSTimer t;
	t.SetFPS(20);

	CMonologueView mv(this);
	mv.Load("data/opening.txt");
	while(IsThreadValid()) {					//	���ꂪValid�̊ԁA�܂�葱����
		if (mv.OnDraw()) break;
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
	CAppInitializer::Init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	�K�������Ă�
	CSingleApp sapp;
	if (sapp.IsValid()) {
		CAppMainWindow().Run();					//	��Œ�`�������C���̃E�B���h�D���쐬
	}
	return 0;
}
