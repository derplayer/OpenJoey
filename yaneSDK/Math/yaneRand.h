//
//	Mersenne Twister法による高精度乱数発生
//	　2^19937-1という非常に長い周期性を持った乱数を高速に生成する！
//

#ifndef __yaneRand_h__
#define __yaneRand_h__


#include "../Auxiliary/yaneSerialize.h"

namespace yaneuraoGameSDK3rd {
namespace Math {

class CRand : public IArchive { // このクラスは、シリアライズ対応！
/**
	Mersenne Twister法による高精度の乱数発生ルーチンです。
	2^19937-1という天文学的な大きさの周期性を持った乱数を高速に生成します。

	class IArchive 派生クラスなので、任意のタイミングで乱数の種を
	そのまま保存しておき、同じ系列の乱数を再度発生させることも出来ます。
*/
public:
	///	乱数の取得。前者はDWORD全域の乱数を取得。
	///	後者は、0～n-1までの乱数を取得。
	///	n==0の場合は0が返る(このほうが使いやすいので)
	DWORD	Get();					//	乱数の取得
	DWORD	Get(DWORD n) {			//	0～n-1の乱数の取得
		if (n==0) { 
/**
#ifdef USE_EXCEPTION
			throw	CRuntimeException();
#else
			return n;
#endif
*/
			return 0;
		}
		return Get() % n;
	}

	void	SetSeed(DWORD dwSeed);
	/**
		乱数の種を設定します。設定された種に従って、乱数は生成されていきます。
		必ず一度は呼び出す必要があります。
		呼び出さないときは、
			SetSeed(4357);
		が、一番最初の乱数取得のときに実行されます。
	*/

	void	Randomize()
	/**
		乱数の種として、現在時刻を与えます。
		要するに、再現性の無い乱数が出来ます。
		SetSeed(GetTickCount())とやっているので、
		SetSeedを呼び出す必要はありません。
	*/
		{	SetSeed(::GetTickCount());	}

	CRand() { m_nMti = 624+1; } /* means m_dwMt is not initialized */
	CRand(DWORD dwSeed) { SetSeed(dwSeed); }
	/**
		コンストラクタは、２種類あり、パラメータ無しのほうは、
		乱数の初期化を行ないません。パラメータ有りのほうは、
		乱数の種を引数として取り、それに基づいた乱数を生成します。
		（内部的にSetSeedを行なうということです。
		よって、SetSeedをこのあと行なう必要はありません）
		前回と同じ乱数系列を再現したい場合などにこれを使います。
	*/

	virtual ~CRand() {}

protected:
	DWORD	m_dwMt[624];	// the array for the state vector
	int		m_nMti;			// initialization counter

protected:
	//	override from IArchive
	virtual void Serialize(ISerialize& s){
		s.Store(ArraySerialize(m_dwMt));	//	メンバ変数まるごと、
		s << m_nMti;						//	シリアライズするのだ！
	}
};

} // end of namespace Math
} // end of namespace yaneuraoGameSDK3rd

#endif
