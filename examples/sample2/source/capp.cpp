#include "stdafx.h"
#include "capp.h"

void	CApp::MainThread() {
	GetDraw()->SetDisplay();

	CKey1 key;
	CFPSTimer timer;
	timer.SetFPS(15);

	//	��������C���v���ɂ���i�I������Ƃ��ɁA���̃E�B���h�D�����ׂĕ���j
	SetMainApp(true);

	CPlane bgplane;
	bgplane->Load("AA076_640480.jpg");

	CPlane charaplane;
	charaplane->Load("������640_480.yga");

	CRootCounter nFade(0, 255, 8);
	CRootCounter nPhase(0, 6, 1);

	CTextFastPlane* pText = new CTextFastPlane;
	pText->GetFont()->SetText("�X�y�[�X�L�[�������Ǝ��̃t�F�[�Y��");
	pText->GetFont()->SetSize(30);
	pText->UpdateTextAA();
	CPlane text(pText);

	while (IsThreadValid()){
		ISurface* pSecondary = GetDraw()->GetSecondary();

		pSecondary->Clear();
		//	�K��BG����ʑS�̂ɑ΂��ē]������Ȃ�΃N���A�͂��Ȃ��Ƃ��ǂ�

		//	�t�F�[�Y���ƂɈقȂ�`��
		switch (nPhase){
		case 0: {
			pSecondary->BltFast(bgplane,0,0);
			//	�����F�����]����BltFast��p����
			pSecondary->BltNatural(charaplane,0,0);
			//	�����F�L���]����yga�摜(�������̉摜)�̓]����
			//	BltNatural��p����̂��킩��₷��
			break;
				}
		case 1: {
			pSecondary->BltNatural(charaplane,0,0);
			// �L�������ɕ\�������āA
			pSecondary->BlendBltFast(bgplane,0,0,255-nFade);
			//	���̂��Ƃ�BG�����������ĕ`��I
			nFade++;
			break;
				}
		case 2: {
			pSecondary->BltFast(bgplane,0,0);
			//	�����F�����]����BltFast��p����
			int sx,sy;
			charaplane->GetSize(sx,sy);	//	�T�[�t�F�[�X�T�C�Y�̎擾
			//	��ʒ��S���x�[�X�|�C���g�Ƃ��āA�]�����Ă݂�
			SIZE dstsize = { sx*nFade>>8 , sy * nFade>>8 };
			pSecondary->BltNatural(charaplane,sx/2,sy/2,nFade/2+128,&dstsize,NULL,NULL,4);
			//	���̏ꍇ�A�Ō�̃p�����[�^�Ŏw�肵�Ă���4�Ƃ����̂́A�摜���S���N�_�Ƃ���
			//	���W���w�肷�����
			nFade++;
			break;
				}
		case 3: {
			pSecondary->BltFast(bgplane,0,0);
			int sx,sy;
			charaplane->GetSize(sx,sy);	//	�T�[�t�F�[�X�T�C�Y�̎擾
			SIZE dstsize = { sx*nFade>>8 , sy * nFade>>8 };
			pSecondary->AddColorBltFast(charaplane,sx/2,sy/2,&dstsize,NULL,NULL,4);
			//	�����T�[�t�F�[�X�����AddColor(���F����)
			nFade++;
			break;
				}
		case 4: {
			pSecondary->BltFast(bgplane,0,0);
			int sx,sy;
			charaplane->GetSize(sx,sy);	//	�T�[�t�F�[�X�T�C�Y�̎擾
			SIZE dstsize = { sx*nFade>>8 , sy * nFade>>8 };
			pSecondary->SubColorBltFast(charaplane,sx/2,sy/2,&dstsize,NULL,NULL,4);
			//	�����T�[�t�F�[�X�����SubColor(���F����)
			nFade++;
			break;
				}
		case 5: {
			pSecondary->BltFast(bgplane,0,0);
			int sx,sy;
			charaplane->GetSize(sx,sy);	//	�T�[�t�F�[�X�T�C�Y�̎擾
			SIZE dstsize = { sx*nFade>>8 , sy * nFade>>8 };
			pSecondary->AddColorBltFastFade(charaplane,sx/2,sy/2,nFade,&dstsize,NULL,NULL,4);
			//	�����T�[�t�F�[�X�����AddColor(���F����)+Fade(�����w��)
			nFade++;
			break;
				}
		case 6: {
			pSecondary->BltFast(bgplane,0,0);
			int sx,sy;
			charaplane->GetSize(sx,sy);	//	�T�[�t�F�[�X�T�C�Y�̎擾
			SIZE dstsize = { sx*nFade>>8 , sy * nFade>>8 };
			pSecondary->SubColorBltFastFade(charaplane,sx/2,sy/2,nFade,&dstsize,NULL,NULL,4);
			//	�����T�[�t�F�[�X�����SubColor(���F����)+Fade(�����w��)
			nFade++;
			break;
				}
		}

		pSecondary->BltNatural(text,20,400);

		GetDraw()->OnDraw();

		key.Input();
		if (key.IsKeyPush(0))	//	ESC�L�[�ŏI��
			break;
		if (key.IsKeyPush(5)) {	//	SPACE�L�[�������ƃt�F�[�Y���C���N�������g
			nPhase++;
			nFade.Reset();
		}

		timer.WaitFrame();
	}
}

//	���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
	virtual void MainThread(){			   //  ���ꂪ���[�J�[�X���b�h
		CApp().Start();
	}
};

//	���킸�ƒm�ꂽWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	{
		//*
		{	//	�G���[���O���t�@�C���ɏo�͂���̂�I
			CTextOutputStreamFile* p = new CTextOutputStreamFile;
			p->SetFileName("Error.txt");
			Err.SelectDevice(smart_ptr<ITextOutputStream>(p));
		}
		//*/

		CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
		//	���K�������Ă�

		CSingleApp sapp;
		if (sapp.IsValid()) {
			CThreadManager::CreateThread(new CAppMainWindow);
			//	��Œ�`�������C���̃E�B���h�D���쐬
//			CThreadManager::CreateThread(new CAppMainWindow);
//			�����������ƁA�����E�B���h�D�����������̂�

		}
		//	������CAppInitializer���X�R�[�v�A�E�g����̂����A���̂Ƃ���
		//	���ׂẴX���b�h�̏I����҂��ƂɂȂ�
	}
	return 0;
}
