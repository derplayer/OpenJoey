//
//	�A�v���P�[�V�����C�j�V�����C�U
//		yaneuraoGameSDK 2nd�̂��߂̃C�j�V�����C�U�B
//		�C�ɂ���Ȃ���Ύ��R�ɃJ�X�^�}�C�Y���Ďg���Ă��������ȁO�O
//

#ifndef __yaneAppInitializer_h__
#define __yaneAppInitializer_h__

#include "../Window/yaneWinHook.h"

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

class CAppCallbackList {
public:
	void	RegistInitCallback(const smart_ptr<function_callback>& p){
		GetInitCallbackList()->push_back(p);
	}
	void	RegistExitCallback(const smart_ptr<function_callback>& p){
		GetExitCallbackList()->push_back(p);
	}

	void	InitCallback();	//	RegistInitCallback���ꂽ�֐������ԂɌĂяo��
	void	ExitCallback();	//	RegistExitCallback���ꂽ�֐������ԂɌĂяo��

	smart_list_ptr<function_callback> * GetInitCallbackList()
	{ return &m_listInitFunc;}
	smart_list_ptr<function_callback> * GetExitCallbackList()
	{ return &m_listExitFunc;}

protected:
	//	���������R�[���o�b�N�p
	smart_list_ptr<function_callback> m_listInitFunc;
	//	�I�����̃R�[���o�b�N�p
	smart_list_ptr<function_callback> m_listExitFunc;
};

class CAppInitializer {
/**
	�A�v���P�[�V�����������̂��߂̃N���X

	�R���X�g���N�^��WinMain���炻�̃p�����[�^�����̂܂ܓn���ĂˁB

	�񃍁[�J��static�I�u�W�F�N�g�ɂ��R�[���o�b�N�\�񂪏o���܂��B
		RegistInitCallback(�R�[���o�b�N��\�񂷂�֐�);
		RegistExitCallback(�R�[���o�b�N��\�񂷂�֐�);
	�Ƃ���΁A�R���X�g���N�^�ƃf�X�g���N�^�ɂ����āA���̊֐���
	�����I�ɃR�[���o�b�N����܂��B�i�����o�^�j

*/
public:

#ifndef COMPILE_YANE_PLUGIN_DLL
	///	WinMain���炻�̃p�����[�^�����̂܂ܓn���ĂˁB
	CAppInitializer(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow);
#else
	CAppInitializer( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved);
	///	DllMain����̃p�����[�^�����̂܂ܓn���ăl�IDll�̏��������ł����Ⴄ����
	///	GetCmdLine()�ŁA�����œn���ꂽlpReserved���擾�ł��܂���
#endif

	///	���̃f�X�g���N�^��RegistExitCallback���Ă������֐����Ăяo�����
	~CAppInitializer();

	///	singleton��ʂ��Ď擾����
	static HINSTANCE GetInstance()		{ return GetObj()->GetInstance_(); }
	static HINSTANCE GetPrevInstance()	{ return GetObj()->GetPrevInstance_(); }
	static LPSTR	 GetCmdLine()		{ return GetObj()->GetCmdLine_(); }
	static int		 GetCmdShow()		{ return GetObj()->GetCmdShow_(); }

#ifndef COMPILE_YANE_PLUGIN_DLL
	/**
		Dll�쐬���ɂ́A���̊֐��͖����Ȃ̂Œ��ӂ��K�v�I
	*/
	///		Init�Ăяo���̂��ƂɃR�[���o�b�N���ė~�������̂�o�^���Ă���
	static void	RegistInitCallback(const smart_ptr<function_callback>& p)
	{	GetCallbackObj()->RegistInitCallback(p); }
#endif

	///		�I�����ɃR�[���o�b�N���ė~�������̂�o�^���Ă���
	static void	RegistExitCallback(const smart_ptr<function_callback>& p)
	{	GetCallbackObj()->RegistExitCallback(p); }

	static bool	IsMultiThread() { return m_bMultiThread; }
	/**
		�}���`�X���b�h���삩�ǂ�����Ԃ�
		���̃N���X��Init���\�b�h���Ăяo���ȍ~�A�f�X�g���N�^��
		�Ăяo�����܂ł̊ԁAtrue�ɂȂ�
	*/

	//////////////////////////////////////////////////////////////

	///		singleton�I�u�W�F�N�g�̎擾
	static CAppInitializer* GetObj() { return m_vObj; }

	///		WinMain�œn���Ă������p�����[�^�̎擾
	HINSTANCE GetInstance_()		{ return m_hInstance; }
	HINSTANCE GetPrevInstance_()	{ return m_hPrevInstance; }
	LPSTR	 GetCmdLine_()			{ return m_lpCmdLine; }
	int		 GetCmdShow_()			{ return m_nCmdShow; }


protected:

	//	instance parameter...
	HINSTANCE	m_hInstance;
	HINSTANCE	m_hPrevInstance;
	LPSTR		m_lpCmdLine;
	int			m_nCmdShow;

private:
	//	singleton�I�u�W�F�N�g
	static CAppInitializer* m_vObj;

	static singleton<CAppCallbackList>	m_AppCallBackObj;
	static CAppCallbackList* GetCallbackObj()
	{ return m_AppCallBackObj.get(); }

	//	�}���`�X���b�h���삩�ǂ�����Ԃ�
	//	Init�ȍ~��true�ɂȂ�
	static bool m_bMultiThread;
};

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

#endif
