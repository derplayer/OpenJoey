
#ifndef __CApp_h__
#define __CApp_h__

#include "../../../yaneSDK/yaneSDK.h"

//	����define�����Ă����΁ACFastDraw���g���ăR���p�C�������
//	�����Ȃ��΁ACDIBDraw���g���ăR���p�C�������
#define _USE_FastDraw

class CApp : public CAppFrame {
public:

#ifdef _USE_FastDraw
	CFastDraw*	GetDraw(void)	{ return& m_Draw; }
#else
	CDIBDraw*	GetDraw(void)	{ return& m_Draw; }
#endif
	CKey*		GetKey(void)	{ return& m_Key;  }

protected:
	//	���������C����
	void	MainThread(void);		  //  ���ꂪ���s�����

#ifdef _USE_FastDraw
	CFastDraw	m_Draw;
#else
	CDIBDraw	m_Draw;
#endif
	CKey		m_Key;
};

#endif
