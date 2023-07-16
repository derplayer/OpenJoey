
#include "stdafx.h"
#include "yaneLoadCache.h"

//////////////////////////////////////////////////////////////////////////////
//	CIDObjectManager

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

void	CIDObjectManager::clear()
	{
		GetObject()->clear();
	}

LRESULT	CIDObjectManager::erase(int nID)
		///	�w�肵��ID�̂��̂��폜����
		///	�R���e�i�����݂��A�����폜�ł����Ƃ��́A0���Ԃ�
	{
		return !GetObject()->erase(nID);
	}

void	CIDObjectManager::setObject(int nID,const smart_obj& object) {
		///	ID�ɑΉ�����I�u�W�F�N�g��ݒ肷��

		//	����ID�̂��̂����݂��Ȃ��̂Ȃ�Α}��
		CIDObject::iterator it;
		it = GetObject()->find(nID);
		if (it==GetObject()->end()){
			//	����key�͑��݂��Ȃ�
			//	�قȁA�}���������[
			GetObject()->insert(pair<int,smart_obj>(nID,object));
		} else {
			//	�ǉ��ǉ��ǉ��`��(���ށ�)o�@�Ă̓X�C�J�`��(���ށ�)o
			it->second = object;
		}
	}

LRESULT CIDObjectManager::getObject(int nID,smart_obj& object) {
		///	�w�肵��ID�ɑΉ�����I�u�W�F�N�g���擾����
		CIDObject::iterator it;
		it = GetObject()->find(nID);
		if (it==GetObject()->end()){
			//	����key�͑��݂��Ȃ�
			return 1;
		}
		object = it->second;
		return 0;
	}

//////////////////////////////////////////////////////////////////////////////

CLoadCache::CLoadCache() :
	m_bCanReloadAgain(true),
	m_pLoadCacheListener(new CDefaultLoadCacheListener),
	m_pCacheStaleListener(new CDefaultCacheStaleListener),
	m_vCacheManager(new CCacheManager)
{}

CLoadCache::~CLoadCache(){
	ReleaseAll();
}

LRESULT		CLoadCache::Load(int nNo){

	//	���X�i���o�R�����Ĕԍ���u������(����)
	GetLoadCacheListener()->OnLoad(nNo);

	LRESULT lr = GetCacheManager()->ReferToObject(GetIDObjectAndNumber(nNo));
	//	�������Q�Ƃ��܂����I

	if (lr==0){
		//	���ɓǂݍ���ǂ�Ł[
		return 0;
	}

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0){
		//	����ȃo�i�i
		//	���ꂨ�������B�����^�C����O�o���Ă��������炢�D�D
		return 1;
	}
	//	����obj�́Asmart_ptr<CLoadCacheInfo>�̃n�Y
	//	smart_ptr<CLoadCacheInfo>* pInfo = obj.get();

	while(GetCacheStaleListener()->isFull()){

		CIDObjectAndNumber on;
		if (GetCacheManager()->releaseLast(on))
			break; // ������ׂ��I�u�W�F�N�g���瑶�݂��Ȃ�

		smart_obj obj;
		if (on.pIDObject->getObject(on.nNum,obj)!=0){
			//	���ꂨ�������B�����^�C����O�o���Ă��������炢�����D�D
			return 1;
		}

//		pInfo = obj.get();
//		GetCacheStaleListener()->OnRelease((*pInfo)->pObj); // ��������X�i�̌Ăяo��
		GetCacheStaleListener()->OnRelease(obj); // ��������X�i�̌Ăяo��
		on.pIDObject->erase(on.nNum); // ���̃I�u�W�F�N�g���폜
	}
	//	�����[�X���Ă���A�ǂݍ��܂Ȃ��ƁA
	//	�ǂݍ��񂾂��ƂɃL���b�V���ɓ���Ȃ�����Ɖ�����ꂽ��
	//	�s���ȃT�[�t�F�[�X���o���Ă��܂��B
	//	�L���b�V�����ɂ��s���T�[�t�F�[�X�ɑ΂���`�F�b�N��
	//	����𗘗p����v���O�����ɂ����Ă͂����Ȃ�

	lr = InnerLoad(obj);
	if (lr!=0) return lr;

	GetCacheStaleListener()->OnLoad(obj);
	//	���X�i�ɓǂݍ��񂾂��Ƃ�ʒm

	return 0;
}

LRESULT		CLoadCache::Release(int nNo){
	GetIDObjectManager()->erase(nNo);
	return GetCacheManager()->release(GetIDObjectAndNumber(nNo));
}

LRESULT		CLoadCache::ReleaseAll(){
	CIDObjectManager* p = GetIDObjectManager();
	p->clear();
//	return GetCacheManager()->releaseAll();

	//	������CLoadCache���ێ����Ă���I�u�W�F�N�g���w���Ă���}�b�v�����N���A���Ă���

	CCacheManager::listchainmap::iterator it = GetCacheManager()->GetMap()->begin();
	while(it!=GetCacheManager()->GetMap()->end()){
		/*
			it	: CCacheManager::listchainmap::iterator
			*it : CCacheManager::listchainmap
				==	fast_list<CIDObjectAndNumber>::listchainmap
				==	map<CIDObjectAndNumber,listchain::iterator>
			(*it).second : fast_list<CIDObjectAndNumber>::listchain::iterator
			*((*it).second) : fast_list<CIDObjectAndNumber>::listchain
		*/
		if ((*((*it).second)).pIDObject == p) {

			GetCacheManager()->GetList()->erase((*it).second);
	//		it = GetCacheManager()->GetMap()->erase(it);
	//	�A�z�R���e�i�������������ɑ����v�f�̐擪���w��iterator��Ԃ��̂�
	//	��W���̊g���Ȃ̂ŁA���̋@�\���g�p���Ă͂����Ȃ�

			GetCacheManager()->GetMap()->erase(it++);
			//	�����ok�B�ڂ�����EffectiveSTL ��9�����Q�Ƃ̂��ƁB

		} else {
			it ++;
		}
	}

	return 0;
}

///	Set�Őݒ肵���A�t�@�C�������X�g�̗v�f����Ԃ��܂��B
int		CLoadCache::GetSize() const
{
	CLoadCache* pThis = const_cast<CLoadCache*> (this);
	return pThis->GetCacheManager()->GetMap()->size();
}

LRESULT	CLoadCache::GetObj(int nNo,smart_obj& obj){
	if (GetIDObjectManager()->getObject(nNo,obj)!=0){
		//	����ȃo�i�i
		//	���ꂨ�������B�����^�C����O�o���Ă��������炢�D�D
		return 1;
	}
	return 0;
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
