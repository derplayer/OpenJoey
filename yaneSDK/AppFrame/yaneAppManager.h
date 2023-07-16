//	yaneAppManager.h :
//		application management class
//			programmed by yaneurao	 '00/03/03
//			reprogrammed by yaneurao '02/03/16

#ifndef __yaneAppManager_h__
#define __yaneAppManager_h__

#include "../thread/yaneCriticalSection.h"

namespace yaneuraoGameSDK3rd {
namespace Window {
	class IWinHook;
	class IWindow;
}
}

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

class IAppDraw;
class IAppBase;
class IAppFrame;

class CAppManager {
protected:
	struct CAppManageInfo {
		IAppBase*		m_lpAppBase;		//	�ЂƂ̃A�v���N���X������
		IAppFrame*		m_lpAppFrame;		//	�������������̃t���[���N���X������
		IAppDraw*		m_lpAppDraw;		//	�ЂƂ̕`��N���X������
		DWORD			m_dwThreadID;		//	���C���X���b�h��ThreadID
	};
	typedef smart_vector_ptr<CAppManageInfo> CAppManageList;
	CAppManageList m_alpInfo;

	//	�I�����ɃR�[���o�b�N��v�����Ă��郊�X�g(CDbg�Ŏg�p)
	//	�����ɓo�^���Ă����΁Astatic��CAppBase�̃C���X�^���X
	//	�ł����Ă�(~CAppBase���Ăяo����Ă��Ȃ�����{����
	//	�I���̂Ƃ��ɎQ�Ɛ��������͂�����)�A���e�����
	typedef smart_vector_ptr<function_callback> CAppCallBackList;
	CAppCallBackList m_alpCallBack;

public:
	static	void	Add(IAppBase*p){  GetObj()->_Add(p); }
	static	void	Del(IAppBase*p){  GetObj()->_Del(p); }
	static	void	Add(IAppFrame*p){  GetObj()->_Add(p); }
	static	void	Del(IAppFrame*p){  GetObj()->_Del(p); }
	static	void	Add(IAppDraw*p){  GetObj()->_Add(p); }
	static	void	Del(IAppDraw*p){  GetObj()->_Del(p); }

	//	�o�^���Ă��������̂��擾�ł���
	static	IAppBase*	 GetMyApp() { return GetObj()->_GetMyApp(); }
	static	IAppFrame*	 GetMyFrame() { return GetObj()->_GetMyFrame(); }
	static	IAppDraw*	 GetMyDraw() { return GetObj()->_GetMyDraw(); }
	static	IWindow*	 GetMyWindow(); // IAppBase�o�R�Ŏ擾

	///	 �����̑�����A�v���X���b�h�ۗ̕L���Ă���E�B���h�D�Ɋ֘A�t����B
	///	�i�E�B���h�D���b�Z�[�W���R�[���o�b�N�����悤�ɂȂ�j
	static void	Hook(IWinHook*p){ GetObj()->_Hook(p); }
	static void	Unhook(IWinHook*p){ GetObj()->_Unhook(p); }

	///	�����̑�����A�v���X���b�h�ۗ̕L����E�B���h�D�n���h����Ԃ�
	static HWND	GetHWnd(){ return GetObj()->_GetHWnd(); }

	///	�t���X�N���[�����[�h���ǂ�����Ԃ�
	static bool	IsFullScreen(){ return GetObj()->_IsFullScreen(); }

	static	int		GetAppInstanceNum(){ return GetObj()->_GetAppInstanceNum(); }
	static	void	StopAllThread() { GetObj()->_StopAllThread(); }
	
	static	void Inc(){	GetObj()->_Inc(); }
	//	�Q�ƃJ�E���g�̃C���N�������g

	static	void Dec(){	GetObj()->_Dec(); }
	//	�Q�ƃJ�E���g�̃f�N�������g

	static	int GetRef(){ return GetObj()->_GetRef(); }

	static CAppCallBackList* GetCallBackList() { return GetObj()->_GetCallBackList(); }
	static CCriticalSection* GetCriticalSection() { return GetObj()->_GetCriticalSection(); }

	//	����public�ɂ��Ƃ��Ȃ���singleton��CheckInstance�Ő����ł��Ȃ�
	CAppManager():m_nRef(0) {}

protected:
	static singleton <CAppManager> m_obj;
	static CAppManager* GetObj() { return m_obj.get(); }

	///	singleton�o�R�ŃA�N�Z�X���s�Ȃ����߁A��static�ȃo�[�W�������K�v
	void	_Add(IAppBase*);
	void	_Del(IAppBase*);
	void	_Add(IAppFrame*);
	void	_Del(IAppFrame*);
	void	_Add(IAppDraw*);
	void	_Del(IAppDraw*);
	IAppBase*	 _GetMyApp();
	IAppFrame*	 _GetMyFrame();
	IAppDraw*	 _GetMyDraw();
	void	_Hook(IWinHook*p);
	void	_Unhook(IWinHook*p);
	HWND	_GetHWnd();
	bool	_IsFullScreen();
	int		_GetAppInstanceNum();
	void	_StopAllThread();
	void _Inc();
	void _Dec();
	int _GetRef();
	CAppCallBackList* _GetCallBackList() { return& m_alpCallBack; }
	CCriticalSection* _GetCriticalSection() { return& m_oCriticalSection; }

private:
	CCriticalSection m_oCriticalSection;
	int m_nRef;	//	�Q�ƃJ�E���g
};

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

#endif
