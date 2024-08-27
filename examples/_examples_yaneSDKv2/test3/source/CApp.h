
#ifndef __CApp_h__
#define __CApp_h__

#include "../../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {
public:

	CSELoader*	 GetSELoader(void) { return& m_SELoader; }
	CBGMLoader*	 GetBGMLoader(void) { return& m_BGMLoader; }
	CDirectDraw* GetDraw(void)	{ return& m_Draw; }
	CKey*		 GetKey(void)	{ return& m_Key;  }

protected:
	//	‚±‚¢‚Â‚ªƒƒCƒ“‚Ë
	void MainThread(void);		  //  ‚±‚ê‚ªÀs‚³‚ê‚é

	CDirectDraw m_Draw;
	CKey		m_Key;
	CBGMLoader  m_BGMLoader;
	CSELoader   m_SELoader;
};

#endif
