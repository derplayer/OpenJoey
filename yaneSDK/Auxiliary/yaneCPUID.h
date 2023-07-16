//
//	CPUの自動判別
//

#ifndef __yaneCPUID_h__
#define __yaneCPUID_h__


namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class CCPUID {
/**
	CPU判別用
	例)
		if (CCPUID::GetObj()->isMMX()) {
			//	MMX用の処理
		}
	というように使う
*/
public:
	int	GetID();
	/**
		CPUIDを返す
		0:未判別(判別エラー)
		1:486以前
		2:MMX,PentiumPro未満
		3:PentiumPro(MMXは非対応)
		-------------------------以下↓は複合される。
			複合された場合、その合計が、この関数の戻り値となる
		4:MMX機能利用可
		8:SSE機能利用可能
		16:SSE2機能利用可能
	*/

	bool	isMMX() { return (GetID()&4) != 0; }
	///	MMX対応のマシンか?

	bool	isSSE() { return (GetID()&8) != 0; }
	///	SSE対応のマシンか?

	bool	isSSE2() { return (GetID()&16) != 0; }
	///	SSE2対応のマシンか?

	static CCPUID* GetObj() { return m_obj.get(); }
	///	このクラスのsingletonオブジェクトを得る

	CCPUID():m_nCPUID(0) {}
protected:
	int	m_nCPUID;
	static singleton<CCPUID> m_obj;
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
