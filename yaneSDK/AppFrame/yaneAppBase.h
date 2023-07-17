//	yaneApp.h :
//		application thread class
//			programmed by yaneurao	'00/02/25

#ifndef __yaneAppBase_h__
#define __yaneAppBase_h__

#include "../Window/yaneWinHook.h"
#include "../Window/yaneWindow.h"
#include "../Thread/yaneThread.h"
#include "../Timer/yaneIntervalTimer.h"

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

class IAppBase {
/**
	アプリケーション基底クラスのインターフェース
*/
public:
	virtual bool	IsThreadValid()=0;
	virtual void	InvalidateThread()=0;
	//	これCAppBaseではCThread::InvalidateThread()の呼び出しに化ける

	virtual IWindow* GetMyWindow()=0;
	virtual HWND	GetHWnd() const=0;
	virtual LRESULT	Run()=0;
	virtual void	Close()=0;
	virtual IIntervalTimer* GetIntervalTimer()=0;

////////////////////////////////////////////////////////////////
//	mainとかsubとか言う概念自体がナンセンスなので
//	これらのメソッドは廃止の方向で．．

	///		これ使うかな？
///	static	IAppBase* GetMainApp()	 { return m_lpMainApp; }
	/// メインアプリへのポインタを返す

///	static	HWND	GetMainWnd()	 { return GetMainApp()->GetHWnd(); }
	/// メインアプリの保有するウィンドゥハンドルを返す

	virtual bool	IsMainApp() const =0;
	virtual void	SetMainApp(bool b) =0;

	///	ワーカースレッドの取得
	virtual IThread* GetWorkerThread()=0;

////////////////////////////////////////////////////////////////

	virtual ~IAppBase(){}

protected:
	//	static IAppBase* m_lpMainApp;
	//	メインウィンドゥ（これの終了をプログラム終了とみなす）
};

class CAppBase : public IWinHook,public IAppBase , public CThread {
/**
	アプリケーション基底クラス

	アプリケーションは、こいつから派生させると良い。

	すなわち、CAppBase とは スレッド生成 + CWindow + メッセージポンプ

	このクラスのインスタンスを複数作成することによって、
	マルチウィンドゥが実現できる。

	☆　ウィンドゥが閉じられるときの処理を記述するには

	WM_CLOSEが呼び出されると、OnPreCloseが呼び出されます。
	これが非0を返せば、WM_CLOSEは処理されません。
	（ウィンドゥは閉じられません）
	そして、このときにそのウィンドゥを閉じるかどうかをユーザーに
	問い合わせるような処理をして、閉じられることが確定したら、
	IAppBase::Closeを呼び出します。そうすれば、ウィンドゥは閉じられます。

*/
public:

	virtual LRESULT Run();
	/**
　	この関数を呼び出すことによって窓が生成され、
	ワーカースレッド(virtual void MainThread()を実行する)を生成します。
	窓が生成されるのを待ってから制御を返します。

	この関数を呼び出さずにCreateThreadで直接スレッドを起動してもかまいません。
	その場合は、窓の生成完了を待たずに制御を返します
	*/

	virtual HWND	GetHWnd() const { return m_oWindow.GetHWnd(); }
	///	ウィンドゥハンドル取得

	virtual IWindow* GetMyWindow() { return &m_oWindow; }
	///	ウィンドゥクラス取得

	bool	IsMainApp() const { return m_bMainApp; }
	void	SetMainApp(bool b){ m_bMainApp = b; }
	/**
		このCAppBaseがメインアプリであるかを設定する
		defaultでfalse
		これをtrueにしておけば、このアプリが終了するときに
		他のスレッドをすべてinvalidateする（ようにWM_QUITを発行する）
	*/

	bool	IsMessage() const { return m_bMessage; }
	///	メッセージループスレッドは生きているか

	///	スレッドの正当性チェック overriden from CThread
	virtual bool	IsThreadValid();
	///	⇒　こいつのなかにメッセージループも書くと良い

	///	スレッドのInvalidate。
	virtual void	InvalidateThread();	//	overriden from CThread
	/**
		スレッド自体を無効にする。Idleモードで動作しているスレッドは、
		Windowメッセージが来ない限り、眠ったままなので、そういう場合には、
		強制的にWM_CLOSEを送ってやる必要がある。
		この関数は、そういう処理をする。たとえば、CDebugWindowは、
		通常、Idleモードで動作している。
	*/

	///	ウィンドゥ最小化時に待機するか
	volatile bool*	GetWaitIfMinimized() {	return &m_bWaitIfMinimized; }
	/**
		IsThreadValid呼び出し時に、ウィンドゥが最小化されていればそのまま
		待機するか？のboolフラグを取得する。
			使用例→　 *GetWaitIfMinimized( ) = true;
	*/

	///	Idleモード(default:false)にするのか
	bool* GetIdle() { return &m_bIdle;}
	/**
		フラグのポインタが返るので、Idleモードにするには、
		*GetIdle( ) = true;のようにする。
		Idleモードにすると、IsThreadValidでWindowメッセージを処理するときに、
		Windowメッセージがが存在しないときは、このスレッドは眠ったままになる。
		（ＣＰＵパワーを食わない）
		Idleモードになっているスレッドを殺すには、
		このクラスのInvalidateThreadを呼び出すこと。
	*/

	///	OnPreCloseを処理せずに強制的にWM_CLOSEを実行する
	virtual void	Close();

	///	ワーカースレッドの取得
	virtual IThread* GetWorkerThread()
		{ return this; }
	/**
		このフレームワークには、メッセージポンプは存在せず
		ワーカースレッドのみで、このCAppBase自体が、
		CThreadの派生クラスなので、それをそのまま返しておけば良い。
	*/

	CAppBase();
	virtual ~CAppBase();

protected:
	//////////////////////////////////////////////////////////////////////////
	///	継承するといいことあるかも（笑）関数の一覧

	///	これが実行スレッドなので継承して、こいつを用意してね！
	///	ウィンドゥ作成後(OnCreate()のあと)に呼び出されるの。
	virtual void MainThread() {
		while (IsThreadValid())
			IThread::getThread()->sleep(20);
			//　こうしておけばスレッドインタラプトに対する応答性があがる
	}

	//	その他も適宜オーバーライドしてねん
	virtual LRESULT OnInit()	{ return 0; }		///	生成直後
	virtual LRESULT OnPreCreate(CWindowOption& opt);///	ウィンドゥ作成直前
	virtual LRESULT OnCreate()	{ return 0; }		///	ウィンドゥ作成直後
	virtual LRESULT OnDestroy() { return 0; }		///	終了直前

	virtual LRESULT OnPreClose() { return 0; }
	///	WM_CLOSE処理前に呼び出され、これを非0を返せばWM_CLOSEは実行されない
	///	その状況下においてウィンドゥをCLOSEさせるためには、Closeを呼び出すこと

	///	brief Window Message to override
	virtual void OnPaint()		{ }

	IIntervalTimer*	GetIntervalTimer();
	/**
		インターバルタイマの取得
		IsThreadValidを呼び出したときに、このコールバックがかかる
	*/

	//////////////////////////////////////////////////////////////////////////
protected:
	CWindow m_oWindow;			//	保有しているwindow
	bool	m_bMessage;			//	メッセージループスレッドは生きているか

	virtual void	ThreadProc();					//	override from CThread
	virtual LRESULT WndProc(HWND,UINT,WPARAM,LPARAM);	//	override in CWinHook
	void	InnerStopThread();

	volatile bool	m_bWaitIfMinimized;		//	ウィンドゥ最小化時に待機するか

	bool	m_bClose;			//	OnPreCloseを呼ばずにWM_CLOSEを処理するか？
	bool	m_bIdle;			//	Idleモード
	HACCEL	m_hAccel;			//	アクセラレータ

	CIntervalTimer	m_vIntervalTimer;
	volatile bool m_bMainApp;		//	メインアプリなのか

private:
	static	DWORD WINAPI RunPrepare(LPVOID lpVoid); //	Runからのジャンプ台
	LRESULT RunThread();						//	ウィンドゥの作成とWorkThreadの作成とMessageLoop
	LRESULT MessagePump(bool bPeek=true);			//	MessagePomp
	//	bPeek==trueならばPeekして、メッセージが無くなるまで処理する
	//	bPeek==falseならば、メッセージをひとつでも処理するまでこの関数から抜けない

	volatile LONG	m_nThreadStatus;				//	スレッドのステータス
};

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

#endif
