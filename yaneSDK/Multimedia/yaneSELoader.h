//
//	yaneSELoader.h :
//
//		ＳＥの統括的ローディング
//

#ifndef __yaneSELoader_h__
#define __yaneSELoader_h__


#include "../Multimedia/yaneSoundLoader.h"
#include "../Math/yaneCounter.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class CSELoader : public CSoundLoader {
public:
	enum { nSELoaderCount = 5 };
	//	ディフォルトでは５フレーム以内ならば再生再開しない

	//	フレームが経過した場合
	void		OnPlay();				///	毎フレーム呼び出してね
	void		PlayN(int nNo);			///	いますぐ再生する(非ループ再生)
	void		Play(int nNo);			///	鳴っていなければ再生する(非ループ再生)
	void		PlayLN(int nNo);		///	いますぐ再生する(ループ再生)
	void		PlayL(int nNo);			///	鳴っていなければ再生する(ループ再生)
	void		PlayT(int nNo,int nTimes,int nInterval=0);
										///	再生回数指定再生
	void		Stop(int nNo);			///	サウンドを止める
	void		Reset();				///	サウンドの排他カウンタのリセット
	bool		IsPlay(int nNo);		///	サウンドは再生中なのか？

	virtual smart_ptr<ISound>	GetSound(int nNo);	///	サウンドの取得
	///	↑これは class CSoundLoader のメンバを使えば良い
	//	↑ダメ。キャッシュ情報クラスそのものが違うのでオーバーライドする必要がある

	///	再生キャンセルフラグ
	///	(このフラグがtrueならば、すべての再生はキャンセルされる)
	bool*		GetCancelFlag()		{ return &m_bCancel; }

	void		OnPlayAndReset();
	///	現在、再生依頼のあるものを強制的に再生する

	///	再生インターバルの設定(ディフォルトではnSELoaderCount)
	void	SetLockInterval(int nLockInterval) { m_nLockInterval = nLockInterval; }
	int		GetLockInterval() { return m_nLockInterval; }

	///	修正のためにCLoadCacheからオーバーライド
//	virtual LRESULT	Set(const string& filename,bool bUseID=false)
//		{ return SetHelper(filename,bUseID,CSELoaderInfo()); }
	//	↑CLoadCacheInfo::pObjにsmart_ptr<CSELoaderInfo>を封入するので不要になった

	CSELoader();
	virtual ~CSELoader();

protected:

	struct CSELoaderInfo
	{
		string strFileName;		//	読み込むべきファイル名
		smart_ptr<ISound> obj;	//	読み込むべきサウンド
		int		nPlay;
		//	プラスのときは、guard timeを意味する
		//		nSELoaderCountで再生されて、0になるまでデクリメントされる
		//	これマイナスならば再生回数なのら＾＾；
		CRootCounter	nInterval;
		//	連続再生のときの再生インターバル
		int		nID;
		//	IDはここにも埋めておかないとオブジェクトをiteratorで回して
		//	巡回するときに、そのオブジェクトに対するメンバを呼び出せない
		CSELoaderInfo() : nPlay(0), nID(-1) {}
	};

	virtual void OnSet(const smart_obj& obj,int nID);
	virtual LRESULT	InnerLoad(const smart_obj& obj);
	//	↑キャッシュ情報クラスそのものが違うのでオーバーライドする必要がある

	bool	m_bCancel;			//	BGMキャンセルフラグ
	int		m_nLockInterval;	//	このフレーム以内ならば再生再開しない
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif
