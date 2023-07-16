// yaneHWndManager.h : HWND manager
//		programmed by yaneurao(M.Isozaki) '02/03/08
//
/**
	このクラス、ThreadLocalのサンプルとして作ってみたが、
	実際は class CAppManager を通じてHWNDは取得できるので、
	つかわない（笑）
*/

#ifndef __yaneHWndManager_h__
#define __yaneHWndManager_h__


namespace yaneuraoGameSDK3rd {
namespace Window {

class IHWndManager {
public:
	virtual HWND GetHWnd()=0;				/// そのスレッドに対応するHWNDの取得
	virtual void SetHWnd(HWND hWnd)=0;		///	そのスレッドに対応するHWNDの設定
	virtual ~IHWndManager(){}
};

class CHWndManager : public IHWndManager {
/**
	そのスレッドに対応するHWNDの設定、取得。

		CHWndManager::GetObj()->GetHWnd();
	のようにして使います。

	窓ごとにスレッドは異なる設計にするはずなので、
	スレッドIDから自分のスレッドの所属する窓のHWNDがわかるはずです。

*/
public:
	virtual HWND GetHWnd();
	/**
		そのスレッドに対応するHWNDの取得
		存在しないときはNULLが返る
	*/
	virtual void SetHWnd(HWND hWnd);
	/**
		そのスレッドに対応するHWNDの設定
	*/

	static CHWndManager* GetObj() { return m_vHWndManager.get(); }
	///	singletonなポインタの取得

protected:
	static singleton<CHWndManager> m_vHWndManager;

	ThreadLocal<HWND>	m_hWnd;
};

} // end of namespace Window
} // end of namespace yaneuraoGameSDK3rd

#endif
