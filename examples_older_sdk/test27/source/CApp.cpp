#include "stdafx.h"
#include "CApp.h"

#include <math.h>

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����
	GetDraw()->SetDisplay(false);
	while(IsThreadValid()){
		//LineBarCountdown(false);
		//LineBarCountdown(true);
		CircleBarCountdown(false);
		CircleBarCountdown(true);
	}
}

void CApp::CircleBarCountdown(bool IsBlur)
{
	CFastPlane* lp = GetDraw()->GetSecondary();
	
	SIZE dstsize={640,480};
	
	// Blur�̂��߂̃t�F�[�h��
	int FadeTable[256*2+1+1] = {0};
	{
		for(int i=0; i<256+1; i++){
			// t �^�C�v
			FadeTable[2*i]   = 128 - (int)(128 * (double)i / 256);
			FadeTable[2*i+1] = 127 + (int)(128 * (double)i / 256);
		}
	}
	// �g��̂��߂�RECT�\����
	RECT RectTable[256+1] = {0};
	{
		for(int i=0; i<256+1; i++){
			/* ���D���Ȃ��̂�I��ł�������
			// t^(1/2) �^�C�v
			RectTable[i].left   = 0   + (int)(320*sqrt((double)i/256));
			RectTable[i].top    = 0   + (int)(240*sqrt((double)i/256));
			RectTable[i].right  = 640 - (int)(320*sqrt((double)i/256));
			RectTable[i].bottom = 480 - (int)(240*sqrt((double)i/256));	
			//*/

			//*
			// t �^�C�v
			RectTable[i].left   = 0   + (int)(320*(double)i/256);
			RectTable[i].top    = 0   + (int)(240*(double)i/256);
			RectTable[i].right  = 640 - (int)(320*(double)i/256);
			RectTable[i].bottom = 480 - (int)(240*(double)i/256);	
			//*/

			/*
			// t^2 �^�C�v
			RectTable[i].left   = 0   + (int)(320*((double)i/256)*((double)i/256));
			RectTable[i].top    = 0   + (int)(240*((double)i/256)*((double)i/256));
			RectTable[i].right  = 640 - (int)(319*((double)i/256)*((double)i/256));
			RectTable[i].bottom = 480 - (int)(239*((double)i/256)*((double)i/256));	
			//*/
		}
	}


	CFastPlane plane1, plane2;
	plane1.CreateSurface(640, 480, false);
	plane2.CreateSurface(640, 480, false);

	CFastPlane CircleBar;
	CircleBar.SetYGAUse(true);
	CircleBar.Load("data/CircleBar.yga");
	CFastPlane CircleGrad;
	CircleGrad.Load("data/CircleGrad.bmp");

	CFastPlane bg;
	bg.Load("data/276.jpg");

	CFPSTimer timer;
	timer.SetFPS(15);
	CFPSLayer l(&timer);
	l.SetPos(0,0);

	CTextFastPlane text, text2;
	text.GetFont()->SetSize(50);
	text2.GetFont()->SetSize(20);
	text2.GetFont()->SetText("Blur%s�Y�[���ŁA�~�O���t�J�E���g�_�E��", (IsBlur)?"����":"�Ȃ�");
	text2.UpdateTextA();

	int nCount = 255;
	int nCountdown = 3;
	int nPhase = 0;
	while(IsThreadValid()) {
  		plane1.BltFast(&bg,0,0);
		text.GetFont()->SetText("%d", nCountdown);
		text.UpdateTextA();

		CircleBar.BltToAlpha(&CircleGrad, nCount, nCount, 0, 255);
		nCount -= 10;
		if (nCount<0){
			nCount = 255;
			nCountdown--;
			CircleBar.BltToAlpha(&CircleGrad, 0, 1, 0, 255);
		}
		plane1.BlendBltFastAlpha(&CircleBar,(640-200)/2,(480-200)/2);

		plane1.BltNaturalPos(&text,320,240,0);

		if(nPhase==0||!IsBlur){
			// �����Blend����
			// nPhase�ɉ����Ċg�傷��
			lp->BltFast(&plane1, 0, 0, &RectTable[nPhase], &dstsize);
		}else{
			// ���̃t���[���̉摜
			// nPhase�ɉ����Ċg�傷��
			lp->BlendBltFast(&plane1, 0, 0, FadeTable[nPhase], &RectTable[nPhase], &dstsize);
			// 1�t���[���O�̉摜
			lp->BlendBltFast(&plane2, 0, 0, FadeTable[nPhase+1]);
		}
		if(IsBlur){
			// �����_�����O��̉摜������Ă���
			plane2.BltFast(lp,0,0);
		}
		lp->BltNaturalPos(&text2,320,460,0);
		GetDraw()->OnDraw();

		// �܂��Ō�܂Ō��������Ƃ��������Ƃ�����^^;
		if(nPhase<=248){ nPhase+=4; }
		else if(nPhase<=252){ nPhase+=2; }
		else { nPhase+=1; }
		if(nPhase>256){ break; }

		timer.WaitFrame();
	}
}

void CApp::LineBarCountdown(bool IsBlur)
{
	CFastPlane* lp = GetDraw()->GetSecondary();
	SIZE dstsize = { 640,480 };

	// Blur�̂��߂̃t�F�[�h��
	int FadeTable[256*2+1+1] = {0};
	{
		for(int i=0; i<256+1; i++){
			// t �^�C�v
			FadeTable[2*i]   = 128 - (int)(128 * (double)i / 256);
			FadeTable[2*i+1] = 127 + (int)(128 * (double)i / 256);
		}
	}
	// �g��̂��߂�RECT�\����
	RECT RectTable[256+1] = {0};
	{
		for(int i=0; i<256+1; i++){
			/*
			// t^(1/2) �^�C�v
			RectTable[i].left   = 0   + (int)(320*sqrt((double)i/256));
			RectTable[i].top    = 0   + (int)(240*sqrt((double)i/256));
			RectTable[i].right  = 640 - (int)(320*sqrt((double)i/256));
			RectTable[i].bottom = 480 - (int)(240*sqrt((double)i/256));	
			//*/

			//*
			// t �^�C�v
			RectTable[i].left   = 0   + (int)(320*(double)i/256);
			RectTable[i].top    = 0   + (int)(240*(double)i/256);
			RectTable[i].right  = 640 - (int)(320*(double)i/256);
			RectTable[i].bottom = 480 - (int)(240*(double)i/256);	
			//*/

			/*
			// t^2 �^�C�v
			RectTable[i].left   = 0   + (int)(320*((double)i/256)*((double)i/256));
			RectTable[i].top    = 0   + (int)(240*((double)i/256)*((double)i/256));
			RectTable[i].right  = 640 - (int)(319*((double)i/256)*((double)i/256));
			RectTable[i].bottom = 480 - (int)(239*((double)i/256)*((double)i/256));	
			//*/
		}
	}

	CFastPlane plane1, plane2;
	plane1.CreateSurface(640, 480, false);
	plane2.CreateSurface(640, 480, false);

	CFastPlane LineBar;
	LineBar.Load("data/LineBar.bmp");

	CFastPlane bg;
	bg.Load("data/276.jpg");

	CFPSTimer timer;
	timer.SetFPS(15);
	CFPSLayer l(&timer);
	l.SetPos(0,0);

	CTextFastPlane text, text2;
	text.GetFont()->SetSize(50);
	text2.GetFont()->SetSize(20);
	text2.GetFont()->SetText("Blur%s�Y�[���ŁA�_�O���t�J�E���g�_�E��", (IsBlur)?"����":"�Ȃ�");
	text2.UpdateTextA();

	int nCount = 255;
	int nCountdown = 3;
	int nPhase = 0;
	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.bottom = 50;
	while(IsThreadValid()) {
  		plane1.BltFast(&bg,0,0);
		text.GetFont()->SetText("%d", nCountdown);
		text.UpdateTextA();

		nCount-=10;
		if (nCount<0){
			rc.right = 200;
			nCount = 255; nCountdown--;
		}else{
			rc.right = 200*(255-nCount)/255;
		}
		plane1.BltFast(&LineBar,(640-200)/2,(480-50)/2,&rc,NULL);

		plane1.BltNaturalPos(&text,320,240,0);

		if(nPhase==0||!IsBlur){
			// �����Blend����
			// nPhase�ɉ����Ċg�傷��
			lp->BltFast(&plane1, 0, 0, &RectTable[nPhase], &dstsize);
		}else{
			// ���̃t���[���̉摜
			// nPhase�ɉ����Ċg�傷��
			lp->BlendBltFast(&plane1, 0, 0, FadeTable[nPhase], &RectTable[nPhase], &dstsize);
			// 1�t���[���O�̉摜
			lp->BlendBltFast(&plane2, 0, 0, FadeTable[nPhase+1]);
		}
		if(IsBlur){
			// �����_�����O��̉摜������Ă���
			plane2.BltFast(lp,0,0);
		}
		lp->BltNaturalPos(&text2,320,460,0);
		GetDraw()->OnDraw();

		// �܂��Ō�܂Ō��������Ƃ��������Ƃ�����^^;
		if(nPhase<=248){ nPhase+=4; }
		else if(nPhase<=252){ nPhase+=2; }
		else { nPhase+=1; }
		if(nPhase>256){ break; }

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
