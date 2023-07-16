// DirectMusic Wrapper

#include "stdafx.h"
#include "../Auxiliary/yaneCOMBase.h"
#include "../AppFrame/yaneAppManager.h"
#include "../Multimedia/yaneDirectSound.h"
#include "../Multimedia/yaneDirectMusic.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

///////////////////////////////////////////////////////////////////////////

bool CDirectMusic::CanUseDirectMusic(){
	//	DirectMusicが使える環境なのかどうかを調べて返す
	static bool bFirst = true;
	static bool bUse   = false;

	if (bFirst){	//	最初の１回のみ調べて返す
		bFirst = false;
		CCOMObject<IDirectMusicPerformance*> obj;
		bUse = obj.CreateInstance(
			CLSID_DirectMusicPerformance,IID_IDirectMusicPerformance)==0;
	}
	return bUse;
}

///////////////////////////////////////////////////////////////////////////

CDirectMusic::CDirectMusic(CDirectSound* p/*=NULL*/) :
	m_nStatus(0),m_lpDMusic(0)
{
	if (GetDirectMusicPerformance()->CreateInstance(
		CLSID_DirectMusicPerformance,IID_IDirectMusicPerformance)!=0){

		m_nStatus = 1;
		return ;
	}

	// DirectSoundにAttachを試みる
	if (p&&p->Get()){
		m_pDirectSound = p->Get();
	}

	//	さらにDirectSoundから初期化する
	//	m_lpDMusic = NULL;	//	これをやっておかないとbugる。
		//	返し値のために渡す変数の初期化を要求するだなんて、
		//	とんでもないぞ>DirectMusic
	if (FAILED(GetDirectMusicPerformance()->get()->
			Init(&m_lpDMusic,GetDirectSound(),NULL))){
		m_nStatus = 2;
		m_lpDMusic = NULL;	//	NULLを保証する
		return ;
	}

	//	ただし、ポート（シンセサイザ）の選択は行なう
	if (FAILED(GetDirectMusicPerformance()->get()->AddPort(NULL))){
	//	上のm_lpDMusicを受けている場合は、
	//	そいつから作ったポートでなければならない
			//	サードパーティ製のソフトシンセはディフォルトで
			//	選択されることは無いのでこの場合、失敗する
			//	だけどドンマイ（笑）
	}

	if (GetDirectMusicLoader()->CreateInstance(
		CLSID_DirectMusicLoader,IID_IDirectMusicLoader)!=0){
		m_nStatus = 3;
	}

	//	メモリ再生を行なうのでセグメントのキャッシュ設定を切る
	//	これを切ってもバグるようだ＾＾；
	GetDirectMusicLoader()->get()->EnableCache(GUID_DirectMusicAllTypes,false);
	GetDirectMusicLoader()->get()->EnableCache(CLSID_DirectMusicSegment,false);
}

CDirectMusic::~CDirectMusic(){
	GetDirectMusicLoader()->Release();	//	Loaderを先に解体しなければならない

	if (GetDirectMusicPerformance()!=NULL){
		GetDirectMusicPerformance()->get()->CloseDown();
		GetDirectMusicPerformance()->Release();
	}
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
