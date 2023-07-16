// yaneIntervalTimer.h :
//
//		インターバルタイマ
//		（ただし、手動式コールバックで使う）
//
//		programmed & designed by yaneurao(M.Isozaki) '02/03/20
//

#ifndef __yaneIntervalTime_h__
#define __yaneIntervalTime_h__


#include "yaneTimer.h"

namespace yaneuraoGameSDK3rd {
namespace Timer {

class IIntervalTimer {
public:
	virtual void	RegistCallBack(void*pVoid,DWORD dwNext,
			DWORD dwInterval,int nRepeat,
			const smart_ptr<function_callback>&fn)=0;
	virtual void	DeleteCallBack(void*pVoid)=0;
	virtual void	CallBack()=0;
	virtual void	SetTimer(const smart_ptr<ITimer>& pTimer)=0;

	virtual ~IIntervalTimer() {}
};

class CIntervalTimer : public IIntervalTimer {
/**
	インターバルタイマを統合的に管理するためのクラス
		（ただし、手動式コールバックで使う）
	コールバック自体は、class CAppBase が、IsThreadValidのなかで
	行なってくれるので、CAppBaseの利用者は、明示的に行なう必要は無い

	使用例：
	//	CAppFrame::MainThread内にて．．
		//	コールバックの関数定義
		smart_ptr<function_callback> fn(
			function_callback_v::Create(&CTest::Test,&test)
		);
		//	コールバックの登録
		GetMyApp()->GetIntervalTimer()->RegistCallBack(
			&test,	//	コールバック登録されているものを
					//	削除するための識別子
			3000,	//	3000ms後に一回目のコールバック
			1000,	//	1000msごとに
			10,		//	10回コールバック
			fn
		);
*/
public:

	virtual void	RegistCallBack(void*pVoid,DWORD dwNext,
			DWORD dwInterval,int nRepeat,
			const smart_ptr<function_callback>&fn);
	/**
		コールバックの登録を行なう。pVoidは、タイマ識別子であり、
		ユーザーがそのクラスのthisポインタを渡しておけば、あとで、
		これを識別子として用いて、登録したコールバックを削除することが出来る

		dwNext	は、１回目のコールバックは何ミリ秒後なのか。
			その時刻がすぎたときに、コールバックがかけられる

		dwIntervalは、１回目以降のコールバックの間隔。
			nRepeatが1のときは関係ない。

		nRepeatは、何回コールバックを行なうのかの指定。
		これを指定した回数だけ、nCallBack間隔のコールバックが行なわれる
		0ならば、無限回数、呼び出す

		fnは、コールバックすべき関数。
	*/

	virtual void	DeleteCallBack(void*pVoid);
	///		RegistCallBackで指定したpVoidと一致するコールバックをすべて消す

	virtual void	CallBack();
	/**
		登録されているコールバックのうち、時間になっているものを
		順番に呼び出す。

		class CAppBase は、IsThreadValidで、この関数を夜斐出しているので、
		CAppBaseのMainThread内では、明示的にこの関数を呼び出す必要は無い。
	*/

	virtual void	SetTimer(const smart_ptr<ITimer>& pTimer);
	/**
		CFixTimer等を使うのならば、
		事前に用意されているタイマーをこいつで置き換えると良い。
		ディフォルトではCTimerが使われる
	*/

	CIntervalTimer();
	virtual ~CIntervalTimer();

protected:
	//	タイマーコールバック構造体
	struct CTimerInfo {
	void*					m_pVoid;
	/**
		タイマー識別子（ユーザーがそのクラスのthisポインタを）
		渡しておけば、そのクラスが生成したタイマのみを消去することが出来る
	*/
	DWORD					m_dwCallBackTimer;
	//	↑タイマーが、この時刻を過ぎれば、コールバックする

	DWORD					m_dwInterval;
	//	コールバックの間隔[ms]

	int						m_nRepeat;
	//	残り、呼び出す回数(0ならば、無限に呼び出す)

	smart_ptr<function_callback>		m_fnCallBack;
	///	コールバックすべき関数

	bool					m_bDelete;
	///	このオブジェクトは次のフェイズで消されるのか？
	///	iteratorがsmart_iteratorでは無いので、フェイズを遅らせて
	///	削除してやる必要がある
	bool	IsValid() const { return! m_bDelete; }
	///	⇒　smart_list_ptrのイテレータ抽象使うのに必要

	};
	typedef smart_list_ptr<CTimerInfo> CTimerList;

	CTimerList	m_listTimerInfo;
	CTimerList* GetTimerInfo(){ return& m_listTimerInfo;}

	smart_ptr<ITimer>	m_pTimer;
	smart_ptr<ITimer>	GetTimer() const { return m_pTimer; }
	//	タイマーカウントは、こいつで取得する
};

} // end of namespace Timer
} // end of namespace yaneuraoGameSDK3rd

#endif
