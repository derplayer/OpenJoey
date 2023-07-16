// MIDI input Wrapper

#include "stdafx.h"
#include "yaneMIDIInput.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

//////////////////////////////////////////////////////////////////////////////

CMIDIInput::CMIDIInput() { Initialize(); }
CMIDIInput::~CMIDIInput() { Terminate(); }

LRESULT CMIDIInput::Initialize(void){	// 初期化処理
	m_bSuccessInit	=	false;
	Terminate(); // 終了処理によって初期化（念のため）

	ZERO(m_byKeyBuffer2);

	m_nNumDevices = midiInGetNumDevs();

	if (m_nNumDevices <0) return 1;
	// 取得できんかったらこの関数自体はエラー 

	for(int i=0;(i < m_nNumDevices) && (i<MAX_MIDI_CH);i++){
		midiInOpen((LPHMIDIIN)&hMidiIn[i],i
			,(DWORD)MidiInProc,(DWORD)this,CALLBACK_FUNCTION);
		// どのチャンネルの信号もみんな平等に扱おうよ！
		if (hMidiIn[i])
			midiInStart(hMidiIn[i]);
	}

	m_bSuccessInit	=	true;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
LRESULT CMIDIInput::Terminate(){ // 終了処理
	if (m_bSuccessInit) {
		for(int i=0;(i < m_nNumDevices) && (i<MAX_MIDI_CH);i++){
			midiInStop(hMidiIn[i]);
			midiInReset(hMidiIn[i]);
			midiInClose(hMidiIn[i]);
		}
	}
	return 0; // 正常終了
}
//////////////////////////////////////////////////////////////////////////////

void CALLBACK CMIDIInput::MidiInProc(HMIDIIN hMidiIn, WORD wMsg, DWORD dwInstance,
						   DWORD dwParam1, DWORD dwParam2)
{
// Note OnとOffでm_byKeyBuffer2を変更
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
			// これがNote Offで有り得る
			myclass->m_byKeyBuffer2[Key] = 0;
			break;
		case 0xb0:
			if (Key == 0x43)	  myclass->m_byKeyBuffer2[253] = Velocity; // left pedal
			else if (Key == 0x42) myclass->m_byKeyBuffer2[254] = Velocity; // middle pedal
			else if (Key == 0x40) myclass->m_byKeyBuffer2[255] = Velocity; // right pedal
			break;
		}

	// 他のメッセージは無視しちゃる！
	}
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CMIDIInput::GetKeyState(){

	if (!m_bSuccessInit) return 1;	//	きみぃ、そもそも初期化失敗しとるがに..
	FlipKeyBuffer(m_nKeyBufNo);

	// ２５６バイト現在の状態として取得！
	//	（コピーなので同期を気にする必要は無い）
	::CopyMemory((LPVOID)&(m_byKeyBuffer[m_nKeyBufNo][0]),
			(LPVOID)&(m_byKeyBuffer2[0]),256);

	return 0;
}

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd
