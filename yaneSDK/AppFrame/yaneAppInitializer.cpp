#include "stdafx.h"

#include "yaneAppInitializer.h"
#include "../Thread/yaneThread.h"
	//	終了するときにすべてのスレッドの終了を確認しておく必要があるので
	//	ここでCThreadManagerに対してアクセスする必要がある

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

//	static member..
CAppInitializer* CAppInitializer::m_vObj=NULL;
singleton<CAppCallbackList>	CAppInitializer::m_AppCallBackObj;
bool CAppInitializer::m_bMultiThread = false;

#ifndef COMPILE_YANE_PLUGIN_DLL
//	WinMainからそのパラメータをそのまま渡してね。
CAppInitializer::CAppInitializer(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){
	//	パラメータをそのまま保存:p
	m_hInstance		=	hInstance;
	m_hPrevInstance	=	hPrevInstance;
	m_lpCmdLine		=	lpCmdLine;
	m_nCmdShow		=	nCmdShow;

	m_vObj = this; // 外部からstaticにアクセスできるようにしておく

	m_bMultiThread = true;
	//	ここ以降は、マルチスレッド動作。
	//	この変数はlatch変数なので、これで良い

	IThread::setThread(NULL);

	GetCallbackObj()->InitCallback();
}

#else

//	DllMainからのパラメータをそのまま渡してネ！Dllの初期化もできちゃうヨン
CAppInitializer::CAppInitializer( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved){
	//	パラメータをそのまま保存:p
	m_hInstance		=	(HINSTANCE)hModule;
	m_hPrevInstance	=	(HINSTANCE)hModule;
	m_lpCmdLine		=	(LPSTR)lpReserved;
	//	ここでnewしてもok
	m_vObj = this; // 外部からstaticにアクセスできるようにしておく
}

#endif

CAppInitializer::~CAppInitializer(){
#ifndef COMPILE_YANE_PLUGIN_DLL
	CThreadManager::GetObj()->WaitAllThreadEnded();
	//	すべてのスレッドの終了を待つ
#endif
	//	この直後は、シングルスレッドモードに戻る
	m_bMultiThread	= false;
	GetCallbackObj()->ExitCallback();
}

void	CAppCallbackList::InitCallback(){
	//	RegistInitCallbackされた関数を順番に呼び出す

	//	このときは、まだシングルスレッドなので
	//	CriticalSectionを使用する必要は無い

	//	コールバック予約されたやつを呼び出す
	GetInitCallbackList()->for_each(function_callback::run);
}

void	CAppCallbackList::ExitCallback(){
	//	RegistExitCallbackされた関数を順番に呼び出す

	//	このときは、既にシングルスレッドなので
	//	CriticalSectionを使用する必要は無い

	//	コールバック予約されたやつを呼び出す
	GetExitCallbackList()->for_each(function_callback::run);
}

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

//////////////////////////////////////////////////////////////////////////////

