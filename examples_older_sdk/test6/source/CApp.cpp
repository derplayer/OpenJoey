#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����

	GetDraw()->SetDisplay(false);				//	Window���[�h
	GetDraw()->CreateSecondaryDIB();

	CFPSTimer t;
	t.SetFPS(30);

	CFPSLayer fps(&t);
	fps.SetPos(0,0);

	const int nMax = 30;

	CDIB32 plane[nMax];
	plane[nMax-1].Load("bg.jpg");
	int i;
	for(i=0;i<nMax-1;i++){
		int sx,sy;
		plane[nMax-1].GetSize(sx,sy);
		plane[i].CreateSurface(sx,sy);
		plane[i].BltFast(&plane[nMax-1],0,0);
		if (i!=nMax-2)	//	�Ō�̃������[�v�͔�����
			plane[nMax-1].ShadeOff();
	}

	//	CTextDIB32���g����F
	CTextDIB32 plane2[2];
	LPCTSTR alpsz[] = {
		"�������p���̎��ɂ�\n�p���̎����܂��ɂ�",
		"�����̓p���̎��ɍ����ɂ�\n�p���̎��ō��ɂ�",
		"����̓p���̎��Ƀo�^�[�ɂ�\n�p���̎����������ɂ�",
		"�����͗g���p���̎��ɍ����ɂ�\n�p���̎����O���H��",
		"��������̓n�`�~�c���p���̎��ɂ�\n�p���̎��������ɂ�",
		"���N�����ς��̓p���̎��ŏ��؂�ɂ�\n�p���̎��O���ɂ�",
		"�Q�}�`",
		NULL,NULL,
	};

	int n = 0 , nv = 1;
	int nt = 0;
	while(IsThreadValid()) {					//	���ꂪValid�̊ԁA�܂�葱����
		GetKey()->Input();

		GetDraw()->GetSecondaryDIB()->Clear();

		//	WAFFLE���S
		GetDraw()->GetSecondaryDIB()->Blt(&plane[n],200,100);

		if (n==0){
			for(i=0;i<2;i++){
				plane2[i].GetFont()->SetSize(30);
				plane2[i].GetFont()->SetQuality(2);
				plane2[i].GetFont()->SetText(alpsz[nt]);
				plane2[i].UpdateText();
			}
			if (alpsz[nt+1]==NULL) nt=0; else nt++;
		}
		
		//	�����p�l��
		if (nv>0) {
			GetDraw()->GetSecondaryDIB()->Blt(&plane2[1],100+n,120+n);	//	�e���ɕ`���Ȃ���
		}
		GetDraw()->GetSecondaryDIB()->Blt(&plane2[0],100,120);
		plane2[1].ShadeOff();	//	�ڂ���
		n+=nv;
		if (n==nMax-1 || n==0) nv = -nv;
		if (n==0) {
			plane2[1].BltFast(&plane2[0],0,0);
		}

		GetDraw()->OnDrawDIB();
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
