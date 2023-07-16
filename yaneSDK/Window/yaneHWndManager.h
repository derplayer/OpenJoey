// yaneHWndManager.h : HWND manager
//		programmed by yaneurao(M.Isozaki) '02/03/08
//
/**
	���̃N���X�AThreadLocal�̃T���v���Ƃ��č���Ă݂����A
	���ۂ� class CAppManager ��ʂ���HWND�͎擾�ł���̂ŁA
	����Ȃ��i�΁j
*/

#ifndef __yaneHWndManager_h__
#define __yaneHWndManager_h__


namespace yaneuraoGameSDK3rd {
namespace Window {

class IHWndManager {
public:
	virtual HWND GetHWnd()=0;				/// ���̃X���b�h�ɑΉ�����HWND�̎擾
	virtual void SetHWnd(HWND hWnd)=0;		///	���̃X���b�h�ɑΉ�����HWND�̐ݒ�
	virtual ~IHWndManager(){}
};

class CHWndManager : public IHWndManager {
/**
	���̃X���b�h�ɑΉ�����HWND�̐ݒ�A�擾�B

		CHWndManager::GetObj()->GetHWnd();
	�̂悤�ɂ��Ďg���܂��B

	�����ƂɃX���b�h�͈قȂ�݌v�ɂ���͂��Ȃ̂ŁA
	�X���b�hID���玩���̃X���b�h�̏������鑋��HWND���킩��͂��ł��B

*/
public:
	virtual HWND GetHWnd();
	/**
		���̃X���b�h�ɑΉ�����HWND�̎擾
		���݂��Ȃ��Ƃ���NULL���Ԃ�
	*/
	virtual void SetHWnd(HWND hWnd);
	/**
		���̃X���b�h�ɑΉ�����HWND�̐ݒ�
	*/

	static CHWndManager* GetObj() { return m_vHWndManager.get(); }
	///	singleton�ȃ|�C���^�̎擾

protected:
	static singleton<CHWndManager> m_vHWndManager;

	ThreadLocal<HWND>	m_hWnd;
};

} // end of namespace Window
} // end of namespace yaneuraoGameSDK3rd

#endif
