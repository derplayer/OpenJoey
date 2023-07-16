#include "stdafx.h"
#include "yaneCDDA.h"
#include "../AppFrame/yaneAppManager.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

namespace {	//	無名namespaceに入れないとダメ
inline int sign(int val) { return val>0?1:(val<0?-1:0); }	//	符号を返す
}

CCDDA::CCDDA() {
	m_bLoopPlay	= false;
	m_uDeviceID	= (UINT)-1;
	m_bOpen		= false;
	m_bNowPlay	= false;
	m_bPaused	= 0;
	//	pauseのpauseに対応するため参照カウントにする
	CAppManager::Hook(this);		//	フックを開始する
	m_hWnd = CAppManager::GetHWnd();// ウィンドゥに関連付ける

	m_nStartPos	=0;
	m_nEndPos	=0;
	m_nPausePos	=0;
}

CCDDA::~CCDDA() {
	if (m_bOpen) Close();			// 呼び忘れてまんでー
	CAppManager::Unhook(this);		//	フックを解除する。
}

LRESULT	CCDDA::Open(const string& strDriveName) {	// CD Infoの取得も兼ねる
	m_strDriveName = strDriveName;
	bool bAgain = false;	// Closeを２度しないためのフラグ
	if (m_bOpen) {
		Close();	// Closeせずに呼ぶかー？うーん。ええんか？
		bAgain = true;
	}

retry: ;
	// open cdaudio
	MCI_OPEN_PARMS open;

	//	mci Open using WDM driver (for WindowsNT)
	ZERO(open);
	open.lpstrDeviceType	= (LPTSTR)MCI_DEVTYPE_CD_AUDIO;
	char szElementName[4];
	DWORD	dwFlags = MCI_OPEN_SHAREABLE | MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID;
	if (strDriveName.empty()) {
		//	ドライブ名の指定がない
	} else {
		//	ドライブ名の指定
		dwFlags |= MCI_OPEN_ELEMENT;
		wsprintf( szElementName, TEXT("%c:"), strDriveName.c_str()[0] );
		open.lpstrElementName = szElementName;
	}

	if (mciSendCommand(0,MCI_OPEN,dwFlags,(DWORD)(LPVOID)&open)){
		if (bAgain) {
			Err.Out("CCDDA::OpenでMCI_OPENに失敗");
			// CDドライブが存在しない場合、このエラー（のはず）
			return 1;
		} else {
			// あるいは、Closeできへてへんのちゃう？
			m_bOpen = true;
			Close();
			bAgain = true;
			goto retry;
		}
	}
	m_uDeviceID = open.wDeviceID;
	m_bOpen = true;	//	失敗したらCloseするために必要

	MCI_STATUS_PARMS status;
	ZERO(status);
	// status cdaudio number of tracks
	status.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
	if (mciSendCommand(m_uDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&status)){
		Err.Out("CCDDA::OpenでMCI_STATUS_ITEM(MCI_STATUS_NUMBER_OF_TRACKS)に失敗");
		Close();	//	閉じちゃう！
		return 2;	// CDが入っていない場合、このエラー。
	}
	int nSongMax = status.dwReturn;
	GetTrackInfo()->resize(nSongMax+2);

	// status cdaudio length
	status.dwItem = MCI_STATUS_LENGTH;
	if (mciSendCommand(m_uDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&status)){
		Err.Out("CCDDA::OpenでMCI_STATUS_ITEM(MCI_STATUS_LENGTH)に失敗");
		Close();	//	閉じちゃう！
		return 3;
	}
	(*GetTrackInfo())[0].m_dwSongLength = status.dwReturn;
	//	最後のトラック+1にもこれを記録しておく
	(*GetTrackInfo())[nSongMax+1].m_dwSongStart = status.dwReturn;
	(*GetTrackInfo())[nSongMax+1].m_dwSongLength = 0;
	
	for(int i=1;i<=nSongMax;i++){
		// status cdaudio position track
		status.dwItem	= MCI_STATUS_POSITION;
		status.dwTrack	= i;
		if (mciSendCommand(m_uDeviceID,MCI_STATUS,MCI_STATUS_ITEM|MCI_TRACK,(DWORD)&status)){
			Err.Out("CCDDA::OpenでMCI_STATUS(MCI_STATUS_POSITION)に失敗");
			Close();	//	閉じちゃう！
			return 4;
		}
		(*GetTrackInfo())[i].m_dwSongStart = status.dwReturn;

		// status cdaudio length track
		status.dwItem	= MCI_STATUS_LENGTH;
		status.dwTrack	= i;
		if (mciSendCommand(m_uDeviceID,MCI_STATUS,MCI_STATUS_ITEM|MCI_TRACK,(DWORD)&status)){
			Err.Out("CCDDA::OpenでMCI_STATUS(MCI_STATUS_LENGTH)に失敗");
			Close();	//	閉じちゃう！
			return 5;
		}
		(*GetTrackInfo())[i].m_dwSongLength = status.dwReturn;
	}

	// Openに失敗していたら、再生できないのだ...

	return 0;
}

LRESULT	CCDDA::Close(){
	if (!m_bOpen) return 0; // Openが成功していないのにCloseはナンセンスだね
	if (m_bNowPlay) Stop();	// stopしないとCloseできないよーん

	// close cdaudio
	if (m_uDeviceID == -1) return 1;
	::mciSendCommand(m_uDeviceID,MCI_CLOSE,0,(DWORD)NULL);
	m_uDeviceID = (UINT)-1;
	//	m_strDriveName = "";
	//	これは、保持しつづけないと、再オープンできない

	m_bOpen = false; // CLOSEしたもんねー！
	return 0;
}

LRESULT CCDDA::Play(){	// 全曲
	return Play(1,INT_MAX);	// 全曲なのだ
}

LRESULT CCDDA::Play(int n){ // 指定のトラックから再生
	return Play(n,n); // １曲だけなのだ
}

LRESULT CCDDA::Play(int n,int m){ // 指定のトラックから指定トラックの再生
	m_bPaused = 0;
	if (!m_bOpen) {
		if (Open(m_strDriveName)) {	//	Openで指定していたドライブを再生
			Err.Out("CCDDA::Playで、Openに失敗。");
			return 1;		// Openされていない！
		}
	}

	//	オーバーする処理を補整
	if (n > GetTrackMax()){
		return 0;
	}
	if (m > GetTrackMax()){
		m = GetTrackMax();
	}
	if (n<0) n=0;
	if (m<0) m=0;

	if (n>m) return 0; // 再生距離が0

	// play cdaudio from 曲頭 to 次の曲頭
	MCI_PLAY_PARMS play;
	play.dwFrom		= (*GetTrackInfo())[n]	.m_dwSongStart;
	play.dwTo		= (*GetTrackInfo())[m+1].m_dwSongStart;
	play.dwCallback = (DWORD) m_hWnd;

	m_nStartPos	= play.dwFrom;
	m_nEndPos	= play.dwTo;

	if (mciSendCommand(m_uDeviceID,MCI_PLAY,MCI_FROM|MCI_TO|MCI_NOTIFY,(DWORD)&play)){
		Err.Out("CCDDA::PlayでMCI_PLAYに失敗");
		return 3;
	}
/*
		// play cdaudio from 曲頭
		MCI_PLAY_PARMS play;
		play.dwFrom		= m_dwSongStart[n];
		play.dwCallback = (DWORD) m_hWnd;
*/

	m_bNowPlay = true;
	return 0;
}

LRESULT CCDDA::PlayPosToTrack(DWORD dwPos,bool bReplay){ // 位置を指定して、そこからそのトラックのみを再生。
	// dwPos = Start Pos[MSF]
	HRESULT hr;
	int nTrack,nHour,nMin,nSec,nMS;
	hr = ConvertToMealTime(dwPos,nTrack,nHour,nMin,nSec,nMS);
	if ( hr != 0 ) return 1;

	DWORD dwEndPos;
	// 最終トラックをオーバーしているか？
	if ( (nTrack+1) > GetTrackMax() ){
		dwEndPos = (DWORD)~0;
		//	↑infinity
	}else{
		dwEndPos = (*GetTrackInfo())[nTrack+1].m_dwSongStart;
	}

	return PlayPosToPos(dwPos,dwEndPos,bReplay);
}

LRESULT CCDDA::PlayDWToEnd(DWORD dwPos,bool bReplay){ 
	DWORD dwEndPos;
	dwEndPos = (DWORD)~0;
	return PlayPosToPos(dwPos,dwEndPos,bReplay);
}

LRESULT CCDDA::PlayPosToPos(DWORD n,DWORD m,bool bReplay){ // 指定のポジションから指定ポジションの再生
	m_bPaused = 0;	//	pauseしてるやつはリセットする
	return PlayDW2(n,m,bReplay);
}

//	内部的に利用
LRESULT CCDDA::PlayDW2(DWORD n,DWORD m,bool bReplay){
	if (m_bPaused>0){
		//	もしpause中..
		//	（Pauseをかけた直後にLoop再生のメッセージが飛んできたのか．．）
		m_nStartPos = n;
		m_nPausePos = n;
		m_nEndPos = m;
		return 0;
	}
	
	if (!m_bOpen) {
		Err.Out("CCDDA::PlayDWが、Openされていないのに呼び出されました");
		return 1;		// Openされていない！
	}
	if (m_uDeviceID == -1) return 1;

	if (n>=m) return 0;	//	perhaps,データトラック

	if (m != ~0) {	// m!=~0(infinity)
		// play cdaudio from 曲頭 to 次の曲頭
		MCI_PLAY_PARMS play;
		play.dwFrom		= n;
		play.dwTo		= m;
		play.dwCallback = (DWORD) m_hWnd;

		if (bReplay){
			//	リプレイによる再生ポジションを指定されているのならば
			//	そこから
		} else {
			m_nStartPos	= n;
		}
		m_nEndPos	= m;

		if (mciSendCommand(m_uDeviceID,MCI_PLAY,MCI_FROM|MCI_TO|MCI_NOTIFY,(DWORD)&play)){
			Err.Out("CCDDA::PlayDWでMCI_PLAYに失敗");
			return 2;
		}
	} else {
		// play cdaudio from 曲頭
		MCI_PLAY_PARMS play;
		play.dwFrom		= n;
		play.dwCallback = (DWORD) m_hWnd;

		if (!bReplay) m_nStartPos	= n;
		m_nEndPos	= (DWORD)~0;	// Infinity

		if (mciSendCommand(m_uDeviceID,MCI_PLAY,MCI_FROM|MCI_NOTIFY,(DWORD)&play)){
			Err.Out("CCDDA::PlayDWでMCI_PLAYに失敗");
			return 2;
		}
	}

	m_bNowPlay = true;
	return 0;
}


LRESULT	CCDDA::Stop(){
	if (!m_bOpen) {		// あられらりー
		Err.Out("CCDDA::Stopが、Openされていないのに呼び出されました");
		return 1;		// Openされていない！
	}
	if (!IsPlay()) {	// 再生されていないものはStopもしない
//		Err.Out("CCDDA::Stopが、再生中でもないのに呼び出されました");
		return 2;
	}
	m_bNowPlay = false;
	if (m_uDeviceID == -1) return 1;

	// stop cdaudio
	mciSendCommand(m_uDeviceID,MCI_STOP,0,(DWORD)NULL);

	return 0;
}

LRESULT	CCDDA::Pause(){
	m_bPaused += sign(m_bPaused);	//	必殺技:p

	if (!m_bOpen) {
	//	Err.Out("CCDDA::Pauseが、Openされていないのに呼び出されました");
		return 1;		// Openされていない！
	}
	if (!IsPlay()) return 2;	// 再生されていないものはPauseもしない

	//	再生中のをpauseしたならば
	m_bPaused = 1;

	if (GetCurrentPos(m_nPausePos)) {
		Err.Out("CCDDA::PauseでGetCurrentPosに失敗");
		return 3;
	}
	Stop();

	return 0;
}

LRESULT	CCDDA::Replay(){
	if (!m_bOpen) return 1;		// Openされていない！
	// PausePosを保存しているので、
	// Pause->Stop->Close->Open->ReplayとすればいったんCDを停止させても大丈夫！

	if (!m_bPaused) return 0; // pauseしてへんて！
	
	if (--m_bPaused==0) {	//	参照カウント方式
		PlayDW2(m_nPausePos,m_nEndPos,true);	// 再生再開
	}
	return 0;
}

LRESULT	CCDDA::GetCurrentPos(DWORD &dw){
	if (!m_bOpen) { dw = 0; return 1; }		// Openされていない！
	if (m_uDeviceID == -1) return 1;

	// 現在の再生ポジションを得る
	MCI_STATUS_PARMS status;
	status.dwItem = MCI_STATUS_POSITION;
	if (mciSendCommand(m_uDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&status)){
		Err.Out("CCDDA::GetCurrentPosでMCI_STATUS(MCI_STATUS_POSITION)に失敗");
		dw = 0;
		return 2;	// CDが入っていない場合、このエラー
	}

	dw = status.dwReturn;
	return 0;
}

LRESULT	CCDDA::LoopPlay(){
	if (m_bLoopPlay) {
		return PlayDW2(m_nStartPos,m_nEndPos,false);
	} else {
		return Stop();
	}
}

void	CCDDA::SetLoopMode(bool bLoop){
	m_bLoopPlay = bLoop;
}

int		CCDDA::GetTrackMax() {
	if ( m_bOpen || Open(m_strDriveName) == 0 )
		return GetTrackInfo()->size()-2;
	return 0;
}

bool	CCDDA::IsPlay() const {
	return m_bNowPlay;
}

LRESULT	CCDDA::Eject(bool bEject){
	if (m_uDeviceID == -1) return 1;

	// MCI_STATUS_PARMS status;
	DWORD command;
	if (bEject) {
		command = MCI_SET_DOOR_OPEN;
	} else {
		command = MCI_SET_DOOR_CLOSED;
	}
	if (mciSendCommand(m_uDeviceID,MCI_SET,command,NULL)){
		Err.Out("CCDDA::Ejectに失敗");
		return 1;
	}
	m_bNowPlay	= false;
	m_bOpen		= false;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

int		CCDDA::GetPlayTrack(){
	DWORD dw;
	if (GetCurrentPos(dw)!=0) return -1;
	dw = ((dw&0xff)<<24) + (dw&0xff00) + ((dw&0xff0000) >> 24);
	int nTracks = GetTrackMax();
	for (int i=1;i<=nTracks;++i){
		DWORD dwStart = (*GetTrackInfo())[i].m_dwSongStart;
		dwStart = ((dwStart&0xff)<<24) + (dwStart&0xff00) + ((dwStart&0xff0000) >> 24);
		if (dw < dwStart) return i-1;
	}
	return nTracks;
}

LRESULT CCDDA::PlayPrev(bool bEndless){
	int n = GetPlayTrack();
	if (n==1) {
		if (bEndless) {	//	最後の曲を再生
			return Play(GetTrackMax());
		} else {
			return 1;	//	失敗
		}
	}
	return Play(n-1);
}

LRESULT CCDDA::PlayNext(bool bEndless){
	int n = GetPlayTrack();
	if (n==GetTrackMax()) {
		if (bEndless) { // 最初の曲を再生
			return Play(1);
		} else {
			return 1;	//	失敗
		}
	}
	return Play(n+1);
}

/*
#define MCI_MSF_MINUTE(msf)				((BYTE)(msf))
#define MCI_MSF_SECOND(msf)				((BYTE)(((WORD)(msf)) >> 8))
#define MCI_MSF_FRAME(msf)				((BYTE)((msf)>>16))
*/

LONG CCDDA::ConvertToSecond(DWORD dwPos){
	DWORD dwMin,dwSec,dwFrm;
	dwFrm = MCI_MSF_FRAME(dwPos);
	dwSec = MCI_MSF_SECOND(dwPos);
	dwMin = MCI_MSF_MINUTE(dwPos);
	return dwMin*60+dwSec;
}

LRESULT CCDDA::ConvertToMealTime(DWORD dwPos,int& nTrack,int&nHour,int&nMin,int&nSec,int&nMS){
	if ( !m_bOpen ) return -1;
	// dwPos = msf
	int n = GetTrackMax();
	int i;
	for (i = 1 ; i <= n ; i++ ){
		if ( ConvertToSecond((*GetTrackInfo())[i].m_dwSongStart)
			> ConvertToSecond(dwPos) ) break;
	}
	nTrack = i-1;

	DWORD min = MCI_MSF_MINUTE(dwPos);
	nHour = min / 60;
	nMin = min % 60;
	nSec = MCI_MSF_SECOND(dwPos);
	nMS = 0;

	return 0;
}

LRESULT CCDDA::ConvertFromMealTime(DWORD& dwPos,int nTrack,int nHour,int nMin,int nSec,int nMS){
	if ( !m_bOpen ) return -1;

	if ( nTrack > GetTrackMax() ) return -1;
	DWORD dwLen = (*GetTrackInfo())[nTrack].m_dwSongLength;
	DWORD dwTMSFSec = ConvertToSecond(dwLen);
//	DWORD dwTSec = nHour * 24*60+nMin *60+nSec;
	DWORD dwTSec = nHour*60*60 + nMin*60 + nSec;

	// 曲の長さ以上の位置を指定したらだめ
	if ( dwTMSFSec < dwTSec ) return -1;

	DWORD dwTPos = (*GetTrackInfo())[nTrack].m_dwSongStart;
	DWORD dwTPosMSFMin = ConvertToSecond(dwTPos);
	DWORD dwPSec = dwTPosMSFMin + dwTSec;
	DWORD dwPosSec,dwPosMin;
	dwPosMin = dwPSec / 60;
	dwPosSec = dwPSec % 60;

	dwPos = MCI_MAKE_MSF(dwPosMin, dwPosSec, 0);

	return 0;
}


//////////////////////////////////////////////////////////////////////////////

// WM_MCINOTIFYの通知を受け取る必要あり
LRESULT CCDDA::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){
	switch(uMsg){
	case MM_MCINOTIFY : {
		// このメッセージはCDのんか？
		if (lParam!=(LPARAM)m_uDeviceID) return 0; // 違うやん！

		// Stopで停止させたときは、MCI_NOTIFY_ABORTED
		if (wParam==MCI_NOTIFY_SUCCESSFUL){
			LoopPlay();
			return 1;
		}
	}

	} // end switch

	return 0;
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
