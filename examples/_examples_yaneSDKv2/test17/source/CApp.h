
#ifndef __CApp_h__
#define __CApp_h__

#include "../../../yaneSDK/yaneSDK.h"

class CApp;

class CMyMenu : public CWinHook {
public:
	CMyMenu(CApp* lp) {
		m_lpApp = lp;
		CAppInitializer::Hook(this);
//		CheckDisplay( 0 );
	}
	virtual ~CMyMenu(){
		CAppInitializer::Unhook(this);
	}
protected:
	//	override from CWinHook
	virtual LRESULT	WndProc(HWND,UINT,WPARAM,LPARAM);

	void	SetDisplayMode( int nBpp );
	void	CheckDisplay( int nDisplay );

	CApp*	GetApp(void) { return m_lpApp; }
private:
	CApp* m_lpApp;
};

class CApp : public CAppFrame {
public:

	CDIBDraw*	GetDraw(void)	{ return& m_Draw; }
	CMyMenu*	GetMenu(void)	{ return& m_Menu; }
	int*		GetDisplayMode(void) { return& m_nDisplayMode; }

	CApp(void);

protected:
	//	Ç±Ç¢Ç¬Ç™ÉÅÉCÉìÇÀ
	void	MainThread(void);		  //  Ç±ÇÍÇ™é¿çsÇ≥ÇÍÇÈ

	CDIBDraw	m_Draw;
	CMyMenu		m_Menu;

	int			m_nDisplayMode;
};

#endif
