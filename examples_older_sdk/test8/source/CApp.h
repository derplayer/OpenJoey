
#ifndef __CApp_h__
#define __CApp_h__

#include "../../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {
public:

	CDIBDraw*	GetDraw(void)	{ return& m_Draw; }
	CKey*		GetKey(void)	{ return& m_Key;  }

protected:
	//	���������C����
	void MainThread(void);		  //  ���ꂪ���s�����

	CDIBDraw	m_Draw;
	CKey		m_Key;
};

#endif
