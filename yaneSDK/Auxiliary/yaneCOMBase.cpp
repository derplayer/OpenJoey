#include "stdafx.h"
#include <objbase.h>
#include "yaneCOMBase.h"
#include "yaneStream.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

//	static member..
ThreadLocal<int> CCOMBase::m_nCount;
CCriticalSection CCOMBase::m_cr;

LRESULT CCOMBase::inc_ref(){
	CCriticalLock cl(GetCriticalSection());
	if (m_nCount.isEmpty()||m_nCount==0){	//	要素あらへんやん？
		m_nCount = 0;
		if (::CoInitialize(NULL)) {	//	COMの初期化
			Err.Out("CoInitializeに失敗...");
			return 1;
		}
	}
	m_nCount = m_nCount + 1;
	return 0;
}

LRESULT CCOMBase::dec_ref(){
	CCriticalLock cl(GetCriticalSection());
	if (m_nCount.isEmpty()){	//	要素あらへんやん？
		return 1;
	}
	m_nCount = m_nCount - 1;
	if (m_nCount==0) {
		::CoUninitialize(); // COMのシャットダウン
	}
	return 0;
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
