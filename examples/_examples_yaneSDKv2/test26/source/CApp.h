
#ifndef __CApp_h__
#define __CApp_h__

#include "../../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {
public:

	CFastDraw* 	GetDraw()			{ return& m_vDraw; }
	CKey*		GetKey()			{ return& m_vKey;  }
	CMouseEx*	GetMouse()			{ return& m_vMouse;}
//	CSceneTransiter* GetTransiter()  { return& m_vTransiter;}

protected:
	//	���������C����
	void MainThread(void);		  //  ���ꂪ���s�����

	CFastDraw		m_vDraw;
	CMouseEx		m_vMouse;
	CKey			m_vKey;
//	CSceneTransiter	m_vTransiter;
};

//	���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
	virtual void MainThread(void){			//	���ꂪ���[�J�[�X���b�h
		CApp().Start();						//	CApp app; app.Start();�̈Ӗ���
	}
};

#endif
