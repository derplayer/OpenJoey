// DirectInput Wrapper

#include "stdafx.h"
#include "yaneDirectInput.h"
#include "../Auxiliary/yaneCOMBase.h"
#include "../AppFrame/yaneAppManager.h"
#include "../AppFrame/yaneAppInitializer.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

CDirectInput::CDirectInput(){
	m_nStatus = 0;
	if (GetDirectInput()->CreateInstance(
		CLSID_DirectInput,IID_IDirectInput)!=0){
		//		NT�ł́A�ǂ������s����D�D�D�B

		// �iWindowsNT4.0+ServicePack3) IDirectInput��GUID���Ⴄ�B
		//	 ����́AMicrosoft�̃`�����{�Ǝv����B
		//	DEFINE_GUID(CLSID_DirectInput,0x25E609E0,0xB259,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
		//	���ł͂Ȃ��A���ɂȂ��Ă���̂��B
		//	DEFINE_GUID(CLSID_DirectInputNT,0x25E609E0,0xB259,0x11CF,0xBF,0xC7,0x44,0x45,0x35,0x54,0x00,0x00);
		//	�@�悭����ƁA�����̍Ōォ��S�ڂ̒l���Ⴄ�̂��I�I
		//	DEFINE_GUID(IID_IDirectInputA,  0x89521360,0xAA8A,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
		/*
		const GUID __CLSID_DirectInputNT
			= {0x25E609E0,0xB259,0x11CF,{0xBF,0xC7,0x44,0x45,0x35,0x54,0x00,0x00}};
//		const GUID __IID_IDirectInputA
		if (GetDirectInput()->CreateInstance(
			__CLSID_DirectInputNT,__IID_IDirectInputA)!=0){
			Err.Out("CDirectInput::CDirectInput�̏�����NT�ł͎��s����񂾂��..");
			m_nStatus = 3;
			return ;
		}
		*/

		//	������GUID���w�肵�Ă��������ł��Ȃ��悤���D�D�H�H
		//	�d���������̂ŁA�Ō�̎�i�ALoadLibrary���s�Ȃ��D�D
		if (GetLib()->Load("dinput.dll")!=0){
			Err.Out("CDirectInput::CDirectInput��LoadLibrary�Ɏ��s");
			m_nStatus = 3;
			//	�����񂶂���_��
			return ;
		}
		typedef LRESULT (WINAPI *dica_proc)(HINSTANCE hinst,DWORD dwVersion,LPDIRECTINPUTA *ppDI,
									LPUNKNOWN punkOuter);
		dica_proc dica = (dica_proc)GetLib()->GetProc("DirectInputCreateA");
		if (dica == NULL) {
			Err.Out("CDirectInput::CDirectInput��GetProc�Ɏ��s");
			m_nStatus = 3;
			return ;
		}
		//	LoadLibrary���Ă���ȏ�A0x0500�Ő������邱�Ƃ͗L�蓾�Ȃ�
		if (dica(CAppInitializer::GetInstance(),0x0300, &m_lpDirectInput, NULL)!=DI_OK){
			Err.Out("CDirectInput::CDirectInput��DirectInputCreate�Ɏ��s");
			m_nStatus = 3;
			return ;
		}
		//	NT�����Ǐ����������I
		m_nStatus = 2;
	} else {
		m_lpDirectInput = GetDirectInput()->get();
	}
	if(m_lpDirectInput==NULL) {
		Err.Out("CDirectInput::CDirectInput��DirectInputInterface�������Ȃ�");
		// DirectX3�͓����Ƃ��̂��H
		m_nStatus = 4;
		return ;
	}

	HINSTANCE hInst = CAppInitializer::GetInstance();
	if(m_lpDirectInput->Initialize(hInst,0x500)!=DI_OK){
		if(m_lpDirectInput->Initialize(hInst,0x300)!=DI_OK){
			Err.Out("CDirectInput::CKeyInput��DirectInput�̏������Ɏ��s");
			m_nStatus = 5;
			return ;
		}

		if (m_nStatus==0){	//	2�̂Ƃ��́ANT�ł̏������v���Z�X�Ƃ݂Ȃ�
			m_nStatus = 1;	//	DirectX3
		}
	} else {
		m_nStatus = 0;
	}
}

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd
