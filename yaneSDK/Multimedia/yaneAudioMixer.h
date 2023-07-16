//	yaneAudioMix.h : ウェーブオーディオミキサ
//
//		programmed by yaneurao '00/02/28
//

#ifndef __yaneAudioMixer_h__
#define __yaneAudioMixer_h__


namespace yaneuraoGameSDK3rd {
namespace Multimedia {

enum eAudioMix {
AUDIO_MIX_MASTER,	// マスターヴォリューム
AUDIO_MIX_WAVE,		// WAVEヴォリューム
AUDIO_MIX_MIDI,		// MIDI出力ヴォリューム
AUDIO_MIX_CD		// CD再生ヴォリューム
};

class IAudioMixer {
public:
	virtual LRESULT SetVolumeRel(eAudioMix eDevice,double dRelVolume)=0;
	virtual LRESULT SetVolumeAbs(eAudioMix eDevice,const vector<DWORD>& adwVol)=0;
	virtual LRESULT SetVolumeOrg(eAudioMix eDevice)=0;
	virtual LRESULT GetVolumeRel(eAudioMix eDevice,double &dRelVolume)const=0;
	virtual LRESULT GetVolumeAbs(eAudioMix eDevice,vector<DWORD>& adwVol)const=0;
	virtual LRESULT	GetVolumeOrg(eAudioMix eDevice,vector<DWORD>& adwVol)const=0;
	virtual	LRESULT	RestoreVolume(eAudioMix eDevice)=0;

	virtual ~IAudioMixer(){}
};

class CAudioMixer : public IAudioMixer {
/**
	オーディオのミキシング用。MIDI、CD、WAVEのミキシングを直接変更する。
	ミキシングをコントロールするので、その機器から直接スピーカーに
	出力している場合は無意味である。

	class CVolumeFader も参照すること。

	相対ヴォリュームは、初期値（CAudioMixerのコンストラクトが
	完成した時点での値）からの相対指定となります。
	初期値ならば1(100%)その２倍ならば2(200%)、０ならば無音量。
*/
public:
	virtual LRESULT SetVolumeRel(eAudioMix eDevice,double dRelVolume);
	///	相対ボリューム設定

	virtual LRESULT SetVolumeAbs(eAudioMix eDevice,const vector<DWORD>& adwVol);
	///	絶対ボリューム設定

	virtual LRESULT SetVolumeOrg(eAudioMix eDevice);
	///	初期ボリュームに設定

	virtual LRESULT GetVolumeRel(eAudioMix eDevice,double &dRelVolume)const;
	///	相対ボリューム取得

	virtual LRESULT GetVolumeAbs(eAudioMix eDevice,vector<DWORD>& adwVol)const;
	///	絶対ボリューム取得

	virtual LRESULT	GetVolumeOrg(eAudioMix eDevice,vector<DWORD>& adwVol)const;
	///	初期ボリューム取得

	virtual	LRESULT	RestoreVolume(eAudioMix eDevice);
	///	現在のボリューム設定を、「初期ボリューム設定」に保存する

	bool	IsSuccessInit() const { return m_bSuccessInit; }
	///	初期化に成功したか？

	CAudioMixer();
	virtual ~CAudioMixer();

protected:
	DWORD	m_dwVol;				//	現在のボリューム設定
	LONG	m_lPan;					//	現在のパン設定

private:
	struct CAudioInfo {
		bool			m_bAudioComponent;		//	コンポーネントがサポートされているか否かのフラグ
		vector<DWORD>	m_dwOriginalVol;		//	元のボリューム設定(チャンネル)
		CAudioInfo() :m_bAudioComponent(false) {}
	};
	CAudioInfo	m_vAudioInfo[4];
	CAudioInfo* GetAudioInfo() { return& m_vAudioInfo[0]; }

	HMIXER		m_hMixer;			//	ミキサデバイスのハンドル

	UINT		m_MixerID;			//	有効なミキサデバイスの識別子
	
	MIXERCAPS	m_MixerCaps;		//	ミキサデバイス情報構造体
	
	MIXERLINE	m_MixerLine[4];		//	4つのAudioMixに対するLine(順番はenum順)
	//	オーディオライン状態とメトリックス情報が入る構造体
	
	MIXERCONTROL		m_MixerControl;
	//	オーディオラインコントロールのステータスとメトリックス情報が入る構造体

	MIXERLINECONTROLS	m_MixerLineControl;
	//	オーディオラインのコントロール情報が入る構造体

	MIXERCONTROLDETAILS	m_MCD;
	//	ミキサコントロールの状態情報構造体

	smart_ptr<MIXERCONTROLDETAILS_UNSIGNED>	m_MCD_UnSigned;
	//	ミキサコントロールのプロパティを取得、設定できる構造体

protected:
	LRESULT	Open();
	LRESULT Close();

	LRESULT	GetMixerLineControl( int iLineID );		//	Detailの取得も兼ねる。下の関数に委譲する
	LRESULT GetMixerLineControl(MIXERLINE& ml);		//	こちらが本物＾＾；
	LRESULT	SetMixerControlDetails( int iLineID, bool bMono );

private:
	LRESULT	GetMixerDev();
	void	ErrAudioMixer( MMRESULT AMErr, LPCSTR strBuf );
	bool	m_bSuccessInit;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif
