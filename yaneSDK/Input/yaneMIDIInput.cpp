// MIDI input Wrapper

#include "stdafx.h"
#include "yaneMIDIInput.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

//////////////////////////////////////////////////////////////////////////////

CMIDIInput::CMIDIInput() { Initialize(); }
CMIDIInput::~CMIDIInput() { Terminate(); }

LRESULT CMIDIInput::Initialize(void){	// ����������
	m_bSuccessInit	=	false;
	Terminate(); // �I�������ɂ���ď������i�O�̂��߁j

	ZERO(m_byKeyBuffer2);

	m_nNumDevices = midiInGetNumDevs();

	if (m_nNumDevices <0) return 1;
	// �擾�ł��񂩂����炱�̊֐����̂̓G���[ 

	for(int i=0;(i < m_nNumDevices) && (i<MAX_MIDI_CH);i++){
		midiInOpen((LPHMIDIIN)&hMidiIn[i],i
			,(DWORD)MidiInProc,(DWORD)this,CALLBACK_FUNCTION);
		// �ǂ̃`�����l���̐M�����݂�ȕ����Ɉ�������I
		if (hMidiIn[i])
			midiInStart(hMidiIn[i]);
	}

	m_bSuccessInit	=	true;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
LRESULT CMIDIInput::Terminate(){ // �I������
	if (m_bSuccessInit) {
		for(int i=0;(i < m_nNumDevices) && (i<MAX_MIDI_CH);i++){
			midiInStop(hMidiIn[i]);
			midiInReset(hMidiIn[i]);
			midiInClose(hMidiIn[i]);
		}
	}
	return 0; // ����I��
}
//////////////////////////////////////////////////////////////////////////////

void CALLBACK CMIDIInput::MidiInProc(HMIDIIN hMidiIn, WORD wMsg, DWORD dwInstance,
						   DWORD dwParam1, DWORD dwParam2)
{
// Note On��Off��m_byKeyBuffer2��ύX
	CMIDIInput* myclass = (CMIDIInput*) dwInstance;

	int Status = dwParam1 & 0xf0;			// Status
	int Key = (dwParam1 >> 8) & 0xff;		// data1
	int Velocity = (dwParam1 >> 16) & 0xff; // data2

	switch (wMsg){
	case MIM_DATA:
		switch (Status) {
		case 0x90:
			// Note On or Off
			myclass->m_byKeyBuffer2[Key] = Velocity;
			break;
		case 0x80:
			// ���ꂪNote Off�ŗL�蓾��
			myclass->m_byKeyBuffer2[Key] = 0;
			break;
		case 0xb0:
			if (Key == 0x43)	  myclass->m_byKeyBuffer2[253] = Velocity; // left pedal
			else if (Key == 0x42) myclass->m_byKeyBuffer2[254] = Velocity; // middle pedal
			else if (Key == 0x40) myclass->m_byKeyBuffer2[255] = Velocity; // right pedal
			break;
		}

	// ���̃��b�Z�[�W�͖����������I
	}
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CMIDIInput::GetKeyState(){

	if (!m_bSuccessInit) return 1;	//	���݂��A�����������������s���Ƃ邪��..
	FlipKeyBuffer(m_nKeyBufNo);

	// �Q�T�U�o�C�g���݂̏�ԂƂ��Ď擾�I
	//	�i�R�s�[�Ȃ̂œ������C�ɂ���K�v�͖����j
	::CopyMemory((LPVOID)&(m_byKeyBuffer[m_nKeyBufNo][0]),
			(LPVOID)&(m_byKeyBuffer2[0]),256);

	return 0;
}

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd
