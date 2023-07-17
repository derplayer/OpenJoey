#include "stdafx.h"
#include "CApp.h"

//	--------------   �V�[���̂�������   -------------------------

//	1.�V�[������enum����
enum SCENE {	//	�V�[����
	SCENE1,SCENE2,SCENE3,
	SCENE_ISEND, // �I�����܂����H�V�[��
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
		text.SetPos(50,50);
		text2.GetFont()->SetText("�E��́~�{�^���ŃE�B���h�D����Ă݂Ă�������");
		text2.SetPos(50,130);
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
	CTextLayer text2;
};
class CScene2 : public CBaseScene {
	virtual void OnInit() {
		dib.Load("data/2.jpg");
		text.GetFont()->SetText("�X�y�[�X�L�[�ŃV�[���P�փW�����v\n���^�[���L�[�ŃV�[���R���R�[��");
		text.SetPos(50,50);
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
		text.SetPos(50,50);
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

//	YesNo�̊m�F�V�[��
class CSceneYesNo : public CBaseScene {
public:
	virtual void OnInit();
	virtual void OnDraw(CPlaneBase*lp);

	CPlaneLoaderBasePre* GetPlaneLoader() { return &m_vPlaneLoader;}

//	void	SetExitFlag(bool b) { m_bOverwrite = !b; }

	CSceneYesNo() { /* m_bOverwrite = true;*/ }

private:
	CDIB32Loader m_vPlaneLoader;
	CGUIButton	 m_vButton[2];	//	�{�^�����Q��
	int			 m_nButton;		//	�{�^�������ꂽ���
	CRootCounter m_nButtonCount;//�@�t�F�[�h�J�E���^
	smart_ptr<CPlaneBase> m_vPlane; // �a�f
//	bool		m_bOverwrite;	//	�㏑�����܂����H�̃V�[�����H
//	int			m_nType;		//	�V�[���̃^�C�v
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
		case SCENE_ISEND: lp = new CSceneYesNo; break;
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
		GetMouse()->Flush();
		if (GetKey()->IsVKeyPush(0)) break;

		//	7.����ŃV�[���R���g���[���[�𒲂ׁA�Ԃ�l����O�Ȃ烋�[�v���甲����悤�ɂ���
	//	if (sc.OnDraw(GetDraw()->GetSecondary())) break;

		//	WM_CLOSE�ł�������Ă�̂��H�O�O
		if (m_bWindowClosing) {
			//	�I�����܂����V�[���Ƃ��Ⴄ�Ȃ�A�I�����܂����V�[���ɔ�΂�
			if (sc.GetSceneNo()!=SCENE_ISEND){
				GetTransiter()->Backup(GetDraw()->GetSecondary(),3);
				sc.CallSceneFast(SCENE_ISEND);
				//	�g�����W�b�^�[�͒�~������
				GetTransiter()->StopTransit();
			}
			m_bWindowClosing = false;
		}

		if (GetTransiter()->OnDraw(GetDraw()->GetSecondary())) break;

		GetDraw()->OnDraw();
		t.WaitFrame();
	}
}

LRESULT	CApp::OnPreClose(void){
	m_bWindowClosing = true;
	//	�P��Ԃ���Close��j�~
	return 1;	
}

//	���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
public:
	CAppMainWindow() {
		m_lpApp = NULL;
	}
protected:
	virtual void MainThread(void){			//	���ꂪ���[�J�[�X���b�h
		m_lpApp = new CApp;		//	m_lpApp�́A���̊Ԃł��������Ă��Ă̓}�Y�C
		m_lpApp->Start();
		DELETE_SAFE(m_lpApp);
	}
	virtual LRESULT OnPreClose(){
		//	WM_CLOSE�n���h��
		if (m_lpApp!=NULL)
			return m_lpApp->OnPreClose();
		return 0;
	}
	CApp*	m_lpApp;
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

/////////////////////////////////////////////////////////
//	�I���V�[�� from ������n�O�O�G
/////////////////////////////////////////////////////////

void CSceneYesNo::OnInit(){

	outer.GetKey()->ResetKey();

	//	�g�����W�b�^�[�ɋL�����ꂽ�v���[���̃R�s�[
	m_vPlane = outer.GetTransiter()->GetPlane(3);
	outer.GetTransiter()->ReleasePlane(3); // �R�s�[�����̂ŏ����ėǂ�

	if (/*!m_bOverwrite*/true) {
		//	�I�����܂����{�b�N�X
		((CDIB32*)(CPlaneBase*)m_vPlane)->SubColorFast(PlaneRGB(172,172,172));
		//	�g�����W�V���������D�悷��V�[��
		SetPrecedentScene(true);
//		m_nType = 0;
	} else {
//		m_nType = GetSceneParam()[0]+1;
	}

	//	PlaneLoader�ɓǂݍ���
	m_vPlaneLoader.Set("data/yesno/list.txt");

	//	�{�^���̐ݒ�
	{
		for(int i=0;i<2;++i){
			m_vButton[i].SetMouse(outer.GetMouse());
			CGUINormalButtonListener *p = new CGUINormalButtonListener;
			p->SetPlaneLoader(&m_vPlaneLoader,i*3);
			p->SetType(2);	//	���]����̃{�^��
			m_vButton[i].SetEvent(smart_ptr<CGUIButtonEventListener>(p,true));
			m_vButton[i].SetXY(216+i*120,240);
		}
	}

	m_nButton = 0;
	m_nButtonCount.Set(0,16);
}

void CSceneYesNo::OnDraw(CPlaneBase*lpPlane){
	//	lpPlane->ClearRect();	//	���̃V�[���̓T�u�V�[���Ȃ̂ła�f�̓N���A���Ȃ�
	lpPlane->BltFast(m_vPlane,0,0);

	//	�����ɃZ���^�����O���ĕ\��
	{
		int n;
		switch (/*m_nType*/ 0){
		case 0: n = 6; break; // �I�����܂����H
		case 1: n = 7; break; // �㏑�����܂����HYes/No
		case 2: n = 8; break; // �I�Ԃ񂩁H
		default : WARNING(true,"�p�����[�^�����Ɠn���`");
		}
		lpPlane->BltNaturalPos(m_vPlaneLoader.GetPlaneBase(n),320,240,0);
	}

	//	�{�^���̕\��
	{
		for(int i=0;i<2;++i){
			m_vButton[i].OnDraw(lpPlane);
		}
	}

	//	�{�^�������ꂽ�񂩁H
	{
		for(int i=0;i<2;++i){
			if (m_nButton==0 && m_vButton[i].IsLClick()) { // Yes
				m_nButton = i+1;
				CGUIButtonEventListener* e	= m_vButton[i].GetEvent();
				CGUINormalButtonListener* p	= (CGUINormalButtonListener*)e;
				p->SetType(32);
				p->SetImageOffset(2);
			}
		}
	}

	//	�{�^��������Ă���A������Ƒ҂��Ă��������
	if (m_nButton!=0) {
		m_nButtonCount++;
		if (m_nButtonCount.IsLapAround()){
			//	�����I�����܂����_�C�A���O�������Ȃ��
			if (/*!m_bOverwrite*/true) {
				if (m_nButton == 1) { // Yes?
					outer.Close();	//	Window�����悤�ɗv��
				}
			} else {
				GetSceneParam()[0] = 2-m_nButton;
			}
			outer.GetMouse()->ResetButton();
			//	�g�����W�b�^�[���ĊJ������
			outer.GetTransiter()->RestartTransit();
			ReturnScene(); return ;
		}
	}
}
