#include "stdafx.h"

class CApp : public CThread {
  virtual void ThreadProc(){

	CFPSTimer timer;
	timer.SetFPS(10);

	CKey1 key(true);	//	�o�b�N�O���E���h����

	while (IsThreadValid()){

		key.Input();
		if (key.IsKeyPush(0))
			break;
		//	�X�y�[�X�L�[�������H
		if (key.IsKeyPush(5)){
			//	��ʃT�C�Y���擾
			int sx = ::GetSystemMetrics(SM_CXSCREEN);
			int sy = ::GetSystemMetrics(SM_CYSCREEN);
			CDIBitmap bmp;
			bmp.CreateSurface(sx,sy,24);

			HDC hdc1 = bmp.GetDC();
			if (hdc1!=NULL){
				HDC hdc2 = ::GetDC(NULL); // �f�X�N�g�b�v��DC���擾
				if (hdc2!=NULL){
					::BitBlt(hdc1,0,0,sx,sy,hdc2,0,0,SRCCOPY);
					::ReleaseDC(NULL,hdc2);
				}
			}

			/*	//	�ʏ�̃Z�J���_���T�[�t�F�[�X����]������ꍇ
			bmp.GetSurfaceInfo()->GeneralBlt(CSurfaceInfo::eSurfaceBltFast,
				GetDraw()->GetSecondary()->GetSurfaceInfo(),&CSurfaceInfo::CBltInfo(),0);
			*/
			bmp.Save("���.bmp");
		}
		timer.WaitFrame();
	}
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

		CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
		//	���K�������Ă�

		CSingleApp sapp;
		if (sapp.IsValid()) {
			CThreadManager::CreateThread(new CApp);
		}
		//	������CAppInitializer���X�R�[�v�A�E�g����̂����A���̂Ƃ���
		//	���ׂẴX���b�h�̏I����҂��ƂɂȂ�
	}
	return 0;
}
