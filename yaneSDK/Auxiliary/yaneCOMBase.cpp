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
	if (m_nCount.isEmpty()||m_nCount==0){	//	�v�f����ւ���H
		m_nCount = 0;
		if (::CoInitialize(NULL)) {	//	COM�̏�����
			Err.Out("CoInitialize�Ɏ��s...");
			return 1;
		}
	}
	m_nCount = m_nCount + 1;
	return 0;
}

LRESULT CCOMBase::dec_ref(){
	CCriticalLock cl(GetCriticalSection());
	if (m_nCount.isEmpty()){	//	�v�f����ւ���H
		return 1;
	}
	m_nCount = m_nCount - 1;
	if (m_nCount==0) {
		::CoUninitialize(); // COM�̃V���b�g�_�E��
	}
	return 0;
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
