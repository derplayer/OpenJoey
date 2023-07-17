
#ifndef __CApp_h__
#define __CApp_h__

#include "../../../yaneSDK/yaneSDK.h"

#define FAST_PLANE_MODE
//	�������define���Ă�����CFastPlane���g��
//#define PLANE_MODE
//	�������define���Ă�����CPlane���g��
//	����define����Ă��Ȃ����CDIB32���g��

class CApp : public CAppFrame {
public:

#ifdef FAST_PLANE_MODE
	CFastDraw* 	GetDraw()			{ return& m_vDraw; }
#else
#ifdef PLANE_MODE
	CDirectDraw* GetDraw()			{ return& m_vDraw; }
#else
	CDIBDraw* 	GetDraw()			{ return& m_vDraw; }
#endif
#endif
//	CKey*		GetKey()			{ return& m_vKey;  }
	CMouseEx*	GetMouse()			{ return& m_vMouse;}
//	CSceneTransiter* GetTransiter()  { return& m_vTransiter;}

protected:
	//	���������C����
	void MainThread(void);		  //  ���ꂪ���s�����

#ifdef FAST_PLANE_MODE
	CFastDraw		m_vDraw;
#else
#ifdef PLANE_MODE
	CDirectDraw 	m_vDraw;
#else
	CDIBDraw		m_vDraw;
#endif
#endif
	CMouseEx		m_vMouse;
//	CKey			m_vKey;
//	CSceneTransiter	m_vTransiter;
};

//	���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
	virtual void MainThread(void){			//	���ꂪ���[�J�[�X���b�h
		CApp().Start();						//	CApp app; app.Start();�̈Ӗ���
	}
	virtual LRESULT OnPreCreate(CWindowOption& opt){
		opt.caption		= "��[���ۂ�ۂ�O�O�G";
		opt.classname	= "YANEAPPLICATION";
		opt.size_x		= 320;
		opt.size_y		= 240;
		opt.style		= WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
		return 0;
	}
};

#endif
