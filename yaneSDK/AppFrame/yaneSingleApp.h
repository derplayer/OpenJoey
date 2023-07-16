#ifndef __yaneSingleApp_h__
#define __yaneSingleApp_h__

#include "../Thread/yaneMutex.h"

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

class CSingleApp {
/**
	二重起動防止に使います。

	bool IsValid();
	//　他に自分と同じアプリが起動していなかったか？

	二重起動していればfalseになるので、その場合は、
	即座にこのプログラムを終了させてください。
	（その場合、そちらにフォーカスが移ります）
	この処理には class CFindWindow を利用しています。
	class CFindWindow は、現在実行されているモジュール名と
	同じモジュール名のプロセスを探して、そのウィンドゥを
	アクティブにするクラスです。

	CFindWindowでのモジュール名の比較は、フルパスではなく、実行ファイル名のみで
	行なわれるため、他フォルダで同名の実行ファイル名のものが起動している
	場合は、それがActiveになることがあります

☆　使用例

// 言わずと知れたWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
　　CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
　　CSingleApp sapp;
　　if (sapp.IsValid()) {
		//	ゲームの処理
　　}
　　return 0;
}

*/
public:
	bool	IsValid(const string& strMutexName = "");
	///　他に自分と同じアプリが起動していなかったか？
	/**
		二重起動の判定は、「同一のフォルダの同一名の実行ファイルならば
		同一とみなす」という処理になっています。もし、これがまずければ、
		そのアプリケーション固有のMutex名を指定することに
		よって、排他することが出来ます。
	*/

protected:
	CMutex	m_oMutex;
	bool	m_bValid;
};

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

#endif
