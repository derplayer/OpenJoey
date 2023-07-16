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
	CDDA�̃g���b�N�����i�[���邽�߂̍\����
	class CCDDA �Ŏg���܂�
*/
	DWORD	m_dwSongLength;	//	�Ȃ̒����i���̔z���[0]�͑S�Ȃ̒���)
	DWORD	m_dwSongStart;	//	�Ȃ̊J�n�|�W�V����
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
	CD�Đ��̂��߂̃R���|�[�l���g�B
	�}���`�h���C�u�ł�CD�Đ������T�|�[�g���Ă��܂��B

	Open�����Ƃ�Play�Œ��ڍĐ����邱�Ƃ��o���܂��B
	Open���郁���b�g�́A�Ȑ��E�Ȓ������
	GetTrackInfo()�@���X�V����邱�ƁB

	�b�c����]�����ăX�^���o�C����̂�Play���Ăяo��������
	�Đ�����̎��Ԃ��P�b�قǒZ�k�ł��邱�Ƃł��B

	CCDDA�̃R���X�g���N�^�ɐ�s���ăE�B���h�D���������Ă���K�v������̂ŁA
	class CAppFrame ���Ŏg�p����悤�ɂ��Ă��������B
*/
public:

	LRESULT	Open(const string& strDriveName="");
	///		CD�������B�Ăяo����CD����:p
	///		�h���C�u���̎w����o����B��jOpen("g"); // G Drive

	LRESULT	Close();
	///	CD�g�p�I���BCD�̉�]�͎~�܂�B

	//////////////////////////////////////////////////////////////////

	LRESULT	Stop();			///	�Đ���~
	LRESULT	Pause();		///	Pause
	LRESULT	Replay();		///	Pause�ł��������~�����Ă������̂��ĊJ

	LRESULT Play();			///	������Đ�

	int		GetPlayTrack();	
	///	���ݍĐ����̃g���b�N��Ԃ�

	LRESULT PlayPrev(bool bEndless=true);	///	�O�̃g���b�N���Đ�
	LRESULT PlayNext(bool bEndless=true);	///	���̃g���b�N���Đ�

	LRESULT Play(int);			///	�g���b�N�ԍ���I�����čĐ�
	LRESULT Play(int,int);		///	�Q�g���b�N�Ԃ̍Đ�

	LRESULT	GetCurrentPos(DWORD &dw);
	/**
	���݂̍Đ��|�W�V�����𓾂�[MSF:Multimedia Sound Format]
	MSF�`���Ƃ́Ah:���� m:�� s:�b f:�t���[��(�Z�N�^)�ŕ\�����`���ł��B
	*.cda�ł́A��ʂ���f/s/m/h�̏��ŁA�e�P�o�C�g�ŕ\������Ă܂��B
	*/

	LRESULT PlayPosToPos(DWORD,DWORD,bool bReplay=false);
	/// �Q�|�W�V�����Ԃ̍Đ�
	LRESULT PlayPosToTrack(DWORD dwPos,bool bReplay=false);
	/// �ʒu���w�肵�āA�������炻�̃g���b�N�݂̂��Đ��B
	LRESULT PlayDWToEnd(DWORD dwPos,bool bReplay=false);
	/// �ʒu���w�肵�āA��������I���g���b�N�̍Ō�܂ōĐ�
	/**
		��L�R��Play�֐��ŁAbReplay==true�ɂ���Ƃ��́A
		���[�v�v���C�̂Ƃ��ɁA����SetStartPos�Őݒ肵���ꏊ�ɖ߂�
	*/
	void	SetStartPos(DWORD dw){
		m_nStartPos = dw;
	}

	bool	IsPlay() const;
	///	���t���Ȃ̂��H

	void	SetLoopMode(bool bLoop);
	///	���[�v�ōĐ����邩(default:false)

	int		GetTrackMax();
	///	�Ȑ��̎擾(Open���Ă��邱�Ƃ��K�v)

	LRESULT	Eject(bool bEject);
	///	CD��Eject���s�Ȃ��B

	bool	IsOpen() const { return m_bOpen;}
	///	CD��open���Ă��邩

	virtual LRESULT ConvertToMealTime(DWORD dwPos,int& nTrack,int&nHour,int&nMin,int&nSec,int&nMS);
	virtual LRESULT ConvertFromMealTime(DWORD& dwPos,int nTrack,int nHour,int nMin,int nSec,int nMS);
	/**
		class CSoundBase ��ConvertToMealTime/ConvertFromMealTime��
		�����d�l�̂��́B�������A�����ł�track���w��ł��邱�ƁA
		�����ă|�W�V�����Ƃ��Ďg����̂́A[ms]�P�ʂł͂Ȃ�[MSF]��
		���邱�Ƃ��Ⴄ�B

		CCDDA::GetCurrentPos�Ŏ擾�����l�����̂܂܂��̊֐��ŕϊ�������A
		�g���b�N�ԍ��{�����b�т���ϊ������l��Play(DWORD,DWORD)�ŁA
		���̂܂܂��̈ʒu����Đ��ł���B
	*/

	//////////////////////////////////////////////////////////////////

	///	�g���b�N���̎擾
	vector <CDDAInfo>*	GetTrackInfo(){ return &m_aTrackInfo; }
	/**
		�S�Ȓ����� GetTrackInfo()[0].m_dwSongLength
		GetTrackInfo()[1].m_dwSongLength
			// 1�g���b�N�ڂ̒���
		GetTrackInfo()[1].m_dwSongStart
			// 1�g���b�N�ڂ̃X�^�[�g�|�W�V����[msf]
		GetTrackInfo()->rbegin()->m_dwSongStart
			//	�Ō�̃g���b�N�̏I���|�W�V����[msf]
	*/

	//////////////////////////////////////////////////////////////////

	CCDDA();
	virtual ~CCDDA();

//////////////////////////////////////////////////////////////////////////////
protected:
	HWND	m_hWnd;			//	MM_MCINOTIFY�ʒm������
	UINT	m_uDeviceID;	//	CD�̃f�o�C�XID
	bool	m_bOpen;		//	�I�[�v�������H(���̊ԁA�b�c�͉����ςȂ�)

	bool	m_bLoopPlay;	//	Loop�Đ�����̂��H
	DWORD	m_nStartPos;	//	Loop�Đ��̊J�n�ʒu
	DWORD	m_nEndPos;		//	Loop�Đ��̏I���ʒu
	LRESULT	LoopPlay();		//	LoopPlay�p

	DWORD	m_nPausePos;	//	Pause�|�W�V����
	int		m_bPaused;		//	Pause���Ȃ̂��H
	bool	m_bNowPlay;		//	����Play���Ȃ̂��H
	string	m_strDriveName;	//	�h���C�u��

	//	�g���b�N���
	vector <CDDAInfo>	m_aTrackInfo;

	LONG ConvertToSecond(DWORD dwPos);	// [MSF] -> [Sec]
	
	LRESULT PlayDW2(DWORD,DWORD,bool bReplay=false); // �Q�|�W�V�����Ԃ̍Đ�

	// override from CWinHook
	LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif
