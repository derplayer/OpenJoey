#ifndef __CApp_h__
#define __CApp_h__

class CApp : public CAppFrame {

	virtual void MainThread();

	///	描画
	CFastDraw* GetDraw() { return GetDrawFactory()->GetDraw(); }
	CFastPlaneFactory* GetDrawFactory() { return &planeFactory_; }

protected:
	CFastPlaneFactory	planeFactory_;
	//	↑こいつがCFastDrawを内包しているので、こいつ経由で描画すれば良い

	CSoundFactory		soundFactory_;
	//	↑Soundについても同様
};

#endif // __CApp_h__