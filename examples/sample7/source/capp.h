#pragma once
#include "../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {
public:

	virtual void MainThread();

	void	ConvertText(const string& inText,string& outText);
	/**
		���ϊ������Ⴄ�I
	*/

protected:
};
