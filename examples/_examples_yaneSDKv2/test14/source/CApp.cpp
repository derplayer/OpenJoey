#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����
	GetDraw()->SetDisplay(false);				//	Window���[�h

#ifdef _USE_FastDraw
	CFastPlane pp;
	CFastPlane bg;
	CFastPlane light;
	pp.Load("data/Shi-20-01.bmp.yga");

	//	���`�����l���t���T�[�t�F�[�X�������I�ɍ쐬
	bg.SetYGAUse(true);
#else
	CDIB32 pp;
	CDIB32 pp2;
	CDIB32 bg;
	CDIB32 light;
	pp.Load("data/Shi-20-01.bmp.yga");
	pp.FadeAlpha(80*255/100);	//	��������90%
	{ // ���T�C�Y�̃T�[�t�F�[�X���쐬
		int sx,sy;
		pp.GetSize(sx,sy);
		pp2.CreateSurface(sx,sy);
	}
#endif

	bg.Load("data/bg.jpg");

	light.Load("data/light.jpg");

	CTextLayer tl;
	tl.GetFont()->SetText("�����L�����̗֊s�̓��u�����h�ɂ��\n�W���M���}�����Ă���i�����������ɔ������I�j\n"
		"�����ă��l�t���摜�̃��A���^�C���t�F�[�h�B\n"
		"�a�f�́A���j�o�[�T���g�����W�V�����ɂ��\n"
		"�����Y�t���A�G�t�F�N�g�B");
	tl.GetFont()->SetSize(25);
	tl.SetPos(40,340);

	int x=0,y=103,nPhase=0;
	int nLight = 255;

	//	for fastplane debugging..
//	nPhase=1;

	CRootCounter r(0,255,8);
	r.SetInit(255);		//	�����l�̐ݒ�
	r.SetReverse(true);	//	�I�[�g���o�[�X�J�E���^
	
	while (IsThreadValid()){
		switch (nPhase){
		case 0: {
			GetDraw()->GetSecondary()->Clear();
			bg.BltToAlpha(&light,nLight,nLight+128,0,255);	//	light����
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&bg,0,0);
			nLight-=10; if (nLight<-128) nLight = 255;

			GetDraw()->GetSecondary()->BlendBltFastAlpha(&pp,x,y);
			r.Inc();
#ifdef _USE_FastDraw
			//	FastPlane�ɂ́A���v���[���ɑ΂���Fade���Ȃ���̓]�����l�C�e�B�u�ɃT�|�[�g����Ă���
			GetDraw()->GetSecondary()->FadeAlphaBlt(&pp,500-x,y,r);
#else
			pp2.FadeBltAlpha(&pp,r);
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&pp2,500-x,y);
#endif
			x++; if (x>200) { x=0; nPhase++; nLight=-128; r.Reset(); }
			break;
				}
		case 1: {
			GetDraw()->GetSecondary()->Clear();
			bg.BltToAlpha(&light,nLight,nLight+128,0,255);	//	light����
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&bg,0,0);
			nLight+=10; if (nLight>255) nLight = -128;

			int sx,sy;
			pp.GetSize(sx,sy);
			SIZE s = { sx*2, sy*2 };
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&pp,x,y,NULL,&s);
			r.Inc();
#ifdef _USE_FastDraw
			GetDraw()->GetSecondary()->FadeAlphaBlt(&pp,400-x,y,r,NULL,&s);
#else
			pp2.FadeBltAlpha(&pp,r);
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&pp2,400-x,y,NULL,&s);
#endif
			x+=2; if (x>200) { x=-100; y=-100; nPhase++; nLight=255; r.Reset(); }
			break;
				}
		case 2: {
			GetDraw()->GetSecondary()->Clear();
			bg.BltToAlpha(&light,nLight,nLight+128,255,0);	//	light����
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&bg,0,0);
			nLight-=10; if (nLight<-128) nLight = 255;

			int sx,sy;
			pp.GetSize(sx,sy);
			SIZE s = { sx*4, sy*4 };
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&pp,x,y,NULL,&s);
			r.Inc();
#ifdef _USE_FastDraw
			GetDraw()->GetSecondary()->FadeAlphaBlt(&pp,200-x,y,r,NULL,&s);
#else
			pp2.FadeBltAlpha(&pp,r);
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&pp2,200-x,y,NULL,&s);
#endif
			x+=2; if (x>100) { x=-300; y=-100; nPhase++; nLight=-128; r.Reset(); }
			break;
				}
		case 3: {
			GetDraw()->GetSecondary()->Clear();
			bg.BltToAlpha(&light,nLight,nLight+128,255,0);	//	light����
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&bg,0,0);
			nLight+=10; if (nLight>255) nLight = -128;

			int sx,sy;
			pp.GetSize(sx,sy);
			SIZE s = { sx*8, sy*8 };
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&pp,x,y,NULL,&s);
			r.Inc();
#ifdef _USE_FastDraw
			GetDraw()->GetSecondary()->FadeAlphaBlt(&pp,-100-x,y,r,NULL,&s);
#else
			pp2.FadeBltAlpha(&pp,r);
			GetDraw()->GetSecondary()->BlendBltFastAlpha(&pp2,-100-x,y,NULL,&s);
#endif
			x+=2; if (x>0) { x=0; y=103; nPhase=0; nLight=255; r.Reset(); }
			break;
				}
		}
		GetDraw()->OnDraw();
		GetKey()->Input();
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
