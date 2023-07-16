
#include "stdafx.h"
#include "yaneSELoader.h"
#include "yaneSound.h"
//	CSoundFactoryが必要とする
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
//	info.nID = nID;	//	IDを追記するためのオーバーライド

	//	objはsmart_ptr<smart_ptr<CLoadCacheInfo>>である
	CLoadCacheInfo& info = *smart_ptr_static_cast<CLoadCacheInfo>(obj);

	//	CSELoaderInfoに必要な値をコピーしておく
	CSELoaderInfo* pSELoaderInfo = new CSELoaderInfo;
	pSELoaderInfo->nID = nID;
	pSELoaderInfo->strFileName = info.strFileName;
	//	rLoadCacheInfo.pObjに封入する
	info.pObj.Add(pSELoaderInfo);
}

LRESULT CSELoader::InnerLoad(const smart_obj& obj){
	if (CSoundLoader::m_sf.isNull()) return -1;	//	factoryが設定されていない

	//	objはsmart_ptr<smart_ptr<CLoadCacheInfo>>である
	CSELoaderInfo& info =
		*(smart_ptr_static_cast<CSELoaderInfo>(
			(smart_ptr_static_cast<CLoadCacheInfo>(obj)->pObj)
		));

	const string strFileName(info.strFileName);

	info.obj = CSoundLoader::m_sf->Create(strFileName);
	LRESULT lr = info.obj->Open(strFileName);

	//	エラーリードならば、NullDeviceを設定する
	if (lr!=0){
		info.obj = smart_ptr<ISound>(new CSoundNullDevice);
	}

	return lr;
}

smart_ptr<ISound> CSELoader::GetSound(int nNo){
	Load(nNo);

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0){
		//	これが見つからないことは無いはずだが..
		return smart_ptr<ISound>(new CSoundNullDevice);
	}

	//	objはsmart_ptr<smart_ptr<CLoadCacheInfo>>である
	CSELoaderInfo& info =
		*(smart_ptr_static_cast<CSELoaderInfo>(
			(smart_ptr_static_cast<CLoadCacheInfo>(obj)->pObj
		)));

	return info.obj;
}

//	フレームが経過した場合
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
		//	it->secondはsmart_ptr<smart_ptr<CLoadCacheInfo>>である
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
			//	回数指定再生モード
			if (!GetSound(info.nID)->IsPlay()) {
				//	m_anInterval[i]分だけ経過したか？
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
		//	it->secondはsmart_ptr<smart_ptr<CLoadCacheInfo>>である
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
		return ; // 存在しないオブジェクトに対する参照..

//	CSELoaderInfo& info = 
//		(*(*(smart_ptr<CSELoaderInfo>*)obj.get()).get());
	//	objはsmart_ptr<smart_ptr<CLoadCacheInfo>>である
	CSELoaderInfo& info =
		*(smart_ptr_static_cast<CSELoaderInfo>(
			(smart_ptr_static_cast<CLoadCacheInfo>(obj)->pObj
		)));

	//	いますぐ再生する
	if (info.nPlay == 0) {
		info.nPlay = m_nLockInterval;
	}
	GetSound(nNo)->SetLoopPlay(false);
}

void		CSELoader::Play(int nNo){
	if (m_bCancel) return ;
	//	鳴っていなければ再生する

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0)
		return ; // 存在しないオブジェクトに対する参照..

//	CSELoaderInfo& info = 
//		(*(*(smart_ptr<CSELoaderInfo>*)obj.get()).get());
	//	objはsmart_ptr<smart_ptr<CLoadCacheInfo>>である
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
	//	いますぐ再生する

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0)
		return ; // 存在しないオブジェクトに対する参照..

//	CSELoaderInfo& info = 
//		(*(*(smart_ptr<CSELoaderInfo>*)obj.get()).get());
	//	objはsmart_ptr<smart_ptr<CLoadCacheInfo>>である
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
	//	鳴っていなければ再生する

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0)
		return ; // 存在しないオブジェクトに対する参照..

//	CSELoaderInfo& info = 
//		(*(*(smart_ptr<CSELoaderInfo>*)obj.get()).get());
	//	objはsmart_ptr<smart_ptr<CLoadCacheInfo>>である
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
	//	再生回数指定再生
	//	鳴っていなければ再生する

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0)
		return ; // 存在しないオブジェクトに対する参照..

//	CSELoaderInfo& info = 
//		(*(*(smart_ptr<CSELoaderInfo>*)obj.get()).get());
	//	objはsmart_ptr<smart_ptr<CLoadCacheInfo>>である
	CSELoaderInfo& info =
		*(smart_ptr_static_cast<CSELoaderInfo>(
			(smart_ptr_static_cast<CLoadCacheInfo>(obj)->pObj
		)));

	info.nPlay = -nTimes;			//	これマイナスならば再生回数なのら＾＾；
	info.nInterval.Set(0,nInterval);	//	このときだけインターバルを設定するのだ
	info.nInterval = nInterval;		//	１回目は速攻なるべし！
	GetSound(nNo)->SetLoopPlay(false);
}

void		CSELoader::Stop(int nNo){
	if (!GetCacheManager()->isExist(GetIDObjectAndNumber(nNo))) return ;
	//	↑存在しないのならばわざわざ参照してオブジェクトを生成することもない

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
		//	it->secondはsmart_ptr<smart_ptr<CLoadCacheInfo>>である
		CSELoaderInfo& info =
			*(smart_ptr_static_cast<CSELoaderInfo>(
				(smart_ptr_static_cast<CLoadCacheInfo>(it->second)->pObj
			)));

		//	自分よりレベルの低いところに対してのみ有効
		info.nPlay = 0;
	}
}

bool	CSELoader::IsPlay(int nID){
	//	サウンドは再生中なのか？
	if (!GetCacheManager()->isExist(GetIDObjectAndNumber(nID))) return false;
	//	↑存在しないのならばわざわざ参照してオブジェクトを生成することもない

	return GetSound(nID)->IsPlay();
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
