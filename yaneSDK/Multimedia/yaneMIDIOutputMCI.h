// yaneMIDIOutputMCI.h
//
//		MIDI出力を、MCIを使って行なう実装例。
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
	MCI(Media Control Interface)によるMIDI再生。

	class CMIDIOutputDM	 も参照のこと。
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
	virtual bool IsLoopPlay() const { return m_bLoopPlay; }// 追加
	virtual LRESULT SetLoopPlay(bool bLoop);
	virtual LONG	GetCurrentPos() const;
	virtual LONG	SetCurrentPos(LONG lPos);
	virtual LONG	GetLength() const;	// [ms]
	virtual LRESULT SetVolume(LONG volume) {return -1;}	// not implement
	virtual LONG	GetVolume() const {return 0;}		// not implement
	string	GetFileName() const { return m_File.GetName(); }
	virtual	int	GetType() const { return 1; } // type id of MIDIOutput by MCI

	static	bool CanUseMCIMIDI();
	///	MCIによるMIDIは使える状況なのか？

protected:
	LRESULT	InnerOpen();	//	MCIをオープンする

	CFile	m_File;
	DWORD	m_dwPosition;
	DWORD	m_wDeviceID;		//	WORDとして使うギミック
	int		m_bPaused;
	LONG	m_lLength;

	LRESULT	LoopPlay();		//	LoopPlay用
	bool	m_bLoopPlay;	//	Loop再生するのか？
	bool	m_bNowPlay;		//	現在再生中か

	// override from IWinHook
	LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif

