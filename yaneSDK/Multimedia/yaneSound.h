//
// yaneSound.h
//
//		MIDI,WAVE出力基底クラス
//

#ifndef __yaneSound_h__
#define __yaneSound_h__


namespace yaneuraoGameSDK3rd {
namespace Multimedia {

/////////////////////////////////////////////////////////////////////////////
class ISound {
/**
	MIDI,WAVE再生クラスのインターフェース
*/
public:
	virtual LRESULT Open(const string& strFileName) = 0;
	/// ファイルの読み込み

	virtual LRESULT Close() = 0;
	///	ファイルを閉じる

	virtual LRESULT Play()	= 0;
	///	再生する

	virtual LRESULT Pause() = 0;
	///	一時停止

	virtual LRESULT Replay() = 0;
	/// 一時停止していたものを再開

	virtual LRESULT Stop()	= 0;
	/// 停止

	virtual bool IsPlay() const	 = 0;
	/// 再生中ならばtrue

	virtual LRESULT SetLoopPlay(bool bLoop) = 0;
	/// ループ再生モードにする（終了後、先頭に戻って再生する）

	virtual bool IsLoopPlay() const = 0;
	/// SetLoopPlayでループ再生モードに設定されているならばtrue

	virtual LRESULT	SetCurrentPos(LONG lPos) = 0;
	virtual LONG	GetCurrentPos() const = 0;
	/// 現在の再生ポジションを設定／取得（[ms]単位）
	///	取得は、エラーのときはマイナスの値が返る

	virtual LONG	GetLength() const = 0;
	///	曲の全体長さを取得する。([ms]単位)

	virtual LRESULT SetVolume(LONG volume)=0;
	virtual LONG	GetVolume() const =0;
	/**
		ヴォリュームを設定／取得する。(DSBVOLUME_MIN～DSBVOLUME_MAXの間の値で
		指定する。DSBVOLUME_MINを指定したときは、無音状態)　ただし、
		すべてのデバイスで、この音量調整をサポートしているとは限らない。
		たとえば、CMIDIOutputDMはサポートしているが、CMIDIOutputMCIは、
		これをサポートしていない。
	*/

	virtual int	GetType() const = 0;
	/**
		RTTIもどき。派生クラスのタイプを返す
		0 : NullDevice
		1 : CMIDIOutputMCI
		2 : CMIDIOutputDM
		3 : CWaveStaticSound
		4 : CWaveStreamSound
		5 : CWaveSound
	*/

	virtual string	GetFileName() const = 0;
	/// 読み込んでいるファイル名を返す

	virtual LRESULT ConvertToMealTime(LONG nPos,int&nHour,int&nMin,int&nSec,int&nMS);
	virtual LRESULT ConvertFromMealTime(LONG&nPos,int nHour,int nMin,int nSec,int nMS);
	/**
		GetCurrentPosで得られたnPos [ms] を、(nHour,nMin,nSec,nMs)すなわち、
		時分秒毛とを変換する。毛というのは、聞きなれない単位だと思いますが
		1/1000（秒）==1[ms]です。前者は[ms]⇒時分秒毛，後者は、
		時分秒毛⇒[ms]です。
	*/

	virtual ~ISound() {}
};

class ISoundRestorable : public ISound {
/**
	Restoreが可能な事を示す class ISound 互換インターフェース
*/
public:
	virtual LRESULT Restore() = 0;
	///	ファイルを再読み込み

	virtual ~ISoundRestorable() {}
};

class IWaveSound : public ISoundRestorable {
/**
	WAVE再生クラスのインターフェース
*/
public:
	virtual LRESULT Restore() = 0;	// 再定義しないとlist_chain::for_eachがコンパイル出来ない^^;
	///	ファイルを再読み込み

	virtual ~IWaveSound() {}
// まだメンバはない^^;
};

class CSoundNullDevice : public ISound {
/**
	俗に言うNULL DEVICE
	MIDIの付いていない環境では、これでをアップキャストして使えば良い
*/
public:
	virtual LRESULT Open(const string& sFileName) { return 0; }
	virtual LRESULT Close()	 { return 0; }
	virtual LRESULT Play()	 { return 0; }
	virtual LRESULT Replay() { return 0; }
	virtual LRESULT Stop()	 { return 0; }
	virtual LRESULT Pause()	 { return 0; }
	virtual bool IsPlay() const { return false; }
	virtual bool IsLoopPlay() const { return m_bLoop; }
	virtual LRESULT SetLoopPlay(bool bLoop) { m_bLoop=bLoop; return 0; }
	virtual LONG	GetCurrentPos() const { return -1; }
	virtual LRESULT	SetCurrentPos(LONG lPos) { return -1;}
	virtual LONG	GetLength() const { return -1;}
	virtual LRESULT SetVolume(LONG volume){ return -1;}
	virtual LONG	GetVolume() const { return DSBVOLUME_MIN;}
	virtual string	GetFileName() const { return ""; }
	virtual int	GetType() const { return 0; }

	CSoundNullDevice() : m_bLoop(false) {}

protected:
	bool m_bLoop;
};

class CProxySound : public ISound {
/**
	ISoundのproxy object
*/
public:
	virtual LRESULT Open(const string& pFileName) { return GetSound()->Open(pFileName); }
	virtual LRESULT Close() { return GetSound()->Close();  }
	virtual LRESULT Play()	{ return GetSound()->Play();   }
	virtual LRESULT Replay(){ return GetSound()->Replay(); }
	virtual LRESULT Stop()	{ return GetSound()->Stop();   }
	virtual LRESULT Pause() { return GetSound()->Pause();  }
	virtual bool IsPlay() const	{ return GetSound()->IsPlay(); }
	virtual bool IsLoopPlay() const { return GetSound()->IsLoopPlay(); }// 追加
	virtual LRESULT SetLoopPlay(bool bLoop) { return GetSound()->SetLoopPlay(bLoop); }
	virtual LONG	GetCurrentPos() const { return GetSound()->GetCurrentPos(); }
	virtual LONG	GetLength() const { return GetSound()->GetLength();}
	virtual LRESULT SetCurrentPos(LONG lPos){ return GetSound()->SetCurrentPos(lPos);}
	virtual LRESULT SetVolume(LONG volume){ return GetSound()->SetVolume(volume);}
	virtual LONG	GetVolume() const { return GetSound()->GetVolume();}
	virtual string	GetFileName() const { return GetSound()->GetFileName(); }
	virtual int	GetType() const { return GetSound()->GetType(); }

	CProxySound::CProxySound() : m_lpSound(new CSoundNullDevice) {}

	void	SelectDevice(const smart_ptr<ISound>& p) { m_lpSound = p;}
	/**
		このクラス自体は、proxyパターンにより実装されているので
		そのproxyオブジェクトを差し替える
	*/
protected:
	smart_ptr<ISound>	m_lpSound;
	smart_ptr<ISound>	GetSound() const { return m_lpSound; }
};

/////////////////////////////////////////////////////////////////////////////

class ISoundFactory;
class CSound {
/**
	smart_ptr<ISound> といちいち書くのは面倒極まりないので．．

	※
		ISurfaceに対するCPlaneと同じ。
		ISoundに対してはCSound。
*/
public:

	LRESULT Load(const string& strFileName);
	///	↑このファイル(wav,mp3,mid,ogg..)を読み込む

	///	ポインタのふりをするための仕掛け
	ISound& operator*() const  {return *get(); }
	ISound* operator->() const {return get();  }

	///	ISurfaceへの暗黙の変換
	operator ISound* () const { return get(); }

	/**
		Null Check
			loadに失敗すれば、このクラスの抱えているポインタsmart_ptr<ISound>は
			Nullになるので、それを検出する。
	*/
	bool	isNull() const { return p_.isNull(); }

	ISound* get() const { return p_.get(); }

	static void SetFactory(const smart_ptr<ISoundFactory>&pFactory)
		{ pFactory_ = pFactory; }
	static smart_ptr<ISoundFactory> GetFactory()
		{ return pFactory_; }

	CSound(){}

	//CSound(const smart_ptr<ISoundFactory>& pFactory);
	///	soundのfactoryを渡せば、それを用いて生成して保持する

	CSound(const smart_ptr<ISound>& sound):	p_(sound) {}
	///	copy constructor

	CSound(ISound* sound):p_(sound) {}
	///　↑これは、所有権もCSoundが保持するので、
	///		CSound sound(new CMIDISound);
	///	のようにして使うこと！

protected:
	smart_ptr<ISound> p_; // p_;　←って泣いてるの？

	static ThreadLocal<smart_ptr<ISoundFactory> > pFactory_;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif
