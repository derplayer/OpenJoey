//	yaneAppManager.h :
//		application management class
//			programmed by yaneurao	 '00/03/03
//			reprogrammed by yaneurao '02/03/16

#ifndef __yaneAppManager_h__
#define __yaneAppManager_h__

#include "../thread/yaneCriticalSection.h"

namespace yaneuraoGameSDK3rd {
namespace Window {
	class IWinHook;
	class IWindow;
}
}

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

class IAppDraw;
class IAppBase;
class IAppFrame;

class CAppManager {
protected:
	struct CAppManageInfo {
		IAppBase*		m_lpAppBase;		//	ひとつのアプリクラスを持つ
		IAppFrame*		m_lpAppFrame;		//	もしかしたら一つのフレームクラスを持つ
		IAppDraw*		m_lpAppDraw;		//	ひとつの描画クラスを持つ
		DWORD			m_dwThreadID;		//	メインスレッドのThreadID
	};
	typedef smart_vector_ptr<CAppManageInfo> CAppManageList;
	CAppManageList m_alpInfo;

	//	終了時にコールバックを要求しているリスト(CDbgで使用)
	//	こいつに登録しておけば、staticなCAppBaseのインスタンス
	//	であっても(~CAppBaseが呼び出されていないから本来は
	//	終了のときに参照数が狂うはずだが)、許容される
	typedef smart_vector_ptr<function_callback> CAppCallBackList;
	CAppCallBackList m_alpCallBack;

public:
	static	void	Add(IAppBase*p){  GetObj()->_Add(p); }
	static	void	Del(IAppBase*p){  GetObj()->_Del(p); }
	static	void	Add(IAppFrame*p){  GetObj()->_Add(p); }
	static	void	Del(IAppFrame*p){  GetObj()->_Del(p); }
	static	void	Add(IAppDraw*p){  GetObj()->_Add(p); }
	static	void	Del(IAppDraw*p){  GetObj()->_Del(p); }

	//	登録しておいたものを取得できる
	static	IAppBase*	 GetMyApp() { return GetObj()->_GetMyApp(); }
	static	IAppFrame*	 GetMyFrame() { return GetObj()->_GetMyFrame(); }
	static	IAppDraw*	 GetMyDraw() { return GetObj()->_GetMyDraw(); }
	static	IWindow*	 GetMyWindow(); // IAppBase経由で取得

	///	 自分の属するアプリスレッドの保有しているウィンドゥに関連付ける。
	///	（ウィンドゥメッセージがコールバックされるようになる）
	static void	Hook(IWinHook*p){ GetObj()->_Hook(p); }
	static void	Unhook(IWinHook*p){ GetObj()->_Unhook(p); }

	///	自分の属するアプリスレッドの保有するウィンドゥハンドルを返す
	static HWND	GetHWnd(){ return GetObj()->_GetHWnd(); }

	///	フルスクリーンモードかどうかを返す
	static bool	IsFullScreen(){ return GetObj()->_IsFullScreen(); }

	static	int		GetAppInstanceNum(){ return GetObj()->_GetAppInstanceNum(); }
	static	void	StopAllThread() { GetObj()->_StopAllThread(); }
	
	static	void Inc(){	GetObj()->_Inc(); }
	//	参照カウントのインクリメント

	static	void Dec(){	GetObj()->_Dec(); }
	//	参照カウントのデクリメント

	static	int GetRef(){ return GetObj()->_GetRef(); }

	static CAppCallBackList* GetCallBackList() { return GetObj()->_GetCallBackList(); }
	static CCriticalSection* GetCriticalSection() { return GetObj()->_GetCriticalSection(); }

	//	これpublicにしとかないとsingletonのCheckInstanceで生成できない
	CAppManager():m_nRef(0) {}

protected:
	static singleton <CAppManager> m_obj;
	static CAppManager* GetObj() { return m_obj.get(); }

	///	singleton経由でアクセスを行なうため、非staticなバージョンが必要
	void	_Add(IAppBase*);
	void	_Del(IAppBase*);
	void	_Add(IAppFrame*);
	void	_Del(IAppFrame*);
	void	_Add(IAppDraw*);
	void	_Del(IAppDraw*);
	IAppBase*	 _GetMyApp();
	IAppFrame*	 _GetMyFrame();
	IAppDraw*	 _GetMyDraw();
	void	_Hook(IWinHook*p);
	void	_Unhook(IWinHook*p);
	HWND	_GetHWnd();
	bool	_IsFullScreen();
	int		_GetAppInstanceNum();
	void	_StopAllThread();
	void _Inc();
	void _Dec();
	int _GetRef();
	CAppCallBackList* _GetCallBackList() { return& m_alpCallBack; }
	CCriticalSection* _GetCriticalSection() { return& m_oCriticalSection; }

private:
	CCriticalSection m_oCriticalSection;
	int m_nRef;	//	参照カウント
};

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

#endif
