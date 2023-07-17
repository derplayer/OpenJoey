#include "stdafx.h"
#include "../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {

	CDirectDraw m_Draw;
	CDirectDraw* GetDraw(void) { return& m_Draw; }

    void MainThread(void) {                 //  ���ꂪ���s�����

		GetDraw()->SetDisplay(false);		//	Window���[�h
		GetDraw()->CreateSecondaryDIB();	//	Secondary�Ƃ���DIB�𐶐�

		CDIB32 dib;
		dib.Load("aru1ss.jpg");

		CFPSTimer t;		//	FPS�����p
		t.SetFPS(30);

		CFPSLayer fps(&t);	//	FPS�l�̕`��

		CTextLayer tl;
		tl.GetFont()->SetSize(30);
		tl.GetFont()->SetText("�X�y�[�X�L�[�������ĂˁI");
		tl.GetFont()->SetColor(RGB(0,80,128));
		tl.SetPos(150,440);

		CKey key;			//	�L�[����

		int	nScene = 0,nPhase = 0;
		while(IsThreadValid()) {				//  ���ꂪValid�̊ԁA�܂�葱����
			key.Input();
			if (key.IsVKeyPress(0)) break;		//	ESC�ŏI��
			if (key.IsVKeyPush(5)) {
				//	�X�y�[�X�L�[�������ꂽ��V�[���ύX
				nScene = (nScene+1)%9;
				nPhase = 0;
				CHAR szBuf[256];
				wsprintf(szBuf,"Scene == %d",nScene);
				tl.GetFont()->SetText(szBuf);
			}

			switch (nScene) {
			case 0:
				//	�w�i�̃N���A
				GetDraw()->GetSecondaryDIB()->Clear();
				GetDraw()->GetSecondaryDIB()->BltFast(&dib,100,0);
				break;
			case 1:
				GetDraw()->GetSecondaryDIB()->BlendBltFast(&dib,100,-nPhase*2,0xa0a0a0,0x404040);
				break;
			case 2:
				GetDraw()->GetSecondaryDIB()->BltFast(&dib,100,0);
				GetDraw()->GetSecondaryDIB()->BlendBltFast(&dib,100,-nPhase*2,0xa0a0a0,DIB32RGB(nPhase,nPhase/2,nPhase));
				break;
			case 3: {
				GetDraw()->GetSecondaryDIB()->BltFast(&dib,100,0);
				GetDraw()->GetSecondaryDIB()->AddColorFast(DIB32RGB(nPhase,nPhase,nPhase));
					} break;
			case 4: {
				GetDraw()->GetSecondaryDIB()->BltFast(&dib,100,0);
				SIZE size;
				size.cx = 1000;
				size.cy = 1000;
				GetDraw()->GetSecondaryDIB()->BlendBltFast(&dib,0,-nPhase*5,0xa0a0a0,0x404040,NULL,&size);
				break;
					}
			case 5: {
				GetDraw()->GetSecondaryDIB()->BltFast(&dib,100,0);
				SIZE size;
				size.cx = 2000;
				size.cy = 4000;
				GetDraw()->GetSecondaryDIB()->BlendBltFast(&dib,-1000,-nPhase*8,0xa0a0a0,0x404040,NULL,&size);
				break;
					}
			case 6:
				GetDraw()->GetSecondaryDIB()->BltFast(&dib,100,0);
				GetDraw()->GetSecondaryDIB()->SubColorFast(DIB32RGB(nPhase,nPhase,nPhase));
				break;
			case 7:
				GetDraw()->GetSecondaryDIB()->BltFast(&dib,100,0);
				GetDraw()->GetSecondaryDIB()->SubColorFast(DIB32RGB(0,nPhase,nPhase/2));
				break;
			case 8: {
				POINT point[4];
				point[0].x = nPhase;	    point[0].y = nPhase*2;
				point[1].x = 400;			point[1].y = 0;
				point[2].x = 0;				point[2].y = 600;
				point[3].x = 400-nPhase/2;	point[3].y = 600-nPhase;
				RECT rect;
				::SetRect(&rect,0,0,640,480);
				GetDraw()->GetSecondaryDIB()->MorphBltFast(&dib,point,&rect);
					} break;
			}
			nPhase = (nPhase+1)&255;

			GetDraw()->OnDrawDIB();
			t.WaitFrame();
        }
    }
};

//  ���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {    //  �A�v���P�[�V�����N���X����h��
    virtual void MainThread(void){          //  ���ꂪ���[�J�[�X���b�h
        CApp().Start();                     //  CApp app; app.Start();�̈Ӗ���
    }
};

//  ���킸�ƒm�ꂽWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    CAppInitializer::Init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);  //  �K�������Ă�
	CSingleApp sapp;
	if (sapp.IsValid()) {
	    CAppMainWindow().Run();                 //  ��Œ�`�������C���̃E�B���h�D���쐬
	}
    return 0;
}
