
#ifndef __CApp_h__
#define __CApp_h__

#include "../../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {
public:

	CFastDraw* 	GetDraw()			{ return& m_vDraw; }
	CKey*		GetKey()			{ return& m_vKey;  }
	CMouseEx*	GetMouse()			{ return& m_vMouse;}
//	CSceneTransiter* GetTransiter()  { return& m_vTransiter;}

protected:
	//	こいつがメインね
	void MainThread(void);		  //  これが実行される

	CFastDraw		m_vDraw;
	CMouseEx		m_vMouse;
	CKey			m_vKey;
//	CSceneTransiter	m_vTransiter;
};

//	これがmain windowのためのクラス。
class CAppMainWindow : public CAppBase {	//	アプリケーションクラスから派生
	virtual void MainThread(void){			//	これがワーカースレッド
		CApp().Start();						//	CApp app; app.Start();の意味ね
	}
};

#endif
