#ifndef __yaneGTL_h__
#define __yaneGTL_h__
//////////////////////////////////////////////////////////////////////////////
//	yaneGTL.h
//		Graphic Template Library
//			for CFastPlane & CDIB32 & CPlane...
//
//			Programmed by yaneurao(M.Isozaki) '01/10/01-'02/04/22
//			Special thanks to Hideo Ikeuchi
//
//		�����l�����ɏ����Â��Ă���A�����ւ�f�J���Ȃ��Ă��܂����B
//		�������ŁA�R���p�C���I�v�V������ /Zm200�Ƃ����悤�ɂ��ăq�[�v���m�ۂ��Ȃ���
//		�����Ȃ��Ȃ��Ă��܂���(��_�G)�
//
//
//		����GTL�́A���Ƃ���yaneSDK2nd��CFastPlane�̂��߂ɊJ���������̂Ȃ̂�
//		���O��CFastPlaneXXX�ɂȂ��Ă��܂��B�i�C�ɂ���Ƃ��Ă���͂��j
//
//////////////////////////////////////////////////////////////////////////////

#include "yaneSurface.h"
#include "../Auxiliary/yaneCPUID.h"

/*
	�����K��
		�P�O�i���̐����Ɠ������A�����ɏ�ʃr�b�g�������B

		XRGB8888
		�Ƃ���΁A��ʂ���X,R,G,B�Ƃ��āA
		���ꂼ�ꂪ8,8,8,8�r�b�g�ł��邱�Ƃ��Ӗ����܂��B
*/

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CFastPlaneBytePal;	// 2:	1�o�C�g�p���b�g(not supported)
class CFastPlaneRGB565;		// 3:	�ʏ�16bit
class CFastPlaneRGB555;		// 4:	�ʏ�15bit
class CFastPlaneRGB888;		// 5:	�ʏ�24bit
class CFastPlaneBGR888;		// 6:		�t�o�[�W����
class CFastPlaneXRGB8888;	// 7:	�ʏ�32bit		//	CDIB32������
class CFastPlaneXBGR8888;	// 8:		�t�o�[�W����
class CFastPlaneARGB4565;	// 10:	Alpha�t��16bit+alpha // RGB565��YGA��ǂݍ��ނƂ��͂���
class CFastPlaneARGB4555;	// 11:	Alpha�t��15bit+alpha // RGB555��YGA��ǂݍ��ނƂ��͂���
	//	�ˁ@�������ʂł͂��邪�A�����S�r�b�g�ɐ������邱�Ƃɂ��A
	//	�e�[�u�������ꂽ�A�����ȃu�����h��񋟂ł���
class CFastPlaneARGB8888;	// 12:	Alpha�t��32bit	//	CDIB32��YGA��ǂݍ��񂾂Ƃ�������
class CFastPlaneABGR8888;	// 13:	Alpha�t��32bit�t��

//	�C�������΃T�|�[�g�O�O�G
class CFastPlaneRGB565565;	//	�{�T�C�Y�̃R�s�[���T�|�[�g���Ă���RGB565
class CFastPlaneRGB555555;	//	�{�T�C�Y�̃R�s�[���T�|�[�g���Ă���RGB555

//	�����x�̉摜�����̂��߂ɁARGB��double��WORD�Ŏ����Ă���悤��
//	�s�N�Z���t�H�[�}�b�g�Ƃɂ��Ă���舵�������̂����A���̂ւ�́A
//	�̂��̂��̊g���Ƃ������Ƃɂ��悤�D�D

//////////////////////////////////////////////////////////////////////////////
//	�������̌v�Z�̂��߂̏�Z�e�[�u��
//////////////////////////////////////////////////////////////////////////////

class CFastPlaneBlendTable {
public:
	//	���̃N���X�A�Q�l�a�̃e�[�u����p�ӂ���̂��O�O�G
	//	���܂ǂ��̃}�V���Ȃ�A�ǂ��Ă��Ɩ�����ˁH�H�O�O�G�G

	static BYTE	abyMulTable[256*256];	//	[x * y / 255]
	static BYTE	abyMulTable2[256*256];	//	[x * (255-y) / 255]
										//	=> [x][y]��[x + (y << 8)]�ƃA�N�Z�X���邱�ƁI
	//	16�r�b�g�������e�[�u��
	static	WORD awdMulTableRGB565[65536*16]; // [RGB(16)* ��(4bit)]
	static	WORD awdMulTableRGB555[65536*16]; // [RGB(15)* ��(4bit)]
			//	�������́A�{���́A���̔����ŗǂ��̂����A
			//	�ŏ�ʂ��s��ɂȂ��Ă���\��������̂ŁA�r�b�g�}�X�N��
			//	�Ƃ�̂����邢�̂ŁA����ł����Ă܂��I�O�O�G

/*
	//	����A���؁[�i�΁j

	//	16�r�b�g�O�a���Z�e�[�u��
	static	WORD awdAddTableRGB565[65536*8]; // [RGB(16)* �O�a�l(4bit)]
	static	WORD awdAddTableRGB555[65536*8]; // [RGB(15)* �O�a�l(4bit)]
	//	16�r�b�g�O�a���Z�e�[�u��
	static	WORD awdSubTableRGB565[65536*8]; // [RGB(16)* �O�a�l(4bit)]
	static	WORD awdSubTableRGB555[65536*8]; // [RGB(15)* �O�a�l(4bit)]
*/

	//	RGB555��256Color�ϊ��e�[�u��
	static	BYTE abyConvertTable555[32768];

	//	��L�̃e�[�u���̂��߂̏������֐��BCFastDraw������������B
	static void InitTable();

	//	�p���b�g���ύX�ɂȂ����Ƃ��ɌĂяo���āAabyConvertTable555�����������ׂ��I
	static void OnChangePalette();
};

class CFastPlaneBytePal /* 8�r�b�gPalette */ {
public:
	BYTE GetR() const { return 0; }	//	�p���b�g�Q�Ƃ��ĕԂ��ׂ��Ȃ̂��H
	void SetR(BYTE r) { _rgb = r; }
	BYTE GetG() const { return 0; }
	void SetG(BYTE g) { _rgb = g; }
	BYTE GetB() const { return 0; }
	void SetB(BYTE b) { _rgb = b; }

	//	dummy�ł��p�ӂ��Ă����Ȃ��ƁA�d��Z�I�y���[�^�����܂������Ȃ�
	BYTE GetA() const { return 255; }
	void SetA(BYTE b) { }

	void SetRGB(BYTE r,BYTE g,BYTE b) { }	// not supported
	void SetRGB(BYTE rgb) { _rgb = rgb; }
	BYTE GetRGB() const { return _rgb; }

	void	Clear() { _rgb = 0; }

	//	�ϊ��q
	//		���蔲�����������ǁA�ϊ��q�A���݂��Ȃ���肸���ƃ}�V�ł���D�D
	template <class X>
	CFastPlaneBytePal& operator = (const X &src){
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		return *this;
	}
	//	�R�s�[�R���X�g���N�^�́A�ʓr�p��
	CFastPlaneBytePal& operator = (const CFastPlaneBytePal &src){
		_rgb = src._rgb;
		return *this;
	}

	//	RGB555�Ƃ̕ϊ��q
	inline CFastPlaneBytePal& operator = (const CFastPlaneRGB555 &src);

	//	��Z�I�y���[�^
	template <class X>
	CFastPlaneBytePal& operator *= (const X &src){
	//	�]������alpha�l��blend����Ɖ���
		WORD alpha8 = ((WORD)src.GetA()) << 8;
		SetRGB(
			CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8]
		);
		return *this;
	}
	//	��L�̏�Z�I�y���[�^�̒萔�o�[�W����
	CFastPlaneBytePal& operator *= (BYTE alpha){
		WORD alpha8 = ((WORD)alpha) << 8;
		SetRGB(
			CFastPlaneBlendTable::abyMulTable[ GetR() + alpha8],
			CFastPlaneBlendTable::abyMulTable[ GetG() + alpha8],
			CFastPlaneBlendTable::abyMulTable[ GetB() + alpha8]
		);
		return *this;
	}

	//	�d��Z�I�y���[�^
	template <class X>
	CFastPlaneBytePal& dmul (const X &src,BYTE alpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		WORD alpha8 = CFastPlaneBlendTable::abyMulTable[alpha + ((WORD)src.GetA() << 8)];
		SetRGB(
			CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8]
		);
		return *this;
	}
	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneBytePal& operator += (const X &src){
		WORD r = (WORD)GetR() + src.GetR();
		if (r>0xff) r=0xff;
		WORD g = (WORD)GetG() + src.GetG();
		if (g>0xff) g=0xff;
		WORD b = (WORD)GetB() + src.GetB();
		if (b>0xff) b=0xff;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneBytePal& add (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() + r;
		if (r2>0xff) r2=0xff;
		WORD g2 = (WORD)GetG() + g;
		if (g2>0xff) g2=0xff;
		WORD b2 = (WORD)GetB() + b;
		if (b2>0xff) b2=0xff;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneBytePal& operator -= (const X &src){
		WORD r = (WORD)GetR() - src.GetR();
		if (r>0xff) r=0;	//	����A�I�[�o�[�t���[���Ă�
		WORD g = (WORD)GetG() - src.GetG();
		if (g>0xff) g=0;
		WORD b = (WORD)GetB() - src.GetB();
		if (b>0xff) b=0;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneBytePal& sub (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() - r;
		if (r2>0xff) r2=0;
		WORD g2 = (WORD)GetG() - g;
		if (g2>0xff) g2=0;
		WORD b2 = (WORD)GetB() - b;
		if (b2>0xff) b2=0;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}

private:
	BYTE _rgb;
};

class CFastPlaneRGB555 /* 16�r�b�gRGB 555 */ {
public:
	BYTE GetR() const { return (_rgb & 0x7c00) >> (10-3); }
	void SetR(BYTE r) { _rgb = (_rgb & ~(0x7c00+0x8000)) | ((WORD)(r&0xf8)<<(10-3)); }
	BYTE GetG() const { return (_rgb & 0x03e0) >> (5-3); }
	void SetG(BYTE g) { _rgb = (_rgb & ~(0x03e0+0x8000)) | ((WORD)(g&0xfc)<<(5-3)); }
	BYTE GetB() const { return (_rgb & 0x001f) << 3; }
	void SetB(BYTE b) { _rgb = (_rgb & ~(0x001f+0x8000)) | (b >> 3); }
		//	+0x8000�́A��ʂP�r�b�g�ɃS�~�����グ��ƌ��Ȃ̂ŁA���Z�b�g���Ă���

	//	dummy�ł��p�ӂ��Ă����Ȃ��ƁA�d��Z�I�y���[�^�����܂������Ȃ�
	BYTE GetA() const { return 255; }
	void SetA(BYTE b) { }

	void SetRGB(BYTE r,BYTE g,BYTE b) { _rgb = ((WORD)(r&0xf8)<<(10-3)) | ((WORD)(g&0xf8)<<(5-3)) | (b >> 3); }
	void SetRGB(WORD rgb) { _rgb = rgb; }
	//	�������_�C���N�g�ɓn���Ƃ��ɁA(WORD)0 �̂悤�ɃL���X�g���Ȃ���
	//	�����Ȃ�������[�̂��A�Ȃ񂾂��_�T���C������̂����D�D

	WORD GetRGB() const { return _rgb; }
	//	�{�T�C�Y�ɂ��ĕԂ�
	DWORD GetRGB_DWORD() const { return	((DWORD)_rgb << 16) | (DWORD)_rgb; }

	void	Clear() { _rgb = 0; }

	//	�ϊ��q
	//		���蔲�����������ǁA�ϊ��q�A���݂��Ȃ���肸���ƃ}�V�ł���D�D
	template <class X>
	CFastPlaneRGB555& operator = (const X &src){
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		return *this;
	}
	//	�R�s�[�R���X�g���N�^�́A�ʓr�p��
	CFastPlaneRGB555& operator = (const CFastPlaneRGB555 &src){
		_rgb = src._rgb;
		return *this;
	}
	//	��Z�I�y���[�^
	template <class X>
	CFastPlaneRGB555& operator *= (const X &src){
	//	�]������alpha�l��blend����Ɖ���
		DWORD alpha8 = src.GetA() << 8;
		SetRGB(
			CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8]
		);
		return *this;
	}
/*	//	����́A�e�[�u���ɂ�荂���������F
	//	��L�̏�Z�I�y���[�^�̒萔�o�[�W����
	CFastPlaneRGB555& operator *= (BYTE alpha){
		WORD alpha8 = ((WORD)alpha) << 8;
		SetRGB(
			CFastPlaneBlendTable::abyMulTable[ GetR() + alpha8],
			CFastPlaneBlendTable::abyMulTable[ GetG() + alpha8],
			CFastPlaneBlendTable::abyMulTable[ GetB() + alpha8]
		);
		return *this;
	}
*/
	//	�d��Z�I�y���[�^
	template <class X>
	CFastPlaneRGB555& dmul (const X &src,BYTE alpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		if ( GetRGB()==src.GetRGB() ) return *this;

		DWORD alpha8 = CFastPlaneBlendTable::abyMulTable[alpha + ((DWORD)src.GetA() << 8)];
		SetRGB(
			CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8]
		);
		return *this;
	}

	//	�e�a���̂���v���[���̂��߂�alpha�������ꂽ�]��
	inline CFastPlaneRGB555& operator *= (const CFastPlaneARGB4555 &src);
	inline CFastPlaneRGB555& operator *= (BYTE alpha);
	inline CFastPlaneRGB555&  dmul (const CFastPlaneARGB4555 &src,BYTE alpha);
	//	�������g�ɑ΂���A�d��Z���A���R�K�v...
	inline CFastPlaneRGB555&  dmul (const CFastPlaneRGB555 &src,BYTE alpha){
		if ( GetRGB()==src.GetRGB() ) return *this;

		DWORD alpha16_2e = ((DWORD)(alpha&0xf0)) << (4+8); // 4 bit�̂ݐ���
		DWORD alpha16_2i = alpha16_2e ^ 0xf0000; // �� 4 bit�𔽓]
		_rgb =
			  CFastPlaneBlendTable::awdMulTableRGB555[src._rgb + alpha16_2e]
			+ CFastPlaneBlendTable::awdMulTableRGB555[	  _rgb + alpha16_2i];
		return *this;
	}
	//	������́Adst = ��src�~src + ��dst�~dst
	inline CFastPlaneRGB555&  dmul (const CFastPlaneARGB4555 &src,BYTE srcalpha,BYTE dstalpha);
	inline CFastPlaneRGB555&  dmul (const CFastPlaneRGB555 &src,BYTE srcalpha,BYTE dstalpha){
		DWORD alpha16_2e = ((DWORD)(srcalpha&0xf0)) << (4+8); // 4 bit�̂ݐ���
		DWORD alpha16_2i = ((DWORD)(dstalpha&0xf0)) << (4+8);
		_rgb =
			  CFastPlaneBlendTable::awdMulTableRGB555[src._rgb + alpha16_2e]
			+ CFastPlaneBlendTable::awdMulTableRGB555[	  _rgb + alpha16_2i];
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneRGB555& operator += (const X &src){
		WORD r = (WORD)GetR() + src.GetR();
		if (r>0xff) r=0xff;
		WORD g = (WORD)GetG() + src.GetG();
		if (g>0xff) g=0xff;
		WORD b = (WORD)GetB() + src.GetB();
		if (b>0xff) b=0xff;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}

	//	�����Ƃ̉��Z�́A�œK�����ꂽ���[�`��
	//		RGB555 satulation add (C)���`
	CFastPlaneRGB555& operator += (const CFastPlaneRGB555& src){
		WORD c, c0,c1;
		c0 = GetRGB();
		c1 = src.GetRGB();
		c = (((c0 & c1) << 1) + ((c0 ^ c1) & 0x7bde)) & 0x8420;
		c = ((c >> 5) + 0x3def) ^ 0x3def;
		SetRGB((c0 + c1 - c) | c);
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneRGB555& add (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() + r;
		if (r2>0xff) r2=0xff;
		WORD g2 = (WORD)GetG() + g;
		if (g2>0xff) g2=0xff;
		WORD b2 = (WORD)GetB() + b;
		if (b2>0xff) b2=0xff;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneRGB555& operator -= (const X &src){
		WORD r = (WORD)GetR() - src.GetR();
		if (r>0xff) r=0;	//	����A�I�[�o�[�t���[���Ă�
		WORD g = (WORD)GetG() - src.GetG();
		if (g>0xff) g=0;
		WORD b = (WORD)GetB() - src.GetB();
		if (b>0xff) b=0;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//		RGB555 satulation sub (C)���`
	CFastPlaneRGB555& operator -= (const CFastPlaneRGB555& src){
		WORD c, c0,c1;
		c0 = GetRGB();
		c1 = src.GetRGB();
		c = (((~c0 & c1) << 1) + ((~c0 ^ c1) & 0x7bde)) & 0x8420;
		c = (( c >> 5) + 0x3def) ^ 0x3def;
		SetRGB((c0 | c) - (c1 | c));
		return *this;
	}

	//	�萔�O�a���Z
	CFastPlaneRGB555& sub (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() - r;
		if (r2>0xff) r2=0;
		WORD g2 = (WORD)GetG() - g;
		if (g2>0xff) g2=0;
		WORD b2 = (WORD)GetB() - b;
		if (b2>0xff) b2=0;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}

	//	ARGB4555�Ƃ̐e�a��
	inline CFastPlaneRGB555& operator += (const CFastPlaneARGB4555& src);
	inline CFastPlaneRGB555& operator -= (const CFastPlaneARGB4555& src);

private:
	WORD _rgb;
};

class CFastPlaneRGB565 /* 16�r�b�gRGB 565 */ {
public:
	BYTE GetR() const { return (_rgb & 0xf800) >> (11-3); }
	void SetR(BYTE r) { _rgb = (_rgb & ~0xf800) | (((WORD)(r&0xf8))<<(11-3)); }
	BYTE GetG() const { return (_rgb & 0x07e0) >> (6-3); }
	void SetG(BYTE g) { _rgb = (_rgb & ~0x07e0) | (((WORD)(g&0xfc))<<(6-3)); }
	BYTE GetB() const { return (_rgb & 0x001f) << 3; }
	void SetB(BYTE b) { _rgb = (_rgb & ~0x001f) | (b >> 3); }

	//	dummy�ł��p�ӂ��Ă����Ȃ��ƁA�d��Z�I�y���[�^�����܂������Ȃ�
	BYTE GetA() const { return 255; }
	void SetA(BYTE b) { }

	void SetRGB(BYTE r,BYTE g,BYTE b) { _rgb = ((WORD)(r&0xf8)<<(11-3)) | ((WORD)(g&0xfc)<<(6-3)) | (b >> 3); }
	void SetRGB(WORD rgb) { _rgb = rgb; }
	WORD GetRGB() const { return _rgb; }

	//	�{�T�C�Y�ɂ��ĕԂ�
	DWORD GetRGB_DWORD() const { return	((DWORD)_rgb << 16) | (DWORD)_rgb; }

	void	Clear() { _rgb = 0; }

	//	�ϊ��q
	//		���蔲�����������ǁA�ϊ��q�A���݂��Ȃ���肸���ƃ}�V�ł���D�D
	template <class X>
	CFastPlaneRGB565& operator = (const X &src){
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		return *this;
	}
	//	�R�s�[�R���X�g���N�^�́A�ʓr�p��
	CFastPlaneRGB565& operator = (const CFastPlaneRGB565 &src){
		_rgb = src._rgb;
		return *this;
	}

	//	��Z�I�y���[�^
	template <class X>
	CFastPlaneRGB565& operator *= (const X &src){
	//	�]������alpha�l��blend����Ɖ���
		DWORD alpha8 = ((DWORD)src.GetA()) << 8;
		SetRGB(
			CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8]
		);
		return *this;
	}

/*	//	����́A�e�[�u���ɂ�荂���������F
	//	��L�̏�Z�I�y���[�^�̒萔�o�[�W����
	CFastPlaneRGB565& operator *= (BYTE alpha){
		WORD alpha8 = ((WORD)alpha) << 8;
		SetRGB(
			CFastPlaneBlendTable::abyMulTable[ GetR() + alpha8],
			CFastPlaneBlendTable::abyMulTable[ GetG() + alpha8],
			CFastPlaneBlendTable::abyMulTable[ GetB() + alpha8]
		);
		return *this;
	}
*/

	//	�d��Z�I�y���[�^
	template <class X>
	CFastPlaneRGB565&  dmul (const X &src,BYTE alpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		if ( GetRGB()==src.GetRGB() ) return *this;

		const DWORD alpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[alpha + ((DWORD)src.GetA() << 8)] << 8;
		SetRGB(
			CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8]
		);
		return *this;
	}

	//	�e�a���̂���v���[���̂��߂�alpha�������ꂽ�]��
	inline CFastPlaneRGB565& operator *= (const CFastPlaneARGB4565 &src);
	inline CFastPlaneRGB565& operator *= (BYTE alpha);
	inline CFastPlaneRGB565&  dmul (const CFastPlaneARGB4565 &src,BYTE alpha);
	//	�������g�ɑ΂���A�d��Z���A���R�K�v...
	inline CFastPlaneRGB565&  dmul (const CFastPlaneRGB565 &src,BYTE alpha){
		if ( GetRGB()==src.GetRGB() ) return *this;

		DWORD alpha16_2e = ((DWORD)(alpha&0xf0)) << (4+8); // 4 bit�̂ݐ���
		DWORD alpha16_2i = alpha16_2e ^ 0xf0000; // �� 4 bit�𔽓]
		_rgb =
			  CFastPlaneBlendTable::awdMulTableRGB565[src._rgb + alpha16_2e]
			+ CFastPlaneBlendTable::awdMulTableRGB565[	  _rgb + alpha16_2i];
		return *this;
	}
	//	������́Adst = ��src�~src + ��dst�~dst
	inline CFastPlaneRGB565&  dmul (const CFastPlaneARGB4565 &src,BYTE srcalpha,BYTE dstalpha);
	//	�������g�ɑ΂���A�d��Z���A���R�K�v...
	inline CFastPlaneRGB565&  dmul (const CFastPlaneRGB565 &src,BYTE srcalpha,BYTE dstalpha){
		DWORD alpha16_2e = ((DWORD)(srcalpha&0xf0)) << (4+8); // 4 bit�̂ݐ���
		DWORD alpha16_2i = ((DWORD)(dstalpha&0xf0)) << (4+8); // 4 bit�̂ݐ���
		_rgb =
			  CFastPlaneBlendTable::awdMulTableRGB565[src._rgb + alpha16_2e]
			+ CFastPlaneBlendTable::awdMulTableRGB565[	  _rgb + alpha16_2i];
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneRGB565& operator += (const X &src){
		WORD r = (WORD)GetR() + src.GetR();
		if (r>0xff) r=0xff;
		WORD g = (WORD)GetG() + src.GetG();
		if (g>0xff) g=0xff;
		WORD b = (WORD)GetB() + src.GetB();
		if (b>0xff) b=0xff;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}

	//	�����Ƃ̉��Z�́A�œK�����ꂽ���[�`��
	//		RGB565 satulation add (C)���` & ��˂��炨
	CFastPlaneRGB565& operator += (const CFastPlaneRGB565& src){
		WORD c, c0,c1;
		c0 = GetRGB();
		c1 = src.GetRGB();

		c = ((c0 & c1) + (((c0 ^ c1) & 0xf7de) >> 1)) & 0x8410;
		//	�����̕����A(C)��˂��炨
		c = (((((short)(c + 0x3df0)) >> 5) & 0xfbff) + 0x200)^0x3ef;

/*	//	����͂�˂��炨�����`����̂�������������
						---------------R ----G-----B-----
						0123012301230123 0123012301230123
add eax, 0x001f7be0		----------Rrrrrr GggggBbbbbb-----
shr eax, 6				---------------- RrrrrrGggggBbbbb b
and eax, 0x0000fbff		---------------- Rrrrr-GggggBbbbb
add eax, 0x00000200		---------------- RrrrrGgggggBbbbb
xor eax, 0x00007bef		---------------- RRRRRGGGGGGBBBBB
		//	�}�X�N���P����邱�Ƃɗ��ӂ���
		c = (((((c + 0x1f7be0)) >> 6) & 0xfbff) + 0x200)^0x7bef;
*/

		SetRGB((c0 + c1 - c) | c);
		return *this;
	}

/*
���`���񂪍l�����ARGB565�p�̃r�b�g�}�X�N�B

�b���x���ł̋L�q�����DWORD/QWORD�Ή��ł��B
-------------------------------------------------------------------
[WORD��]

					R----G-----B----
add eax, 0x3df0		RGggggBbbbbb----
sar eax, 5			RRRRRRGggggBbbbb b
and eax, 0xfbff		RRRRR-GggggBbbbb
add eax, 0x0200		RRRRRGgggggBbbbb
xor eax, 0x03ef		RRRRRGGGGGGBBBBB


[DWORD��]

						R----G-----B---- R----G-----B----
add eax, 0x3df7bdf0		RGggggBbbbbbRrrr rGggggBbbbbb----
sar eax, 5				RRRRRRGggggBbbbb bRrrrrGggggBbbbb b
and eax, 0xfbff7bff		RRRRR-GggggBbbbb -Rrrr-GggggBbbbb
add eax, 0x02004200		RRRRRGgggggBbbbb RrrrrGgggggBbbbb
xor eax, 0x03ef7bef		RRRRRGGGGGGBBBBB RRRRRGGGGGGBBBBB

-------------------------------------------------------------------

*/

	//	�萔�O�a���Z
	CFastPlaneRGB565& add (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() + r;
		if (r2>0xff) r2=0xff;
		WORD g2 = (WORD)GetG() + g;
		if (g2>0xff) g2=0xff;
		WORD b2 = (WORD)GetB() + b;
		if (b2>0xff) b2=0xff;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}


	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneRGB565& operator -= (const X &src){
		WORD r = (WORD)GetR() - src.GetR();
		if (r>0xff) r=0;	//	����A�I�[�o�[�t���[���Ă�
		WORD g = (WORD)GetG() - src.GetG();
		if (g>0xff) g=0;
		WORD b = (WORD)GetB() - src.GetB();
		if (b>0xff) b=0;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}

	//		RGB565 satulation sub (C)���` & ��˂��炨
	CFastPlaneRGB565& operator -= (const CFastPlaneRGB565& src){
		WORD c, c0,c1;
		c0 = GetRGB();
		c1 = src.GetRGB();
		//	�A�Z���u����cy�g���ċL�q�ł���Ηǂ��̂���...
//		c = (((~c0 & c1) << 1) + ((~c0 ^ c1) & 0xf7de)) & 0x10820;
		//	�����̕����A(C)��˂��炨

		c = ((~c0 & c1) + (((~c0 ^ c1) & 0xf7de) >> 1)) & 0x8410;
		c = (((((short)(c + 0x3df0)) >> 5) & 0xfbff) + 0x200)^0x3ef;
	
		SetRGB((c0 | c) - (c1 | c));
		return *this;
	}

	//	�萔�O�a���Z
	CFastPlaneRGB565& sub (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() - r;
		if (r2>0xff) r2=0;
		WORD g2 = (WORD)GetG() - g;
		if (g2>0xff) g2=0;
		WORD b2 = (WORD)GetB() - b;
		if (b2>0xff) b2=0;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}

	//	ARGB4565�Ƃ̐e�a��
	inline CFastPlaneRGB565& operator += (const CFastPlaneARGB4565& src);
	inline CFastPlaneRGB565& operator -= (const CFastPlaneARGB4565& src);

private:
	WORD _rgb;
};

class CFastPlaneARGB4565 /* 16�r�b�gRGB 565 + A4�r�b�g == 24�r�b�g�Ǝ�ARGB */ {
public:
	BYTE GetR() const { return (_rgb & 0xf800) >> (11-3); }
	void SetR(BYTE r) { _rgb = (_rgb & ~0xf800) | (((WORD)(r&0xf8))<<(11-3)); }
	BYTE GetG() const { return (_rgb & 0x07e0) >> (6-3); }
	void SetG(BYTE g) { _rgb = (_rgb & ~0x07e0) | (((WORD)(g&0xfc))<<(6-3)); }
	BYTE GetB() const { return (_rgb & 0x001f) << 3; }
	void SetB(BYTE b) { _rgb = (_rgb & ~0x001f) | (b >> 3); }
	BYTE GetA() const { return _a << 4; }
	void SetA(BYTE a) { _a	 = a >> 4; }

	void SetRGB(BYTE r,BYTE g,BYTE b) { _rgb = ((WORD)(r&0xf8)<<(11-3)) | ((WORD)(g&0xfc)<<(6-3)) | (b >> 3); }
	void SetRGB(WORD rgb) { _rgb = rgb; _a = 15; }
	WORD GetRGB() const { return _rgb; }
	DWORD GetRGBA() const { return *(DWORD*)&_rgb; }

	void	Clear() { _rgb = 0; _a = 0; }

	//	�ϊ��q
	//		���蔲�����������ǁA�ϊ��q�A���݂��Ȃ���肸���ƃ}�V�ł���D�D
	template <class X>
	CFastPlaneARGB4565& operator = (const X &src){
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		_a = 15;	//	����T�|�[�g�v���[������̃R�s�[�Ȃ̂ŁA����ݒ肵�Ă����˂΂Ȃ�Ȃ��I
		return *this;
	}
	//	�R�s�[�R���X�g���N�^�́A�ʓr�p��
	//		�˂����́A�����R�s�[����̂Œ��ӂ���
	CFastPlaneARGB4565& operator = (const CFastPlaneARGB4565 &src){
		_rgb = src._rgb;
		_a	 = src._a;
		return *this;
	}
	//	�������T�[�t�F�[�X����̃R�s�[
	CFastPlaneARGB4565& operator = (const CFastPlaneRGB565 &src){
		_rgb = src.GetRGB();
		_a	 = 15;	//	15�Ƃ̓��̍ő�l
		return *this;
	}
	//	�����݂̃v���[������̃R�s�[
	inline CFastPlaneARGB4565& operator = (const CFastPlaneARGB8888 &src);
	inline CFastPlaneARGB4565& operator = (const CFastPlaneABGR8888 &src);
	inline CFastPlaneARGB4565& operator = (const CFastPlaneARGB4555 &src);

	//	���˃��T�[�t�F�[�X�ցA���ȊO�̂݃R�s�[
	CFastPlaneARGB4565& CopyWithoutAlpha(const CFastPlaneARGB4565 &src){
		_rgb = src._rgb;
		return *this;
	}
	//	���l�̔��]
	CFastPlaneARGB4565& FlushAlpha(const CFastPlaneARGB4565 &src){
		_a = 15-src._a;
		return *this;
	}

	//	��Z�I�y���[�^
	template <class X>
	CFastPlaneARGB4565& operator *= (const X &src){
	//	�]������alpha�l��blend����Ɖ���
		DWORD alpha8 = ((DWORD)src.GetA()) << 8;
		SetRGB(
			CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8]
		);
		return *this;
	}
	//	��L�̏�Z�I�y���[�^�̒萔�o�[�W����
	CFastPlaneARGB4565& operator *= (BYTE alpha){
		//	�P�Ƀ��������������ق���������łȂ����H�H
		/*
		DWORD alpha8 = ((DWORD)alpha) << 8;
		BYTE r = CFastPlaneBlendTable::abyMulTable[ GetR() + alpha8];
		BYTE g = CFastPlaneBlendTable::abyMulTable[ GetG() + alpha8];
		BYTE b = CFastPlaneBlendTable::abyMulTable[ GetB() + alpha8];
		SetRGB(r,g,b);
		*/
		DWORD alpha8 = ((DWORD)alpha) << 8;
		SetA( CFastPlaneBlendTable::abyMulTable[GetA() + alpha8] );
		return *this;
	}
	//	�d��Z�I�y���[�^
	template <class X>
	CFastPlaneARGB4565&	 dmul (const X &src,BYTE alpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		DWORD alpha8 = CFastPlaneBlendTable::abyMulTable[alpha + ((DWORD)src.GetA() << 8)];
		SetRGB(
			CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8]
		);
		return *this;
	}
	template <class X>
	CFastPlaneARGB4565&	 dmul (const X &src,BYTE srcalpha,BYTE dstalpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		DWORD srcalpha8 = CFastPlaneBlendTable::abyMulTable[srcalpha + ((DWORD)src.GetA() << 8)];
		DWORD dstalpha8 = CFastPlaneBlendTable::abyMulTable[dstalpha + ((DWORD)	 GetA() << 8)];
		SetRGB(
			CFastPlaneBlendTable::abyMulTable[src.GetR() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetR() + dstalpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetG() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetG() + dstalpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetB() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetB() + dstalpha8]
		);
		return *this;
	}

	CFastPlaneARGB4565&	 MulAlphaToAlpha(const CFastPlaneARGB4565&src){
/*
	PhotoShop�I���C������
		�V������  = ��src + (1-��src)�~��dst
		�V����RGB =	 (RGBSrc�~��src�@�{�@RGBDst�~(1-��src)�~��dst)/(��src + ��dst)
		�ˁ@������A��ɕ�����ƁA
		//	��src' = ��src / (��src + ��dst)
		//	��dst' = ((1-��src)�~��dst) / (��src + ��dst)
		�Ƃ��āARGBSrc�~��src' + RGBDst�~��dst'
*/
		BYTE nAlphaSrc = src.GetA();
		BYTE nAlphaDst = GetA();
		BYTE nNewBeta = (CFastPlaneBlendTable::abyMulTable[(15*16-(nAlphaSrc)) + (nAlphaDst << 8)]);
		BYTE nNewAlpha = nAlphaSrc + nNewBeta;
		//	��src' = ��src / (��src + ��dst)
		//	��dst' = ��dst / (��src + ��dst)
		SetA(nNewAlpha);

		int nAlpha = nAlphaSrc + nNewBeta;
		if (nAlpha != 0) {
		//	�������[���̂Ƃ��́ARGB���v�Z����ɒl���Ȃ�
			BYTE nAlphaSrcD = (nAlphaSrc*15) / nAlpha;
			BYTE nAlphaDstD = (nNewBeta*15) / nAlpha;
			//	0�`15�ɃX�P�[�����O���āA�u�����h�e�[�u���𗘗p����

			//	��̃p�����[�^�Ńu�����h����
			DWORD alpha16e = ((DWORD)(nAlphaSrcD)) << (8+8); // 4 bit
			DWORD alpha16i = ((DWORD)(nAlphaDstD)) << (8+8); // �� 4 bit�𔽓]
			_rgb = CFastPlaneBlendTable::awdMulTableRGB565[src._rgb + alpha16e] +
				   CFastPlaneBlendTable::awdMulTableRGB565[	   _rgb + alpha16i];
			//	�����ꂾ���Ńu�����h��������񂩁[�A�A�������ȁ[�O�O�G
		}
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneARGB4565& operator += (const X &src){
		WORD r = (WORD)GetR() + src.GetR();
		if (r>0xff) r=0xff;
		WORD g = (WORD)GetG() + src.GetG();
		if (g>0xff) g=0xff;
		WORD b = (WORD)GetB() + src.GetB();
		if (b>0xff) b=0xff;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneARGB4565& add (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() + r;
		if (r2>0xff) r2=0xff;
		WORD g2 = (WORD)GetG() + g;
		if (g2>0xff) g2=0xff;
		WORD b2 = (WORD)GetB() + b;
		if (b2>0xff) b2=0xff;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}
	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneARGB4565& operator -= (const X &src){
		WORD r = (WORD)GetR() - src.GetR();
		if (r>0xff) r=0;	//	����A�I�[�o�[�t���[���Ă�
		WORD g = (WORD)GetG() - src.GetG();
		if (g>0xff) g=0;
		WORD b = (WORD)GetB() - src.GetB();
		if (b>0xff) b=0;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneARGB4565& sub (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() - r;
		if (r2>0xff) r2=0;
		WORD g2 = (WORD)GetG() - g;
		if (g2>0xff) g2=0;
		WORD b2 = (WORD)GetB() - b;
		if (b2>0xff) b2=0;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}

//private:
	WORD _rgb;
	BYTE _a;	//	���ۂɂ́A������4�r�b�g�����g��Ȃ�
};

class CFastPlaneARGB4555 /* 16�r�b�gRGB 555 + A4�r�b�g == 24�r�b�g�Ǝ�ARGB */ {
public:
	BYTE GetR() const { return (_rgb & 0x7c00) >> (10-3); }
	void SetR(BYTE r) { _rgb = (_rgb & ~0x7c00) | ((WORD)(r&0xf8)<<(10-3)); }
	BYTE GetG() const { return (_rgb & 0x03e0) >> (5-3); }
	void SetG(BYTE g) { _rgb = (_rgb & ~0x03e0) | ((WORD)(g&0xf8)<<(5-3)); }
	BYTE GetB() const { return (_rgb & 0x001f) << 3; }
	void SetB(BYTE b) { _rgb = (_rgb & ~0x001f) | (b >> 3); }
	BYTE GetA() const { return _a << 4; }
	void SetA(BYTE a) { _a	 = a >> 4; }

	void SetRGB(BYTE r,BYTE g,BYTE b) { _rgb = ((WORD)(r&0xf8)<<(10-3)) | ((WORD)(g&0xf8)<<(5-3)) | (b >> 3); }
	void SetRGB(WORD rgb) { _rgb = rgb; _a = 15; }
	WORD GetRGB() const { return _rgb; }
	DWORD GetRGBA() const { return *(DWORD*)&_rgb; }

	void	Clear() { _rgb = 0; _a = 0; }

	//	�ϊ��q
	//		���蔲�����������ǁA�ϊ��q�A���݂��Ȃ���肸���ƃ}�V�ł���D�D
	template <class X>
	CFastPlaneARGB4555& operator = (const X &src){
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		_a = 15;	//	����T�|�[�g�v���[������̃R�s�[�Ȃ̂ŁA����ݒ肵�Ă����˂΂Ȃ�Ȃ��I
		return *this;
	}
	//	�R�s�[�R���X�g���N�^�́A�ʓr�p��
	//		�˂����́A�����R�s�[����̂Œ��ӂ���
	CFastPlaneARGB4555& operator = (const CFastPlaneARGB4555 &src){
		_rgb = src._rgb;
		_a	 = src._a;
		return *this;
	}
	//	�������T�[�t�F�[�X����̃R�s�[
	CFastPlaneARGB4555& operator = (const CFastPlaneRGB555 &src){
		_rgb = src.GetRGB();
		_a	 = 15;	//	15�Ƃ̓��̍ő�l
		return *this;
	}
	//	�����݂̃v���[������̃R�s�[
	inline CFastPlaneARGB4555& operator = (const CFastPlaneARGB8888 &src);
	inline CFastPlaneARGB4555& operator = (const CFastPlaneABGR8888 &src);
	inline CFastPlaneARGB4555& operator = (const CFastPlaneARGB4565 &src);

	//	���˃��T�[�t�F�[�X�ցA���ȊO�̂݃R�s�[
	CFastPlaneARGB4555& CopyWithoutAlpha(const CFastPlaneARGB4555 &src){
		_rgb = src._rgb;
		return *this;
	}
	//	���l�̔��]
	CFastPlaneARGB4555& FlushAlpha(const CFastPlaneARGB4555 &src){
		_a = 15-src._a;
		return *this;
	}

	//	��Z�I�y���[�^
	template <class X>
	CFastPlaneARGB4555& operator *= (const X &src){
	//	�]������alpha�l��blend����Ɖ���
		DWORD alpha8 = ((DWORD)src.GetA()) << 8;
		SetRGB(
			CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8]
		);
		return *this;
	}
	//	��L�̏�Z�I�y���[�^�̒萔�o�[�W����
	CFastPlaneARGB4555& operator *= (BYTE alpha){
		/*
		DWORD alpha8 = ((DWORD)alpha) << 8;
		BYTE r = CFastPlaneBlendTable::abyMulTable[ GetR() + alpha8];
		BYTE g = CFastPlaneBlendTable::abyMulTable[ GetG() + alpha8];
		BYTE b = CFastPlaneBlendTable::abyMulTable[ GetB() + alpha8];
		SetRGB(r,g,b);
		*/
		//	�P�Ƀ����������邩�H
		DWORD alpha8 = ((DWORD)alpha) << 8;
		SetA( CFastPlaneBlendTable::abyMulTable[GetA() + alpha8] );
		return *this;
	}
	//	�d��Z�I�y���[�^
	template <class X>
	CFastPlaneARGB4555&	 dmul (const X &src,BYTE alpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		DWORD alpha8 = CFastPlaneBlendTable::abyMulTable[alpha + ((DWORD)src.GetA() << 8)];
		SetRGB(
			CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8]
		);
		return *this;
	}
	template <class X>
	CFastPlaneARGB4555&	 dmul (const X &src,BYTE srcalpha,BYTE dstalpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		DWORD srcalpha8 = CFastPlaneBlendTable::abyMulTable[srcalpha + ((DWORD)src.GetA() << 8)];
		DWORD dstalpha8 = CFastPlaneBlendTable::abyMulTable[dstalpha + ((DWORD)	 GetA() << 8)];
		SetRGB(
			CFastPlaneBlendTable::abyMulTable[src.GetR() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetR() + dstalpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetG() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetG() + dstalpha8],
			CFastPlaneBlendTable::abyMulTable[src.GetB() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetB() + dstalpha8]
		);
		return *this;
	}

	CFastPlaneARGB4555&	 MulAlphaToAlpha(const CFastPlaneARGB4555&src){
/*
	PhotoShop�I���C������
		�V������  = ��src + (1-��src)�~��dst
		�V����RGB =	 (RGBSrc�~��src�@�{�@RGBDst�~��dst)/(��src + ��dst)
		�ˁ@������A��ɕ�����ƁA
		//	��src' = ��src / (��src + ��dst)
		//	��dst' = ��dst / (��src + ��dst)
		�Ƃ��āARGBSrc�~��src' + RGBDst�~��dst'
*/
		BYTE nAlphaSrc = src.GetA();
		BYTE nAlphaDst = GetA();
		BYTE nNewBeta = (CFastPlaneBlendTable::abyMulTable[(15*16-(nAlphaSrc)) + (nAlphaDst << 8)]);
		BYTE nNewAlpha = nAlphaSrc + nNewBeta;
		//	��src' = ��src / (��src + ��dst)
		//	��dst' = ��dst / (��src + ��dst)
		SetA(nNewAlpha);

		int nAlpha = nAlphaSrc + nNewBeta;
		if (nAlpha != 0) {
		//	�������[���̂Ƃ��́ARGB���v�Z����ɒl���Ȃ�
			BYTE nAlphaSrcD = (nAlphaSrc*15) / nAlpha;
			BYTE nAlphaDstD = (nNewBeta*15) / nAlpha;
			//	0�`15�ɃX�P�[�����O���āA�u�����h�e�[�u���𗘗p����

			//	��̃p�����[�^�Ńu�����h����
			DWORD alpha16e = ((DWORD)(nAlphaSrcD)) << (8+8); // 4 bit
			DWORD alpha16i = ((DWORD)(nAlphaDstD)) << (8+8); // �� 4 bit�𔽓]

			_rgb = CFastPlaneBlendTable::awdMulTableRGB555[src._rgb + alpha16e] +
				   CFastPlaneBlendTable::awdMulTableRGB555[	   _rgb + alpha16i];
			//	�����ꂾ���Ńu�����h��������񂩁[�A�A�������ȁ[�O�O�G
		}
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneARGB4555& operator += (const X &src){
		WORD r = (WORD)GetR() + src.GetR();
		if (r>0xff) r=0xff;
		WORD g = (WORD)GetG() + src.GetG();
		if (g>0xff) g=0xff;
		WORD b = (WORD)GetB() + src.GetB();
		if (b>0xff) b=0xff;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneARGB4555& add (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() + r;
		if (r2>0xff) r2=0xff;
		WORD g2 = (WORD)GetG() + g;
		if (g2>0xff) g2=0xff;
		WORD b2 = (WORD)GetB() + b;
		if (b2>0xff) b2=0xff;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}
	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneARGB4555& operator -= (const X &src){
		WORD r = (WORD)GetR() - src.GetR();
		if (r>0xff) r=0;	//	����A�I�[�o�[�t���[���Ă�
		WORD g = (WORD)GetG() - src.GetG();
		if (g>0xff) g=0;
		WORD b = (WORD)GetB() - src.GetB();
		if (b>0xff) b=0;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneARGB4555& sub (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() - r;
		if (r2>0xff) r2=0;
		WORD g2 = (WORD)GetG() - g;
		if (g2>0xff) g2=0;
		WORD b2 = (WORD)GetB() - b;
		if (b2>0xff) b2=0;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}

//private:
	WORD _rgb;
	BYTE _a;	//	���ۂɂ́A������4�r�b�g�����g��Ȃ�
};

class CFastPlaneRGB888 /* 24�r�b�gRGB 888 �ʏ�A����̂͂�.. */{
public:
	BYTE GetR() const { return _r; }
	void SetR(BYTE r) { _r = r; }
	BYTE GetG() const { return _g; }
	void SetG(BYTE g) { _g = g; }
	BYTE GetB() const { return _b; }
	void SetB(BYTE b) { _b = b; }

	//	dummy�ł��p�ӂ��Ă����Ȃ��ƁA�d��Z�I�y���[�^�����܂������Ȃ�
	BYTE GetA() const { return 255; }
	void SetA(BYTE b) { }

	void SetRGB(BYTE r,BYTE g,BYTE b) { _r=r; _g=g; _b=b; }
	void SetRGB(DWORD rgb) {
		*(WORD*)(&_b) = (WORD)(rgb & 0xffff);
		*((BYTE*)(&_b)+2) = (BYTE)(rgb >> 16);
		//	��DIBSection����VideoMemory��̃T�[�t�F�[�X���ƃ}�Y�[(߄D�)

//		*(DWORD*)(&_b) = ((*(DWORD*)(&_b)) & 0xff000000) | rgb;
	}
	DWORD GetRGB() const {
		return *(WORD*)(&_b) | (((DWORD)_r) << 16);

		//	��DIBSection����VideoMemory��̃T�[�t�F�[�X���ƃ}�Y�[(߄D�)
//		return *(DWORD*)(&_b) & 0xffffff;
		//	������A�I�[�o�[�A�N�Z�X���Ă��邪�A
		//	���[�N����o�C�g�ł��邱�Ƃ͖����̂��H
		//	���������Ȃ�΁A��̂ɂ��Ă��΁A�P�O�����炢�������ς��̂����D�D		//	�˃T�[�t�F�[�X��������������h�b�g�Ŋm�ۂ���悤�ɕύX
	}
	void	Clear() {
		*(WORD*)(&_b) = 0; *((BYTE*)(&_b)+2)=0;
		//	��DIBSection����VideoMemory��̃T�[�t�F�[�X���ƃ}�Y�[(߄D�)
		//		�����������̃R�[�h�A�ǂݏo���ď������݂�����x���f�D�D
		// *(DWORD*)(&_b) &= 0xff000000;
	}

	//	�ϊ��q
	//		���蔲�����������ǁA�ϊ��q�A���݂��Ȃ���肸���ƃ}�V�ł���D�D
	template <class X>
	CFastPlaneRGB888& operator = (const X &src){
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		return *this;
	}
	//	�R�s�[�R���X�g���N�^�́A�ʓr�p��
	CFastPlaneRGB888& operator = (const CFastPlaneRGB888 &src){
		//_r = src._r;
		//_g = src._g;
		//_b = src._b;

		*(WORD*)(&_b) = *(WORD*)(&src._b);
		_r = src._r;

		//	��DIBSection����VideoMemory��̃T�[�t�F�[�X���ƃ}�Y�[(߄D�)
	//	*(DWORD*)(&_b) = ((*(DWORD*)(&_b)) & 0xff000000) | ((*(DWORD*)(&src._b)) & 0xffffff);
		return *this;
	}
	//	��Z�I�y���[�^
	template <class X>
	CFastPlaneRGB888& operator *= (const X &src){
	//	�]������alpha�l��blend����Ɖ���
		if ( GetRGB()==src.GetRGB() ) return *this;
		const DWORD alpha8 = ((DWORD)src.GetA()) << 8;
		//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8];
		return *this;
	}
	//	��L�̏�Z�I�y���[�^�̒萔�o�[�W����
	CFastPlaneRGB888& operator *= (BYTE alpha){
		DWORD alpha8 = ((DWORD)alpha) << 8;
		BYTE r = CFastPlaneBlendTable::abyMulTable[ GetR() + alpha8];
		BYTE g = CFastPlaneBlendTable::abyMulTable[ GetG() + alpha8];
		BYTE b = CFastPlaneBlendTable::abyMulTable[ GetB() + alpha8];
		SetRGB(r,g,b);
		return *this;
	}
	//	�d��Z�I�y���[�^
	template <class X>
	CFastPlaneRGB888&  dmul (const X &src,BYTE alpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		if ( GetRGB()==src.GetRGB() ) return *this;
		const DWORD alpha8 = CFastPlaneBlendTable::abyMulTable[(DWORD)alpha + (((DWORD)src.GetA()) << 8)] << 8;
		//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8];
		//SetRGB(r,g,b);
		return *this;
	}
	template <class X>
	CFastPlaneRGB888&  dmul (const X &src,BYTE srcalpha,BYTE dstalpha){
		DWORD srcalpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[srcalpha + (((DWORD)src.GetA()) << 8)] << 8;
		DWORD dstalpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[dstalpha + (((DWORD)	   GetA()) << 8)] << 8;
		BYTE r = CFastPlaneBlendTable::abyMulTable[src.GetR() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetR() + dstalpha8];
		BYTE g = CFastPlaneBlendTable::abyMulTable[src.GetG() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetG() + dstalpha8];
		BYTE b = CFastPlaneBlendTable::abyMulTable[src.GetB() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetB() + dstalpha8];
		SetRGB(r,g,b);
		return *this;
	}
	inline CFastPlaneRGB888&  dmul (const CFastPlaneRGB888 &src,BYTE alpha){
		if ( GetRGB()==src.GetRGB() ) return *this;
		const DWORD alpha8 = alpha << 8;
		// SetRGB()�o�R���Ɖ��̂��x�� '02/03/15	 by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8];
		//SetRGB(r,g,b);
		return *this;
	}
	inline CFastPlaneRGB888&  dmul (const CFastPlaneRGB888 &src,BYTE srcalpha,BYTE dstalpha){
		DWORD srcalpha8 = ((DWORD)srcalpha) << 8;
		DWORD dstalpha8 = ((DWORD)dstalpha) << 8;
		BYTE r = CFastPlaneBlendTable::abyMulTable[src.GetR() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetR() + dstalpha8];
		BYTE g = CFastPlaneBlendTable::abyMulTable[src.GetG() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetG() + dstalpha8];
		BYTE b = CFastPlaneBlendTable::abyMulTable[src.GetB() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetB() + dstalpha8];
		SetRGB(r,g,b);
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneRGB888& operator += (const X &src){
		WORD r = (WORD)GetR() + src.GetR();
		if (r>0xff) r=0xff;
		WORD g = (WORD)GetG() + src.GetG();
		if (g>0xff) g=0xff;
		WORD b = (WORD)GetB() + src.GetB();
		if (b>0xff) b=0xff;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�����Ƃ̉��Z�́A�œK�����ꂽ���[�`��
	//		RGB888 satulation add (C)���`
	CFastPlaneRGB888& operator += (const CFastPlaneRGB888& src){
		DWORD c, c0,c1;
		c0 = GetRGB();
		c1 = src.GetRGB();
		c = (((c0 & c1) << 1) + ((c0 ^ c1) & 0xfefefe)) & 0x1010100;
		c = ((c >> 8) + 0x7f7f7f) ^ 0x7f7f7f;
		SetRGB((c0 + c1 - c) | c);
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneRGB888& add (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() + r;
		if (r2>0xff) r2=0xff;
		WORD g2 = (WORD)GetG() + g;
		if (g2>0xff) g2=0xff;
		WORD b2 = (WORD)GetB() + b;
		if (b2>0xff) b2=0xff;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneRGB888& operator -= (const X &src){
		WORD r = (WORD)GetR() - src.GetR();
		if (r>0xff) r=0;	//	����A�I�[�o�[�t���[���Ă�
		WORD g = (WORD)GetG() - src.GetG();
		if (g>0xff) g=0;
		WORD b = (WORD)GetB() - src.GetB();
		if (b>0xff) b=0;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�����Ƃ̌��Z�́A�œK�����ꂽ���[�`��
	//		RGB888 satulation sub (C)���`
	CFastPlaneRGB888& operator -= (const CFastPlaneRGB888& src){
		DWORD c, c0,c1;
		c0 = GetRGB();
		c1 = src.GetRGB();
		c = (((~c0 & c1) << 1) + ((~c0 ^ c1) & 0xfefefe)) & 0x1010100;
		c = ((c >> 8) + 0x7f7f7f) ^ 0x7f7f7f;
		SetRGB((c0 | c) - (c1 | c));
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneRGB888& sub (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() - r;
		if (r2>0xff) r2=0;
		WORD g2 = (WORD)GetG() - g;
		if (g2>0xff) g2=0;
		WORD b2 = (WORD)GetB() - b;
		if (b2>0xff) b2=0;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}
	//	�e�a���̂��郿�t���v���[���Ƃ̖O�a���Z�A���Z
	inline CFastPlaneRGB888& operator += (const CFastPlaneARGB8888& src);
	inline CFastPlaneRGB888& operator -= (const CFastPlaneARGB8888& src);

private:
	BYTE _b,_g,_r;
};

class CFastPlaneBGR888 /* 24�r�b�gRGB 888 �t�o�[�W���� */{
public:
	BYTE GetR() const { return _r; }
	void SetR(BYTE r) { _r = r; }
	BYTE GetG() const { return _g; }
	void SetG(BYTE g) { _g = g; }
	BYTE GetB() const { return _b; }
	void SetB(BYTE b) { _b = b; }

	//	dummy�ł��p�ӂ��Ă����Ȃ��ƁA�d��Z�I�y���[�^�����܂������Ȃ�
	BYTE GetA() const { return 255; }
	void SetA(BYTE b) { }

	void SetRGB(BYTE r,BYTE g,BYTE b) { _r=r; _g=g; _b=b; }
	void SetRGB(DWORD rgb) {	*(WORD*)(&_r) = (WORD)(rgb & 0xffff);
								*((BYTE*)(&_r)+2) = (BYTE)(rgb >> 16);
	}
	DWORD GetRGB() const {
		return *(DWORD*)(&_r) & 0xffffff;
		//	������A�I�[�o�[�A�N�Z�X���Ă��邪�A
		//	���[�N����o�C�g�ł��邱�Ƃ͖����̂��H
		//	���������Ȃ�΁A��̂ɂ��Ă��΁A�Q�O�����炢�������ς��̂����D�D
		//	�˃T�[�t�F�[�X��������������h�b�g�Ŋm�ۂ���悤�ɕύX
//		return *(WORD*)(&_r) | (((DWORD)_b) << 16);
	}
	void	Clear() { *(WORD*)(&_r) = 0; *((BYTE*)(&_r)+2)=0; }

	//	�ϊ��q
	//		���蔲�����������ǁA�ϊ��q�A���݂��Ȃ���肸���ƃ}�V�ł���D�D
	template <class X>
	CFastPlaneBGR888& operator = (const X &src){
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		return *this;
	}
	//	�R�s�[�R���X�g���N�^�́A�ʓr�p��
	CFastPlaneBGR888& operator = (const CFastPlaneBGR888 &src){
		_r = src._r;
		_g = src._g;
		_b = src._b;
		return *this;
	}
	//	��Z�I�y���[�^
	template <class X>
	CFastPlaneBGR888& operator *= (const X &src){
	//	�]������alpha�l��blend����Ɖ���
		if ( GetRGB()==src.GetRGB() ) return *this;
		const DWORD alpha8 = (DWORD)src.GetA() << 8;
		//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8];
		//SetRGB(r,g,b);
		return *this;
	}
	//	��L�̏�Z�I�y���[�^�̒萔�o�[�W����
	CFastPlaneBGR888& operator *= (BYTE alpha){
		DWORD alpha8 = ((DWORD)alpha) << 8;
		BYTE r = CFastPlaneBlendTable::abyMulTable[ GetR() + alpha8];
		BYTE g = CFastPlaneBlendTable::abyMulTable[ GetG() + alpha8];
		BYTE b = CFastPlaneBlendTable::abyMulTable[ GetB() + alpha8];
		SetRGB(r,g,b);
		return *this;
	}
	//	�d��Z�I�y���[�^
	template <class X>
	CFastPlaneBGR888&  dmul (const X &src,BYTE alpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		if ( GetRGB()==src.GetRGB() ) return *this;
		const DWORD alpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[alpha + ((DWORD)src.GetA() << 8)] << 8;
		//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8];
		//SetRGB(r,g,b);
		return *this;
	}
	template <class X>
	CFastPlaneBGR888&  dmul (const X &src,BYTE srcalpha,BYTE dstalpha){
		DWORD srcalpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[srcalpha + ((DWORD)src.GetA() << 8)] << 8;
		DWORD dstalpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[dstalpha + ((DWORD)	  GetA() << 8)] << 8;
		BYTE r = CFastPlaneBlendTable::abyMulTable[src.GetR() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetR() + dstalpha8];
		BYTE g = CFastPlaneBlendTable::abyMulTable[src.GetG() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetG() + dstalpha8];
		BYTE b = CFastPlaneBlendTable::abyMulTable[src.GetB() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetB() + dstalpha8];
		SetRGB(r,g,b);
		return *this;
	}
	inline CFastPlaneBGR888&  dmul (const CFastPlaneBGR888 &src,BYTE alpha){
		if ( GetRGB()==src.GetRGB() ) return *this;
		const DWORD alpha8 = (DWORD)alpha << 8;
		// SetRGB()�o�R���Ɖ��̂��x�� '02/03/15	 by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8];
		//SetRGB(r,g,b);
		return *this;
	}
	inline CFastPlaneBGR888&  dmul (const CFastPlaneBGR888 &src,BYTE srcalpha,BYTE dstalpha){
		DWORD srcalpha8 = ((DWORD)srcalpha) << 8;
		DWORD dstalpha8 = ((DWORD)dstalpha) << 8;
		BYTE r = CFastPlaneBlendTable::abyMulTable[src.GetR() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetR() + dstalpha8];
		BYTE g = CFastPlaneBlendTable::abyMulTable[src.GetG() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetG() + dstalpha8];
		BYTE b = CFastPlaneBlendTable::abyMulTable[src.GetB() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetB() + dstalpha8];
		SetRGB(r,g,b);
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneBGR888& operator += (const X &src){
		WORD r = (WORD)GetR() + src.GetR();
		if (r>0xff) r=0xff;
		WORD g = (WORD)GetG() + src.GetG();
		if (g>0xff) g=0xff;
		WORD b = (WORD)GetB() + src.GetB();
		if (b>0xff) b=0xff;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�����Ƃ̉��Z�́A�œK�����ꂽ���[�`��
	//		RGB888 satulation add (C)���`
	CFastPlaneBGR888& operator += (const CFastPlaneBGR888& src){
		DWORD c, c0,c1;
		c0 = GetRGB();
		c1 = src.GetRGB();
		c = (((c0 & c1) << 1) + ((c0 ^ c1) & 0xfefefe)) & 0x1010100;
		c = ((c >> 8) + 0x7f7f7f) ^ 0x7f7f7f;
		SetRGB((c0 + c1 - c) | c);
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneBGR888& add (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() + r;
		if (r2>0xff) r2=0xff;
		WORD g2 = (WORD)GetG() + g;
		if (g2>0xff) g2=0xff;
		WORD b2 = (WORD)GetB() + b;
		if (b2>0xff) b2=0xff;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneBGR888& operator -= (const X &src){
		WORD r = (WORD)GetR() - src.GetR();
		if (r>0xff) r=0;	//	����A�I�[�o�[�t���[���Ă�
		WORD g = (WORD)GetG() - src.GetG();
		if (g>0xff) g=0;
		WORD b = (WORD)GetB() - src.GetB();
		if (b>0xff) b=0;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�����Ƃ̌��Z�́A�œK�����ꂽ���[�`��
	//		RGB888 satulation sub (C)���`
	CFastPlaneBGR888& operator -= (const CFastPlaneBGR888& src){
		DWORD c, c0,c1;
		c0 = GetRGB();
		c1 = src.GetRGB();
		c = (((~c0 & c1) << 1) + ((~c0 ^ c1) & 0xfefefe)) & 0x1010100;
		c = ((c >> 8) + 0x7f7f7f) ^ 0x7f7f7f;
		SetRGB((c0 | c) - (c1 | c));
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneBGR888& sub (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() - r;
		if (r2>0xff) r2=0;
		WORD g2 = (WORD)GetG() - g;
		if (g2>0xff) g2=0;
		WORD b2 = (WORD)GetB() - b;
		if (b2>0xff) b2=0;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}
	inline CFastPlaneBGR888& operator += (const CFastPlaneABGR8888& src);
	inline CFastPlaneBGR888& operator -= (const CFastPlaneABGR8888& src);

private:
	BYTE _r,_g,_b;	//	�t��
};

class CFastPlaneXRGB8888 /* 32�r�b�gXRGB 8888 X:=�S�~�r�b�g �ʏ킱��̂͂� */{
public:
	BYTE GetR() const { return _r; }
	void SetR(BYTE r) { _r = r; }
	BYTE GetG() const { return _g; }
	void SetG(BYTE g) { _g = g; }
	BYTE GetB() const { return _b; }
	void SetB(BYTE b) { _b = b; }

	BYTE GetA() const { return 255; }

	void SetRGB(BYTE r,BYTE g,BYTE b) { _r=r; _g=g; _b=b; _dummy=0; }
	//	a�̓S�~���߂Ȃ��ׂ�

	void SetRGB(DWORD rgb) { *(DWORD*)(&_b) = rgb; }	//	�S�~����Ă�������
	//DWORD GetRGB() const { return *(DWORD*)(&_b) & 0xffffff; }
		//	���O�̂��߁A
		//	��ʃo�C�g�ɃS�~�����グ�Ă���Ƃ����ň��̎��Ԃ��l�����Ă���
		//	�i�������A����ȃr�f�I�J�[�h�́A�͂�̂ĂȂ͂�I�R(`�D�L)�m�j
	//	�ˁ@�������ACFastPlaneXRGB8888��p�ӂ����Ƃ��ɂ��܂������Ȃ��̂�
	//		�K��Clear()���Ăяo���悤�ɂ��Ȃ��ƁD�D
	DWORD GetRGB() const { return *(DWORD*)(&_b); }
//	//	����ρA���v��BHDC�o�R��rendering�����Ƃ��́AMSB�N���A���Ă�������I
	void	Clear() { *(DWORD*)(&_b) = 0; }

	//	�ϊ��q
	//		���蔲�����������ǁA�ϊ��q�A���݂��Ȃ���肸���ƃ}�V�ł���D�D
	template <class X>
	CFastPlaneXRGB8888& operator = (const X &src){
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		return *this;
	}
	//	�R�s�[�R���X�g���N�^�́A�ʓr�p��
	inline CFastPlaneXRGB8888& operator = (const CFastPlaneXRGB8888 &src){
		*(DWORD*)(&_b) = *(DWORD*)(&src._b);
		return *this;
	}
	//	��Z�I�y���[�^
	template <class X>
	CFastPlaneXRGB8888& operator *= (const X &src){
	//	�]������alpha�l��blend����Ɖ���
		if ( (GetRGB()&0xffffff)==(src.GetRGB()&0xffffff) ) return *this;
		const DWORD alpha8 = (DWORD)src.GetA() << 8;
		// SetRGB()�o�R���Ɖ��̂��x�� '02/03/15	 by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8];
		_dummy = 0;
		return *this;
	}
	//	��L�̏�Z�I�y���[�^�̒萔�o�[�W����
	inline CFastPlaneXRGB8888& operator *= (BYTE alpha){
		const DWORD alpha8 = (DWORD)alpha << 8;
		// SetRGB()�o�R���Ɖ��̂��x�� '02/03/15	 by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[ GetB() + alpha8];
		_dummy = 0;
		return *this;
	}
	//	�d��Z�I�y���[�^
	template <class X>
	CFastPlaneXRGB8888&	 dmul (const X &src, BYTE alpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		if ( (GetRGB()&0xffffff)==(src.GetRGB()&0xffffff) ) return *this;
		const DWORD alpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[alpha + ((DWORD)src.GetA() << 8)] << 8;
		// SetRGB()�o�R���Ɖ��̂��x�� '02/03/15	 by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8];
		_dummy = 0;
		return *this;
	}
	template <class X>
	CFastPlaneXRGB8888&	 dmul (const X &src,BYTE srcalpha,BYTE dstalpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		const DWORD srcalpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[srcalpha + ((DWORD)src.GetA() << 8)] << 8;
		const DWORD dstalpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[dstalpha + ((DWORD)	GetA() << 8)] << 8;
		// SetRGB()�o�R���Ɖ��̂��x�� '02/03/15	 by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetR() + dstalpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetG() + dstalpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetB() + dstalpha8];
		_dummy = 0;
		return *this;
	}
	inline CFastPlaneXRGB8888&	dmul (const CFastPlaneXRGB8888 &src, BYTE alpha){
		if ( (GetRGB()&0xffffff)==(src.GetRGB()&0xffffff) ) return *this;
		const DWORD alpha8 = (DWORD)alpha << 8;
		// SetRGB()�o�R���Ɖ��̂��x�� '02/03/15	 by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8];
		_dummy = 0;
		return *this;
	}
	//	������́Adst = ��src�~src + ��dst�~dst
	inline CFastPlaneXRGB8888&	dmul (const CFastPlaneXRGB8888 &src,BYTE srcalpha,BYTE dstalpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		const DWORD srcalpha8 = (DWORD)srcalpha << 8;
		const DWORD dstalpha8 = (DWORD)dstalpha << 8;
		// SetRGB()�o�R���Ɖ��̂��x�� '02/03/15	 by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetR() + dstalpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetG() + dstalpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetB() + dstalpha8];
		_dummy = 0;
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneXRGB8888& operator += (const X &src){
		DWORD r = GetR() + src.GetR();
		if (r>0xff) r=0xff;
		DWORD g = GetG() + src.GetG();
		if (g>0xff) g=0xff;
		DWORD b = GetB() + src.GetB();
		if (b>0xff) b=0xff;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�����Ƃ̉��Z�́A�œK�����ꂽ���[�`��
	//		RGB888 satulation add (C)���`
	CFastPlaneXRGB8888& operator += (const CFastPlaneXRGB8888& src){
		DWORD c, c0,c1;
		c0 = GetRGB();
		c1 = src.GetRGB();
		c = (((c0 & c1) << 1) + ((c0 ^ c1) & 0xfefefe)) & 0x1010100;
		c = ((c >> 8) + 0x7f7f7f) ^ 0x7f7f7f;
		SetRGB((c0 + c1 - c) | c);
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneXRGB8888& add (BYTE r,BYTE g,BYTE b){
//		WORD r2 = (WORD)GetR() + r;
//		if (r2>0xff) r2=0xff;
//		WORD g2 = (WORD)GetG() + g;
//		if (g2>0xff) g2=0xff;
//		WORD b2 = (WORD)GetB() + b;
//		if (b2>0xff) b2=0xff;
//		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		DWORD c, c0,c1;
		c0 = GetRGB();
		c1 = (r<<16)|(g<<8)|b;
		c = (((c0 & c1) << 1) + ((c0 ^ c1) & 0xfefefe)) & 0x1010100;
		c = ((c >> 8) + 0x7f7f7f) ^ 0x7f7f7f;
		SetRGB((c0 + c1 - c) | c);
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneXRGB8888& operator -= (const X &src){
		DWORD r = GetR() - src.GetR();
		if (r>0xff) r=0;	//	����A�I�[�o�[�t���[���Ă�
		DWORD g = GetG() - src.GetG();
		if (g>0xff) g=0;
		DWORD b = GetB() - src.GetB();
		if (b>0xff) b=0;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�����Ƃ̌��Z�́A�œK�����ꂽ���[�`��
	//		RGB888 satulation sub (C)���`
	CFastPlaneXRGB8888& operator -= (const CFastPlaneXRGB8888& src){
		DWORD c, c0,c1;
		c0 = GetRGB();
		c1 = src.GetRGB();
		c = (((~c0 & c1) << 1) + ((~c0 ^ c1) & 0xfefefe)) & 0x1010100;
		c = ((c >> 8) + 0x7f7f7f) ^ 0x7f7f7f;
		SetRGB((c0 | c) - (c1 | c));
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneXRGB8888& sub (BYTE r,BYTE g,BYTE b){
//		WORD r2 = (WORD)GetR() - r;
//		if (r2>0xff) r2=0;
//		WORD g2 = (WORD)GetG() - g;
//		if (g2>0xff) g2=0;
//		WORD b2 = (WORD)GetB() - b;
//		if (b2>0xff) b2=0;
//		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		DWORD c, c0,c1;
		c0 = GetRGB();
		c1 = (r<<16)|(g<<8)|b;
		c = (((~c0 & c1) << 1) + ((~c0 ^ c1) & 0xfefefe)) & 0x1010100;
		c = ((c >> 8) + 0x7f7f7f) ^ 0x7f7f7f;
		SetRGB((c0 | c) - (c1 | c));
		return *this;
	}

	//	�e�a���̂��郿�t���v���[���Ƃ̖O�a���Z�A���Z
	inline CFastPlaneXRGB8888& operator += (const CFastPlaneARGB8888& src);
	inline CFastPlaneXRGB8888& operator -= (const CFastPlaneARGB8888& src);

private:
	BYTE _b,_g,_r,_dummy;
};

class CFastPlaneXBGR8888 /* 32�r�b�gXRGB 8888 X:=�S�~�r�b�g �t�o�[�W���� */{
public:
	BYTE GetR() const { return _r; }
	void SetR(BYTE r) { _r = r; }
	BYTE GetG() const { return _g; }
	void SetG(BYTE g) { _g = g; }
	BYTE GetB() const { return _b; }
	void SetB(BYTE b) { _b = b; }

	BYTE GetA() const { return 255; }

	void SetRGB(BYTE r,BYTE g,BYTE b) { _r=r; _g=g; _b=b; _dummy=0; }
	//	a�̓S�~���߂Ȃ��ׂ�

	void SetRGB(DWORD rgb) { *(DWORD*)(&_r) = rgb; }
//	DWORD GetRGB() const { return *(DWORD*)(&_r) & 0xffffff; }
		//	���O�̂��߁A
		//	��ʃo�C�g�ɃS�~�����グ�Ă���Ƃ����ň��̎��Ԃ��l�����Ă���
		//	�i�������A����ȃr�f�I�J�[�h�́A�͂�̂ĂȂ͂�I�R(`�D�L)�m�j
	DWORD GetRGB() const { return *(DWORD*)(&_r); }
	//	����ρA���v��BHDC�o�R��rendering�����Ƃ��́AMSB�N���A���Ă�������I
	void	Clear() { *(DWORD*)(&_r) = 0; }

	//	�ϊ��q
	//		���蔲�����������ǁA�ϊ��q�A���݂��Ȃ���肸���ƃ}�V�ł���D�D
	template <class X>
	CFastPlaneXBGR8888& operator = (const X &src){
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		return *this;
	}
	//	�R�s�[�R���X�g���N�^�́A�ʓr�p��
	CFastPlaneXBGR8888& operator = (const CFastPlaneXBGR8888 &src){
		*(DWORD*)(&_r) = *(DWORD*)(&src._r);
		return *this;
	}
	//	��Z�I�y���[�^
	template <class X>
	CFastPlaneXBGR8888& operator *= (const X &src){
	//	�]������alpha�l��blend����Ɖ���
		if ( (GetRGB()&0xffffff)==(src.GetRGB()&0xffffff) ) return *this;
		const DWORD alpha8 = (DWORD)src.GetA() << 8;
		//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8];
		_dummy = 0;
		//SetRGB(r,g,b);
		return *this;
	}
	//	��L�̏�Z�I�y���[�^�̒萔�o�[�W����
	CFastPlaneXBGR8888& operator *= (BYTE alpha){
		DWORD alpha8 = ((DWORD)alpha) << 8;
		// �����o�֐��o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[ GetB() + alpha8];
		_dummy = 0;
		//SetRGB(r,g,b);
		return *this;
	}
	//	�d��Z�I�y���[�^
	template <class X>
	CFastPlaneXBGR8888&	 dmul (const X &src,BYTE alpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		if ( (GetRGB()&0xffffff)==(src.GetRGB()&0xffffff) ) return *this;
		const DWORD alpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[alpha + (src.GetA() << 8)] << 8;
		//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8];
		_dummy = 0;
		//SetRGB(r,g,b);
		return *this;
	}
	template <class X>
	CFastPlaneXBGR8888&	 dmul (const X &src,BYTE srcalpha,BYTE dstalpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		DWORD srcalpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[srcalpha + ((DWORD)src.GetA() << 8)] << 8;
		DWORD dstalpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[dstalpha + ((DWORD)	   GetA() << 8)] << 8;
		// �����o�֐��o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetR() + dstalpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetG() + dstalpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetB() + dstalpha8];
		_dummy = 0;
		//SetRGB(r,g,b);
		return *this;
	}
	inline CFastPlaneXBGR8888&	dmul (const CFastPlaneXBGR8888 &src,BYTE alpha){
		if ( (GetRGB()&0xffffff)==(src.GetRGB()&0xffffff) ) return *this;
		const DWORD alpha8 = (DWORD)alpha << 8;
		//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8];
		_dummy = 0;
		//SetRGB(r,g,b);
		return *this;
	}
	//	������́Adst = ��src�~src + ��dst�~dst
	inline CFastPlaneXBGR8888&	dmul (const CFastPlaneXBGR8888 &src,BYTE srcalpha,BYTE dstalpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		DWORD srcalpha8 = (DWORD)srcalpha << 8;
		DWORD dstalpha8 = (DWORD)dstalpha << 8;
		// �����o�֐��o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetR() + dstalpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetG() + dstalpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetB() + dstalpha8];
		_dummy = 0;
		//SetRGB(r,g,b);
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneXBGR8888& operator += (const X &src){
		WORD r = (WORD)GetR() + src.GetR();
		if (r>0xff) r=0xff;
		WORD g = (WORD)GetG() + src.GetG();
		if (g>0xff) g=0xff;
		WORD b = (WORD)GetB() + src.GetB();
		if (b>0xff) b=0xff;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�����Ƃ̉��Z�́A�œK�����ꂽ���[�`��
	//		RGB888 satulation add (C)���`
	CFastPlaneXBGR8888& operator += (const CFastPlaneXBGR8888& src){
		DWORD c, c0,c1;
		c0 = GetRGB();
		c1 = src.GetRGB();
		c = (((c0 & c1) << 1) + ((c0 ^ c1) & 0xfefefe)) & 0x1010100;
		c = ((c >> 8) + 0x7f7f7f) ^ 0x7f7f7f;
//		SetRGB((c0 + c1 - c) | c);
		*(DWORD*)(&_r) = ((c0 + c1 - c) | c);
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneXBGR8888& add (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() + r;
		if (r2>0xff) r2=0xff;
		WORD g2 = (WORD)GetG() + g;
		if (g2>0xff) g2=0xff;
		WORD b2 = (WORD)GetB() + b;
		if (b2>0xff) b2=0xff;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneXBGR8888& operator -= (const X &src){
		WORD r = (WORD)GetR() - src.GetR();
		if (r>0xff) r=0;	//	����A�I�[�o�[�t���[���Ă�
		WORD g = (WORD)GetG() - src.GetG();
		if (g>0xff) g=0;
		WORD b = (WORD)GetB() - src.GetB();
		if (b>0xff) b=0;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�����Ƃ̌��Z�́A�œK�����ꂽ���[�`��
	//		RGB888 satulation sub (C)���`
	CFastPlaneXBGR8888& operator -= (const CFastPlaneXBGR8888& src){
		DWORD c, c0,c1;
		c0 = GetRGB();
		c1 = src.GetRGB();
		c = (((~c0 & c1) << 1) + ((~c0 ^ c1) & 0xfefefe)) & 0x1010100;
		c = ((c >> 8) + 0x7f7f7f) ^ 0x7f7f7f;
//		SetRGB((c0 | c) - (c1 | c));
		*(DWORD*)(&_r) = ((c0 | c) - (c1 | c));
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneXBGR8888& sub (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() - r;
		if (r2>0xff) r2=0;
		WORD g2 = (WORD)GetG() - g;
		if (g2>0xff) g2=0;
		WORD b2 = (WORD)GetB() - b;
		if (b2>0xff) b2=0;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}

	//	�e�a���̂��郿�t���v���[���Ƃ̖O�a���Z�A���Z
	inline CFastPlaneXBGR8888& operator += (const CFastPlaneABGR8888& src);
	inline CFastPlaneXBGR8888& operator -= (const CFastPlaneABGR8888& src);

private:
	BYTE _r,_g,_b,_dummy;	//	�t�Ȃ̂�
};

class CFastPlaneARGB8888 /* 32�r�b�gARGB 8888 A:=Alpha */{
public:
	BYTE GetR() const { return _r; }
	void SetR(BYTE r) { _r = r; }
	BYTE GetG() const { return _g; }
	void SetG(BYTE g) { _g = g; }
	BYTE GetB() const { return _b; }
	void SetB(BYTE b) { _b = b; }

	//		�� 255�ɂ����ق����ǂ��̂��낤���H
	void SetRGB(BYTE r,BYTE g,BYTE b) { _r=r; _g=g; _b=b; _a=255; }

	//		���̃Z�b�g�́Anative�Ȃ̂�alpha�l�́A��������B
	void SetRGB(DWORD rgb) { *(DWORD*)(&_b) = rgb; }
	DWORD GetRGB() const { return *(DWORD*)(&_b) & 0xffffff; }
	DWORD GetRGBA() const { return *(DWORD*)&_b; }

	BYTE GetA() const { return _a; }
	void SetA(BYTE a) { _a = a; }

	void	Clear() { *(DWORD*)(&_b) = 0; }
	
	//	�ϊ��q
	//		���蔲�����������ǁA�ϊ��q�A���݂��Ȃ���肸���ƃ}�V�ł���D�D
	template <class X>
	CFastPlaneARGB8888& operator = (const X &src){
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		// SetRGB�ŃZ�b�g����邩�疳�� by ENRA
		//_a = 255;	//	����T�|�[�g�v���[������̃R�s�[�Ȃ̂ŁA����ݒ肵�Ă����˂΂Ȃ�Ȃ��I
		return *this;
	}
	//	�R�s�[�R���X�g���N�^�́A�ʓr�p��
	//		�ˁ@���̃R�s�[�ɂ����āA���l���R�s�[����邱�Ƃɗ��ӂ���
	CFastPlaneARGB8888& operator = (const CFastPlaneARGB8888 &src){
		*(DWORD*)(&_b) = *(DWORD*)(&src._b);
		return *this;
	}
	inline CFastPlaneARGB8888& operator = (const CFastPlaneARGB4565 &src);
	inline CFastPlaneARGB8888& operator = (const CFastPlaneARGB4555 &src);
	inline CFastPlaneARGB8888& operator = (const CFastPlaneABGR8888 &src);

	//	���˃��T�[�t�F�[�X�ցA���ȊO�̂݃R�s�[
	CFastPlaneARGB8888& CopyWithoutAlpha(const CFastPlaneARGB8888 &src){
		*(WORD*)(&_b) = *(WORD*)(&src._b);
		_r = src._r;
		//	��DIBSection����VideoMemory��̃T�[�t�F�[�X���ƃ}�Y�[(߄D�)
//		*(DWORD*)(&_b) =
//			(*(DWORD*)(&src._b) & 0xffffff) | (*(DWORD*)(&_b) & 0xff000000);
		return *this;
	}
	//	���l�̔��]
	CFastPlaneARGB8888& FlushAlpha(const CFastPlaneARGB8888 &src){
		_a = ~src._a;
		return *this;
	}

	//	��Z�I�y���[�^
	template <class X>
	CFastPlaneARGB8888& operator *= (const X &src){
	//	�]������alpha�l��blend����Ɖ���
		if ( (GetRGB()&0xffffff)==(src.GetRGB()&0xffffff) ) return *this;
		const DWORD alpha8 = (DWORD)src.GetA() << 8;
		//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8];
		_a = 255;
		//SetRGB(r,g,b);
		return *this;
	}
	//	��L�̏�Z�I�y���[�^�̒萔�o�[�W����
	CFastPlaneARGB8888& operator *= (BYTE alpha){
		/*
		DWORD alpha8 = ((DWORD)alpha) << 8;
		//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[ GetB() + alpha8];
		_a = 255;
		//SetRGB(r,g,b);
		*/
		//	�P�Ƀ����������邩�H
		DWORD alpha8 = ((DWORD)alpha) << 8;
		SetA( CFastPlaneBlendTable::abyMulTable[GetA() + alpha8] );
		return *this;
	}
	//	�d��Z�I�y���[�^
	template <class X>
	CFastPlaneARGB8888&	 dmul (const X &src,BYTE alpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		if ( (GetRGB()&0xffffff)==(src.GetRGB()&0xffffff) ) return *this;
		const DWORD alpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[(DWORD)alpha + ((DWORD)src.GetA() << 8)] << 8;
		//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8];
		_a = 255;
		//SetRGB(r,g,b);
		return *this;
	}
	template <class X>
	CFastPlaneARGB8888&	 dmul (const X &src,BYTE srcalpha,BYTE dstalpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		DWORD srcalpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[(DWORD)srcalpha + ((DWORD)src.GetA() << 8)] << 8;
		DWORD dstalpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[(DWORD)dstalpha + ((DWORD)	 GetA() << 8)] << 8;
		//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetR() + dstalpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetG() + dstalpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetB() + dstalpha8];
		_a = 255;
		//SetRGB(r,g,b);
		return *this;
	}

	CFastPlaneARGB8888&	 MulAlphaToAlpha(const CFastPlaneARGB8888&src){
/*
	PhotoShop�I���C������
		�V������  = ��src + (1-��src)�~��dst
		�V����RGB =	 (RGBSrc�~��src�@�{�@RGBDst�~��dst)/(��src + ��dst)
		�ˁ@������A��ɕ�����ƁA
		//	��src' = ��src / (��src + ��dst)
		//	��dst' = ��dst / (��src + ��dst)
		�Ƃ��āARGBSrc�~��src' + RGBDst�~��dst'
*/
		DWORD nAlphaSrc = src.GetA();
		DWORD nAlphaDst = GetA();
		DWORD nNewBeta = (CFastPlaneBlendTable::abyMulTable[(255-(nAlphaSrc)) + (nAlphaDst << 8)]);
		DWORD nNewAlpha = nAlphaSrc + nNewBeta;
		//	��src' = ��src / (��src + ��dst)
		//	��dst' = ��dst / (��src + ��dst)
		SetA((BYTE)nNewAlpha);

		int nAlpha = nAlphaSrc + nNewBeta;
		if (nAlpha != 0) {
		//	�������[���̂Ƃ��́ARGB���v�Z����ɒl���Ȃ�
			DWORD nAlphaSrcD8 = ((nAlphaSrc*255) / nAlpha) << 8;
			DWORD nAlphaDstD8 = ((nNewBeta*255) / nAlpha) << 8;

			//	��̃p�����[�^�Ńu�����h����
			//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
			_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + nAlphaSrcD8] + CFastPlaneBlendTable::abyMulTable[ GetR() + nAlphaDstD8];
			_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + nAlphaSrcD8] + CFastPlaneBlendTable::abyMulTable[ GetG() + nAlphaDstD8];
			_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + nAlphaSrcD8] + CFastPlaneBlendTable::abyMulTable[ GetB() + nAlphaDstD8];
			_a = 255;
			//SetRGB(r,g,b);
		}
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	//		�ˁ@���̖O�a���Z�ɂ����āA���l�͖�������邱�Ƃɗ��ӂ���
	template <class X>
	CFastPlaneARGB8888& operator += (const X &src){
		WORD r = (WORD)GetR() + src.GetR();
		if (r>0xff) r=0xff;
		WORD g = (WORD)GetG() + src.GetG();
		if (g>0xff) g=0xff;
		WORD b = (WORD)GetB() + src.GetB();
		if (b>0xff) b=0xff;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneARGB8888& add (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() + r;
		if (r2>0xff) r2=0xff;
		WORD g2 = (WORD)GetG() + g;
		if (g2>0xff) g2=0xff;
		WORD b2 = (WORD)GetB() + b;
		if (b2>0xff) b2=0xff;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneARGB8888& operator -= (const X &src){
		WORD r = (WORD)GetR() - src.GetR();
		if (r>0xff) r=0;	//	����A�I�[�o�[�t���[���Ă�
		WORD g = (WORD)GetG() - src.GetG();
		if (g>0xff) g=0;
		WORD b = (WORD)GetB() - src.GetB();
		if (b>0xff) b=0;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneARGB8888& sub (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() - r;
		if (r2>0xff) r2=0;
		WORD g2 = (WORD)GetG() - g;
		if (g2>0xff) g2=0;
		WORD b2 = (WORD)GetB() - b;
		if (b2>0xff) b2=0;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}

private:
	BYTE _b,_g,_r,_a;
};

class CFastPlaneABGR8888 /* 32�r�b�gABGR 8888 A:=Alpha */{
public:
	BYTE GetR() const { return _r; }
	void SetR(BYTE r) { _r = r; }
	BYTE GetG() const { return _g; }
	void SetG(BYTE g) { _g = g; }
	BYTE GetB() const { return _b; }
	void SetB(BYTE b) { _b = b; }

	//		�� 255�ɂ����ق����ǂ��̂��낤���H
	void SetRGB(BYTE r,BYTE g,BYTE b) { _r=r; _g=g; _b=b; _a=255; }

	//		���̃Z�b�g�́Anative�Ȃ̂�alpha�l�́A��������B
	void SetRGB(DWORD rgb) { *(DWORD*)(&_r) = rgb; }
	DWORD GetRGB() const { return *(DWORD*)(&_r) & 0xffffff; }
	DWORD GetRGBA() const { return *(DWORD*)&_r; }

	BYTE GetA() const { return _a; }
	void SetA(BYTE a) { _a = a; }

	void	Clear() { *(DWORD*)(&_r) = 0; }
	
	//	�ϊ��q
	//		���蔲�����������ǁA�ϊ��q�A���݂��Ȃ���肸���ƃ}�V�ł���D�D
	template <class X>
	CFastPlaneABGR8888& operator = (const X &src){
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		// SetRGB�ŃZ�b�g����邩�疳�� by ENRA
		//_a = 255;	//	����T�|�[�g�v���[������̃R�s�[�Ȃ̂ŁA����ݒ肵�Ă����˂΂Ȃ�Ȃ��I
		return *this;
	}
	//	�R�s�[�R���X�g���N�^�́A�ʓr�p��
	//		�ˁ@���̃R�s�[�ɂ����āA���l���R�s�[����邱�Ƃɗ��ӂ���
	CFastPlaneABGR8888& operator = (const CFastPlaneABGR8888 &src){
		*(DWORD*)(&_r) = *(DWORD*)(&src._r);
		return *this;
	}
	inline CFastPlaneABGR8888& operator = (const CFastPlaneARGB4565 &src);
	inline CFastPlaneABGR8888& operator = (const CFastPlaneARGB4555 &src);
	inline CFastPlaneABGR8888& operator = (const CFastPlaneARGB8888 &src);

	//	���˃��T�[�t�F�[�X�ցA���ȊO�̂݃R�s�[
	CFastPlaneABGR8888& CopyWithoutAlpha(const CFastPlaneABGR8888 &src){
		*(DWORD*)(&_r) =
			(*(DWORD*)(&src._r) & 0xffffff) | (*(DWORD*)(&_r) & 0xff000000);
		return *this;
	}
	//	���l�̔��]
	CFastPlaneABGR8888& FlushAlpha(const CFastPlaneABGR8888 &src){
		_a = ~ src._a;
		return *this;
	}

	//	��Z�I�y���[�^
	template <class X>
	CFastPlaneABGR8888& operator *= (const X &src){
	//	�]������alpha�l��blend����Ɖ���
		if ( (GetRGB()&0xffffff)==(src.GetRGB()&0xffffff) ) return *this;
		const DWORD alpha8 = (DWORD)src.GetA() << 8;
		//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8];
		_a = 255;
		//SetRGB(r,g,b);
		return *this;
	}
	//	��L�̏�Z�I�y���[�^�̒萔�o�[�W����
	CFastPlaneABGR8888& operator *= (BYTE alpha){
		/*
		DWORD alpha8 = ((DWORD)alpha) << 8;
		//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[ GetB() + alpha8];
		_a = 255;
		//SetRGB(r,g,b);
		*/

		//	�P�Ƀ����������邩�H
		DWORD alpha8 = ((DWORD)alpha) << 8;
		SetA( CFastPlaneBlendTable::abyMulTable[GetA() + alpha8] );
		return *this;
	}
	//	�d��Z�I�y���[�^
	template <class X>
	CFastPlaneABGR8888&	 dmul (const X &src,BYTE alpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		if ( (GetRGB()&0xffffff)==(src.GetRGB()&0xffffff) ) return *this;
		const DWORD alpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[(DWORD)alpha + ((DWORD)src.GetA() << 8)] << 8;
		//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetR() + alpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetG() + alpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + alpha8] + CFastPlaneBlendTable::abyMulTable2[ GetB() + alpha8];
		_a = 255;
		//SetRGB(r,g,b);
		return *this;
	}
	template <class X>
	CFastPlaneABGR8888&	 dmul (const X &src,BYTE srcalpha,BYTE dstalpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		DWORD srcalpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[(DWORD)srcalpha + ((DWORD)src.GetA() << 8)] << 8;
		DWORD dstalpha8 = (DWORD)CFastPlaneBlendTable::abyMulTable[(DWORD)dstalpha + ((DWORD)	   GetA() << 8)] << 8;
		//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
		_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetR() + dstalpha8];
		_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetG() + dstalpha8];
		_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + srcalpha8] + CFastPlaneBlendTable::abyMulTable[ GetB() + dstalpha8];
		_a = 255;
		//SetRGB(r,g,b);
		return *this;
	}

	CFastPlaneABGR8888&	 MulAlphaToAlpha(const CFastPlaneABGR8888&src){
/*
	PhotoShop�I���C������
		�V������  = ��src + (1-��src)�~��dst
		�V����RGB =	 (RGBSrc�~��src�@�{�@RGBDst�~��dst)/(��src + ��dst)
		�ˁ@������A��ɕ�����ƁA
		//	��src' = ��src / (��src + ��dst)
		//	��dst' = ��dst / (��src + ��dst)
		�Ƃ��āARGBSrc�~��src' + RGBDst�~��dst'
*/
		DWORD nAlphaSrc = src.GetA();
		DWORD nAlphaDst = GetA();
		DWORD nNewBeta = (CFastPlaneBlendTable::abyMulTable[(255-(nAlphaSrc)) + (nAlphaDst << 8)]);
		DWORD nNewAlpha = nAlphaSrc + nNewBeta;
		//	��src' = ��src / (��src + ��dst)
		//	��dst' = ��dst / (��src + ��dst)
		SetA((BYTE)nNewAlpha);

		int nAlpha = nAlphaSrc + nNewBeta;
		if (nAlpha != 0) {
		//	�������[���̂Ƃ��́ARGB���v�Z����ɒl���Ȃ�
			DWORD nAlphaSrcD8 = ((nAlphaSrc*255) / nAlpha) << 8;
			DWORD nAlphaDstD8 = ((nNewBeta*255) / nAlpha) << 8;

			//	��̃p�����[�^�Ńu�����h����
			//	SetRGB()�o�R���Ɖ��̂��x�� '02/03/15  by ENRA
			_r = CFastPlaneBlendTable::abyMulTable[src.GetR() + nAlphaSrcD8] + CFastPlaneBlendTable::abyMulTable[ GetR() + nAlphaDstD8];
			_g = CFastPlaneBlendTable::abyMulTable[src.GetG() + nAlphaSrcD8] + CFastPlaneBlendTable::abyMulTable[ GetG() + nAlphaDstD8];
			_b = CFastPlaneBlendTable::abyMulTable[src.GetB() + nAlphaSrcD8] + CFastPlaneBlendTable::abyMulTable[ GetB() + nAlphaDstD8];
			_a = 255;
			//SetRGB(r,g,b);
		}
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	//		�ˁ@���̖O�a���Z�ɂ����āA���l�͖�������邱�Ƃɗ��ӂ���
	template <class X>
	CFastPlaneABGR8888& operator += (const X &src){
		WORD r = (WORD)GetR() + src.GetR();
		if (r>0xff) r=0xff;
		WORD g = (WORD)GetG() + src.GetG();
		if (g>0xff) g=0xff;
		WORD b = (WORD)GetB() + src.GetB();
		if (b>0xff) b=0xff;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneABGR8888& add (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() + r;
		if (r2>0xff) r2=0xff;
		WORD g2 = (WORD)GetG() + g;
		if (g2>0xff) g2=0xff;
		WORD b2 = (WORD)GetB() + b;
		if (b2>0xff) b2=0xff;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}

	//	�O�a���Z�I�y���[�^
	template <class X>
	CFastPlaneABGR8888& operator -= (const X &src){
		WORD r = (WORD)GetR() - src.GetR();
		if (r>0xff) r=0;	//	����A�I�[�o�[�t���[���Ă�
		WORD g = (WORD)GetG() - src.GetG();
		if (g>0xff) g=0;
		WORD b = (WORD)GetB() - src.GetB();
		if (b>0xff) b=0;
		SetRGB((BYTE)r,(BYTE)g,(BYTE)b);
		return *this;
	}
	//	�萔�O�a���Z
	CFastPlaneABGR8888& sub (BYTE r,BYTE g,BYTE b){
		WORD r2 = (WORD)GetR() - r;
		if (r2>0xff) r2=0;
		WORD g2 = (WORD)GetG() - g;
		if (g2>0xff) g2=0;
		WORD b2 = (WORD)GetB() - b;
		if (b2>0xff) b2=0;
		SetRGB((BYTE)r2,(BYTE)g2,(BYTE)b2);
		return *this;
	}

private:
	BYTE _r,_g,_b,_a;	//	���ꂪARGB8888�Ƌt���ɂȂ邾��
};

//////////////////////////////////////////////////////////////////////////////
//	�{�T�C�Y�ł̃R�s�[�̂��߂̃N���X
//////////////////////////////////////////////////////////////////////////////

//	�{�T�C�Y�̃R�s�[���T�|�[�g���Ă���RGB565
class CFastPlaneRGB565565 {
public:
	void SetRGB(DWORD rgb) { _rgbrgb = rgb; }
	DWORD GetRGB() const { return _rgbrgb; }
	//	WORD�œ�ݒ�
	void SetRGBWORD(WORD rgb) { _rgbrgb = rgb + (rgb<<16); }

	//	getter�́A���ʂ݂̂�Ԃ��Bsetter�́A��ʂɂ��Z�b�g����
	BYTE GetR() const { return (BYTE)((_rgbrgb & 0xf800) >> (11-3)); }
	void SetR(BYTE r) { _rgbrgb = (_rgbrgb & ~0xfffff800) | (((DWORD)(r&0xf8))<<(11-3)); _rgbrgb += _rgbrgb<<16; }
	BYTE GetG() const { return (BYTE)((_rgbrgb & 0x07e0) >> (6-3)); }
	void SetG(BYTE g) { _rgbrgb = (_rgbrgb & ~0xffff07e0) | (((DWORD)(g&0xfc))<<(6-3)); _rgbrgb += _rgbrgb<<16; }
	BYTE GetB() const { return (BYTE)((_rgbrgb & 0x001f) << 3); }
	void SetB(BYTE b) { _rgbrgb = (_rgbrgb & ~0xffff001f) | (b >> 3); _rgbrgb += _rgbrgb<<16; }

	//	�R�s�[�R���X�g���N�^�́A�ʓr�p��
	CFastPlaneRGB565565& operator = (const CFastPlaneRGB565565 &src){
		_rgbrgb = src._rgbrgb;
		return *this;
	}

	//	---- �~�����̂́A����I�ȋ@�\�̂�

	//	�����Ƃ̉��Z�́A�œK�����ꂽ���[�`��
	//		RGB565 satulation add (C)���` & ��˂��炨
	CFastPlaneRGB565565& operator += (const CFastPlaneRGB565565& src){
		DWORD c, c0,c1;
		c0 = GetRGB();
		c1 = src.GetRGB();

		c = ((c0 & c1) + (((c0 ^ c1) & 0xf7def7de) >> 1)) & 0x84108410;
		//	�����̕����A(C)��˂��炨
		c = (((((long)(c + 0x3df7bdf0)) >> 5) & 0xfbff7bff) + 0x2004200)^0x3ef7bef;

/*
[DWORD��]

							R----G-----B---- R----G-----B----
	add eax, 0x3df7bdf0		RGggggBbbbbbRrrr rGggggBbbbbb----
	sar eax, 5				RRRRRRGggggBbbbb bRrrrrGggggBbbbb b
	and eax, 0xfbff7bff		RRRRR-GggggBbbbb -Rrrr-GggggBbbbb
	add eax, 0x02004200		RRRRRGgggggBbbbb RrrrrGgggggBbbbb
	xor eax, 0x03ef7bef		RRRRRGGGGGGBBBBB RRRRRGGGGGGBBBBB
*/

		SetRGB((c0 + c1 - c) | c);
		return *this;
	}

	//	�����Ƃ̉��Z�́A�œK�����ꂽ���[�`��
	//		RGB565565 satulation sub (C)���` & ��˂��炨
	CFastPlaneRGB565565& operator -= (const CFastPlaneRGB565565& src){
		DWORD c, c0,c1;
		c0 = GetRGB();
		c1 = src.GetRGB();

		c = ((~c0 & c1) + (((~c0 ^ c1) & 0xf7def7de) >> 1)) & 0x84108410;
		//	�����̕����A(C)��˂��炨
		c = (((((long)(c + 0x3df7bdf0)) >> 5) & 0xfbff7bff) + 0x2004200)^0x3ef7bef;

		SetRGB((c0 | c) - (c1 | c));
		return *this;
	}

	CFastPlaneRGB565565& operator *= (BYTE alpha){
		DWORD alpha16 = ((DWORD)(alpha&0xf0)) << (4+8); // 4 bit�̂ݐ���
		_rgbrgb =		 CFastPlaneBlendTable::awdMulTableRGB565[(_rgbrgb & 0xffff) + alpha16]
			+	 ((DWORD)CFastPlaneBlendTable::awdMulTableRGB565[(_rgbrgb >> 16)	+ alpha16] << 16);
		return *this;
	}

private:
	DWORD _rgbrgb;
};

//	�{�T�C�Y�̃R�s�[���T�|�[�g���Ă���RGB555
class CFastPlaneRGB555555{
public:
	void SetRGB(DWORD rgb) { _rgbrgb = rgb; }
	DWORD GetRGB() const { return _rgbrgb; }
	//	WORD�œ�ݒ�
	void SetRGBWORD(WORD rgb) { _rgbrgb = rgb + (rgb<<16); }

	//	getter�́A���ʂ݂̂�Ԃ��Bsetter�́A��ʂɂ��Z�b�g����
	BYTE GetR() const { return (BYTE)((_rgbrgb & 0x7c00) >> (10-3)); }
	void SetR(BYTE r) { _rgbrgb = (_rgbrgb & ~(0xffff7c00+0x8000)) | ((DWORD)(r&0xf8)<<(10-3)); _rgbrgb += _rgbrgb<<16; }
	BYTE GetG() const { return (BYTE)((_rgbrgb & 0x03e0) >> (5-3)); }
	void SetG(BYTE g) { _rgbrgb = (_rgbrgb & ~(0xffff03e0+0x8000)) | ((DWORD)(g&0xfc)<<(5-3)); _rgbrgb += _rgbrgb<<16; }
	BYTE GetB() const { return (BYTE)((_rgbrgb & 0x001f) << 3); }
	void SetB(BYTE b) { _rgbrgb = (_rgbrgb & ~(0xffff001f+0x8000)) | (b >> 3); _rgbrgb += _rgbrgb<<16; }

	//	�R�s�[�R���X�g���N�^�́A�ʓr�p��
	CFastPlaneRGB555555& operator = (const CFastPlaneRGB555555 &src){
		_rgbrgb = src._rgbrgb;
		return *this;
	}
	//	�����Ƃ̉��Z�́A�œK�����ꂽ���[�`��
	//		RGB555555 satulation add (C)���` & ��˂��炨
	CFastPlaneRGB555555& operator += (const CFastPlaneRGB555555& src){
		DWORD c, c0,c1;
		c0 = GetRGB();
		c1 = src.GetRGB();
		c = (((c0 & c1) << 1) + ((c0 ^ c1) & 0x7bde7bde)) & 0x84208420;
		c = ((c >> 5) + 0x3def3def) ^ 0x3def3def;
		SetRGB((c0 + c1 - c) | c);
		return *this;
	}
	//		RGB555 satulation sub (C)���`
	CFastPlaneRGB555555& operator -= (const CFastPlaneRGB555555& src){
		DWORD c, c0,c1;
		c0 = GetRGB();
		c1 = src.GetRGB();
		c = (((~c0 & c1) << 1) + ((~c0 ^ c1) & 0x7bde7bde)) & 0x84208420;
		c = (( c >> 5) + 0x3def3def) ^ 0x3def3def;
		SetRGB((c0 | c) - (c1 | c));
		return *this;
	}
	CFastPlaneRGB555555& operator *= (BYTE alpha){
		DWORD alpha16 = ((DWORD)(alpha&0xf0)) << (4+8); // 4 bit�̂ݐ���
		_rgbrgb =		 CFastPlaneBlendTable::awdMulTableRGB555[(_rgbrgb & 0xffff) + alpha16]
			+	 ((DWORD)CFastPlaneBlendTable::awdMulTableRGB555[(_rgbrgb >> 16)	+ alpha16] << 16);
		return *this;
	}

private:
	DWORD _rgbrgb;
};

//////////////////////////////////////////////////////////////////////////////

//	���ꉻ���ꂽ�A��e���v���[�g�^�C�v�̕ϊ��q

	inline CFastPlaneBytePal& CFastPlaneBytePal::operator = (const CFastPlaneRGB555 &src){
/*
		//	RGB�e�Q�r�b�g����A�V�X�e���J���[�ւ̕ϊ��e�[�u��
		//	�{���́A������RGB555�ɑ΂��ėp�ӂ���Ηǂ��̂���
		//	�ʓ|�Ȃ̂ŁA��������ŗǂ��O�O�G
		//	����Ӗ��A����͋C�x�߂ɉ߂��Ȃ��B
		static BYTE abyTable[4*4*4] = {
			0,0,  1,249,		// RGB 000,100,200,300
			0,0,  1,249,		// RGB 010,110,210,310
			2,2,  3,251,		// RGB 020,120,220,320
			2,2,251,251,		// RGB 030,130,230,330
			0,0,  1,249,		// RGB 001,101,201,301
			0,0,  1,  9,		// RGB 011,111,211,311
			2,2,  3,251,		// RGB 021,121,221,321
			2,2,251,251,		// RGB 031,131,231,331
			4,4,  5,  5,		// RGB 002,102,202,302
			4,4,  5,253,		// RGB 012,112,212,312
			6,6,  7,255,		// RGB 022,122,222,322
			254,254,255,255,	// RGB 032,132,232,332
			255,252,253,253,	// RGB 003,103,203,303
			4,4,5,253,			// RGB 013,113,213,313
			254,254,255,255,	// RGB 023,123,223,323
			254,254,255,255,	// RGB 033,133,233,333
		};
		SetRGB(abyTable[
			((src.GetR()&0xc0) >> 6) |
			((src.GetG()&0xc0) >> 4) |
			((src.GetB()&0xc0) >> 2)
		]);
*/
		//	�ϊ��e�[�u���ɂ��܂����I
		SetRGB(CFastPlaneBlendTable::abyConvertTable555[src.GetRGB()]);
		return *this;
	}

//	for RGB565

	//	�e�a���̂���v���[���̂��߂�alpha�������ꂽ�]��
	inline CFastPlaneRGB565& CFastPlaneRGB565::operator *= (const CFastPlaneARGB4565 &src)
	{
	//	�]������alpha�l��blend����Ɖ���
		DWORD alpha16e	= ((DWORD)src._a) << 16; // 4 bit
		DWORD alpha16i = alpha16e ^ 0xf0000; // �� 4 bit�𔽓]
		_rgb =	 CFastPlaneBlendTable::awdMulTableRGB565[src._rgb + alpha16e] +
				 CFastPlaneBlendTable::awdMulTableRGB565[	 _rgb + alpha16i];
		//	�����ꂾ���Ńu�����h��������񂩁[�A�A�������ȁ[�O�O�G
		return *this;
	}
	inline CFastPlaneRGB565& CFastPlaneRGB565::operator *= (BYTE alpha)
	{
		DWORD alpha16 = ((DWORD)(alpha&0xe0)) << (4+8); // 4 bit�̂ݐ���
		_rgb =	 CFastPlaneBlendTable::awdMulTableRGB565[_rgb + alpha16];
		//	�����ꂾ���Ńu�����h��������񂩁[�A�A�������ȁ[�O�O�G
		return *this;
	}
	inline CFastPlaneRGB565&  CFastPlaneRGB565::dmul (const CFastPlaneARGB4565 &src,BYTE alpha)
	{
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		DWORD alpha16 = CFastPlaneBlendTable::abyMulTable[(DWORD)(src._a << 4) + (alpha << 8)] & 0xf0;
		DWORD alpha16e = (alpha16) << (8+4);   // 4 bit
		DWORD alpha16i = alpha16e ^ 0xf0000;   // �� 4 bit�𔽓]
		_rgb =
			 CFastPlaneBlendTable::awdMulTableRGB565[src._rgb + alpha16e] +
			 CFastPlaneBlendTable::awdMulTableRGB565[	 _rgb + alpha16i];
		return *this;
	}
	//	�����݂̃v���[������̃R�s�[
	inline CFastPlaneARGB4565& CFastPlaneARGB4565::operator = (const CFastPlaneARGB8888 &src)
	{
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		SetA(src.GetA());
		return *this;
	}
	inline CFastPlaneARGB4565& CFastPlaneARGB4565::operator = (const CFastPlaneABGR8888 &src)
	{
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		SetA(src.GetA());
		return *this;
	}
	inline CFastPlaneARGB4565& CFastPlaneARGB4565::operator = (const CFastPlaneARGB4555 &src)
	{
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		SetA(src.GetA());
		return *this;
	}

//	for RGB555

	//	�e�a���̂���v���[���̂��߂�alpha�������ꂽ�]��
	inline CFastPlaneRGB555& CFastPlaneRGB555::operator *= (const CFastPlaneARGB4555 &src)
	{
	//	�]������alpha�l��blend����Ɖ���
		DWORD alpha16e = ((DWORD)src._a) << 16; // 4 bit
		DWORD alpha16i = alpha16e ^ 0xf0000; // �� 4 bit�𔽓]
		_rgb = CFastPlaneBlendTable::awdMulTableRGB555[src._rgb + alpha16e] +
			   CFastPlaneBlendTable::awdMulTableRGB555[	   _rgb + alpha16i];
		//	�����ꂾ���Ńu�����h��������񂩁[�A�A�������ȁ[�O�O�G
		return *this;
	}
	inline CFastPlaneRGB555& CFastPlaneRGB555::operator *= (BYTE alpha)
	{
		DWORD alpha16 = ((DWORD)(alpha&0xf0)) << (4+8); // 4 bit�̂ݐ���
		_rgb =	 CFastPlaneBlendTable::awdMulTableRGB555[_rgb + alpha16];
		//	�����ꂾ���Ńu�����h��������񂩁[�A�A�������ȁ[�O�O�G
		// _rgb = rgb & 0x7fff; // �ꉞ�A�ŏ�ʂ��}�X�N���ɂ�D�D
		return *this;
	}
	inline CFastPlaneRGB555&  CFastPlaneRGB555::dmul (const CFastPlaneARGB4555 &src,BYTE alpha)
	{
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		DWORD alpha16 = CFastPlaneBlendTable::abyMulTable[(DWORD)(src._a << 4) + (alpha << 8)] & 0xf0;
		DWORD alpha16e = (alpha16) << (8+4);   // 4 bit
		DWORD alpha16i = alpha16e ^ 0xf0000;   // �� 4 bit�𔽓]
		_rgb =
			 CFastPlaneBlendTable::awdMulTableRGB555[src._rgb + alpha16e] +
			 CFastPlaneBlendTable::awdMulTableRGB555[	 _rgb + alpha16i];
		return *this;
	}

	//	�����݂̃v���[������̃R�s�[
	inline CFastPlaneARGB4555& CFastPlaneARGB4555::operator = (const CFastPlaneARGB8888 &src)
	{
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		SetA(src.GetA());
		return *this;
	}
	inline CFastPlaneARGB4555& CFastPlaneARGB4555::operator = (const CFastPlaneABGR8888 &src)
	{
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		SetA(src.GetA());
		return *this;
	}
	inline CFastPlaneARGB4555& CFastPlaneARGB4555::operator = (const CFastPlaneARGB4565 &src)
	{
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		SetA(src.GetA());
		return *this;
	}

	//	�����݂̃v���[������̃R�s�[
	inline CFastPlaneARGB8888& CFastPlaneARGB8888::operator = (const CFastPlaneARGB4565 &src)
	{
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		SetA(src.GetA());
		return *this;
	}
	inline CFastPlaneARGB8888& CFastPlaneARGB8888::operator = (const CFastPlaneABGR8888 &src)
	{
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		SetA(src.GetA());
		return *this;
	}
	inline CFastPlaneARGB8888& CFastPlaneARGB8888::operator = (const CFastPlaneARGB4555 &src)
	{
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		SetA(src.GetA());
		return *this;
	}

	//	�����݂̃v���[������̃R�s�[
	inline CFastPlaneABGR8888& CFastPlaneABGR8888::operator = (const CFastPlaneARGB4565 &src)
	{
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		SetA(src.GetA());
		return *this;
	}
	inline CFastPlaneABGR8888& CFastPlaneABGR8888::operator = (const CFastPlaneARGB4555 &src)
	{
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		SetA(src.GetA());
		return *this;
	}
	inline CFastPlaneABGR8888& CFastPlaneABGR8888::operator = (const CFastPlaneARGB8888 &src)
	{
		SetRGB(src.GetR(),src.GetG(),src.GetB());
		SetA(src.GetA());
		return *this;
	}

	inline CFastPlaneRGB555& CFastPlaneRGB555::operator += (const CFastPlaneARGB4555& src){
		CFastPlaneRGB555 tmp = *(CFastPlaneRGB555*)(&src);
		tmp *= src.GetA();
		*this += tmp;
		return *this;
	}
	inline CFastPlaneRGB565& CFastPlaneRGB565::operator += (const CFastPlaneARGB4565& src){
		CFastPlaneRGB565 tmp = *(CFastPlaneRGB565*)(&src);
		tmp *= src.GetA();
		*this += tmp;
		return *this;
	}
	inline CFastPlaneRGB888& CFastPlaneRGB888::operator += (const CFastPlaneARGB8888& src){
		CFastPlaneRGB888 tmp = *(CFastPlaneRGB888*)(&src);
		tmp *= src.GetA();
		*this += tmp;
		return *this;
	}
	inline CFastPlaneBGR888& CFastPlaneBGR888::operator += (const CFastPlaneABGR8888& src){
		CFastPlaneBGR888 tmp = *(CFastPlaneBGR888*)(&src);
		tmp *= src.GetA();
		*this += tmp;
		return *this;
	}
	inline CFastPlaneXRGB8888& CFastPlaneXRGB8888::operator += (const CFastPlaneARGB8888& src){
		CFastPlaneXRGB8888 tmp = *(CFastPlaneXRGB8888*)(&src);
		tmp *= src.GetA();
		//	�ŏ�ʂ́A�O�a���Z�̉ߒ��Ń}�X�N�����
		*this += tmp;
		return *this;
	}
	inline CFastPlaneXBGR8888& CFastPlaneXBGR8888::operator += (const CFastPlaneABGR8888& src){
		CFastPlaneXBGR8888 tmp = *(CFastPlaneXBGR8888*)(&src);
		tmp *= src.GetA();
		//	�ŏ�ʂ́A�O�a���Z�̉ߒ��Ń}�X�N�����
		*this += tmp;
		return *this;
	}
	inline CFastPlaneRGB555& CFastPlaneRGB555::operator -= (const CFastPlaneARGB4555& src){
		CFastPlaneRGB555 tmp = *(CFastPlaneRGB555*)(&src);
		tmp *= src.GetA();
		*this -= tmp;
		return *this;
	}
	inline CFastPlaneRGB565& CFastPlaneRGB565::operator -= (const CFastPlaneARGB4565& src){
		CFastPlaneRGB565 tmp = *(CFastPlaneRGB565*)(&src);
		tmp *= src.GetA();
		*this -= tmp;
		return *this;
	}
	inline CFastPlaneRGB888& CFastPlaneRGB888::operator -= (const CFastPlaneARGB8888& src){
		CFastPlaneRGB888 tmp = *(CFastPlaneRGB888*)(&src);
		tmp *= src.GetA();
		*this -= tmp;
		return *this;
	}
	inline CFastPlaneBGR888& CFastPlaneBGR888::operator -= (const CFastPlaneABGR8888& src){
		CFastPlaneBGR888 tmp = *(CFastPlaneBGR888*)(&src);
		tmp *= src.GetA();
		*this -= tmp;
		return *this;
	}
	inline CFastPlaneXRGB8888& CFastPlaneXRGB8888::operator -= (const CFastPlaneARGB8888& src){
		CFastPlaneXRGB8888 tmp = *(CFastPlaneXRGB8888*)(&src);
		tmp *= src.GetA();
		*this -= tmp;
		return *this;
	}
	inline CFastPlaneXBGR8888& CFastPlaneXBGR8888::operator -= (const CFastPlaneABGR8888& src){
		CFastPlaneXRGB8888 tmp = *(CFastPlaneXRGB8888*)(&src);
		tmp *= src.GetA();
		*this -= tmp;
		return *this;
	}

	//	������́Adst = ��src�~src + ��dst�~dst
	inline CFastPlaneRGB565&  CFastPlaneRGB565::dmul (const CFastPlaneARGB4565 &src,BYTE srcalpha,BYTE dstalpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		DWORD alpha16s = CFastPlaneBlendTable::abyMulTable[(DWORD)(src._a << 4) + (srcalpha << 8)] & 0xf0;
		DWORD alpha16e = (alpha16s) << (8+4);	// 4 bit
		//	��dst = max( 'cause RGB565)
		DWORD alpha16d = CFastPlaneBlendTable::abyMulTable[(DWORD)(	   15 << 4) + (dstalpha << 8)] & 0xf0;
		DWORD alpha16i = (alpha16d) << (8+4);	// 4 bit
		_rgb =
			 CFastPlaneBlendTable::awdMulTableRGB565[src._rgb + alpha16e] +
			 CFastPlaneBlendTable::awdMulTableRGB565[	 _rgb + alpha16i];
		return *this;
	}
	//	������́Adst = ��src�~src + ��dst�~dst
	inline CFastPlaneRGB555&  CFastPlaneRGB555::dmul (const CFastPlaneARGB4555 &src,BYTE srcalpha,BYTE dstalpha){
	//	�]������alpha�l��blend����Ɖ���
	//	���̃I�y���[�^�́A�����ɂ���ɁA������alpha���|����
		DWORD alpha16s = CFastPlaneBlendTable::abyMulTable[(DWORD)(src._a << 4) + (srcalpha << 8)] & 0xf0;
		DWORD alpha16e = (alpha16s) << (8+4);	// 4 bit
		//	��dst = max( 'cause RGB555)
		DWORD alpha16d = CFastPlaneBlendTable::abyMulTable[(DWORD)(	   15 << 4) + (dstalpha << 8)] & 0xf0;
		DWORD alpha16i = (alpha16d) << (8+4);	// 4 bit
		_rgb =
			 CFastPlaneBlendTable::awdMulTableRGB555[src._rgb + alpha16e] +
			 CFastPlaneBlendTable::awdMulTableRGB555[	 _rgb + alpha16i];
		return *this;
	}

//////////////////////////////////////////////////////////////////////////////
//	CFastPlane�Ԃ̓]���̂��߂̊֐��I�u�W�F�N�g
//////////////////////////////////////////////////////////////////////////////

//	�]��������̃R�s�[�̂��߂�functor
class CFastPlaneCopySrc {
public:
	template <class _DST,class _SRC>
	inline void operator () (_DST& dst,_SRC& src) const {
		dst = src;
	}
};

//	�]��������̉��F�R�s�[�̂��߂�functor
class CFastPlaneCopyAdd {
public:
	template <class _DST,class _SRC>
	inline void operator () (_DST& dst,_SRC& src) const {
		dst += src;
	}
};

//	�]��������̌��F�R�s�[�̂��߂�functor
class CFastPlaneCopySub {
public:
	template <class _DST,class _SRC>
	inline void operator () (_DST& dst,_SRC& src) const {
		dst -= src;
	}
};

//	�]��������̏�Z�R�s�[�̂��߂�functor
class CFastPlaneCopyMul {
public:
	template <class _DST,class _SRC>
	inline void operator () (_DST& dst,_SRC& src) const {
		dst *= src;
	}
};

//	�]��������̏�Z�R�s�[�̂��߂�functor
//	�i���T�[�t�F�[�X���烿�T�[�t�F�[�X�ւ�PhotoShop���C���I�����j
class CFastPlaneCopyMulAlphaToAlpha {
public:
	template <class _DST,class _SRC>
	inline void operator () (_DST& dst,_SRC& src) const {
		dst.MulAlphaToAlpha(src);
/*
	PhotoShop���C���I����
		�V������  = ��src + (1-��src)�~��dst
		�V����RGB =	 (RGBSrc�~��src�@�{�@RGBDst�~��dst)/(��src + ��dst)
*/
	}
};

//	�]��������̏��Z�R�s�[�̂��߂�functor
class CFastPlaneCopyDiv {
public:
	template <class _DST,class _SRC>
	inline void operator () (_DST& dst,_SRC& src) const {
		dst /= src;
	}
};

//	�]�������猸�����w��t�̃u�����h�]���̂��߂�functor
class CFastPlaneCopyMulAlpha {
public:
	CFastPlaneCopyMulAlpha(DWORD alpha) : alpha_((BYTE)alpha){};// { _alpha=alpha; }
	//	�������Ŏw�肷��̂́A������

	template <class _DST,class _SRC>
	inline void operator () (_DST& dst,_SRC& src) const {
		dst.dmul(src,alpha_);
	}

private:
	BYTE	alpha_;
};

//	�]�������猸�����w��t�̃u�����h�]���̂��߂�functor
class CFastPlaneCopyMulAlphaAB {
public:
	CFastPlaneCopyMulAlphaAB(DWORD srcalpha,DWORD dstalpha) {
		_srcalpha=(BYTE)srcalpha;
		_dstalpha=(BYTE)dstalpha;
	}
	//	�������Ŏw�肷��̂́A������

	template <class _DST,class _SRC>
	inline void operator () (_DST& dst,_SRC& src) const {
		dst.dmul(src,_srcalpha,_dstalpha);
	}

private:
	BYTE	_srcalpha;
	BYTE	_dstalpha;
};


//	�]��������̒萔�{���F�R�s�[�̂��߂�functor
class CFastPlaneCopyAddMulConst {
public:
	CFastPlaneCopyAddMulConst(DWORD alpha) : _alpha((BYTE)alpha) {}
	/*
		DWORD�Ŏ󂯎���āABYTE�ɃL���X�g���Ă���̂́ABltter��
		DWORD���p�����[�^�Ƃ��ēn������
	*/
	template <class _DST,class _SRC>
	inline void operator () (_DST& dst,_SRC& src) const {
		_SRC tmp = src;
		tmp *= _alpha;
		dst += tmp;
	}

private:
	BYTE	_alpha;
};

//	�]��������̔����F���萔�{���F�R�s�[�̂��߂�functor
template <class _TYPE>
class CFastPlaneCopyAddMulConstSrcColorKey {
public:
	CFastPlaneCopyAddMulConstSrcColorKey(_TYPE rgb,DWORD alpha) : _alpha((BYTE)alpha) { _colorkey = rgb; }
	/*
		DWORD�Ŏ󂯎���āABYTE�ɃL���X�g���Ă���̂́ABltter��
		DWORD���p�����[�^�Ƃ��ēn������
	*/
	template <class _DST,class _SRC>
	inline void operator () (_DST& dst,_SRC& src) const {
		if (src.GetRGB() != _colorkey.GetRGB()){
			_SRC tmp = src;
			tmp *= _alpha;
			dst += tmp;
		}
	}

private:
	BYTE	_alpha;
	_TYPE	_colorkey;
};


//	�]��������̒萔�{���F�R�s�[�̂��߂�functor
class CFastPlaneCopySubMulConst {
public:
	CFastPlaneCopySubMulConst(DWORD alpha) : _alpha((BYTE)alpha) {}
	/*
		DWORD�Ŏ󂯎���āABYTE�ɃL���X�g���Ă���̂́ABltter��
		DWORD���p�����[�^�Ƃ��ēn������
	*/
	template <class _DST,class _SRC>
	inline void operator () (_DST& dst,_SRC& src) const {
		_SRC tmp = src;
		tmp *= _alpha;
		dst -= tmp;
	}

private:
	BYTE	_alpha;
};

//	�]��������̔����F���萔�{���F�R�s�[�̂��߂�functor
template <class TYPE>
class CFastPlaneCopySubMulConstSrcColorKey {
public:
	CFastPlaneCopySubMulConstSrcColorKey(TYPE rgb,DWORD alpha) : alpha_((BYTE)alpha) , colorkey_(rgb) {}
	/*
		DWORD�Ŏ󂯎���āABYTE�ɃL���X�g���Ă���̂́ABltter��
		DWORD���p�����[�^�Ƃ��ēn������
	*/
	template <class DST,class SRC>
	inline void operator () (DST& dst,SRC& src) const {
		if (src.GetRGB() != colorkey_.GetRGB()){
			SRC tmp = src;
			tmp *= alpha_;
			dst -= tmp;
		}
	}

private:
	BYTE	alpha_;
	TYPE	colorkey_;
};


//	���߂�FillColor
template <class DST>
class CFastPlaneFillColor {
public:
	CFastPlaneFillColor(const DST fill) : fill_(fill) {}
	//	�������Ŏw�肷��̂́A������

	inline void operator () (DST& dst,DST& src) const {
		dst = fill_;
	}

private:
	DST	fill_;
};

//	�ŏ�ʂ��N���A����
class CFastPlaneClearAlpha {
public:
	template <class _DST>
	inline void operator () (_DST& dst,_DST& src) const {
		dst.SetA(0);
	}
};

//	RGB�l�̔��]
class CFastPlaneFlush {
public:
	template <class _DST,class _SRC>
	inline void operator () (_DST& dst,_SRC& src) const {
		dst.SetRGB (src.GetRGB() ^ 0xffffff);
	}
	inline void operator () (CFastPlaneRGB565& dst,CFastPlaneRGB565 & src) const {
		dst.SetRGB (src.GetRGB() ^ 0xffff);
	}
	inline void operator () (CFastPlaneRGB555& dst,CFastPlaneRGB555 & src) const {
		dst.SetRGB (src.GetRGB() ^ 0x7fff);
	}
	inline void operator () (CFastPlaneBytePal& dst,CFastPlaneBytePal & src) const {
		dst.SetRGB (src.GetRGB() ^ 0xff);
	}
};

//	�]�����J���[�L�[�t���R�s�[�̂��߂�functor
template <class TYPE>
	//	_TYPE�@�ˁ@WORD,DWORD,..
class CFastPlaneCopySrcColorKey {
public:
	CFastPlaneCopySrcColorKey(TYPE rgb) : colorkey_(rgb) {}
	//	�K��DWORD�`�œn���悤�ɂ���΁ARGB�̕ϊ��q���s�v�ɂȂ�̂����A
	//	��������ƁARGB��DWORD���ȏ�̃s�N�Z���t�H�[�}�b�g�ɑΉ��ł��Ȃ�

	template <class DST,class SRC>
	inline void operator () (DST& dst,SRC& src) const {
		if (src.GetRGB() != colorkey_.GetRGB())
			dst = src;
	}
private:
	TYPE	colorkey_;
};

/*
	//	��������ǁA�x�������O�O�G�G

//	�]�����J���[�L�[�t���R�s�[�̂��߂�functor
//		RGB565565,RGB555555�̂��߂̃R�s�[functor
template <class _TYPE>
	//	_TYPE�@�ˁ@WORD,DWORD,..
class CFastPlaneCopySrcColorKeyDouble {
public:
	CFastPlaneCopySrcColorKeyDouble(_TYPE rgb){ _colorkey = rgb; }

	template <class _DST,class _SRC>
	inline void operator () (_DST& dst,_SRC& src) const {
		DWORD dwSrc = src.GetRGB();
		DWORD dwColorKey = _colorkey.GetRGB();
		DWORD dw = dwSrc ^ dwColorKey;

		//	�r�b�g���Z�ɂ��R�s�[ (C) ��˂��炨

		//	���݂̂��H��݂̂��H���邢�͗������H
		dw = (((dw & 0x7fff7fff) + 0x7fff7fff) | dw) & 0x80008000;
		//	���̕����ARGB555555�Ȃ�΁A�P��
		//	(dw + 0x7fff7fff) & 0x80008000�ōςނ̂����A�A

		//	�}�X�N����
		dw = ((dw >> 15) + 0x7fff7fff) ^ 0x7fff7fff;

		DWORD dwDst = dst.GetRGB();
		dst.SetRGB( (dwSrc & dw) | (dwDst & ~dw));

	}
private:
	_TYPE	_colorkey;
};
*/

//	�]�����J���[�L�[�t���R�s�[�̂��߂�functor
//		(Src�����t���摜�Ȃ�΁Aalpha�t���̓]���ƂȂ�)
template <class _TYPE>
	//	_TYPE�@�ˁ@WORD,DWORD,..
class CFastPlaneBlendSrcColorKey {
public:
	CFastPlaneBlendSrcColorKey(const _TYPE dw): _colorkey(dw){}

	template <class _DST,class _SRC>
	inline void operator () (_DST& dst,_SRC& src) const {
		if (src.GetRGB() != _colorkey.GetRGB())
			dst *= src;
	}
private:
	_TYPE	_colorkey;
};

//	�]�����J���[�L�[�t���u�����h�R�s�[�̂��߂�functor
//		���t���摜�̃t�F�[�h���]���Ɏg��
//		(Src�����t���̃R���e�i�Ȃ�΁Aalpha�t���̓]���ƂȂ邪�A
//		Src�����t���̃R���e�i�łȂ���΁Aalpha�͕t���Ȃ��B�P�Ȃ锼�����j
template <class _TYPE>
	//	_TYPE�@�ˁ@WORD,DWORD,..
class CFastPlaneBlendMulAlphaSrcColorKey {
public:
	CFastPlaneBlendMulAlphaSrcColorKey(const _TYPE dw,const DWORD byRate): _colorkey(dw),_nRate((BYTE)byRate){}

	template <class _DST,class _SRC>
	inline void operator () (_DST& dst,_SRC& src) const {
		if (src.GetRGB() != _colorkey.GetRGB())
			dst.dmul(src,_nRate);
	}
private:
	_TYPE	_colorkey;
	BYTE	_nRate;	//	�������䗦
};

//	����̂́A�����u�����h��
template <class TYPE>
	//	TYPE�@�ˁ@WORD,DWORD,..
class CFastPlaneBlendMulAlphaSrcColorKeyAB {
public:
	CFastPlaneBlendMulAlphaSrcColorKeyAB(TYPE dw,DWORD bySrcRate,DWORD byDstRate): colorkey_(dw),nSrcRate_((BYTE)bySrcRate),nDstRate_((BYTE)byDstRate){}

	template <class DST,class SRC>
	inline void operator () (DST& dst,SRC& src) const {
		if (src.GetRGB() != colorkey_.GetRGB())
			dst.dmul(src,nSrcRate_,nDstRate_);
	}
private:
	TYPE	colorkey_;
	BYTE	nSrcRate_;	//	�]�����������䗦
	BYTE	nDstRate_;	//	�]���攼�����䗦
};


//	�]�����J���[�L�[�t��AddColor�̂��߂�functor
template <class TYPE>
	//	_TYPE�@�ˁ@WORD,DWORD,..
class CFastPlaneCopyAddSrcColorKey {
public:
	CFastPlaneCopyAddSrcColorKey(const TYPE dw) : colorkey_(dw) {}

	template <class DST,class SRC>
	inline void operator () (DST& dst,SRC& src) const {
		if (src.GetRGB() != colorkey_.GetRGB())
			dst += src;
	}
private:
	TYPE	colorkey_;
};

//	�]�����J���[�L�[�t��AddColor�̂��߂�functor
template <class TYPE>
	//	TYPE�@�ˁ@WORD,DWORD,..
class CFastPlaneCopySubSrcColorKey {
public:
	CFastPlaneCopySubSrcColorKey(const TYPE dw) : colorkey_(dw) {}

	template <class DST,class SRC>
	inline void operator () (DST& dst,SRC& src) const {
		if (src.GetRGB() != colorkey_.GetRGB())
			dst -= src;
	}
private:
	TYPE	colorkey_;
};


//	Blue���A���ɓ˂����ނƂ��ɁA���j�A�ȃ}�b�s���O���s�Ȃ�
class CFastPlaneBltToAlpha {
public:
	CFastPlaneBltToAlpha(int nSrcMin,int nSrcMax,int nDstMin,int nDstMax){
		//	�}�b�s���O�e�[�u���͖�����Ȃ���
		int i,j;
		for(i=0;(i<=nSrcMin) && (i<256);++i){
			_abyTable[i] = (BYTE)nDstMin;
		}

		//	�͈͊O�ɑ΂��鏈���̉��P
		i = nSrcMin+1; if (i<0) { i=0; }
		for(j=1,i=nSrcMin+1;(i<nSrcMax) && (i<256);++i,++j) {

			//	nSrcMin == nSrcMax�ł��邱�Ƃ͂��肦�Ȃ�
			//	(���[�v���� nSrcMin+1 �` nSrcMax-1�܂łȂ̂�)
			int n = (nDstMin + j * (nDstMax-nDstMin) / (nSrcMax - nSrcMin));

			if (n <0) n = 0; ef ( n>255) n = 255;
			_abyTable[i] = n;
		}
		i = nSrcMax; if (i<0) i=0;
		for(;i<256;++i)
			_abyTable[i] = (BYTE)nDstMax;
	}

	template <class _DST,class _SRC>
	inline void operator () (_DST& dst,_SRC& src) const {
		dst.SetA(_abyTable[src.GetB()]);
	}
private:
	BYTE _abyTable[256];
};

//	�]��������̒萔���F�̂��߂�functor
template <class TYPE>
class CFastPlaneConstAdd {
public:
/*
	CFastPlaneConstAdd(COLORREF rgb){
		_rgb.SetR(rgb	  & 0xff);
		_rgb.SetG((rgb>>8) & 0xff);
		_rgb.SetB((rgb>>16)& 0xff);
	}
*/
	CFastPlaneConstAdd(const TYPE rgb) : rgb_(rgb) {}

	template <class DST>
	inline void operator () (DST& dst,DST& src) const {
		dst += rgb_;
	}

private:
	TYPE rgb_;
};

//	�]��������̌��F�R�s�[�̂��߂�functor
template <class TYPE>
class CFastPlaneConstSub {
public:
/*
	CFastPlaneConstSub(COLORREF rgb){
		_rgb.SetR(rgb	  & 0xff);
		_rgb.SetG((rgb>>8) & 0xff);
		_rgb.SetB((rgb>>16)& 0xff);
	}
*/
	CFastPlaneConstSub(TYPE rgb) : rgb_(rgb) {}

	template <class DST>
	inline void operator () (DST& dst,DST& src) const {
		dst -= rgb_;
	}

private:
	TYPE rgb_;
};

//	�]��������̒萔��Z�R�s�[�̂��߂�functor
class CFastPlaneConstMul {
public:
	CFastPlaneConstMul(DWORD nRate): nRate_((BYTE)nRate) {}

	template <class DST>
	inline void operator () (DST& dst,DST& src) const {
		dst *= nRate_;
	}

private:
	BYTE nRate_;
};

//	���ȊO�̃R�s�[�̂��߂�functor
class CFastPlaneCopyWithoutAlpha {
public:
	template <class DST,class SRC>
	inline void operator () (DST& dst,SRC& src) const {
		dst.CopyWithoutAlpha(src);
	}
};

//	���l�𔽓]������
class CFastPlaneFlushAlpha {
public:
	template <class DST,class SRC>
	inline void operator () (DST& dst,SRC& src) const {
		dst.FlushAlpha(src);
	}
};

//////////////////////////////////////////////////////////////////////////////
//	(CSrc*,CDst*,nSrcX,nSrcY)��v������^�C�v�̓]��functor
//////////////////////////////////////////////////////////////////////////////

//	�]��������̃R�s�[�̂��߂�functor
//		CFastPlane::BltFastAlphaMask16�̓]���Ɏg��
class CFastPlaneCopySrcMask16dmul {
public:
	CFastPlaneCopySrcMask16dmul(BYTE*abyAlphaTable) : _abyAlphaTable(abyAlphaTable) {}

	template <class DST,class SRC>
	inline void operator () (DST& dst,SRC& src,int nSrcX,int nSrcY) const {
		dst.dmul(src,_abyAlphaTable[(nSrcX & 0xf) + ((nSrcY & 0xf)<<4)]);
			//	�����̏�Z�͒�`����Ă���͂��D�D
	}
private:
	BYTE *_abyAlphaTable;	//	16*16 = 256�T�C�Y
};

//	�]�����J���[�L�[�t���R�s�[�̂��߂�functor
//		CFastPlane::BltAlphaMask16�̓]���Ɏg��
template <class TYPE>
class CFastPlaneCopySrcColorKeyMask16dmul {
public:
	CFastPlaneCopySrcColorKeyMask16dmul(TYPE rgb,BYTE *abyAlphaTable) : abyAlphaTable_(abyAlphaTable){ _colorkey = rgb; }
	//	�K��DWORD�`�œn���悤�ɂ���΁ARGB�̕ϊ��q���s�v�ɂȂ�̂����A
	//	��������ƁARGB��DWORD���ȏ�̃s�N�Z���t�H�[�}�b�g�ɑΉ��ł��Ȃ�

	template <class DST,class SRC>
	inline void operator () (DST& dst,SRC& src,int nSrcX,int nSrcY) const {
		if (src.GetRGB() != colorkey_.GetRGB())
			dst.dmul(src,abyAlphaTable_[(nSrcX & 0xf) + ((nSrcY & 0xf)<<4)]);
			//	�����̏�Z�͒�`����Ă���͂��D�D
	}
private:
	TYPE	colorkey_;
	BYTE*	abyAlphaTable_;	//	16*16 = 256�T�C�Y
};

//////////////////////////////////////////////////////////////////////////////
//	�]���̂��߂�functor
//////////////////////////////////////////////////////////////////////////////

class CFastPlaneEffect {
private:
	struct BurstTags {
		struct NoBurst {};	//	�]�������������Ȃ�
		struct MMXBurst {};	//	MMX���g�p���ē]��������������
	};

public:
	//	�����o�֐��e���v���[�g�T�|�[�g���Ă�˂��ȁc

	//	---- ���̃v���[���ɑ΂���Effect��������֐�
	template <class SrcClass,class EffectFunctor>
	static LRESULT Effect(SrcClass _src,const CSurfaceInfo* pInfo,EffectFunctor f,LPCRECT lpRect=NULL){
		WARNING(pInfo->GetPtr() == NULL,"CFastPlaneEffect::Effect��pInfo->GetPtr() == NULL");
		RECT r = pInfo->GetClipRect(lpRect);
		LONG lPitch	 = pInfo->GetPitch();
		SrcClass* pSurface = (SrcClass*)pInfo->GetPtr();

		//	8���[�v�W�J
		int nLoop8 = (r.right - r.left);
		//	8���[�v�]��
		int nLoop8mod = nLoop8 & 7;
		nLoop8 >>= 3;

		for(int y=r.top;y<r.bottom;y++){
			SrcClass *p = (SrcClass *)((BYTE*)pSurface + y*lPitch + r.left*sizeof(SrcClass));

			//�@���̓]�����ƂɁA���[�v�̃G���h�`�F�b�N���K�v���Ƃ�����A���ɍ���Ȃ�
			int n;
			for(n=0;n<nLoop8;n++){
				//	�]��������]����ցA���̊֐��G�t�F�N�g�������Ȃ���]��
				f(p[0],p[0]);
				f(p[1],p[1]);
				f(p[2],p[2]);
				f(p[3],p[3]);
				f(p[4],p[4]);
				f(p[5],p[5]);
				f(p[6],p[6]);
				f(p[7],p[7]);
				p+=8;
			}
			//	�]�蕪���A�]��
			for (n=0;n<nLoop8mod;n++){
				f(p[n],p[n]);
			}
		}
		return 0;
	}

	//	---- ���̃v���[���ɑ΂���Mosaic��������֐�
	//	�i����Ȋ֐���肽�������񂾂��D�D�j
	template <class SrcClass,class Fn>
	static LRESULT EffectMosaic(SrcClass src_,const CSurfaceInfo* pInfo,Fn fn,int nMosaicLevel,LPCRECT lpRect=NULL){
		WARNING(pInfo->GetPtr() == NULL,"CFastPlaneEffect::Effect��pInfo->GetPtr() == NULL");
		RECT r = pInfo->GetClipRect(lpRect);
		LONG lPitch	 = pInfo->GetPitch();
		SrcClass* pSurface = (SrcClass*)pInfo->GetPtr();

		int d = nMosaicLevel;
		for(int y=r.top;y<r.bottom;y+=d){
			int d2;		//	���[�̒[��
			if (y+d>r.bottom) d2=r.bottom-y; else d2=d;
			for(int x=r.left;x<r.right;x+=d){
				int d1;	//	�E�[�̒[��
				if (x+d>r.right) d1=r.right-x; else d1=d;
				
				SrcClass *p,*p2;
				p = (SrcClass*)((BYTE*)pSurface + y*lPitch + x * sizeof(SrcClass));
				SrcClass c;	// ��\�_�̐F
				/*	//	������\�_�Ƃ���̂́A���܂�ǂ��Ȃ��B
					//	�������\�X�Ƃ����ق����D�D
				c = *p;
				*/
				//	�������\�l�Ƃ���
				int x2 = x+(d>>1); if (x2>=r.right)	 x2=r.right-1;
				int y2 = y+(d>>1); if (y2>=r.bottom) y2=r.bottom-1;
				p2 = (SrcClass*)((BYTE*)pSurface + y*lPitch + x * sizeof(SrcClass));
				c = *p2;

				for(int py=0;py<d2;py++){
					p2 = p;
					for(int px=0;px<d1;px++){
						fn(*(p++),c);
					//	���ꂪ�R�s�[���Ӗ�����̂������Ӗ�����̂��͒m��Ȃ�
					}
					p = (SrcClass*)((BYTE*)p2 + lPitch);	//	next line
				}
			}
		}

		return 0;
	}

	//	�ʏ�]��
	template <class SrcClass, class DstClass, class BltFunctor>
	static LRESULT Blt(	SrcClass src, const CSurfaceInfo* lpSrcInfo,
						DstClass dst, CSurfaceInfo* lpDstInfo,
						BltFunctor f, const CSurfaceInfo::CBltInfo* pInfo)
	{
		return Blt(src, lpSrcInfo, dst, lpDstInfo, f, pInfo, BurstTags::NoBurst());
	}

	#ifndef yaneSDK_CantUseInlineAssembler
	//	�\�Ȃ��MMX���g�p���ē]��������������
	static LRESULT Blt(	CFastPlaneXRGB8888 src, const CSurfaceInfo* lpSrcInfo,
						CFastPlaneXRGB8888 dst, CSurfaceInfo* lpDstInfo,
						CFastPlaneCopyMulAlpha f, const CSurfaceInfo::CBltInfo* pInfo)
	{
		if ( CCPUID::GetObj()->isMMX() ) {
			return Blt(src, lpSrcInfo, dst, lpDstInfo, f, pInfo, BurstTags::MMXBurst());
		} else {
			return Blt(src, lpSrcInfo, dst, lpDstInfo, f, pInfo, BurstTags::NoBurst());
		}
	}
	#endif

private:
	template <class SrcClass, class DstClass, class BltFunctor>
	static LRESULT Blt(	SrcClass src, const CSurfaceInfo* lpSrcInfo,
						DstClass dst, CSurfaceInfo* lpDstInfo,
						BltFunctor f, const CSurfaceInfo::CBltInfo* pInfo,
						BurstTags::NoBurst)
	{
		//	assertion
		WARNING(lpSrcInfo->GetPtr() == NULL,"CFastPlaneEffect::Blt��lpSrcInfo->GetPtr() == NULL");
		WARNING(lpDstInfo->GetPtr() == NULL,"CFastPlaneEffect::Blt��lpDstInfo->GetPtr() == NULL");

		// �N���b�s���O���̏���
		// �]���͈͂Ȃ��̎��́A���̂܂܋A��
		CFastPlaneEffectClipper clip;
		if ( Clipping( lpDstInfo , lpSrcInfo , pInfo , &clip ) != 0 ) return 1;

		RECT rcSrcRect = clip.rcSrcRect;
		RECT rcDstRect = clip.rcDstRect;
		
		// �]����̉����Əc���̐ݒ�
		int		nDstWidth = rcDstRect.right - rcDstRect.left;
		int		nDstHeight = rcDstRect.bottom - rcDstRect.top;
		int		nSrcWidth = rcSrcRect.right - rcSrcRect.left;
		LONG	lPitchSrc = lpSrcInfo->GetPitch();
		LONG	lPitchDst = lpDstInfo->GetPitch();
		LONG	nAddSrcWidth = lpSrcInfo->GetPitch() - (nSrcWidth * sizeof(SrcClass));	// �N���b�s���O���Ĕ�΂����̎Z�o
		LONG	nAddDstWidth = lpDstInfo->GetPitch() - (nDstWidth * sizeof(DstClass));	// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
		SrcClass*	lpSrc = (SrcClass*)((BYTE*)lpSrcInfo->GetPtr() +(rcSrcRect.left*sizeof(SrcClass))+rcSrcRect.top*lPitchSrc );		// �N���b�s���O�����̃J�b�g
		DstClass*	lpDst = (DstClass*)((BYTE*)lpDstInfo->GetPtr() +(rcDstRect.left*sizeof(DstClass))+rcDstRect.top*lPitchDst );		// �w�肳�ꂽx,y�̈ʒu����

		if ( clip.bActualSize ){
			//���{

			//	'01/10/05	��˂��炨����
			//	8���[�v�W�J
			const int nLoop8 = nDstWidth >> 3;
			//	8���[�v�]��
			const int nLoop8mod = (nDstWidth & 7);

			nAddSrcWidth+=nLoop8mod*sizeof(SrcClass);
			nAddDstWidth+=nLoop8mod*sizeof(DstClass);

			if ( nLoop8mod==0 ) {
				for(int y=0;y<nDstHeight;++y){
					int n;
					for(n=0;n<nLoop8;++n){
						//	�]��������]����ցA���̊֐��G�t�F�N�g�������Ȃ���]��
						f(lpDst[0],lpSrc[0]);
						f(lpDst[1],lpSrc[1]);
						f(lpDst[2],lpSrc[2]);
						f(lpDst[3],lpSrc[3]);
						f(lpDst[4],lpSrc[4]);
						f(lpDst[5],lpSrc[5]);
						f(lpDst[6],lpSrc[6]);
						f(lpDst[7],lpSrc[7]);
						lpDst+=8; lpSrc+=8;
					}

					lpDst = (DstClass*) ( (LPBYTE)lpDst+nAddDstWidth);
					lpSrc = (SrcClass*) ( (LPBYTE)lpSrc+nAddSrcWidth);
				}
			} else {
				for(int y=0;y<nDstHeight;++y){
					int n;
					for(n=0;n<nLoop8;++n){
						//	�]��������]����ցA���̊֐��G�t�F�N�g�������Ȃ���]��
						f(lpDst[0],lpSrc[0]);
						f(lpDst[1],lpSrc[1]);
						f(lpDst[2],lpSrc[2]);
						f(lpDst[3],lpSrc[3]);
						f(lpDst[4],lpSrc[4]);
						f(lpDst[5],lpSrc[5]);
						f(lpDst[6],lpSrc[6]);
						f(lpDst[7],lpSrc[7]);
						lpDst+=8; lpSrc+=8;
					}
					//	�]�蕪���A�]��
					for(n=0;n<nLoop8mod;++n){
						f(lpDst[n],lpSrc[n]);
					}
					//	���[�v�̊O�ŃC���N�������g�����ق����A�����I
					//	lpDst+=nLoop8mod; lpSrc+=nLoop8mod;
					//	������́A����nAddDst(Src)Width�Ɏ��O���Z���Ă���I

					lpDst = (DstClass*) ( (LPBYTE)lpDst+nAddDstWidth);
					lpSrc = (SrcClass*) ( (LPBYTE)lpSrc+nAddSrcWidth);
				}
			}
		}else{
			// �񓙔{
			int		nInitialX = clip.nInitialX ;	//	-DX�@ :�@�Â̏����l = -DX
			int		nStepX = clip.nStepX;		//	 2*SX :�@��+=2*SX
			int		nCmpX = clip.nCmpX;		//	 2*DX :�@��>0�Ȃ�΃�-=2*DX���Ă�
			int		nStepsX = clip.nStepsX;	//	 SrcX�̈��̉��Z��(������)
			int		nInitialY = clip.nInitialY;
			int		nStepY = clip.nStepY;
			int		nCmpY = clip.nCmpY;
			int		nStepsY = clip.nStepsY;
			DWORD	AddSrcPixel = sizeof(SrcClass) * nStepsX;
//			DWORD	AddWidthSrc = lpSrcInfo->GetPitch() * nStepsY;
			DWORD	nAddSrcHeight= lpSrcInfo->GetPitch() * nStepsY;						// y���̐������ŉ��Z�����l
//			DWORD	nAddDstWidth = lpDstInfo->GetPitch() - (nWidth*sizeof(DstClass));	// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
			int		i, j;
			int		nExCnt, nEyCnt;
			nEyCnt = EIY;
			for ( j = 0 ; j < nDstHeight ; j++ )
			{
				nEyCnt += EY;											// �]����Y�̏������̉��Z
				if ( nEyCnt >= 0 )
				{
					lpSrc = (SrcClass*)((BYTE*)lpSrc + lPitchSrc );		// �]����Y�����̃��C���ɂ���
					nEyCnt -= EY2;										// Y�̕␳�l
				}

				SrcClass*	lpSrcBack = lpSrc;
				nExCnt = EIX;
				for ( i = 0 ; i < nDstWidth ; i++ )
				{
					nExCnt += EX;		// �]������X�̏������̉��Z
					if ( nExCnt >= 0 )
					{
						lpSrc++;
						nExCnt -= EX2;	// X�̕␳�l
					}

					f(lpDst[i],*lpSrc);
					// �]������X�̐������̉��Z
					lpSrc = (SrcClass*)((BYTE*)lpSrc + AddSrcPixel);

//					lpDst++;			//	�]�����X�����Z
				}
				lpSrc = (SrcClass*)((BYTE*)lpSrcBack + nAddSrcHeight );	// �]����X���[�v�Ői�񂾕��߂��Ay���̐����������Z����
				lpDst = (DstClass*)((BYTE*)lpDst + lPitchDst );		//	�]����̐������̉��Z
			}
		}
		
		return 0;
	}
	#ifndef yaneSDK_CantUseInlineAssembler
	static LRESULT Blt(	CFastPlaneXRGB8888 src, const CSurfaceInfo* lpSrcInfo,
						CFastPlaneXRGB8888 dst, CSurfaceInfo* lpDstInfo,
						CFastPlaneCopyMulAlpha f, const CSurfaceInfo::CBltInfo* pInfo,
						BurstTags::MMXBurst);
	#endif

public:
	//	---- ���̃v���[���ɑ΂���Effect��������֐�
	//	�{�R�s�[�T�|�[�g��
	template <class SrcClass,class SrcClass2
		,class EffectFunctor,class EffectFunctor2>
	static LRESULT EffectDouble(SrcClass _src,const CSurfaceInfo* pInfo
		,SrcClass2 _src2,EffectFunctor f,EffectFunctor2 f2,LPCRECT lpRect=NULL){
		WARNING(pInfo->GetPtr() == NULL,"CFastPlaneEffect::Effect��pInfo->GetPtr() == NULL");
		RECT r = pInfo->GetClipRect(lpRect);
		LONG lPitch	 = pInfo->GetPitch();
		SrcClass* pSurface = (SrcClass*)pInfo->GetPtr();

		//	8���[�v�W�J
		int nLoop8 = (r.right - r.left);
		//	����DWORD���E
		bool bAlignFalse = (r.left & 1)!=0;
		if (bAlignFalse) nLoop8--;
		//	8���[�v�]�� / 2
		int nLoop8mod = nLoop8 & 7;
		nLoop8 >>= 3;
		//	�E��DWORD���E
		bool bAlignFalse2 = (nLoop8mod & 1)!=0;
		nLoop8mod >>= 1;

		for(int y=r.top;y<r.bottom;y++){
			SrcClass *p = (SrcClass *)((BYTE*)pSurface + y*lPitch + r.left*sizeof(SrcClass));

			//	�[��WORD�݂̂��s���ď���
			if (bAlignFalse) {
				f(*p,*p);
				p++;
			}

			//�@���̓]�����ƂɁA���[�v�̃G���h�`�F�b�N���K�v���Ƃ�����A���ɍ���Ȃ�
			int n;
			for(n=0;n<nLoop8;n++){
				//	�]��������]����ցA���̊֐��G�t�F�N�g�������Ȃ���]��
				f2(*(SrcClass2*)(p	),*(SrcClass2*)(p  ));
				f2(*(SrcClass2*)(p+2),*(SrcClass2*)(p+2));
				f2(*(SrcClass2*)(p+4),*(SrcClass2*)(p+4));
				f2(*(SrcClass2*)(p+6),*(SrcClass2*)(p+6));
				p+=8;
			}
			//	�]�蕪���A�]��
			for (n=0;n<nLoop8mod;n++){
				f2(*(SrcClass2*)(p	),*(SrcClass2*)(p  ));
				p+=2;
			}
			//	�[��WORD�݂̂�����
			if (bAlignFalse2) {
				f(*p,*p);
//				p++;
			}
		}
		return 0;
	}

	//	---- �]���̂Ƃ��ɂȂ�₩�₷��֐�
	//	�{�R�s�[�T�|�[�g��
	template <class SrcClass,class DstClass,class SrcClass2,class DstClass2,class BltFunctor,class BltFunctor2>
	static LRESULT BltDouble(
		SrcClass _src,const CSurfaceInfo* lpSrcInfo,
		DstClass _dst,CSurfaceInfo* lpDstInfo,
		SrcClass2 _src2,
		DstClass2 _dst2,
		BltFunctor f,
		BltFunctor2 f2,
		const CSurfaceInfo::CBltInfo* pInfo){

		WARNING(lpSrcInfo->GetPtr() == NULL,"CFastPlaneEffect::Blt��lpSrcInfo->GetPtr() == NULL");
		WARNING(lpDstInfo->GetPtr() == NULL,"CFastPlaneEffect::Blt��lpDstInfo->GetPtr() == NULL");

		//	LONG lPitch	 = pInfo->GetPitch();
		//	DWORD* pSurface = pInfo->GetPtr();

		//	CDIB32Base* srcp = lpSrcDIB->GetDIB32BasePtr();
		//	CDIB32Base* dstp = lpDstDIB->GetDIB32BasePtr();

		// �N���b�s���O���̏���
		// �]���͈͂Ȃ��̎��́A���̂܂܋A��
		CFastPlaneEffectClipper clip;
		if ( Clipping( lpDstInfo , lpSrcInfo , pInfo , &clip ) != 0 ) return 1;
		
		RECT rcSrcRect = clip.rcSrcRect;
		RECT rcDstRect = clip.rcDstRect;
//		RECT rcClipRect = clip.rcClipRect;
		
		// �]����̉����Əc���̐ݒ�
		int		nDstWidth = rcDstRect.right - rcDstRect.left;
		int		nDstHeight = rcDstRect.bottom - rcDstRect.top;
		int		nSrcWidth = rcSrcRect.right - rcSrcRect.left;
		LONG	lPitchSrc = lpSrcInfo->GetPitch();
		LONG	lPitchDst = lpDstInfo->GetPitch();
		LONG	nAddSrcWidth = lpSrcInfo->GetPitch() - (nSrcWidth * sizeof(SrcClass));	// �N���b�s���O���Ĕ�΂����̎Z�o
		LONG	nAddDstWidth = lpDstInfo->GetPitch() - (nDstWidth * sizeof(DstClass));	// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
		SrcClass*	lpSrc = (SrcClass*)((BYTE*)lpSrcInfo->GetPtr() +(rcSrcRect.left*sizeof(SrcClass))+rcSrcRect.top*lPitchSrc );		// �N���b�s���O�����̃J�b�g
		DstClass*	lpDst = (DstClass*)((BYTE*)lpDstInfo->GetPtr() +(rcDstRect.left*sizeof(DstClass))+rcDstRect.top*lPitchDst );		// �w�肳�ꂽx,y�̈ʒu����

		if ( clip.bActualSize ){
			//���{

			//	'01/10/05	��˂��炨����
			//	8���[�v�W�J
			int nLoop8 = nDstWidth;
			//	DWORD���E
			bool bAlignFalse = (rcDstRect.left & 1)!=0;
			if (bAlignFalse) nLoop8--;
			//	8���[�v�]�� / 2
			int nLoop8mod = nLoop8 & 7;
			nLoop8 >>= 3;
			//	�E��DWORD���E
			bool bAlignFalse2 = (nLoop8mod & 1)!=0;
			nLoop8mod >>= 1;

			for(int y=0;y<nDstHeight;y++){

				//	�[��WORD�݂̂��s���ď���
				if (bAlignFalse) {
					f(*lpDst,*lpSrc);
					lpDst++; lpSrc++;
				}
				//	DWORD�P�ʂŃR�s�[
				int n;
				for(n=0;n<nLoop8;n++){
					//	�]��������]����ցA���̊֐��G�t�F�N�g�������Ȃ���]��
					f2(*(DstClass2*)(lpDst	),*(SrcClass2*)(lpSrc  ));
					f2(*(DstClass2*)(lpDst+2),*(SrcClass2*)(lpSrc+2));
					f2(*(DstClass2*)(lpDst+4),*(SrcClass2*)(lpSrc+4));
					f2(*(DstClass2*)(lpDst+6),*(SrcClass2*)(lpSrc+6));
					lpDst+=8; lpSrc+=8;
/*
				//	���̃R�[�h�̓��W�X�^���t���j�Q���ꂻ���Ȃ̂ŁA
				//	�ǂ��炪�������͔����ȂƂ���D�D

					f2(*(DstClass2*)(lpDst	),*(SrcClass2*)(lpSrc));
					lpDst+=2; lpSrc+=2;
					f2(*(DstClass2*)(lpDst)	 ,*(SrcClass2*)(lpSrc));
					lpDst+=2; lpSrc+=2;
					f2(*(DstClass2*)(lpDst)	 ,*(SrcClass2*)(lpSrc));
					lpDst+=2; lpSrc+=2;
					f2(*(DstClass2*)(lpDst)	 ,*(SrcClass2*)(lpSrc));
					lpDst+=2; lpSrc+=2;
*/
				}
				//	�]�蕪���A�]��
				for (n=0;n<nLoop8mod;n++){
					f2(*(DstClass2*)(lpDst	),*(SrcClass2*)(lpSrc  ));
					lpDst+=2; lpSrc+=2;
				}
				//	�[��WORD�݂̂�����
				if (bAlignFalse2) {
					f(*lpDst,*lpSrc);
					lpDst++; lpSrc++;
				}
				//	��L�̃C���N�������g�Q�͂���ɍœK���̗]�n�����邪�A
				//	�ʓ|�Ȃ̂ŁA���ꂮ�炢�ł������D�D
				lpDst = (DstClass*) ( (LPBYTE)lpDst+nAddDstWidth);
				lpSrc = (SrcClass*) ( (LPBYTE)lpSrc+nAddSrcWidth);
			}
		}else{
			// �񓙔{
			int		nInitialX = clip.nInitialX ;	//	-DX�@ :�@�Â̏����l = -DX
			int		nStepX = clip.nStepX;		//	 2*SX :�@��+=2*SX
			int		nCmpX = clip.nCmpX;		//	 2*DX :�@��>0�Ȃ�΃�-=2*DX���Ă�
			int		nStepsX = clip.nStepsX;	//	 SrcX�̈��̉��Z��(������)
			int		nInitialY = clip.nInitialY;
			int		nStepY = clip.nStepY;
			int		nCmpY = clip.nCmpY;
			int		nStepsY = clip.nStepsY;
			DWORD	AddSrcPixel = sizeof(SrcClass) * nStepsX;
//			DWORD	AddWidthSrc = lpSrcInfo->GetPitch() * nStepsY;
			DWORD	nAddSrcHeight= lpSrcInfo->GetPitch() * nStepsY;						// y���̐������ŉ��Z�����l
//			DWORD	nAddDstWidth = lpDstInfo->GetPitch() - (nWidth*sizeof(DstClass));	// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
			int		i, j;
			int		nExCnt, nEyCnt;
			nEyCnt = EIY;
			for ( j = 0 ; j < nDstHeight ; j++ )
			{
				nEyCnt += EY;											// �]����Y�̏������̉��Z
				if ( nEyCnt >= 0 )
				{
					lpSrc = (SrcClass*)((BYTE*)lpSrc + lPitchSrc );		// �]����Y�����̃��C���ɂ���
					nEyCnt -= EY2;										// Y�̕␳�l
				}

				SrcClass*	lpSrcBack = lpSrc;
				nExCnt = EIX;
				for ( i = 0 ; i < nDstWidth ; i++ )
				{
					nExCnt += EX;		// �]������X�̏������̉��Z
					if ( nExCnt >= 0 )
					{
						lpSrc++;
						nExCnt -= EX2;	// X�̕␳�l
					}

					f(lpDst[i],*lpSrc);
					// �]������X�̐������̉��Z
					lpSrc = (SrcClass*)((BYTE*)lpSrc + AddSrcPixel);

//					lpDst++;			//	�]�����X�����Z
				}
				lpSrc = (SrcClass*)((BYTE*)lpSrcBack + nAddSrcHeight );	// �]����X���[�v�Ői�񂾕��߂��Ay���̐����������Z����
				lpDst = (DstClass*)((BYTE*)lpDst + lPitchDst );		//	�]����̐������̉��Z
			}
		}
		
		return 0;
	}

#pragma warning(default:4701) // �l�����蓖�Ă��ĂȂ��ϐ�����warning

	//	---- ���p�`�����p�`�֓]������֐�
	template <class SrcClass,class DstClass,class BltFunctor>
	static LRESULT Morph(
		SrcClass		_src,				// �]�����v���[���̃N���X
		const CSurfaceInfo*	lpSrcInfo,		// �]�����v���[���̏��
		DstClass		_dst,				// �]����v���[���̃N���X
		CSurfaceInfo*	lpDstInfo,			// �]����v���[���̏��
		BltFunctor		f,					// �G�t�F�N�g��functor
		const CSurfaceInfo::CMorphInfo* pInfo	// Morph���
	) {

		if (lpSrcInfo==NULL || lpDstInfo==NULL || pInfo==NULL) return -1;

		WARNING(lpSrcInfo->GetPtr() == NULL,"CFastPlaneEffect::Morph��lpSrcInfo->GetPtr() == NULL");
		WARNING(lpDstInfo->GetPtr() == NULL,"CFastPlaneEffect::Morph��lpDstInfo->GetPtr() == NULL");

		//-------------------------------
		LPCPOINT		lpSrcPoint = pInfo->lpSrcPoint;
		LPCPOINT		lpDstPoint = pInfo->lpDstPoint;
		LPCRECT			lpClipRect = pInfo->lpClipRect;
		bool			bContinual = pInfo->bContinual;	// �ׂ荇�킹�Ƀ��[�t�B���O�]���������̌q���ڂ𒲐�
		int				nAngle	   = pInfo->nAngle;		// �A���O����
		//-------------------------------

		//--- �ǉ� '02/03/06  by ENRA ---
		// ���̃R�[�h���ƁADstPoint��DstRect����݂͂łĂ���
		// �I�[�o�[�A�N�Z�X���邶���(;�L�D`)
		RECT rcDstClip;
		if (lpClipRect==NULL) {
			SIZE size = lpDstInfo->GetSize();
			::SetRect(&rcDstClip,0,0,size.cx,size.cy);
			lpClipRect = &rcDstClip;
		}

		/**
			14bit(������)-18bit(������)
			14bit������[�ƁA16384*16384�T�C�Y��
			�T�[�t�F�[�X�܂ň����������[���Ƃ��
		*/
		#define POINT_SHIFT ( 18 )				   // �����_�ȉ���bit��
		#define POINT_ONE	( 1<<POINT_SHIFT )	   // �Œ菬���_�̐��E�ł� 1, �C���N�������g���Ɏg��
		#define POINT_HALF	( 1<<(POINT_SHIFT-1) ) // �Œ菬���_�̐��E�ł� 0.5, �l�̌ܓ��Ɏg��

		// �n�_�ƏI�_���������\����
		typedef struct {
			POINT	Start;
			POINT	End;
		} LINE;

		// ����Ƃ���ŏo�Ă���J�E���^
		int i;

		///////////////////////////////////////////////////////////////////////
		// �]����̒��_����A��ԍ���̓_�ƁA��ԉE���̓_�̔z��ԍ������߂�
		///////////////////////////////////////////////////////////////////////

		// �]���撸�_��̂�����ԍ����(y���W��������)�_�ƁA��ԉE����(y���W���傫��)�_�̔z��ԍ������߂�
		// ��) 4�p�`���l����ƁAy���W�̑召�֌W�� lpDstPoint[3] < lpDstPoint[1] < lpDstPoint[2] < lpDstPoint[0] �������ꍇ
		// DstMinYIdx = 3 : ��ԏ�ɂ���_(top)
		// DstMaxYIdx = 0 : ��ԉ��ɂ���_(bottom)
		// �ƂȂ�B

		//[A1]
		// ���܂ł̍ŏ��̓_�� y���W��������
		// x���W�����܂ł̓_��菬����������(��荶��ɂ�������)
		// ���̓_��V���ɍŏ��̓_�Ƃ���

		//[A2]
		// ���܂ł̍ő�̓_�� y���W��������
		// x���W�����܂ł̓_���傫��������(���E���ɂ�������)
		// ���̓_��V���ɍő�̓_�Ƃ���

		// �ЂƂ܂��AlpDstPoint[0] ��(�L�蓾�Ȃ�����)�ő�̓_�ł�����A�ŏ��̓_�ł�������̂Ƃ���
		int	  MinIdx=0;				   // ��ԍ���ɂ��钸�_�̔z��ԍ�
		int	  MaxIdx=0;				   // ��ԉE���ɂ��钸�_�̔z��ԍ�
		POINT MinPoint=lpDstPoint[0];  // ���̃|�C���g�ɂ͈�ԍ���ɂ���_�����邱�ƂɂȂ�
		POINT MaxPoint=lpDstPoint[0];  // ���̃|�C���g�ɂ͈�ԉE��ɂ���_�����邱�ƂɂȂ�

		// ����݂Ԃ��ɓ]���撸�_������Ă���
		for( i=1; i<nAngle; i++ ){
			// �ŏ��̓_��T��
			if( lpDstPoint[i].y < MinPoint.y ){
				// ���܂ł̍ŏ��̓_��菬�����_��������A���̓_��V���ɍŏ��̓_�Ƃ���
				MinIdx = i;
				MinPoint = lpDstPoint[i];
				continue; // ���ꂪ�ŏ��̓_�Ȃ�A�ő�̓_�ɂ͂Ȃ蓾�Ȃ�
			}
			else if( lpDstPoint[i].y == MinPoint.y && lpDstPoint[i].x < MinPoint.x ){
				// [A1]
				MinIdx = i;
				MinPoint = lpDstPoint[i];
				continue; // ���ꂪ�ŏ��̓_�Ȃ�A�ő�̓_�ɂ͂Ȃ蓾�Ȃ�
			}

			// �ő�̓_��T��
			if( MaxPoint.y < lpDstPoint[i].y ){
				// ���܂ł̍ő�̓_��� �傫���_��������
				MaxIdx = i;
				MaxPoint = lpDstPoint[i];
			}
			else if( MaxPoint.y == lpDstPoint[i].y && MaxPoint.x < lpDstPoint[i].x ){
				// [A2]
				MaxIdx = i;
				MaxPoint = lpDstPoint[i];
			}

		}

		WARNING( MinIdx < 0,"CFastPlaneEffect::Morph�� MinIdx < 0");
		WARNING( MaxIdx < 0,"CFastPlaneEffect::Morph�� MaxIdx < 0");
		WARNING( nAngle <= MinIdx ,"CFastPlaneEffect::Morph�� nAngle <= MinIdx");
		WARNING( nAngle <= MaxIdx ,"CFastPlaneEffect::Morph�� nAngle <= MaxIdx");

		///////////////////////////////////////////////////////////////////////
		// �O�ς̌v�Z
		///////////////////////////////////////////////////////////////////////

		// ��ԏ����� y���W�̒��_(top)�ƁA���̑O��̒��_
		// ( top �� lpDstPoint[2] �������� lpDstPoint[1] �� lpDstPoint[3] )
		// �ō\�������x�N�g�����m�̊O�ς��Ƃ�

		int PrevIdx=MinIdx-1; // ��ԍŏ��̒��_�̔z��ԍ��̑O�̔z��ԍ� (=MinIdx-1)
		int NextIdx=MinIdx+1; // ��ԍŏ��̒��_�̔z��ԍ��̎��̔z��ԍ� (=MinIdx+1)

		// �����ŏ��̒��_�̔z��ԍ��� 0 ���������O�͍Ō�̔z��ԍ�
		if( MinIdx == 0 ){
			PrevIdx = nAngle-1;
		}
		// �����ŏ��̒��_�̔z��ԍ��� nAngle-1 �����������͍ŏ��̔z��ԍ�
		if( MinIdx == nAngle-1 ){
			NextIdx = 0;
		}

		// �O�όv�Z
		int nVec = (lpDstPoint[PrevIdx].x - lpDstPoint[MinIdx].x)
				   * (lpDstPoint[NextIdx].y - lpDstPoint[MinIdx].y)
				   -
				   (lpDstPoint[NextIdx].x - lpDstPoint[MinIdx].x)
				   * (lpDstPoint[PrevIdx].y - lpDstPoint[MinIdx].y);

		///////////////////////////////////////////////////////////////////////
		// �����ɂ�����ƉE���ɂ�����̎n�I�_�����߂Ă���
		///////////////////////////////////////////////////////////////////////

		// ���̊O�ς����Ȃ獶���B�܂�E�E�E
		// lpDstPoint[top] -> lpDstPoint[top+1] -> ... -> lpDstPoint[bottom-1] -> lpDstPoint[bottom]
		// �Ƃ������_��Ō��΂����͍�(LeftLine)�ɂ���A
		// lpDstPoint[top] -> lpDstPoint[top-1] -> ... -> lpDstPoint[bottom+1] -> lpDstPoint[bottom]
		// �Ƃ������_��Ō��΂����͉E(RightLine)�ɂ���B
		// ���R�A���̊O�ς����Ȃ�E���B�܂�E�E�E�����̋t (^^;
		// ���Ȃ݂ɁA�]���͍��̐�����n�܂�E�̐��Ɍ�������(x���W������)�i��

		// �����A�E���̐��̐�
		int nLeftLine=0, nRightLine=0;

		// ���_������񂾐�����鎞�Ɏn�_���A�I�_���̔z��ԍ��ƂȂ�ϐ�
		int StartIdx, EndIdx;

		// �]����̒��_��ŏ�ӁA�������͒�ӂ��A�����ɂȂ��Ă����Ƃ���
		// ���̐����̐��̓J�E���g���Ȃ��l�ɂ���
		int LeftMaxIdx=MaxIdx, RightMinIdx=MinIdx;

		// �����E���̐��ŁA���̒��ɂ͎n�_�ƏI�_�̍��W�������Ă���
		// ���̖{���𐔂������ new �Ń��������m�ۂ���
		LINE *LeftDstLine, *RightDstLine, *LeftSrcLine, *RightSrcLine;

		if( nVec > 0 ){ // �����

			// ��ӂ�������������(MaxIdx��1�O�̔z��ԍ��� y���W�Ɠ�������������)
			// �Ō�̍����̐��̓J�E���g���Ȃ�
			if( MaxIdx == 0 ){
				if( lpDstPoint[MaxIdx].y == lpDstPoint[nAngle-1].y ){
					LeftMaxIdx = nAngle - 1;
				}
			}
			else{
				if( lpDstPoint[MaxIdx].y == lpDstPoint[MaxIdx-1].y ){
					LeftMaxIdx = MaxIdx - 1;
				}
			}

			// �����ɂ�����̐��𐔂���
			for( i=MinIdx; i!=LeftMaxIdx; ){
				if( ++i == nAngle ){ // ��ԍŌ�̔z��𒴂��Ă��܂�����
					i=0; // �ŏ��ɖ߂�
				}
				nLeftLine++;
			}

			// ��ӂ�������������(MinIdx��1�O�̔z��ԍ��� y���W�Ɠ�������������)
			// �ŏ��̉E���̐��̓J�E���g���Ȃ�
			if( MinIdx == 0 ){
				if( lpDstPoint[MinIdx].y == lpDstPoint[nAngle-1].y ){
					RightMinIdx = nAngle - 1;
				}
			}
			else{
				if( lpDstPoint[MinIdx].y == lpDstPoint[MinIdx-1].y ){
					RightMinIdx = MinIdx - 1;
				}
			}

			// �E���ɂ�����̐��𐔂���
			for( i=RightMinIdx; i!=MaxIdx; ){
				if( --i == -1 ){  // �ŏ��̒��_��1�O�͖����̂�
					i=nAngle - 1; // �Ō�̒��_��
				}
				nRightLine++;
			}

			// �]����A�]�����̉E���̐��ƁA�����̐��̃o�b�t�@���m��
			LeftDstLine	 = new LINE[nLeftLine+1];
			RightDstLine = new LINE[nRightLine+1];
			LeftSrcLine	 = new LINE[nLeftLine+1];
			RightSrcLine = new LINE[nRightLine+1];

			// �����̐��̎n�I�_�������Ă���
			for( i=0, EndIdx=MinIdx; EndIdx!=LeftMaxIdx ; i++ ){
				StartIdx = EndIdx++;

				if( EndIdx == nAngle ){
					EndIdx = 0;
				}

				// �]����A�]�����̍����̐��̎n�I�_�������Ă���
				LeftDstLine[i].Start = lpDstPoint[StartIdx];
				LeftDstLine[i].End	 = lpDstPoint[EndIdx];
				LeftSrcLine[i].Start = lpSrcPoint[StartIdx];
				LeftSrcLine[i].End	 = lpSrcPoint[EndIdx];
			}

			// �E���̐��̎n�I�_�������Ă���
			for( i=0, EndIdx=RightMinIdx; EndIdx!=MaxIdx ; i++ ){
				StartIdx = EndIdx--; // i �͏I�_���̔z��ԍ�

				if( EndIdx == -1 ){
					EndIdx = nAngle-1;
				}

				// �]����A�]�����̉E���̐��̎n�I�_�������Ă���
				RightDstLine[i].Start = lpDstPoint[StartIdx];
				RightDstLine[i].End	  = lpDstPoint[EndIdx];
				RightSrcLine[i].Start = lpSrcPoint[StartIdx];
				RightSrcLine[i].End	  = lpSrcPoint[EndIdx];
			}

		}
		else{ // �E���

			// ��ӂ�������������(MinIdx��1���̔z��ԍ��� y���W�Ɠ�������������)
			// �ŏ��̉E���̐��̓J�E���g���Ȃ�
			if( MinIdx == nAngle - 1 ){
				if( lpDstPoint[MinIdx].y == lpDstPoint[0].y ){
					RightMinIdx = 0;
				}
			}
			else{
				if( lpDstPoint[MinIdx].y == lpDstPoint[MinIdx+1].y ){
					RightMinIdx = MinIdx + 1;
				}
			}

			// �E���ɂ�����̐��𐔂���
			for( i=RightMinIdx; i!=MaxIdx; ){
				if( ++i == nAngle ){ // ��ԍŌ�̔z��𒴂��Ă��܂�����
					i=0; // �ŏ��ɖ߂�
				}
				nRightLine++;
			}

			// ��ӂ�������������(MaxIdx��1���̔z��ԍ��� y���W�Ɠ�������������)
			// �Ō�̍����̐��̓J�E���g���Ȃ�
			if( MaxIdx == nAngle - 1 ){
				if( lpDstPoint[MaxIdx].y == lpDstPoint[0].y ){
					LeftMaxIdx = 0;
				}
			}
			else{
				if( lpDstPoint[MaxIdx].y == lpDstPoint[MaxIdx+1].y ){
					LeftMaxIdx = MaxIdx + 1;
				}
			}

			// �����ɂ�����̐��𐔂���
			for( i=MinIdx; i!=LeftMaxIdx; ){
				if( --i == -1 ){ // �ŏ��̒��_��1�O�͖����̂�
					i=nAngle - 1;	 // �Ō�̒��_��
				}
				nLeftLine++;
			}

			// �]����A�]�����̉E���̐��ƁA�����̐��̃o�b�t�@���m��
			LeftDstLine	 = new LINE[nLeftLine+1];
			RightDstLine = new LINE[nRightLine+1];
			LeftSrcLine	 = new LINE[nLeftLine+1];
			RightSrcLine = new LINE[nRightLine+1];

			// �E���̐��̎n�I�_�������Ă���
			for( i=0, EndIdx=RightMinIdx; EndIdx!=MaxIdx ; i++ ){
				StartIdx = EndIdx++;

				if( EndIdx == nAngle ){
					EndIdx = 0;
				}

				// �]����A�]�����̐��̎n�I�_�������Ă���
				RightDstLine[i].Start = lpDstPoint[StartIdx];
				RightDstLine[i].End	  = lpDstPoint[EndIdx];
				RightSrcLine[i].Start = lpSrcPoint[StartIdx];
				RightSrcLine[i].End	  = lpSrcPoint[EndIdx];
			}

			// �����̐��̎n�I�_�������Ă���
			for( i=0, EndIdx=MinIdx; EndIdx!=LeftMaxIdx ; i++ ){
				StartIdx = EndIdx--;

				if( EndIdx == -1 ){
					EndIdx = nAngle-1;
				}

				// �]����A�]�����̐��̎n�I�_�������Ă���
				LeftDstLine[i].Start = lpDstPoint[StartIdx];
				LeftDstLine[i].End	 = lpDstPoint[EndIdx];
				LeftSrcLine[i].Start = lpSrcPoint[StartIdx];
				LeftSrcLine[i].End	 = lpSrcPoint[EndIdx];
			}

		}

		// �X�L�������C������ԉ��̍��W�ƈ�v�������̑΍�
		RightDstLine[nRightLine] = RightDstLine[nRightLine-1];
		RightSrcLine[nRightLine] = RightSrcLine[nRightLine-1];
		LeftDstLine[nLeftLine]	 = LeftDstLine[nLeftLine-1];
		LeftSrcLine[nLeftLine]	 = LeftSrcLine[nLeftLine-1];

		///////////////////////////////////////////////////////////////////////
		// �]�����[�v�X�^�[�g
		///////////////////////////////////////////////////////////////////////

		// [D1]
		// �]����̍����̐��̎n�_���X�L�������C���Əd�Ȃ�Ȃ������ꍇ
		// ���������͂������Ȃ̂� if ���̏��������������ɂ����Ă�������
		// �ς� if ���̏������������ق��Ă���` (^^;

		// [D2]
		// �����̐������̃X�L�������C���Əd�Ȃ��Ă��獶���̐����d�Ȃ����񐔃C���N�������g
		// ���Ȃ݂�1��ڂ̃��[�v(nScanY = �ŏ���y���W�̎���)�͐�΂ɒʂ�n�Y

		// [D3]
		// �]����̃X�L�������C�������̍s�ɍs��������
		// �]����̍����̃A�N�e�B�u���C���ɑΉ�����]�����̃A�N�e�B�u���C���̎n�I�_�� x���W�Ay���W���ǂꂾ���������邩
		// = (�]�����̍��̃��C���� x���W�̎n�I�_�̋���) / (�]����̃A�N�e�B�u���C���� y���W�̎n�I�_�̋��� + (1 or 0) )
		// = (�]�����̍��̃��C���� y���W�̎n�I�_�̋���) / (�]����̃A�N�e�B�u���C���� y���W�̎n�I�_�̋��� + (1 or 0) )
		// bContinual = true �������ꍇ�́An�������邽�߂�1��������B
		// �f�t�H���g�� n-1�����ƂȂ�B

		// [D4]
		// �]����̉E���̐��̎n�_���X�L�������C���Əd�Ȃ�Ȃ������ꍇ
		// ���������͂������Ȃ̂� if ���̏��������������ɂ����Ă�������
		// �ς� if ���̏������������ق��Ă���` (^^;

		// [D5]
		// �E���̐������̃X�L�������C���Əd�Ȃ��Ă���A�E���̐����d�Ȃ����񐔃C���N�������g
		// ���Ȃ݂ɂ����1��ڂ̃��[�v(nScanY = �ŏ���y���W�̎���)�͐�΂ɒʂ�n�Y

		// [D6]
		// �]����̃X�L�������C�������̍s�ɍs��������
		// �]����̉E���̃A�N�e�B�u���C���ɑΉ�����]�����̃A�N�e�B�u���C���̎n�I�_�� x���W�Ay���W���ǂꂾ���������邩
		// = (�]�����̍��̃��C���� x���W�̎n�I�_�̋���) / (�]����̃A�N�e�B�u���C���� y���W�̎n�I�_�̋��� + (1 or 0) )
		// = (�]�����̍��̃��C���� y���W�̎n�I�_�̋���) / (�]����̃A�N�e�B�u���C���� y���W�̎n�I�_�̋��� + (1 or 0) )
		// bContinual = true �������ꍇ�́An�������邽�߂�1��������B
		// �f�t�H���g�� n-1�����ƂȂ�B

		// [D7]
		// �]�����̃X�L�������C��(�̌X��)�����߂�B�܂�A
		// �]����̍����̃A�N�e�B�u���C���̌�_�ɑΉ������]�����̓_���n�_�Ƃ��A
		// �]����̉E���̃A�N�e�B�u���C���̌�_�ɑΉ������]�����̓_���I�_�Ƃ����������߂�B
		// �]����� x���W���C���N�������g����邽�тɓ]�����X�L�������C���� (x,y)���W���ǂꂾ���������邩
		// = (�]�����̍��̃��C���� x���W�̎n�I�_�̋���) / (�]����̃A�N�e�B�u���C���� x���W�̎n�I�_�̋���)
		// = (�]�����̍��̃��C���� y���W�̎n�I�_�̋���) / (�]����̃A�N�e�B�u���C���� �ux�v���W�̎n�I�_�̋���)
		// ���� x���W�̎n�I�_�̋������u0�v��������A�����(��ԏ�A�������͈�ԉ���)���_�Ȃ̂ŁA
		// �]�����X�L�������C���̌X�����u0�v�Ƃ���
		// �n�߂� (lRightDstCrsPntX - lLeftDstCrsPntX)>>POINT_SHIFT �Ƃ��Ă����̂����A���ꂾ�ƃ_��

		// [D8]
		// lpSrcBefore �ɑ����Z���Ă����ړ��ʂ��v�Z����ϐ���������
		// �N���b�s���O�̈悪�w�肳��Ă���΁A�]���O�ɑ�������������
		// ���X���̍��W�ɂ͎l�̌ܓ��p�� POINT_HALF ����������Ă���̂ł����ł� 0 �Ƃ���

		// [D9]
		// �����̃A�N�e�B�u���C����̓_(�X�L�������C���ƍ����̐��̌�_�� x���W)����
		// �E���̃A�N�e�B�u���C����̓_(�X�L�������C���ƉE���̐��̌�_�� x���W)�� x���W���C���N�������g���Ȃ���]��
		// �]����̃X�L�����|�C���^���̂��C���N�������g�����A�z��C���f�b�N�X�ŃA�N�Z�X

		// [D10]
		// �N���b�s���O�̈悪�w�肳��Ă��邩�ǂ����ɂ���āA�]���J�n�_�A�I���_�����߂�
		// �w�肳��Ă��Ȃ����(=NULL)�X�L�������C���ƃA�N�e�B�u���C���̌�_�����̂܂ܗp����B
		// �N���b�s���O�̈悪�w�肳��Ă���΁A�P�[�X�o�C�P�[�X�̎n�I�_�����߂Ă����B
		// �N���b�s���O�̃p�^�[���͈ȉ���6�ʂ肪�l������
		// [ : lpClipRect->left
		// ] : lpCliprect->right
		//
		//			�]����X�L�������C����
		//		 ���[					  �E�[
		//		  ��					   ��
		//(1)  [  ---------------------------  ]   ���[����E�[�܂őS���]��
		//(2) [	  ]--------------------------	   �]������
		//(3)	  ---------------------------[	 ] �]������
		//(4)	  [-----------]--------------	   ���[���� ] �܂œ]��
		//(5)	  ------[----------]---------	   [ ���� ] �܂œ]��
		//(6)	  ------------[-------------- ]	   [ ����E�[�܂œ]��
		//
		// ���Ȃ݂ɏ�}�́u---------�v�͓]����X�L�������C����\�����āA
		// ���[�� lLeftDstCrsPntX �ŁA�E�[�� lRightDstCrsPntX �ł���B
		// �����͌Œ菬���_�Ƃ��Ďg�p���Ă���̂ŁA
		// ����ɍ��킹�ăN���b�s���O�̈���w��RECT�\����(lpClipRect)���Œ菬���_�Ƃ��Ďg�p����
		// ���������[�v���ɖ��񖈉�V�t�g���Z����̂͂Ȃ�Ȃ̂ŁA
		// �e�X��\�ߌŒ菬���_�����Ă������ϐ�(lRectL, lRectR)���g�p����B

		// [D11]
		// �]����X�L�������C���� x���W���C���N�������g����邽�тɁA
		// �]�����X�L�������C�����ǂꂾ���������邩( x���W������, y���W������ )�����߂�
		// bContinual�t���O�� true �������� n ����
		// bContinual�t���O�� false �������� n-1 ���� �ƂȂ�B
		// �n�߂�
		// lSrcScanStepX = ( lRightSrcPntX - lLeftSrcPntX ) / ( DstDx + nContinual );
		// ������������ł̓_��

		// [D12]
		// �N���b�s���O�̈�̎w��ɂ���āA�]������y���W�̔�����s��
		// �w�肳��Ă���΁A�^�_��Ƃ̑召�֌W���r����B
		// �N���b�s���O��`�̏�[�� top�A���[���� -1 �����Ƃ���� bot
		// �^�_��� y���W�̍ŏ��_(��ԏ�ɂ���_)�� min�A�ő�_(��ԉ��ɂ���_)�� max �Ƃ����
		//
		// 1. top < bot < min < max , �]���̈斳��
		// 2. top < min < bot < max , �J�n�_:min �I���_:bot
		// 3. top < min < max < bot , �J�n�_:min �I���_:max
		// 4. min < top < bot < max , �J�n�_:top �I���_:bot
		// 5. min < top < max < bot , �J�n�_:top �I���_:max
		// 6. min < max < top < bot , �]���̈斳��
		//
		// �J�n�_�� top �������ꍇ�́A���̍��W�܂ŃX�L�������C����i�߂�̂ŁA
		// �A�N�e�B�u���C�������߂āA�|�C���^��i�߂Ȃ���΂Ȃ�Ȃ��B
		// �I���_�͂����P�� nEndY �ɃZ�b�g���邾��
		// ���Ȃ݂ɁA���� if���� ���������N���b�s���O�̈悪 NULL ���ǂ����̔�������Ă���̂ŁA
		// ��`�̍��[�ƉE�[���Œ菬���_���������ɂ���Ă���

		// �]�����A�]����̃v���[���̏��GET
		const LONG	lPitchSrc = lpSrcInfo->GetPitch();
		const LONG	lPitchDst = lpDstInfo->GetPitch();

		// �]�����A�]����̃X�L�����|�C���^
		// �]�����͖����[�v���Ɉ�ԍ���̓_(���_)����̈ړ������v�Z����
		// �]����͂܂��]���J�ny���W�܂ňړ������A���̌�͈�s���C���N�������g���Ă���
		SrcClass*	lpSrc;
		DstClass*	lpDst;
		// �]�����̃X�L�����|�C���^���ړ������鎞�ɖ��� GetPtr() ���ĂԂ̂��Ȃ�Ȃ̂ŁE�E�E
		const SrcClass*	lpSrcLeftTop = (const SrcClass*)lpSrcInfo->GetPtr();
		// �X�L�����|�C���^�̓]�����O�̃|�C���^���o���Ă����΁A����ɑ�����(Step)�𑫂���OK
		SrcClass*	lpSrcBefore;
//		DstClass*	lpDstBefore; // �ł��������͎g��Ȃ��Ă�OK

		// �]����̍����A�E���̊e���̎n�_(y���W�l�Ƃ��Ă͏I�_��菬�����͂�)���X�L�������C���Əd�Ȃ�����
		// ���̉񐔂��A���܂��ɃX�L�������̐�(�A�N�e�B�u���C��)�̔z��ԍ��ŁA�܂�A
		// LeftSrcLine[nLeftCrsCnt-1] �� RightSrcLine[nLeftCrcCnt-1] �̊ԂŃX�L�������Ƃ�������
		int nLeftCrsCnt = 0, nRightCrsCnt = 0;

		// �]���t���O bContinual �ɂ���� �������@��ύX
		// �X�L�������C���� ( �I�_���W�l - �n�_���W�l ) �����̂܂ܗp����� n-1 �����ɂȂ�̂ŁA
		// true �̎��� 1�Afalse �̎� 0 �Ƃ���΁A���� nContinual ���������
		// true:n����, false:(n-1)���� �ƂȂ�
		// �]����̐}�`����`�̏ꍇ�̂� true ���w��ł���B�ʏ�� false�B
		const int nContinual = (bContinual ? 1 : 0);

		// �]����̃A�N�e�B�u���C���ƃX�L�������C���Ƃ�������Ă鍶���ƉE���A�e�X�̌�_�� x���W
		// ����� x�����̃X�e�b�v�l�������Ă����΁A���C����̓_�����߂�̂��y
		// �Œ菬���_�Ƃ��Ďg�p����B
		LONG lLeftDstCrsPntX=0, lRightDstCrsPntX=0;

		// ���ꂪ���̃X�e�b�v�l(=dx/dy)�ł���
		LONG lLeftDstStepX=0, lRightDstStepX=0;

		// �]����̃A�N�e�B�u���C���ƃX�L�������C���Ƃ̌�_�ɑΉ�����]�����̍��W�l
		// �]���悪�C���N�������g�����ƁA���̍��W�l�ɃX�e�b�v�l�𑫂���OK
		// �Œ菬���_�Ƃ��Ďg�p
		LONG lLeftSrcPntX=0, lRightSrcPntX=0, lLeftSrcPntY=0, lRightSrcPntY=0;

		// ���ꂪ���̃X�e�b�v�l(=dy/dx)
		// �]����̃X�L�������C���̃C���N�������g�ɂ��A�]�����̃��C�����ǂꂾ���������邩
		// �Œ菬���_�Ƃ��Ďg�p
		LONG lLeftSrcStepX=0, lRightSrcStepX=0, lLeftSrcStepY=0, lRightSrcStepY=0;

		// �]�����̃X�L�������C���̃X�e�b�v�l
		// �]�����̃X�L�������C���̎n�_(lLeftSrcPntX, lLeftSrcPntY)�������_�Ƃ���
		// ���̃X�e�b�v�l�������Ă����A�I�_(lRightSrcPntX, lRightSrcPntY)�܂ł��ǂ蒅��
		// �Œ菬���_�Ƃ��Ďg�p
		LONG lSrcScanStepX=0, lSrcScanStepY=0;

		// �]����̑����� dx , dy
		LONG DstDx=0, DstDy=0;

		// �]�����X�L�����|�C���^���ړ�������̂Ɏg�p����ϐ�
		LONG lSrcScanPntX=0, lSrcScanPntY=0;

		// �����Ƃ��ē��͂��ꂽ�N���b�s���O�̈���w��RECT�\���̂��Œ菬���_����������
		LONG lRectL=0, lRectR=0; // ���ӂƉE�ӂ� x���W�������K�v

		// �]����̃X�L�������C���̍��W�l
		LONG nScanX=0, nScanY=0; // ���[�v���Ɏg�p����
		LONG nEndX=0,	 nEndY=0;	 // �I�����W�l�A�N���b�s���O�̗L�閳���ɂ���ĕς�邩��

		// [D12]
		if( lpClipRect == NULL ){ // �N���b�s���O����������

			nScanY = lpDstPoint[MinIdx].y;
			nEndY  = lpDstPoint[MaxIdx].y;
		}
		else{ // �N���b�s���O�w��

			// �܂��A�Œ菬���_���ARECT�̉E�[�͓_�ɍ��킹�āu-1�v���Ă���̂ɒ���
			lRectL = (lpClipRect->left		  << POINT_SHIFT) + POINT_HALF;
			lRectR = (( lpClipRect->right-1 ) << POINT_SHIFT) + POINT_HALF;
			// �����Ƃ͎v�����ǁA�ꉞ��`�̍��E�`�F�b�N
			WARNING( lRectR < lRectL ,"CFastPlaneEffect::Morph�� ��` lpClipRect�� left �� right ���t����");

			// �����Ⴄ���ጩ�ɂ����̂ňꎞ�I�Ɏg�p
			LONG min = lpDstPoint[MinIdx].y;
			LONG max = lpDstPoint[MaxIdx].y;
			LONG top = lpClipRect->top;			
			LONG bot = lpClipRect->bottom - 1; // RECT�\���̂�POINT���r���邽�� -1 ����

			// �����Ƃ͎v�����ǁA�ꉞ��`�̏㉺�`�F�b�N
			WARNING( bot < top,"CFastPlaneEffect::Morph�� ��` lpClipRect�� top �� bottom ���t����");

			if( bot < min || max < top ){ // ��L 1. or 6.
				return 0; // �]���̈悠��܂ւ�Ł`
			}

			// �]���J�n�_�����߂�
			if( top <= min ){ // ��L 2. or 3.
				nScanY = min;
			}
			else{ // ��L 4. or 5.
				nScanY = top;

				// ���̏ꍇ�̓X�L�����|�C���^�� (top-1) �܂Ői�߂Ȃ��Ƃ����Ȃ�
				top--; // ���� top-1 ������̂͂߂�ǂ������̂�

				// �܂��A�]����� y���W (top-1) �ƌ������͍����A�E�����ꂼ�ꉽ�Ԗڂ̐���(=���񒸓_�Əd�Ȃ�����)�����߂�

				for( nLeftCrsCnt++; nLeftCrsCnt<nLeftLine; nLeftCrsCnt++ ){ // �܂��͍����̐��͉��Ԗڂ�
					if( LeftDstLine[nLeftCrsCnt-1].Start.y <= top && top < LeftDstLine[nLeftCrsCnt-1].End.y ){
						break;
					}
				}

				for( nRightCrsCnt++; nRightCrsCnt<nRightLine; nRightCrsCnt++ ){ // ���ɉE���̐��͉��Ԗڂ�
					if( RightDstLine[nRightCrsCnt-1].Start.y <= top && top < RightDstLine[nRightCrsCnt-1].End.y ){
						break;
					}
				}

				// ���̋��܂������̎n�I�_���X��(dx/dy)�����߂āA((top-1) - ���̐��̎n�_)*(dx/dy)�ɂ�� x���W�����߂�
				// �܂��]����̐��ԍ��ɑΉ������]�����̐������l�� (top-1) �̎��� x���W�Ay���W�����߂�

				// �܂��͍��� (�R�����g�͉��̃��[�v�œ����������Ă�̂ŁA��������Q��
				lLeftDstCrsPntX = (LeftDstLine[nLeftCrsCnt-1].Start.x << POINT_SHIFT) + POINT_HALF;
				DstDy = LeftDstLine[nLeftCrsCnt-1].End.y - LeftDstLine[nLeftCrsCnt-1].Start.y;
				WARNING( DstDy == 0 , "CFastPlaneEffect::Morph�� �����̐��ɐ����Ȑ�����" );
				lLeftDstStepX = ( (LeftDstLine[nLeftCrsCnt-1].End.x - LeftDstLine[nLeftCrsCnt-1].Start.x) << POINT_SHIFT )
								/ ( DstDy );
				lLeftSrcPntX = (LeftSrcLine[nLeftCrsCnt-1].Start.x << POINT_SHIFT) + POINT_HALF;
				lLeftSrcPntY = (LeftSrcLine[nLeftCrsCnt-1].Start.y << POINT_SHIFT) + POINT_HALF;
				lLeftSrcStepX= ( (LeftSrcLine[nLeftCrsCnt-1].End.x - LeftSrcLine[nLeftCrsCnt-1].Start.x) << POINT_SHIFT )
							   / ( DstDy + nContinual );
				lLeftSrcStepY= ( (LeftSrcLine[nLeftCrsCnt-1].End.y - LeftSrcLine[nLeftCrsCnt-1].Start.y) << POINT_SHIFT)
							   / ( DstDy + nContinual );

				int dy = top - LeftDstLine[nLeftCrsCnt-1].Start.y; // ����X�e�b�v�l�𑫂��΂�����
				lLeftDstCrsPntX += lLeftDstStepX * dy; // �]���捶�� x���W
				lLeftSrcPntX	+= lLeftSrcStepX * dy; // �]�������� x���W
				lLeftSrcPntY	+= lLeftSrcStepY * dy; // �]�������� y���W

				// ���ɉE�� (�R�����g�͓��l�Ɉȉ��̃��[�v���Q��)
				lRightDstCrsPntX = (RightDstLine[nRightCrsCnt-1].Start.x << POINT_SHIFT) + POINT_HALF;
				DstDy = RightDstLine[nRightCrsCnt-1].End.y - RightDstLine[nRightCrsCnt-1].Start.y;
				WARNING( DstDy == 0 , "CFastPlaneEffect::Morph�� �E���̐��ɐ����Ȑ�����" );
				lRightDstStepX = ( (RightDstLine[nRightCrsCnt-1].End.x - RightDstLine[nRightCrsCnt-1].Start.x) << POINT_SHIFT )
								 / ( DstDy );
				lRightSrcPntX = (RightSrcLine[nRightCrsCnt-1].Start.x << POINT_SHIFT) + POINT_HALF;
				lRightSrcPntY = (RightSrcLine[nRightCrsCnt-1].Start.y << POINT_SHIFT) + POINT_HALF;
				lRightSrcStepX = ( (RightSrcLine[nRightCrsCnt-1].End.x - RightSrcLine[nRightCrsCnt-1].Start.x) << POINT_SHIFT )
								 / ( DstDy + nContinual );
				lRightSrcStepY = ( (RightSrcLine[nRightCrsCnt-1].End.y - RightSrcLine[nRightCrsCnt-1].Start.y) << POINT_SHIFT )
								 / ( DstDy + nContinual );

				dy = top - RightDstLine[nRightCrsCnt-1].Start.y; // ����X�e�b�v�l�𑫂��΂�����
				lRightDstCrsPntX += lRightDstStepX * dy; // �]����E�� x���W
				lRightSrcPntX	 += lRightSrcStepX * dy; // �]�����E�� x���W
				lRightSrcPntY	 += lRightSrcStepY * dy; // �]�����E�� y���W

			}

			// �]���I���_�����߂�
			if( max <= bot ){ // ��L 3. or 5.
				nEndY = max;
			}
			else{ // ��L 2. or 4.
				nEndY = bot;
			}
		}

		// �]����X�L�����|�C���^���X�L�����J�n���C���̐擪�Ɉړ�
		lpDst = (DstClass*)((BYTE*)lpDstInfo->GetPtr()+(lPitchDst * nScanY) );

		// �]����̃X�L�������C����1�s���i�߂Ă���(�܂� y���W���C���N�������g���Ă���)
		for( ; nScanY <= nEndY; nScanY++ ){

			if( !( LeftDstLine[nLeftCrsCnt].Start.y == nScanY ) ){ // [D1]

				// �]����̍����̃A�N�e�B�u���C���ƃX�L�������C���̌�_�� x���W�����߂�
				lLeftDstCrsPntX += lLeftDstStepX;

				// �]����̍����̃A�N�e�B�u���C���ɑΉ�����]�����̃A�N�e�B�u���C���̃X�L�����J�n���W�����߂�
				lLeftSrcPntX += lLeftSrcStepX;
				lLeftSrcPntY += lLeftSrcStepY;

				// �]�����̃X�L�����|�C���^���ړ� �I�[�o�[�t���[���邩������Ȃ��̂ŁA��ɃV�t�g���Z
				lpSrc = (SrcClass*)( (BYTE*)lpSrcLeftTop
									 + (lLeftSrcPntY >> POINT_SHIFT) * lPitchSrc
									 + (lLeftSrcPntX >> POINT_SHIFT) * sizeof(SrcClass) );

			}
			else{ // [D2]

				nLeftCrsCnt++; // �d�Ȃ����񐔃C���N�������g

				// �����l�Ƃ��ē]����̍����̃A�N�e�B�u���C���̎n�_�� x���W����
				// �l�̌ܓ����邽�߂ɌŒ菬���_�̐��E�ł� 0.5 �𑫂��Ă���
				lLeftDstCrsPntX = (LeftDstLine[nLeftCrsCnt-1].Start.x << POINT_SHIFT) + POINT_HALF;

				// ���̍����̃A�N�e�B�u���C���̌X�� y���W��1�����閈�� x���W���ǂꂾ���������邩(=dx/dy)
				// �X�������̏ꍇ�ƕ��̏ꍇ�ł̏ꍇ�������K�v!!�����͗v��Ȃ����� (^^;
				DstDy = LeftDstLine[nLeftCrsCnt-1].End.y - LeftDstLine[nLeftCrsCnt-1].Start.y;
				WARNING( DstDy == 0 , "CFastPlaneEffect::Morph�� �����̐��ɐ����Ȑ�����" );

				lLeftDstStepX = ( (LeftDstLine[nLeftCrsCnt-1].End.x - LeftDstLine[nLeftCrsCnt-1].Start.x) << POINT_SHIFT )
								/ ( DstDy );

				// �����l�Ƃ��āA�]����̍����̃A�N�e�B�u���C���ɑΉ������]�����̃��C���̎n�_ x���W�l�Ay���W�l����
				lLeftSrcPntX = (LeftSrcLine[nLeftCrsCnt-1].Start.x << POINT_SHIFT) + POINT_HALF;
				lLeftSrcPntY = (LeftSrcLine[nLeftCrsCnt-1].Start.y << POINT_SHIFT) + POINT_HALF;

				// [D3]
				lLeftSrcStepX= ( (LeftSrcLine[nLeftCrsCnt-1].End.x - LeftSrcLine[nLeftCrsCnt-1].Start.x) << POINT_SHIFT )
							   / ( DstDy + nContinual );
				lLeftSrcStepY= ( (LeftSrcLine[nLeftCrsCnt-1].End.y - LeftSrcLine[nLeftCrsCnt-1].Start.y) << POINT_SHIFT)
							   / ( DstDy + nContinual );

				// �X�L�����|�C���^��]�����̃A�N�e�B�u���C���̎n�_�ɃZ�b�g
				lpSrc = (SrcClass*)( (BYTE*)lpSrcLeftTop
									 + LeftSrcLine[nLeftCrsCnt-1].Start.y * lPitchSrc
									 + LeftSrcLine[nLeftCrsCnt-1].Start.x * sizeof(SrcClass) );
			}

			if( ! (RightDstLine[nRightCrsCnt].Start.y == nScanY ) ){ // [D4]

				// �]����̉E���̃A�N�e�B�u���C���ƃX�L�������C���̌�_�� x���W�����߂�
				lRightDstCrsPntX += lRightDstStepX;

				// �]����̉E���̃A�N�e�B�u���C���ɑΉ�����]�����̃A�N�e�B�u���C���̃X�L�����I�����W�����߂�
				lRightSrcPntX += lRightSrcStepX;
				lRightSrcPntY += lRightSrcStepY;
			}
			else{ // [D5]

				nRightCrsCnt++; // �d�Ȃ����񐔃C���N�������g

				// �����l�Ƃ��ē]����̉E���̃A�N�e�B�u���C���̎n�_�� x���W����
				lRightDstCrsPntX = (RightDstLine[nRightCrsCnt-1].Start.x << POINT_SHIFT) + POINT_HALF;

				// ���̒��_�܂ł�y���W�̋������v�Z
				DstDy = RightDstLine[nRightCrsCnt-1].End.y - RightDstLine[nRightCrsCnt-1].Start.y;
				WARNING( DstDy == 0 , "CFastPlaneEffect::Morph�� �E���̐��ɐ����Ȑ�����" );

				// ���̉E���̃A�N�e�B�u���C���̌X�� y���W��1�����閈�� x���W���ǂꂾ���������邩(=dx/dy)
				lRightDstStepX = ( (RightDstLine[nRightCrsCnt-1].End.x - RightDstLine[nRightCrsCnt-1].Start.x) << POINT_SHIFT )
								 / ( DstDy );

				// �����l�Ƃ��āA�]����̉E���̃A�N�e�B�u���C���ɑΉ������]�����̃��C���̎n�_ x���W�l�Ay���W�l����
				lRightSrcPntX = (RightSrcLine[nRightCrsCnt-1].Start.x << POINT_SHIFT) + POINT_HALF;
				lRightSrcPntY = (RightSrcLine[nRightCrsCnt-1].Start.y << POINT_SHIFT) + POINT_HALF;

				// [D6]
				lRightSrcStepX = ( (RightSrcLine[nRightCrsCnt-1].End.x - RightSrcLine[nRightCrsCnt-1].Start.x) << POINT_SHIFT )
								 / ( DstDy + nContinual );
				lRightSrcStepY = ( (RightSrcLine[nRightCrsCnt-1].End.y - RightSrcLine[nRightCrsCnt-1].Start.y) << POINT_SHIFT )
								 / ( DstDy + nContinual );
			}

			// [D7]
			DstDx = (lRightDstCrsPntX>>POINT_SHIFT) - (lLeftDstCrsPntX>>POINT_SHIFT);
			WARNING( DstDx < 0 , "CFastPlaneEffect::Morph�� �X�L�������C���� x���W���E���t" );

			if( DstDx != 0 ){ // ���_����Ȃ�����

				// [D11]
				lSrcScanStepX = ( ( (lRightSrcPntX>>POINT_SHIFT) - (lLeftSrcPntX>>POINT_SHIFT) ) << POINT_SHIFT )
								/ ( DstDx + nContinual );
				lSrcScanStepY = ( ( (lRightSrcPntY>>POINT_SHIFT) - (lLeftSrcPntY>>POINT_SHIFT) ) << POINT_SHIFT )
								/ ( DstDx + nContinual );
			}
			else{ // ���_������(���_����Ȃ��Ă����_�ɋɋߖT�������炱�����ɂȂ�)

				lSrcScanStepX = lSrcScanStepY = 0; // �ǂ�����񂵂��]�����[�v�����Ȃ��̂� 0�ɂ��Ă����Ă������ł��傤
			}

			lpSrcBefore	 = lpSrc; // ���̃|�C���^�Ɉڂ鎞�Ɋy����l�ɓ]�����̎n�_�̃|�C���^���o���Ă���
			lSrcScanPntX = lSrcScanPntY = POINT_HALF; // [D8]

			// [D10]
			if( lpClipRect == NULL ){ // �N���b�v�̈悪��������

				// �Œ菬���_������
				nScanX = lLeftDstCrsPntX  >> POINT_SHIFT; // �]���J�n���W�l
				nEndX  = lRightDstCrsPntX >> POINT_SHIFT; // �]���I�����W�l

			}
			else{ // �N���b�s���O�̈悪�w�肳��Ă����̂œ]�����������߂�

				if( lRectR < lLeftDstCrsPntX || lRightDstCrsPntX < lRectL ){ // ��} (2) or (3)
					// �]���̈悠��܂ւ�`�A�]����̃X�L�����|�C���^�����̍s�ɂ��Ď��̍s�֌�������
					lpDst = (DstClass*)((BYTE*)lpDst + lPitchDst );
					continue;
				}

				// �]���J�n x���W�l�����߂�
				if( lRectL <= lLeftDstCrsPntX ){ // ��} (1) or (4)
					nScanX = lLeftDstCrsPntX >> POINT_SHIFT;
				}
				else{ // ��} (5) or (6)
					nScanX = lpClipRect->left;

					// �]�����̃X�L�����|�C���^���ړ����ɂ�Ȃ��
					int dx = (lRectL - lLeftDstCrsPntX) >> POINT_SHIFT; // �ړ���

					lSrcScanPntX = lSrcScanStepX * dx; // �ړ����ɑ����ʂ�������
					lSrcScanPntY = lSrcScanStepY * dx; // �]�����J�n�_�����߂�

					// �]�����̃X�L�����|�C���^��]���J�n�_�ֈړ�
					lpSrc = (SrcClass*) ((BYTE*)lpSrcBefore
										+ (lSrcScanPntY >> POINT_SHIFT) * lPitchSrc
										+ (lSrcScanPntX >> POINT_SHIFT) * sizeof(SrcClass) );
				}

				// �]���I�� x���W�l�����߂�
				if( lRightDstCrsPntX <= lRectR ){ // ��} (1) or (6)
					nEndX = lRightDstCrsPntX >> POINT_SHIFT;
				}
				else{ // ��} (4) or (5)
					nEndX = lpClipRect->right-1; // ��`�̉E�[�� -1 ���Ƃ��ɂ�Ȃ��
				}


			}

			for( ; nScanX <= nEndX; nScanX++ ){ // [D9]

				f(lpDst[nScanX],*lpSrc); // �]��

				lSrcScanPntX += lSrcScanStepX; // �]�����̃X�L�����|�C���^��
				lSrcScanPntY += lSrcScanStepY; // ���̍��W�l�܂ł̑��΋���

				// �]�����̃X�L�����|�C���^�𑝉� �I�[�o�[�t���[���邩������Ȃ��̂Ő�ɃV�t�g���Z
				lpSrc = (SrcClass*) ((BYTE*)lpSrcBefore
									+ (lSrcScanPntY >> POINT_SHIFT) * lPitchSrc
									+ (lSrcScanPntX >> POINT_SHIFT) * sizeof(SrcClass) );

			} // �]����X�L�������C���� x���W�C���N�������g���[�v

			lpDst = (DstClass*)((BYTE*)lpDst + lPitchDst ); // �]����̃X�L�����|�C���^�����̍s��

		} // �]����X�L�������C���� y���W�C���N�������g���[�v

		delete [] LeftDstLine;
		delete [] RightDstLine;
		delete [] LeftSrcLine;
		delete [] RightSrcLine;

		return 0;

	}

	//	Blt�Ɠ��������Afunctor�ɓ]������(x,y)���n��
	//	---- �]���̂Ƃ��ɂȂ�₩�₷��֐�
	template <class SrcClass,class DstClass,class BltFunctor>
	static LRESULT BltSrcXY(SrcClass _src,CSurfaceInfo* lpSrcInfo,DstClass _dst
	,CSurfaceInfo* lpDstInfo,BltFunctor f,int x,int y,LPCRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPCRECT lpClipRect=NULL){

		WARNING(lpSrcInfo->GetPtr() == NULL,"CFastPlaneEffect::Blt��lpSrcInfo->GetPtr() == NULL");
		WARNING(lpDstInfo->GetPtr() == NULL,"CFastPlaneEffect::Blt��lpDstInfo->GetPtr() == NULL");

		//	LONG lPitch	 = pInfo->GetPitch();
		//	DWORD* pSurface = pInfo->GetPtr();

		//	CDIB32Base* srcp = lpSrcDIB->GetDIB32BasePtr();
		//	CDIB32Base* dstp = lpDstDIB->GetDIB32BasePtr();

		// �N���b�s���O���̏���
		// �]���͈͂Ȃ��̎��́A���̂܂܋A��
		CFastPlaneEffectClipper clip;
		if ( Clipping( lpDstInfo , lpSrcInfo , pInfo , &clip ) != 0 ) return 1;
		
		RECT rcSrcRect = clip.rcSrcRect;
		RECT rcDstRect = clip.rcDstRect;
//		RECT rcClipRect = clip.rcClipRect;
		
		// �]����̉����Əc���̐ݒ�
		int		nDstWidth = rcDstRect.right - rcDstRect.left;
		int		nDstHeight = rcDstRect.bottom - rcDstRect.top;
		int		nSrcWidth = rcSrcRect.right - rcSrcRect.left;
		LONG	lPitchSrc = lpSrcInfo->GetPitch();
		LONG	lPitchDst = lpDstInfo->GetPitch();
		LONG	nAddSrcWidth = lpSrcInfo->GetPitch() - (nSrcWidth * sizeof(SrcClass));	// �N���b�s���O���Ĕ�΂����̎Z�o
		LONG	nAddDstWidth = lpDstInfo->GetPitch() - (nDstWidth * sizeof(DstClass));	// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
		SrcClass*	lpSrc = (SrcClass*)((BYTE*)lpSrcInfo->GetPtr() +(rcSrcRect.left*sizeof(SrcClass))+rcSrcRect.top*lPitchSrc );		// �N���b�s���O�����̃J�b�g
		DstClass*	lpDst = (DstClass*)((BYTE*)lpDstInfo->GetPtr() +(rcDstRect.left*sizeof(DstClass))+rcDstRect.top*lPitchDst );		// �w�肳�ꂽx,y�̈ʒu����

		if ( clip.bActualSize ){
			//���{

			//	'01/10/05	��˂��炨����
			//	8���[�v�W�J
			int nLoop8 = nDstWidth;
			//	8���[�v�]��
			int nLoop8mod = nLoop8 & 7;
			nLoop8 >>= 3;

			nAddSrcWidth+=nLoop8mod*sizeof(SrcClass);
			nAddDstWidth+=nLoop8mod*sizeof(DstClass);

			int nSrcY = rcSrcRect.top;

			for(int y=0;y<nDstHeight;y++){
				int nSrcX = rcSrcRect.left;
				for(int n=0;n<nLoop8;n++){
					//	�]��������]����ցA���̊֐��G�t�F�N�g�������Ȃ���]��
					f(lpDst[0],lpSrc[0],nSrcX+0,nSrcY);
					f(lpDst[1],lpSrc[1],nSrcX+1,nSrcY);
					f(lpDst[2],lpSrc[2],nSrcX+2,nSrcY);
					f(lpDst[3],lpSrc[3],nSrcX+3,nSrcY);
					f(lpDst[4],lpSrc[4],nSrcX+4,nSrcY);
					f(lpDst[5],lpSrc[5],nSrcX+5,nSrcY);
					f(lpDst[6],lpSrc[6],nSrcX+6,nSrcY);
					f(lpDst[7],lpSrc[7],nSrcX+7,nSrcY);
					lpDst+=8; lpSrc+=8; nSrcX+=8;
				}
				//	�]�蕪���A�]��
				for (n=0;n<nLoop8mod;n++){
					f(lpDst[n],lpSrc[n],nSrcX+n,nSrcY);
				}
				//	���[�v�̊O�ŃC���N�������g�����ق����A�����I
				//	lpDst+=nLoop8mod; lpSrc+=nLoop8mod;
				//	������́A����nAddDst/SrcWidth�Ɏ��O���Z���Ă���I

				lpDst = (DstClass*) ( (LPBYTE)lpDst+nAddDstWidth);
				lpSrc = (SrcClass*) ( (LPBYTE)lpSrc+nAddSrcWidth);
				nSrcY++;
			}
		}else{
			// �񓙔{
			int		nInitialX = clip.nInitialX ;	//	-DX�@ :�@�Â̏����l = -DX
			int		nStepX = clip.nStepX;		//	 2*SX :�@��+=2*SX
			int		nCmpX = clip.nCmpX;		//	 2*DX :�@��>0�Ȃ�΃�-=2*DX���Ă�
			int		nStepsX = clip.nStepsX;	//	 SrcX�̈��̉��Z��(������)
			int		nInitialY = clip.nInitialY;
			int		nStepY = clip.nStepY;
			int		nCmpY = clip.nCmpY;
			int		nStepsY = clip.nStepsY;
			DWORD	AddSrcPixel = sizeof(SrcClass) * nStepsX;
			DWORD	AddWidthSrc = lpSrcInfo->GetPitch() * nStepsY;
			DWORD	nAddSrcHeight= lpSrcInfo->GetPitch() * nStepsY;						// y���̐������ŉ��Z�����l
//			DWORD	nAddDstWidth = lpDstInfo->GetPitch() - (nWidth*sizeof(DstClass));	// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
			int		EIX= nInitialX;
			int		EIY= nInitialY;
			int		EX = nStepX;
			int		EY = nStepY;
			int		EX2= nCmpX;
			int		EY2= nCmpY;
			int		i, j;
			int		nExCnt, nEyCnt;
			nEyCnt = EIY;

			int nSrcY = rcSrcRect.top;

			for ( j = 0 ; j < nDstHeight ; j++ )
			{
				nEyCnt += EY;											// �]����Y�̏������̉��Z
				if ( nEyCnt >= 0 )
				{
					lpSrc = (SrcClass*)((BYTE*)lpSrc + lPitchSrc );		// �]����Y�����̃��C���ɂ���
					nSrcY ++;
					nEyCnt -= EY2;										// Y�̕␳�l
				}

				SrcClass*	lpSrcBack = lpSrc;
				int nSrcX = rcSrcRect.left;
				nExCnt = EIX;

				for ( i = 0 ; i < nDstWidth ; i++ )
				{
					nExCnt += EX;		// �]������X�̏������̉��Z
					if ( nExCnt >= 0 )
					{
						lpSrc++;
						nSrcX++;
						nExCnt -= EX2;	// X�̕␳�l
					}

					f(lpDst[i],*lpSrc,nSrcX,nSrcY);
					// �]������X�̐������̉��Z
					lpSrc = (SrcClass*)((BYTE*)lpSrc + AddSrcPixel);
					nSrcX += nStepsX;

//					lpDst++;			//	�]�����X�����Z
				}
				lpSrc = (SrcClass*)((BYTE*)lpSrcBack + nAddSrcHeight );	// �]����X���[�v�Ői�񂾕��߂��Ay���̐����������Z����
				nSrcY += nStepsY;

				lpDst = (DstClass*)((BYTE*)lpDst + lPitchDst );		//	�]����̐������̉��Z
			}
		}
		
		return 0;
	}
#pragma warning(disable:4701) // �l�����蓖�Ă��ĂȂ��ϐ�����warning

protected:
	class CFastPlaneEffectClipper {
	public:
		bool	bActualSize;
		RECT	rcSrcRect;
		RECT	rcDstRect;
		RECT	rcClipRect;
		// Bresenham
		int		nInitialX;	//	-DX�@ :�@�Â̏����l = -DX
		int		nStepX;		//	 2*SX :�@��+=2*SX
		int		nCmpX;		//	 2*DX :�@��>0�Ȃ�΃�-=2*DX���Ă�
		int		nStepsX;	//	 SrcX�̈��̉��Z��(������)
		int		nInitialY;
		int		nStepY;
		int		nCmpY;
		int		nStepsY;
	};

	static LRESULT Clipping(CSurfaceInfo*lpDstInfo,const CSurfaceInfo*lpSrcInfo,
		const CSurfaceInfo::CBltInfo* pInfo,
		CFastPlaneEffectClipper* lpClipper);

	//	�C����������֐��ǉ����邩���ˁB�i���Ȃ������ˁO�O�G�j
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif	//	__yaneGTL_h__
