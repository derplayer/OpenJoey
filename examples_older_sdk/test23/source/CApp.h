
#ifndef __CApp_h__
#define __CApp_h__

#include "../../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {
public:

	CDIBDraw* 	GetDraw()			{ return& m_vDraw; }
	CKey*		GetKey()			{ return& m_vKey;  }
	CMouseEx*	GetMouse()			{ return& m_vMouse;}
	CSceneTransiter* GetTransiter()  { return& m_vTransiter;}

	//	--- WM_CLOSEˆ—Œn ---
	CApp(){
		m_bWindowClosing	= false;
	}
	void		Close() { GetMyApp()->Close(); }

	// WM_CLOSE‚É‘Î‚·‚éˆ—
	LRESULT	OnPreClose(void);

protected:
	//	‚±‚¢‚Â‚ªƒƒCƒ“‚Ë
	void MainThread(void);		  //  ‚±‚ê‚ªÀs‚³‚ê‚é

	CDIBDraw		m_vDraw;
	CMouseEx		m_vMouse;
	CKey			m_vKey;
	CSceneTransiter	m_vTransiter;

	//	Window•Â‚¶‚æ‚¤‚Æ‚µ‚Ä‚ñ‚Ì‚©H
	bool			m_bWindowClosing;
};

#endif
