//	yaneAppFrame.h :
//			programmed by yaneurao	'00/06/16

#ifndef __yaneAppFrame_h__
#define __yaneAppFrame_h__

#include "yaneAppBase.h"
#include "yaneAppManager.h"

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

class IAppFrame {
public:
	virtual void	Start()=0;
	virtual void	MesSleep(int nTime)=0;
	virtual void	InvalidateThread()=0;
	virtual IAppBase* GetMyApp()const=0;
	virtual bool	IsThreadValid()const=0;
	virtual void	MainThread() = 0;
	virtual bool	IsMainApp() const =0;
	virtual void	SetMainApp(bool b) =0;

	//	コンストラクタ～デストラクタで、CAppManagerに登録しておく
	IAppFrame();
	virtual ~IAppFrame();
};

class CAppFrame : public IAppFrame {
/**
	アプリケーションの中核となるクラスです。

	アプリケーションとは、アプリケーションクラス(class CAppBaseの派生クラス)
	のことです。アプリケーションクラスは、一つのウィンドゥと
	一つのスレッドループ、１つの描画クラスを保有します。

	このワーカースレッドのなかから呼び出すのがCAppFrameというわけです。

	使い方：

	ユーザーは、CAppFrameを派生し、そこにメンバとして自分の使いたい
	コンポーネントをメンバとして持たせます。

	virtual void MainThread() = 0;
	この関数をオーバーライドして、Startメンバ関数を呼び出してください。
	そうすれば、初期化後、この関数が実行されます。

	このクラスの基底クラスである、class IAppFrame のコンストラクタで
	CAppManagerに登録してあるので、同一スレッドならばどこからでも
	CAppManager::GetMyFrameで取得することが出来ます。
	必要に応じて、こいつをダウンキャストして使うと良いでしょう。

*/
public:
	///	スレッドの正当性
	virtual bool IsThreadValid()const { return GetMyApp()->IsThreadValid(); }
	/**
		MainThreadのなかでは、この関数がtrueの間、まわり続けるようにします。
		（ただし、これがtrueであっても強制的に抜けても構いません）
	*/

	///	スレッドの無効化
	virtual void	InvalidateThread() { GetMyApp()->InvalidateThread(); }
	/**
		この関数を実行すると、IsThreadValidがfalseを返すようになります。
		CAppManager::GetMyFrameと絡めて、外部からこのフレームスレッドに
		停止要求を出すのに使うと便利です。
	*/

	///	呼び出し元のアプリクラスの取得
	virtual IAppBase* GetMyApp()const { return m_lpMyApp; }
	/**
		この関数を呼び出すことで、自分の属するアプリケーションクラスの
		取得が出来ます。（ほとんど使う必要はないはずですが）
	*/

	virtual void	Start();
	/**
		このクラスでの実行に際しては、MainThreadをオーバーライドしておき
		このメソッドを呼び出してください。そうすれば初期化が行なわれたあと
		MainThreadが呼び出されます。
	*/

	virtual void	MesSleep(int nTime);
	/**
		Windowsメッセージ処理をしないと、
		IntervalTimerでのコールバックがされないので、
		WindowMessage処理を行ないながらSleepする関数です。
		nTime[ms]の間、Sleepします。
		また、少なくとも、nTime[ms]は経過するまでは、この関数から抜けません。
		class CVolumeFaderでフェードさせるときなどに呼び出すと
		効果的かも知れません。
	*/

	virtual void	SetMainApp(bool b) { GetMyApp()->SetMainApp(b); }
	virtual bool	IsMainApp() const { return GetMyApp()->IsMainApp(); }
	/**
		メインアプリであるかを設定／取得する
		（default : false）

		メインアプリに設定しておけばこのCAppFrame派生クラスが
		終了するときに、自動的に他のCAppBase派生クラスのインスタンスを
		終了させる。
	*/

private:
	IAppBase* m_lpMyApp;
};

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

#endif
