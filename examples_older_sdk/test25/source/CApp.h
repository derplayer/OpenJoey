
#ifndef __CApp_h__
#define __CApp_h__

#include "../../../yaneSDK/yaneSDK.h"

#define FAST_PLANE_MODE
//	↑これをdefineしておけばCFastPlaneを使う
//#define PLANE_MODE
//	↑これをdefineしておけばCPlaneを使う
//	両方defineされていなければCDIB32を使う

class CApp : public CAppFrame {
public:

#ifdef FAST_PLANE_MODE
	CFastDraw* 	GetDraw()			{ return& m_vDraw; }
#else
#ifdef PLANE_MODE
	CDirectDraw* GetDraw()			{ return& m_vDraw; }
#else
	CDIBDraw* 	GetDraw()			{ return& m_vDraw; }
#endif
#endif
//	CKey*		GetKey()			{ return& m_vKey;  }
	CMouseEx*	GetMouse()			{ return& m_vMouse;}
//	CSceneTransiter* GetTransiter()  { return& m_vTransiter;}

protected:
	//	こいつがメインね
	void MainThread(void);		  //  これが実行される

#ifdef FAST_PLANE_MODE
	CFastDraw		m_vDraw;
#else
#ifdef PLANE_MODE
	CDirectDraw 	m_vDraw;
#else
	CDIBDraw		m_vDraw;
#endif
#endif
	CMouseEx		m_vMouse;
//	CKey			m_vKey;
//	CSceneTransiter	m_vTransiter;
};

//	これがmain windowのためのクラス。
class CAppMainWindow : public CAppBase {	//	アプリケーションクラスから派生
	virtual void MainThread(void){			//	これがワーカースレッド
		CApp().Start();						//	CApp app; app.Start();の意味ね
	}
	virtual LRESULT OnPreCreate(CWindowOption& opt){
		opt.caption		= "らーじぽんぽん＾＾；";
		opt.classname	= "YANEAPPLICATION";
		opt.size_x		= 320;
		opt.size_y		= 240;
		opt.style		= WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
		return 0;
	}
};

#endif
