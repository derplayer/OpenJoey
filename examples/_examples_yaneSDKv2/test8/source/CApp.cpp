#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����

	GetDraw()->SetDisplay(false);				//	Window���[�h

	CFPSTimer t;
	t.SetFPS(30);

	CFPSLayer fps(&t);
	fps.SetPos(0,0);

	CDIB32 plane;
	plane.Load("data/twinlove2.jpg");

	CTextLayer tl;
	tl.GetFont()->SetText("�X�y�[�X�L�[�������Ǝ��̃p�^�[���ɍs���ɂ�B���^�[���őO�̂ɖ߂�");
	tl.SetPos(0,440);

	CTextLayer tl2;
	tl2.SetPos(200,460);

	int nPhase = 0;
	int	nV = 8;
	int nPat = 0;		//	�p�^�[���i���o�[
	while(IsThreadValid()) {					//	���ꂪValid�̊ԁA�܂�葱����
		GetKey()->Input();
		if (GetKey()->IsVKeyPush(5)) {
			nPat++;
			if (nPat==20) nPat = 0;
			nPhase = 8; nV = 8;
			GetDraw()->SetBrightness(256);
		}
		if (GetKey()->IsVKeyPush(6)) {
			if (nPat) nPat--;
			nPhase = 8; nV = 8;
			GetDraw()->SetBrightness(256);
		}
		CHAR buf[256];
		::wsprintf(buf,"nPat = %d  , nPhase = %d",nPat,nPhase);
		tl2.GetFont()->SetText(buf);

		CDIB32* lpDIB =	GetDraw()->GetSecondary();
		lpDIB->Clear(DIB32RGB(255,255,255));

		switch ((nPat/5)&3){
		case 0:
			lpDIB->RotateBltFast(&plane,NULL,0,0,nPhase,1<<16,nPat%5/*,&clip*/);	//	��]�`��
			break;
		case 1:
			lpDIB->RotateBltFast(&plane,NULL,200,0,nPhase,1<<15,nPat%5/*,&clip*/);	//	50%�k���̉�]�`��
			break;
		case 2:
			lpDIB->RotateBltFast(&plane,NULL,0,0,nPhase,nPhase*512,nPat%5/*,&clip*/);	//	�t�F�[�h�t���g�債�Ȃ���̉�]�`��
			GetDraw()->SetBrightness(nPhase/4 + 128);
			break;
		case 3:
			lpDIB->RotateBlt(&plane,NULL,0,0    ,    nPhase      ,1<<16,4     /*,&clip*/);	//	��]�`��
			lpDIB->RotateBlt(&plane,NULL,0,0    ,    nPhase      ,1<<15,nPat%5/*,&clip*/);	//	��]�`��
			lpDIB->RotateBlt(&plane,NULL,400,0  ,512-nPhase      ,1<<15,nPat%5/*,&clip*/);	//	��]�`��
			lpDIB->RotateBlt(&plane,NULL,0,300  ,    256+nPhase  ,1<<15,nPat%5/*,&clip*/);	//	��]�`��
			lpDIB->RotateBlt(&plane,NULL,400,300,512-nPhase+256  ,1<<15,nPat%5/*,&clip*/);	//	��]�`��
			break;
		}

		GetDraw()->OnDraw();

		t.WaitFrame();
		if ((nPhase == 512) || (nPhase == 0)) ::Sleep(200);
		nPhase+=nV;
		if ((nPhase == 512) || (nPhase == 0)) nV = -nV;
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
