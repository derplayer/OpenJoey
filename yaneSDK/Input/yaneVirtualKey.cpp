//	yaneVirtualKey.cpp :

#include "stdafx.h"
#include "yaneVirtualKey.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

//bool	operator <(const CVKeyBase& x,const CVKeyBase& y)  { return x.m_key < y.m_key; }

//////////////////////////////////////////////////////////////////////////////
CVirtualKey::CVirtualKey(){
	ClearKeyDevice();
	ClearKeyList();
}

CVirtualKey::~CVirtualKey() {}
//////////////////////////////////////////////////////////////////////////////

//	キーデバイスの登録
void	CVirtualKey::ClearKeyDevice(){
	m_alpDevice.clear();
}

void	CVirtualKey::AddDevice(const smart_ptr<IKeyBase>& keybase){
	m_alpDevice.insert(keybase);
}

void	CVirtualKey::RemoveDevice(const smart_ptr<IKeyBase>& keybase){
	int nDeviceNo=0;
	smart_vector_ptr<IKeyBase>::iterator it = m_alpDevice.begin();
	while (true){
		if (it==m_alpDevice.end()){
			return ;	//	みつかんねー
		}
		if ((*it).get()==keybase.get()){
			m_alpDevice.erase(it);
			break;
		}
		it++; nDeviceNo++;
	}
	
	//	さらに仮想キーの削除作業
	for(int i=0;i<g_VIRTUAL_KEY_MAX;i++){
		CVKeyList::iterator it;
		for(it = m_VKey[i].begin();it!=m_VKey[i].end();){
			int& nDevNo = (*it)->m_nDeviceNo;
			if (nDevNo == nDeviceNo){
				it = m_VKey[i].erase(it);
			} else if (nDevNo > nDeviceNo) {
				nDevNo--;
				it++;
			} else {
				it++;
			}
		}
	}
}

smart_ptr<IKeyBase> CVirtualKey::GetDevice(int n){
	return m_alpDevice[n];
}

LRESULT	CVirtualKey::Input(){
	smart_vector_ptr<IKeyBase>::iterator it = m_alpDevice.begin();
	LRESULT lr = 0;
	while (it!=m_alpDevice.end()){
		lr |= (*it)->Input();
		it++;
	}

	FlipKeyBuffer(m_nKeyBufNo);
	::ZeroMemory(m_byKeyBuffer[m_nKeyBufNo],256);

	//	各デバイスの内容を仮想キーに反映させる
	for(int i=0;i<g_VIRTUAL_KEY_MAX;i++){
		CVKeyList::iterator it;
		it = m_VKey[i].begin();
		while (it!=m_VKey[i].end()) {
			if (m_alpDevice[(*it)->m_nDeviceNo]->IsKeyPress((*it)->m_nKey)) {
				m_byKeyBuffer[m_nKeyBufNo][i] = 255;
				break;
			}
			it++;
		}
	}
	return lr;
}

//////////////////////////////////////////////////////////////////////////////

//	仮想キーの追加・削除
void	CVirtualKey::ClearKeyList(){
	for(int i=0;i<g_VIRTUAL_KEY_MAX;i++) {
		m_VKey[i].clear();	//	auto_ptrなので、これでオッケ～
	}
}

void	CVirtualKey::AddKey(int vkey,int nDeviceNo,int nKey){
	CKeyInfo* lp = new CKeyInfo;
	lp->m_nDeviceNo = nDeviceNo;
	lp->m_nKey		= nKey;
	m_VKey[vkey].insert(lp);
}

void	CVirtualKey::RemoveKey(int vkey,int nDeviceNo,int nKey){
	CVKeyList::iterator it;
	for(it = m_VKey[vkey].begin();it!=m_VKey[vkey].end();){
		if (((*it)->m_nDeviceNo == nDeviceNo) &&
			((*it)->m_nKey		== nKey)){
			it = m_VKey[vkey].erase(it);
		} else {
			it++;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd
