//
//	アプリケーションイニシャライザ
//		yaneuraoGameSDK 2ndのためのイニシャライザ。
//		気にいらなければ自由にカスタマイズして使ってくださいな＾＾
//

#ifndef __yaneAppInitializer_h__
#define __yaneAppInitializer_h__

#include "../Window/yaneWinHook.h"

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

class CAppCallbackList {
public:
	void	RegistInitCallback(const smart_ptr<function_callback>& p){
		GetInitCallbackList()->push_back(p);
	}
	void	RegistExitCallback(const smart_ptr<function_callback>& p){
		GetExitCallbackList()->push_back(p);
	}

	void	InitCallback();	//	RegistInitCallbackされた関数を順番に呼び出す
	void	ExitCallback();	//	RegistExitCallbackされた関数を順番に呼び出す

	smart_list_ptr<function_callback> * GetInitCallbackList()
	{ return &m_listInitFunc;}
	smart_list_ptr<function_callback> * GetExitCallbackList()
	{ return &m_listExitFunc;}

protected:
	//	初期化時コールバック用
	smart_list_ptr<function_callback> m_listInitFunc;
	//	終了時のコールバック用
	smart_list_ptr<function_callback> m_listExitFunc;
};

class CAppInitializer {
/**
	アプリケーション初期化のためのクラス

	コンストラクタにWinMainからそのパラメータをそのまま渡してね。

	非ローカルstaticオブジェクトによるコールバック予約が出来ます。
		RegistInitCallback(コールバックを予約する関数);
		RegistExitCallback(コールバックを予約する関数);
	とすれば、コンストラクタとデストラクタにおいて、その関数に
	自動的にコールバックされます。（複数登録可）

*/
public:

#ifndef COMPILE_YANE_PLUGIN_DLL
	///	WinMainからそのパラメータをそのまま渡してね。
	CAppInitializer(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow);
#else
	CAppInitializer( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved);
	///	DllMainからのパラメータをそのまま渡してネ！Dllの初期化もできちゃうヨン
	///	GetCmdLine()で、ここで渡されたlpReservedが取得できまする
#endif

	///	このデストラクタでRegistExitCallbackしておいた関数が呼び出される
	~CAppInitializer();

	///	singletonを通じて取得する
	static HINSTANCE GetInstance()		{ return GetObj()->GetInstance_(); }
	static HINSTANCE GetPrevInstance()	{ return GetObj()->GetPrevInstance_(); }
	static LPSTR	 GetCmdLine()		{ return GetObj()->GetCmdLine_(); }
	static int		 GetCmdShow()		{ return GetObj()->GetCmdShow_(); }

#ifndef COMPILE_YANE_PLUGIN_DLL
	/**
		Dll作成時には、↓の関数は無効なので注意が必要！
	*/
	///		Init呼び出しのあとにコールバックして欲しいものを登録しておく
	static void	RegistInitCallback(const smart_ptr<function_callback>& p)
	{	GetCallbackObj()->RegistInitCallback(p); }
#endif

	///		終了時にコールバックして欲しいものを登録しておく
	static void	RegistExitCallback(const smart_ptr<function_callback>& p)
	{	GetCallbackObj()->RegistExitCallback(p); }

	static bool	IsMultiThread() { return m_bMultiThread; }
	/**
		マルチスレッド動作かどうかを返す
		このクラスのInitメソッドを呼び出し以降、デストラクタが
		呼び出されるまでの間、trueになる
	*/

	//////////////////////////////////////////////////////////////

	///		singletonオブジェクトの取得
	static CAppInitializer* GetObj() { return m_vObj; }

	///		WinMainで渡しておいたパラメータの取得
	HINSTANCE GetInstance_()		{ return m_hInstance; }
	HINSTANCE GetPrevInstance_()	{ return m_hPrevInstance; }
	LPSTR	 GetCmdLine_()			{ return m_lpCmdLine; }
	int		 GetCmdShow_()			{ return m_nCmdShow; }


protected:

	//	instance parameter...
	HINSTANCE	m_hInstance;
	HINSTANCE	m_hPrevInstance;
	LPSTR		m_lpCmdLine;
	int			m_nCmdShow;

private:
	//	singletonオブジェクト
	static CAppInitializer* m_vObj;

	static singleton<CAppCallbackList>	m_AppCallBackObj;
	static CAppCallbackList* GetCallbackObj()
	{ return m_AppCallBackObj.get(); }

	//	マルチスレッド動作かどうかを返す
	//	Init以降はtrueになる
	static bool m_bMultiThread;
};

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

#endif
