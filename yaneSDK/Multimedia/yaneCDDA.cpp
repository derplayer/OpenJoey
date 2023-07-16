#include "stdafx.h"
#include "yaneCDDA.h"
#include "../AppFrame/yaneAppManager.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

namespace {	//	����namespace�ɓ���Ȃ��ƃ_��
inline int sign(int val) { return val>0?1:(val<0?-1:0); }	//	������Ԃ�
}

CCDDA::CCDDA() {
	m_bLoopPlay	= false;
	m_uDeviceID	= (UINT)-1;
	m_bOpen		= false;
	m_bNowPlay	= false;
	m_bPaused	= 0;
	//	pause��pause�ɑΉ����邽�ߎQ�ƃJ�E���g�ɂ���
	CAppManager::Hook(this);		//	�t�b�N���J�n����
	m_hWnd = CAppManager::GetHWnd();// �E�B���h�D�Ɋ֘A�t����

	m_nStartPos	=0;
	m_nEndPos	=0;
	m_nPausePos	=0;
}

CCDDA::~CCDDA() {
	if (m_bOpen) Close();			// �ĂіY��Ă܂�Ł[
	CAppManager::Unhook(this);		//	�t�b�N����������B
}

LRESULT	CCDDA::Open(const string& strDriveName) {	// CD Info�̎擾�����˂�
	m_strDriveName = strDriveName;
	bool bAgain = false;	// Close���Q�x���Ȃ����߂̃t���O
	if (m_bOpen) {
		Close();	// Close�����ɌĂԂ��[�H���[��B�����񂩁H
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
		//	�h���C�u���̎w�肪�Ȃ�
	} else {
		//	�h���C�u���̎w��
		dwFlags |= MCI_OPEN_ELEMENT;
		wsprintf( szElementName, TEXT("%c:"), strDriveName.c_str()[0] );
		open.lpstrElementName = szElementName;
	}

	if (mciSendCommand(0,MCI_OPEN,dwFlags,(DWORD)(LPVOID)&open)){
		if (bAgain) {
			Err.Out("CCDDA::Open��MCI_OPEN�Ɏ��s");
			// CD�h���C�u�����݂��Ȃ��ꍇ�A���̃G���[�i�̂͂��j
			return 1;
		} else {
			// ���邢�́AClose�ł��ւĂւ�̂��Ⴄ�H
			m_bOpen = true;
			Close();
			bAgain = true;
			goto retry;
		}
	}
	m_uDeviceID = open.wDeviceID;
	m_bOpen = true;	//	���s������Close���邽�߂ɕK�v

	MCI_STATUS_PARMS status;
	ZERO(status);
	// status cdaudio number of tracks
	status.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
	if (mciSendCommand(m_uDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&status)){
		Err.Out("CCDDA::Open��MCI_STATUS_ITEM(MCI_STATUS_NUMBER_OF_TRACKS)�Ɏ��s");
		Close();	//	�����Ⴄ�I
		return 2;	// CD�������Ă��Ȃ��ꍇ�A���̃G���[�B
	}
	int nSongMax = status.dwReturn;
	GetTrackInfo()->resize(nSongMax+2);

	// status cdaudio length
	status.dwItem = MCI_STATUS_LENGTH;
	if (mciSendCommand(m_uDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&status)){
		Err.Out("CCDDA::Open��MCI_STATUS_ITEM(MCI_STATUS_LENGTH)�Ɏ��s");
		Close();	//	�����Ⴄ�I
		return 3;
	}
	(*GetTrackInfo())[0].m_dwSongLength = status.dwReturn;
	//	�Ō�̃g���b�N+1�ɂ�������L�^���Ă���
	(*GetTrackInfo())[nSongMax+1].m_dwSongStart = status.dwReturn;
	(*GetTrackInfo())[nSongMax+1].m_dwSongLength = 0;
	
	for(int i=1;i<=nSongMax;i++){
		// status cdaudio position track
		status.dwItem	= MCI_STATUS_POSITION;
		status.dwTrack	= i;
		if (mciSendCommand(m_uDeviceID,MCI_STATUS,MCI_STATUS_ITEM|MCI_TRACK,(DWORD)&status)){
			Err.Out("CCDDA::Open��MCI_STATUS(MCI_STATUS_POSITION)�Ɏ��s");
			Close();	//	�����Ⴄ�I
			return 4;
		}
		(*GetTrackInfo())[i].m_dwSongStart = status.dwReturn;

		// status cdaudio length track
		status.dwItem	= MCI_STATUS_LENGTH;
		status.dwTrack	= i;
		if (mciSendCommand(m_uDeviceID,MCI_STATUS,MCI_STATUS_ITEM|MCI_TRACK,(DWORD)&status)){
			Err.Out("CCDDA::Open��MCI_STATUS(MCI_STATUS_LENGTH)�Ɏ��s");
			Close();	//	�����Ⴄ�I
			return 5;
		}
		(*GetTrackInfo())[i].m_dwSongLength = status.dwReturn;
	}

	// Open�Ɏ��s���Ă�����A�Đ��ł��Ȃ��̂�...

	return 0;
}

LRESULT	CCDDA::Close(){
	if (!m_bOpen) return 0; // Open���������Ă��Ȃ��̂�Close�̓i���Z���X����
	if (m_bNowPlay) Stop();	// stop���Ȃ���Close�ł��Ȃ���[��

	// close cdaudio
	if (m_uDeviceID == -1) return 1;
	::mciSendCommand(m_uDeviceID,MCI_CLOSE,0,(DWORD)NULL);
	m_uDeviceID = (UINT)-1;
	//	m_strDriveName = "";
	//	����́A�ێ����Â��Ȃ��ƁA�ăI�[�v���ł��Ȃ�

	m_bOpen = false; // CLOSE��������ˁ[�I
	return 0;
}

LRESULT CCDDA::Play(){	// �S��
	return Play(1,INT_MAX);	// �S�ȂȂ̂�
}

LRESULT CCDDA::Play(int n){ // �w��̃g���b�N����Đ�
	return Play(n,n); // �P�Ȃ����Ȃ̂�
}

LRESULT CCDDA::Play(int n,int m){ // �w��̃g���b�N����w��g���b�N�̍Đ�
	m_bPaused = 0;
	if (!m_bOpen) {
		if (Open(m_strDriveName)) {	//	Open�Ŏw�肵�Ă����h���C�u���Đ�
			Err.Out("CCDDA::Play�ŁAOpen�Ɏ��s�B");
			return 1;		// Open����Ă��Ȃ��I
		}
	}

	//	�I�[�o�[���鏈����␮
	if (n > GetTrackMax()){
		return 0;
	}
	if (m > GetTrackMax()){
		m = GetTrackMax();
	}
	if (n<0) n=0;
	if (m<0) m=0;

	if (n>m) return 0; // �Đ�������0

	// play cdaudio from �ȓ� to ���̋ȓ�
	MCI_PLAY_PARMS play;
	play.dwFrom		= (*GetTrackInfo())[n]	.m_dwSongStart;
	play.dwTo		= (*GetTrackInfo())[m+1].m_dwSongStart;
	play.dwCallback = (DWORD) m_hWnd;

	m_nStartPos	= play.dwFrom;
	m_nEndPos	= play.dwTo;

	if (mciSendCommand(m_uDeviceID,MCI_PLAY,MCI_FROM|MCI_TO|MCI_NOTIFY,(DWORD)&play)){
		Err.Out("CCDDA::Play��MCI_PLAY�Ɏ��s");
		return 3;
	}
/*
		// play cdaudio from �ȓ�
		MCI_PLAY_PARMS play;
		play.dwFrom		= m_dwSongStart[n];
		play.dwCallback = (DWORD) m_hWnd;
*/

	m_bNowPlay = true;
	return 0;
}

LRESULT CCDDA::PlayPosToTrack(DWORD dwPos,bool bReplay){ // �ʒu���w�肵�āA�������炻�̃g���b�N�݂̂��Đ��B
	// dwPos = Start Pos[MSF]
	HRESULT hr;
	int nTrack,nHour,nMin,nSec,nMS;
	hr = ConvertToMealTime(dwPos,nTrack,nHour,nMin,nSec,nMS);
	if ( hr != 0 ) return 1;

	DWORD dwEndPos;
	// �ŏI�g���b�N���I�[�o�[���Ă��邩�H
	if ( (nTrack+1) > GetTrackMax() ){
		dwEndPos = (DWORD)~0;
		//	��infinity
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

LRESULT CCDDA::PlayPosToPos(DWORD n,DWORD m,bool bReplay){ // �w��̃|�W�V��������w��|�W�V�����̍Đ�
	m_bPaused = 0;	//	pause���Ă��̓��Z�b�g����
	return PlayDW2(n,m,bReplay);
}

//	�����I�ɗ��p
LRESULT CCDDA::PlayDW2(DWORD n,DWORD m,bool bReplay){
	if (m_bPaused>0){
		//	����pause��..
		//	�iPause�������������Loop�Đ��̃��b�Z�[�W�����ł����̂��D�D�j
		m_nStartPos = n;
		m_nPausePos = n;
		m_nEndPos = m;
		return 0;
	}
	
	if (!m_bOpen) {
		Err.Out("CCDDA::PlayDW���AOpen����Ă��Ȃ��̂ɌĂяo����܂���");
		return 1;		// Open����Ă��Ȃ��I
	}
	if (m_uDeviceID == -1) return 1;

	if (n>=m) return 0;	//	perhaps,�f�[�^�g���b�N

	if (m != ~0) {	// m!=~0(infinity)
		// play cdaudio from �ȓ� to ���̋ȓ�
		MCI_PLAY_PARMS play;
		play.dwFrom		= n;
		play.dwTo		= m;
		play.dwCallback = (DWORD) m_hWnd;

		if (bReplay){
			//	���v���C�ɂ��Đ��|�W�V�������w�肳��Ă���̂Ȃ��
			//	��������
		} else {
			m_nStartPos	= n;
		}
		m_nEndPos	= m;

		if (mciSendCommand(m_uDeviceID,MCI_PLAY,MCI_FROM|MCI_TO|MCI_NOTIFY,(DWORD)&play)){
			Err.Out("CCDDA::PlayDW��MCI_PLAY�Ɏ��s");
			return 2;
		}
	} else {
		// play cdaudio from �ȓ�
		MCI_PLAY_PARMS play;
		play.dwFrom		= n;
		play.dwCallback = (DWORD) m_hWnd;

		if (!bReplay) m_nStartPos	= n;
		m_nEndPos	= (DWORD)~0;	// Infinity

		if (mciSendCommand(m_uDeviceID,MCI_PLAY,MCI_FROM|MCI_NOTIFY,(DWORD)&play)){
			Err.Out("CCDDA::PlayDW��MCI_PLAY�Ɏ��s");
			return 2;
		}
	}

	m_bNowPlay = true;
	return 0;
}


LRESULT	CCDDA::Stop(){
	if (!m_bOpen) {		// �������[
		Err.Out("CCDDA::Stop���AOpen����Ă��Ȃ��̂ɌĂяo����܂���");
		return 1;		// Open����Ă��Ȃ��I
	}
	if (!IsPlay()) {	// �Đ�����Ă��Ȃ����̂�Stop�����Ȃ�
//		Err.Out("CCDDA::Stop���A�Đ����ł��Ȃ��̂ɌĂяo����܂���");
		return 2;
	}
	m_bNowPlay = false;
	if (m_uDeviceID == -1) return 1;

	// stop cdaudio
	mciSendCommand(m_uDeviceID,MCI_STOP,0,(DWORD)NULL);

	return 0;
}

LRESULT	CCDDA::Pause(){
	m_bPaused += sign(m_bPaused);	//	�K�E�Z:p

	if (!m_bOpen) {
	//	Err.Out("CCDDA::Pause���AOpen����Ă��Ȃ��̂ɌĂяo����܂���");
		return 1;		// Open����Ă��Ȃ��I
	}
	if (!IsPlay()) return 2;	// �Đ�����Ă��Ȃ����̂�Pause�����Ȃ�

	//	�Đ����̂�pause�����Ȃ��
	m_bPaused = 1;

	if (GetCurrentPos(m_nPausePos)) {
		Err.Out("CCDDA::Pause��GetCurrentPos�Ɏ��s");
		return 3;
	}
	Stop();

	return 0;
}

LRESULT	CCDDA::Replay(){
	if (!m_bOpen) return 1;		// Open����Ă��Ȃ��I
	// PausePos��ۑ����Ă���̂ŁA
	// Pause->Stop->Close->Open->Replay�Ƃ���΂�������CD���~�����Ă����v�I

	if (!m_bPaused) return 0; // pause���Ăւ�āI
	
	if (--m_bPaused==0) {	//	�Q�ƃJ�E���g����
		PlayDW2(m_nPausePos,m_nEndPos,true);	// �Đ��ĊJ
	}
	return 0;
}

LRESULT	CCDDA::GetCurrentPos(DWORD &dw){
	if (!m_bOpen) { dw = 0; return 1; }		// Open����Ă��Ȃ��I
	if (m_uDeviceID == -1) return 1;

	// ���݂̍Đ��|�W�V�����𓾂�
	MCI_STATUS_PARMS status;
	status.dwItem = MCI_STATUS_POSITION;
	if (mciSendCommand(m_uDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&status)){
		Err.Out("CCDDA::GetCurrentPos��MCI_STATUS(MCI_STATUS_POSITION)�Ɏ��s");
		dw = 0;
		return 2;	// CD�������Ă��Ȃ��ꍇ�A���̃G���[
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
		Err.Out("CCDDA::Eject�Ɏ��s");
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
		if (bEndless) {	//	�Ō�̋Ȃ��Đ�
			return Play(GetTrackMax());
		} else {
			return 1;	//	���s
		}
	}
	return Play(n-1);
}

LRESULT CCDDA::PlayNext(bool bEndless){
	int n = GetPlayTrack();
	if (n==GetTrackMax()) {
		if (bEndless) { // �ŏ��̋Ȃ��Đ�
			return Play(1);
		} else {
			return 1;	//	���s
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

	// �Ȃ̒����ȏ�̈ʒu���w�肵���炾��
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

// WM_MCINOTIFY�̒ʒm���󂯎��K�v����
LRESULT CCDDA::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){
	switch(uMsg){
	case MM_MCINOTIFY : {
		// ���̃��b�Z�[�W��CD�̂񂩁H
		if (lParam!=(LPARAM)m_uDeviceID) return 0; // �Ⴄ���I

		// Stop�Œ�~�������Ƃ��́AMCI_NOTIFY_ABORTED
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
