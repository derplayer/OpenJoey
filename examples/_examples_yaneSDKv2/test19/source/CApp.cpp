#include "stdafx.h"
#include "CApp.h"


void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����

	GetDraw()->SetDisplay(false);				//	Window���[�h

	CFPSTimer t;
	t.SetFPS(30);

	CTextLayer text;
	text.GetFont()->SetBackColor(CLR_INVALID);
	text.GetFont()->SetText("���ƒ����̃{�^����Windows�̃{�^���Ɠ����悤�ȓ�������܂�\n"
		"�E�̃{�^���̓J�[�\�����d�˂�ƃA�N�e�B�u�ɂȂ�^�C�v�̃{�^���ł�");
	text.SetPos(50,300);

	CDIB32Loader loader;
	loader.Set("data/botton_cfg.txt");	//	�{�^���R���t�B�O

	CGUIButton bt[3];
	{
		for(int i=0;i<3;++i){
			bt[i].SetMouse(GetMouse());
			CGUINormalButtonListener *p = new CGUINormalButtonListener;
			switch (i) {
			case 0:
				p->SetPlaneLoader(&loader,0);
				break;
			case 1:
				p->SetPlaneLoader(&loader,2);
				p->SetType(2);	//	���]����̃{�^��
				break;
			case 2:
				p->SetPlaneLoader(&loader,0);
				p->SetType(9);	//	��ɒu����Active�ɂȂ�{�^��
				break;
			}
			bt[i].SetEvent(smart_ptr<CGUIButtonEventListener>(p,true));
			bt[i].SetXY(100+i*150,200);
		}
	}

	while(IsThreadValid()) {					//	���ꂪValid�̊ԁA�܂�葱����
		GetKey()->Input();
		if (GetKey()->IsVKeyPush(0)) break;
		GetMouse()->Flush();					//	���t���[������Ń}�E�X���Q�b�g
		{
			//	�{�^���̕`��
			for(int i=0;i<3;++i){
				bt[i].OnDraw(GetDraw());
			}
		}
		//	�{�^���C�x���g�̃n���h�����O��P
		{
			CGUIButtonEventListener* e = bt[0].GetEvent();
			if (((CGUINormalButtonListener*)e)->IsLClick()){
				//	�{�^���O�������ꂽ�Ȃ�΁A�{�^���P�̃}�[�N�𔽓]
				e = (CGUIButtonEventListener*)bt[1].GetEvent();
				CGUINormalButtonListener* p = (CGUINormalButtonListener*)e;
				p->SetReverse(true);
			}
		}
		//	�{�^���C�x���g�̃n���h�����O��Q
		{
			if (bt[2].IsLClick()){	//	���X�i���o�R�����Ƀ{�^���̏�Ԃ��`�F�b�N�ł���I
				//	�{�^���O�������ꂽ�Ȃ�΁A�{�^���P�̃}�[�N�𐳏�ɖ߂�
				CGUIButtonEventListener* e = (CGUIButtonEventListener*)bt[1].GetEvent();
				CGUINormalButtonListener* p = (CGUINormalButtonListener*)e;
				p->SetReverse(false);
			}
		}

		//	�}�E�X���͂̕K�v������̂Ȃ�΁A�����ōs�Ȃ��΁A
		//	��̃{�^���C�x���g�ŋz�����ꂽ�}�E�X���b�Z�[�W�͂����ɓ͂��Ȃ��B

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
