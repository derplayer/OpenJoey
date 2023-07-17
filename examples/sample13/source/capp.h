
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
	//	CSpriteCharaはサーフェースの生成に
	//	CPlaneに設定されているfactoryを使用するのだ

	CKey1		m_vKey;
};

#endif
