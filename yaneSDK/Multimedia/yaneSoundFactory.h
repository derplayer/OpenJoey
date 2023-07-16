/*
	サウンドクラス統括用クラス
*/

#ifndef __yaneSoundFactory_h__
#define __yaneSoundFactory_h__


#include "../Auxiliary/yaneStringMap.h"
#include "../AppFrame/yaneObjectCreater.h"

#include "../Multimedia/yaneDirectSound.h"
#include "../Multimedia/yaneDirectMusic.h"
#include "../Multimedia/yaneWaveOutput.h"
//#include "../Multimedia/yaneDirectSound.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

//class ISound;
//class CSoundParameter;
//class CDirectSound;
//class CDirectMusic;
//class IWaveOutput;
//class CWaveOutputDirectSound;

class ISoundFactory {
/**
	class ISound 派生クラスのfactoryのインターフェース
*/
public:
	virtual	smart_ptr<ISound>	Create(const string& filename)=0;
	virtual	smart_ptr<ISound>	Create(const string& filename,bool bStreamPlay)=0;
	virtual smart_ptr<ISound>	CreateSound()=0;
	virtual smart_ptr<ISound>	CreateMIDI()=0;
	virtual list_chain<ISound>*	GetSoundList()=0;
	virtual void			IncRefDirectSound()=0;
	virtual void			DecRefDirectSound()=0;
	virtual CDirectSound*	GetDirectSound()=0;
	virtual void			IncRefDirectMusic()=0;
	virtual void			DecRefDirectMusic()=0;
	virtual CDirectMusic*	GetDirectMusic()=0;
	virtual void			IncRefWaveOutput()=0;
	virtual void			DecRefWaveOutput()=0;
	virtual IWaveOutput*	GetWaveOutput()=0;

	//	property
	virtual void	SelectMIDIFactory(int nFirst,int nSecond) = 0;
	virtual smart_ptr<CSoundParameter> GetSoundParameter() const = 0;
	virtual void	SetSoundParameter(const smart_ptr<CSoundParameter>& p) = 0;
	virtual void	SetStreamPlay(bool b) = 0;
	virtual bool	IsStreamPlay() const = 0;

	virtual ~ISoundFactory() {}
};

class CSoundFactory : public ISoundFactory {
/**
	class ISound 派生クラスのfactory

	[注意点]　生成されたISound派生クラスよりも先に、このクラスが解体されると
	　　　　　例外が発生する。これらを同一クラスのメンバに持つ場合は、
	　　　　　CSoundFactoryを先に宣言する必要がある。

	このクラスのコンストラクタで、
	class CSound::SetFactoryで、このクラスのインスタンスを登録する

	よって、以降、CSoundとしてサウンドを再生することができる

*/
public:
	//	--------- SoundBase派生クラスのfactory
	///	1.SoundBuffer（サウンド再生クラス）を生成する
	virtual smart_ptr<ISound>	CreateSound();

	///	2.MIDI出力用のクラス(CMIDIOutputDMかCMIDIOutputMCI)を生成する
	virtual smart_ptr<ISound>	CreateMIDI();
	/**
		3.拡張子に応じて、CreateSound,CreateMIDIを行う
		後者のCreateは、stream再生にするかどうかを指定する。
		(このオプション指定が他で設定したオプションより優先される)
		
		この返し値のsmart_ptrがnullであることはない。
		(少なくともCSoundNullDeviceが返る)
	*/
	virtual smart_ptr<ISound>	Create(const string& filename);
	virtual	smart_ptr<ISound>	Create(const string& filename,bool bStreamPlay);

	///	上記のfactoryで生成したインスタンスのlist_chainを獲得する
	list_chain<ISound>*	GetSoundList() { return& m_listSound; }

	//	--------- property -------------------

	/**
		CSoundParameterの設定・取得
		class ISoundStream 派生のプラグインクラスの指定と、
		サウンドバッファのグローバルフォーカスの指定は、
		こいつに対して行う
	*/
	virtual smart_ptr<CSoundParameter> GetSoundParameter() const { return m_pSoundParameter; }
	virtual void SetSoundParameter(const smart_ptr<CSoundParameter>& p) { m_pSoundParameter = p; }

	/**
		IncRefDirectSoundしたあとに、GetDirectSoundで
		CDirectSound*を取得することが可能。
		使い終わったらDecRefDirectSoundすべし。
	*/
	void			IncRefDirectSound();
	void			DecRefDirectSound();
	CDirectSound*	GetDirectSound();

	/**
		IncRefDirectSoundしたあとに、GetDirectMusicで
		CDirectMusic*を取得することが可能。
		使い終わったらDecRefDirectMusicすべし。
	*/
	void			IncRefDirectMusic();
	void			DecRefDirectMusic();
	CDirectMusic*	GetDirectMusic();

	/**
		IncRefWaveOutputしたあとに、GetWaveOutputで
		IWaveOutput*を取得することが可能。
		使い終わったらDecRefWaveOutputすべし。
	*/
	void			IncRefWaveOutput();
	void			DecRefWaveOutput();
	IWaveOutput*	GetWaveOutput();

	/**
		MIDIの再生するfactoryを切り替える
		　　nFirst　: 最初に選ばれるfactory
		　　nSecond : nFirstが使用できないときに選ばれるfactory
		値の意味は
		　　0 : CSoundNullDevice(再生しない)
		　　1 : CMIDIOutputMCI(MCIによる再生)
		　　2 : CMIDIOutputDM (DirectMusicによる再生)
		ディフォルトでは
		　　nFirst == 2	, nSecond == 1
		すなわち、DirectMusicによる再生が可能な環境ならば、それを用い、
		それが不可な環境ならば、MCIによって再生する。
		（サウンド再生が不可能な環境であれば、CreateMIDIでは
		　CSoundNullDeviceのインスタンスが返る）
	*/
	virtual void SelectMIDIFactory(int nFirst,int nSecond)
	{
		m_nMIDIFactoryFirst =nFirst;
		m_nMIDIFactorySecond=nSecond;
	}

	/// ストリーム再生するかを設定・取得
	/// 次回CreateSoundで生成されるISound派生クラスに反映される
	void	SetStreamPlay(bool b) { m_bStreamPlay = b; }
	bool	IsStreamPlay() const { return m_bStreamPlay; }

	CSoundFactory();
	virtual ~CSoundFactory();

protected:
	ref_creater<CDirectSound>	m_vDirectSound;
	ref_creater_and_mediator<CDirectMusic, CDirectSound>	m_vDirectMusic;
	ref_creater_and_mediator<CWaveOutputDirectSound, CDirectSound>	m_vWaveOutputDS;
	//	必要になったときにのみ生成される

	list_chain<ISound>	m_listSound;
	//	このクラスのfactoryで生成したインスタンスのlist_chain

	int		m_nMIDIFactoryFirst;
	int		m_nMIDIFactorySecond;

	smart_ptr<ISound>	InnerCreate(int nDevice);
	//	ISoundのFactory..
	//	nDevice ==	0 : class CSoundNullDevice
	//				1 : class CMIDIOutputMCI
	//				2 : class CMIDIOutputDM
	//				3 : class CWaveSound
	//				4 : 強制的に非StreamSound
	//				5 : 強制的にStreamSound

	// CSoundParameterを保持する
	smart_ptr<CSoundParameter> m_pSoundParameter;

	// ストリーム再生するか(default:false)
	bool m_bStreamPlay;

private:
	CSoundFactory* GetMyClass() { return this; }
	//　コンストラクタのベース初期化メンバのなかで
	//　thisを使うと警告が出て、癪に障るので．．

	bool	InnerDeleteChain(ISound* p);
	//　このクラスの持つ生成したISoundのchain
	//　すなわち、list_chain<ISound>から
	//　オブジェクト解体時にコールバックされる。
	//　この関数は、chainからpを外すのに使われる
	//　リストにpが見つかればtrueを返す

	void	InnerDeleteChainForWO(ISound* p);
	//　InnerDeleteChain()＋DecRefWaveOutput()

	void	InnerDeleteChainForDM(ISound* p);
	//　InnerDeleteChain()＋DecRefDirectMusic()
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneSoundFactory_h__
