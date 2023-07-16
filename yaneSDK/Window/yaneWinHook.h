// yaneWinHook.h
//	WindowMessageHooker
//	  WindowメッセージをフックさせることでWindowのサブクラス化をアシストする
//	  (c) yaneurao '99/06/23
//
//		programmed by yaneurao(M.Isozaki)	'99/06/21
//		reprogrammed by yaneurao			'00/02/25

#ifndef __yaneWinHook_h__
#define __yaneWinHook_h__


#include "../Thread/yaneCriticalSection.h"

namespace yaneuraoGameSDK3rd {
namespace Window {

class IWinHook {
/**
	ウィンドゥメッセージをフックするのに利用します。
	メッセージをフックしたいクラスは、このクラスから派生して、この関数

	virtual LRESULT WndProc(HWND,UINT,WPARAM,LPARAM) = 0; 

	をオーバーライドします。この関数はメッセージを処理した場合、
	非0を返すようにコーディングします。

	class CWinHookList も参照してください。
*/
public:
	virtual	LRESULT	WndProc(HWND,UINT,WPARAM,LPARAM) = 0;
	virtual ~IWinHook(){}
};

class IWinHookList {
public:
	virtual void	Add(IWinHook*)=0;		//	自分自身をフックに加える
	virtual void	Del(IWinHook*)=0;		//	自分自身をフックから外す
	virtual void	Clear()=0;				//	すべてをクリアする

	//	メッセージのDispatcher
	virtual LRESULT Dispatch(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,WNDPROC pWndProc=NULL)=0;
	virtual LRESULT DispatchDlg(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,WNDPROC pWndProc=NULL)=0;
};

class CWinHookList : public IWinHookList {
/**
	ウィンドゥメッセージをフックするのに利用します。
	CWinHookから派生されたクラスのポインタを保持し、管理しています。

	１つのウィンドゥクラスに対して１つのCWinHookListが対応します。
	マルチスレッド対応です。

	メッセージの配信には、Windowクラスから、これを呼び出します。
	そうすると、自分の保持しているCWinHookのWndProcに委譲していきます。
	委譲順序は、必ずAddした順番であり、WndProcが非0を返すものに出会うまで
	WndProcを呼び出しつづけます。管理しているすべてのCWinHookのWndProcを
	呼び出した場合は、ディフォルトの処理関数として::DefWindowProcを
	呼び出します。

	CAppBaseはメッセージポンプは別スレッドなので、Hookを開始した次の瞬間から、
	WMメッセージが飛んでくるので、初期化が終了していないタイミングで
	WMメッセージを処理しないように十分注意してください。

	メッセージポンプを別スレッドで行なう場合、Hookを開始した次の瞬間から、
	WMメッセージが飛んでくるので、初期化が終了していないタイミングで
	WMメッセージを処理しないように十分注意してください。

*/
public:
	virtual void	Add(IWinHook*);		//	自分自身をフックに加える
	virtual void	Del(IWinHook*);		//	自分自身をフックから外す
	virtual void	Clear();			//	すべてをクリアする

	///	メッセージのDispatcher
	///	後者は、Dialog用で、最後にDefWindowProcを呼び出さない
	virtual LRESULT Dispatch(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,WNDPROC pWndProc=NULL);
	virtual LRESULT DispatchDlg(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,WNDPROC pWndProc=NULL);

protected:
	typedef list<IWinHook*> hooklist;
	hooklist			m_HookPtrList;	//	フックリスト
	CCriticalSection	m_oCriticalSection;
	CCriticalSection*	GetCriticalSection() { return& m_oCriticalSection;}
};

class CMessagePooler : public IWinHook {
/**
		ボタン等は押し下げ情報が来るので、特定のメッセージを
		hookする形で情報を取得する

		例)
		ボタンならば

			uMsg = WM_COMMAND , wParam = nEditControl
		だから、このように
		事前にuMsgとwParamをペアにしてメッセージdispatcherに
		登録しておき、あとで解析キューのデータを調べると良い

	これを行なうためのヘルパクラスが、CMessagePoolerである。
	uMsgとwParamの対を登録しておけば、そのメッセージが来ると
	Poolしておいてくれる。(あとで調べて取り出すことが出来る)

	//	メッセージのうち古いものは上書きされる(^^;
*/
public:
	struct CInfo {
	bool	isPool() const { return bPool; }
	/// メッセージがpoolされているか?

	void	reset() { bPool = false; }
	///	メッセージをresetする(メッセージがpoolされていない状態にする)

	void	init() {
		bPool = false;
		lParam=uMsg=wParam=0;
		bWparamEnable=bLparamEnable = false;
	}
	///	この構造体を初期化する(hookすべきメッセージも初期化される)

	bool	isEmpty() const {
		return uMsg==0 &&
			(!bWparamEnable||wParam==0) &&
			(!bLparamEnable||lParam==0);
	}
	///	この構造体にはまだhookすべきメッセージが登録されていない?

	WPARAM	getWParam() { return wParam; }
	void	setWParam(WPARAM w) { wParam = w;}
	///	poolされているメッセージのwParamを設定/取得する

	LPARAM	getLParam() { return lParam; }
	void	setLParam(LPARAM l) { lParam = l;}
	///	poolされているメッセージのlParamを設定/取得する

	void	pool(WPARAM w,LPARAM l){
		if(!bWparamEnable) wParam=w;
		if(!bLparamEnable) lParam=l;
		bPool = true;
	}
	///	bWparamEnableがfalseならば、wParamにwを取り込む
	/// bLparamEnableがfalseならば、lParamにlを取り込む
	///	bPoolはtrueになるのでisPoolでtrueを返すようになる
	
	void	setMsg(UINT u,WPARAM w,bool bWEnable,LPARAM l,bool bLEnable)
	{ uMsg=w; wParam = w; lParam=l; bWparamEnable=bWEnable; bLparamEnable=bLEnable; }
	///	poolするメッセージを登録
	/**
		WEnableがtrueだと、登録したWPARAMもisMsgで比較するときに比較対象とするの意味
		LEnableがtrueだと、登録したLPARAMもisMsgで比較するときに比較対象とするの意味
	*/

	bool	isMsg(UINT u,WPARAM w,LPARAM l)
	{ return uMsg==w && (!bWparamEnable || wParam== w) && (!bLparamEnable || lParam== l);}
	///	poolすべきメッセージと一致したらtrue

	CInfo():bPool(false),lParam(0),uMsg(0),wParam(0),bWparamEnable(false),bLparamEnable(false) {}

	protected:
	UINT	uMsg;	//	この
	WPARAM wParam;	//	uMsg,wParamの対をpoolする
	LPARAM lParam;
	bool bPool;
	bool bWparamEnable;	//	一致する条件に、WPARAMは入るのか？
	bool bLparamEnable;	//	一致する条件に、LPARAMは入るのか？
	};

	int		SetPoolData(UINT uMsg,WPARAM wParam,bool bWEnable,
		LPARAM lParam,bool bLEnable);
	/**
		戻り値は識別子。これをGetPoolの引数として渡して情報(CInfo)を取得する
		そのCInfoは、uMsg,wParamの対が来ると、isPool()がtrueにする。
		同時にそのときのlParamを保存する。(CInfoはあとからきたメッセージで上書きされる)
	*/

	void	ClearPoolData(UINT uMsg,WPARAM wParam,LPARAM lParam);
	/**
		Poolデータのなかから、uMsg,wParam,lParamで指定されたものを削除する
		他の識別子には影響はない
		SetPoolDataで設定するときに、bWEnableをfalseにしておけばwParamは
		比較対象にはならない。(何を渡しても良い)
		SetPoolDataで設定するときに、bLEnableをfalseにしておけばlParamは
		比較対象にはならない。(何を渡しても良い)
	*/

	CInfo*	GetInfo(int nID) { return& m_aInfo[nID]; }
	/**
		SetPoolDataの戻り値をIDとして指定して、その情報を取得する
	*/

protected:
	// override from CWinHook
	virtual LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	vector<CInfo> m_aInfo;
};

} // end of namespace Window
} // end of namespace yaneuraoGameSDK3rd

#endif
