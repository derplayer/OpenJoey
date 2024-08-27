
#ifndef __CApp_h__
#define __CApp_h__

#include "../../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {
public:

	CDIBDraw* 	GetDraw(void)	{ return& m_vDraw; }
	CKey*		GetKey(void)	{ return& m_vKey;  }
	CMouseEx*	GetMouse(void)	{ return& m_vMouse;}

protected:
	//	‚±‚¢‚Â‚ªƒƒCƒ“‚Ë
	void MainThread(void);		  //  ‚±‚ê‚ªÀs‚³‚ê‚é

	CDIBDraw	m_vDraw;
	CMouseEx	m_vMouse;
	CKey		m_vKey;
};

#endif
