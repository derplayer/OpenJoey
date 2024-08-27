#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����

	GetDraw()->SetDisplay(false);

	CFastPlane plane;
	plane.Load("data/113b.jpg");

	//	plane.Load("data/skip.yga");
	//	�������ł͍s�Ȃ��Ă��Ȃ����Ayga�摜��ǂݍ��߂΁A
	//	�����I�ɓ����Ń��t���T�[�t�F�[�X�����������B
	//	���������t���T�[�t�F�[�X�ɑ΂��Ă��A���t�����F�����A���t�����F�����A
	//	���t���u�����h�����T�|�[�g���Ă���B

	CFastPlane bg;
	bg.Load("data/276.jpg");

	CFPSTimer timer;
	timer.SetFPS(120);
	CFPSLayer l(&timer);
	l.SetPos(0,0);

	CTextFastPlane text;
	text.GetFont()->SetSize(40);

	CTextFastPlane text2;
	text2.GetFont()->SetSize(20);
	text2.GetFont()->SetColor(RGB(128,50,60));
	text2.GetFont()->SetText("��ʃ��[�h��8bpp,15/16bpp,24bpp,32bpp�ł������Ă݂ĂˁI");
	text2.UpdateTextA();
	//	��UpdateTextA���g���΁A�����I�Ɏ����I�Ƀ��t���T�[�t�F�[�X�����������

	int nCount = 0;

	while(IsThreadValid()) {					//	���ꂪValid�̊ԁA�܂�葱����

		int nCountL = nCount & 0xff;
		int nCountH = nCount >> 8;
		if (nCountL==0){
			switch (nCountH) {
			case 0: text.GetFont()->SetText("�萔�{�O�a���Z"); break;
			case 1: text.GetFont()->SetText("�萔�{�O�a���Z"); break;
			case 2: text.GetFont()->SetText("������"); break;
			case 3: text.GetFont()->SetText("��ʑS�̂ɑ΂���O�a���Z"); break;
			case 4: text.GetFont()->SetText("��ʑS�̂ɑ΂���O�a���Z"); break;
			}
			//	�������A�A���`�F���t���ŕ\������̂��I
			text.UpdateTextA();
		}

		GetDraw()->GetSecondary()->BltFast(&bg,0,0);

			//	RGB555,565�̂Ƃ��́A���ɑ΂��āA4bit�e�[�u���Ŋ|���Z���邽�߁A
			//	���4bit�݂̂����L���ł͖����D�D

		switch (nCountH) {
		case 0:
			GetDraw()->GetSecondary()->AddColorAlphaBlt(&plane,60+nCountL,0,nCountL);
			break;
		case 1:
			GetDraw()->GetSecondary()->SubColorAlphaBlt(&plane,60+nCountL,0,nCountL);
			break;
		case 2:
			GetDraw()->GetSecondary()->BlendBltFast(&plane,60+nCountL,0,nCountL);
			break;
		case 3:
			GetDraw()->GetSecondary()->AddColorFast(RGB(nCountL,nCountL,nCountL));
			break;
		case 4:
			GetDraw()->GetSecondary()->SubColorFast(RGB(nCountL,nCountL,nCountL));
			break;
		}

		GetDraw()->GetSecondary()->BltNaturalPos(&text,320,420,0);
		GetDraw()->GetSecondary()->BltNaturalPos(&text2,320,450,0);

		GetDraw()->OnDraw();

		nCount++; if (nCount == 256*5) nCount = 0;

		timer.WaitFrame();
	}
}

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
