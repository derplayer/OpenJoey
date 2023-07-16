
#include "stdafx.h"

#ifdef USE_FastDraw

#include "yaneFastPlane.h"
#include "yaneFastDraw.h"
#include "yaneDirectDraw.h"
#include "yaneDIBitmap.h"
#include "yaneGraphicLoader.h"
#include "../Auxiliary/yaneFile.h"
#include "yaneGTL.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

//////////////////////////////////////////////////////
//	static members..

ThreadLocal<CFastDraw*> CFastPlane::m_pDefaultFastDraw;

//////////////////////////////////////////////////////

CFastPlane::CFastPlane(CFastDraw* pFastDraw)
{
	m_pFastDraw = NULL;
	SetFastDraw(pFastDraw);

	m_nSizeX	=	0;
	m_nSizeY	=	0;
	m_lpSurface	=	NULL;
	m_lpPalette	=	NULL;

	ResetColorKey();
	m_hDC		=	NULL;

	//	�V�X�e����������Ɋm�ۂ���̂��I
	m_bUseSystemMemory	=	true;

	m_bYGA			= false;
	m_bYGAUse		= false;
	m_bMySurface	= false;
	m_bSecondary256 = false;
	//	256�F���[�h�Ȃ�΁A�ʏ�RGB555�̃T�[�t�F�[�X���쐬����̂����A
	//	�Z�J���_��������256�F�T�[�t�F�[�X
	m_bLoad256		= false;
	m_bSecondary256DIB=false;

	//	RGB555��MySurface�ɑ΂��āADIB Section���g�p����̂��H
	m_hBitmap		= NULL;
	m_hDC			= NULL;

	//	�����C���T�[�t�F�[�X
	m_bAutoRestore = false;
	//	�I�[�i�[�h���[(Restore���Ăяo����Ȃ��BPrimary,Secondary�͂���)
	m_bOwnerDraw	= false;
	m_bPrimary		= false;

	m_bNowRestoring	= false;
}

CFastPlane::~CFastPlane(){
	Release();
}

smart_ptr<ISurface> CFastPlane::clone() {
	return smart_ptr<ISurface>(new CFastPlane(GetMyFastDraw()));
}

LRESULT	CFastPlane::Release(){
	//	owner create surface
	//	256�F���[�h�̉��z�Z�J���_����DIB�Ƃ��č쐬���Ă��񂩁H
	if (m_hBitmap){
		m_bMySurface = false;
		if (m_hDC!=NULL){
			::DeleteDC(m_hDC);
			m_hDC = NULL;
		}
		if (m_hBitmap!=NULL){
			::DeleteObject(m_hBitmap);
			m_hBitmap = NULL;
		}
	}

	if (m_bMySurface) {
		switch (GetSurfaceType()){
		case 3: {
			CFastPlaneRGB565* pwd = (CFastPlaneRGB565*)GetSurfaceInfo()->GetPtr();
			delete [] pwd; } break;
		case 4: {
			CFastPlaneRGB555* pwd = (CFastPlaneRGB555*)GetSurfaceInfo()->GetPtr();
			delete [] pwd; } break;
		case 5: {
			CFastPlaneRGB888* pwd = (CFastPlaneRGB888*)GetSurfaceInfo()->GetPtr();
			delete [] pwd; } break;
		case 6: {
			CFastPlaneBGR888* pwd = (CFastPlaneBGR888*)GetSurfaceInfo()->GetPtr();
			delete [] pwd; } break;
		case 7: {
			CFastPlaneXRGB8888* pwd = (CFastPlaneXRGB8888*)GetSurfaceInfo()->GetPtr();
			delete [] pwd; } break;
		case 8: {
			CFastPlaneXBGR8888* pwd = (CFastPlaneXBGR8888*)GetSurfaceInfo()->GetPtr();
			delete [] pwd; } break;
		case 10: {
			CFastPlaneARGB4565* pwd = (CFastPlaneARGB4565*)GetSurfaceInfo()->GetPtr();
			delete [] pwd; } break;
		case 11: {
			CFastPlaneARGB4555* pwd = (CFastPlaneARGB4555*)GetSurfaceInfo()->GetPtr();
			delete [] pwd; } break;
		case 12: {
			CFastPlaneARGB8888* pdw = (CFastPlaneARGB8888*)GetSurfaceInfo()->GetPtr();
			delete [] pdw; } break;
		case 13: {
			CFastPlaneABGR8888* pdw = (CFastPlaneABGR8888*)GetSurfaceInfo()->GetPtr();
			delete [] pdw; } break;
		default:
			Err.Out("CFastPlane::Release�ŕs���T�[�t�F�[�X�̃����[�X");
		}
		m_bMySurface = false;
	}

	RELEASE_SAFE(m_lpSurface);
	RELEASE_SAFE(m_lpPalette);
	m_strBitmapFile.erase();
	m_nSizeX	=	0;
	m_nSizeY	=	0;
	m_bLoad256	= false;
	GetSurfaceInfo()->SetInit(false);	//	�T�[�t�F�[�X����������

	if (!m_bNowRestoring && GetMyFastDraw()!=NULL){
		GetMyFastDraw()->GetFastPlaneList()->erase(this);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////

LRESULT	CFastPlane::GetSize(int &x,int &y) const {
	x = m_nSizeX;
	y = m_nSizeY;
	return const_cast<CFastPlane*>(this)->GetSurfaceInfo()->IsInit()?0:1;
}

//	�T�[�t�F�[�X�̃��X�g�ɑ΂��镜�A����
LRESULT	CFastPlane::Restore(){
	LRESULT lr = 0;
	m_bNowRestoring = true;	//	���X�g�A���ɂ��ARelease�Ńt�b�N��������̋֎~�I
	int nType = GetSurfaceInfo()->GetSurfaceType();
	if (nType!=0){	//	Surface�����݂��邩�ǂ����́A�����Ŕ��肷��K�v������
		//	�V�X�e����������̃T�[�t�F�[�X��Lost���Ȃ��̂����O�O�G
		//	���݂̉��bpp�ƈقȂ�T�[�t�F�[�X�Ȃ�΁A��̂��č��Ȃ����K�v������
		bool bRestore = false;
		if (m_lpSurface!=NULL && m_lpSurface->IsLost()) {
			//	&& �_�����Z�q�́A������E�ւ̕]����ۏ؂��邵�A
			//	VC�̍œK���ɂ��A�����������ɉE���]������Ȃ��̂��ۏ؂����
			bRestore = true;
		} else {
		//	���݂̉��bpp�ƃR���p�`�Ŗ����T�[�t�F�[�X�Ȃ�΃��X�g�A����
			int nType2 = GetMyFastDraw()->GetPrimary()->GetSurfaceType();
			switch (nType2){
			case 2:	//	256�F�̂Ƃ��́ARGB555�Ƃ݂Ȃ�
				if (nType==2 && m_bSecondary256) break; // 256�F�p�Z�J���_�����H
				if (nType==4 || nType == 11) break;
				bRestore = true; break;
			case 3:
				if (nType==3 || nType == 10) break;
				bRestore = true; break;
			case 4:
				//	256�F���[�h�̎��ɓǂݍ��񂾃r�b�g�}�b�v�Ȃ񂩐M�p�ł��Ȃ�
				if (m_bLoad256 && IsLoaded()) {
					bRestore = true; break;
				}
				if (nType==4 || nType == 11) break;
				bRestore = true; break;
			case 5:
			case 6:
			case 7:
			case 8:
				if (nType==nType2 || nType == 12) break;
				bRestore = true; break;
			}
		}

		if (bRestore){
			if (m_bAutoRestore){
			//	�����C���T�[�t�F�[�X�Ȃ�΁A�T�[�t�F�[�X���쐬��A�������R�s�[
				LRESULT lrErr = 0;
				if (m_nSizeX!=0 && m_nSizeY!=0){
					CFastPlane plane;
					plane.CreateSurface(m_nSizeX,m_nSizeY,IsYGA());
					lrErr = plane.BltFast(this,0,0);
					// ���̕ϊ��\��������H
					if (lrErr==0){
						InnerCreateSurface(m_nSizeX,m_nSizeY,m_bYGA,m_bSecondary256);
						lrErr = BltFast(&plane,0,0);
					}
				}
				//	�]���Ɏ��s������A����[�Ȃ����玩�O�Ń��X�g�A���Ă񂩁D�D
				if (lrErr!=0) goto RestoreRetry;

				//	���݂̉�ʃ��[�h�ɍ��킹��ColorKey��ݒ肵�Ȃ���
				UpdateColorKey();
			} else {
RestoreRetry:;
				//	�r�b�g�}�b�v�t�@�C���Ȃ�΂���𕜌�����
				if (IsLoaded()){
					lr = InnerLoad(m_strBitmapFile);
				} else {
					//	�{���́A���̂Ƃ��A�ϊ��q��p�ӂ��ĕϊ��ق����ǂ�
					//	�������A�����̂��ׂĂ̑g�ݍ��킹�͖c��ł���D�D
					//	��x�AARGB8888�ɕϊ����āA��������^�[�Q�b�g�^��
					//	�߂��΁A�����ł������̂����D�D

					if (m_nSizeX!=0 && m_nSizeY!=0){
						//	�Ƃ肠�����A�T�[�t�F�[�X���Ȃ����Ƃ����D�D
						InnerCreateSurface(m_nSizeX,m_nSizeY,m_bYGA,m_bSecondary256);
					}
				}
				//	���݂̉�ʃ��[�h�ɍ��킹��ColorKey��ݒ肵�Ȃ���
				UpdateColorKey();
				//	�I�[�i�[�h���[�����m��Ȃ��̂ŁA����𕜌�����
				lr |= OnDraw();	//	�Ϗ�����
			}
		}
	}
	m_bNowRestoring = false;	//	���X�g�A�I���ɂ��ARelease�Ńt�b�N��������̉����I
	return lr;
}


LRESULT CFastPlane::Load(const string& strBitmapFileName){
	Release();
	ResetColorKey();
	// ���Ƃ�Restore�ł���悤�Ƀt�@�C�������i�[���Ă����B
	LRESULT lr = InnerLoad(strBitmapFileName);
	m_strBitmapFile = strBitmapFileName;
	return lr;
}

//	�t�@�C������Ԃ�
string	CFastPlane::GetFileName() const{
	return m_strBitmapFile;
}

////////////////////////////////////////////////////////////////////////////////////

//	�r�b�g�}�b�v�̓����I�ȃ��[�h�B�i�[�t�@�C�����ɂ͉e�����Ȃ�
LRESULT	CFastPlane::InnerLoad(const string& strFileName){
/**
	ISurface::Load����������Ă���̂ŁA��{�I�ȓǂݍ��݂́A
	�T�[�t�F�[�X�쐬��ɁAISurface::Load�ɈϏ�����Ηǂ�
*/

	/**
		InnerLoad�́A
		�r�b�g�}�b�v�t�@�C�����A�����F�̎w�蓙��j�󂵂Ȃ����Ƃ�ۏ؂���
		���̂���Release�O�ɂ��ׂĂ̐ݒ��ۑ�����
	*/
	string strBitmap(m_strBitmapFile);
	//	colorkey�̐ݒ���ۑ�

	bool bUsePosColorKey = m_bUsePosColorKey;
	int	nColorKeyX = m_nColorKeyX, nColorKeyY = m_nColorKeyY;
	ISurfaceRGB rgbColorKey = m_rgbColorKey;
	{
		Release();	//	���
	}
	m_strBitmapFile = strBitmap;
	m_bUsePosColorKey = bUsePosColorKey;
	m_nColorKeyX = nColorKeyX; m_nColorKeyY = nColorKeyY;
	m_rgbColorKey = rgbColorKey;


	//	#notdefined#�Ȃ�΁A����I�������Ƃ��ċA��
	if (strFileName == "#notdefined#"){
		return 0;
	}

	//	nSurfaceType�̃T�[�t�F�[�X���쐬���A�����ɓǂݍ���

	int nSurfaceType = GetMyFastDraw()->GetPrimary()->GetSurfaceType();

	m_bLoad256 = false;
	if (m_bYGAUse){
	//	���T�[�t�F�[�X���쐬����̂��H
		nSurfaceType = GetYGASurfaceType(nSurfaceType);
		//	�Ή����郿�T�[�t�F�[�X�̔ԍ��𓾂�
	} else {
		switch(nSurfaceType){
		case 2: nSurfaceType = 4; m_bLoad256=true; break;
		// ---- 8bpp�Ȃ�� RGB555�ō쐬�B
		//	���̂Ƃ��ǂݍ��񂾃r�b�g�}�b�v�́A
		//	���̉�ʃ��[�h�Ɉڍs�����Ƃ��Ƀ��X�g�A���ׂ�
		}
	}

	LRESULT lr = ISurface::LoadByType(strFileName,nSurfaceType);
	if (lr!=0) return lr;

	// �����F�����Z�b�g���Ƃ����[
	// ResetColorKey();�@���܂�����('02/11/01)yane.
	// �����F���X�V���Ƃ����[
	UpdateColorKey();

	return lr;
}

HDC CFastPlane::GetDC(){
	if (m_hBitmap!=NULL){
		return m_hDC;
	}
	
	if (m_hDC!=NULL) {
		Err.Out("CFastPlane::EndPaint���Ăяo����Ă��Ȃ��̂�BeginPaint���Ăяo���ꂽ");
		return NULL;
	}
	if (m_lpSurface==NULL) return NULL;
	if (m_lpSurface->GetDC(&m_hDC)!=DD_OK) return NULL;
	return m_hDC;
}

void CFastPlane::ReleaseDC(){
	if (m_hBitmap!=NULL){
		return ;
	}

	if (m_hDC==NULL) {
		Err.Out("CFastPlane::BeginPaint���Ăяo����Ă��Ȃ��̂�EndPaint���Ăяo���ꂽ");
		return ;
	}
	if (m_lpSurface==NULL) return ;
	if (m_lpSurface->ReleaseDC(m_hDC)!=DD_OK) return ;
	m_hDC = NULL;
}

LRESULT CFastPlane::InnerCreateMySurface(int sx,int sy,int nSurfaceType,bool bClear/*=true*/){

	m_nSizeX = sx; m_nSizeY = sy;
	SIZE rc = { m_nSizeX,m_nSizeY };

	switch (nSurfaceType){
	case 4:	{ //	RGB555���쐬
		CFastPlaneRGB555* lpSurface;
		if (m_bSecondary256DIB){
			//	256�F�p�̉��z�Z�J���_���ł���Ȃ�΁ADIBSection�ō���Ă���
			//	(HDC���擾�ł���悤�ɂ��邽��)
			BITMAPINFO bmi;
			ZERO(bmi);
			bmi.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth		=  m_nSizeX;
			bmi.bmiHeader.biHeight		= -m_nSizeX;
			bmi.bmiHeader.biPlanes		= 1;
			bmi.bmiHeader.biBitCount	= 16;
			bmi.bmiHeader.biCompression	= BI_RGB;
			HDC hdc = ::GetDC(NULL); // hWnd�̂ق��������񂩁H
			m_hBitmap = ::CreateDIBSection(hdc /* NULL*/, &bmi , DIB_RGB_COLORS, (void**)&lpSurface, NULL, 0 );
			//	������ō����DIB��RGB555�̂悤�����H
			::ReleaseDC(NULL,hdc);
			if (m_hBitmap==NULL) return 1;
			m_hDC = ::CreateCompatibleDC(NULL);
//			m_hDC = ::CreateCompatibleDC(::GetDC(NULL));
			::SelectObject(m_hDC,m_hBitmap);
		} else {
			lpSurface = new CFastPlaneRGB555[m_nSizeX * m_nSizeY];
		}
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneRGB555),rc);
		break;
				}
		//	���̂Ƃ��AHDC�͎擾�ł��Ȃ��B�S�����l
	case 3:	{ //	RGB565���쐬
		CFastPlaneRGB565* lpSurface = new CFastPlaneRGB565[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneRGB565),rc);
		break;
				}
	case 5: { //	RGB888���쐬
		CFastPlaneRGB888* lpSurface = new CFastPlaneRGB888[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneRGB888),rc);
		break;
			}
	case 6: { //	BGR888���쐬
		CFastPlaneBGR888* lpSurface = new CFastPlaneBGR888[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneBGR888),rc);
		break;
			}
	case 7: { //	XRGB8888���쐬
		CFastPlaneXRGB8888* lpSurface = new CFastPlaneXRGB8888[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneXRGB8888),rc);
		break;
			}
	case 8: { //	XBGR8888���쐬
		CFastPlaneXBGR8888* lpSurface = new CFastPlaneXBGR8888[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneXBGR8888),rc);
		break;
			}

	case 10:	{ //	ARGB4565���쐬
		CFastPlaneARGB4565* lpSurface = new CFastPlaneARGB4565[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneARGB4565),rc);
		break;
				}
	case 11: { //	ARGB4555���쐬
		CFastPlaneARGB4555* lpSurface = new CFastPlaneARGB4555[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneARGB4555),rc);
		break;
				}
	case 12:	//	ARGB8888
			{ //	ARGB8888���쐬
		CFastPlaneARGB8888* lpSurface = new CFastPlaneARGB8888[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneARGB8888),rc);
		break;
				}
	case 13:	//	ABGR8888
			{ //	ABGR8888���쐬
		CFastPlaneABGR8888* lpSurface = new CFastPlaneABGR8888[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneABGR8888),rc);
		break;
				}
	default:
		return 1;	//	�T�|�[�g���Ăւ�ŁO�O�G
	}

	m_bYGA = (nSurfaceType>=10);

	GetSurfaceInfo()->SetSurfaceType(nSurfaceType);
	m_bMySurface = true; // ������new�����T�[�t�F�[�X�ł��邱�Ƃ��Ӗ�����}�[�J�[

	//	�N���A�t���O�����ĂȂ��Ȃ�N���A���Ȃ�
	if (bClear) Clear();

	//	hook �J�n����
	if (!m_bNowRestoring && GetMyFastDraw()!=NULL){
		GetMyFastDraw()->GetFastPlaneList()->insert(this);
	}
	return 0;
}

LRESULT CFastPlane::CreateSurfaceByType(int sx,int sy,int nType){
	return InnerCreateMySurface(sx,sy,nType,false);
}

LRESULT CFastPlane::InnerCreateSurface(int sx,int sy,bool bYGA,bool bSecondary256){
	Release();

	if (sx==0 || sy==0) {
		return 1;	//	����ȃT�[�t�F�[�X�A���ق��Ă���[�O�O�G
	}

	//	���݂̉�ʃ��[�h�ɉ��������̂ɂ���K�v����
	int nType = GetMyFastDraw()->GetPrimary()->GetSurfaceType();

	if (!bYGA) {
		//	YGA�摜�ł͖����̂Ŗ�������DirectDrawSurface
		//	�������A8bpp�̂Ƃ��́ARGB555�̃T�[�t�F�[�X�����
		if (nType == 2 && !bSecondary256) {
			return InnerCreateMySurface(sx,sy,4);
		} else {
			if (bSecondary256) m_bSecondary256 = true;
			//	���̃T�[�t�F�[�X�́A256�F�p�Z�J���_��
		}

		///	���݂̃v���C�}�����傫�ȃT�[�t�F�[�X�͍쐬�Ɏ��s����
		///	(DirectX3�̐���)
		int nPx,nPy;
		GetMyFastDraw()->GetPrimary()->GetSize(nPx,nPy);
		bool bCreateDirectDrawSurface = !(nPx < sx || nPx < sy);
		if (bCreateDirectDrawSurface) {

			DDSURFACEDESC ddsd;
			ZERO(ddsd);
			ddsd.dwSize = sizeof(ddsd);
			ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
			//	�����I�ɃV�X�e�����������g���I�v�V����
			if (m_bUseSystemMemory) {
				ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			} else {
				ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
			}
			// �T�C�Y��ۑ����Ă���
			m_nSizeX = ddsd.dwWidth	 = sx;
			m_nSizeY = ddsd.dwHeight = sy;

			//	RGB888�ŁA��o�C�g�ŏI����Ă���Ƃ�炵���̂ŁA
			//	�]���Ɋm�ۂ���悤�ɂ���BdWidth����Ȃ�΋����Ŋm�ۂ���悤�ɂ���
			if (GetMyFastDraw()->GetPrimary()->GetBpp()==24 && ((ddsd.dwWidth & 1)==1)){
				ddsd.dwWidth++;	//	�����I�ɋ����o�C�g�ɃA���C��
			}
			LPDIRECTDRAW lpDraw = GetMyFastDraw()->GetDDraw();
			if (lpDraw==NULL) return 1;
			if (lpDraw->CreateSurface(&ddsd,&m_lpSurface,NULL)!=DD_OK){
				Err.Out("CFastPlane::InnerCreateSurface��CreateSurface�Ɏ��s");
				return 2; // ������[
			}
		} else {
			return InnerCreateMySurface(sx,sy,nType);
		}

		UpdateSurfaceInfo();

		//	m_nSurfaceRef = 1;	//	�Q�ƃJ�E���g�̐ݒ�O�O
		Clear();	//	�O�̂��߃N���A���Ă���(�ŏ�ʂ�0�ɂ��邽��)
	} else {
		//	�x�f�`�摜�Ȃ̂ŁA���݂̉�ʃ��[�h�ɉ��������̂ɂ���K�v����
		int nType = GetMyFastDraw()->GetPrimary()->GetSurfaceType();
		switch (nType){
		case 3:	 //	ARGB4565���쐬
			return InnerCreateMySurface(sx,sy,10);
			break;

		case 2:	 // ��8bpp�̂Ƃ��́ARGB555�Ȃ̂ŁA����ɑ΂���YGA��ARGB4555
		case 4:	 //	ARGB4555���쐬
			return InnerCreateMySurface(sx,sy,11);
			break;

		//	RGB���Ȃ��ARGB8888�ł����񂿂Ⴄ�H
		case 5:	//	RGB888
		case 7:	//	XRGB8888
			return InnerCreateMySurface(sx,sy,12);
			break;
		//	BGR���Ȃ�ΖO�a���Z�̂��ƂƂ��l����ABGR8888�ɂ��Ƃ����H
		case 6:	//	BGR888
		case 8:	//	XBGR8888
			return InnerCreateMySurface(sx,sy,13);
			break;
		}

		//	�N���A���Ă����Ȃ��ƃ��ɃS�~���c�����܂܂ɂȂ�D�D�O�O�G
		Clear();
	}

	m_bYGA = bYGA;

	//	hook �J�n����
	//	--- �����ȊO�́AInnerCreateMySurface�̂Ȃ���hook���Ă���̂�ok
	if (GetMyFastDraw()!=NULL){
		GetMyFastDraw()->GetFastPlaneList()->insert(this);
	}
	return 0;
}

LRESULT CFastPlane::CreateSurface(int sx,int sy,bool bYGA){
	//	���ʂ�CreateSurface����ƁA����͊ԈႢ�Ȃ��I�[�i�[�h���[�v���[���ł���
//	m_bOwnerDraw	= false;
//	m_bBitmap		= false;
	ResetColorKey();

	LRESULT lr = InnerCreateSurface(sx,sy,bYGA || m_bYGAUse);
	if (lr) return lr;

	UpdateColorKey();

	//	CreateSurface���Ă�񂾂���AFillColor�̓��Z�b�g���ׂ�? '00/09/09
	//	�˂������A�������Ă��܂��ƁA��ʂ�؂�ւ������ƁAFillColor���ς���Ă��܂��D�D

//	m_bOwnerDraw	= true;

	return 0;
}

//	�v���C�}���T�[�t�F�[�X�̐���
LRESULT	CFastPlane::CreatePrimary(bool& bUseFlip,int nSx,int nSy){
	m_bPrimary = true;

	Release();
	ResetColorKey();
	LPDIRECTDRAW lpDraw = GetMyFastDraw()->GetDDraw();
	if (lpDraw==NULL) return 1;
	/* if (!IWindow::IsFullScreen()) */
	bUseFlip = false;

sur_retry: ;
	DDSURFACEDESC ddsd;
	ZERO(ddsd);
	ddsd.dwSize = sizeof(ddsd);

	if (bUseFlip) {
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE
			| DDSCAPS_FLIP
			| DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;
	} else {
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	}
	if (lpDraw->CreateSurface(&ddsd,&m_lpSurface,NULL)!=DD_OK){
		Err.Out("CFastPlane::CreatePrimary�Ɏ��s");
		if (bUseFlip) {
			bUseFlip=false;
			// Flipping Surface�̓r�f�I��������ɔz�u����K�v�����邽�߁A
			// Create�Ƀ~�X�����Ƃ��l������
			goto sur_retry;
		}
		return 1; // ������[
	}
	//	�����ŃR�s�[���Ă���
	if (nSx && nSy) {
		m_nSizeX = nSx; m_nSizeY = nSy;	//	�T�C�Y�͂�����̗p
	} else {
		GetMyFastDraw()->GetSize(m_nSizeX,m_nSizeY);
	}

	UpdateSurfaceInfo();

	if (GetSurfaceType() == 2) {
		//	8bpp�������A����Z�J���_��256���l�A
		//	256�F���[�h�₯�ǁARGB555�ł͖����A8bpp�Ƃ��ėp�ӂ��ꂽ
		//	����ȃT�[�t�F�[�X�Ƃ��Đ\������
		m_bSecondary256 = true;
	}

	//	���̃T�[�t�F�[�X�ɑ΂��Ă��A�J���[�L�[�͐ݒ肵�Ƃ����D�D
	UpdateColorKey();

//	m_nSurfaceRef = 1;		//	�Q�ƃJ�E���g�����Ă����Ȃ��Ƃ��܂��������Ȃ�
	m_bOwnerDraw = true;	//	�����On�ɂ��Ȃ���Restore����Ă��܂�
	return 0;
}

//	�Z�J���_���T�[�t�F�[�X�̐���
LRESULT CFastPlane::CreateSecondary(CFastPlane*lpPrimary,bool& bUseFlip){
	Release();
	ResetColorKey();

	LPDIRECTDRAW lpDraw = GetMyFastDraw()->GetDDraw();
	if (lpDraw==NULL) return 1;

	//	��flip���[�`�����������I���T�|�[�g�I
sur_retry: ;
	if (bUseFlip) {
		// flip���g���ȏ�A�V�X�e���������Ƀo�b�t�@���m�ۂ���̂͂܂����D�D�D
		DDSCAPS ddscaps;		
		ZERO(ddscaps);	//	����Ȃ����ǈꉞ��
		if (m_bUseSystemMemory) {
			ddscaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_SYSTEMMEMORY;
		} else {
			ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		}
		if (lpPrimary->GetSurface()->GetAttachedSurface(&ddscaps,&m_lpSurface)!=DD_OK){
			Err.Out("CFastPlane::CreateSecondary��GetAttachedSurface�Ɏ��s");
			// �ق₩�猾��񂱂�����Ȃ��I
			bUseFlip = false;
			goto sur_retry;
		}
		lpPrimary->GetSize(m_nSizeX,m_nSizeY);
		//	�ꉞ�N���A���Ƃ���...
		Clear();
		//	�T�[�t�F�[�X�̃`�F�b�N���X�V
		UpdateSurfaceInfo();
	} else {
		int nSizeX,nSizeY;
		lpPrimary->GetSize(nSizeX,nSizeY);
		if (InnerCreateSurface(nSizeX,nSizeY)!=0){
			Err.Out("CFastPlane::CreateSecondary�Ɏ��s");
			return 0;
		}
		//	Update��Clear�́AInnerCreateSurface���ʓ|������
	}

	//	���̃T�[�t�F�[�X�ɑ΂��Ă��A�J���[�L�[�͐ݒ肵�Ƃ����D�D
	UpdateColorKey();

//	m_nSurfaceRef = 1;		//	�Q�ƃJ�E���g�����Ă����Ȃ��Ƃ��܂��������Ȃ�
	m_bOwnerDraw = true;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//	�V�����T�[�t�F�[�X��������Ƃ��ɂ́A���̊֐����Ăяo�����ƁI
LRESULT	CFastPlane::UpdateSurfaceInfo(){
	int nSurfaceType = CDirectDrawSurfaceManager().GetSurfaceType(GetSurface());

	//	���ۂ�Lock��Ƃ��K�v�ł���
	DDSURFACEDESC ddsd = { sizeof (ddsd) };
	LRESULT hres;
	ddsd.dwSize = sizeof(ddsd);
	while ((hres = GetSurface()->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING)
		;
	SIZE rc = { m_nSizeX,m_nSizeY };

	if (hres != DD_OK) {
		if (IsPrimarySurface()){
			//	primary�����m���
			GetSurfaceInfo()->Init(NULL,0,rc,nSurfaceType);
			///	primary�́ALock�Ɏ��s���邱�Ƃ����邩�珉���������ł����Ƃ��ɂႢ����
			GetSurfaceInfo()->SetLocker(smart_ptr<ISurfaceLocker>(new IDisableSurfaceLocker));
			///	Surface��Lock�͕K�����s����悤�ɂ��Ƃ��Ȃ��Ă͂�����
		} else {
			//	primary�łȂ��̂�lock���s������_����
			GetSurfaceInfo()->SetInit(false);
		}
		return 1;	//	Lock�Ɏ��s
	}
	GetSurfaceInfo()->Init(ddsd.lpSurface,ddsd.lPitch,rc,nSurfaceType);
	//	���A�h���X����肷�邽�߂ɁA���ꂪ�K�v

	//	unlock��Y���Ƒ�ςȂ��ƂɁO�O�G
	GetSurface()->Unlock(NULL);

	return 0;
}

LPDIRECTDRAWSURFACE CFastPlane::GetSurface(){
	return m_lpSurface;
}

LPDIRECTDRAWPALETTE CFastPlane::GetPalette(){
	return m_lpPalette;
}


LRESULT CFastPlane::SetSystemMemoryUse(bool bEnable){
	m_bUseSystemMemory = bEnable;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

/*
LRESULT		CFastPlane::SetFillColor(ISurfaceRGB c){
	if (m_lpSurface==NULL) return -1;
	m_FillColor = c;
	return GetSurfaceInfo()->GetMatchColor(c,m_dwFillColor);
}

ISurfaceRGB		CFastPlane::GetFillColor() const {
	return m_FillColor;
}
*/

//////////////////////////////////////////////////////////////////////////////
// todo
// �f�B�X�v���C�̐F���𒲂ׂ�̂�GetDisplayMode�͎g���Ă͂����Ȃ�
/*
int		CFastPlane::GetBpp(){
	return CBppManager::GetBpp();
}

void	CFastPlane::InnerGetBpp() {
	CBppManager::Reset();
}
*/

//////////////////////////////////////////////////////////////////////////////

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif // USE_DirectDraw
