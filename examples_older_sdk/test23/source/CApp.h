
#ifndef __CApp_h__
#define __CApp_h__

#include "../../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {
public:

	CDIBDraw* 	GetDraw()			{ return& m_vDraw; }
	CKey*		GetKey()			{ return& m_vKey;  }
	CMouseEx*	GetMouse()			{ return& m_vMouse;}
	CSceneTransiter* GetTransiter()  { return& m_vTransiter;}

	//	--- WM_CLOSE処理系 ---
	CApp(){
		m_bWindowClosing	= false;
	}
	void		Close() { GetMyApp()->Close(); }

	// WM_CLOSEに対する処理
	LRESULT	OnPreClose(void);

protected:
	//	こいつがメインね
	void MainThread(void);		  //  これが実行される

	CDIBDraw		m_vDraw;
	CMouseEx		m_vMouse;
	CKey			m_vKey;
	CSceneTransiter	m_vTransiter;

	//	Window閉じようとしてんのか？
	bool			m_bWindowClosing;
};

#endif
