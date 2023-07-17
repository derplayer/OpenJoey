#include "stdafx.h"
#include "CApp.h"
#include "md5.h"
#include "resource.h"

void	CApp::MainThread(void)
{
	CFPSTimer timer;
	timer.SetFPS(10);

	//	これをメインプリにする（終了するときに、他のウィンドゥをすべて閉じる）
	SetMainApp(true);

	CDialogHelper dialog(GetMyApp()->GetMyWindow());
	int nButtonID1 = dialog.HookButtonOnClick(IDC_BUTTON1);
	int nButtonID2 = dialog.HookButtonOnClick(IDC_BUTTON2);

	while(IsThreadValid()) {					//	これがValidの間、まわり続ける
		if (dialog.GetPoolInfo(nButtonID1)->isPool()){
		//	ボタンおされとる！
			dialog.GetPoolInfo(nButtonID1)->reset();
			//	押し下げ情報のクリア
			Listing();
		}
		if (dialog.GetPoolInfo(nButtonID2)->isPool()){
		//	ボタンおされとる！
			dialog.GetPoolInfo(nButtonID2)->reset();
			//	押し下げ情報のクリア
			Compare();
		}
		timer.WaitFrame();
	}
}

//	これがmain windowのためのクラス。
class CAppMainWindow : public CAppBase {	//	アプリケーションクラスから派生
	virtual void MainThread(void){			//	これがワーカースレッド
		CApp().Start();						//	CApp app; app.Start();の意味ね
	}
	virtual LRESULT OnPreCreate(CWindowOption &opt){
		opt.dialog = MAKEINTRESOURCE(IDD_DIALOG1);	//	ダイアログなのだ！
		return 0;
	}
};

//	言わずと知れたWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	必ず書いてね
	CSingleApp sapp;
	if (sapp.IsValid()) {
		CThreadManager::CreateThread(new CAppMainWindow());					//	上で定義したメインのウィンドゥを作成
		//	上で定義したメインのウィンドゥを作成
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////

void	CApp::Listing(){
	//	リストアップ
	string src_path;
	{
		CFolderDialog fd;
		LRESULT ret = fd.GetOpenFolderName(src_path, CFile::GetCurrentDir().c_str(),
			"ファイルリストを書き出すフォルダを選択して下さい.\n"
			"サブフォルダ中もリスティング対象です.\n"
			);
		if (ret==0){
			CStringScanner::Replace(src_path,"\\","/");
			if(*src_path.rbegin()!='/') { src_path += '/'; }
		} else {
			return;
		}
	}

	CDbg().Clear();
	CDbg().Out("【リスティングを開始します】");
	{
		CDir dir;
		dir.SetPath(src_path);
		dir.SetFindFile("*");
		dir.EnablePackFile(false);
		dir.EnableSubdir(true);

		string filename;
		CFile file;
		file.Open("file_list.txt","w");
		const int nLength = src_path.length();	//	これを除去しなければならない
		while(!dir.FindFile(filename)){
			string purename;
			purename = filename;
			purename.erase(0,nLength);

			DWORD dwSize;
			string hash;
			CalcMD5(filename,dwSize,hash);
			fprintf(file.GetFilePtr(),"%s\n\t%d\t%s\n",purename.c_str(),dwSize,hash.c_str());
			CDbg().Out("　完了 %s", purename.c_str());
		}
	}
	CDbg().Out("file_list.txtに【リスティングが終了しました】");
}

void	CApp::Compare(){	//	比較
	CFile filelist;
	if (filelist.Read("file_list.txt")!=0){
		CMsgDlg box;
		box.Out("異常終了","file_list.txtを用意してください！");
		return ;
	}

	//	コンペア
	string src_path;
	{
		CFolderDialog fd;
		LRESULT ret = fd.GetOpenFolderName(src_path, CFile::GetCurrentDir().c_str(),
			"比較するフォルダを選択して下さい.\n"
			"サブフォルダ中も比較対象です.\n"
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
	CDbg().Out("file_list.txtと【比較を開始します】");
	fprintf(logfile.GetFilePtr(), "%s\n", "【比較を開始します】\n-----");
	{
		string line,line2;
		while(true){
			if (filelist.ReadLine(line)!=0) break;
			if (filelist.ReadLine(line2)!=0) break;

			string filename;
			filename = line; // これそのままファイルやろ？
			filename = src_path + filename;
			fprintf(logfile.GetFilePtr(), "%s\n", filename.c_str());

			DWORD dwSize;
			string hash;
			LRESULT ret = CalcMD5(filename,dwSize,hash);
			if ( ret==1 ) {
				CDbg().Out("%s", filename.c_str());
				CDbg().Out("　×オープンに失敗");
				fprintf(logfile.GetFilePtr(), "%s\n", "　×オープンに失敗");
				continue ;
			}
			ef ( ret==2 ) {
				CDbg().Out("%s", filename.c_str());
				CDbg().Out("　×MD5の取得に失敗");
				fprintf(logfile.GetFilePtr(), "%s\n", "　×MD5の取得に失敗");
				continue ;
			}

			LPCSTR lp = line2.c_str();
			DWORD dwSize2;
			CStringScanner::GetNum(lp,*(int*)&dwSize2);
			CStringScanner::SkipSpace(lp);
			const string hash2 = CStringScanner::GetStr(lp);

			if (dwSize!=dwSize2 || hash!=hash2) {
				CDbg().Out("%s", filename.c_str());
				CDbg().Out("　×不一致");
				fprintf(logfile.GetFilePtr(), "%s\n", "　×不一致");
			} else {
				fprintf(logfile.GetFilePtr(), "%s\n", "　○一致");
			}
		}
	}

	CDbg().Out("【比較結果をcompare_result.txtに書き込みました】");
	CDbg().Out("【比較が終了しました】");
	fprintf(logfile.GetFilePtr(), "%s\n", "-----\n【比較が終了しました】");
}

LRESULT	CApp::CalcMD5(const string& filename, DWORD& dwSize, string& result){
	//	ファイル開いて
	CFile file;
	if ( file.Open(filename, "rb")!=0 ) {
		//	ファイル空なんとちゃう？
		dwSize = 0;
		result.clear();
		return 1;
	}

	//	ファイルサイズを得る
	fseek(file.GetFilePtr(), 0, SEEK_END);
	dwSize = ftell(file.GetFilePtr());
	fseek(file.GetFilePtr(), 0, SEEK_SET);

	if ( dwSize!=0 ) {
		MD5 md5(file.GetFilePtr());	//	fileポインタ直接渡せる
		LPCSTR p = md5.hex_digest();
		result = p;
		delete [] p;	//	これdeleteせんといかんという..
	} else {
		result.clear();
	}
	return 0;
}
