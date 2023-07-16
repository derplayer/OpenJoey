#include "stdafx.h"
#include "yaneAppFrame.h"
#include "../Timer/yaneTimer.h"

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

//	�R���X�g���N�^�`�f�X�g���N�^�ŁACAppManager�ɓo�^���Ă���
IAppFrame::IAppFrame(void) {
	CAppManager::Add(this);
}

IAppFrame::~IAppFrame(){
	CAppManager::Del(this);
}

void	CAppFrame::MesSleep(int nTime) {
	CTimer t;
	while (IsThreadValid() && (t.Get()<(DWORD)nTime))
//		::Sleep(10);	//	����҂��Ƃ��O�O�G
		GetMyApp()->GetWorkerThread()->sleep(10);
		//	�������ő҂��Ă�����StopThread�Œʒm�������Ƃ���
		//	�����ɏI���ł���
		/**
			���̃X���b�h�́ACAppBase���瑖���Ă���
			���[�J�[�X���b�h�ɈႢ�Ȃ��D�D
		*/
}

void	CAppFrame::Start()
{
	m_lpMyApp = CAppManager::GetMyApp();
	MainThread();
}

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd
