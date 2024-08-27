
#ifndef __CApp_h__
#define __CApp_h__

#include "../../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {
public:

	CDIBDraw*	GetDraw()		{ return& m_vDraw; }
	CKey*		GetKey()		{ return& m_vKey;  }
	CMouseEx*	GetMouse()		{ return& m_vMouse; }
	CMouseLayer*GetMouseLayer()	{ return& m_vMouseLayer; }

protected:
	//	‚±‚¢‚Â‚ªƒƒCƒ“‚Ë
	void	MainThread(void);		  //  ‚±‚ê‚ªÀs‚³‚ê‚é

	CDIBDraw	m_vDraw;
	CKey		m_vKey;
	CMouseEx	m_vMouse;
	CMouseLayer m_vMouseLayer;
};

#endif
