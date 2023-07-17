#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����

	GetDraw()->SetDisplay(false);				//	Window���[�h

	CFPSTimer t;
	t.SetFPS(30);

//	CFPSLayer fps(&t);
//	fps.SetPos(0,0);

#ifdef FAST_PLANE_MODE
	CFastPlane bg;
	bg.SetYGAUse(true);	//	�����Ń��T�[�t�F�[�X���쐬
	CFastPlane yga;
#else
	CDIB32 bg;
	CDIB32 yga;
#endif
	bg.Load("data/grd.jpg");
	yga.Load("data/title.yga");

	CTextLayer tl;
	tl.GetFont()->SetText("stencil alpha with gradation layer");
	tl.GetFont()->SetSize(30);
	tl.SetPos(60,440);

	bool bAlpha = true;	//  stencil alpha or silhouette alpha
	bool bBG	= true;	//	grd.jog or CG_4_003.jpg

	CRootCounter r(0,500,4);
	r.SetReverse(true);

	while(IsThreadValid()) {					//	���ꂪValid�̊ԁA�܂�葱����
		GetKey()->Input();
		if (GetKey()->IsVKeyPush(0)) break;

#ifdef FAST_PLANE_MODE
		GetDraw()->GetSecondary()->SetFillColor(RGB(r/2,0,(511-r)/2));
		GetDraw()->GetSecondary()->Clear();
#else
		GetDraw()->GetSecondary()->Clear(DIB32RGB(r/2,0,(511-r)/2));
#endif

		//	�����ŁA���l�����������܂܉摜��]������
		yga.BltFastWithoutAlpha(&bg,-r,-50);

		GetDraw()->GetSecondary()->BlendBltFastAlpha(&yga,100,60);
		r.Inc();
		if (r.IsLapAroundI()) {
			yga.FlushAlpha();
			if (bAlpha) {
				if (bBG) {
					tl.GetFont()->SetText("silhouette alpha with gradation layer");
				} else {
					tl.GetFont()->SetText("silhouette alpha with jpeg layer");
				}
			} else {
				if (bBG) {
					tl.GetFont()->SetText("stencil alpha with jpeg layer");
					bg.Load("data/CG_4_003.jpg");
				} else {
					tl.GetFont()->SetText("stencil alpha with gradation layer");
					bg.Load("data/grd.jpg");
				}
				bBG = !bBG;
			}
			bAlpha = !bAlpha;
		}
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
