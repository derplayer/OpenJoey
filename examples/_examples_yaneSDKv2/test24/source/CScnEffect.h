#ifndef __CSCNEFFECT_H__
#define __CSCNEFFECT_H__

#include "../../../yaneSDK/yaneSDK.h"

class CMyScenarioEffectFactory : public CScenarioEffectFactory {
	virtual CScenarioEffect* CreateInstance(int nNo);
};

#endif //__CSCNEFFECT_H__
