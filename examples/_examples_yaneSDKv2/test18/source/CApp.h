
#ifndef __CApp_h__
#define __CApp_h__

#include "../../../yaneSDK/yaneSDK.h"

#define FAST_PLANE_MODE
// ���@�����define�����CFastPlane���g��

class CApp : public CAppFrame {
public:

#ifdef FAST_PLANE_MODE
	CFastDraw* 	GetDraw(void)	{ return& m_Draw; }
#else
	CDIBDraw* 	GetDraw(void)	{ return& m_Draw; }
#endif
	CKey*		GetKey(void)	{ return& m_Key;  }

protected:
	//	���������C����
	void MainThread(void);		  //  ���ꂪ���s�����

#ifdef FAST_PLANE_MODE
	CFastDraw	m_Draw;
#else
	CDIBDraw	m_Draw;
#endif
	CKey		m_Key;
};

#endif
