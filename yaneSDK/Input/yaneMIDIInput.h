// yaneMIDIInput.h
//	 This is a MIDI input wrapper.
//		programmed by yaneurao(M.Isozaki) '99/08/26
//		modified by yaneurao '00/03/12

#ifndef __yaneMIDIInput_h__
#define __yaneMIDIInput_h__


#include "yaneKeyBase.h"

#define MAX_MIDI_CH 8		//	8�`�����l���ŏ\���H

namespace yaneuraoGameSDK3rd {
namespace Input {

class CMIDIInput : public IKeyBase {
public:
	//	from CKeyBase
	LRESULT GetKeyState(); // 256�o�C�g��Ԃ�ǂݍ���
	BYTE	GetVelocity(int key)const { return m_byKeyBuffer[m_nKeyBufNo][key]; }

	CMIDIInput();
	virtual ~CMIDIInput();

	//////////////////////////////////////////////////////////////////
protected:
	//	MIDI���̓R�[���o�b�N�֐�
	static void CALLBACK MidiInProc(HMIDIIN hMidiIn, WORD wMsg, DWORD dwInstance,
						   DWORD dwParam1, DWORD dwParam2);

	HMIDIIN hMidiIn[MAX_MIDI_CH];
	int		m_nNumDevices;
	BYTE	m_byKeyBuffer2[256];	//	real time key buffer for MidiInProc
	bool	m_bSuccessInit;

private:
	LRESULT Initialize();
	LRESULT Terminate();
};

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd

#endif
