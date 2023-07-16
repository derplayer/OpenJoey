// yaneWinHook.h
//	WindowMessageHooker
//	  Window���b�Z�[�W���t�b�N�����邱�Ƃ�Window�̃T�u�N���X�����A�V�X�g����
//	  (c) yaneurao '99/06/23
//
//		programmed by yaneurao(M.Isozaki)	'99/06/21
//		reprogrammed by yaneurao			'00/02/25

#ifndef __yaneWinHook_h__
#define __yaneWinHook_h__


#include "../Thread/yaneCriticalSection.h"

namespace yaneuraoGameSDK3rd {
namespace Window {

class IWinHook {
/**
	�E�B���h�D���b�Z�[�W���t�b�N����̂ɗ��p���܂��B
	���b�Z�[�W���t�b�N�������N���X�́A���̃N���X����h�����āA���̊֐�

	virtual LRESULT WndProc(HWND,UINT,WPARAM,LPARAM) = 0; 

	���I�[�o�[���C�h���܂��B���̊֐��̓��b�Z�[�W�����������ꍇ�A
	��0��Ԃ��悤�ɃR�[�f�B���O���܂��B

	class CWinHookList ���Q�Ƃ��Ă��������B
*/
public:
	virtual	LRESULT	WndProc(HWND,UINT,WPARAM,LPARAM) = 0;
	virtual ~IWinHook(){}
};

class IWinHookList {
public:
	virtual void	Add(IWinHook*)=0;		//	�������g���t�b�N�ɉ�����
	virtual void	Del(IWinHook*)=0;		//	�������g���t�b�N����O��
	virtual void	Clear()=0;				//	���ׂĂ��N���A����

	//	���b�Z�[�W��Dispatcher
	virtual LRESULT Dispatch(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,WNDPROC pWndProc=NULL)=0;
	virtual LRESULT DispatchDlg(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,WNDPROC pWndProc=NULL)=0;
};

class CWinHookList : public IWinHookList {
/**
	�E�B���h�D���b�Z�[�W���t�b�N����̂ɗ��p���܂��B
	CWinHook����h�����ꂽ�N���X�̃|�C���^��ێ����A�Ǘ����Ă��܂��B

	�P�̃E�B���h�D�N���X�ɑ΂��ĂP��CWinHookList���Ή����܂��B
	�}���`�X���b�h�Ή��ł��B

	���b�Z�[�W�̔z�M�ɂ́AWindow�N���X����A������Ăяo���܂��B
	��������ƁA�����̕ێ����Ă���CWinHook��WndProc�ɈϏ����Ă����܂��B
	�Ϗ������́A�K��Add�������Ԃł���AWndProc����0��Ԃ����̂ɏo��܂�
	WndProc���Ăяo���Â��܂��B�Ǘ����Ă��邷�ׂĂ�CWinHook��WndProc��
	�Ăяo�����ꍇ�́A�f�B�t�H���g�̏����֐��Ƃ���::DefWindowProc��
	�Ăяo���܂��B

	CAppBase�̓��b�Z�[�W�|���v�͕ʃX���b�h�Ȃ̂ŁAHook���J�n�������̏u�Ԃ���A
	WM���b�Z�[�W�����ł���̂ŁA���������I�����Ă��Ȃ��^�C�~���O��
	WM���b�Z�[�W���������Ȃ��悤�ɏ\�����ӂ��Ă��������B

	���b�Z�[�W�|���v��ʃX���b�h�ōs�Ȃ��ꍇ�AHook���J�n�������̏u�Ԃ���A
	WM���b�Z�[�W�����ł���̂ŁA���������I�����Ă��Ȃ��^�C�~���O��
	WM���b�Z�[�W���������Ȃ��悤�ɏ\�����ӂ��Ă��������B

*/
public:
	virtual void	Add(IWinHook*);		//	�������g���t�b�N�ɉ�����
	virtual void	Del(IWinHook*);		//	�������g���t�b�N����O��
	virtual void	Clear();			//	���ׂĂ��N���A����

	///	���b�Z�[�W��Dispatcher
	///	��҂́ADialog�p�ŁA�Ō��DefWindowProc���Ăяo���Ȃ�
	virtual LRESULT Dispatch(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,WNDPROC pWndProc=NULL);
	virtual LRESULT DispatchDlg(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,WNDPROC pWndProc=NULL);

protected:
	typedef list<IWinHook*> hooklist;
	hooklist			m_HookPtrList;	//	�t�b�N���X�g
	CCriticalSection	m_oCriticalSection;
	CCriticalSection*	GetCriticalSection() { return& m_oCriticalSection;}
};

class CMessagePooler : public IWinHook {
/**
		�{�^�����͉���������񂪗���̂ŁA����̃��b�Z�[�W��
		hook����`�ŏ����擾����

		��)
		�{�^���Ȃ��

			uMsg = WM_COMMAND , wParam = nEditControl
		������A���̂悤��
		���O��uMsg��wParam���y�A�ɂ��ă��b�Z�[�Wdispatcher��
		�o�^���Ă����A���Ƃŉ�̓L���[�̃f�[�^�𒲂ׂ�Ɨǂ�

	������s�Ȃ����߂̃w���p�N���X���ACMessagePooler�ł���B
	uMsg��wParam�̑΂�o�^���Ă����΁A���̃��b�Z�[�W�������
	Pool���Ă����Ă����B(���ƂŒ��ׂĎ��o�����Ƃ��o����)

	//	���b�Z�[�W�̂����Â����̂͏㏑�������(^^;
*/
public:
	struct CInfo {
	bool	isPool() const { return bPool; }
	/// ���b�Z�[�W��pool����Ă��邩?

	void	reset() { bPool = false; }
	///	���b�Z�[�W��reset����(���b�Z�[�W��pool����Ă��Ȃ���Ԃɂ���)

	void	init() {
		bPool = false;
		lParam=uMsg=wParam=0;
		bWparamEnable=bLparamEnable = false;
	}
	///	���̍\���̂�����������(hook���ׂ����b�Z�[�W�������������)

	bool	isEmpty() const {
		return uMsg==0 &&
			(!bWparamEnable||wParam==0) &&
			(!bLparamEnable||lParam==0);
	}
	///	���̍\���̂ɂ͂܂�hook���ׂ����b�Z�[�W���o�^����Ă��Ȃ�?

	WPARAM	getWParam() { return wParam; }
	void	setWParam(WPARAM w) { wParam = w;}
	///	pool����Ă��郁�b�Z�[�W��wParam��ݒ�/�擾����

	LPARAM	getLParam() { return lParam; }
	void	setLParam(LPARAM l) { lParam = l;}
	///	pool����Ă��郁�b�Z�[�W��lParam��ݒ�/�擾����

	void	pool(WPARAM w,LPARAM l){
		if(!bWparamEnable) wParam=w;
		if(!bLparamEnable) lParam=l;
		bPool = true;
	}
	///	bWparamEnable��false�Ȃ�΁AwParam��w����荞��
	/// bLparamEnable��false�Ȃ�΁AlParam��l����荞��
	///	bPool��true�ɂȂ�̂�isPool��true��Ԃ��悤�ɂȂ�
	
	void	setMsg(UINT u,WPARAM w,bool bWEnable,LPARAM l,bool bLEnable)
	{ uMsg=w; wParam = w; lParam=l; bWparamEnable=bWEnable; bLparamEnable=bLEnable; }
	///	pool���郁�b�Z�[�W��o�^
	/**
		WEnable��true���ƁA�o�^����WPARAM��isMsg�Ŕ�r����Ƃ��ɔ�r�ΏۂƂ���̈Ӗ�
		LEnable��true���ƁA�o�^����LPARAM��isMsg�Ŕ�r����Ƃ��ɔ�r�ΏۂƂ���̈Ӗ�
	*/

	bool	isMsg(UINT u,WPARAM w,LPARAM l)
	{ return uMsg==w && (!bWparamEnable || wParam== w) && (!bLparamEnable || lParam== l);}
	///	pool���ׂ����b�Z�[�W�ƈ�v������true

	CInfo():bPool(false),lParam(0),uMsg(0),wParam(0),bWparamEnable(false),bLparamEnable(false) {}

	protected:
	UINT	uMsg;	//	����
	WPARAM wParam;	//	uMsg,wParam�̑΂�pool����
	LPARAM lParam;
	bool bPool;
	bool bWparamEnable;	//	��v��������ɁAWPARAM�͓���̂��H
	bool bLparamEnable;	//	��v��������ɁALPARAM�͓���̂��H
	};

	int		SetPoolData(UINT uMsg,WPARAM wParam,bool bWEnable,
		LPARAM lParam,bool bLEnable);
	/**
		�߂�l�͎��ʎq�B�����GetPool�̈����Ƃ��ēn���ď��(CInfo)���擾����
		����CInfo�́AuMsg,wParam�̑΂�����ƁAisPool()��true�ɂ���B
		�����ɂ��̂Ƃ���lParam��ۑ�����B(CInfo�͂��Ƃ��炫�����b�Z�[�W�ŏ㏑�������)
	*/

	void	ClearPoolData(UINT uMsg,WPARAM wParam,LPARAM lParam);
	/**
		Pool�f�[�^�̂Ȃ�����AuMsg,wParam,lParam�Ŏw�肳�ꂽ���̂��폜����
		���̎��ʎq�ɂ͉e���͂Ȃ�
		SetPoolData�Őݒ肷��Ƃ��ɁAbWEnable��false�ɂ��Ă�����wParam��
		��r�Ώۂɂ͂Ȃ�Ȃ��B(����n���Ă��ǂ�)
		SetPoolData�Őݒ肷��Ƃ��ɁAbLEnable��false�ɂ��Ă�����lParam��
		��r�Ώۂɂ͂Ȃ�Ȃ��B(����n���Ă��ǂ�)
	*/

	CInfo*	GetInfo(int nID) { return& m_aInfo[nID]; }
	/**
		SetPoolData�̖߂�l��ID�Ƃ��Ďw�肵�āA���̏����擾����
	*/

protected:
	// override from CWinHook
	virtual LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	vector<CInfo> m_aInfo;
};

} // end of namespace Window
} // end of namespace yaneuraoGameSDK3rd

#endif
