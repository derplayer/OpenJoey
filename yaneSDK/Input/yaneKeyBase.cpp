// yaneKeyBase.cpp

#include "stdafx.h"
#include "yaneKeyBase.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

//////////////////////////////////////////////////////////////////////////////

IKeyBase::IKeyBase(){
	m_nKeyBufNo=0;	// ���ƕ\��flip���āA�������Ƃ�̂ɗ��p
	Reset();
}

void	IKeyBase::Reset(){
	ZERO(m_byKeyBuffer);
}

//////////////////////////////////////////////////////////////////////////////

bool IKeyBase::IsKeyPress(int key) const {
	return	(m_byKeyBuffer[m_nKeyBufNo][key]) !=0;
};

bool IKeyBase::IsKeyPush(int key) const {
	// �����ꂽ�u�Ԃ�true�ɂ���ꍇ
	if (!(m_byKeyBuffer[  m_nKeyBufNo][key])) return false;
	if (  m_byKeyBuffer[1-m_nKeyBufNo][key] ) return false;
	return true;
}

const BYTE*	IKeyBase::GetKeyData() const {
	return	(BYTE*)&m_byKeyBuffer[m_nKeyBufNo];
}

//////////////////////////////////////////////////////////////////////////////

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd
