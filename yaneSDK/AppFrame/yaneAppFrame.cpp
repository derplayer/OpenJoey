#include "stdafx.h"
#include "yaneAppFrame.h"
#include "../Timer/yaneTimer.h"

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

//	コンストラクタ～デストラクタで、CAppManagerに登録しておく
IAppFrame::IAppFrame(void) {
	CAppManager::Add(this);
}

IAppFrame::~IAppFrame(){
	CAppManager::Del(this);
}

void	CAppFrame::MesSleep(int nTime) {
	CTimer t;
	while (IsThreadValid() && (t.Get()<(DWORD)nTime))
//		::Sleep(10);	//	ちょぴっとずつ＾＾；
		GetMyApp()->GetWorkerThread()->sleep(10);
		//	↑こいつで待っておけばStopThreadで通知が来たときに
		//	即座に終了できる
		/**
			このスレッドは、CAppBaseから走っている
			ワーカースレッドに違いない．．
		*/
}

void	CAppFrame::Start()
{
	m_lpMyApp = CAppManager::GetMyApp();
	MainThread();
}

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd
