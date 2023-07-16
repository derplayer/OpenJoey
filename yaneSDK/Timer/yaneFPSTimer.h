// yaneFPSTimer.h
//
//	正確かつ無駄なくウェイトを掛ける
//
//	SetFPSでFPSをセットして、あとは、描画前後にでも、Wait()するだけで良い。
//

#ifndef __yaneFPSTimer_h__
#define __yaneFPSTimer_h__


#include "yaneTimer.h"

namespace yaneuraoGameSDK3rd {
namespace Timer {

class IFPSTimer {
public:
	virtual void SetFPS(DWORD fps)=0;
	virtual DWORD GetFPS()=0;
	virtual DWORD GetRealFPS()=0;
	virtual DWORD GetCPUPower()=0;
	virtual DWORD GetSkipFrame() const =0;
	virtual void ResetTimer()=0;
	virtual void WaitFrame()=0;
	virtual bool ToBeSkip() const=0;
	virtual void ResetElapseTimeCounter()=0;
	virtual DWORD GetElapseTimeCounter()=0;
	virtual void ResetCallCounter()=0;
	virtual DWORD GetCallCounter()=0;

	virtual ~IFPSTimer(){}
};

class CFPSTimer : public IFPSTimer {
/**
	時間を効率的に待つためのタイマー。
	フレームレート（一秒間の描画枚数）を６０ＦＰＳ（Frames Par Second）に
	調整する時などに使う。

*/
public:
	virtual void SetFPS(DWORD fps);
	/**
		FPSの設定（イニシャライズを兼ねる）
		ディフォルトでは６０ＦＰＳ。
	*/

	virtual DWORD GetFPS();
	///	 FPSの取得

	virtual DWORD GetRealFPS();
	/**
		 FPSの取得（測定値）
		1秒間に何回WaitFrameを呼び出すかを、
		前回32回の呼び出し時間の平均から算出する。
	*/

	virtual DWORD GetCPUPower();
	/**
		 CPU稼動率の取得（測定値）
		ＣＰＵの稼働率に合わせて、0～100の間の値が返る。
		ただしこれは、WaitFrameでSleepした時間から算出されたものであって、
		あくまで参考値である。
	*/

	virtual DWORD GetSkipFrame() const;
	/**
	SetFPSされた値までの描画に、ToBeSkipがtrueになっていたフレーム数を返す。
	ただし、ここで言うフレーム数とは、WaitFrameの呼び出しごとに
	１フレームと計算。
	*/

	virtual void ResetTimer();
	/**
		待ち時間の端数のリセット（普通、使う必要はない）
		WaitFrameでは、前回のWaitFrameを呼び出した時間を、今回の待ち時間を
		算出するのに使う（SetFPSで設定した値に近づけようとするため、前回、
		不足していた時間を今回の待ち時間から減らす）ので、前回の呼び出し時間を
		現在時にすることによって、今回の待ち時間への修正項を消すのがこの関数。
		通常、使うことは無い。
	*/

	virtual void WaitFrame();
	/**
		１フレーム分の時間が来るまで待つ
	　	メインループのなかでは、描画処理を行なったあと、
		このWaitFrameを呼び出せば、SetFPSで設定した
		フレームレートに自動的に調整される。
	*/

	virtual bool ToBeSkip() const;
	/**	
		次のフレームをスキップすべきか？
	もし、これがtrueを返してくるならば、そのフレームの描画をまるごと
	スキップしてしまえば（ただし、キャラの移動等の計算だけは行なう）、
	秒間のＦＰＳは遅いマシンでも一定と仮定してプログラムすることが出来る。
	ただし、そのときもWaitFrameは呼び出すこと。
	詳しい解説は、やねうらおのホームページ
		http://www.sun-inet.or.jp/~yaneurao/
	の天才ゲームプログラマ養成ギプス第２１章を参照のこと。

	また、フレームをスキップするかどうかは、WaitFrameの段階で確定する。
	そのため、次のWaitFrameを呼び出すまで、ToBeSkipは同じ真偽値を返し続ける。
	*/


	/**
		WaitFrameで消費した時間を累計でカウントしている。
		そのカウンタのりセットと取得するのがこの関数である。
	*/
	virtual void ResetElapseTimeCounter();
	///		消費時間のリセット
	virtual DWORD GetElapseTimeCounter();
	///		消費時間の取得

	/**
		WaitFrameを呼び出すごとに内部的な呼び出しカウンタが
		インクリメントされる。そのカウンタをリセットするのと取得するのが
		この関数である。
	*/
	virtual void ResetCallCounter();
	///		呼出しカウンタのリセット
	virtual DWORD GetCallCounter();
	///		呼出しカウンタの取得

	CFPSTimer();
	virtual ~CFPSTimer();

protected:
	DWORD m_dwFPS;		  //  FPS(ディフォルトで60)
	DWORD m_dwFPSWait;	  // =1000*0x10000/FPS; // 60FPSに基づくウェイト
	DWORD m_dwLastDraw;	  // 前回の描画時刻
	DWORD m_dwFPSWaitTT;  // 前回発生した端数時間の保持のため
	DWORD m_dwElapsedTime;	  // 前回リセットされてからElapseされた時間
	int	  m_nUnuseSleep;	  // Wait関数内でSleepを利用するか
	DWORD m_dwDrawTime[32];	   // FPS測定用の描画時間計算用
	DWORD m_dwElapseTime[32];  // CPU Power測定用
	DWORD m_nDrawCount;		   // （ここ32回の呼出し時間の平均からFPSを求める）
	DWORD m_dwCallCount;	// WaitFrameを呼び出された回数

	bool	m_bFrameSkip;		 //	次のフレームはスキップするのか？
	DWORD	m_dwFrameSkipCounter; // 　フレームスキップカウンタ
	DWORD	m_dwFrameSkipCounterN; // 計測中のフレームスキップカウンタ

	CTimer	m_vTimer;
	CTimer* GetTimer() { return &m_vTimer; }
};

} // end of namespace Timer
} // end of namespace yaneuraoGameSDK3rd

#endif
