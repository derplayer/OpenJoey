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
	sin,cos�̃e�[�u����񋟂��܂��B������sin,cos�������ł��܂��B
	0�`511�ň���i360�K�j	���ʂ�<<16����ĕԂ�܂��B

	grobal�ϐ�gSinTable��p�ӂ��Ă���̂ŁA�����o�R�ŃA�N�Z�X���ă`��
*/
public:
	///	n:0-511�ň���B���ʂ�<<16����ĕԂ�
	LONG	Cos(int n) const { return m_alCosTable[n & 511]; }
	LONG	Sin(int n) const { return m_alCosTable[(n+384) & 511]; }

	///	n:0-511�ň���BCos(n)*r �̐�������Ԃ�(�������ۂ߂���)
	LONG	Cos(int n,int r) const { 
		LONG	l = (LONG)roundRShift((DWORD64)Cos(n) * r,16);
		return l;
	}
	LONG	Sin(int n,int r) const { 
		LONG	l = (LONG)roundRShift((DWORD64)Sin(n) * r,16);
		return l;
	}

	/**
		������ArcTan��񋟁BC�����atan2�Ɠ����B
		���ʂ�0-65535(1����65536)

		(x,y)��(0,0)�̂Ƃ���CRuntimeException��throw�����
	*/
	WORD	Atan(int x,int y) const
	{
		if (x==0 && y==0)
			throw	CRuntimeException("CSinTable::Atan��(x,y)==(0,0)");
		if (y < 0) return Atan0(-x,-y) + 0x8000;
		return Atan0(x,y);
	}
	/*
		//	(Cos��,Sin��)��atan�����߂Ă݂�:
		for(int i=0;i<512;i++){
			int x = gSinTable.Cos(i,10000);
			int y = gSinTable.Sin(i,10000);
			int at = gSinTable.Atan(x,y);
			at = (at + 0x40) >> 7; // �ۂߏ���
			CHAR buf[32];
			wsprintf(buf,"i:%d x:%d y:%d at:%d",i,x,y,at);

			CDbg().Out(buf);
		}
	*/

	CSinTable();

protected:
	LONG	m_alCosTable[512];	//	cos table
	WORD	m_alAtanTable[256];	//	atan table

	//	y>=0��p��Atan
	WORD	Atan0(int x,int y) const {
		if (x<0) return Atan1(y,-x)+0x4000;
		return Atan1(x,y);
	}

	//	x>=0,y>=0��p��Atan
	WORD	Atan1(int x,int y) const {
		if (x==y) return 0x2000;
		if (y>x) return 0x4000-Atan2(y,x);
		return Atan2(x,y);
	}

	//	x>y , x>=0 , y>=0��p��Atan
	WORD	Atan2(int x,int y) const {
		if (x==0) return 0;
		WORD r = ((DWORD64)y<<8)/x;
		return m_alAtanTable[r];
		//	x>y��藼�ӂ�x�Ŋ�����0<y/x<1�B�����0<=(y<<8)/x<256
	}
};

/// Sin�e�[�u��
extern const CSinTable gSinTable;
/*
	�C���X�^���X���ЂƂp�ӂ��Ă����΁A�ǂ�����ł��g����ɂ偛(���ށ�)o
*/

} // end of namespace Math
} // end of namespace yaneuraoGameSDK3rd

#endif
