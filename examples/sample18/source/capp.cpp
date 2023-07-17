#include "stdafx.h"

#include "capp.h"

void	CApp::MainThread() {

	GetDraw()->SetDisplay();

	CFPSTimer timer;
	timer.SetFPS(10);

	//	��������C���v���ɂ���i�I������Ƃ��ɁA���̃E�B���h�D�����ׂĕ���j
	SetMainApp(true);

	//------- sound�Đ��̃T���v�� -----------

	//	1.mp3���X�g���[���Đ��ōĐ�����T���v��
	CSound mysound;					//	CSound��p��(����̎��̂�smart_ptr<ISound>
	mysound.Load("sound.mp3");		//	�t�@�C�������w�肵�ēǂݍ���
	mysound->Play();				//	�Đ�����Ȃ��
	//	���̏ꍇ�Amp3�����܂�ɒ�����static buffer�ɓ���Ȃ��̂ŁA
	//	�Đ��ł��Ȃ��\��������B(6M���炢��MP3�t�@�C���͂܂��Đ�����Ȃ�)
	mysound->Stop();
	//	���Đ����~�߂�ɂ�Stop���Ăяo��

	//	2.�X�g���[���Đ��̏ꍇ
	soundFactory_.SetStreamPlay(true);
	mysound.Load("sound.mp3");		//	�t�@�C�������w�肵�ēǂݍ���
	mysound->Play();				//	�Đ�����Ȃ��
	//	�X�g���[���Đ��Ȃ̂ōĐ��ł���(�Ǝv����)
	mysound->Stop();
	//	���Đ����~�߂�ɂ�Stop���Ăяo��

	//	3.SoundLoader��p���čĐ�������ꍇ
	CSoundLoader loader;
	loader.SetStreamPlay(true);
	//	���X�g���[���Đ����ǂ������ACSoundFactory����������I�Ɏw�肷�邱�Ƃ��o����
	//	(���̃����o���Ăяo���Đݒ肵���ꍇ�ACSoundFactory�Ŏw�肵�����̂��D�悳���)

	loader.SetSoundFactory(smart_ptr<ISoundFactory>(&soundFactory_,false));
	//	��SoundFactory��ݒ肵�Ă���
	loader.Set("sound.cfg.txt",true);
	//	���t�@�C������ݒ�t�@�C����ǂݍ��܂���
	string s = loader.GetFileName(3);
	//	���ԍ����w�肵�ăt�@�C�������擾�ł���@�\
	//	CDbg().Out(s);
	CSound sound = loader.GetSound(7);
	//	��GetSound����΁AISound�h���N���X�̃C���X�^���X����������A
	//	�t�@�C������̓ǂݍ��݂��s�Ȃ���
	sound->Play();
	//	Play
	sound->Stop();
	//	��~
	sound->Close();
	//	���
	sound = loader.GetSound(3);
	//	����Sound���擾����
	//	�ȉ��A��������āASound�����낢��Đ��ł���B


	//	4.SELoader��p����ꍇ
	{
	CSELoader loader;

	CDefaultCacheStaleListener* listener = new CDefaultCacheStaleListener;
	listener->SetMax(10); // 10�z���ēǂݍ��񂾏ꍇ�A�Â����̂��������Ă���
	loader.SetCacheStaleListener(smart_ptr<ICacheStaleListener>(listener));

	loader.SetSoundFactory(smart_ptr<ISoundFactory>(&soundFactory_,false));
	loader.Set("se.cfg.txt",true);

	CSound sound = loader.GetSound(10);
	sound->Play();
	}

	//	5.PlaneLoader��p����ꍇ
	CPlaneLoader planeloader;
	planeloader.Set("plane.cfg.txt",true);

	while (IsThreadValid()){
		ISurface* pSecondary = GetDraw()->GetSecondary();

		//	PlaneLoader����v���[����������Ă����`�悷��ꍇ
		CPlane plane = planeloader.GetPlane(5);
		pSecondary->BltFast(plane,0,0);

		GetDraw()->OnDraw();

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
		/*
		{	//	�G���[���O���t�@�C���ɏo�͂���̂�I
			CTextOutputStreamFile* p = new CTextOutputStreamFile;
			p->SetFileName("Error.txt");
			Err.SelectDevice(smart_ptr<ITextOutputStream>(p));
		}
		*/

		CSingleApp sapp;
		//	��������ɏ����Ȃ��ƁA�X���b�h���N������
		//	CAppInitializer�̃f�X�g���N�^�őS�X���b�h�̏I����҂悤�ȏꍇ
		//	CSingleApp�̃f�X�g���N�^����ɋN�����Ă��܂��̂ŁA�܂����̂��D�D

		CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
		//	���K�������Ă�

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
