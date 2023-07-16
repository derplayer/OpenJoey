#include "stdafx.h"
#include "yaneGTL.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

BYTE	CFastPlaneBlendTable::abyMulTable[256*256];
BYTE	CFastPlaneBlendTable::abyMulTable2[256*256];
WORD	CFastPlaneBlendTable::awdMulTableRGB565[65536*16];
WORD	CFastPlaneBlendTable::awdMulTableRGB555[65536*16];
/*
WORD	CFastPlaneBlendTable::awdAddTableRGB565[65536*8];
WORD	CFastPlaneBlendTable::awdAddTableRGB555[65536*8];
WORD	CFastPlaneBlendTable::awdSubTableRGB565[65536*8];
WORD	CFastPlaneBlendTable::awdSubTableRGB555[65536*8];
*/
BYTE	CFastPlaneBlendTable::abyConvertTable555[32768];
	//	����Œx����΁A32bpp�ɑ΂��Ă��A���������̂�p�ӂ��邪�D�D�D

void	CFastPlaneBlendTable::InitTable()
	{
		{
			for(int i=0;i<256;i++){
				for(int j=0;j<256;j++){
					abyMulTable[i + (j<<8)] = (BYTE)(i*j/255);
					//	����narrowing�ϊ����I�[�o�[�t���[���Ȃ����Ƃ́A
					//	255*255/255 == 255�ł��邱�Ƃ��疾�炩�B
					abyMulTable2[i + (j<<8)] = (BYTE)(i*(255-j)/255);
				}
			}
		}

		{
			//	for rgb565 & argb3565
			for(int a=0;a<16;a++){
			 for(int r=0;r<32;r++){
			  for(int g=0;g<64;g++){
			   for(int b=0;b<32;b++){
					DWORD dw = b + (g<<5) + (r<<11) + (a<<16);
					WORD data;
					data= b*a/15 + (g*a/15 << 5) + (r*a/15 <<11);
					awdMulTableRGB565[dw] = data;

/*
	//	�O�a���Z����ь��Z�̃e�[�u�����͂��؁[�O�O�G

					//	����ȂƂ����if�����Ȃ��Ă��O�O�G
					if (a<8) {
						data = ((b+a5>31)?31:b+a5) + (((g+a6>63)?63:g+a6)<<5) + (((r+a5)>31?31:r+a5)<<11);
						awdAddTableRGB565[dw] = data;
						data = ((b<a5)?0:b-a5) + (((g<a6)?0:g-a6)<<5) + (((r<a5)?0:r-a5)<<11);
						awdSubTableRGB565[dw] = data;
					}
*/

			   }
			  }
			 }
			}
		}
		{
			//	for rgb555 & argb3555
			for(int a=0;a<16;a++){
			 for(int r=0;r<32;r++){
			  for(int g=0;g<32;g++){
			   for(int b=0;b<32;b++){
					DWORD dw =	b + (g<<5) + (r<<10) + (a<<16);
					WORD  data;
					data = b*a/15 + (g*a/15 << 5) + (r*a/15 <<10);
					awdMulTableRGB555[dw		  ] = data;
					awdMulTableRGB555[dw + (1<<15)] = data; // �s�͂��ȃr�f�I�J�[�h�΍�O�O�G
					//	�������́A�{���́A���̔����ŗǂ��̂����A
					//	�ŏ�ʂ��s��ɂȂ��Ă���\��������̂ŁA�r�b�g�}�X�N��
					//	�Ƃ�̂����邢�̂ŁA����ł����Ă܂��I�O�O�G

/*
	//	�O�a���Z�̃e�[�u�����͂��؁[�i�΁j

					//	����ȂƂ����if�����Ȃ��Ă��O�O�G
					if (a<8) {
						data = ((b+a5>31)?31:b+a5) + (((g+a6>63)?63:g+a6)<<5) + (((r+a5)>31?31:r+a5)<<11);
						awdAddTableRGB555[dw] = data;
						awdAddTableRGB555[dw + (1<<15)] = data; // �s�͂��ȃr�f�I�J�[�h�΍�O�O�G
						data = ((b<a5)?0:b-a5) + (((g<a6)?0:g-a6)<<5) + (((r<a5)?0:r-a5)<<11);
						awdSubTableRGB555[dw] = data;
						awdSubTableRGB555[dw + (1<<15)] = data; // �s�͂��ȃr�f�I�J�[�h�΍�O�O�G
					}
*/

			   }
			  }
			 }
			}
		}
	}

void	CFastPlaneBlendTable::OnChangePalette(){
	HDC hdc = ::GetDC(NULL);
	if (hdc==NULL) return ;
	PALETTEENTRY pal[256];
	int nPal = GetSystemPaletteEntries(hdc,0,256,&pal[0]);
	//	�p���b�g��256,16,4,2�ł��邱�Ƃ����肤��
	ReleaseDC(NULL,hdc);
	//	��256�F���[�h�ł́A��������
	if (nPal == 0) return ;

	for(int r=0;r<32;r++){
	 for(int g=0;g<32;g++){
	  for(int b=0;b<32;b++){
		DWORD dw =	b + (g<<5) + (r<<10);
		BYTE byR=(r << 3),byG=(g << 3),byB=(b << 3);
		BYTE nNearest = 0;
		// 0����������K�v�͂Ȃ����A0������Ă����Ȃ��Ɓu�l�����蓖�ĂĂ��Ȃ��ϐ��ɑ΂���v�Q�Ƃ�warning���o��
		int nMinDistance=INT_MAX;
		for(int i=0;i<nPal;i++){
			BYTE byR2 = pal[i].peRed;
			BYTE byG2 = pal[i].peGreen;
			BYTE byB2 = pal[i].peBlue;

			//	���`�����ōl����ƁH
			int nDistance =
				abs(byR-byR2) + abs(byG-byG2) + abs(byB-byB2);

			if (nDistance < nMinDistance){
				nMinDistance = nDistance;
				nNearest = (BYTE) i;
			}
		}
		abyConvertTable555[dw] = nNearest;
	  }
	 }
	}
}

///////////////////////////////////////////////////////////////////////////////

//	�N���b�p�[
LRESULT CFastPlaneEffect::Clipping(CSurfaceInfo*lpDstInfo,const CSurfaceInfo*lpSrcInfo,const CSurfaceInfo::CBltInfo* pInfo,CFastPlaneEffectClipper* lpClipper){
	int		nSsizeX, nSsizeY, nDsizeX, nDsizeY;
	RECT rcSrcRect,rcDstRect,rcClipRect;
	int		nInitialX;	//	-DX�@ :�@�Â̏����l = -DX
	int		nStepX;		//	 2*SX :�@��+=2*SX
	int		nCmpX;		//	 2*DX :�@��>0�Ȃ�΃�-=2*DX���Ă�
	int		nStepsX;	//	 SrcX�̈��̉��Z��(������)
	int		nInitialY;
	int		nStepY;
	int		nCmpY;
	int		nStepsY;

	if (pInfo->pSrcRect==NULL) {
		::SetRect(&rcSrcRect,0,0,lpSrcInfo->GetSize().cx,lpSrcInfo->GetSize().cy);
	} else {
		//--- �C�� '02/12/04  by ENRA ---
//		rcSrcRect = *pInfo->pSrcRect;
		RECT rcTemp;
		::SetRect(&rcTemp,0,0,lpSrcInfo->GetSize().cx,lpSrcInfo->GetSize().cy);
		if ( ::IntersectRect(&rcSrcRect, pInfo->pSrcRect, &rcTemp)==0 ) {	//	���N��optimize����
			return 1;	//	�]����RECT���Ƃ�ł��Ȃ����ł���
		}
		//-------------------------------
	}
	nSsizeX = rcSrcRect.right - rcSrcRect.left;
	nSsizeY = rcSrcRect.bottom - rcSrcRect.top;

	int x,y;
	if (pInfo->pDstPoint==NULL){
		x = y = 0;
	} else {
		x = pInfo->pDstPoint->x;
		y = pInfo->pDstPoint->y;
	}
	if (pInfo->pDstSize==NULL) {
//		rcDstRect = *(lpDstInfo->GetRect());
		//	lpDstSize == NULL�̂Ƃ��́A
		//	Src����̓��{�̓]��
		::SetRect(&rcDstRect,x,y,x+nSsizeX,y+nSsizeY);
	} else {
		::SetRect(&rcDstRect,x,y,x+pInfo->pDstSize->cx,y+pInfo->pDstSize->cy);
	}
	nDsizeX = rcDstRect.right - rcDstRect.left;
	nDsizeY = rcDstRect.bottom - rcDstRect.top;

	int nBasePoint = pInfo->nBasePoint;
	switch (nBasePoint){
		case 0:                                   break;
		case 1: x -= nDsizeX>>1;                  break;
		case 2: x -= nDsizeX;                     break;
		case 3:                  y -= nDsizeY>>1; break;
		case 4: x -= nDsizeX>>1; y -= nDsizeY>>1; break;
		case 5: x -= nDsizeX;    y -= nDsizeY>>1; break;
		case 6:                  y -= nDsizeY;    break;
		case 7: x -= nDsizeX>>1; y -= nDsizeY;    break;
		case 8: x -= nDsizeX;    y -= nDsizeY;    break;
	}

	//	�N���b�v�̈�
	if (pInfo->pDstClip == NULL){
		::SetRect(&rcClipRect,0,0,lpDstInfo->GetSize().cx,lpDstInfo->GetSize().cy);
	} else {
		rcClipRect = *pInfo->pDstClip;
	}
	LPRECT lpClip = &rcClipRect;

	//--- �ǉ� '02/03/04  by ENRA ---
	{	// �N���b�vRect�́A�]����Rect�ɓ����Ȃ��Ƃ����Ȃ�
		// ������ClipRect���I�͂���ȏ��ɂ������ꍇ�̏����͂ǂ����悤�c
		RECT rcDstRect2 = { 0,0,lpDstInfo->GetSize().cx,lpDstInfo->GetSize().cy };
		int t;
		//--- �C�� '02/04/08  by ENRA ---
		// ����ɂO�ɂ����炠���񂩂���^^;;
		t = lpClip->left  - rcDstRect2.left;
		if (t<0)	{ lpClip->left	= rcDstRect2.left; }
		t = lpClip->top - rcDstRect2.top;
		if (t<0)	{ lpClip->top = rcDstRect2.top; }
		//-------------------------------
		t = lpClip->right  - rcDstRect2.right;
		if (t>0)	{ lpClip->right	 = rcDstRect2.right; }
		t = lpClip->bottom - rcDstRect2.bottom;
		if (t>0)	{ lpClip->bottom = rcDstRect2.bottom; }
	}
	//-------------------------------

	if ( (nSsizeX == nDsizeX) && (nSsizeY == nDsizeY)){
		lpClipper->bActualSize = true;
		// �N���b�s���O����
		// this clipping algorithm is thought by yaneurao(M.Isozaki)
		int sx = rcSrcRect.right - rcSrcRect.left;
		int sy = rcSrcRect.bottom - rcSrcRect.top;
		::SetRect(&rcDstRect,x,y,x+sx,y+sy);

		int t;
		t = lpClip->left-x;
		if (t>0)	{ rcSrcRect.left += t;	rcDstRect.left	= lpClip->left; }
		t = lpClip->top -y;
		if (t>0)	{ rcSrcRect.top	 += t;	rcDstRect.top	= lpClip->top;	 }
		t = x+sx-lpClip->right;
		if (t>0)	{ rcSrcRect.right -= t;	rcDstRect.right	= lpClip->right; }
		t = y+sy-lpClip->bottom;
		if (t>0)	{ rcSrcRect.bottom -= t; rcDstRect.bottom = lpClip->bottom; }

		//	invalid rect ?
		if (rcSrcRect.left >= rcSrcRect.right ||
			rcSrcRect.top  >= rcSrcRect.bottom) return 1;

	}else{ 
		//	�񓙔{�̓]���H
		lpClipper->bActualSize = false;
		int dx = rcDstRect.right - rcDstRect.left;
		int dy = rcDstRect.bottom - rcDstRect.top;
		::SetRect(&rcDstRect,x,y,x+dx,y+dy);

		//	�u���[���n���̏����l���v�Z����

		//	Initial(x,y) = -Dst size(x,y)
		nInitialX = rcDstRect.left - rcDstRect.right;
		nInitialY = rcDstRect.top  - rcDstRect.bottom;

		nStepX	= (rcSrcRect.right	- rcSrcRect.left) ;
		nStepY	= (rcSrcRect.bottom - rcSrcRect.top ) ;

		nCmpX = - (nInitialX);
		nCmpY = - (nInitialY);

		// invalid rect
		if (nCmpX == 0 || nCmpY == 0) return 1;

		// �N���b�s���O����
		// this clipping algorithm is thought by yaneurao(M.Isozaki)

		//	�~���[���Ɣ�~���[���Ńu���[���n���̏����l���Ⴄ
		int t;
		t = lpClip->left-rcDstRect.left;
		if (t>0)	{
			nInitialX+=t*nStepX;	//	���ۂɃu���[���n�����Ă݂�
			if (nInitialX > 0){
				int s = nInitialX / nCmpX + 1;
				rcSrcRect.left += s;	//	not mirror!
				nInitialX		-= s*nCmpX;
			}
			rcDstRect.left	 = lpClip->left;
		}
		t = lpClip->top -rcDstRect.top;
		if (t>0)	{
			nInitialY+=t*nStepY;	//	���ۂɃu���[���n�����Ă݂�
			if (nInitialY > 0){
				int s = nInitialY / nCmpY +1;
				rcSrcRect.top += s;
				nInitialY		-= s*nCmpY;
			}
			rcDstRect.top = lpClip->top;
		}
		t = rcDstRect.right-lpClip->right;
		if (t>0)	{
			rcDstRect.right	 = lpClip->right;
		}
		//	srcRect�̎Z�o�Bdst�̂͂ݏo���������T����̂����At*m_nStepX/m_nCmp��roundup����K�v����B

		t = rcDstRect.bottom-lpClip->bottom;
		if (t>0)	{ /*rcSrcRect.bottom -= t;*/
			rcDstRect.bottom = lpClip->bottom;}

		//	invalid rect ?
		if (rcSrcRect.left >= rcSrcRect.right ||
			rcSrcRect.top  >= rcSrcRect.bottom ||
			rcDstRect.left >= rcDstRect.right ||
			rcDstRect.top  >= rcDstRect.bottom) return 1;

		//	m_nStepX < m_nCmpX��ۏ؂���B
		nStepsX = nStepX/nCmpX;
		nStepX -= nCmpX*nStepsX;

		nStepsY = nStepY/nCmpY;
		nStepY -= nCmpY*nStepsY;

		// �ǂ��łP�������̂�������Ȃ��̂ŁA�����ň����̂��� By Tia
		nInitialX--;
		nInitialY--;

		lpClipper->nCmpX = nCmpX;
		lpClipper->nCmpY = nCmpY;
		lpClipper->nInitialX = nInitialX;
		lpClipper->nInitialY = nInitialY;
		lpClipper->nStepsX = nStepsX;
		lpClipper->nStepsY = nStepsY;
		lpClipper->nStepX = nStepX;
		lpClipper->nStepY = nStepY;
	}
	lpClipper->rcDstRect = rcDstRect;
	lpClipper->rcSrcRect = rcSrcRect;
	lpClipper->rcClipRect = rcClipRect;

	return 0;
}

#ifndef yaneSDK_CantUseInlineAssembler
///	XRGB8888->XRGB8888 ���l���w�肵�ē]��
LRESULT CFastPlaneEffect::Blt(CFastPlaneXRGB8888, const CSurfaceInfo* lpSrcInfo,
							  CFastPlaneXRGB8888, CSurfaceInfo* lpDstInfo,
							  CFastPlaneCopyMulAlpha f, const CSurfaceInfo::CBltInfo* pInfo,
							  CFastPlaneEffect::BurstTags::MMXBurst)
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
	LONG	nAddSrcWidth = lpSrcInfo->GetPitch() - (nSrcWidth * sizeof(DWORD));	// �N���b�s���O���Ĕ�΂����̎Z�o
	LONG	nAddDstWidth = lpDstInfo->GetPitch() - (nDstWidth * sizeof(DWORD));	// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
	DWORD*	lpSrc = (DWORD*)((BYTE*)lpSrcInfo->GetPtr() +(rcSrcRect.left*sizeof(DWORD))+rcSrcRect.top*lPitchSrc );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)lpDstInfo->GetPtr() +(rcDstRect.left*sizeof(DWORD))+rcDstRect.top*lPitchDst );		// �w�肳�ꂽx,y�̈ʒu����

	BYTE	bAlpha = *(BYTE*)(&f);
	DWORD	dwSrcRGBRate	= bAlpha*0x10000+bAlpha*0x100+bAlpha;
	DWORD	dwDstRGBRate	= (0xff-bAlpha)*0x10000+(0xff-bAlpha)*0x100+(0xff-bAlpha);

// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:��	 ���l:���f	  --------------
	if ( clip.bActualSize == true )
	{
		_asm
		{
			MOV			EDX, nDstHeight
			MOV			ECX, nDstWidth

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

			PXOR		MM0, MM0
			PXOR		MM1, MM1

			MOVD		MM2, dwSrcRGBRate
			PXOR		MM4, MM4

			MOVD		MM3, dwDstRGBRate
			PUNPCKLBW	MM2, MM4								// WORD�P�ʂŏ�Z����̂�

			PUNPCKLBW	MM3, MM4			// UnPair

		LoopY_11:
			SHR			ECX, 1
			JNB			LoopX_11								//	�������ۂ��̔���
			// Step1
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM1, [EDI]
			PUNPCKLBW	MM0, MM4								// WORD�P�ʂŏ�Z����̂�

			PUNPCKLBW	MM1, MM4
			PMULLW		MM0, MM2

			PMULLW		MM1, MM3
			NOP

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8				// UnPair

			PACKUSWB	MM0, MM4
			ADD			EDI, 4

			ADD			ESI, 4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_11

		LoopX_11: // 16�N���b�N��T�C�N��(��Z�� 3�N���b�N�� 24�N���b�N)
			// Step2
			MOVD		MM0, [ESI]			// 1// UnPair

			MOVD		MM5, [ESI+4]		// 2
			PUNPCKLBW	MM0, MM4			// 1				// WORD�P�ʂŏ�Z����̂�

			MOVD		MM1, [EDI]			// 1
			PMULLW		MM0, MM2			// 1

			MOVD		MM6, [EDI+4]		// 2
			PUNPCKLBW	MM1, MM4			// 1

			PMULLW		MM1, MM3			// 1// UnPair
			PUNPCKLBW	MM5, MM4			// 2				// WORD�P�ʂŏ�Z����̂�

			PMULLW		MM5, MM2			// 2
			PUNPCKLBW	MM6, MM4			// 2

			PMULLW		MM6, MM3			// 2
			NOP			// PMUL�X�g�[�����

			PADDUSB		MM0, MM1			// 1// UnPair

			PSRLW		MM0, 8				// 1// UnPair

			PACKUSWB	MM0, MM4			// 1
			PADDUSB		MM5, MM6			// 2

			PSRLW		MM5, 8				// 2// UnPair

			PACKUSWB	MM5, MM4			// 2
			ADD			ESI, 8

			MOVD		[EDI], MM0			// 1

			MOVD		[EDI+4], MM5		// 2// UnPair

			ADD			EDI, 8
			DEC			ECX

			JNZ			LoopX_11			// UnPair

		EndLoop_11:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nDstWidth							//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_11			// UnPair

			EMMS
		}
	}
// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:�L	 ���l:���f	  --------------
	else if ( 1 )
	{
		// �񓙔{
		int		nInitialX = clip.nInitialX ;	//	-DX�@ :�@�Â̏����l = -DX
		int		nStepX = clip.nStepX;		//	 2*SX :�@��+=2*SX
		int		nCmpX = clip.nCmpX;		//	 2*DX :�@��>0�Ȃ�΃�-=2*DX���Ă�
		int		nStepsX = clip.nStepsX;	//	 SrcX�̈��̉��Z��(������)
		int		nInitialY = clip.nInitialY;
		int		nStepY = clip.nStepY;
		int		nCmpY = clip.nCmpY;
		int		nStepsY = clip.nStepsY;

		LONG	lPitchSrc = lpSrcInfo->GetPitch();
		DWORD	AddSrcPixel = sizeof(DWORD) * nStepsX;
		DWORD	AddWidthSrc = lPitchSrc * nStepsY;
		int		EIX= nInitialX;
		int		EIY= nInitialY;
		int		EX = nStepX;
		int		EY = nStepY;
		int		EX2= nCmpX;
		int		EY2= nCmpY;
		DWORD*	lpSrcBack;
		DWORD	Mask7f = 0x007f7f7f;

		_asm
		{
			MOVD		MM6, Mask7f
			MOV			EDX, nDstHeight

			MOVQ		MM1, MM6
			MOV			ECX, nDstWidth

			PUNPCKLDQ	MM6, MM1								// 0x007f7f7f
			MOV			EDI, lpDst

			MOV			ESI, lpSrc
			PXOR		MM0, MM0

			PXOR		MM1, MM1
			PXOR		MM4, MM4

			MOVD		MM2, dwSrcRGBRate
			MOV			EBX, EIY								// nEyCnt = EIY;

			MOVD		MM3, dwDstRGBRate
			PUNPCKLBW	MM2, MM4								// WORD�P�ʂŏ�Z����̂�

			PUNPCKLBW	MM3, MM4			// UnPair

		LoopY_12:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_12: // 14�N���b�N��T�C�N��(��Z�� 3�N���b�N�� 18�N���b�N)
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM1, [EDI]
			PUNPCKLBW	MM0, MM4								// WORD�P�ʂŏ�Z����̂�

			PUNPCKLBW	MM1, MM4
			PMULLW		MM0, MM2

			PMULLW		MM1, MM3
			NOP

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PACKUSWB	MM0, MM4
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_12								// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_12:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_12

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nDstWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_12								// if ( EY >= 0 )

			ADD			ESI, lPitchSrc						// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l

		SkipY_12:
			DEC			EDX
			JNZ			LoopY_12

			EMMS
		}
	} // if

	return 0;
}

#endif // #ifndef yaneSDK_CantUseInlineAssembler

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
