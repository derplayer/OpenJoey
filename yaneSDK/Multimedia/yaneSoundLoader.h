//
//	yaneSoundLoader.h :
//
//		サウンドの統括的ローディング
//

#ifndef __yaneSoundLoader_h__
#define __yaneSoundLoader_h__


#include "../Auxiliary/yaneLoadCache.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class ISound;
class ISoundFactory;

class CSoundLoader : public CLoadCache {
/**
	class ISound 派生クラスのための LoadCache
*/
public:
	virtual smart_ptr<ISound> GetSound(int nNo);		///	サウンドの取得

	///	これ絶対設定してちょーだい！
	///	StreamSoundで再生するかどうかのポリシーもこれが関与しとるんで
	void	SetSoundFactory(const smart_ptr<ISoundFactory>& sf) { m_sf = sf; }
	smart_ptr<ISoundFactory> GetSoundFactory() const { return m_sf; }

	///	StreamPlayを強制的に有効／無効にするためのオプション
	///	(CSoundFactoryでの設定より優先される)
	void	SetStreamPlay(bool bStreamPlay)
	{
		m_bEnforceStreamPlay = true;
		m_bStreamPlay = bStreamPlay;
	}

	///	nNoのファイル名を取得
	string	GetFileName(int nNo);

	CSoundLoader():m_bEnforceStreamPlay(false){}

protected:
	smart_ptr<ISoundFactory> m_sf;
	virtual	LRESULT InnerLoad(const smart_obj& obj);
	bool m_bEnforceStreamPlay;
	bool m_bStreamPlay;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif