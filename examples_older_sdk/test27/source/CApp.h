
#ifndef __CApp_h__
#define __CApp_h__

#include "../../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {
public:

	CFastDraw* 	GetDraw()			{ return& m_vDraw; }
	CKey*		GetKey()			{ return& m_vKey;  }
	CMouseEx*	GetMouse()			{ return& m_vMouse;}
//	CSound*		GetSound()		{ return& m_vSound;}
//	CSceneTransiter* GetTransiter()  { return& m_vTransiter;}
	CFPSTimer*	GetFPSTimer()	{ return& m_vFps; }
	//	FPS�ݒ�
	void	SetFPS(int nFPS=30){
		GetFPSTimer()->SetFPS(nFPS);	//	�e�L�g�[��redraw���ĂȂ��Ⴂ�O�O�G
	}

protected:
	//	���������C����
	void MainThread(void);		  //  ���ꂪ���s�����

	void CircleBarCountdown(bool IsBlur);
	void LineBarCountdown(bool IsBlur);

	CFastDraw		m_vDraw;
	CMouseEx		m_vMouse;
	CKey			m_vKey;
//	CSound			m_vSound;
//	CSceneTransiter	m_vTransiter;
	CFPSTimer		m_vFps;
	CDbg			m_check;
};

//	���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
	virtual void MainThread(void){			//	���ꂪ���[�J�[�X���b�h
		CApp().Start();						//	CApp app; app.Start();�̈Ӗ���
	}
};

#endif
