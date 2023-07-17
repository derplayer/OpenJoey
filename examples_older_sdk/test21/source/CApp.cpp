#include "stdafx.h"
#include "CApp.h"

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����

	GetDraw()->SetDisplay(false);				//	Window���[�h
	CFPSTimer t;
	t.SetFPS(30);

	CKey3 key;
	int nNum = CJoyStick::GetDirectInputJoyStickNum(); // �W���C�X�e�B�b�N�̖{��
	if (nNum==0) return ; // JoyStick�����ɂ�I�O�O�G

	auto_array<CJoyStick> aJoy(nNum);
	{
		for ( int i = 0; i < nNum ; i++){
			int j = (int)jsDIRECT_JOYSTICK1+i;
			aJoy[i].SelectDevice( (JoySelector)j );
			aJoy[i].SetButtonMax(32);
		}
	}
	CTextPlane tMode[2];
	tMode[0].GetFont()->SetSize(15);
	tMode[0].SetColorKey(RGB(255,255,255));
	tMode[0].GetFont()->SetText("ON");
	tMode[0].UpdateText();
	tMode[1].GetFont()->SetSize(15);
	tMode[1].SetColorKey(RGB(255,255,255));
	tMode[1].GetFont()->SetText("OFF");
	tMode[1].UpdateText();
	POINT	pos[] = { 
		40,0,
		40,40,
		20,20,
		60,20,
	};

	while(IsThreadValid()) {					//	���ꂪValid�̊ԁA�܂�葱����
		GetDraw()->Clear();
		key.Input();

		for ( int k = 0; k < nNum ; k++){
		aJoy[k].GetKeyState();
			for ( int i = 0 ; i < 4 ; i++ ){
				int nMode;
				if ( aJoy[k].IsKeyPress(i) )  nMode = 0;else nMode =1;
				GetDraw()->BltFast(&tMode[nMode],pos[i].x,pos[i].y+k*90);
			}
			for ( i = 0 ; i < 32 ; i++ ){
				int nMode;
				if ( aJoy[k].IsKeyPress(i+4) )  nMode = 0;else nMode =1;
				GetDraw()->BltFast(&tMode[nMode],i*30,60+k*90);
			}
		}
		if ( key.IsVKeyPress(0) ) break; 

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
