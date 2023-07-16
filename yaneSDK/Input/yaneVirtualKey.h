//	yaneVirtualKey.h :
//		Virtual Key Class for CKeyBase
//				programmed by yaneurao(M.Isozaki) '99/11/13
//

#ifndef __yaneVirtualKey_h__
#define __yaneVirtualKey_h__


#include "yaneKeyBase.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

const int g_VIRTUAL_KEY_MAX=88;	//	仮想キーの最大（88鍵:p）

class CVirtualKey : public IKeyBase {
/**
	統合キー入力クラス。CKeyBaseの派生クラスを利用して、
	それらをひとまとめにして管理できる。

	たとえば、キーボードの↑キーと、テンキーの８キー、
	ジョイスティックの↑入力を、一つの仮想キーとして登録することによって、
	それらのどれか一つが入力されているかを、関数をひとつ呼び出すだけで
	判定できるようになる。

	実際にclass CKey,class CKey2は、
	このクラスの応用事例なので、そちらも参照すること。

	全体的な流れは、キーデバイスの登録→仮想キーの設定としておいて、
	InputしたのちIsVKeyPress/IsVKeyPushで判定です。
*/
public:
	CVirtualKey();
	virtual ~CVirtualKey();

	///	キーデバイスの登録
	/**	キーデバイスとは、CKeyBaseの派生クラスのインスタンス。
		具体的にはCDirectInput,CMIDIInput,CJoyStickのインスタンスが
		挙げられる。入力したいキーデバイスをまず登録する。
		そしてInputを呼び出すことによって、それらのGetStateが呼び出される。
	*/
	void	ClearKeyDevice();			///	デバイスのクリア
	void	AddDevice(const smart_ptr<IKeyBase>&);		///	デバイスの追加
	void	RemoveDevice(const smart_ptr<IKeyBase>&);	///	デバイスの削除
	smart_ptr<IKeyBase> GetDevice(int n);	///	ｎ番目に登録したデバイスの取得
	/**
		この関数を使えばｎ番目（０から始まる）のAddDeviceしたデバイスを
		取得できる。）
	*/

	///	仮想キーの追加・削除
	void	ClearKeyList();							///	仮想キーのリセット
	void	AddKey(int vkey,int nDeviceNo,int key);		///	仮想キーの追加
	void	RemoveKey(int vkey,int nDeviceNo,int key);	///	仮想キーの削除
	/**
		vkeyは、仮想キー番号。これは0～VIRTUAL_KEY_MAX-1
		(現在88とdefineされている)番まで登録可能。
		キーデバイスnDeviceNoは、GetDeviceで指定するナンバーと同じもの。
		keyは、そのキーデバイスのkeyナンバー。
	*/

	///	----	overriden from IKeyBase	 ------

	virtual LRESULT	Input();
	///　登録しておいたデバイスから読みこみ

///	virtual bool	IsKeyPress(int vkey);		///	仮想キーを調べる
///	virtual bool	IsKeyPush(int vkey);		///	仮想キーを調べる
/**
	仮想キーのvkeyが押されているかどうかを調べる。
	IsKeyPressのほうは、現在押されていればtrue。IsKeyPushは、
	前回押されていなくて、今回押されていればtrue。
*/

///	virtual const BYTE*	GetKeyData() const;
	/// 取得したキー情報(BYTE m_byKeyBuffer[256])へのポインタを得る)
	///	直接ポインタを得てぐりぐりする人のために＾＾；

protected:
	class CKeyInfo {
	public:
		int			m_nDeviceNo;
		int			m_nKey;
	};
	typedef smart_vector_ptr<CKeyInfo> CVKeyList;
	CVKeyList	m_VKey[g_VIRTUAL_KEY_MAX];			//	仮想キーリスト

	smart_vector_ptr<IKeyBase>	m_alpDevice;		//	入力キーデバイスリスト
};

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd

#endif
