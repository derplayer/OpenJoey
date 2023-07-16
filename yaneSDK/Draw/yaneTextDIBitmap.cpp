#include "stdafx.h"
#include "yaneSurface.h"
#include "yaneTextDIBitmap.h"
/**
	���̃N���X�́AyaneSDK2nd�̂��̂�ˊэH���ŃR�s�y��������
*/

namespace yaneuraoGameSDK3rd {
namespace Draw {

CTextDIBitmap::CTextDIBitmap(){
	m_nTextX = 0;
	m_nTextY = 0;
	m_bVertical = false;
}

void CTextDIBitmap::SetVertical(bool bVertical){
	m_bVertical = bVertical;
}

LRESULT	CTextDIBitmap::UpdateText(){
	LRESULT lr;
	if (!IsVertical()) {	// �������̏ꍇ
		lr = UpdateTextHorizontal();
	} else {				// �c�����̏ꍇ
		//	font��90�K�|��
		GetFont()->SetVerticalFont(!GetFont()->IsVerticalFont());
		lr = UpdateTextVertical();
		GetFont()->SetVerticalFont(!GetFont()->IsVerticalFont());
	}
	return lr;
}

LRESULT	CTextDIBitmap::UpdateTextA(){	//	�A���`�F���t���`��
	LRESULT lr;
	if (!IsVertical()) {	// �������̏ꍇ
		lr = UpdateTextHorizontalA();
	} else {				// �c�����̏ꍇ
		//	font��90�K�|��
		GetFont()->SetVerticalFont(!GetFont()->IsVerticalFont());
		lr = UpdateTextVerticalA();
		GetFont()->SetVerticalFont(!GetFont()->IsVerticalFont());
	}
	return lr;
}

//	����̃R�s�y�����O�O�G
LRESULT	CTextDIBitmap::UpdateTextAA(){	//	�A���`�F���t���`��
	LRESULT lr;
	if (!IsVertical()) {	// �������̏ꍇ
		lr = UpdateTextHorizontalAA();
	} else {				// �c�����̏ꍇ
		//	font��90�K�|��
		GetFont()->SetVerticalFont(!GetFont()->IsVerticalFont());
		lr = UpdateTextVerticalAA();
		GetFont()->SetVerticalFont(!GetFont()->IsVerticalFont());
	}
	return lr;
}

//////////////////////////////////////////////////////////
/*

	::GetGlyphOutline�́A
		Win9x�n�ł́Abold�w�肷��Ɖ�����
		(������ōs�����́A���̂����ňꕔ������������)
		�܂��AServicePack�𓖂ĂĂ��Ȃ�NT4.0���ƁA
			��]�������Ƃ��ɁAOS��������
		���ƁA�ꕶ���������ϊ��ł��Ȃ��̂ŁA
		�r���ɃC�^���b�N�������܂����Ă���Ƃ��̒�����
		���O�ł��Ȃ��Ƃ����Ȃ��B

	�ȏ�𗝉����Ďg���̂Ȃ�΁A�����͂Ȃ��̂����ǁD�D
*/
//////////////////////////////////////////////////////////
// ������

inline LRESULT CTextDIBitmap::UpdateTextHorizontal(){
	int sx,sy;
	m_Font.GetSize(sx,sy);
	// m_Font�̃e�L�X�g����Ȃ�v���[���͍��Ȃ�
	//	(�ǂ���Bltter���`�F�b�N���ĕs���ȃv���[���Ȃ�Γ]�����Ȃ��̂�)
	if (CreateSurface(sx,sy,32)) return 1;
	// �w�i�F�œh��Ԃ�
	Clear();

	m_Font.OnDraw(GetDC(),m_nTextX,m_nTextY);
	return 0;
}



inline LRESULT CTextDIBitmap::UpdateTextHorizontalA(){	//	�A���`�F���t���`��
	int sx,sy;
	m_Font.GetSize(sx,sy);
	if (CreateSurface(sx,sy,32,true)) return 1;
	//�@�w�i�F�œh��Ԃ�
	//	�������Ȃ��Ək�������T�[�t�F�X���R�s�[�������ɃS�~�����̂�
	Clear();

	COLORREF rgb = m_Font.GetColor();
	COLORREF bk	 = m_Font.GetBackColor();

	m_Font.SetBackColor(CLR_INVALID);
	m_Font.SetColor(RGB(255,255,255));	//	���ɂ���
	int	nQuality = m_Font.GetQuality();
	m_Font.SetQuality(4);	//	�A���`�F�������ŏ���
	int nFontSize = m_Font.GetSize();
	int nFontHeight = m_Font.GetHeight();
	m_Font.SetSize(nFontSize<<1);	//	2�{�T�C�Y�ŕ`��
	m_Font.SetHeight(nFontHeight<<1); //�@�s�Ԃ�2�{�T�C�Y�ŕ`��
	//	SetSize�́A�s�Ԃ��ύX���Ă��܂��̂ŁA
	//	SetSize����O�ɁA�s�Ԃ��擾���Ă����A������Q�{���Ȃ���΂Ȃ�Ȃ�

	CDIBitmap d_dib;
	int sx2,sy2;
	m_Font.GetSize(sx2,sy2);
	// m_Font�̃e�L�X�g����ł��v���[�������D
	if ((sx2 == 0) || (sy2 == 0)) {
		sx2 = sy2 = 1;
	}
	if (d_dib.CreateSurface(sx2,sy2,32,true)) return 2;
	// �w�i�F�œh��Ԃ�
	d_dib.GetSurfaceInfo()->SetFillColor(GetSurfaceInfo()->GetFillColor());
	d_dib.Clear();
	//	d_dib.GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	m_Font.OnDraw(d_dib.GetDC(),m_nTextX<<1,m_nTextY<<1);	//	2�{�̃t�H���g�T�C�Y�ŕ`��

	//	�Q�~�Q�����{�ɏk��
	{
	  sx2>>=1; sy2>>=1;
	  if (sx2<sx) { sx = sx2; }	//�@�Ȃ��H�������ꂽ�v���[���^�Q�̂ق�����������...
	  if (sy2<sy) { sy = sy2; }
	  DWORD dwColor = ((rgb & 0xff) << 16) + (rgb & 0xff00) + ((rgb & 0xff0000)>>16);	//	�]�u����
	  ULONG lDstWidth = d_dib.GetWidth();
	  for(int y=0;y<sy;y++){
		DWORD *lpdw		= (DWORD*)(GetPtr() + 4*GetWidth() * y);
		DWORD *lpdw2a	= (DWORD*)(d_dib.GetPtr() + (4*lDstWidth * (y*2)));
		DWORD *lpdw2b	= lpdw2a + lDstWidth; // �����C��
		//	����ꂽ���邳�����`�����l���Ɏ����Ă���
		for(int x=0;x<sx;x++){
			DWORD dwAlpha;
			dwAlpha =  ((*lpdw2a)&0xff) + ((*(lpdw2a+1))&0xff);
			dwAlpha += ((*lpdw2b)&0xff) + ((*(lpdw2b+1))&0xff);
			*lpdw = ((dwAlpha << (24-2)) & 0xff000000) + dwColor;	//	�F�́Argb�Ŏw�肳�ꂽ�F
			lpdw++; lpdw2a+=2; lpdw2b+=2;
		}
	  }
	}

	//	���A������O�O�G�@�Ȃ񂬂�ȁ[�i�΁j
	m_Font.SetSize(nFontSize);
	m_Font.SetHeight(nFontHeight);
	//	�������Ŗ߂��Ȃ��ƁA�����̉e�𐶐�����Ƃ��ɉR�̃t�H���g�T�C�Y�ɂȂ����܂��O�O�G

	//	�����̉e
	if (bk!=CLR_INVALID) {
	//	�e��������񂩁D�D�D��炵���Ȃ��O�O�G
		CTextDIBitmap dib;
		*dib.GetFont() = *GetFont();	//	�t�H���g�܂邲�ƃR�s�[���Ă܂��[�I�I�O�O�G
		dib.GetFont()->SetColor(bk);
		dib.GetFont()->SetBackColor(CLR_INVALID);	//	�����CLR_INVALID�ɂ��Ă����΁A���̍ċA�͂Q�x�ڂŒ�~����
//		dib.GetFont()->SetText(m_Font.GetText());
//		dib.SetTextPos(2,2);	//	���炵�ĕ`��I
		{
			int nOx, nOy;
			dib.GetFont()->GetShadowOffset(nOx,nOy);
			dib.SetTextPos(nOx,nOy); // ���炵�ĕ`��I
		}
		dib.UpdateTextA();
		//	PhotoShop�̃��C���I�s�����x�t������

		int alpha;
		int beta;
		int total;
		DWORD dwRGB;

		//	����dib�̃T�C�Y�́A���T�C�Y�Ɖ���ł��Ȃ������m��Ȃ��D�D
//		int nSize = GetRect()->right*GetRect()->bottom;
//		for (int i=0;i<nSize;i++){

		//	dib�ɂ��炽�ɕ`�悳������ƃT�C�Y���قȂ邩���m��Ȃ��D�D
		ULONG lSrcWidth = GetWidth();
		ULONG lDstWidth = dib.GetWidth();
		for(int y=0;y<sy;y++){
			DWORD *lpdw = (DWORD*)(GetPtr() + (4* lSrcWidth * y));
			DWORD *lpdw2 = (DWORD*)(dib.GetPtr() + (4 * lDstWidth * y));
			//	����ꂽ���邳�����`�����l���Ɏ����Ă���
			for(int x=0;x<sx;x++){
				/*
				���̃A���S���Y���ɂ��ẮA��˂��炨�̃z�[���y�[�W��
				�X�[�p�[�v���O���}�[�ւ̓��@�u��CB��@PhotoShop�̃��C�������A���S���Y���ɂ��āiYGA�t�H�[�}�b�g��񏥂���) 00/10/18�v
				���Q�Ƃ̂���
				*/
				alpha = *lpdw >> 24;
				beta  = *lpdw2 >> 24;
				beta  = (255-alpha)*beta / 255;	//	�G�l���M�[�c�ʂ̌v�Z
				//	���z�䗦�����܂����̂ŁA����Ɋ�Â��ĕ��z
				total = alpha + beta;
				if (total != 0){	//	�[���̂Ƃ��́A���R�������Ȃ��ėǂ�
					if (beta == 0){		//�]���悪�O�Ȃ�A���̂܂ܓ]��
						*lpdw2 = *lpdw;
					}else{
						dwRGB = ((*lpdw & 0xff) * alpha + (*lpdw2 & 0xff) * beta) / total;
						dwRGB += (((*lpdw & 0xff00) * alpha + (*lpdw2 & 0xff00) * beta) / total) & 0xff00;
						dwRGB += (((*lpdw & 0xff0000) * alpha + (*lpdw2 & 0xff0000) * beta) / total) & 0xff0000;
						dwRGB += total << 24;	//	alpha�l�́A�G�l���M�[����
						*lpdw = dwRGB;
					}
				}
				lpdw++; lpdw2++;
			}
		}
	}

	//	���A������O�O�G�@�Ȃ񂬂�ȁ[�Q�i�΁j
	m_Font.SetQuality(nQuality);
	m_Font.SetColor(rgb);
	m_Font.SetBackColor(bk);

//	*GetYGA() = true;	//	for BltNatural

	return 0;
}

//	����̃R�s�y�����O�O�G
inline LRESULT CTextDIBitmap::UpdateTextHorizontalAA(){	//	�A���`�F���t���`��
	int sx,sy;
	m_Font.GetSize(sx,sy);
	if (CreateSurface(sx,sy,32,true)) return 1;
	//�@�w�i�F�œh��Ԃ�
	//	�������Ȃ��Ək�������T�[�t�F�X���R�s�[�������ɃS�~�����̂�

	//	GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	Clear();

	COLORREF rgb = m_Font.GetColor();
	COLORREF bk	 = m_Font.GetBackColor();

	m_Font.SetBackColor(CLR_INVALID);
	m_Font.SetColor(RGB(255,255,255));	//	���ɂ���
	int	nQuality = m_Font.GetQuality();
	m_Font.SetQuality(4);	//	�A���`�F�������ŏ���
	int nFontSize = m_Font.GetSize();
	int nFontHeight = m_Font.GetHeight();
	m_Font.SetSize(nFontSize<<2);	//	4�{�T�C�Y�ŕ`��
	m_Font.SetHeight(nFontHeight<<2); //�@�s�Ԃ�4�{�T�C�Y�ŕ`��

	CDIBitmap d_dib;
	int sx2,sy2;
	m_Font.GetSize(sx2,sy2);
	// m_Font�̃e�L�X�g����ł��v���[�������D
	if ((sx2 == 0) || (sy2 == 0)) {
		sx2 = sy2 = 1;
	}
	if (d_dib.CreateSurface(sx2,sy2,32,true)) return 2;
	// �w�i�F�œh��Ԃ�
	d_dib.GetSurfaceInfo()->SetFillColor(GetSurfaceInfo()->GetFillColor());
	//	d_dib.GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	d_dib.Clear();
	m_Font.OnDraw(d_dib.GetDC(),m_nTextX<<2,m_nTextY<<2);	//	4�{�̃t�H���g�T�C�Y�ŕ`��

	//	�S�~�S�����{�ɏk��
	{
	  sx2>>=2; sy2>>=2;
	  if (sx2<sx) { sx = sx2; }	//�@�Ȃ��H�������ꂽ�v���[���^�Q�̂ق�����������...
	  if (sy2<sy) { sy = sy2; }

	  ULONG dstWidth = d_dib.GetWidth();
	  ULONG srcWidth = GetWidth();
	  DWORD dwColor = ((rgb & 0xff) << 16) + (rgb & 0xff00) + ((rgb & 0xff0000)>>16);	//	�]�u����
	  for(int y=0;y<sy;y++){
		DWORD *lpdw = (DWORD*)(GetPtr() + (4 * srcWidth * y));
		DWORD *lpdw2a =(DWORD*)(d_dib.GetPtr() + (4*dstWidth * (y*4	 )));
		DWORD *lpdw2b = lpdw2a		   +  dstWidth; // �����C��
		DWORD *lpdw2c = lpdw2b		   +  dstWidth; // �����C��
		DWORD *lpdw2d = lpdw2c		   +  dstWidth; // �����C��
		//	����ꂽ���邳�����`�����l���Ɏ����Ă���
		for(int x=0;x<sx;x++){
			DWORD dwAlpha;
			dwAlpha =	((*lpdw2a)&0xff) + ((*(lpdw2a+1))&0xff) + ((*(lpdw2a+2))&0xff) + ((*(lpdw2a+3))&0xff);
			dwAlpha +=	((*lpdw2b)&0xff) + ((*(lpdw2b+1))&0xff) + ((*(lpdw2b+2))&0xff) + ((*(lpdw2b+3))&0xff);
			dwAlpha +=	((*lpdw2c)&0xff) + ((*(lpdw2c+1))&0xff) + ((*(lpdw2c+2))&0xff) + ((*(lpdw2c+3))&0xff);
			dwAlpha +=	((*lpdw2d)&0xff) + ((*(lpdw2d+1))&0xff) + ((*(lpdw2d+2))&0xff) + ((*(lpdw2d+3))&0xff);
			*lpdw = ((dwAlpha << (24-4)) & 0xff000000) + dwColor;	//	�F�́Argb�Ŏw�肳�ꂽ�F
			lpdw++; lpdw2a+=4; lpdw2b+=4; lpdw2c+=4; lpdw2d+=4;
		}
	  }
	}

	//	���A������O�O�G�@�Ȃ񂬂�ȁ[�i�΁j
	m_Font.SetSize(nFontSize);
	m_Font.SetHeight(nFontHeight);
	//	�������Ŗ߂��Ȃ��ƁA�����̉e�𐶐�����Ƃ��ɉR�̃t�H���g�T�C�Y�ɂȂ����܂��O�O�G

	//	�����̉e
	if (bk!=CLR_INVALID) {
	//	�e��������񂩁D�D�D��炵���Ȃ��O�O�G
		CTextDIBitmap dib;
		*dib.GetFont() = *GetFont();	//	�t�H���g�܂邲�ƃR�s�[���Ă܂��[�I�I�O�O�G
		dib.GetFont()->SetColor(bk);
		dib.GetFont()->SetBackColor(CLR_INVALID);	//	�����CLR_INVALID�ɂ��Ă����΁A���̍ċA�͂Q�x�ڂŒ�~����
//		dib.GetFont()->SetText(m_Font.GetText());
//		dib.SetTextPos(2,2);	//	���炵�ĕ`��I
		{
			int nOx, nOy;
			dib.GetFont()->GetShadowOffset(nOx,nOy);
			dib.SetTextPos(nOx,nOy); // ���炵�ĕ`��I
		}
		dib.UpdateTextAA();

		//	PhotoShop�̃��C���I�s�����x�t������
		int alpha;
		int beta;
		int total;
		DWORD dwRGB;

		//	dib�ɂ��炽�ɕ`�悳������ƃT�C�Y���قȂ邩���m��Ȃ��D�D
		ULONG lSrcWidth = GetWidth();
		ULONG lDstWidth = dib.GetWidth();

		for(int y=0;y<sy;y++){
			DWORD *lpdw = (DWORD*)(GetPtr() + (4*lSrcWidth * y));
			DWORD *lpdw2 = (DWORD*)(dib.GetPtr() + (4*lDstWidth * y));
			//	����ꂽ���邳�����`�����l���Ɏ����Ă���
			for(int x=0;x<sx;x++){
				/*
				���̃A���S���Y���ɂ��ẮA��˂��炨�̃z�[���y�[�W��
				�X�[�p�[�v���O���}�[�ւ̓��@�u��CB��@PhotoShop�̃��C�������A���S���Y���ɂ��āiYGA�t�H�[�}�b�g��񏥂���) 00/10/18�v
				���Q�Ƃ̂���
				*/
				alpha = *lpdw >> 24;
				beta  = *lpdw2 >> 24;
				beta  = (255-alpha)*beta / 255;	//	�G�l���M�[�c�ʂ̌v�Z
				//	���z�䗦�����܂����̂ŁA����Ɋ�Â��ĕ��z
				total = alpha + beta;
				if (total != 0){	//	�[���̂Ƃ��́A���R�������Ȃ��ėǂ�
					if (beta == 0){		//�]���悪�O�Ȃ�A���̂܂ܓ]��
						*lpdw2 = *lpdw;
					}else{
						dwRGB = ((*lpdw & 0xff) * alpha + (*lpdw2 & 0xff) * beta) / total;
						dwRGB += (((*lpdw & 0xff00) * alpha + (*lpdw2 & 0xff00) * beta) / total) & 0xff00;
						dwRGB += (((*lpdw & 0xff0000) * alpha + (*lpdw2 & 0xff0000) * beta) / total) & 0xff0000;
						dwRGB += total << 24;	//	alpha�l�́A�G�l���M�[����
						*lpdw = dwRGB;
					}
				}
				lpdw++; lpdw2++;
			}
		}
	}

	//	���A������O�O�G�@�Ȃ񂬂�ȁ[�Q�i�΁j
	m_Font.SetQuality(nQuality);
	m_Font.SetColor(rgb);
	m_Font.SetBackColor(bk);

//	*GetYGA() = true;	//	for BltNatural

	return 0;
}


//////////////////////////////////////////////////////////
// �c����

inline LRESULT CTextDIBitmap::UpdateTextVertical(){
	int sx,sy;

	m_Font.GetSize(sx,sy);
//----------------------------�ύX
	if (CreateSurface(sy,sx,32)) return 1;	// �t�ɂȂ�
	// �w�i�F�œh��Ԃ�
	//	�������Ȃ��Ək�������T�[�t�F�X���R�s�[�������ɃS�~�����̂�
	//GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	Clear();

	// �e�̌������c�����p�ɕύX
	int nOx, nOy;
	m_Font.GetShadowOffset(nOx, nOy);
	m_Font.SetShadowOffset(nOy, -nOx);

	// �c�����ϊ��p�ɂ�����T�[�t�F�X���쐬
	CDIBitmap d_dib2;
	if (d_dib2.CreateSurface(sx, sy,32)) return 2;
	// �w�i�F�œh��Ԃ�
	d_dib2.GetSurfaceInfo()->SetFillColor(GetSurfaceInfo()->GetFillColor());
	// d_dib2.GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	d_dib2.Clear();

 //	m_Font.OnDraw(GetDC(),m_nTextX,m_nTextY);
	m_Font.OnDraw(d_dib2.GetDC(),m_nTextX,m_nTextY);

	// �����c�ϊ�
	{
		LONG width, height;
		GetSize(width, height);
//		ULONG lSrcWidth = GetWidth();
		ULONG lDstWidth = d_dib2.GetWidth();
		for (int y=0;y<sy;y++) {
			DWORD *lpdw = (DWORD*)(GetPtr() + 4*(width - y -1));	// �������̏と���͏c�����̉E����
			DWORD *lpdw2 = (DWORD*)(d_dib2.GetPtr() + 4*(lDstWidth * y));
			for (int x=0;x<sx;x++) {
				*lpdw = *lpdw2;
				lpdw+=width;	// �P����
				lpdw2++;
			}
		}
	}

	// �e�̌��������ɖ߂�
	m_Font.SetShadowOffset(nOx, nOy);
//----------------------------


//	*GetYGA() = false;	//	for BltNatural
	return 0;
}

inline LRESULT CTextDIBitmap::UpdateTextVerticalA(){	//	�A���`�F���t���`��
	int sx,sy;
	m_Font.GetSize(sx,sy);
//----------------------------�ύX
	if (CreateSurface(sy,sx,32,true)) return 1;	// �t�ɂȂ�
	// �w�i�F�œh��Ԃ�
	//	�������Ȃ��Ək�������T�[�t�F�X���R�s�[�������ɃS�~�����̂�
	//GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	Clear();
//----------------------------

	COLORREF rgb = m_Font.GetColor();
	COLORREF bk	 = m_Font.GetBackColor();

	m_Font.SetBackColor(CLR_INVALID);
	m_Font.SetColor(RGB(255,255,255));	//	���ɂ���
	int	nQuality = m_Font.GetQuality();
	m_Font.SetQuality(4);	//	�A���`�F�������ŏ���
	int nFontSize = m_Font.GetSize();
	int nFontHeight = m_Font.GetHeight();
	m_Font.SetSize(nFontSize<<1);	//	2�{�T�C�Y�ŕ`��
	m_Font.SetHeight(nFontHeight<<1); //�@�s�Ԃ�2�{�T�C�Y�ŕ`��
	//	SetSize�́A�s�Ԃ��ύX���Ă��܂��̂ŁA
	//	SetSize����O�ɁA�s�Ԃ��擾���Ă����A������Q�{���Ȃ���΂Ȃ�Ȃ�

	CDIBitmap d_dib;
	int sx2,sy2;
	m_Font.GetSize(sx2,sy2);
	// m_Font�̃e�L�X�g����ł��v���[�������D
	if ((sx2 == 0) || (sy2 == 0)) {
		sx2 = sy2 = 1;
	}
	if (d_dib.CreateSurface(sx2,sy2,32,true)) return 2;
	// �w�i�F�œh��Ԃ�
	d_dib.GetSurfaceInfo()->SetFillColor(GetSurfaceInfo()->GetFillColor());
	//d_dib.GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	d_dib.Clear();
	m_Font.OnDraw(d_dib.GetDC(),m_nTextX<<1,m_nTextY<<1);	//	2�{�̃t�H���g�T�C�Y�ŕ`��


//----------------------------�ǉ�
	// �e�̌������c�����p�ɕύX
	int nOx, nOy;
	m_Font.GetShadowOffset(nOx, nOy);
	m_Font.SetShadowOffset(nOy, -nOx);

	// �c�����ϊ��p�ɂ�����T�[�t�F�X���쐬
	CDIBitmap d_dib2;
	if (d_dib2.CreateSurface(sx, sy,32,true)) return 2;
	// �w�i�F�œh��Ԃ�
	d_dib2.GetSurfaceInfo()->SetFillColor(GetSurfaceInfo()->GetFillColor());
	//d_dib2.GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	d_dib2.Clear();
//----------------------------


/*
	//	���{�̂Ƃ��̓]���͂���ŗǂ����D�D�D
	DWORD *lpdw = GetPtr();
	DWORD dwColor = ((rgb & 0xff) << 16) + (rgb & 0xff00) + ((rgb & 0xff0000)>>16);	//	�]�u����
	//	���邳�����`�����l���Ɏ����Ă���
	int nSize = GetRect()->right * GetRect()->bottom;
	for (int i=0;i<nSize;i++){
		*lpdw = ((*lpdw << 8)&0xff000000) + dwColor;	//	�F�́Argb�Ŏw�肳�ꂽ�F
		lpdw++;
	}
*/

	//	�Q�~�Q�����{�ɏk��
	{
	  sx2>>=1; sy2>>=1;
	  if (sx2<sx) { sx = sx2; }	//�@�Ȃ��H�������ꂽ�v���[���^�Q�̂ق�����������...
	  if (sy2<sy) { sy = sy2; }
	  DWORD dwColor = ((rgb & 0xff) << 16) + (rgb & 0xff00) + ((rgb & 0xff0000)>>16);	//	�]�u����

	  ULONG lDstWidth = d_dib.GetWidth();
	  ULONG lDstWidth2 = d_dib2.GetWidth();

	  for(int y=0;y<sy;y++){
//----------------------------�ύX
	//	DWORD *lpdw = GetPtr() + (GetRect()->right * y);
		DWORD *lpdw = (DWORD*)(d_dib2.GetPtr() + (4*lDstWidth2 * y));	// �c�����ϊ��p�̂��̂��g��
//----------------------------
		DWORD *lpdw2a = (DWORD*)(d_dib.GetPtr() + 4*(lDstWidth * (y*2	 )));
		DWORD *lpdw2b = lpdw2a		   + lDstWidth; // �����C��
		//	����ꂽ���邳�����`�����l���Ɏ����Ă���
		for(int x=0;x<sx;x++){
			DWORD dwAlpha;
			dwAlpha =  ((*lpdw2a)&0xff) + ((*(lpdw2a+1))&0xff);
			dwAlpha += ((*lpdw2b)&0xff) + ((*(lpdw2b+1))&0xff);
			*lpdw = ((dwAlpha << (24-2)) & 0xff000000) + dwColor;	//	�F�́Argb�Ŏw�肳�ꂽ�F
			lpdw++; lpdw2a+=2; lpdw2b+=2;
		}
	  }
	}

	//	���A������O�O�G�@�Ȃ񂬂�ȁ[�i�΁j
	m_Font.SetSize(nFontSize);
	m_Font.SetHeight(nFontHeight);
	//	�������Ŗ߂��Ȃ��ƁA�����̉e�𐶐�����Ƃ��ɉR�̃t�H���g�T�C�Y�ɂȂ����܂��O�O�G

	//	�����̉e
	if (bk!=CLR_INVALID) {
	//	�e��������񂩁D�D�D��炵���Ȃ��O�O�G
		CTextDIBitmap dib;
		*dib.GetFont() = *GetFont();	//	�t�H���g�܂邲�ƃR�s�[���Ă܂��[�I�I�O�O�G
		dib.GetFont()->SetColor(bk);
		dib.GetFont()->SetBackColor(CLR_INVALID);	//	�����CLR_INVALID�ɂ��Ă����΁A���̍ċA�͂Q�x�ڂŒ�~����
//		dib.GetFont()->SetText(m_Font.GetText());
//		dib.SetTextPos(2,2);	//	���炵�ĕ`��I
		{
			int nOx, nOy;
			dib.GetFont()->GetShadowOffset(nOx,nOy);
			dib.SetTextPos(nOx,nOy); // ���炵�ĕ`��I
		}
		dib.UpdateTextA();
		//	PhotoShop�̃��C���I�s�����x�t������

		int alpha;
		int beta;
		int total;
		DWORD dwRGB;

		//	����dib�̃T�C�Y�́A���T�C�Y�Ɖ���ł��Ȃ������m��Ȃ��D�D
//		int nSize = GetRect()->right*GetRect()->bottom;
//		for (int i=0;i<nSize;i++){

		ULONG lDstWidth = dib.GetWidth();
		ULONG lDstWidth2 = d_dib2.GetWidth();

		//	dib�ɂ��炽�ɕ`�悳������ƃT�C�Y���قȂ邩���m��Ȃ��D�D
		for(int y=0;y<sy;y++){
//----------------------------�ύX
	//		DWORD *lpdw = GetPtr() + (GetRect()->right * y);
			DWORD *lpdw = (DWORD*)(d_dib2.GetPtr() + (4*lDstWidth2 * y));	// �c�����ϊ��p�̂��̂��g��
//----------------------------
			DWORD *lpdw2 = (DWORD*)(dib.GetPtr() + (4*lDstWidth * y));
			//	����ꂽ���邳�����`�����l���Ɏ����Ă���
			for(int x=0;x<sx;x++){
				/*
				���̃A���S���Y���ɂ��ẮA��˂��炨�̃z�[���y�[�W��
				�X�[�p�[�v���O���}�[�ւ̓��@�u��CB��@PhotoShop�̃��C�������A���S���Y���ɂ��āiYGA�t�H�[�}�b�g��񏥂���) 00/10/18�v
				���Q�Ƃ̂���
				*/
				alpha = *lpdw >> 24;
				beta  = *lpdw2 >> 24;
				beta  = (255-alpha)*beta / 255;	//	�G�l���M�[�c�ʂ̌v�Z
				//	���z�䗦�����܂����̂ŁA����Ɋ�Â��ĕ��z
				total = alpha + beta;
				if (total != 0){	//	�[���̂Ƃ��́A���R�������Ȃ��ėǂ�
					if (beta == 0){		//�]���悪�O�Ȃ�A���̂܂ܓ]��
						*lpdw2 = *lpdw;
					}else{
						dwRGB = ((*lpdw & 0xff) * alpha + (*lpdw2 & 0xff) * beta) / total;
						dwRGB += (((*lpdw & 0xff00) * alpha + (*lpdw2 & 0xff00) * beta) / total) & 0xff00;
						dwRGB += (((*lpdw & 0xff0000) * alpha + (*lpdw2 & 0xff0000) * beta) / total) & 0xff0000;
						dwRGB += total << 24;	//	alpha�l�́A�G�l���M�[����
						*lpdw = dwRGB;
					}
				}
				lpdw++; lpdw2++;
			}
		}
	}

	//	���A������O�O�G�@�Ȃ񂬂�ȁ[�Q�i�΁j
	m_Font.SetQuality(nQuality);
	m_Font.SetColor(rgb);
	m_Font.SetBackColor(bk);


//----------------------------�ǉ�
	// �����c�ϊ�
	{
		LONG width, height;
		GetSize(width, height);
		ULONG lDstWidth2 = d_dib2.GetWidth();
		for (int y=0;y<sy;y++) {
			DWORD *lpdw = (DWORD*)(GetPtr() + 4*(width - y -1));	// �������̏と���͏c�����̉E����
			DWORD *lpdw2 = (DWORD*)(d_dib2.GetPtr() + 4*(lDstWidth2 * y));
			for (int x=0;x<sx;x++) {
				*lpdw = *lpdw2;
				lpdw+=width;	// �P����
				lpdw2++;
			}
		}
	}

	// �e�̌��������ɖ߂�
	m_Font.SetShadowOffset(nOx, nOy);
//----------------------------


//	*GetYGA() = true;	//	for BltNatural

	return 0;
}

//	����̃R�s�y�����O�O�G
inline LRESULT CTextDIBitmap::UpdateTextVerticalAA(){	//	�A���`�F���t���`��
	int sx,sy;
	m_Font.GetSize(sx,sy);
//----------------------------�ύX
	//	if (CreateSurface(sx,sy)) return 1;
	if (CreateSurface(sy,sx,32,true)) return 1;	// �t�ɂȂ�
	//�@�w�i�F�œh��Ԃ�
	//	�������Ȃ��Ək�������T�[�t�F�X���R�s�[�������ɃS�~�����̂�
	//GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	Clear();
//----------------------------

	COLORREF rgb = m_Font.GetColor();
	COLORREF bk	 = m_Font.GetBackColor();

	m_Font.SetBackColor(CLR_INVALID);
	m_Font.SetColor(RGB(255,255,255));	//	���ɂ���
	int	nQuality = m_Font.GetQuality();
	m_Font.SetQuality(4);	//	�A���`�F�������ŏ���
	int nFontSize = m_Font.GetSize();
	int nFontHeight = m_Font.GetHeight();
	m_Font.SetSize(nFontSize<<2);	//	4�{�T�C�Y�ŕ`��
	m_Font.SetHeight(nFontHeight<<2); //�@�s�Ԃ�4�{�T�C�Y�ŕ`��

	CDIBitmap d_dib;
	int sx2,sy2;
	m_Font.GetSize(sx2,sy2);
	// m_Font�̃e�L�X�g����ł��v���[�������D
	if ((sx2 == 0) || (sy2 == 0)) {
		sx2 = sy2 = 1;
	}
	if (d_dib.CreateSurface(sx2,sy2,32,true)) return 2;
	// �w�i�F�œh��Ԃ�
	d_dib.GetSurfaceInfo()->SetFillColor(GetSurfaceInfo()->GetFillColor());
	//d_dib.GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	d_dib.Clear();
	m_Font.OnDraw(d_dib.GetDC(),m_nTextX<<2,m_nTextY<<2);	//	4�{�̃t�H���g�T�C�Y�ŕ`��


//----------------------------�ǉ�
	// �e�̌������c�����p�ɕύX
	int nOx, nOy;
	m_Font.GetShadowOffset(nOx, nOy);
	m_Font.SetShadowOffset(nOy, -nOx);

	// �c�����ϊ��p�ɂ�����T�[�t�F�X���쐬
	CDIBitmap d_dib2;
	if (d_dib2.CreateSurface(sx,sy,32,true)) return 2;
	// �w�i�F�œh��Ԃ�
	d_dib2.GetSurfaceInfo()->SetFillColor(GetSurfaceInfo()->GetFillColor());
	//d_dib2.GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	d_dib2.Clear();
//----------------------------


	//	�S�~�S�����{�ɏk��
	{
	  sx2>>=2; sy2>>=2;
	  if (sx2<sx) { sx = sx2; }	//�@�Ȃ��H�������ꂽ�v���[���^�Q�̂ق�����������...
	  if (sy2<sy) { sy = sy2; }
	  DWORD dwColor = ((rgb & 0xff) << 16) + (rgb & 0xff00) + ((rgb & 0xff0000)>>16);	//	�]�u����
	
	  ULONG lDstWidth = d_dib.GetWidth();
	  ULONG lDstWidth2 = d_dib2.GetWidth();
	  for(int y=0;y<sy;y++){
//----------------------------�ύX
	//	DWORD *lpdw = GetPtr() + (GetRect()->right * y);
		DWORD *lpdw = (DWORD*)(d_dib2.GetPtr() + (4*lDstWidth2 * y));	// �c�����ϊ��p�̂��̂��g��
//----------------------------
		DWORD *lpdw2a = (DWORD*)(d_dib.GetPtr() + (4*lDstWidth * (y*4	 )));
		DWORD *lpdw2b = (DWORD*)(lpdw2a		   +  lDstWidth); // �����C��
		DWORD *lpdw2c = (DWORD*)(lpdw2b		   +  lDstWidth); // �����C��
		DWORD *lpdw2d = (DWORD*)(lpdw2c		   +  lDstWidth); // �����C��
		//	����ꂽ���邳�����`�����l���Ɏ����Ă���
		for(int x=0;x<sx;x++){
			DWORD dwAlpha;
			dwAlpha =	((*lpdw2a)&0xff) + ((*(lpdw2a+1))&0xff) + ((*(lpdw2a+2))&0xff) + ((*(lpdw2a+3))&0xff);
			dwAlpha +=	((*lpdw2b)&0xff) + ((*(lpdw2b+1))&0xff) + ((*(lpdw2b+2))&0xff) + ((*(lpdw2b+3))&0xff);
			dwAlpha +=	((*lpdw2c)&0xff) + ((*(lpdw2c+1))&0xff) + ((*(lpdw2c+2))&0xff) + ((*(lpdw2c+3))&0xff);
			dwAlpha +=	((*lpdw2d)&0xff) + ((*(lpdw2d+1))&0xff) + ((*(lpdw2d+2))&0xff) + ((*(lpdw2d+3))&0xff);
			*lpdw = ((dwAlpha << (24-4)) & 0xff000000) + dwColor;	//	�F�́Argb�Ŏw�肳�ꂽ�F
			lpdw++; lpdw2a+=4; lpdw2b+=4; lpdw2c+=4; lpdw2d+=4;
		}
	  }
	}

	//	���A������O�O�G�@�Ȃ񂬂�ȁ[�i�΁j
	m_Font.SetSize(nFontSize);
	m_Font.SetHeight(nFontHeight);
	//	�������Ŗ߂��Ȃ��ƁA�����̉e�𐶐�����Ƃ��ɉR�̃t�H���g�T�C�Y�ɂȂ����܂��O�O�G

	//	�����̉e
	if (bk!=CLR_INVALID) {
	//	�e��������񂩁D�D�D��炵���Ȃ��O�O�G
		CTextDIBitmap dib;
		*dib.GetFont() = *GetFont();	//	�t�H���g�܂邲�ƃR�s�[���Ă܂��[�I�I�O�O�G
		dib.GetFont()->SetColor(bk);
		dib.GetFont()->SetBackColor(CLR_INVALID);	//	�����CLR_INVALID�ɂ��Ă����΁A���̍ċA�͂Q�x�ڂŒ�~����
//		dib.GetFont()->SetText(m_Font.GetText());
//		dib.SetTextPos(2,2);	//	���炵�ĕ`��I
		{
			int nOx, nOy;
			dib.GetFont()->GetShadowOffset(nOx,nOy);
			dib.SetTextPos(nOx,nOy); // ���炵�ĕ`��I
		}
		dib.UpdateTextAA();

		//	PhotoShop�̃��C���I�s�����x�t������
		int alpha;
		int beta;
		int total;
		DWORD dwRGB;

		//	dib�ɂ��炽�ɕ`�悳������ƃT�C�Y���قȂ邩���m��Ȃ��D�D

		ULONG lDstWidth = dib.GetWidth();
		ULONG lDstWidth2 = d_dib2.GetWidth();
		for(int y=0;y<sy;y++){
//----------------------------�ύX
		//	DWORD *lpdw = GetPtr() + (GetRect()->right * y);
			DWORD *lpdw = (DWORD*)(d_dib2.GetPtr() + (4*lDstWidth2 * y));	// �c�����ϊ��p�̂��̂��g��
//----------------------------
			DWORD *lpdw2 = (DWORD*)(dib.GetPtr() + (4*lDstWidth * y));
			//	����ꂽ���邳�����`�����l���Ɏ����Ă���
			for(int x=0;x<sx;x++){
				/*
				���̃A���S���Y���ɂ��ẮA��˂��炨�̃z�[���y�[�W��
				�X�[�p�[�v���O���}�[�ւ̓��@�u��CB��@PhotoShop�̃��C�������A���S���Y���ɂ��āiYGA�t�H�[�}�b�g��񏥂���) 00/10/18�v
				���Q�Ƃ̂���
				*/
				alpha = *lpdw >> 24;
				beta  = *lpdw2 >> 24;
				beta  = (255-alpha)*beta / 255;	//	�G�l���M�[�c�ʂ̌v�Z
				//	���z�䗦�����܂����̂ŁA����Ɋ�Â��ĕ��z
				total = alpha + beta;
				if (total != 0){	//	�[���̂Ƃ��́A���R�������Ȃ��ėǂ�
					if (beta == 0){		//�]���悪�O�Ȃ�A���̂܂ܓ]��
						*lpdw2 = *lpdw;
					}else{
						dwRGB = ((*lpdw & 0xff) * alpha + (*lpdw2 & 0xff) * beta) / total;
						dwRGB += (((*lpdw & 0xff00) * alpha + (*lpdw2 & 0xff00) * beta) / total) & 0xff00;
						dwRGB += (((*lpdw & 0xff0000) * alpha + (*lpdw2 & 0xff0000) * beta) / total) & 0xff0000;
						dwRGB += total << 24;	//	alpha�l�́A�G�l���M�[����
						*lpdw = dwRGB;
					}
				}
				lpdw++; lpdw2++;
			}
		}
	}

	//	���A������O�O�G�@�Ȃ񂬂�ȁ[�Q�i�΁j
	m_Font.SetQuality(nQuality);
	m_Font.SetColor(rgb);
	m_Font.SetBackColor(bk);


//----------------------------�ǉ�
	// �����c�ϊ�
	{
		LONG width, height;
		GetSize(width, height);

		ULONG lDstWidth2 = d_dib2.GetWidth();

		for (int y=0;y<sy;y++) {
			DWORD *lpdw = (DWORD*)(GetPtr() + 4*(width - y -1));	// �������̏と���͏c�����̉E����
			DWORD *lpdw2 = (DWORD*)(d_dib2.GetPtr() + 4*(lDstWidth2 * y));
			for (int x=0;x<sx;x++) {
				*lpdw = *lpdw2;
				lpdw+=width;	// �P����
				lpdw2++;
			}
		}
	}

	// �e�̌��������ɖ߂�
	m_Font.SetShadowOffset(nOx, nOy);
//----------------------------


//	*GetYGA() = true;	//	for BltNatural

	return 0;
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
