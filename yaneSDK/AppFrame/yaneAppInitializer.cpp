#include "stdafx.h"

#include "yaneAppInitializer.h"
#include "../Thread/yaneThread.h"
	//	�I������Ƃ��ɂ��ׂẴX���b�h�̏I�����m�F���Ă����K�v������̂�
	//	������CThreadManager�ɑ΂��ăA�N�Z�X����K�v������

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

//	static member..
CAppInitializer* CAppInitializer::m_vObj=NULL;
singleton<CAppCallbackList>	CAppInitializer::m_AppCallBackObj;
bool CAppInitializer::m_bMultiThread = false;

#ifndef COMPILE_YANE_PLUGIN_DLL
//	WinMain���炻�̃p�����[�^�����̂܂ܓn���ĂˁB
CAppInitializer::CAppInitializer(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){
	//	�p�����[�^�����̂܂ܕۑ�:p
	m_hInstance		=	hInstance;
	m_hPrevInstance	=	hPrevInstance;
	m_lpCmdLine		=	lpCmdLine;
	m_nCmdShow		=	nCmdShow;

	m_vObj = this; // �O������static�ɃA�N�Z�X�ł���悤�ɂ��Ă���

	m_bMultiThread = true;
	//	�����ȍ~�́A�}���`�X���b�h����B
	//	���̕ϐ���latch�ϐ��Ȃ̂ŁA����ŗǂ�

	IThread::setThread(NULL);

	GetCallbackObj()->InitCallback();
}

#else

//	DllMain����̃p�����[�^�����̂܂ܓn���ăl�IDll�̏��������ł����Ⴄ����
CAppInitializer::CAppInitializer( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved){
	//	�p�����[�^�����̂܂ܕۑ�:p
	m_hInstance		=	(HINSTANCE)hModule;
	m_hPrevInstance	=	(HINSTANCE)hModule;
	m_lpCmdLine		=	(LPSTR)lpReserved;
	//	������new���Ă�ok
	m_vObj = this; // �O������static�ɃA�N�Z�X�ł���悤�ɂ��Ă���
}

#endif

CAppInitializer::~CAppInitializer(){
#ifndef COMPILE_YANE_PLUGIN_DLL
	CThreadManager::GetObj()->WaitAllThreadEnded();
	//	���ׂẴX���b�h�̏I����҂�
#endif
	//	���̒���́A�V���O���X���b�h���[�h�ɖ߂�
	m_bMultiThread	= false;
	GetCallbackObj()->ExitCallback();
}

void	CAppCallbackList::InitCallback(){
	//	RegistInitCallback���ꂽ�֐������ԂɌĂяo��

	//	���̂Ƃ��́A�܂��V���O���X���b�h�Ȃ̂�
	//	CriticalSection���g�p����K�v�͖���

	//	�R�[���o�b�N�\�񂳂ꂽ����Ăяo��
	GetInitCallbackList()->for_each(function_callback::run);
}

void	CAppCallbackList::ExitCallback(){
	//	RegistExitCallback���ꂽ�֐������ԂɌĂяo��

	//	���̂Ƃ��́A���ɃV���O���X���b�h�Ȃ̂�
	//	CriticalSection���g�p����K�v�͖���

	//	�R�[���o�b�N�\�񂳂ꂽ����Ăяo��
	GetExitCallbackList()->for_each(function_callback::run);
}

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

//////////////////////////////////////////////////////////////////////////////

