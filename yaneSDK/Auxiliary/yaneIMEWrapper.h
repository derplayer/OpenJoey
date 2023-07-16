// yaneIMEWrapper.h : easy ime wrapper
//		programmed by yaneurao(M.Isozaki) '02/03/08
//

#ifndef __yaneIMEWrapper_h__
#define __yaneIMEWrapper_h__


#include <imm.h>	// for ImmAssociateContext

#pragma comment(lib,"imm32.lib")

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class IIMEWrapper {
public:
	virtual void Show()=0;		/// IMEの表示
	virtual void Hide()=0;		///	IMEの非表示
	virtual ~IIMEWrapper(){}
};

class CIMEWrapper : public IIMEWrapper {
/**

	IMEの表示・非表示を制御します。
	Showを連続で呼び出しても、
	一度Hideを呼び出せばIMEは消えます。
	（参照カウントは取っていません）

		CIMEWrapper::GetObj()->Show();
		CIMEWrapper::GetObj()->Hide();
	のようにして使います。

	注意：
	ウィンドゥクラス側で、フルスクリーンモードになったときには、
	自動的にオフになるようにしています。
	これは、DirectDrawが、IME(やGDI)を無視して転送してしまうためです。

	一応、Clipperを仕掛けておけばそこは回避できるのですが、
	フルスクリーン時にFlipを行なう設定だと、IMEはちらつきますし、
	ゲーム中にIMEを使うことは無いだろうということで、こういう仕様にします。
	もし気に入らない場合はその２点に気をつけながら変更するようにしてください。

	また、IMEの表示・非表示はWindowと関連して行なわれるため、
	Windowを作る前にHideしても意味がないので注意してください。
*/
public:
	virtual void Show();	/// IMEの表示
	virtual void Hide();	///	IMEの非表示

	CIMEWrapper() { m_nStat=0; }
	virtual ~CIMEWrapper() {
	//	if (m_nStat==2) Show();	//	消しているならば出現させる
	//	↑singletonのデストラクタからsingletonの呼び出しは禁止
	}

	static CIMEWrapper* GetObj() { return m_vIMEWrapper.get(); }
	/**
		singletonなポインタの取得

		ただし、proxy_ptrなので、非ローカルなstaticオブジェクトの
		コンストラクタから参照してはいけない
	*/

protected:
	static singleton<CIMEWrapper> m_vIMEWrapper;

	int m_nStat;
	//	現在の状態 0:不定 1:Show 2:Hide
	//	⇒連続的な呼び出しを防止する
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
