#include "stdafx.h"
#include "CApp.h"
#include "md5.h"
#include "resource.h"

void	CApp::MainThread(void)
{
	CFPSTimer timer;
	timer.SetFPS(10);

	//	��������C���v���ɂ���i�I������Ƃ��ɁA���̃E�B���h�D�����ׂĕ���j
	SetMainApp(true);

	CDialogHelper dialog(GetMyApp()->GetMyWindow());
	int nButtonID1 = dialog.HookButtonOnClick(IDC_BUTTON1);
	int nButtonID2 = dialog.HookButtonOnClick(IDC_BUTTON2);

	while(IsThreadValid()) {					//	���ꂪValid�̊ԁA�܂�葱����
		if (dialog.GetPoolInfo(nButtonID1)->isPool()){
		//	�{�^��������Ƃ�I
			dialog.GetPoolInfo(nButtonID1)->reset();
			//	�����������̃N���A
			Listing();
		}
		if (dialog.GetPoolInfo(nButtonID2)->isPool()){
		//	�{�^��������Ƃ�I
			dialog.GetPoolInfo(nButtonID2)->reset();
			//	�����������̃N���A
			Compare();
		}
		timer.WaitFrame();
	}
}

//	���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
	virtual void MainThread(void){			//	���ꂪ���[�J�[�X���b�h
		CApp().Start();						//	CApp app; app.Start();�̈Ӗ���
	}
	virtual LRESULT OnPreCreate(CWindowOption &opt){
		opt.dialog = MAKEINTRESOURCE(IDD_DIALOG1);	//	�_�C�A���O�Ȃ̂��I
		return 0;
	}
};

//	���킸�ƒm�ꂽWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	�K�������Ă�
	CSingleApp sapp;
	if (sapp.IsValid()) {
		CThreadManager::CreateThread(new CAppMainWindow());					//	��Œ�`�������C���̃E�B���h�D���쐬
		//	��Œ�`�������C���̃E�B���h�D���쐬
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////

void	CApp::Listing(){
	//	���X�g�A�b�v
	string src_path;
	{
		CFolderDialog fd;
		LRESULT ret = fd.GetOpenFolderName(src_path, CFile::GetCurrentDir().c_str(),
			"�t�@�C�����X�g�������o���t�H���_��I�����ĉ�����.\n"
			"�T�u�t�H���_�������X�e�B���O�Ώۂł�.\n"
			);
		if (ret==0){
			CStringScanner::Replace(src_path,"\\","/");
			if(*src_path.rbegin()!='/') { src_path += '/'; }
		} else {
			return;
		}
	}

	CDbg().Clear();
	CDbg().Out("�y���X�e�B���O���J�n���܂��z");
	{
		CDir dir;
		dir.SetPath(src_path);
		dir.SetFindFile("*");
		dir.EnablePackFile(false);
		dir.EnableSubdir(true);

		string filename;
		CFile file;
		file.Open("file_list.txt","w");
		const int nLength = src_path.length();	//	������������Ȃ���΂Ȃ�Ȃ�
		while(!dir.FindFile(filename)){
			string purename;
			purename = filename;
			purename.erase(0,nLength);

			DWORD dwSize;
			string hash;
			CalcMD5(filename,dwSize,hash);
			fprintf(file.GetFilePtr(),"%s\n\t%d\t%s\n",purename.c_str(),dwSize,hash.c_str());
			CDbg().Out("�@���� %s", purename.c_str());
		}
	}
	CDbg().Out("file_list.txt�Ɂy���X�e�B���O���I�����܂����z");
}

void	CApp::Compare(){	//	��r
	CFile filelist;
	if (filelist.Read("file_list.txt")!=0){
		CMsgDlg box;
		box.Out("�ُ�I��","file_list.txt��p�ӂ��Ă��������I");
		return ;
	}

	//	�R���y�A
	string src_path;
	{
		CFolderDialog fd;
		LRESULT ret = fd.GetOpenFolderName(src_path, CFile::GetCurrentDir().c_str(),
			"��r����t�H���_��I�����ĉ�����.\n"
			"�T�u�t�H���_������r�Ώۂł�.\n"
			);
		if (ret==0){
			CStringScanner::Replace(src_path,"\\","/");
			if(*src_path.rbegin()!='/') { src_path += '/'; }
		} else {
			return;
		}
	}

	CFile logfile;
	logfile.Open("compare_result.txt","w");

	CDbg().Clear();
	CDbg().Out("file_list.txt�Ɓy��r���J�n���܂��z");
	fprintf(logfile.GetFilePtr(), "%s\n", "�y��r���J�n���܂��z\n-----");
	{
		string line,line2;
		while(true){
			if (filelist.ReadLine(line)!=0) break;
			if (filelist.ReadLine(line2)!=0) break;

			string filename;
			filename = line; // ���ꂻ�̂܂܃t�@�C�����H
			filename = src_path + filename;
			fprintf(logfile.GetFilePtr(), "%s\n", filename.c_str());

			DWORD dwSize;
			string hash;
			LRESULT ret = CalcMD5(filename,dwSize,hash);
			if ( ret==1 ) {
				CDbg().Out("%s", filename.c_str());
				CDbg().Out("�@�~�I�[�v���Ɏ��s");
				fprintf(logfile.GetFilePtr(), "%s\n", "�@�~�I�[�v���Ɏ��s");
				continue ;
			}
			ef ( ret==2 ) {
				CDbg().Out("%s", filename.c_str());
				CDbg().Out("�@�~MD5�̎擾�Ɏ��s");
				fprintf(logfile.GetFilePtr(), "%s\n", "�@�~MD5�̎擾�Ɏ��s");
				continue ;
			}

			LPCSTR lp = line2.c_str();
			DWORD dwSize2;
			CStringScanner::GetNum(lp,*(int*)&dwSize2);
			CStringScanner::SkipSpace(lp);
			const string hash2 = CStringScanner::GetStr(lp);

			if (dwSize!=dwSize2 || hash!=hash2) {
				CDbg().Out("%s", filename.c_str());
				CDbg().Out("�@�~�s��v");
				fprintf(logfile.GetFilePtr(), "%s\n", "�@�~�s��v");
			} else {
				fprintf(logfile.GetFilePtr(), "%s\n", "�@����v");
			}
		}
	}

	CDbg().Out("�y��r���ʂ�compare_result.txt�ɏ������݂܂����z");
	CDbg().Out("�y��r���I�����܂����z");
	fprintf(logfile.GetFilePtr(), "%s\n", "-----\n�y��r���I�����܂����z");
}

LRESULT	CApp::CalcMD5(const string& filename, DWORD& dwSize, string& result){
	//	�t�@�C���J����
	CFile file;
	if ( file.Open(filename, "rb")!=0 ) {
		//	�t�@�C����Ȃ�Ƃ��Ⴄ�H
		dwSize = 0;
		result.clear();
		return 1;
	}

	//	�t�@�C���T�C�Y�𓾂�
	fseek(file.GetFilePtr(), 0, SEEK_END);
	dwSize = ftell(file.GetFilePtr());
	fseek(file.GetFilePtr(), 0, SEEK_SET);

	if ( dwSize!=0 ) {
		MD5 md5(file.GetFilePtr());	//	file�|�C���^���ړn����
		LPCSTR p = md5.hex_digest();
		result = p;
		delete [] p;	//	����delete����Ƃ�����Ƃ���..
	} else {
		result.clear();
	}
	return 0;
}
