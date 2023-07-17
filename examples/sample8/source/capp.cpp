#include "stdafx.h"
#include "resource.h"
#include "capp.h"
void	CApp::MainThread() {
/**
	�X���C�_�[�ƁA�h���b�v�_�E�����X�g�{�b�N�X�͖�����
	���߂�ɂ�

*/

	CFPSTimer timer;
	timer.SetFPS(10);

	//	��������C���v���ɂ���i�I������Ƃ��ɁA���̃E�B���h�D�����ׂĕ���j
	SetMainApp(true);

	CDialogHelper dialog(GetMyApp()->GetMyWindow());
	int nButtonID = dialog.HookButtonOnClick(IDC_BUTTON1);

	string s;
	CDIBitmap dib;
	dib.Load("1s.jpg");

	CDialogHelper::CListBoxInfo listbox[2];
	dialog.Attach(listbox[0],IDC_LIST1,true); // �ЂƂ̓��X�g�{�b�N�X
	dialog.Attach(listbox[1],IDC_COMBO1,false); // �ЂƂ̓R���{�{�b�N�X

	listbox[0].AddString("����ɂ�");			// 0
	listbox[0].AddString("�������I������I");	// 1
	listbox[0].AddString("�V�F�[�[�[�I�I�I");		// 2
	listbox[0].AddString("�킭����",1);			// ��1�Ȃ̂ŁA
		//	1�̒��O�A���Ȃ킿�u�������I������v�̒��O�̍s�ɑ}�������
//	listbox[0].SetCurSel(2);

	listbox[1].AddString("�肷�Ƃ���");
	listbox[1].AddString("�肷�ƂɁ[");

	while (IsThreadValid()){
		// ---	�_�C�A���O����������܂킷

		//	�G�f�B�b�g�{�b�N�X�̕�����擾
		string t = dialog.GetText(IDC_EDIT1);
		int r1 = dialog.GetCheck(IDC_RADIO1);
		int r2 = dialog.GetCheck(IDC_RADIO2);
		int r3 = dialog.GetCheck(IDC_CHECK1);
		dialog.SetText(IDC_EDIT2,"���W�I�{�^���̏�ԁ�"+CStringScanner::NumToString(r1)
			+":"+CStringScanner::NumToString(r2)
			+"\r\n���X�g�{�b�N�X�̏��"+CStringScanner::NumToString(listbox[0].GetCurSel())
			+"\r\n�R���{�{�b�N�X�̏��"+CStringScanner::NumToString(listbox[1].GetCurSel())
			+"\r\n"+t+s);
		dialog.SetText(IDC_STATIC1,"�`�F�b�N�{�^���̏�ԁ�"+CStringScanner::NumToString(r3));
		if (dialog.GetPoolInfo(nButtonID)->isPool()){
		//	�{�^��������Ƃ�I
			dialog.GetPoolInfo(nButtonID)->reset();
			//	�����������̃N���A
			s += "����";
		}
		//	�s�N�`���[�R���g���[���ɉ摜��`��
		{
			HWND h = dialog.GetHWnd(IDC_STATIC2);
			HDC dc = ::GetDC(h);
			::BitBlt(dc,0,0,100,75,dib.GetDC(),0,0,SRCCOPY);
			::ReleaseDC(h,dc);
		}

		timer.WaitFrame();
	}
}

//	���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
	virtual void MainThread(){			   //  ���ꂪ���[�J�[�X���b�h
		CApp().Start();
	}
	virtual LRESULT OnPreCreate(CWindowOption &opt){
		opt.dialog = MAKEINTRESOURCE(IDD_DIALOG1);	//	�_�C�A���O�Ȃ̂��I
		return 0;
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
			CThreadManager::CreateThread(new CAppMainWindow);
		}
		//	������CAppInitializer���X�R�[�v�A�E�g����̂����A���̂Ƃ���
		//	���ׂẴX���b�h�̏I����҂��ƂɂȂ�
	}
	return 0;
}
