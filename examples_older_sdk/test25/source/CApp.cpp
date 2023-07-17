#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����

	//	DirectInput�́A�}���`�X���b�h��Ή��Ȃ̂��B
	//	������A���̃N���X�̃����o��CKey���������邱�Ƃ͂��܂̂Ƃ���s�Ȃ̂��B

	GetDraw()->SetDisplay(false,320,240);

	static volatile int nNo=0;
	nNo++;
	CDbg().Out("��"+CStringScanner::NumToString(nNo)+"�X���b�h�������ꂽ�҂��I");

#ifdef FAST_PLANE_MODE
	CFastPlane dib;
#else
#ifdef PLANE_MODE
	CPlane dib;
#else
	CDIB32 dib;
#endif
#endif
	dib.Load("data/113s.jpg");

	CFPSTimer timer;
	timer.SetFPS(10);
	CFPSLayer l(&timer);
	l.SetPos(0,0);

	int nCount = 0;
	smart_ptr<CAppMainWindow> p[5];
	while(IsThreadValid()) {					//	���ꂪValid�̊ԁA�܂�葱����
		nCount ++;
		if (nCount==30 && nNo <=4){
			//	�q�����Y�ށO�O�G
			CDbg().Out("�΂̖�؂�H�ׂĎq�����Y�ނ񂾂҂��I");
			
			//	��������CAppBase�̃|�C���^�́A����Thread���ӔC�������āA����
			//	�֐����甲����܂łɉ�̂��Ȃ���΂Ȃ�Ȃ��B
			p[nNo].Add(new CAppMainWindow);
			p[nNo]->Run();

			//	��Œ�`�������C���̃E�B���h�D���쐬
		}

		GetDraw()->GetSecondary()->BltFast(&dib,0,0);
		GetDraw()->OnDraw();

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
