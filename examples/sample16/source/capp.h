
#ifndef __CApp_h__
#define __CApp_h__

#include "../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {
public:

//	CFastDraw*		GetDraw()	{ return& m_vDraw; }
	CFastDraw*		GetDraw()	{ return m_vDrawFactory.GetDraw(); }
	CVirtualKey*	GetKey()	{ return& m_vKey;  }

protected:
	void MainThread();

//	CFastDraw	m_vDraw;
	CFastPlaneFactory m_vDrawFactory;
	//	CSpriteChara�̓T�[�t�F�[�X�̐�����
	//	CPlane�ɐݒ肳��Ă���factory���g�p����̂�

	CKey1		m_vKey;
};

#endif
