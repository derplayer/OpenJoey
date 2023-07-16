// yaneCDDA.h
//	CDDA wrapper
//
//		programmed by yaneurao(M.Isozaki) '99/07/08
//

#ifndef __yaneCDDA_h__
#define __yaneCDDA_h__


#include "../Window/yaneWinHook.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

struct CDDAInfo {
/**
	CDDAのトラック情報を格納するための構造体
	class CCDDA で使います
*/
	DWORD	m_dwSongLength;	//	曲の長さ（この配列の[0]は全曲の長さ)
	DWORD	m_dwSongStart;	//	曲の開始ポジション
};

class ICDDA {
public:
	virtual LRESULT	Open(const string& strDriveName="")=0;
	virtual LRESULT	Close()=0;
	virtual LRESULT	Stop()=0;
	virtual LRESULT	Pause()=0;
	virtual LRESULT	Replay()=0;
	virtual LRESULT Play()=0;
	virtual int		GetPlayTrack()=0;

	virtual LRESULT PlayPrev(bool bEndless=true)=0;
	virtual LRESULT PlayNext(bool bEndless=true)=0;

	virtual LRESULT Play(int)=0;
	virtual LRESULT Play(int,int)=0;

	virtual LRESULT	GetCurrentPos(DWORD &dw)=0;

	virtual LRESULT PlayPosToPos(DWORD,DWORD,bool bReplay=false)=0;
	virtual LRESULT PlayPosToTrack(DWORD dwPos,bool bReplay=false)=0;
	virtual LRESULT PlayDWToEnd(DWORD dwPos,bool bReplay=false)=0;
	virtual void	SetStartPos(DWORD dw)=0;
	virtual bool	IsPlay() const=0;
	virtual void	SetLoopMode(bool bLoop)=0;
	virtual int		GetTrackMax()=0;
	virtual LRESULT	Eject(bool bEject)=0;
	virtual bool	IsOpen() const=0;
	virtual LRESULT ConvertToMealTime(DWORD dwPos,int& nTrack,int&nHour,int&nMin,int&nSec,int&nMS)=0;
	virtual LRESULT ConvertFromMealTime(DWORD& dwPos,int nTrack,int nHour,int nMin,int nSec,int nMS)=0;
	virtual vector <CDDAInfo>*	GetTrackInfo()=0;

	virtual ~ICDDA(){}
};

class CCDDA : public IWinHook , public ICDDA {
/**
	CD再生のためのコンポーネント。
	マルチドライブでのCD再生等もサポートしています。

	OpenせずともPlayで直接再生することが出来ます。
	Openするメリットは、曲数・曲長さ情報
	GetTrackInfo()　が更新されること。

	ＣＤを回転させてスタンバイするのでPlayを呼び出したあと
	再生するの時間が１秒ほど短縮できることです。

	CCDDAのコンストラクタに先行してウィンドゥが完成している必要があるので、
	class CAppFrame 内で使用するようにしてください。
*/
public:

	LRESULT	Open(const string& strDriveName="");
	///		CD初期化。呼び出すとCD回るよ:p
	///		ドライブ名の指定も出来る。例）Open("g"); // G Drive

	LRESULT	Close();
	///	CD使用終了。CDの回転は止まる。

	//////////////////////////////////////////////////////////////////

	LRESULT	Stop();			///	再生停止
	LRESULT	Pause();		///	Pause
	LRESULT	Replay();		///	Pauseでいったん停止させていたものを再開

	LRESULT Play();			///	頭から再生

	int		GetPlayTrack();	
	///	現在再生中のトラックを返す

	LRESULT PlayPrev(bool bEndless=true);	///	前のトラックを再生
	LRESULT PlayNext(bool bEndless=true);	///	次のトラックを再生

	LRESULT Play(int);			///	トラック番号を選択して再生
	LRESULT Play(int,int);		///	２トラック間の再生

	LRESULT	GetCurrentPos(DWORD &dw);
	/**
	現在の再生ポジションを得る[MSF:Multimedia Sound Format]
	MSF形式とは、h:時間 m:分 s:秒 f:フレーム(セクタ)で表される形式です。
	*.cdaでは、上位からf/s/m/hの順で、各１バイトで表現されてます。
	*/

	LRESULT PlayPosToPos(DWORD,DWORD,bool bReplay=false);
	/// ２ポジション間の再生
	LRESULT PlayPosToTrack(DWORD dwPos,bool bReplay=false);
	/// 位置を指定して、そこからそのトラックのみを再生。
	LRESULT PlayDWToEnd(DWORD dwPos,bool bReplay=false);
	/// 位置を指定して、そこから終了トラックの最後まで再生
	/**
		上記３つのPlay関数で、bReplay==trueにするときは、
		ループプレイのときに、次のSetStartPosで設定した場所に戻る
	*/
	void	SetStartPos(DWORD dw){
		m_nStartPos = dw;
	}

	bool	IsPlay() const;
	///	演奏中なのか？

	void	SetLoopMode(bool bLoop);
	///	ループで再生するか(default:false)

	int		GetTrackMax();
	///	曲数の取得(Openしていることが必要)

	LRESULT	Eject(bool bEject);
	///	CDのEjectを行なう。

	bool	IsOpen() const { return m_bOpen;}
	///	CDをopenしているか

	virtual LRESULT ConvertToMealTime(DWORD dwPos,int& nTrack,int&nHour,int&nMin,int&nSec,int&nMS);
	virtual LRESULT ConvertFromMealTime(DWORD& dwPos,int nTrack,int nHour,int nMin,int nSec,int nMS);
	/**
		class CSoundBase のConvertToMealTime/ConvertFromMealTimeと
		同じ仕様のもの。ただし、ここではtrackを指定できること、
		そしてポジションとして使われるのは、[ms]単位ではなく[MSF]で
		あることが違う。

		CCDDA::GetCurrentPosで取得した値をそのままこの関数で変換したり、
		トラック番号＋時分秒毛から変換した値をPlay(DWORD,DWORD)で、
		そのままその位置から再生できる。
	*/

	//////////////////////////////////////////////////////////////////

	///	トラック情報の取得
	vector <CDDAInfo>*	GetTrackInfo(){ return &m_aTrackInfo; }
	/**
		全曲長さは GetTrackInfo()[0].m_dwSongLength
		GetTrackInfo()[1].m_dwSongLength
			// 1トラック目の長さ
		GetTrackInfo()[1].m_dwSongStart
			// 1トラック目のスタートポジション[msf]
		GetTrackInfo()->rbegin()->m_dwSongStart
			//	最後のトラックの終了ポジション[msf]
	*/

	//////////////////////////////////////////////////////////////////

	CCDDA();
	virtual ~CCDDA();

//////////////////////////////////////////////////////////////////////////////
protected:
	HWND	m_hWnd;			//	MM_MCINOTIFY通知が来る
	UINT	m_uDeviceID;	//	CDのデバイスID
	bool	m_bOpen;		//	オープン中か？(この間、ＣＤは回りっぱなし)

	bool	m_bLoopPlay;	//	Loop再生するのか？
	DWORD	m_nStartPos;	//	Loop再生の開始位置
	DWORD	m_nEndPos;		//	Loop再生の終了位置
	LRESULT	LoopPlay();		//	LoopPlay用

	DWORD	m_nPausePos;	//	Pauseポジション
	int		m_bPaused;		//	Pause中なのか？
	bool	m_bNowPlay;		//	現在Play中なのか？
	string	m_strDriveName;	//	ドライブ名

	//	トラック情報
	vector <CDDAInfo>	m_aTrackInfo;

	LONG ConvertToSecond(DWORD dwPos);	// [MSF] -> [Sec]
	
	LRESULT PlayDW2(DWORD,DWORD,bool bReplay=false); // ２ポジション間の再生

	// override from CWinHook
	LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif
