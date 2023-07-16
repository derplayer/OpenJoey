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
		//		NTでは、どうも失敗する．．．。

		// （WindowsNT4.0+ServicePack3) IDirectInputのGUIDが違う。
		//	 これは、Microsoftのチョンボと思われる。
		//	DEFINE_GUID(CLSID_DirectInput,0x25E609E0,0xB259,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
		//	↑ではなく、↓になっているのだ。
		//	DEFINE_GUID(CLSID_DirectInputNT,0x25E609E0,0xB259,0x11CF,0xBF,0xC7,0x44,0x45,0x35,0x54,0x00,0x00);
		//	　よく見ると、↑この最後から４つ目の値が違うのだ！！
		//	DEFINE_GUID(IID_IDirectInputA,  0x89521360,0xAA8A,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
		/*
		const GUID __CLSID_DirectInputNT
			= {0x25E609E0,0xB259,0x11CF,{0xBF,0xC7,0x44,0x45,0x35,0x54,0x00,0x00}};
//		const GUID __IID_IDirectInputA
		if (GetDirectInput()->CreateInstance(
			__CLSID_DirectInputNT,__IID_IDirectInputA)!=0){
			Err.Out("CDirectInput::CDirectInputの初期化NTでは失敗するんだよな..");
			m_nStatus = 3;
			return ;
		}
		*/

		//	↑直接GUIDを指定しても初期化できないようだ．．？？
		//	仕方が無いので、最後の手段、LoadLibraryを行なう．．
		if (GetLib()->Load("dinput.dll")!=0){
			Err.Out("CDirectInput::CDirectInputでLoadLibraryに失敗");
			m_nStatus = 3;
			//	じぇんじぇんダメ
			return ;
		}
		typedef LRESULT (WINAPI *dica_proc)(HINSTANCE hinst,DWORD dwVersion,LPDIRECTINPUTA *ppDI,
									LPUNKNOWN punkOuter);
		dica_proc dica = (dica_proc)GetLib()->GetProc("DirectInputCreateA");
		if (dica == NULL) {
			Err.Out("CDirectInput::CDirectInputでGetProcに失敗");
			m_nStatus = 3;
			return ;
		}
		//	LoadLibraryしている以上、0x0500で成功することは有り得ない
		if (dica(CAppInitializer::GetInstance(),0x0300, &m_lpDirectInput, NULL)!=DI_OK){
			Err.Out("CDirectInput::CDirectInputでDirectInputCreateに失敗");
			m_nStatus = 3;
			return ;
		}
		//	NTだけど初期化成功！
		m_nStatus = 2;
	} else {
		m_lpDirectInput = GetDirectInput()->get();
	}
	if(m_lpDirectInput==NULL) {
		Err.Out("CDirectInput::CDirectInputでDirectInputInterfaceが得られない");
		// DirectX3は入っとらんのか？
		m_nStatus = 4;
		return ;
	}

	HINSTANCE hInst = CAppInitializer::GetInstance();
	if(m_lpDirectInput->Initialize(hInst,0x500)!=DI_OK){
		if(m_lpDirectInput->Initialize(hInst,0x300)!=DI_OK){
			Err.Out("CDirectInput::CKeyInputでDirectInputの初期化に失敗");
			m_nStatus = 5;
			return ;
		}

		if (m_nStatus==0){	//	2のときは、NTでの初期化プロセスとみなす
			m_nStatus = 1;	//	DirectX3
		}
	} else {
		m_nStatus = 0;
	}
}

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd
