#include "stdafx.h"
#include "yaneSoundFactory.h"

#include "yaneSound.h"
//	CSoundFactoryが必要とする
#include "../Multimedia/yaneSoundFactory.h"
#include "../Multimedia/yaneDirectMusic.h"
#include "../Multimedia/yaneDirectSound.h"
#include "../Multimedia/yaneWaveOutput.h"
#include "yaneSoundLoader.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

//////////////////////////////////////////////////////////////////////////////

smart_ptr<ISound>	CSoundLoader::GetSound(int nNo){

	Load(nNo);

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0){
		//	これが見つからないことは無いはずだが..
		return smart_ptr<ISound>(new CSoundNullDevice);
	}

	//	CLoadCacheInfo
	return smart_ptr_static_cast<ISound>(
		(smart_ptr_static_cast<CLoadCacheInfo>(obj))->pObj);
}

///	nNoのファイル名を取得
string	CSoundLoader::GetFileName(int nNo ){

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0){
		//	これが見つからないことは無いはずだが..
		return "";
	}

	return (smart_ptr_static_cast<CLoadCacheInfo>(obj))->strFileName;
}

//////////////////////////////////////////////////////////////////////////////

LRESULT		CSoundLoader::InnerLoad(const smart_obj& obj){
	if (m_sf.isNull()) return -1;	//	factoryが設定されていない

	CLoadCacheInfo& info = *(smart_ptr_static_cast<CLoadCacheInfo>(obj)).get();

	string strFileName(info.strFileName);

	smart_ptr<ISound> p;
	if (!m_bEnforceStreamPlay){
		p = m_sf->Create(strFileName);
	} else {
		p = m_sf->Create(strFileName,m_bStreamPlay);
	}

	info.pObj = smart_ptr_static_cast<void>(p);
	//	Createすれば、読み込みに失敗すればNullDeviceが返る

	if (p->GetType() == 0) {
		//	CSoundNullDevice?
		return 1; // 読み込みに失敗
	}

	//	InnerLoadでサーフェースを読み込むと明示的に
	//	解放する処理が出来なくなってくる..

	LRESULT lr = p->Open(strFileName);
/*
	//	エラーリードならば、NullDeviceを返す
	//	(こうしておけば次からInnerLoadが呼び出されないが..)
	if (lr!=0){
		(*(smart_ptr<ISound>*)info.pObj.get())
			= smart_ptr<ISound>(new CSoundNullDevice);
	}
*/
	return lr;
	return 0;
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
