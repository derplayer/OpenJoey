#include "stdafx.h"
#include "CApp.h"

//	--------------   �V�[���̂�������   -------------------------

//	1.�V�[������enum����
enum SCENE {	//	�V�[����
	SCENE1,SCENE2,SCENE3
};

//	2.�x�[�X�V�[������� (CScene��mediator���瑽�d�p������)
class CBaseScene : public CScene , public mediator<CApp> {
};	//	����Ŋ����O�O

//	3.�V�[������� ( �x�[�X�V�[������h�������� )
class CScene1 : public CBaseScene {
	//	�������́AOnInit���I�[�o�[���[�h���čs�Ȃ��B���̂Ȃ���outer���g����B
	virtual void OnInit() {
		dib.Load("data/1.jpg");
		text.GetFont()->SetText("�X�y�[�X�L�[�ŃV�[���Q�փW�����v\n���^�[���L�[�ŃV�[���P���X�^�b�N�ɐς�ŃV�[���R�փW�����v");
		text.SetPos(50,200);
		outer.GetKey()->ResetKey();
		//	���L�[���͂̓��Z�b�g���Ȃ��ƁA�L�[��������Ĕ��ł����̂Ȃ��d���͂ɂȂ�
	}
	virtual void OnDraw(CPlaneBase*lp){
		//	���@outer���ʎq�ŁACApp�N���X�ɃA�N�Z�X�ł���I
		if (outer.GetKey()->IsVKeyPush(5)) {
			outer.GetTransiter()->Backup(lp);
			outer.GetTransiter()->BeginTransit(19);
			JumpScene(SCENE2);
			return ;
		}
		if (outer.GetKey()->IsVKeyPush(6)) {
			outer.GetTransiter()->Backup(lp);
			outer.GetTransiter()->BeginTransit(25);
			PushScene(SCENE1);	//	PushScene + JumpScene == CallScene
			JumpScene(SCENE3);
		}

		lp->ClearRect();
		lp->BltFast(&dib,0,0);
	}	
	CDIB32 dib;
	CTextLayer text;
};
class CScene2 : public CBaseScene {
	virtual void OnInit() {
		dib.Load("data/2.jpg");
		text.GetFont()->SetText("�X�y�[�X�L�[�ŃV�[���P�փW�����v\n���^�[���L�[�ŃV�[���R���R�[��");
		text.SetPos(50,200);
		outer.GetKey()->ResetKey();
		//	���L�[���͂̓��Z�b�g���Ȃ��ƁA�L�[��������Ĕ��ł����̂Ȃ��d���͂ɂȂ�
	}
	virtual void OnDraw(CPlaneBase*lp){
		//	���@outer���ʎq�ŁACApp�N���X�ɃA�N�Z�X�ł���I
		if (outer.GetKey()->IsVKeyPush(5)) {
			outer.GetTransiter()->Backup(lp);
			outer.GetTransiter()->BeginTransit(21);
			JumpScene(SCENE1);
			return ;
		}
		if (outer.GetKey()->IsVKeyPush(6)) {
			outer.GetTransiter()->Backup(lp);
			outer.GetTransiter()->BeginTransit(22);
			CallScene(SCENE3);
			return ;
		}

		lp->ClearRect();
		lp->BltFast(&dib,0,0);
	}	
	CDIB32 dib;
	CTextLayer text;
};
class CScene3 : public CBaseScene {
	virtual void OnInit() {
		dib.Load("data/3.jpg");
		text.GetFont()->SetText("�X�y�[�X�L�[�Ń��^�[��\n���^�[���L�[�ŏI��");
		text.SetPos(50,200);
		outer.GetKey()->ResetKey();
		//	���L�[���͂̓��Z�b�g���Ȃ��ƁA�L�[��������Ĕ��ł����̂Ȃ��d���͂ɂȂ�
	}
	virtual void OnDraw(CPlaneBase*lp){
		if (outer.GetKey()->IsVKeyPush(5)) {
			outer.GetTransiter()->Backup(lp);
			outer.GetTransiter()->BeginTransit(23);
			ReturnScene();
			return ;
		}
		if (outer.GetKey()->IsVKeyPush(6)) {
			ExitScene();
			return ;
		}

		lp->ClearRect();
		lp->BltFast(&dib,0,0);
	}	
	CDIB32 dib;
	CTextLayer text;
};

//	4.��̂ɑΉ�����factory������
class CMySceneFactory : public CSceneFactory,public mediator<CApp> {
public:
	CMySceneFactory(CApp* pv) : mediator<CApp>(pv){}
protected:
	virtual CScene* CreateScene(int nScene) {
		CBaseScene* lp;
		switch ((SCENE)nScene){
		case SCENE1: lp = new CScene1; break;
		case SCENE2: lp = new CScene2; break;
		case SCENE3: lp = new CScene3; break;
		default:	 lp = NULL;	// error..
		}
		lp->SetOutClass(&outer);
		return lp;
	}
};
/////////////////////////////////////////////////

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����

	GetDraw()->SetDisplay(false);				//	Window���[�h

	CFPSTimer t;
	t.SetFPS(30);

	//	5.�V�[���R���g���[���[��SceneFactory��n���Ă��
	CSceneControl sc(smart_ptr<CSceneFactory>(new CMySceneFactory(this),true));
	sc.JumpScene(SCENE1);

	//	6.�g�����W�b�^���g���Ȃ�΁A�g�����W�b�^��CSceneControl��n���Ă��
	GetTransiter()->SetSceneControl(&sc);

	while(IsThreadValid()) {					//	���ꂪValid�̊ԁA�܂�葱����
		GetKey()->Input();
		if (GetKey()->IsVKeyPush(0)) break;

		//	7.����ŃV�[���R���g���[���[�𒲂ׁA�Ԃ�l����O�Ȃ烋�[�v���甲����悤�ɂ���
	//	if (sc.OnDraw(GetDraw()->GetSecondary())) break;
		if (GetTransiter()->OnDraw(GetDraw()->GetSecondary())) break;

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
