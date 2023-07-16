// yaneDirectInput.h
//	 This is DirectInput wrapper.
//		programmed by yaneurao(M.Isozaki) '99/06/18
//		modified by yaneurao '00/02/29

#ifndef __yaneDirectMusic_h__
#define __yaneDirectMusic_h__


#include "../Auxiliary/yaneCOMBase.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class CDirectSound;
class CDirectMusic {
/**
	DirectMusicの初期化のためのクラス
	class CMIDIOutputDM にて使用
*/
public:
	static bool CanUseDirectMusic();
	/**
		DirectMusicが使える環境なのかどうかを調べて返す
		調べるのは最初の１回目の呼び出しのみなので何度呼び出しても良い。
	*/

	/**
		コンストラクタで自動的にDirectMusicの初期化を試みるので、
		以下の関数で、必要なものを取得すればそれでｏｋ。
	*/
	CCOMObject<IDirectMusicPerformance*>* GetDirectMusicPerformance()
		{ return& m_vDirectMusicPerformance;}
	//	IDirectMusicPerformaceが、DirectMusicの母体

	CCOMObject<IDirectMusicLoader*>* GetDirectMusicLoader()
		{ return& m_vDirectMusicLoader;}
	//	実際の再生はIDirectMusicLoaderがIDirectMusicSegmentを
	//	作成して、そいつが担う。

	int	GetStatus() const { return m_nStatus; }
	/**
		DirectInputの初期化状況について、リザルトを返す
		0:ok(DirectX6以降)
		1,2,3:failure(DirectX6が入ってない or
			DirectMusic初期化まわりの失敗)
	*/

	CDirectMusic(CDirectSound* p=NULL);
	/**
		関連するDirectSoundオブジェクトを指定してやれば、
		このDirectMusicは、そのDirectSoundのプライマリを利用して
		再生することが出来る。これを指定しなければ、ディフォルトである
		22kHzで再生することになる。
	*/

	virtual ~CDirectMusic();

protected:
	LPDIRECTSOUND m_pDirectSound;	//	CDirectSound経由で取得したやつ
	LPDIRECTSOUND GetDirectSound() const { return m_pDirectSound; }

	CCOMObject<IDirectMusicPerformance*>	m_vDirectMusicPerformance;
	IDirectMusic* m_lpDMusic;
	//	IDirectMusicは、単にDirectSoundと結合するためのもの
	CCOMObject<IDirectMusicLoader*>	m_vDirectMusicLoader;
	int		m_nStatus;	//	この変数の意味についてはGetStatusを参照のこと

};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif
