
#include "stdafx.h"
#include "yaneSELoader.h"
#include "yaneSound.h"
//	CSoundFactory���K�v�Ƃ���
#include "../Multimedia/yaneSoundFactory.h"
#include "../Multimedia/yaneDirectMusic.h"
#include "../Multimedia/yaneDirectSound.h"
#include "../Multimedia/yaneWaveOutput.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

//////////////////////////////////////////////////////////////////////////////

CSELoader::CSELoader(){
	m_bCancel			= false;
	m_nLockInterval		= nSELoaderCount;
}
CSELoader::~CSELoader(){
}

//////////////////////////////////////////////////////////////////////////////

void CSELoader::OnSet(const smart_obj& obj,int nID){
//	CSELoaderInfo& info = 
//		(*(*(smart_ptr<CSELoaderInfo>*)obj.get()).get());
//	info.nID = nID;	//	ID��ǋL���邽�߂̃I�[�o�[���C�h

	//	obj��smart_ptr<smart_ptr<CLoadCacheInfo>>�ł���
	CLoadCacheInfo& info = *smart_ptr_static_cast<CLoadCacheInfo>(obj);

	//	CSELoaderInfo�ɕK�v�Ȓl���R�s�[���Ă���
	CSELoaderInfo* pSELoaderInfo = new CSELoaderInfo;
	pSELoaderInfo->nID = nID;
	pSELoaderInfo->strFileName = info.strFileName;
	//	rLoadCacheInfo.pObj�ɕ�������
	info.pObj.Add(pSELoaderInfo);
}

LRESULT CSELoader::InnerLoad(const smart_obj& obj){
	if (CSoundLoader::m_sf.isNull()) return -1;	//	factory���ݒ肳��Ă��Ȃ�

	//	obj��smart_ptr<smart_ptr<CLoadCacheInfo>>�ł���
	CSELoaderInfo& info =
		*(smart_ptr_static_cast<CSELoaderInfo>(
			(smart_ptr_static_cast<CLoadCacheInfo>(obj)->pObj)
		));

	const string strFileName(info.strFileName);

	info.obj = CSoundLoader::m_sf->Create(strFileName);
	LRESULT lr = info.obj->Open(strFileName);

	//	�G���[���[�h�Ȃ�΁ANullDevice��ݒ肷��
	if (lr!=0){
		info.obj = smart_ptr<ISound>(new CSoundNullDevice);
	}

	return lr;
}

smart_ptr<ISound> CSELoader::GetSound(int nNo){
	Load(nNo);

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0){
		//	���ꂪ������Ȃ����Ƃ͖����͂�����..
		return smart_ptr<ISound>(new CSoundNullDevice);
	}

	//	obj��smart_ptr<smart_ptr<CLoadCacheInfo>>�ł���
	CSELoaderInfo& info =
		*(smart_ptr_static_cast<CSELoaderInfo>(
			(smart_ptr_static_cast<CLoadCacheInfo>(obj)->pObj
		)));

	return info.obj;
}

//	�t���[�����o�߂����ꍇ
void		CSELoader::OnPlay(){
	if (m_bCancel) return ;

	for(
		CIDObjectManager::CIDObject::iterator it
			= GetIDObjectManager()->GetObject()->begin();
		it!=GetIDObjectManager()->GetObject()->end();
		++it
	){
//		CSELoaderInfo& info =
//			(*(*(smart_ptr<CSELoaderInfo>*)it->second.get()).get());
		//	it->second��smart_ptr<smart_ptr<CLoadCacheInfo>>�ł���
		CSELoaderInfo& info =
			**(smart_ptr<CSELoaderInfo>*)(
				**(smart_ptr<CLoadCacheInfo>*)(it->second).get()
			).pObj.get();

		int& n = info.nPlay;
		if (n == m_nLockInterval){
			GetSound(info.nID)->Play();
		}
		if (n > 0) {
			n--;
		} ef (n < 0) {
			//	�񐔎w��Đ����[�h
			if (!GetSound(info.nID)->IsPlay()) {
				//	m_anInterval[i]�������o�߂������H
				info.nInterval++;
				if (info.nInterval.IsEnd()){
					GetSound(info.nID)->Play();
					n++;
					info.nInterval.Reset();
				}
			}
		}
	}
}

void		CSELoader::OnPlayAndReset(){
	if (m_bCancel) return ;
	for(
		CIDObjectManager::CIDObject::iterator it
			= GetIDObjectManager()->GetObject()->begin();
		it!=GetIDObjectManager()->GetObject()->end();
		++it
	){
//		CSELoaderInfo& info = 
//			(*(*(smart_ptr<CSELoaderInfo>*)it->second.get()).get());
		//	it->second��smart_ptr<smart_ptr<CLoadCacheInfo>>�ł���
		CSELoaderInfo& info =
			*(smart_ptr_static_cast<CSELoaderInfo>(
				(smart_ptr_static_cast<CLoadCacheInfo>(it->second)->pObj
			)));

		if (info.nPlay == m_nLockInterval){
			GetSound(info.nID)->Play();
		}
		info.nPlay = 0;
	}
}

void		CSELoader::PlayN(int nNo){
	if (m_bCancel) return ;

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0)
		return ; // ���݂��Ȃ��I�u�W�F�N�g�ɑ΂���Q��..

//	CSELoaderInfo& info = 
//		(*(*(smart_ptr<CSELoaderInfo>*)obj.get()).get());
	//	obj��smart_ptr<smart_ptr<CLoadCacheInfo>>�ł���
	CSELoaderInfo& info =
		*(smart_ptr_static_cast<CSELoaderInfo>(
			(smart_ptr_static_cast<CLoadCacheInfo>(obj)->pObj
		)));

	//	���܂����Đ�����
	if (info.nPlay == 0) {
		info.nPlay = m_nLockInterval;
	}
	GetSound(nNo)->SetLoopPlay(false);
}

void		CSELoader::Play(int nNo){
	if (m_bCancel) return ;
	//	���Ă��Ȃ���΍Đ�����

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0)
		return ; // ���݂��Ȃ��I�u�W�F�N�g�ɑ΂���Q��..

//	CSELoaderInfo& info = 
//		(*(*(smart_ptr<CSELoaderInfo>*)obj.get()).get());
	//	obj��smart_ptr<smart_ptr<CLoadCacheInfo>>�ł���
	CSELoaderInfo& info =
		*(smart_ptr_static_cast<CSELoaderInfo>(
			(smart_ptr_static_cast<CLoadCacheInfo>(obj)->pObj
		)));

	if (info.nPlay == 0) {
		if (!GetSound(nNo)->IsPlay()) {
			info.nPlay = m_nLockInterval;
		}
	}
	GetSound(nNo)->SetLoopPlay(false);
}

void		CSELoader::PlayLN(int nNo){
	if (m_bCancel) return ;
	//	���܂����Đ�����

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0)
		return ; // ���݂��Ȃ��I�u�W�F�N�g�ɑ΂���Q��..

//	CSELoaderInfo& info = 
//		(*(*(smart_ptr<CSELoaderInfo>*)obj.get()).get());
	//	obj��smart_ptr<smart_ptr<CLoadCacheInfo>>�ł���
	CSELoaderInfo& info =
		*(smart_ptr_static_cast<CSELoaderInfo>(
			(smart_ptr_static_cast<CLoadCacheInfo>(obj)->pObj
		)));

	if (info.nPlay == 0) {
		info.nPlay = m_nLockInterval;
	}
	GetSound(nNo)->SetLoopPlay(true);
}

void		CSELoader::PlayL(int nNo){
	if (m_bCancel) return ;
	//	���Ă��Ȃ���΍Đ�����

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0)
		return ; // ���݂��Ȃ��I�u�W�F�N�g�ɑ΂���Q��..

//	CSELoaderInfo& info = 
//		(*(*(smart_ptr<CSELoaderInfo>*)obj.get()).get());
	//	obj��smart_ptr<smart_ptr<CLoadCacheInfo>>�ł���
	CSELoaderInfo& info =
		*(smart_ptr_static_cast<CSELoaderInfo>(
			(smart_ptr_static_cast<CLoadCacheInfo>(obj)->pObj
		)));

	if (info.nPlay == 0) {
		if (!GetSound(nNo)->IsPlay()) {
			info.nPlay = m_nLockInterval;
		}
	}
	GetSound(nNo)->SetLoopPlay(true);
}

void		CSELoader::PlayT(int nNo,int nTimes,int nInterval){
	if (m_bCancel) return ;
	//	�Đ��񐔎w��Đ�
	//	���Ă��Ȃ���΍Đ�����

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0)
		return ; // ���݂��Ȃ��I�u�W�F�N�g�ɑ΂���Q��..

//	CSELoaderInfo& info = 
//		(*(*(smart_ptr<CSELoaderInfo>*)obj.get()).get());
	//	obj��smart_ptr<smart_ptr<CLoadCacheInfo>>�ł���
	CSELoaderInfo& info =
		*(smart_ptr_static_cast<CSELoaderInfo>(
			(smart_ptr_static_cast<CLoadCacheInfo>(obj)->pObj
		)));

	info.nPlay = -nTimes;			//	����}�C�i�X�Ȃ�΍Đ��񐔂Ȃ̂�O�O�G
	info.nInterval.Set(0,nInterval);	//	���̂Ƃ������C���^�[�o����ݒ肷��̂�
	info.nInterval = nInterval;		//	�P��ڂ͑��U�Ȃ�ׂ��I
	GetSound(nNo)->SetLoopPlay(false);
}

void		CSELoader::Stop(int nNo){
	if (!GetCacheManager()->isExist(GetIDObjectAndNumber(nNo))) return ;
	//	�����݂��Ȃ��̂Ȃ�΂킴�킴�Q�Ƃ��ăI�u�W�F�N�g�𐶐����邱�Ƃ��Ȃ�

	GetSound(nNo)->Stop();
}

void		CSELoader::Reset(){
	for(
		CIDObjectManager::CIDObject::iterator it
			= GetIDObjectManager()->GetObject()->begin();
		it!=GetIDObjectManager()->GetObject()->end();
		++it
	){
//		CSELoaderInfo& info = 
//			(*(*(smart_ptr<CSELoaderInfo>*)it->second.get()).get());
		//	it->second��smart_ptr<smart_ptr<CLoadCacheInfo>>�ł���
		CSELoaderInfo& info =
			*(smart_ptr_static_cast<CSELoaderInfo>(
				(smart_ptr_static_cast<CLoadCacheInfo>(it->second)->pObj
			)));

		//	������背�x���̒Ⴂ�Ƃ���ɑ΂��Ă̂ݗL��
		info.nPlay = 0;
	}
}

bool	CSELoader::IsPlay(int nID){
	//	�T�E���h�͍Đ����Ȃ̂��H
	if (!GetCacheManager()->isExist(GetIDObjectAndNumber(nID))) return false;
	//	�����݂��Ȃ��̂Ȃ�΂킴�킴�Q�Ƃ��ăI�u�W�F�N�g�𐶐����邱�Ƃ��Ȃ�

	return GetSound(nID)->IsPlay();
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
