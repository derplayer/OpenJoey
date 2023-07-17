#pragma once
#include "../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {

	virtual void MainThread();

	///	•`‰æ
	CFastDraw* GetDraw() { return planeFactory_.GetDraw(); }

protected:
	CFastPlaneFactory	planeFactory_;
	CSoundFactory		soundFactory_;
};
