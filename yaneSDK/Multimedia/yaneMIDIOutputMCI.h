// yaneMIDIOutputMCI.h
//
//		MIDI�o�͂��AMCI���g���čs�Ȃ�������B
//

#ifndef __yaneMIDIOutputMCI_h__
#define __yaneMIDIOutputMCI_h__


#include "yaneSound.h"
#include "../Auxiliary/yaneFile.h"
#include "../Window/yaneWinHook.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class CMIDIOutputMCI : public ISound, public IWinHook {
/**
	MCI(Media Control Interface)�ɂ��MIDI�Đ��B

	class CMIDIOutputDM	 ���Q�Ƃ̂��ƁB
*/
public:
	CMIDIOutputMCI();
	virtual ~CMIDIOutputMCI();

	virtual LRESULT Open(const string &pFileName);
	virtual LRESULT Close();
	virtual LRESULT Play();
	virtual LRESULT Replay();
	virtual LRESULT Stop();
	virtual LRESULT Pause(){ return Stop(); }
	virtual bool IsPlay() const;
	virtual bool IsLoopPlay() const { return m_bLoopPlay; }// �ǉ�
	virtual LRESULT SetLoopPlay(bool bLoop);
	virtual LONG	GetCurrentPos() const;
	virtual LONG	SetCurrentPos(LONG lPos);
	virtual LONG	GetLength() const;	// [ms]
	virtual LRESULT SetVolume(LONG volume) {return -1;}	// not implement
	virtual LONG	GetVolume() const {return 0;}		// not implement
	string	GetFileName() const { return m_File.GetName(); }
	virtual	int	GetType() const { return 1; } // type id of MIDIOutput by MCI

	static	bool CanUseMCIMIDI();
	///	MCI�ɂ��MIDI�͎g����󋵂Ȃ̂��H

protected:
	LRESULT	InnerOpen();	//	MCI���I�[�v������

	CFile	m_File;
	DWORD	m_dwPosition;
	DWORD	m_wDeviceID;		//	WORD�Ƃ��Ďg���M�~�b�N
	int		m_bPaused;
	LONG	m_lLength;

	LRESULT	LoopPlay();		//	LoopPlay�p
	bool	m_bLoopPlay;	//	Loop�Đ�����̂��H
	bool	m_bNowPlay;		//	���ݍĐ�����

	// override from IWinHook
	LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif

