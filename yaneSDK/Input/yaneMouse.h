// MouseInput.h :
//	マウス入力用
//		programmed by yaneurao(M.Isozaki) '99/7/31
//
//	たいしたクラスではない。:p
//
//		※　ダブルクリックの感知はしない。
//

#ifndef __yaneMouseInput_h__
#define __yaneMouseInput_h__


#include "../Window/yaneWinHook.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

class IMouse {
public:
	virtual LRESULT GetXY(int &x,int &y)const=0;
	virtual bool	RButton()const=0;
	virtual bool	LButton()const=0;
	virtual LRESULT GetInfo(int &x,int &y,int &b)const=0;
	virtual void	GetButton(bool&bL,bool&bR)=0;
	virtual void	ResetButton()=0;
	virtual LRESULT SetXY(int x,int y)=0;
	virtual void	SetOutScreenInput(bool bEnable)=0;

	virtual ~IMouse(){}
};

class CMouse : public IWinHook,public IMouse {
/**
	マウスのリアルタイム状態取得用のクラスです。

	先行してウィンドゥが完成している必要があるので
	class CAppFrame 派生クラス内で使用するようにしてください。
*/
public:
	virtual LRESULT GetXY(int &x,int &y)const;
	///		マウスポジションを得る（クライアント座標系にて）

	virtual bool	RButton()const;
	///		右ボタン状態を得る（現在のリアルタイムの情報）

	virtual bool	LButton()const;
	///		左ボタン状態を得る（現在のリアルタイムの情報）

	virtual LRESULT GetInfo(int &x,int &y,int &b)const;
	///	マウスポジションとボタン状態を返す
	///	(b:右ボタン押下ならば+1,左ボタン押下ならば+2 両方ならば+1+2==+3)

	virtual void	GetButton(bool&bL,bool&bR);
	///	前回のGetButtonから押されたか？
	virtual void	ResetButton();
	///	GetButton↑で取得できるボタン状態のリセット

	virtual LRESULT SetXY(int x,int y);
	///	マウスを指定のポジションに移動（クライアント座標系にて）

	virtual void	SetOutScreenInput(bool bEnable);
	/**
		押したまま、マウスを画面外にやったとき、どうなるのか？
			true  == ボタンは押したままだと見なされる
			false == ボタンは離されたものと見なされる
			defaultではfalse
	*/

	CMouse();
	virtual ~CMouse();

protected:
	LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam); // メッセージのコールバック

	bool	m_bRB;					//　マウスボタン状態
	bool	m_bLB;
	bool	m_bHistRB;				//	履歴
	bool	m_bHistLB;
	bool	m_bOutScreenInput;		//	画面外での入力
};

class CFixMouse : public IMouse {
/**
	マウスのリアルタイム情報の取得のためのクラスです

	ゲームでCMouseを使う場合、１フレームの間は、
	固定値が返ってきたほうが望ましい。

	class CTimer に対する CFixTimer の関係と同じである。
*/
public:
	///	フラッシュさせる
	virtual LRESULT	Flush();
	/**
		マウスの座標、ボタン状態を更新する。
		これをした瞬間の状態に基づいて各メンバ関数で値が返るようになる。
		あとはほとんど、class CMouse と同じ。ただし、
	*/

	virtual LRESULT GetXY(int &x,int &y)const;
	virtual bool	RButton()const;
	virtual bool	LButton()const;
	virtual LRESULT GetInfo(int &x,int &y,int &b)const;

	///	前回のFlushから押されたか？
	virtual void	GetButton(bool&bL,bool&bR);
	virtual bool	IsPushRButton()const;
	virtual bool	IsPushLButton()const;

	///	前回のFlushから押し上げられたか？
	virtual void	GetUpButton(bool&bL,bool&bR);
	virtual bool	IsPushUpRButton()const;
	virtual bool	IsPushUpLButton()const;

	virtual void	ResetButton();
	///	↑ボタン状態リセット

	virtual LRESULT SetXY(int x,int y);
	///	マウスを指定のポジションに移動（クライアント座標系にて）
	///	これで座標を移動させた場合、FlushしなくともGetXYすればその座標が返る。

	/**
		ガードタイムとは、シーン管理をしていたりするとき、ボタンが押されて、
		次のシーンに移動して、次のシーンでいきなりボタンが押されたと判定されて
		しまうことを防止するために、一定時間「入力が無い」と嘘を返すための
		機構です。SetGuardTimeで設定した数だけFlushメンバ関数を呼び出すまでは、
		GetButton/IsPushRButton/IsPushLButtonではすべてボタンは
		押されていないと返ります。
	*/
	///	ガードタイム中かどうかを返す
	virtual bool	IsGuardTime()const;
	///	ガードタイムを設定する
	void	SetGuardTime(int nTime);

	virtual void	SetOutScreenInput(bool bEnable)
		{ GetMouse()->SetOutScreenInput(bEnable);}

	CFixMouse();
	virtual ~CFixMouse();

protected:
	bool	m_bRBN;					//　Flushしたときのマウスボタン状態
	bool	m_bLBN;
	int		m_nRLBN;				//	Flushしたときのマウスボタン状態
	int		m_nX,m_nY;				//	Flushしたときのマウスのポジション
	int		m_nGuardTime;			//	ガードタイム
	bool	m_bHistRB;				//	履歴
	bool	m_bHistLB;

	CMouse	m_vMouse;				//	こいつに委譲
	CMouse* GetMouse() { return &m_vMouse; }
};

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd

#endif
