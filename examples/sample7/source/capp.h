#pragma once
#include "../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {
public:

	virtual void MainThread();

	void	ConvertText(const string& inText,string& outText);
	/**
		↑変換しちゃう！
	*/

protected:
};
