#include "stdafx.h"

class CApp : public CThread {
public:
	void ThreadProc(){
		//�@���W�X�g���ݒ�`�擾
		CRegistry reg("Software\\yaneu");

		string str;
		str = "�Ă��Ă�";
		reg.SetValue("folder1","moji",str);
		str.erase(); // �������Ă��火�����Đ������������Ă��邩���e�X�g
		if (reg.GetValue("folder1","moji",str)==0){
			CDbg().Out(str);
		}

		DWORD dw1 = 89383; // ���₭��������H
		reg.SetValue("folder1","dword",dw1);
		dw1 = 0;
		if (reg.GetValue("folder1","dword",dw1)==0){
			CDbg().Out(dw1);
		}

		//	���[�U�[��`�f�[�^�̏����o��
		DWORD dw[3];
		dw[0] = 123;
		dw[1] = 234;
		dw[2] = 345;
		int nSize =3*sizeof(DWORD);
		reg.SetValue("folder1","dataex",(BYTE*)&dw,nSize);
		dw[0] = 0;
		dw[1] = 0;
		dw[2] = 0;
//		nSize = 4;	//	�����ۂ̃f�[�^�T�C�Y�ƈقȂ�Ƃ��̗v�����̓G���[�ɂȂ�
		if (reg.GetValue("folder1","dataex",(BYTE*)&dw,nSize)==0){
			int n1 = dw[0];
			int n2 = dw[1];
			int n3 = dw[2];
			CDbg().Out("data = %d,%d,%d  size = %d",n1,n2,n3,nSize);
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
