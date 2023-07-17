#include "stdafx.h"
#include "capp.h"

#include "resource.h"
#include <time.h>
#include  <mbctype.h> // _ismbblead

void	CApp::ConvertText(const string& inText,string& outText){
//	�͂肫���āA�ϊ�����I

	//	���͂��i�C������(�L�D�M)
	if (inText.empty()){
		outText.erase();
		return ;
	}

	string className = "lambda";

	//	�������N���X���ɂ���
	{
        struct tm *newtime;
		time_t long_time;
        time( &long_time );                /* long �����Ƃ��Ď������擾 */
        newtime = localtime( &long_time ); /* ���n�����ɕϊ� */
		className +=
			CStringScanner::NumToStringZ(newtime->tm_year % 100,2)
			+ CStringScanner::NumToStringZ(newtime->tm_mon+1,2)
			+ CStringScanner::NumToStringZ(newtime->tm_mday,2)
			+ CStringScanner::NumToStringZ(newtime->tm_hour,2)
			+ CStringScanner::NumToStringZ(newtime->tm_min,2)
			+ CStringScanner::NumToStringZ(newtime->tm_sec,2)
			+ "_"
			+ CStringScanner::NumToStringZ(rand()%10000,4);
			;
	}

	const string nl		 = "\r\n";	//	nextline

	string funcType; //  = "int";
	//	���͂� '(' �܂�
	string funcArg; //  = "int x,int y";
	//	���͂� ')' �܂�
	string body; //		 = " { return x + y; }";
	//	���̂��� '{'�ƁA����ɑΉ����� '}'�܂�

	///	�ʓ|�Ȃ̂ł��҂؂ł�����i�E�́E�j�ˬ!?
	string::const_iterator it = inText.begin();
	while (it!=inText.end()){
		if (*it=='('){
			//	�����܂ł��I
			it++;
			break;
		}
		funcType += *it;
		//	2�o�C�g�����Ȃ�2����
		if (_ismbblead(*it)) {
			it+=2;
		} else {
			it++;
		}
	}
//	if (it==inText.end()) return ; // ���߂�
	while (it!=inText.end()){
		if (*it==')'){
			//	�����܂ł��I
			break;
		}
		funcArg += *it;
		if (_ismbblead(*it)) { it+=2; } else { it++; }
	}
	while (it!=inText.end()){
		if (*it=='{'){
			//	�����܂ł��I
			it++;
			break;
		}
		if (_ismbblead(*it)) { it+=2; } else { it++; }
	}
	while (it!=inText.end()){
		if (*it=='}'){
			//	�����܂ł��I
			it++;
			break;
		}
		body += *it;
		if (_ismbblead(*it)) { it+=2; } else { it++; }
	}

	vector<string> constructor;
	///	��	CApp* pThis this
	///�@�Ƃ����悤�ɃR���X�g���N�^�ł��炤
	///	1.�^�� 2.�����o�� 3.���ϐ���
	///	�̏��Ԃ�push���Ă�����̂Ƃ���
	/*
		constructor.push_back("CApp*");
		constructor.push_back("pThis");
		constructor.push_back("this");
		constructor.push_back("int");
		constructor.push_back("c");
		constructor.push_back("d");
	*/
	string stemp;
	while (it!=inText.end()){
		if (*it==':' || *it==' ' || *it=='\r' || *it=='\n' || *it==';') {
			if (!stemp.empty()){
				constructor.push_back(stemp);
				stemp.erase();
			}
			int nMod = constructor.size()%3;
			if (*it=='\n' && nMod !=0){
			/*
				���s������΁A�f�B�t�H���g�����𖄂߂Ă�����
				��)
					int c : d; => int , c , d
					int c;	   => int , c , c �����̍Ō��c
			*/
				if (nMod == 1){
					constructor.push_back("???");
					constructor.push_back("???");
				}
				if (nMod == 2){
					int n = constructor.size()/3 * 3; // 3��align
					constructor.push_back(constructor[n+1]);
				}
			}
			goto next;
		}
		stemp += *it;
next:;
		if (_ismbblead(*it)) { it+=2; } else { it++; }
	}
	{	//	���̃R�s�y
		int nMod = constructor.size()%3;
		if (nMod == 1){
			constructor.push_back("???");
			constructor.push_back("???");
		}
		if (nMod == 2){
			int n = constructor.size()/3 * 3; // 3��align
			constructor.push_back(constructor[n+1]);
		}
	}

	outText =
		"// lambda�N���X�̒�`" + nl
		+ "class " + className + " {" + nl;

	outText +=
		"public:" + nl;

	if (!constructor.empty()){
		//	�R���X�g���N�^
		outText += className + "(";
		//	constructor��3����push���Ă���Ɖ��肵�ėǂ�
		int i,j;
		for(i=0,j=1;i<(int)constructor.size();i+=3,j++){
			if (i>0) outText += ','; // ��؂�̃J���}
			outText += 
				constructor[i] + " "
				"Arg"+CStringScanner::NumToString(j);
		}
		outText += "):" + nl + "\t";
		for(i=0,j=1;i<(int)constructor.size();i+=3,j++){
			if (i!=0 && (i/3)%3 == 0) outText += nl + "\t"; // �Ƃ��ǂ����s
			if (i>0) outText += ','; // ��؂�̃J���}
			outText += 
				constructor[i+1] + "("
				"Arg"+CStringScanner::NumToString(j)
				+")";
		}
		outText += "{}" + nl;
	}

	//	�I�y���[�^
	outText += funcType + " operator()(" + funcArg + ")"+nl
		+ "{" + body + "}" + nl;

	if (!constructor.empty()){
		//	protected�����o
		outText +=
			"protected:"+nl;
		//	constructor��3����push���Ă���Ɖ��肵�ėǂ�
		for(int i=0;i<(int)constructor.size();i+=3){
			outText +=
			"\t"	+	constructor[i]	+	"\t" + constructor[i+1] + ";" + nl;
		}
	}

	outText +=
		"};" + nl
		+ nl;

	outText +=  "// �֐��I�u�W�F�N�g(�R�s�y���Ė��ߍ���Ńl)" + nl
		+ className + '(';
	if (!constructor.empty()){
		for(int i=0;i<(int)constructor.size();i+=3){
			if (i!=0) outText += ',';
			if (i!=0 && (i/3)%5 == 0) outText += nl + "\t"; // �Ƃ��ǂ����s
			outText += constructor[i+2];
		}
	}
	
	outText += 	")" + nl + nl;

	outText
		+= "/** // ���\�[�X" + nl
		+ inText + nl
		+ "*/";

}

void	CApp::MainThread() {

	CFPSTimer timer;
	timer.SetFPS(10);

	//	��������C���v���ɂ���i�I������Ƃ��ɁA���̃E�B���h�D�����ׂĕ���j
	SetMainApp(true);

	CDialogHelper dialog(GetMyApp()->GetMyWindow());

	string text;
	while (IsThreadValid()){
		string t = dialog.GetText(IDC_EDIT1);
		if (t!=text) {	//	���e������Ƃ�f�H�������Ĕ��f����I
			string outText;
			ConvertText(t,outText);
			dialog.SetText(IDC_EDIT2,outText);
			text = t;
		}
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
