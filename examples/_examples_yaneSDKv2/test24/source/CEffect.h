#ifndef __CEFFECT_H__
#define __CEFFECT_H__

#include "../../../yaneSDK/yaneSDK.h"

class CEffect {
public:
	CEffect(void);
	~CEffect();

public:
	LRESULT ShadeOff(CDIB32*lpDib,int nEffectLevel,LPRECT lpRect = NULL );
	LRESULT MosaicEffect(CDIB32*lpDib,int nEffectLevel,LPRECT lpRect = NULL);

private:
	RECT GetClipRect(LPRECT lpRect1,LPRECT lpRect2){
		RECT rc1 = *lpRect1;
		RECT rc2 = *lpRect2;

	if (rc2.left > rc1.left)	{ rc1.left   = rc2.left;  }
	if (rc2.right< rc1.right)	{ rc1.right  = rc2.right; }
	if (rc2.top	 > rc1.top)		{ rc1.top	 = rc2.top;	  }
	if (rc2.bottom<rc1.bottom)	{ rc1.bottom = rc2.bottom;}

	//	invalid rect,but..
	//	if (r.left >= r.right || r.top	>= r.bottom) return 1;

	return rc1;
	}	
};

#endif
