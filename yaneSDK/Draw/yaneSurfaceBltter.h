//
//	yaneSurfaceBltter.h
//		�T�[�t�F�[�X�̓]���⏕
//

#ifndef __yaneSurfaceBltter_h__
#define __yaneSurfaceBltter_h__


#include "yaneSurface.h"
#include "yaneGTL.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CSurfaceBltter {
/**
	CSurfaceInfo��GeneralBlt�̓]���̎����̂��߂̃w���p�֐�

	Blt1 : �قڂ��ׂẴT�[�t�F�[�X�Ԃł̓]�����T�|�[�g����
			WORD�^�̃T�[�t�F�[�X(RGB565��)�ɑ΂��Ă�DWORD�������āA
			CFastPlaneEffect::BltDouble���Ăяo���B
			���@CFastPlaneCopySrc�Ŏg�p�B

	Blt2 : Blt1�̏k�ޔ�
			WORD�^�T�[�t�F�[�X�ɑ΂������ȓ]���͍s�Ȃ�Ȃ�
			�T�|�[�g�F
			�����T�[�t�F�[�X��
			�񃿃T�[�t�F�[�X�˃��T�[�t�F�[�X
			���T�[�t�F�[�X�˔񃿃T�[�t�F�[�X

	Blt3 : Blt2�Ɠ��������AWORD�^�T�[�t�F�[�X�ɑ΂��āA
				RGB555��RGB555555
				RGB565��RGB565565
			�ƁA����ɑΉ�����DWORD�N���X��p���ē]������B

	Blt4 : Blt3�Ɠ��������A�قȂ�T�[�t�F�[�X�Ԃ̓]���̓T�|�[�g���Ȃ�
			(������)

	Blt5 : Blt2�̏k�ޔ�
			�قȂ�T�[�t�F�[�X�Ԃ̓]���̓T�|�[�g���Ȃ�
			(������)

	Blt6 :
			���˔񃿁C���˃��@�ւ̓]��
			�� BltAlphaFast

	Blt7 :
			�ʏ�]���C���˔񃿂ւ̓]��
			���@BlendBltFast

	Effect1 : �T�[�t�F�[�X�ɑ΂��āA�G�t�F�N�g��������
			WORD�^�̃T�[�t�F�[�X(RGB565��)�ɑ΂��Ă�
			CFastPlaneEffect::BltDouble���Ăяo���B

		��FFn��CFastPlaneCopySrcColorKeyDouble�Ȃ��
		Fn �� CFastPlaneCopySrcColorKeyDouble<CFastPlaneRGB565565> rgb(dword);
		�ƃo�C���h���āABlt2���Ăяo���ȂǁB

	Effect2 : �T�[�t�F�[�X�ɑ΂��āA�G�t�F�N�g��������
			WORD�^�̃T�[�t�F�[�X(RGB565��)�ɑ΂���
			CFastPlaneEffect::BltDouble���g�Ăяo���Ȃ��h

	Morph2	:	Blt2��Morph��
		��	MorphBltFast�Ŏg�p

	Mosaic2	:	Effect2��Mosaic��

	Blt1Line : 1���C������Blt����B
			�s�N�Z���t�H�[�}�b�g�̃R���o�[�W�����Ɏg��

	���}�N����
		���@Fn�́AISurfaceRGB dw�ŗ^����ꂽ�����F��񓙂���������
	CSurfaceBltter_BltHelper1 : �������i�ʓ|�Ȃ̂Łj
	CSurfaceBltter_BltHelper2 : Blt2�R���p�`
	CSurfaceBltter_BltHelper2_2 : Blt2�R���p�`
			ISurfaceRGB dw�ƁADWORD�p�����[�^�ЂƂƂ�
			��	CFastPlaneBlendMulAlphaSrcColorKey(BlendBlt)���Ŏg�p�B
	CSurfaceBltter_BltHelper2_3 : Blt2�R���p�`
			ISurfaceRGB dw�ƁADWORD�p�����[�^�ӂ��Ƃ�
			��	CFastPlaneBlendMulAlphaSrcColorKeyAB(BlendConstBlt)���Ŏg�p�B
	CSurfaceBltter_MorphHelper2 : Blt2�R���p�`
			��	MorphBlt���Ŏg�p�B
	CSurfaceBltter_BltHelper4 : Blt4�R���p�`
			��	CFastPlaneCopyAddColor���Ŏg�p�B
	CSurfaceBltter_BltHelper5_2 : Blt5�R���p�`
			ISurfaceRGB dw�ƁADWORD�p�����[�^�ЂƂƂ�
			��	CFastPlaneCopyAddMulConstSrcColorKey���Ŏg�p�B

	CSurfaceBltter_EffectHelper1 : Blt4�ƃR���p�`
		������́AISurfaceRGB(ARGB8888)���p�����[�^�Ƃ��ĂƂ�
			��	CFastPlaneFillColor�Ŏg�p�B

		C++�̃e���v���[�g�@�\�ł́A
			F<T> �Ƃ����悤�Ȋ֐���F<T>�ł͂Ȃ�F�����������Ƃ��ēn�����Ƃ��o���Ȃ��B
			�����ŁA�}�N�����K�v�ɂȂ�B

		BltHelper�n�́A�}�N���Ƃ��Ď�������Ă���̂ŌĂяo���ӏ��ɂ�
		�C�����C���W�J�����̂ŁA���̓_�ɒ��ӂ��邱�ƁB

*/
public:
	template <class Fn>
	static LRESULT Blt1(const CSurfaceInfo* pSrc,CSurfaceInfo* pDst,const CSurfaceInfo::CBltInfo *pInfo,Fn fn){
	//	���̂悤�ɃR�[�f�B���O���Ă����΁A���̊֐��𔲂���Ƃ��Ɏ����I�Ƀf�X�g���N�^���Ăяo����A
	//	�����Unlock�����F
	if (pDst->Lock()!=0) return 1;
	CSurfaceLockerGuard g1(pDst); 
	if (pSrc->Lock()!=0) return 1;
	CSurfaceLockerGuard g2(pSrc); 

	int nS1 = pDst->GetSurfaceType();
	int nS2 = pSrc->GetSurfaceType();

	//	BltFast�����́AAutoRestore�Ŏg�p���邽�߁A
	//	�قڂ��ׂĂ̕ϊ����T�|�[�g�I

	switch (nS2){
	case 3:
		switch (nS1){
		//	RGB565 ->565
		case 3: {
			CFastPlaneEffect::BltDouble(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneRGB565(),pDst,
				CFastPlaneXRGB8888(),	//	DWORD��������
				CFastPlaneXRGB8888(),	//	DWORD��������
				fn,
				fn,
				pInfo);
			break; }
		//	RGB565->RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB565->RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB565->BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB565->XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB565->XRGB8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB565 -> ARGB4565�̓T�|�[�g
		case 10: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneARGB4565(),pDst,
				fn,
				pInfo);
			break; }
		default:
		//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
			return 2;
		} break;
	case 4:
		switch (nS1){
		//	RGB555 -> 8bpp �̓T�|�[�g
		case 2: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneBytePal(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB555 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		case 4: {
			CFastPlaneEffect::BltDouble(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneRGB555(),pDst,
				CFastPlaneXRGB8888(),	//	DWORD��������
				CFastPlaneXRGB8888(),	//	DWORD��������
				fn,
				fn,
				pInfo);
			break; }
		//	RGB555->RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB555->BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break;}
		//	RGB555->XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB555->XRGB8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		case 11: {
		//	RGB555 -> ARGB4555�̓T�|�[�g
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneARGB4555(),pDst,
				fn,
				pInfo);
			break; }
		default:
			//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
			return 2;
		} break;
	case 5:
		switch (nS1){
		//	RGB888 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB888 -> RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB888->RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB888->BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break;}
		//	RGB888->XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB888->XRGB8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ARGB8888�̓T�|�[�g
		case 12: {
			CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,
				pInfo);
			break;}
		default:
			//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
			return 2;
		} break;
	case 6:
		switch (nS1){
		//	BGR888 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		//	BGR888 -> RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	BGR888->RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	BGR888->BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break;}
		//	BGR888->XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	BGR888->XRGB8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ARGB8888�̓T�|�[�g
		case 12: {
			CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,
				pInfo);
			break;}
		//	BGR -> ABGR�T�|�[�g
		case 13: {
			CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,
				pInfo);
			break;}
		default:
			//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
			return 2;
		} break;
	case 7:
		switch (nS1){
		//	XRGB8888 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		//	XRGB8888 -> RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	XRGB8888->RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	XRGB8888->BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break;}
		//	XRGB8888->XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	XRGB8888->XRGB8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ARGB8888�̓T�|�[�g
		case 12: {
			CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,
				pInfo);
			break;}
		default:
			//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
			return 2;
		} break;
	case 8:
		switch (nS1){
		//	XBGR8888 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		//	XBGR8888 -> RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	XBGR8888->RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	XBGR8888->BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break;}
		//	XBGR8888->XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	XBGR8888->XBGR8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ARGB8888�̓T�|�[�g
		case 12: {
			CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,
				pInfo);
			break;}
		//	BGR->ABGR�T�|�[�g
		case 13: {
			CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,
				pInfo);
			break;}
		default:
			//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
			return 2;
		} break;

	//	���T�[�t�F�[�X�Ԃ̕ϊ�
	case 10:
		switch (nS1){
		//	ARGB4565 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> XBGR8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> ARGB4565
		case 10: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneARGB4565(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> ARGB4555
		case 11: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneARGB4555(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> ARGB8888
		case 12: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4565 -> ABGR8888
		case 13: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,
				pInfo);
			break; }
		default:
			//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
			return 2;
		} break;

	case 11:
		switch (nS1){
		//	ARGB4555 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> XBGR8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> ARGB4565
		case 10: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneARGB4565(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> ARGB4555
		case 11: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneARGB4555(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> ARGB8888
		case 12: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB4555 -> ABGR8888
		case 13: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,
				pInfo);
			break; }
		default:
			//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
			return 2;
		} break;

	case 12:
		switch (nS1){
		//	ARGB8888 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> XBGR8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> ARGB4565
		case 10: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneARGB4565(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> ARGB4555
		case 11: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneARGB4555(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> ARGB8888
		case 12: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ARGB8888 -> ABGR8888
		case 13: {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,
				pInfo);
			break; }
		default:
			//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
			return 2;
		} break;

	case 13:
		switch (nS1){
		//	ABGR8888 -> RGB565
		case 3: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> RGB555
		case 4: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> RGB888
		case 5: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> BGR888
		case 6: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> XRGB8888
		case 7: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> XBGR8888
		case 8: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> ARGB4565
		case 10: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneARGB4565(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> ARGB4555
		case 11: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneARGB4555(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> ARGB8888
		case 12: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,
				pInfo);
			break; }
		//	ABGR8888 -> ABGR8888
		case 13: {
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,
				pInfo);
			break; }
		default:
			//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
			return 2;
		} break;

	//	���̃\�[�X�T�[�t�F�[�X�^�C�v�͂Ȃ��˂�H
	default:
		//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
		;
	}

	return 0;
	}	//	the end of function Blt1

	/////////////////////////////////////////////////////////////////////////////

	template <class Fn>
	static LRESULT Blt2(const CSurfaceInfo* pSrc,CSurfaceInfo* pDst,const CSurfaceInfo::CBltInfo *pInfo,Fn fn){
	//	���̂悤�ɃR�[�f�B���O���Ă����΁A���̊֐��𔲂���Ƃ��Ɏ����I�Ƀf�X�g���N�^���Ăяo����A
	//	�����Unlock�����F
	if (pDst->Lock()!=0) return 1;
	CSurfaceLockerGuard g1(pDst); 
	if (pSrc->Lock()!=0) return 1;
	CSurfaceLockerGuard g2(pSrc); 

	int nS1 = pDst->GetSurfaceType();
	int nS2 = pSrc->GetSurfaceType();

	//	�قȂ�T�[�t�F�[�X�Ԃ̓]���͊�{�I�ɃT�|�[�g���Ȃ��I
	if (nS1!=nS2) {

		//	���ʃT�|�[�g�I�O�O�G
		//	���T�[�t�F�[�X����̓]��
		//	���v���[���Ƃ̐e�a��
		if (nS1==3 && nS2==10) {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,pInfo);
		}ef(nS1==4 && nS2==11) {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,pInfo);
		}ef(nS1==5 && nS2==12){
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,pInfo);
		}ef(nS1==6 && nS2==13){
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,pInfo);
		}ef(nS1==7 && nS2==12){
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,pInfo);
		}ef(nS1==8 && nS2==13){
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,pInfo);
		}
		
		//	���܂��ŃT�|�[�g�O�O�G
		//	���T�[�t�F�[�X�ւ̓]��

		//	RGB565 -> ARGB4565�̓T�|�[�g
		ef (nS2==3 && nS1==10){
			CFastPlaneEffect::Blt(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneARGB4565(),pDst,
				fn,pInfo);
		}ef(nS2==4 && nS1==11){
		//	RGB555 -> ARGB4555�̓T�|�[�g
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneARGB4555(),pDst,
				fn,pInfo);
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ARGB8888�̓T�|�[�g
		}ef(nS1==12){
			switch (nS2){
			case 5: {
				CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 6: {
				CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 7: {
				CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 8: {
				CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			default:
				//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
				return 2;
			}
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ABGR8888�̓T�|�[�g
		}ef(nS1==13){
			switch (nS2){
			case 5: {
				CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 6: {
				CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 7: {
				CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 8: {
				CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			default:
				//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
				return 2;
			}
		} else {
			//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
			return 2;
		}
		return 0;
	}

	switch( nS1 ) {
	case 3:	{
		CFastPlaneEffect::Blt(
			CFastPlaneRGB565(),pSrc,
			CFastPlaneRGB565(),pDst,
				fn,pInfo);
				break;
		}
	case 4: {
		CFastPlaneEffect::Blt(
			CFastPlaneRGB555(),pSrc,
			CFastPlaneRGB555(),pDst,
				fn,pInfo);
				break;
		}
	case 5:	{
		CFastPlaneEffect::Blt(
			CFastPlaneRGB888(),pSrc,
			CFastPlaneRGB888(),pDst,
				fn,pInfo);
				break;
		}
	case 6: {
		CFastPlaneEffect::Blt(
			CFastPlaneBGR888(),pSrc,
			CFastPlaneBGR888(),pDst,
				fn,pInfo);
				break;
		}
	case 7: {
		CFastPlaneEffect::Blt(
			CFastPlaneXRGB8888(),pSrc,
			CFastPlaneXRGB8888(),pDst,
				fn,pInfo);
				break;
		}
	case 8: {
		CFastPlaneEffect::Blt(
			CFastPlaneXBGR8888(),pSrc,
			CFastPlaneXBGR8888(),pDst,
				fn,pInfo);
				break;
		}
	case 10: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4565(),pSrc,
			CFastPlaneARGB4565(),pDst,
				fn,pInfo);
				break;
		}
	case 11: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4555(),pSrc,
			CFastPlaneARGB4555(),pDst,
				fn,pInfo);
				break;
		}
	case 12: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB8888(),pSrc,
			CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
		}
	case 13: {
		CFastPlaneEffect::Blt(
			CFastPlaneABGR8888(),pSrc,
			CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
		}
	}

	return 0;
	}	//	the end of function Blt2

// ���̊֐��̈�����Blt2�ƃR���p�`
//	�������Afn�́Adw��H�킹�Ă�����̉�
#define	CSurfaceBltter_BltHelper2(pSrc,pDst,pInfo,fn,dw){		\
	if (pDst->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g1(pDst);						\
	if (pSrc->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g2(pSrc);						\
	int nS1 = pDst->GetSurfaceType();					\
	int nS2 = pSrc->GetSurfaceType();					\
	BYTE a = (BYTE)(dw>>24) , r = (BYTE)((dw>>16)&0xff),	\
		g = (BYTE)((dw>>8)&0xff) , b = (BYTE)(dw & 0xff);	\
	if (nS1!=nS2) {										\
		if (nS1==3 && nS2==10) {						\
			CFastPlaneARGB4565 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB4565(),pSrc,				\
				CFastPlaneRGB565(),pDst,				\
				fn<CFastPlaneARGB4565>(rgb),pInfo);		\
		}ef(nS1==4 && nS2==11) {						\
			CFastPlaneARGB4555 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB4555(),pSrc,				\
				CFastPlaneRGB555(),pDst,				\
				fn<CFastPlaneARGB4555>(rgb),pInfo);		\
		}ef(nS1==5 && nS2==12){							\
			CFastPlaneARGB8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB8888(),pSrc,				\
				CFastPlaneRGB888(),pDst,				\
				fn<CFastPlaneARGB8888>(rgb),pInfo);		\
		}ef(nS1==6 && nS2==13){							\
			CFastPlaneABGR8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneABGR8888(),pSrc,				\
				CFastPlaneBGR888(),pDst,				\
				fn<CFastPlaneABGR8888>(rgb),pInfo);		\
		}ef(nS1==7 && nS2==12){							\
			CFastPlaneARGB8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB8888(),pSrc,				\
				CFastPlaneXRGB8888(),pDst,				\
				fn<CFastPlaneARGB8888>(rgb),pInfo);		\
		}ef(nS1==8 && nS2==13){							\
			CFastPlaneABGR8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneABGR8888(),pSrc,				\
				CFastPlaneXBGR8888(),pDst,				\
				fn<CFastPlaneABGR8888>(rgb),pInfo);		\
		}												\
														\
		ef (nS2==3 && nS1==10){							\
			CFastPlaneRGB565 rgb;						\
			rgb.SetRGB(r,g,b);							\
			CFastPlaneEffect::Blt(						\
				CFastPlaneRGB565(),pSrc,				\
				CFastPlaneARGB4565(),pDst,				\
				fn<CFastPlaneRGB565>(rgb),pInfo);		\
		}ef(nS2==4 && nS1==11){							\
			CFastPlaneRGB555 rgb;						\
			rgb.SetRGB(r,g,b);							\
			CFastPlaneEffect::Blt(						\
				CFastPlaneRGB555(),pSrc,				\
				CFastPlaneARGB4555(),pDst,				\
				fn<CFastPlaneRGB555>(rgb),pInfo);		\
		}ef(nS1==12){									\
			switch (nS2){								\
			case 5: {									\
				CFastPlaneRGB888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneRGB888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneRGB888>(rgb),pInfo);		\
				break;									\
					}									\
			case 6: {									\
				CFastPlaneBGR888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneBGR888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneBGR888>(rgb),pInfo);		\
				break;									\
					}									\
			case 7: {									\
				CFastPlaneXRGB8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXRGB8888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneXRGB8888>(rgb),pInfo);		\
				break;									\
					}									\
			case 8: {									\
				CFastPlaneXBGR8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXBGR8888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneXBGR8888>(rgb),pInfo);		\
				break;									\
					}									\
			default:									\
				return 2;								\
			}											\
		}ef(nS1==13){									\
			switch (nS2){								\
			case 5: {									\
				CFastPlaneRGB888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneRGB888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneRGB888>(rgb),pInfo);		\
				break;									\
					}									\
			case 6: {									\
				CFastPlaneBGR888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneBGR888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneBGR888>(rgb),pInfo);		\
				break;									\
					}									\
			case 7: {									\
				CFastPlaneXRGB8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXRGB8888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneXRGB8888>(rgb),pInfo);		\
				break;									\
					}									\
			case 8: {									\
				CFastPlaneXBGR8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXBGR8888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneXBGR8888>(rgb),pInfo);		\
				break;									\
					}									\
			default:									\
				return 2;								\
			}											\
		} else {										\
			return 2;									\
		}												\
		return 0;										\
	}													\
														\
	switch( nS1 ) {										\
	case 3:	{											\
		CFastPlaneRGB565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB565(),pSrc,					\
			CFastPlaneRGB565(),pDst,					\
			fn<CFastPlaneRGB565>(rgb),pInfo);			\
				break;									\
		}												\
	case 4: {											\
		CFastPlaneRGB555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB555(),pSrc,					\
			CFastPlaneRGB555(),pDst,					\
			fn<CFastPlaneRGB555>(rgb),pInfo);			\
				break;									\
		}												\
	case 5:	{											\
		CFastPlaneRGB888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB888(),pSrc,					\
			CFastPlaneRGB888(),pDst,					\
			fn<CFastPlaneRGB888>(rgb),pInfo);			\
				break;									\
		}												\
	case 6: {											\
		CFastPlaneBGR888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneBGR888(),pSrc,					\
			CFastPlaneBGR888(),pDst,					\
			fn<CFastPlaneBGR888>(rgb),pInfo);			\
				break;									\
		}												\
	case 7: {											\
		CFastPlaneXRGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXRGB8888(),pSrc,					\
			CFastPlaneXRGB8888(),pDst,					\
			fn<CFastPlaneXRGB8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 8: {											\
		CFastPlaneXBGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXBGR8888(),pSrc,					\
			CFastPlaneXBGR8888(),pDst,					\
			fn<CFastPlaneXBGR8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 10: {											\
		CFastPlaneARGB4565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4565(),pSrc,					\
			CFastPlaneARGB4565(),pDst,					\
			fn<CFastPlaneARGB4565>(rgb),pInfo);			\
				break;									\
		}												\
	case 11: {											\
		CFastPlaneARGB4555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4555(),pSrc,					\
			CFastPlaneARGB4555(),pDst,					\
			fn<CFastPlaneARGB4555>(rgb),pInfo);			\
				break;									\
		}												\
	case 12: {											\
		CFastPlaneARGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB8888(),pSrc,					\
			CFastPlaneARGB8888(),pDst,					\
			fn<CFastPlaneARGB8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 13: {											\
		CFastPlaneABGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneABGR8888(),pSrc,					\
			CFastPlaneABGR8888(),pDst,					\
			fn<CFastPlaneABGR8888>(rgb),pInfo);			\
				break;									\
		}												\
	}													\
														\
	return 0;											\
														\
	}	//	the end of function BltHelper2

// ���̊֐��̈�����Blt2�ƃR���p�`
//	�������Afn�́Adw��H�킹�Ă�����̉�
#define	CSurfaceBltter_BltHelper2_2(pSrc,pDst,pInfo,fn,dw,p1){		\
	if (pDst->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g1(pDst);						\
	if (pSrc->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g2(pSrc);						\
	int nS1 = pDst->GetSurfaceType();					\
	int nS2 = pSrc->GetSurfaceType();					\
	BYTE a = (BYTE)(dw>>24) , r = (BYTE)((dw>>16)&0xff),	\
		g = (BYTE)((dw>>8)&0xff) , b = (BYTE)(dw & 0xff);	\
	if (nS1!=nS2) {										\
		if (nS1==3 && nS2==10) {						\
			CFastPlaneARGB4565 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB4565(),pSrc,				\
				CFastPlaneRGB565(),pDst,				\
				fn<CFastPlaneARGB4565>(rgb,p1),pInfo);	\
		}ef(nS1==4 && nS2==11) {						\
			CFastPlaneARGB4555 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB4555(),pSrc,				\
				CFastPlaneRGB555(),pDst,				\
				fn<CFastPlaneARGB4555>(rgb,p1),pInfo);	\
		}ef(nS1==5 && nS2==12){							\
			CFastPlaneARGB8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB8888(),pSrc,				\
				CFastPlaneRGB888(),pDst,				\
				fn<CFastPlaneARGB8888>(rgb,p1),pInfo);	\
		}ef(nS1==6 && nS2==13){							\
			CFastPlaneABGR8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneABGR8888(),pSrc,				\
				CFastPlaneBGR888(),pDst,				\
				fn<CFastPlaneABGR8888>(rgb,p1),pInfo);	\
		}ef(nS1==7 && nS2==12){							\
			CFastPlaneARGB8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB8888(),pSrc,				\
				CFastPlaneXRGB8888(),pDst,				\
				fn<CFastPlaneARGB8888>(rgb,p1),pInfo);	\
		}ef(nS1==8 && nS2==13){							\
			CFastPlaneABGR8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneABGR8888(),pSrc,				\
				CFastPlaneXBGR8888(),pDst,				\
				fn<CFastPlaneABGR8888>(rgb,p1),pInfo);	\
		}												\
														\
		ef (nS2==3 && nS1==10){							\
			CFastPlaneRGB565 rgb;						\
			rgb.SetRGB(r,g,b);							\
			CFastPlaneEffect::Blt(						\
				CFastPlaneRGB565(),pSrc,				\
				CFastPlaneARGB4565(),pDst,				\
				fn<CFastPlaneRGB565>(rgb,p1),pInfo);	\
		}ef(nS2==4 && nS1==11){							\
			CFastPlaneRGB555 rgb;						\
			rgb.SetRGB(r,g,b);							\
			CFastPlaneEffect::Blt(						\
				CFastPlaneRGB555(),pSrc,				\
				CFastPlaneARGB4555(),pDst,				\
				fn<CFastPlaneRGB555>(rgb,p1),pInfo);	\
		}ef(nS1==12){									\
			switch (nS2){								\
			case 5: {									\
				CFastPlaneRGB888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneRGB888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneRGB888>(rgb,p1),pInfo);	\
				break;									\
					}									\
			case 6: {									\
				CFastPlaneBGR888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneBGR888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneBGR888>(rgb,p1),pInfo);	\
				break;									\
					}									\
			case 7: {									\
				CFastPlaneXRGB8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXRGB8888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneXRGB8888>(rgb,p1),pInfo);	\
				break;									\
					}									\
			case 8: {									\
				CFastPlaneXBGR8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXBGR8888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneXBGR8888>(rgb,p1),pInfo);	\
				break;									\
					}									\
			default:									\
				return 2;								\
			}											\
		}ef(nS1==13){									\
			switch (nS2){								\
			case 5: {									\
				CFastPlaneRGB888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneRGB888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneRGB888>(rgb,p1),pInfo);	\
				break;									\
					}									\
			case 6: {									\
				CFastPlaneBGR888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneBGR888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneBGR888>(rgb,p1),pInfo);	\
				break;									\
					}									\
			case 7: {									\
				CFastPlaneXRGB8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXRGB8888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneXRGB8888>(rgb,p1),pInfo);	\
				break;									\
					}									\
			case 8: {									\
				CFastPlaneXBGR8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXBGR8888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneXBGR8888>(rgb,p1),pInfo);	\
				break;									\
					}									\
			default:									\
				return 2;								\
			}											\
		} else {										\
			return 2;									\
		}												\
		return 0;										\
	}													\
														\
	switch( nS1 ) {										\
	case 3:	{											\
		CFastPlaneRGB565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB565(),pSrc,					\
			CFastPlaneRGB565(),pDst,					\
			fn<CFastPlaneRGB565>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 4: {											\
		CFastPlaneRGB555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB555(),pSrc,					\
			CFastPlaneRGB555(),pDst,					\
			fn<CFastPlaneRGB555>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 5:	{											\
		CFastPlaneRGB888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB888(),pSrc,					\
			CFastPlaneRGB888(),pDst,					\
			fn<CFastPlaneRGB888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 6: {											\
		CFastPlaneBGR888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneBGR888(),pSrc,					\
			CFastPlaneBGR888(),pDst,					\
			fn<CFastPlaneBGR888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 7: {											\
		CFastPlaneXRGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXRGB8888(),pSrc,					\
			CFastPlaneXRGB8888(),pDst,					\
			fn<CFastPlaneXRGB8888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 8: {											\
		CFastPlaneXBGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXBGR8888(),pSrc,					\
			CFastPlaneXBGR8888(),pDst,					\
			fn<CFastPlaneXBGR8888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 10: {											\
		CFastPlaneARGB4565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4565(),pSrc,					\
			CFastPlaneARGB4565(),pDst,					\
			fn<CFastPlaneARGB4565>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 11: {											\
		CFastPlaneARGB4555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4555(),pSrc,					\
			CFastPlaneARGB4555(),pDst,					\
			fn<CFastPlaneARGB4555>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 12: {											\
		CFastPlaneARGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB8888(),pSrc,					\
			CFastPlaneARGB8888(),pDst,					\
			fn<CFastPlaneARGB8888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 13: {											\
		CFastPlaneABGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneABGR8888(),pSrc,					\
			CFastPlaneABGR8888(),pDst,					\
			fn<CFastPlaneABGR8888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	}													\
														\
	return 0;											\
														\
	}	//	the end of function BltHelper2_2

// ���̊֐��̈�����Blt2�ƃR���p�`
//	�������Afn�́Adw��H�킹�Ă�����̉�
#define	CSurfaceBltter_BltHelper2_3(pSrc,pDst,pInfo,fn,dw,p1,p2){		\
	if (pDst->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g1(pDst);						\
	if (pSrc->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g2(pSrc);						\
	int nS1 = pDst->GetSurfaceType();					\
	int nS2 = pSrc->GetSurfaceType();					\
	BYTE a = (BYTE)(dw>>24) , r = (BYTE)((dw>>16)&0xff),	\
		g = (BYTE)((dw>>8)&0xff) , b = (BYTE)(dw & 0xff);	\
	if (nS1!=nS2) {										\
		if (nS1==3 && nS2==10) {						\
			CFastPlaneARGB4565 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB4565(),pSrc,				\
				CFastPlaneRGB565(),pDst,				\
				fn<CFastPlaneARGB4565>(rgb,p1,p2),pInfo);	\
		}ef(nS1==4 && nS2==11) {						\
			CFastPlaneARGB4555 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB4555(),pSrc,				\
				CFastPlaneRGB555(),pDst,				\
				fn<CFastPlaneARGB4555>(rgb,p1,p2),pInfo);	\
		}ef(nS1==5 && nS2==12){							\
			CFastPlaneARGB8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB8888(),pSrc,				\
				CFastPlaneRGB888(),pDst,				\
				fn<CFastPlaneARGB8888>(rgb,p1,p2),pInfo);	\
		}ef(nS1==6 && nS2==13){							\
			CFastPlaneABGR8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneABGR8888(),pSrc,				\
				CFastPlaneBGR888(),pDst,				\
				fn<CFastPlaneABGR8888>(rgb,p1,p2),pInfo);	\
		}ef(nS1==7 && nS2==12){							\
			CFastPlaneARGB8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneARGB8888(),pSrc,				\
				CFastPlaneXRGB8888(),pDst,				\
				fn<CFastPlaneARGB8888>(rgb,p1,p2),pInfo);	\
		}ef(nS1==8 && nS2==13){							\
			CFastPlaneABGR8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Blt(						\
				CFastPlaneABGR8888(),pSrc,				\
				CFastPlaneXBGR8888(),pDst,				\
				fn<CFastPlaneABGR8888>(rgb,p1,p2),pInfo);	\
		}												\
														\
		ef (nS2==3 && nS1==10){							\
			CFastPlaneRGB565 rgb;						\
			rgb.SetRGB(r,g,b);							\
			CFastPlaneEffect::Blt(						\
				CFastPlaneRGB565(),pSrc,				\
				CFastPlaneARGB4565(),pDst,				\
				fn<CFastPlaneRGB565>(rgb,p1,p2),pInfo);	\
		}ef(nS2==4 && nS1==11){							\
			CFastPlaneRGB555 rgb;						\
			rgb.SetRGB(r,g,b);							\
			CFastPlaneEffect::Blt(						\
				CFastPlaneRGB555(),pSrc,				\
				CFastPlaneARGB4555(),pDst,				\
				fn<CFastPlaneRGB555>(rgb,p1,p2),pInfo);	\
		}ef(nS1==12){									\
			switch (nS2){								\
			case 5: {									\
				CFastPlaneRGB888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneRGB888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneRGB888>(rgb,p1,p2),pInfo);	\
				break;									\
					}									\
			case 6: {									\
				CFastPlaneBGR888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneBGR888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneBGR888>(rgb,p1,p2),pInfo);	\
				break;									\
					}									\
			case 7: {									\
				CFastPlaneXRGB8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXRGB8888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneXRGB8888>(rgb,p1,p2),pInfo);	\
				break;									\
					}									\
			case 8: {									\
				CFastPlaneXBGR8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXBGR8888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneXBGR8888>(rgb,p1,p2),pInfo);	\
				break;									\
					}									\
			default:									\
				return 2;								\
			}											\
		}ef(nS1==13){									\
			switch (nS2){								\
			case 5: {									\
				CFastPlaneRGB888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneRGB888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneRGB888>(rgb,p1,p2),pInfo);	\
				break;									\
					}									\
			case 6: {									\
				CFastPlaneBGR888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneBGR888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneBGR888>(rgb,p1,p2),pInfo);	\
				break;									\
					}									\
			case 7: {									\
				CFastPlaneXRGB8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXRGB8888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneXRGB8888>(rgb,p1,p2),pInfo);	\
				break;									\
					}									\
			case 8: {									\
				CFastPlaneXBGR8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Blt(					\
				CFastPlaneXBGR8888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneXBGR8888>(rgb,p1,p2),pInfo);	\
				break;									\
					}									\
			default:									\
				return 2;								\
			}											\
		} else {										\
			return 2;									\
		}												\
		return 0;										\
	}													\
														\
	switch( nS1 ) {										\
	case 3:	{											\
		CFastPlaneRGB565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB565(),pSrc,					\
			CFastPlaneRGB565(),pDst,					\
			fn<CFastPlaneRGB565>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 4: {											\
		CFastPlaneRGB555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB555(),pSrc,					\
			CFastPlaneRGB555(),pDst,					\
			fn<CFastPlaneRGB555>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 5:	{											\
		CFastPlaneRGB888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB888(),pSrc,					\
			CFastPlaneRGB888(),pDst,					\
			fn<CFastPlaneRGB888>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 6: {											\
		CFastPlaneBGR888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneBGR888(),pSrc,					\
			CFastPlaneBGR888(),pDst,					\
			fn<CFastPlaneBGR888>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 7: {											\
		CFastPlaneXRGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXRGB8888(),pSrc,					\
			CFastPlaneXRGB8888(),pDst,					\
			fn<CFastPlaneXRGB8888>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 8: {											\
		CFastPlaneXBGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXBGR8888(),pSrc,					\
			CFastPlaneXBGR8888(),pDst,					\
			fn<CFastPlaneXBGR8888>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 10: {											\
		CFastPlaneARGB4565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4565(),pSrc,					\
			CFastPlaneARGB4565(),pDst,					\
			fn<CFastPlaneARGB4565>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 11: {											\
		CFastPlaneARGB4555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4555(),pSrc,					\
			CFastPlaneARGB4555(),pDst,					\
			fn<CFastPlaneARGB4555>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 12: {											\
		CFastPlaneARGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB8888(),pSrc,					\
			CFastPlaneARGB8888(),pDst,					\
			fn<CFastPlaneARGB8888>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	case 13: {											\
		CFastPlaneABGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneABGR8888(),pSrc,					\
			CFastPlaneABGR8888(),pDst,					\
			fn<CFastPlaneABGR8888>(rgb,p1,p2),pInfo);		\
				break;									\
		}												\
	}													\
														\
	return 0;											\
														\
	}	//	the end of function BltHelper2_3

/** ���̊֐��̈�����Blt2�ƃR���p�`
	�������Afn�́Adw��H�킹�Ă�����̉�

		CSurfaceInfo* pDst	:	�G�t�F�N�g���{���Ώ�
		LPCRECT		lpRect	:	�G�t�F�N�g���{���͈�
		template<FN>	Fn	:	�G�t�F�N�^�[
		ISurfaceRGB		dw	:	�G�t�F�N�^�[�̂��߂̃p�����[�^
		(ARGB8888)
*/
#define CSurfaceBltter_EffectHelper1(pDst,lpRect,fn,dw){			\
	if (pDst->Lock()!=0) return 1;						\
	CSurfaceLockerGuard guard(pDst);					\
	int nS1 = pDst->GetSurfaceType();					\
	BYTE a = (BYTE)(dw>>24) , r = (BYTE)((dw>>16)&0xff),	\
		g = (BYTE)((dw>>8)&0xff) , b = (BYTE)(dw & 0xff);	\
	switch( nS1 ) {										\
	case 3: {											\
		CFastPlaneRGB565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneRGB565565 rgb2;						\
		rgb2.SetRGBWORD(rgb.GetRGB());					\
		CFastPlaneEffect::EffectDouble(					\
			CFastPlaneRGB565(),pDst,					\
			CFastPlaneRGB565565(),						\
			fn<CFastPlaneRGB565>(rgb),					\
			fn<CFastPlaneRGB565565>(rgb2),				\
			lpRect); } break;							\
	case 4: {											\
		CFastPlaneRGB555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneRGB555555 rgb2;						\
		rgb2.SetRGBWORD(rgb.GetRGB());					\
		CFastPlaneEffect::EffectDouble(					\
			CFastPlaneRGB555(),pDst,					\
			CFastPlaneRGB555555(),						\
			fn<CFastPlaneRGB555>(rgb),					\
			fn<CFastPlaneRGB555555>(rgb2),				\
			lpRect); } break;							\
	case 5: {											\
		CFastPlaneRGB888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Effect(						\
			CFastPlaneRGB888(),pDst,					\
			fn<CFastPlaneRGB888>(rgb),					\
			lpRect); } break;							\
	case 6: {											\
		CFastPlaneBGR888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Effect(						\
			CFastPlaneBGR888(),pDst,					\
			fn<CFastPlaneBGR888>(rgb),					\
			lpRect); } break;							\
	case 7: {											\
		CFastPlaneXRGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Effect(						\
			CFastPlaneXRGB8888(),pDst,					\
			fn<CFastPlaneXRGB8888>(rgb),				\
			lpRect); } break;							\
	case 8: {											\
		CFastPlaneXBGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Effect(						\
			CFastPlaneXBGR8888(),pDst,					\
			fn<CFastPlaneXBGR8888>(rgb),				\
			lpRect); } break;							\
	case 10: {											\
		CFastPlaneARGB4565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Effect(						\
			CFastPlaneARGB4565(),pDst,					\
			fn<CFastPlaneARGB4565>(rgb),				\
			lpRect); } break;							\
	case 11: {											\
		CFastPlaneARGB4555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Effect(						\
			CFastPlaneARGB4555(),pDst,					\
			fn<CFastPlaneARGB4555>(rgb),				\
			lpRect); } break;							\
	case 12: {											\
		CFastPlaneARGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Effect(						\
			CFastPlaneARGB8888(),pDst,					\
			fn<CFastPlaneARGB8888>(rgb),				\
			lpRect); } break;							\
	case 13: {											\
		CFastPlaneABGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Effect(						\
			CFastPlaneABGR8888(),pDst,					\
			fn<CFastPlaneABGR8888>(rgb),				\
			lpRect); } break;							\
	}													\
	return 0;											\
	}	//	end of CSurfaceBltter_EffectHelper1

#define CSurfaceBltter_EffectHelper1b(pDst,lpRect,fn,dw){			\
	if (pDst->Lock()!=0) return 1;						\
	CSurfaceLockerGuard guard(pDst);					\
	int nS1 = pDst->GetSurfaceType();					\
	BYTE a = (BYTE)(dw>>24) , r = (BYTE)((dw>>16)&0xff),	\
		g = (BYTE)((dw>>8)&0xff) , b = (BYTE)(dw & 0xff);	\
	switch( nS1 ) {										\
	case 3: {											\
		CFastPlaneRGB565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneXRGB8888 rgb2;						\
		rgb2.SetRGB(rgb.GetRGB_DWORD());				\
		CFastPlaneEffect::EffectDouble(					\
			CFastPlaneRGB565(),pDst,					\
			CFastPlaneXRGB8888(),						\
			fn<CFastPlaneRGB565>(rgb),					\
			fn<CFastPlaneXRGB8888>(rgb2),				\
			lpRect); } break;							\
	case 4: {											\
		CFastPlaneRGB555 rgb;							\
		CFastPlaneXRGB8888 rgb2;						\
		rgb.SetRGB(r,g,b);								\
		rgb2.SetRGB(rgb.GetRGB_DWORD());				\
		CFastPlaneEffect::EffectDouble(					\
			CFastPlaneRGB555(),pDst,					\
			CFastPlaneXRGB8888(),						\
			fn<CFastPlaneRGB555>(rgb),					\
			fn<CFastPlaneXRGB8888>(rgb2),				\
			lpRect); } break;							\
	case 5: {											\
		CFastPlaneRGB888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Effect(						\
			CFastPlaneRGB888(),pDst,					\
			fn<CFastPlaneRGB888>(rgb),					\
			lpRect); } break;							\
	case 6: {											\
		CFastPlaneBGR888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Effect(						\
			CFastPlaneBGR888(),pDst,					\
			fn<CFastPlaneBGR888>(rgb),					\
			lpRect); } break;							\
	case 7: {											\
		CFastPlaneXRGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Effect(						\
			CFastPlaneXRGB8888(),pDst,					\
			fn<CFastPlaneXRGB8888>(rgb),				\
			lpRect); } break;							\
	case 8: {											\
		CFastPlaneXBGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Effect(						\
			CFastPlaneXBGR8888(),pDst,					\
			fn<CFastPlaneXBGR8888>(rgb),				\
			lpRect); } break;							\
	case 10: {											\
		CFastPlaneARGB4565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Effect(						\
			CFastPlaneARGB4565(),pDst,					\
			fn<CFastPlaneARGB4565>(rgb),				\
			lpRect); } break;							\
	case 11: {											\
		CFastPlaneARGB4555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Effect(						\
			CFastPlaneARGB4555(),pDst,					\
			fn<CFastPlaneARGB4555>(rgb),				\
			lpRect); } break;							\
	case 12: {											\
		CFastPlaneARGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Effect(						\
			CFastPlaneARGB8888(),pDst,					\
			fn<CFastPlaneARGB8888>(rgb),				\
			lpRect); } break;							\
	case 13: {											\
		CFastPlaneABGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Effect(						\
			CFastPlaneABGR8888(),pDst,					\
			fn<CFastPlaneABGR8888>(rgb),				\
			lpRect); } break;							\
	}													\
	return 0;											\
	}	//	end of CSurfaceBltter_EffectHelper1b

	template <class Fn>
	static LRESULT Blt3(const CSurfaceInfo* pSrc,CSurfaceInfo* pDst,
		const CSurfaceInfo::CBltInfo *pInfo,Fn fn){
	//	���̂悤�ɃR�[�f�B���O���Ă����΁A���̊֐��𔲂���Ƃ��Ɏ����I�Ƀf�X�g���N�^���Ăяo����A
	//	�����Unlock�����F
	if (pDst->Lock()!=0) return 1;
	CSurfaceLockerGuard g1(pDst); 
	if (pSrc->Lock()!=0) return 1;
	CSurfaceLockerGuard g2(pSrc); 

	int nS1 = pDst->GetSurfaceType();
	int nS2 = pSrc->GetSurfaceType();

	//	�قȂ�T�[�t�F�[�X�Ԃ̓]���͊�{�I�ɃT�|�[�g���Ȃ��I
	if (nS1!=nS2) {

		//	���ʃT�|�[�g�I�O�O�G
		//	���T�[�t�F�[�X����̓]��
		//	���v���[���Ƃ̐e�a��
		if (nS1==3 && nS2==10) {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,pInfo);
		}ef(nS1==4 && nS2==11) {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,pInfo);
		}ef(nS1==5 && nS2==12){
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,pInfo);
		}ef(nS1==6 && nS2==13){
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,pInfo);
		}ef(nS1==7 && nS2==12){
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,pInfo);
		}ef(nS1==8 && nS2==13){
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,pInfo);
		}
		
		//	���܂��ŃT�|�[�g�O�O�G
		//	���T�[�t�F�[�X�ւ̓]��

		//	RGB565 -> ARGB4565�̓T�|�[�g
		ef (nS2==3 && nS1==10){
			CFastPlaneEffect::Blt(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneARGB4565(),pDst,
				fn,pInfo);
		}ef(nS2==4 && nS1==11){
		//	RGB555 -> ARGB4555�̓T�|�[�g
			CFastPlaneEffect::Blt(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneARGB4555(),pDst,
				fn,pInfo);
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ARGB8888�̓T�|�[�g
		}ef(nS1==12){
			switch (nS2){
			case 5: {
				CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 6: {
				CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 7: {
				CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 8: {
				CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			default:
				//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
				return 2;
			}
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ABGR8888�̓T�|�[�g
		}ef(nS1==13){
			switch (nS2){
			case 5: {
				CFastPlaneEffect::Blt(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 6: {
				CFastPlaneEffect::Blt(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 7: {
				CFastPlaneEffect::Blt(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 8: {
				CFastPlaneEffect::Blt(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			default:
				//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
				return 2;
			}
		} else {
			//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
			return 2;
		}
		return 0;
	}

	switch( nS1 ) {
		///////////////////////////////////
		//	�����@����������Blt2�ƈႤ�@����
	case 3:	{
		CFastPlaneEffect::BltDouble(
			CFastPlaneRGB565(),pSrc,
			CFastPlaneRGB565(),pDst,
			CFastPlaneRGB565565(),
			CFastPlaneRGB565565(),
				fn,fn,pInfo);
				break;
		}
	case 4: {
		CFastPlaneEffect::BltDouble(
			CFastPlaneRGB555(),pSrc,
			CFastPlaneRGB555(),pDst,
			CFastPlaneRGB555555(),
			CFastPlaneRGB555555(),
				fn,fn,pInfo);
				break;
		}
		//	�����@����������Blt2�ƈႤ�@����
		///////////////////////////////////
	case 5:	{
		CFastPlaneEffect::Blt(
			CFastPlaneRGB888(),pSrc,
			CFastPlaneRGB888(),pDst,
				fn,pInfo);
				break;
		}
	case 6: {
		CFastPlaneEffect::Blt(
			CFastPlaneBGR888(),pSrc,
			CFastPlaneBGR888(),pDst,
				fn,pInfo);
				break;
		}
	case 7: {
		CFastPlaneEffect::Blt(
			CFastPlaneXRGB8888(),pSrc,
			CFastPlaneXRGB8888(),pDst,
				fn,pInfo);
				break;
		}
	case 8: {
		CFastPlaneEffect::Blt(
			CFastPlaneXBGR8888(),pSrc,
			CFastPlaneXBGR8888(),pDst,
				fn,pInfo);
				break;
		}
	case 10: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4565(),pSrc,
			CFastPlaneARGB4565(),pDst,
				fn,pInfo);
				break;
		}
	case 11: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4555(),pSrc,
			CFastPlaneARGB4555(),pDst,
				fn,pInfo);
				break;
		}
	case 12: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB8888(),pSrc,
			CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
		}
	case 13: {
		CFastPlaneEffect::Blt(
			CFastPlaneABGR8888(),pSrc,
			CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
		}
	}

	return 0;
	}	//	the end of function Blt3

// ���̊֐��̈�����Blt4�ƃR���p�`
//	�������Afn�́Adw��H�킹�Ă�����̉�
#define	CSurfaceBltter_BltHelper4(pSrc,pDst,pInfo,fn,dw){		\
	if (pDst->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g1(pDst);						\
	if (pSrc->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g2(pSrc);						\
	int nS1 = pDst->GetSurfaceType();					\
	int nS2 = pSrc->GetSurfaceType();					\
	BYTE a = (BYTE)(dw>>24) , r = (BYTE)((dw>>16)&0xff),	\
		g = (BYTE)((dw>>8)&0xff) , b = (BYTE)(dw & 0xff);	\
	if (nS1!=nS2) {										\
		return 2;										\
	}													\
	switch( nS1 ) {										\
	case 3: {											\
		CFastPlaneRGB565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneRGB565565 rgb2;						\
		rgb2.SetRGBWORD(rgb.GetRGB());					\
		CFastPlaneEffect::BltDouble(					\
			CFastPlaneRGB565(),pSrc,					\
			CFastPlaneRGB565(),pDst,					\
			CFastPlaneRGB565565(),						\
			CFastPlaneRGB565565(),						\
			fn<CFastPlaneRGB565>(rgb),					\
			fn<CFastPlaneRGB565565>(rgb2),				\
				pInfo);									\
					} break;							\
	case 4: {											\
		CFastPlaneRGB555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneRGB555555 rgb2;						\
		rgb2.SetRGBWORD(rgb.GetRGB());					\
		CFastPlaneEffect::BltDouble(					\
			CFastPlaneRGB555(),pSrc,					\
			CFastPlaneRGB555(),pDst,					\
			CFastPlaneRGB555555(),						\
			CFastPlaneRGB555555(),						\
			fn<CFastPlaneRGB555>(rgb),					\
			fn<CFastPlaneRGB555555>(rgb2),				\
				pInfo);									\
					} break;							\
	case 5:	{											\
		CFastPlaneRGB888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB888(),pSrc,					\
			CFastPlaneRGB888(),pDst,					\
			fn<CFastPlaneRGB888>(rgb),pInfo);			\
				break;									\
		}												\
	case 6: {											\
		CFastPlaneBGR888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneBGR888(),pSrc,					\
			CFastPlaneBGR888(),pDst,					\
			fn<CFastPlaneBGR888>(rgb),pInfo);			\
				break;									\
		}												\
	case 7: {											\
		CFastPlaneXRGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXRGB8888(),pSrc,					\
			CFastPlaneXRGB8888(),pDst,					\
			fn<CFastPlaneXRGB8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 8: {											\
		CFastPlaneXBGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXBGR8888(),pSrc,					\
			CFastPlaneXBGR8888(),pDst,					\
			fn<CFastPlaneXBGR8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 10: {											\
		CFastPlaneARGB4565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4565(),pSrc,					\
			CFastPlaneARGB4565(),pDst,					\
			fn<CFastPlaneARGB4565>(rgb),pInfo);			\
				break;									\
		}												\
	case 11: {											\
		CFastPlaneARGB4555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4555(),pSrc,					\
			CFastPlaneARGB4555(),pDst,					\
			fn<CFastPlaneARGB4555>(rgb),pInfo);			\
				break;									\
		}												\
	case 12: {											\
		CFastPlaneARGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB8888(),pSrc,					\
			CFastPlaneARGB8888(),pDst,					\
			fn<CFastPlaneARGB8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 13: {											\
		CFastPlaneABGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneABGR8888(),pSrc,					\
			CFastPlaneABGR8888(),pDst,					\
			fn<CFastPlaneABGR8888>(rgb),pInfo);			\
				break;									\
		}												\
	}													\
														\
	return 0;											\
														\
	}	//	the end of function BltHelper4

// ���̊֐��̈�����Blt5�ƃR���p�`
//	�������Afn�́Adw(ISurfaceRGB),p1��H�킹�Ă�����̉�
#define	CSurfaceBltter_BltHelper5_2(pSrc,pDst,pInfo,fn,dw,p1){	\
	if (pDst->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g1(pDst);						\
	if (pSrc->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g2(pSrc);						\
	int nS1 = pDst->GetSurfaceType();					\
	int nS2 = pSrc->GetSurfaceType();					\
	BYTE a = (BYTE)(dw>>24) , r = (BYTE)((dw>>16)&0xff),	\
		g = (BYTE)((dw>>8)&0xff) , b = (BYTE)(dw & 0xff);	\
	if (nS1!=nS2) {										\
		return 2;										\
	}													\
	switch( nS1 ) {										\
	case 3:	{											\
		CFastPlaneRGB565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB565(),pSrc,					\
			CFastPlaneRGB565(),pDst,					\
			fn<CFastPlaneRGB565>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 4:	{											\
		CFastPlaneRGB555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneRGB555(),pSrc,					\
			CFastPlaneRGB555(),pDst,					\
			fn<CFastPlaneRGB555>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 6: {											\
		CFastPlaneBGR888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneBGR888(),pSrc,					\
			CFastPlaneBGR888(),pDst,					\
			fn<CFastPlaneBGR888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 7: {											\
		CFastPlaneXRGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXRGB8888(),pSrc,					\
			CFastPlaneXRGB8888(),pDst,					\
			fn<CFastPlaneXRGB8888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 8: {											\
		CFastPlaneXBGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneXBGR8888(),pSrc,					\
			CFastPlaneXBGR8888(),pDst,					\
			fn<CFastPlaneXBGR8888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 10: {											\
		CFastPlaneARGB4565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4565(),pSrc,					\
			CFastPlaneARGB4565(),pDst,					\
			fn<CFastPlaneARGB4565>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 11: {											\
		CFastPlaneARGB4555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB4555(),pSrc,					\
			CFastPlaneARGB4555(),pDst,					\
			fn<CFastPlaneARGB4555>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 12: {											\
		CFastPlaneARGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneARGB8888(),pSrc,					\
			CFastPlaneARGB8888(),pDst,					\
			fn<CFastPlaneARGB8888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	case 13: {											\
		CFastPlaneABGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Blt(							\
			CFastPlaneABGR8888(),pSrc,					\
			CFastPlaneABGR8888(),pDst,					\
			fn<CFastPlaneABGR8888>(rgb,p1),pInfo);		\
				break;									\
		}												\
	}													\
														\
	return 0;											\
														\
	}	//	the end of function BltHelper5_2

/**
	fn , fn2(���T�[�t�F�[�X�ɑ΂���G�t�F�N�^)
*/
	template <class Fn,class Fn2>
	static LRESULT Blt6(const CSurfaceInfo* pSrc,CSurfaceInfo* pDst,
		const CSurfaceInfo::CBltInfo *pInfo,Fn fn,Fn2 fn2){
	//	���̂悤�ɃR�[�f�B���O���Ă����΁A���̊֐��𔲂���Ƃ��Ɏ����I�Ƀf�X�g���N�^���Ăяo����A
	//	�����Unlock�����F
	if (pDst->Lock()!=0) return 1;
	CSurfaceLockerGuard g1(pDst); 
	if (pSrc->Lock()!=0) return 1;
	CSurfaceLockerGuard g2(pSrc); 

	int nS1 = pDst->GetSurfaceType();
	int nS2 = pSrc->GetSurfaceType();

	//	���t���T�[�t�F�[�X����A�������T�[�t�F�[�X�ւ̓]�������T�|�[�g���Ȃ��I
	//	����������A���t���ւ́ABlt,BltFast�ł����ł���
	//	���t���T�[�t�F�[�X���烿�t���T�[�t�F�[�X�ւ̓]�����T�|�[�g���Ƃ������ȁD�D
	if (((nS1==3 || nS1==10) && nS2==10) ||
		((nS1==4 || nS1==11) && nS2==11) ||
		((nS1==5 || nS1==7 || nS1==12)&&(nS2==12))||
		((nS1==6 || nS1==8 || nS1==13)&&(nS2==13))
		) {
		//	ok , surface with alpha to normal surface
	} else {
		return 2;
	}

	switch( nS1 ) {
	case 3: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4565(),pSrc,
			CFastPlaneRGB565(),pDst,
			fn,pInfo); break;
		}
	case 4: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4555(),pSrc,
			CFastPlaneRGB555(),pDst,
			fn,pInfo); break;
		}
	case 5: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB8888(),pSrc,
			CFastPlaneRGB888(),pDst,
			fn,pInfo); break;
		}
	case 6: {
		CFastPlaneEffect::Blt(
			CFastPlaneABGR8888(),pSrc,
			CFastPlaneBGR888(),pDst,
			fn,pInfo); break;
		}
	case 7: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB8888(),pSrc,
			CFastPlaneXRGB8888(),pDst,
			fn,pInfo); break;
		}
	case 8: {
		CFastPlaneEffect::Blt(
			CFastPlaneABGR8888(),pSrc,
			CFastPlaneXBGR8888(),pDst,
			fn,pInfo); break;
		}

	case 10: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4565(),pSrc,
			CFastPlaneARGB4565(),pDst,
			fn2,pInfo); break;
		}
	case 11: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4555(),pSrc,
			CFastPlaneARGB4555(),pDst,
			fn2,pInfo); break;
		}
	case 12: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB8888(),pSrc,
			CFastPlaneARGB8888(),pDst,
			fn2,pInfo); break;
		}
	case 13: {
		CFastPlaneEffect::Blt(
			CFastPlaneABGR8888(),pSrc,
			CFastPlaneABGR8888(),pDst,
			fn2,pInfo); break;
		}
	}

	return 0;
	}	//	the end of function Blt6


	template <class Fn>
	static LRESULT Blt7(CSurfaceInfo* pSrc,CSurfaceInfo* pDst,CSurfaceInfo::CBltInfo *pInfo,Fn fn){
	//	���̂悤�ɃR�[�f�B���O���Ă����΁A���̊֐��𔲂���Ƃ��Ɏ����I�Ƀf�X�g���N�^���Ăяo����A
	//	�����Unlock�����F
	if (pDst->Lock()!=0) return 1;
	CSurfaceLockerGuard g1(pDst); 
	if (pSrc->Lock()!=0) return 1;
	CSurfaceLockerGuard g2(pSrc); 

	int nS1 = pDst->GetSurfaceType();
	int nS2 = pSrc->GetSurfaceType();

	//	�قȂ�T�[�t�F�[�X�Ԃ̓]���͊�{�I�ɃT�|�[�g���Ȃ��I
	if (nS1!=nS2) {

		//	���ʃT�|�[�g�I�O�O�G
		//	���T�[�t�F�[�X����̓]��
		//	���v���[���Ƃ̐e�a��
		if (nS1==3 && nS2==10) {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,pInfo);
		}ef(nS1==4 && nS2==11) {
			CFastPlaneEffect::Blt(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,pInfo);
		}ef(nS1==5 && nS2==12){
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,pInfo);
		}ef(nS1==6 && nS2==13){
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,pInfo);
		}ef(nS1==7 && nS2==12){
			CFastPlaneEffect::Blt(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,pInfo);
		}ef(nS1==8 && nS2==13){
			CFastPlaneEffect::Blt(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,pInfo);
		} else {
			//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
			return 2;
		}
		return 0;
	}

	switch( nS1 ) {
	case 3:	{
		CFastPlaneEffect::Blt(
			CFastPlaneRGB565(),pSrc,
			CFastPlaneRGB565(),pDst,
				fn,pInfo);
				break;
		}
	case 4: {
		CFastPlaneEffect::Blt(
			CFastPlaneRGB555(),pSrc,
			CFastPlaneRGB555(),pDst,
				fn,pInfo);
				break;
		}
	case 5:	{
		CFastPlaneEffect::Blt(
			CFastPlaneRGB888(),pSrc,
			CFastPlaneRGB888(),pDst,
				fn,pInfo);
				break;
		}
	case 6: {
		CFastPlaneEffect::Blt(
			CFastPlaneBGR888(),pSrc,
			CFastPlaneBGR888(),pDst,
				fn,pInfo);
				break;
		}
	case 7: {
		CFastPlaneEffect::Blt(
			CFastPlaneXRGB8888(),pSrc,
			CFastPlaneXRGB8888(),pDst,
				fn,pInfo);
				break;
		}
	case 8: {
		CFastPlaneEffect::Blt(
			CFastPlaneXBGR8888(),pSrc,
			CFastPlaneXBGR8888(),pDst,
				fn,pInfo);
				break;
		}
	case 10: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4565(),pSrc,
			CFastPlaneARGB4565(),pDst,
				fn,pInfo);
				break;
		}
	case 11: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB4555(),pSrc,
			CFastPlaneARGB4555(),pDst,
				fn,pInfo);
				break;
		}
	case 12: {
		CFastPlaneEffect::Blt(
			CFastPlaneARGB8888(),pSrc,
			CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
		}
	case 13: {
		CFastPlaneEffect::Blt(
			CFastPlaneABGR8888(),pSrc,
			CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
		}
	}

	return 0;
	}	//	the end of function Blt7


	template <class Fn>
	static LRESULT Effect2(CSurfaceInfo* pSrc,LPCRECT prc,Fn fn){
	//	���̂悤�ɃR�[�f�B���O���Ă����΁A���̊֐��𔲂���Ƃ��Ɏ����I�Ƀf�X�g���N�^���Ăяo����A
	//	�����Unlock�����F
	if (pSrc->Lock()!=0) return 1;
	CSurfaceLockerGuard g1(pSrc); 

	int nS1 = pSrc->GetSurfaceType();

	switch( nS1 ) {
		///////////////////////////////////
		//	�����@����������Effect1�ƈႤ�@����
	case 3:	{
		CFastPlaneEffect::Effect(
			CFastPlaneRGB565(),pSrc,
			fn,prc); } break;
	case 4: {
		CFastPlaneEffect::Effect(
			CFastPlaneRGB555(),pSrc,
			fn,prc); } break;
		//	�����@����������Blt2�ƈႤ�@����
		///////////////////////////////////
	case 5:	{
		CFastPlaneEffect::Effect(
			CFastPlaneRGB888(),pSrc,
			fn,prc); } break;
	case 6: {
		CFastPlaneEffect::Effect(
			CFastPlaneBGR888(),pSrc,
			fn,prc); } break;
	case 7: {
		CFastPlaneEffect::Effect(
			CFastPlaneXRGB8888(),pSrc,
			fn,prc); } break;
	case 8: {
		CFastPlaneEffect::Effect(
			CFastPlaneXBGR8888(),pSrc,
			fn,prc); } break;
	case 10: {
		CFastPlaneEffect::Effect(
			CFastPlaneARGB4565(),pSrc,
			fn,prc); } break;
	case 11: {
		CFastPlaneEffect::Effect(
			CFastPlaneARGB4555(),pSrc,
			fn,prc); } break;
	case 12: {
		CFastPlaneEffect::Effect(
			CFastPlaneARGB8888(),pSrc,
			fn,prc); } break;
	case 13: {
		CFastPlaneEffect::Effect(
			CFastPlaneABGR8888(),pSrc,
			fn,prc); } break;
	}

	return 0;
	}	//	the end of function Effect2

	template <class Fn>
	static LRESULT Mosaic2(CSurfaceInfo* pSrc,LPCRECT prc,Fn fn,int d){
	//	���̂悤�ɃR�[�f�B���O���Ă����΁A���̊֐��𔲂���Ƃ��Ɏ����I�Ƀf�X�g���N�^���Ăяo����A
	//	�����Unlock�����F
	if (pSrc->Lock()!=0) return 1;
	CSurfaceLockerGuard g1(pSrc); 

	int nS1 = pSrc->GetSurfaceType();

	switch( nS1 ) {
	case 3:	{
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneRGB565(),pSrc,
			fn,d,prc); } break;
	case 4: {
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneRGB555(),pSrc,
			fn,d,prc); } break;
	case 5:	{
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneRGB888(),pSrc,
			fn,d,prc); } break;
	case 6: {
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneBGR888(),pSrc,
			fn,d,prc); } break;
	case 7: {
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneXRGB8888(),pSrc,
			fn,d,prc); } break;
	case 8: {
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneXBGR8888(),pSrc,
			fn,d,prc); } break;
	case 10: {
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneARGB4565(),pSrc,
			fn,d,prc); } break;
	case 11: {
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneARGB4555(),pSrc,
			fn,d,prc); } break;
	case 12: {
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneARGB8888(),pSrc,
			fn,d,prc); } break;
	case 13: {
		CFastPlaneEffect::EffectMosaic(
			CFastPlaneABGR8888(),pSrc,
			fn,d,prc); } break;
	}

	return 0;
	}	//	the end of function Mosaic2

	//////////////////////////////////////////////////////////////////////
	///	Morph�n

	template <class Fn>
	static LRESULT Morph2(const CSurfaceInfo* pSrc,CSurfaceInfo* pDst,const CSurfaceInfo::CMorphInfo *pInfo,Fn fn){
	//	���̂悤�ɃR�[�f�B���O���Ă����΁A���̊֐��𔲂���Ƃ��Ɏ����I�Ƀf�X�g���N�^���Ăяo����A
	//	�����Unlock�����F
	if (pDst->Lock()!=0) return 1;
	CSurfaceLockerGuard g1(pDst); 
	if (pSrc->Lock()!=0) return 1;
	CSurfaceLockerGuard g2(pSrc); 

	int nS1 = pDst->GetSurfaceType();
	int nS2 = pSrc->GetSurfaceType();

	//	�قȂ�T�[�t�F�[�X�Ԃ̓]���͊�{�I�ɃT�|�[�g���Ȃ��I
	if (nS1!=nS2) {

		//	���ʃT�|�[�g�I�O�O�G
		//	���T�[�t�F�[�X����̓]��
		//	���v���[���Ƃ̐e�a��
		if (nS1==3 && nS2==10) {
			CFastPlaneEffect::Morph(
				CFastPlaneARGB4565(),pSrc,
				CFastPlaneRGB565(),pDst,
				fn,pInfo);
		}ef(nS1==4 && nS2==11) {
			CFastPlaneEffect::Morph(
				CFastPlaneARGB4555(),pSrc,
				CFastPlaneRGB555(),pDst,
				fn,pInfo);
		}ef(nS1==5 && nS2==12){
			CFastPlaneEffect::Morph(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneRGB888(),pDst,
				fn,pInfo);
		}ef(nS1==6 && nS2==13){
			CFastPlaneEffect::Morph(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneBGR888(),pDst,
				fn,pInfo);
		}ef(nS1==7 && nS2==12){
			CFastPlaneEffect::Morph(
				CFastPlaneARGB8888(),pSrc,
				CFastPlaneXRGB8888(),pDst,
				fn,pInfo);
		}ef(nS1==8 && nS2==13){
			CFastPlaneEffect::Morph(
				CFastPlaneABGR8888(),pSrc,
				CFastPlaneXBGR8888(),pDst,
				fn,pInfo);
		}
		
		//	���܂��ŃT�|�[�g�O�O�G
		//	���T�[�t�F�[�X�ւ̓]��

		//	RGB565 -> ARGB4565�̓T�|�[�g
		ef (nS2==3 && nS1==10){
			CFastPlaneEffect::Morph(
				CFastPlaneRGB565(),pSrc,
				CFastPlaneARGB4565(),pDst,
				fn,pInfo);
		}ef(nS2==4 && nS1==11){
		//	RGB555 -> ARGB4555�̓T�|�[�g
			CFastPlaneEffect::Morph(
				CFastPlaneRGB555(),pSrc,
				CFastPlaneARGB4555(),pDst,
				fn,pInfo);
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ARGB8888�̓T�|�[�g
		}ef(nS1==12){
			switch (nS2){
			case 5: {
				CFastPlaneEffect::Morph(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 6: {
				CFastPlaneEffect::Morph(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 7: {
				CFastPlaneEffect::Morph(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			case 8: {
				CFastPlaneEffect::Morph(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
					}
			default:
				//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
				return 2;
			}
		//	RGB888/BGR888/XRGB8888/ARGB8888 -> ABGR8888�̓T�|�[�g
		}ef(nS1==13){
			switch (nS2){
			case 5: {
				CFastPlaneEffect::Morph(
				CFastPlaneRGB888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 6: {
				CFastPlaneEffect::Morph(
				CFastPlaneBGR888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 7: {
				CFastPlaneEffect::Morph(
				CFastPlaneXRGB8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			case 8: {
				CFastPlaneEffect::Morph(
				CFastPlaneXBGR8888(),pSrc,
				CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
					}
			default:
				//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
				return 2;
			}
		} else {
			//	�c�O�Ȃ����T�|�[�g�ł����O�O�G
			return 2;
		}
		return 0;
	}

	switch( nS1 ) {
	case 3:	{
		CFastPlaneEffect::Morph(
			CFastPlaneRGB565(),pSrc,
			CFastPlaneRGB565(),pDst,
				fn,pInfo);
				break;
		}
	case 4: {
		CFastPlaneEffect::Morph(
			CFastPlaneRGB555(),pSrc,
			CFastPlaneRGB555(),pDst,
				fn,pInfo);
				break;
		}
	case 5:	{
		CFastPlaneEffect::Morph(
			CFastPlaneRGB888(),pSrc,
			CFastPlaneRGB888(),pDst,
				fn,pInfo);
				break;
		}
	case 6: {
		CFastPlaneEffect::Morph(
			CFastPlaneBGR888(),pSrc,
			CFastPlaneBGR888(),pDst,
				fn,pInfo);
				break;
		}
	case 7: {
		CFastPlaneEffect::Morph(
			CFastPlaneXRGB8888(),pSrc,
			CFastPlaneXRGB8888(),pDst,
				fn,pInfo);
				break;
		}
	case 8: {
		CFastPlaneEffect::Morph(
			CFastPlaneXBGR8888(),pSrc,
			CFastPlaneXBGR8888(),pDst,
				fn,pInfo);
				break;
		}
	case 10: {
		CFastPlaneEffect::Morph(
			CFastPlaneARGB4565(),pSrc,
			CFastPlaneARGB4565(),pDst,
				fn,pInfo);
				break;
		}
	case 11: {
		CFastPlaneEffect::Morph(
			CFastPlaneARGB4555(),pSrc,
			CFastPlaneARGB4555(),pDst,
				fn,pInfo);
				break;
		}
	case 12: {
		CFastPlaneEffect::Morph(
			CFastPlaneARGB8888(),pSrc,
			CFastPlaneARGB8888(),pDst,
				fn,pInfo);
				break;
		}
	case 13: {
		CFastPlaneEffect::Morph(
			CFastPlaneABGR8888(),pSrc,
			CFastPlaneABGR8888(),pDst,
				fn,pInfo);
				break;
		}
	}

	return 0;
	}	//	the end of function Morph2

// ���̊֐��̈�����Blt2�ƃR���p�`
//	�������Afn�́Adw��H�킹�Ă�����̉�
#define	CSurfaceBltter_MorphHelper2(pSrc,pDst,pInfo,fn,dw){		\
	if (pDst->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g1(pDst);						\
	if (pSrc->Lock()!=0) return 1;						\
	CSurfaceLockerGuard g2(pSrc);						\
	int nS1 = pDst->GetSurfaceType();					\
	int nS2 = pSrc->GetSurfaceType();					\
	BYTE a = (BYTE)(dw>>24) , r = (BYTE)((dw>>16)&0xff),	\
		g = (BYTE)((dw>>8)&0xff) , b = (BYTE)(dw & 0xff);	\
	if (nS1!=nS2) {										\
		if (nS1==3 && nS2==10) {						\
			CFastPlaneARGB4565 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Morph(					\
				CFastPlaneARGB4565(),pSrc,				\
				CFastPlaneRGB565(),pDst,				\
				fn<CFastPlaneARGB4565>(rgb),pInfo);		\
		}ef(nS1==4 && nS2==11) {						\
			CFastPlaneARGB4555 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Morph(					\
				CFastPlaneARGB4555(),pSrc,				\
				CFastPlaneRGB555(),pDst,				\
				fn<CFastPlaneARGB4555>(rgb),pInfo);		\
		}ef(nS1==5 && nS2==12){							\
			CFastPlaneARGB8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Morph(					\
				CFastPlaneARGB8888(),pSrc,				\
				CFastPlaneRGB888(),pDst,				\
				fn<CFastPlaneARGB8888>(rgb),pInfo);		\
		}ef(nS1==6 && nS2==13){							\
			CFastPlaneABGR8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Morph(					\
				CFastPlaneABGR8888(),pSrc,				\
				CFastPlaneBGR888(),pDst,				\
				fn<CFastPlaneABGR8888>(rgb),pInfo);		\
		}ef(nS1==7 && nS2==12){							\
			CFastPlaneARGB8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Morph(					\
				CFastPlaneARGB8888(),pSrc,				\
				CFastPlaneXRGB8888(),pDst,				\
				fn<CFastPlaneARGB8888>(rgb),pInfo);		\
		}ef(nS1==8 && nS2==13){							\
			CFastPlaneABGR8888 rgb;						\
			rgb.SetRGB(r,g,b);							\
			rgb.SetA(a);								\
			CFastPlaneEffect::Morph(					\
				CFastPlaneABGR8888(),pSrc,				\
				CFastPlaneXBGR8888(),pDst,				\
				fn<CFastPlaneABGR8888>(rgb),pInfo);		\
		}												\
														\
		ef (nS2==3 && nS1==10){							\
			CFastPlaneRGB565 rgb;						\
			rgb.SetRGB(r,g,b);							\
			CFastPlaneEffect::Morph(					\
				CFastPlaneRGB565(),pSrc,				\
				CFastPlaneARGB4565(),pDst,				\
				fn<CFastPlaneRGB565>(rgb),pInfo);		\
		}ef(nS2==4 && nS1==11){							\
			CFastPlaneRGB555 rgb;						\
			rgb.SetRGB(r,g,b);							\
			CFastPlaneEffect::Morph(					\
				CFastPlaneRGB555(),pSrc,				\
				CFastPlaneARGB4555(),pDst,				\
				fn<CFastPlaneRGB555>(rgb),pInfo);		\
		}ef(nS1==12){									\
			switch (nS2){								\
			case 5: {									\
				CFastPlaneRGB888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Morph(				\
				CFastPlaneRGB888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneRGB888>(rgb),pInfo);		\
				break;									\
					}									\
			case 6: {									\
				CFastPlaneBGR888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Morph(				\
				CFastPlaneBGR888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneBGR888>(rgb),pInfo);		\
				break;									\
					}									\
			case 7: {									\
				CFastPlaneXRGB8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Morph(				\
				CFastPlaneXRGB8888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneXRGB8888>(rgb),pInfo);		\
				break;									\
					}									\
			case 8: {									\
				CFastPlaneXBGR8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Morph(				\
				CFastPlaneXBGR8888(),pSrc,				\
				CFastPlaneARGB8888(),pDst,				\
				fn<CFastPlaneXBGR8888>(rgb),pInfo);		\
				break;									\
					}									\
			default:									\
				return 2;								\
			}											\
		}ef(nS1==13){									\
			switch (nS2){								\
			case 5: {									\
				CFastPlaneRGB888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Morph(				\
				CFastPlaneRGB888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneRGB888>(rgb),pInfo);		\
				break;									\
					}									\
			case 6: {									\
				CFastPlaneBGR888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Morph(				\
				CFastPlaneBGR888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneBGR888>(rgb),pInfo);		\
				break;									\
					}									\
			case 7: {									\
				CFastPlaneXRGB8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Morph(				\
				CFastPlaneXRGB8888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneXRGB8888>(rgb),pInfo);		\
				break;									\
					}									\
			case 8: {									\
				CFastPlaneXBGR8888 rgb;					\
				rgb.SetRGB(r,g,b);						\
				CFastPlaneEffect::Morph(				\
				CFastPlaneXBGR8888(),pSrc,				\
				CFastPlaneABGR8888(),pDst,				\
				fn<CFastPlaneXBGR8888>(rgb),pInfo);		\
				break;									\
					}									\
			default:									\
				return 2;								\
			}											\
		} else {										\
			return 2;									\
		}												\
		return 0;										\
	}													\
														\
	switch( nS1 ) {										\
	case 3:	{											\
		CFastPlaneRGB565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Morph(						\
			CFastPlaneRGB565(),pSrc,					\
			CFastPlaneRGB565(),pDst,					\
			fn<CFastPlaneRGB565>(rgb),pInfo);			\
				break;									\
		}												\
	case 4: {											\
		CFastPlaneRGB555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Morph(						\
			CFastPlaneRGB555(),pSrc,					\
			CFastPlaneRGB555(),pDst,					\
			fn<CFastPlaneRGB555>(rgb),pInfo);			\
				break;									\
		}												\
	case 5:	{											\
		CFastPlaneRGB888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Morph(						\
			CFastPlaneRGB888(),pSrc,					\
			CFastPlaneRGB888(),pDst,					\
			fn<CFastPlaneRGB888>(rgb),pInfo);			\
				break;									\
		}												\
	case 6: {											\
		CFastPlaneBGR888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Morph(						\
			CFastPlaneBGR888(),pSrc,					\
			CFastPlaneBGR888(),pDst,					\
			fn<CFastPlaneBGR888>(rgb),pInfo);			\
				break;									\
		}												\
	case 7: {											\
		CFastPlaneXRGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Morph(						\
			CFastPlaneXRGB8888(),pSrc,					\
			CFastPlaneXRGB8888(),pDst,					\
			fn<CFastPlaneXRGB8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 8: {											\
		CFastPlaneXBGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Morph(						\
			CFastPlaneXBGR8888(),pSrc,					\
			CFastPlaneXBGR8888(),pDst,					\
			fn<CFastPlaneXBGR8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 10: {											\
		CFastPlaneARGB4565 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Morph(						\
			CFastPlaneARGB4565(),pSrc,					\
			CFastPlaneARGB4565(),pDst,					\
			fn<CFastPlaneARGB4565>(rgb),pInfo);			\
				break;									\
		}												\
	case 11: {											\
		CFastPlaneARGB4555 rgb;							\
		rgb.SetRGB(r,g,b);								\
		rgb.SetA(a);									\
		CFastPlaneEffect::Morph(						\
			CFastPlaneARGB4555(),pSrc,					\
			CFastPlaneARGB4555(),pDst,					\
			fn<CFastPlaneARGB4555>(rgb),pInfo);			\
				break;									\
		}												\
	case 12: {											\
		CFastPlaneARGB8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Morph(						\
			CFastPlaneARGB8888(),pSrc,					\
			CFastPlaneARGB8888(),pDst,					\
			fn<CFastPlaneARGB8888>(rgb),pInfo);			\
				break;									\
		}												\
	case 13: {											\
		CFastPlaneABGR8888 rgb;							\
		rgb.SetRGB(r,g,b);								\
		CFastPlaneEffect::Morph(						\
			CFastPlaneABGR8888(),pSrc,					\
			CFastPlaneABGR8888(),pDst,					\
			fn<CFastPlaneABGR8888>(rgb),pInfo);			\
				break;									\
		}												\
	}													\
														\
	return 0;											\
														\
	}	//	the end of function MorphHelper2

	//////////////////////////////////////////////////////////////////////
	template <class Src,class Dst,class Fn>
	static void Blt1Line(const Src* pSrc,Dst *pDst,int nLength,Fn fn){
	/**
		Src ���� Dst�ɂP���C����(nLength)���A�]������
		Surface��lock�͂��Ȃ�
	*/
		for(int i=0;i<nLength;i++){
			fn(pDst[i],pSrc[i]);
		}
	}

	static LRESULT Blt1Line(const void* pSrc,int nSrcSurfaceType,void *pDst,int nDstSurfaceType,int nLength);
	/**
		Src ���� Dst�ɂP���C����(nLength)���A�]������
		Surface��lock�͂��Ȃ��B�T�[�t�F�[�X�̃^�C�v�Ƃ́A
		CSurfaceInfo�̃T�[�t�F�[�X�^�C�v�Ɠ����Ӗ��B

		�������A�\�[�X�T�[�t�F�[�X�͂��ׂẴp�^�[�����T�|�[�g���Ă���
		�킯�ł͂Ȃ��B

		�T�|�[�g���Ă���g�ݍ��킹
		�P�D
			nSrcSurfaceType : 4(RGB555),5(RGB888),12(ARGB8888)
			nDstSurfaceType : 0,1,2,9�̓G���[���^�[���B2�`13�B
			�]������RGB555,RGB888�́Abmp�摜�ł��肤�邩��B
			�]������ARGB8888�́Ayga�摜�̓ǂݍ��ݗp�B
		�Q�D
			nSrcSurfaceType : 0,1,2,9�̓G���[���^�[���B2�`13�B
			nDstSurfaceType : 5(RGB888),12(ARGB8888)
			bmp��yga�̕ۑ��p
	*/

};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
