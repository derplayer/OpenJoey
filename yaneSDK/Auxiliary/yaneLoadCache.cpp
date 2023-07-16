
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
		///	指定したIDのものを削除する
		///	コンテナが存在し、無事削除できたときは、0が返る
	{
		return !GetObject()->erase(nID);
	}

void	CIDObjectManager::setObject(int nID,const smart_obj& object) {
		///	IDに対応するオブジェクトを設定する

		//	このIDのものが存在しないのならば挿入
		CIDObject::iterator it;
		it = GetObject()->find(nID);
		if (it==GetObject()->end()){
			//	そのkeyは存在しない
			//	ほな、挿入すっかー
			GetObject()->insert(pair<int,smart_obj>(nID,object));
		} else {
			//	追加追加追加～○(≧∇≦)o　夏はスイカ～○(≧∇≦)o
			it->second = object;
		}
	}

LRESULT CIDObjectManager::getObject(int nID,smart_obj& object) {
		///	指定したIDに対応するオブジェクトを取得する
		CIDObject::iterator it;
		it = GetObject()->find(nID);
		if (it==GetObject()->end()){
			//	そのkeyは存在しない
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

	//	リスナを経由させて番号を置換する(かも)
	GetLoadCacheListener()->OnLoad(nNo);

	LRESULT lr = GetCacheManager()->ReferToObject(GetIDObjectAndNumber(nNo));
	//	こいつを参照しました！

	if (lr==0){
		//	既に読み込んどるでー
		return 0;
	}

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0){
		//	そんなバナナ
		//	これおかしい。ランタイム例外出してもいいぐらい．．
		return 1;
	}
	//	このobjは、smart_ptr<CLoadCacheInfo>のハズ
	//	smart_ptr<CLoadCacheInfo>* pInfo = obj.get();

	while(GetCacheStaleListener()->isFull()){

		CIDObjectAndNumber on;
		if (GetCacheManager()->releaseLast(on))
			break; // 解放すべきオブジェクトすら存在しない

		smart_obj obj;
		if (on.pIDObject->getObject(on.nNum,obj)!=0){
			//	これおかしい。ランタイム例外出してもいいぐらいだが．．
			return 1;
		}

//		pInfo = obj.get();
//		GetCacheStaleListener()->OnRelease((*pInfo)->pObj); // 解放時リスナの呼び出し
		GetCacheStaleListener()->OnRelease(obj); // 解放時リスナの呼び出し
		on.pIDObject->erase(on.nNum); // このオブジェクトを削除
	}
	//	リリースしてから、読み込まないと、
	//	読み込んだあとにキャッシュに入らないからと解放されたら
	//	不正なサーフェースが出来てしまう。
	//	キャッシュ溢れによる不正サーフェースに対するチェックを
	//	これを利用するプログラムにさせてはいけない

	lr = InnerLoad(obj);
	if (lr!=0) return lr;

	GetCacheStaleListener()->OnLoad(obj);
	//	リスナに読み込んだことを通知

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

	//	自分のCLoadCacheが保持しているオブジェクトを指しているマップだけクリアしていく

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
	//	連想コンテナが消去した次に続く要素の先頭を指すiteratorを返すのは
	//	非標準の拡張なので、この機能を使用してはいけない

			GetCacheManager()->GetMap()->erase(it++);
			//	これでok。詳しくはEffectiveSTL 第9項を参照のこと。

		} else {
			it ++;
		}
	}

	return 0;
}

///	Setで設定した、ファイル名リストの要素数を返します。
int		CLoadCache::GetSize() const
{
	CLoadCache* pThis = const_cast<CLoadCache*> (this);
	return pThis->GetCacheManager()->GetMap()->size();
}

LRESULT	CLoadCache::GetObj(int nNo,smart_obj& obj){
	if (GetIDObjectManager()->getObject(nNo,obj)!=0){
		//	そんなバナナ
		//	これおかしい。ランタイム例外出してもいいぐらい．．
		return 1;
	}
	return 0;
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
