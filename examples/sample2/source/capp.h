#ifndef __CApp_h__
#define __CApp_h__

class CApp : public CAppFrame {

	virtual void MainThread();

	///	�`��
	CFastDraw* GetDraw() { return GetDrawFactory()->GetDraw(); }
	CFastPlaneFactory* GetDrawFactory() { return &planeFactory_; }

protected:
	CFastPlaneFactory	planeFactory_;
	//	��������CFastDraw�����Ă���̂ŁA�����o�R�ŕ`�悷��Ηǂ�

	CSoundFactory		soundFactory_;
	//	��Sound�ɂ��Ă����l
};

#endif // __CApp_h__