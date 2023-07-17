
#ifndef __CApp_h__
#define __CApp_h__

#include "../../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {
public:

	CDIBDraw* 	GetDraw()			{ return& m_vDraw; }
	CKey*		GetKey()			{ return& m_vKey;  }
	CMouseEx*	GetMouse()			{ return& m_vMouse;}
	CSceneTransiter* GetTransiter()  { return& m_vTransiter;}

protected:
	//	���������C����
	void MainThread(void);		  //  ���ꂪ���s�����

	CDIBDraw		m_vDraw;
	CMouseEx		m_vMouse;
	CKey			m_vKey;
	CSceneTransiter	m_vTransiter;
};

#endif
