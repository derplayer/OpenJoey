//	yaneSinTable.h :
//
//		Sin Table
//

#ifndef __yaneSinTable_h__
#define __yaneSinTable_h__


#include "yaneRound.h"

namespace yaneuraoGameSDK3rd {
namespace Math {

class CSinTable {
/**
	sin,cosのテーブルを提供します。高速なsin,cosが実現できます。
	0～511で一周（360゜）	結果は<<16されて返ります。

	grobal変数gSinTableを用意してあるので、そこ経由でアクセスしてチョ
*/
public:
	///	n:0-511で一周。結果は<<16されて返る
	LONG	Cos(int n) const { return m_alCosTable[n & 511]; }
	LONG	Sin(int n) const { return m_alCosTable[(n+384) & 511]; }

	///	n:0-511で一周。Cos(n)*r の整数部を返す(小数部丸めあり)
	LONG	Cos(int n,int r) const { 
		LONG	l = (LONG)roundRShift((DWORD64)Cos(n) * r,16);
		return l;
	}
	LONG	Sin(int n,int r) const { 
		LONG	l = (LONG)roundRShift((DWORD64)Sin(n) * r,16);
		return l;
	}

	/**
		高速なArcTanを提供。C言語のatan2と同じ。
		結果は0-65535(1周が65536)

		(x,y)が(0,0)のときはCRuntimeExceptionがthrowされる
	*/
	WORD	Atan(int x,int y) const
	{
		if (x==0 && y==0)
			throw	CRuntimeException("CSinTable::Atanで(x,y)==(0,0)");
		if (y < 0) return Atan0(-x,-y) + 0x8000;
		return Atan0(x,y);
	}
	/*
		//	(Cosθ,Sinθ)のatanを求めてみる:
		for(int i=0;i<512;i++){
			int x = gSinTable.Cos(i,10000);
			int y = gSinTable.Sin(i,10000);
			int at = gSinTable.Atan(x,y);
			at = (at + 0x40) >> 7; // 丸め処理
			CHAR buf[32];
			wsprintf(buf,"i:%d x:%d y:%d at:%d",i,x,y,at);

			CDbg().Out(buf);
		}
	*/

	CSinTable();

protected:
	LONG	m_alCosTable[512];	//	cos table
	WORD	m_alAtanTable[256];	//	atan table

	//	y>=0専用のAtan
	WORD	Atan0(int x,int y) const {
		if (x<0) return Atan1(y,-x)+0x4000;
		return Atan1(x,y);
	}

	//	x>=0,y>=0専用のAtan
	WORD	Atan1(int x,int y) const {
		if (x==y) return 0x2000;
		if (y>x) return 0x4000-Atan2(y,x);
		return Atan2(x,y);
	}

	//	x>y , x>=0 , y>=0専用のAtan
	WORD	Atan2(int x,int y) const {
		if (x==0) return 0;
		WORD r = ((DWORD64)y<<8)/x;
		return m_alAtanTable[r];
		//	x>yより両辺をxで割って0<y/x<1。よって0<=(y<<8)/x<256
	}
};

/// Sinテーブル
extern const CSinTable gSinTable;
/*
	インスタンスをひとつ用意しておけば、どこからでも使えるにょ○(≧∇≦)o
*/

} // end of namespace Math
} // end of namespace yaneuraoGameSDK3rd

#endif
