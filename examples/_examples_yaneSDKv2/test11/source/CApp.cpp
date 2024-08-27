#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����

	GetDraw()->SetDisplay(false);				//	Window���[�h

	CFPSTimer t;
	t.SetFPS(30);

	CDIB32 plane;
	{
		CDIB32 planeB;
		planeB.Load("data/satomi.jpg");
		plane.CreateSurface(640,480);
		plane.BltFast(&planeB,0,0);	//	�����L�΂��Ă���
		plane.BltFast(&planeB,280,0);
		plane.BltFast(&planeB,280*2,0);
	}

	CTextDIB32 td;
	td.GetFont()->SetSize(30);
	td.GetFont()->SetText("�X�y�[�X�����Ύ��ɍs���ɂ�");
	td.UpdateText();

	plane.Blt(&td,130,400);

	CBumpMap bumpmap;
	MakeBumpMap(bumpmap,0);

	int x=0,y=0,vx=5,vy=5;
	int nType = 1;
	CDIB32* lpDIB =	GetDraw()->GetSecondary();
	lpDIB->BltFast(&plane,0,0);
	while(IsThreadValid()) {					//	���ꂪValid�̊ԁA�܂�葱����
		GetKey()->Input();
		if (GetKey()->IsVKeyPush(0)) break;

		if (GetKey()->IsVKeyPush(5)) {
			MakeBumpMap(bumpmap,nType);
			nType++; if (nType >= 6) nType = 0;
		}

		RECT rect;
		::SetRect(&rect,x,y,x+300,y+300);
		lpDIB->BltFast(&plane,x,y,&rect);
		//	�ړ�
		x+=vx;y+=vy;
		//	�ǂɂԂ������̂Ŕ��]
		if (x<0)		{ vx = -vx; x+=vx; }
		if (x>=640-300)	{ vx = -vx; x+=vx; }
		if (y<0)		{ vy = -vy; y+=vy; }
		if (y>=480-300) { vy = -vy; y+=vy; }
		::SetRect(&rect,x,y,x+300,y+300);
		lpDIB->BumpMapFastBlt(&plane,&bumpmap,x,y,&rect);
		GetDraw()->OnDraw();
	}
}

void	CApp::MakeBumpMap(CBumpMap&bump,int nType){
	switch (nType) {
	case 0:	CBumpEffecter::LensEffect1(&bump ,300,300,10);  break;
	case 1:	CBumpEffecter::LensEffect1(&bump ,300,300,2);   break;
	case 2:	CBumpEffecter::SwirlEffect1(&bump,300,300,100); break;
	case 3:	CBumpEffecter::SwirlEffect1(&bump,300,300,30);  break;
	case 4:	CBumpEffecter::SwirlEffect2(&bump,300,300,3);   break;
	case 5:	CBumpEffecter::SwirlEffect2(&bump,300,300,5);  break;
	}
}


//	���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
	virtual void MainThread(void){			//	���ꂪ���[�J�[�X���b�h
		CApp().Start();						//	CApp app; app.Start();�̈Ӗ���
	}
	virtual LRESULT OnPreCreate(CWindowOption& opt){
		opt.caption		= "�摜 Viewer";
		opt.classname	= "YANEAPPLICATION";
		opt.size_x		= 640;
		opt.size_y		= 480;
		opt.style		= WS_SYSMENU | WS_MINIMIZEBOX;
		return 0;
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
