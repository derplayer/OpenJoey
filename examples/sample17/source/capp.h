
#ifndef __CApp_h__
#define __CApp_h__

#include "../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {
public:

	CVirtualKey*	GetKey()	{ return& m_Key;  }
//	CFastDraw*		GetDraw()	{ return& m_Draw; }
	CFastDraw* GetDraw() { return planeFactory_.GetDraw(); }

protected:
	//	‚±‚¢‚Â‚ªƒƒCƒ“‚Ë
	void	MainThread(void);		  //  ‚±‚ê‚ªÀs‚³‚ê‚é

//	CFastDraw	m_Draw;
	CFastPlaneFactory	planeFactory_;
	CKey1		m_Key;

};

#endif
