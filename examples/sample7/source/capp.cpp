#include "stdafx.h"
#include "capp.h"

#include "resource.h"
#include <time.h>
#include  <mbctype.h> // _ismbblead

void	CApp::ConvertText(const string& inText,string& outText){
//	はりきって、変換しる！

	//	入力がナイだった(´Д｀)
	if (inText.empty()){
		outText.erase();
		return ;
	}

	string className = "lambda";

	//	時刻をクラス名にする
	{
        struct tm *newtime;
		time_t long_time;
        time( &long_time );                /* long 整数として時刻を取得 */
        newtime = localtime( &long_time ); /* 現地時刻に変換 */
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
	//	入力の '(' まで
	string funcArg; //  = "int x,int y";
	//	入力の ')' まで
	string body; //		 = " { return x + y; }";
	//	そのあと '{'と、それに対応する '}'まで

	///	面倒なのでこぴぺでいいや（・∀・）ｱﾋｬ!?
	string::const_iterator it = inText.begin();
	while (it!=inText.end()){
		if (*it=='('){
			//	ここまでだ！
			it++;
			break;
		}
		funcType += *it;
		//	2バイト文字なら2つ送る
		if (_ismbblead(*it)) {
			it+=2;
		} else {
			it++;
		}
	}
//	if (it==inText.end()) return ; // だめぽ
	while (it!=inText.end()){
		if (*it==')'){
			//	ここまでだ！
			break;
		}
		funcArg += *it;
		if (_ismbblead(*it)) { it+=2; } else { it++; }
	}
	while (it!=inText.end()){
		if (*it=='{'){
			//	ここまでだ！
			it++;
			break;
		}
		if (_ismbblead(*it)) { it+=2; } else { it++; }
	}
	while (it!=inText.end()){
		if (*it=='}'){
			//	ここまでだ！
			it++;
			break;
		}
		body += *it;
		if (_ismbblead(*it)) { it+=2; } else { it++; }
	}

	vector<string> constructor;
	///	↑	CApp* pThis this
	///　というようにコンストラクタでもらう
	///	1.型名 2.メンバ名 3.元変数名
	///	の順番でpushしてあるものとする
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
				改行があれば、ディフォルト引数を埋めておこう
				例)
					int c : d; => int , c , d
					int c;	   => int , c , c ←この最後のc
			*/
				if (nMod == 1){
					constructor.push_back("???");
					constructor.push_back("???");
				}
				if (nMod == 2){
					int n = constructor.size()/3 * 3; // 3でalign
					constructor.push_back(constructor[n+1]);
				}
			}
			goto next;
		}
		stemp += *it;
next:;
		if (_ismbblead(*it)) { it+=2; } else { it++; }
	}
	{	//	↑のコピペ
		int nMod = constructor.size()%3;
		if (nMod == 1){
			constructor.push_back("???");
			constructor.push_back("???");
		}
		if (nMod == 2){
			int n = constructor.size()/3 * 3; // 3でalign
			constructor.push_back(constructor[n+1]);
		}
	}

	outText =
		"// lambdaクラスの定義" + nl
		+ "class " + className + " {" + nl;

	outText +=
		"public:" + nl;

	if (!constructor.empty()){
		//	コンストラクタ
		outText += className + "(";
		//	constructorは3個ずつpushしてあると仮定して良い
		int i,j;
		for(i=0,j=1;i<(int)constructor.size();i+=3,j++){
			if (i>0) outText += ','; // 区切りのカンマ
			outText += 
				constructor[i] + " "
				"Arg"+CStringScanner::NumToString(j);
		}
		outText += "):" + nl + "\t";
		for(i=0,j=1;i<(int)constructor.size();i+=3,j++){
			if (i!=0 && (i/3)%3 == 0) outText += nl + "\t"; // ときどき改行
			if (i>0) outText += ','; // 区切りのカンマ
			outText += 
				constructor[i+1] + "("
				"Arg"+CStringScanner::NumToString(j)
				+")";
		}
		outText += "{}" + nl;
	}

	//	オペレータ
	outText += funcType + " operator()(" + funcArg + ")"+nl
		+ "{" + body + "}" + nl;

	if (!constructor.empty()){
		//	protectedメンバ
		outText +=
			"protected:"+nl;
		//	constructorは3個ずつpushしてあると仮定して良い
		for(int i=0;i<(int)constructor.size();i+=3){
			outText +=
			"\t"	+	constructor[i]	+	"\t" + constructor[i+1] + ";" + nl;
		}
	}

	outText +=
		"};" + nl
		+ nl;

	outText +=  "// 関数オブジェクト(コピペして埋め込んでネ)" + nl
		+ className + '(';
	if (!constructor.empty()){
		for(int i=0;i<(int)constructor.size();i+=3){
			if (i!=0) outText += ',';
			if (i!=0 && (i/3)%5 == 0) outText += nl + "\t"; // ときどき改行
			outText += constructor[i+2];
		}
	}
	
	outText += 	")" + nl + nl;

	outText
		+= "/** // 元ソース" + nl
		+ inText + nl
		+ "*/";

}

void	CApp::MainThread() {

	CFPSTimer timer;
	timer.SetFPS(10);

	//	これをメインプリにする（終了するときに、他のウィンドゥをすべて閉じる）
	SetMainApp(true);

	CDialogHelper dialog(GetMyApp()->GetMyWindow());

	string text;
	while (IsThreadValid()){
		string t = dialog.GetText(IDC_EDIT1);
		if (t!=text) {	//	内容かわっとるデ？処理して反映しる！
			string outText;
			ConvertText(t,outText);
			dialog.SetText(IDC_EDIT2,outText);
			text = t;
		}
	}
}

//	これがmain windowのためのクラス。
class CAppMainWindow : public CAppBase {	//	アプリケーションクラスから派生
	virtual void MainThread(){			   //  これがワーカースレッド
		CApp().Start();
	}
	virtual LRESULT OnPreCreate(CWindowOption &opt){
		opt.dialog = MAKEINTRESOURCE(IDD_DIALOG1);	//	ダイアログなのだ！
		return 0;
	}
};


//	言わずと知れたWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	{
		/*
		{	//	エラーログをファイルに出力するのら！
			CTextOutputStreamFile* p = new CTextOutputStreamFile;
			p->SetFileName("Error.txt");
			Err.SelectDevice(smart_ptr<ITextOutputStream>(p));
		}
		*/

		CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
		//	↑必ず書いてね

		CSingleApp sapp;
		if (sapp.IsValid()) {
			CThreadManager::CreateThread(new CAppMainWindow);
		}
		//	ここでCAppInitializerがスコープアウトするのだが、このときに
		//	すべてのスレッドの終了を待つことになる
	}
	return 0;
}
