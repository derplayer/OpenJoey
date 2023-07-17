
#ifndef __CApp_h__
#define __CApp_h__

#include "../../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {
public:

	CFastDraw* 	GetDraw()			{ return& m_vDraw; }
	CKey*		GetKey()			{ return& m_vKey;  }
	CMouseEx*	GetMouse()			{ return& m_vMouse;}
//	CSound*		GetSound()		{ return& m_vSound;}
//	CSceneTransiter* GetTransiter()  { return& m_vTransiter;}
	CFPSTimer*	GetFPSTimer()	{ return& m_vFps; }
	//	FPS設定
	void	SetFPS(int nFPS=30){
		GetFPSTimer()->SetFPS(nFPS);	//	テキトーにredrawしてなちゃい＾＾；
	}

protected:
	//	こいつがメインね
	void MainThread(void);		  //  これが実行される

	void CircleBarCountdown(bool IsBlur);
	void LineBarCountdown(bool IsBlur);

	CFastDraw		m_vDraw;
	CMouseEx		m_vMouse;
	CKey			m_vKey;
//	CSound			m_vSound;
//	CSceneTransiter	m_vTransiter;
	CFPSTimer		m_vFps;
	CDbg			m_check;
};

//	これがmain windowのためのクラス。
class CAppMainWindow : public CAppBase {	//	アプリケーションクラスから派生
	virtual void MainThread(void){			//	これがワーカースレッド
		CApp().Start();						//	CApp app; app.Start();の意味ね
	}
};

#endif
