#include "stdafx.h"
#include "CApp.h"

#include "CScnEffect.h"

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����
	GetDraw()->SetDisplay(false);

	//	�\�t�g�E�F�A�}�E�X�J�[�\��
	{
		smart_ptr<CPlaneBase> v(new CDIB32,true);
		v->Load("interface/common/mouse.yga");
		GetMouseLayer()->SetPlane(v,2,52);
	}

	//	���ꂪ�V�i���I���{�����邽�߂̕�̂ƂȂ�N���X
	CScenarioDraw scn;

	//	�e�� plug-in
	scn.SetTextDraw(smart_ptr<CTextDrawBase>(new CTextDrawDIB32A,true));
	scn.SetBGLoader(smart_ptr<CPlaneLoaderBasePre>(new CDIB32Loader,true));
	scn.SetBGMLoader(smart_ptr<CBGMLoader>(new CBGMLoader,true));
	scn.SetSELoader(smart_ptr<CSELoader>(new CSELoader,true));
	scn.SetSCLoader(smart_ptr<CPlaneLoaderBasePre>(new CDIB32Loader,true));
	scn.SetFaceLoader(smart_ptr<CPlaneLoaderBasePre>(new CDIB32Loader,true));
	scn.SetNameLoader(smart_ptr<CPlaneLoaderBasePre>(new CDIB32Loader,true));
	scn.SetButtonLoader(smart_ptr<CPlaneLoaderBasePre>(new CDIB32Loader,true));
	//	effect factory
	scn.SetScenarioEffectFactory(smart_ptr<CScenarioEffectFactory>(new CMyScenarioEffectFactory,true));

	//	�ȉ��̂Q�͏��L���͈ڂ��Ȃ�
	scn.SetKey(GetKey());
	scn.SetMouse(GetMouse());

	//	�ݒ�t�@�C��
	scn.SetConfigFile("interface/talk1/layout_define.txt");

	//	BGM�̓X�g���[���Đ������I
	scn.GetBGMLoader()->UseStreamSound(true);

	//	FPS�̐ݒ�
	CFPSTimer fps;
	fps.SetFPS(30);	//	�e�L�g�[��redraw���ĂȂ��Ⴂ�O�O�G

	//	scn.SetSkipFast(false);
	//	�������ݒ肷��΁A������{�^���𖳌��ɂł���

	//	�e�X�g���[�h
	const bool bTest = false;
	int nScn = 520;
ReScan:;
	
	{	//	���A�Đ����̂r�d�����~������
		scn.GetBGMLoader()->Stop();
		scn.GetSELoader()->ReleaseAll();
		scn.ReleaseCacheAll();
		scn.ResetEffect();
	}

	if (!bTest) {
		//	�Ώۃt�@�C���ǂݍ���
		GetMouseLayer()->Enable(false);
		CFileDialog filedlg;
		CHAR buf[MAX_PATH];
		if (filedlg.GetOpenFile(buf)!=0) return ;
		scn.Open(buf);
		GetMouseLayer()->Enable(true);
	} else {
		CHAR buf[MAX_PATH];
		::wsprintf(buf,"scn/scn%.3d.html",nScn);
		CDbg().Out(buf);
		scn.Open(buf);
		nScn++;
	}

	while (IsThreadValid()){

		//	���t���[���Ăяo���������X�g�O�O
		GetKey()->Input();
		GetMouse()->Flush();
		scn.GetSELoader()->OnPlay();

		if (GetKey()->IsVKeyPress(0)) break;	//	ESC�L�[�ŏI�������
//		GetDraw()->Clear();						//	BG�̃N���A
		scn.OnDraw(GetDraw()->GetSecondary());	//	�V�i���I��ʂ̕`��
		{
			HDC hdc = GetDraw()->GetSecondary()->GetDC();
			scn.OnDrawText(hdc);
			GetDraw()->GetSecondary()->ReleaseDC();
		}
		GetDraw()->OnDraw();			//	����ʂ֓]��
		if (scn.Input()) goto ReScan;	//	�L�[���͂��󂯕t���Ă���ɉ����Ĕ�������
		//	���@OnDraw�̂��Ƃōs�Ȃ�����
		fps.WaitFrame();				//	�t���[���҂�
	}

//	int p = 123;

}

//	���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
	virtual void MainThread(void){			//	���ꂪ���[�J�[�X���b�h
		CApp().Start();						//	CApp app; app.Start();�̈Ӗ���
	}
	//	�L���v�V�����ύX���Ƃ���Ƃǂ₳���O�O
	virtual LRESULT OnPreCreate(CWindowOption& opt){
		opt.caption		= "������n";
		opt.classname	= "YANESDKAPPLICATION";
		opt.size_x		= 640;
		opt.size_y		= 480;
		opt.style		= WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION;
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
