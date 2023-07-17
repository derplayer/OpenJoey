#include "stdafx.h"
#include "CApp.h"

class CMyRegionHook : public CRegionHook {
public:
	CMyRegionHook(CDIB32*lp):CRegionHook(lp) {}
protected:
	//	�h���b�O�\�̈�̐ݒ�
	virtual	bool MustBeDrag(const POINT& pt){
		if (pt.y<100) {
			CMsgDlg().Out("�G������_���ɂ�I","��͌��I�����Ɖ��I");
			return false;
		} ef (pt.y<250) {
			CMsgDlg().Out("�G������_���ɂ�I","����ȂƂ���G��Ȃ��ŃG�b�`�I�����Ɖ��I");
			return false;
		} ef (pt.y>300) {
			CMsgDlg().Out("�G������_���ɂ�I","���Ȃ��ϑԁI�H�����Ə�I");
			return false;
		} ef (pt.x<130) {
			CMsgDlg().Out("�G������_���ɂ�I","�n���I�����ƉE��I�킩��Ȃ��́H");
			return false;
		} ef (pt.x>150) {
			CMsgDlg().Out("�G������_���ɂ�I","�n���I�����ƍ���A���I");
			return false;
		} else {
			return true;	//	�h���b�O�\
		}
	}
};

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����
	CDIB32 pp;
	pp.UseDIBSection(true);
	pp.Load("data/aoisora.bmp");
	CMyRegionHook rgn(&pp);	//	���̃��[�W�����ɂ���̂�
	
	{	//	�Z�J���_���o�b�t�@�́A�r�b�g�}�b�v�T�C�Y�����p�ӂ���
		int sx,sy;
		pp.GetSize(sx,sy);
		GetDraw()->SetDisplay(false,sx,sy);		//	Window���[�h
		//	�����ŏ��߂ăE�B���h�D��\������
	}

	CFPSTimer fps;
	fps.SetFPS(5);	//	�e�L�g�[��redraw���ĂȂ��Ⴂ�O�O�G
	CKey key;
	CRootCounter r(128,255,16);
	r.SetReverse(true);
	r.SetInit(255);
	while (IsThreadValid()){
		key.Input();
		if (key.IsVKeyPress(0)) break;	//	ESC�L�[�ŏI�������
		GetDraw()->Clear(DIB32RGB(255,255,255));
		r.Inc();
		GetDraw()->BlendBltFast(&pp,0,0,DIB32RGB(255-r,255-r,255-r),DIB32RGB(r,r,r));
		GetDraw()->OnDraw();
		fps.WaitFrame();
	}
}

//	���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
	virtual void MainThread(void){			//	���ꂪ���[�J�[�X���b�h
		CApp().Start();						//	CApp app; app.Start();�̈Ӗ���
	}
	virtual LRESULT OnPreCreate(CWindowOption& opt){
		opt.caption = "�����";
		opt.classname="�����";
		//	�ŏ��̓E�C���h�D���o���Ȃ�
		opt.size_x	= 0;
		opt.size_y	= 0;
		opt.style	= WS_POPUP;
		return 0;
	}
};

//	���킸�ƒm�ꂽWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CAppInitializer::Init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	�K�������Ă�
//	CSingleApp sapp;
//	if (sapp.IsValid()) {
		CAppMainWindow().Run();					//	��Œ�`�������C���̃E�B���h�D���쐬
//	}
	return 0;
}
