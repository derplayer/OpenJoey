#pragma once
#include "../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {
public:

	virtual void MainThread();

	void	ConvertText(const string& inText,string& outText,
		bool bConstDef,bool bSynonymDef);
	/**
		ª•ÏŠ·‚µ‚¿‚á‚¤I
	*/

};
