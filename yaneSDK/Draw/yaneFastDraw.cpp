//
//	DirectDrawSurface and MemorySurface Wrapper
//

#include "stdafx.h"

#ifdef USE_FastDraw

#include "yaneDirectDraw.h"
#include "yaneFastDraw.h"
#include "yaneFastPlane.h"
#include "yaneGTL.h"
#include "../Window/yaneWindow.h"
#include "../AppFrame/yaneAppManager.h"	//	GetHWnd
#include "../AppFrame/yaneAppBase.h"

//////////////////////////////////////////////////////////////////////////////
//	CFastDraw : ��ʉ𑜓x��؂�ւ���Ȃ�
//////////////////////////////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Draw {

CFastDraw::CFastDraw() :
	m_DirectDrawBase(true),	//	DirectDraw emulation only
	//	���ƁADirectDraw��Emulation Only�̂ق��������I	
	m_Primary(GetThis()),
	m_Secondary256(GetThis()),
	m_Secondary(GetThis())
{
	CFastPlane::SetDefaultFastDraw(this);

//	m_nScreenXSize	= 640;
//	m_nScreenYSize	= 480;
	//	����́A��������Ă���E�B���h�D�T�C�Y�ɏ�����ق��������񂿂����H
	IWindow* pWin = CAppManager::GetMyWindow();
	CWindowOption* opt;
	if (pWin!=NULL){
		opt = pWin->GetWindowOption();
	} else {
		opt = NULL;
	}
	if (opt==NULL){
		m_nScreenXSize	= 640;
		m_nScreenYSize	= 480;
	} else {
		m_nScreenXSize = opt->size_x;
		m_nScreenYSize = opt->size_y;
	}

	m_nScreenColor	= 16;
	m_bFullScr		= false;

	//	FullScr���̃t���b�v�̎g�p�B�g������̂Ȃ�Ύg�����ق����ǂ�
//	m_bUseFlip		= false;

	{
	RECT rc,rc2;
	::SetRect(&rc,0,0,0,0);
	::SetRect(&rc2,0,0,0,0);
	::AdjustWindowRect(&rc,WS_CAPTION | WS_SYSMENU,true);
	::AdjustWindowRect(&rc2,WS_CAPTION ,false);
	//	�t���X�N���[�����̃��j���[�T�C�Y�𐳊m�ɎZ�o���邽�߁A���j���[�����𒲂ׂ�(c)yaneurao'00/12/15
	//	�i�Ȃ�ƃ��j���[�����͂n�r���ƂɈقȂ�O�O�G �j
	m_nMenu = (rc.bottom-rc.top) - (rc2.bottom-rc2.top);
	}

	m_nSecondaryOffsetX = 0;
	m_nSecondaryOffsetY = 0;

	m_nBrightness	= 256;
	m_bLostSurface	= false; 

	m_hWnd	= CAppManager::GetHWnd();
	CAppManager::Add(this);
	CAppManager::Hook(this);		//	�t�b�N���J�n����

	//	�v���C�}���̓n�[�h�E�F�A��������Ɋm��
	GetPrimary()->SetSystemMemoryUse(false);
//	GetPrimary2()->SetSystemMemoryUse(false);
	//	�����ȊO�́A���ׂăV�X�e����������Ɋm�ۂ���
	//	CFastPlane�́A�f�B�t�H���g�ŃV�X�e���������Ȃ̂ŁA����łn�j

	//	�Z�J���_���Ɋւ��ẮA�������X�g�A�T�[�t�F�[�X�Ƃ��Ă���
	//	�i�ǂ����AOnDraw�̓I�[�o�[���C�h����Ă��Ȃ��̂Łj
//	GetPrimary2()->SetAutoRestore(true);
	GetSecondary()->SetAutoRestore(true);

	// yaneGTL.h�Ŏg�p����A�u�����h�e�[�u����������
	CFastPlaneBlendTable::InitTable();	
}

CFastDraw::~CFastDraw(){
	CAppManager::Unhook(this);		//	�t�b�N����������B
	CAppManager::Del(this);
}

//////////////////////////////////////////////////////////////////////////////

int		CFastDraw::GetMenuHeight(){
//	���j���[�����Ă���Ȃ�΁A���̃��j���[������Ԃ�
//	�i���Ă��Ȃ���΂O�j
	if (::GetMenu(m_hWnd)!=NULL) return m_nMenu;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

void CFastDraw::BeginChangeDisplay(){
	GetCriticalSection()->Enter();	//	�r�����b�N���Ă�����

	m_bDisplayChanging = true;
	m_bChangeDisplayMode = false;
	if (GetDDraw()==NULL) return ;

	// FullScreenMode��Flip�p�ɃA�^�b�`���Ă���A�������K�v�Ȃ�����...
	//	�Z�J���_���A�N���b�p�A�v���C�}���̉��
 
	m_Secondary256.Release();
//	m_Secondary.Release();
	//	�����̃Z�J���_���́A�V�X�e����������ɍ쐬���ꂽ
	//	���z�Z�J���_���T�[�t�F�[�X�Ȃ̂ŉ������K�v������
	//	�i�ނ���A������Ȃ����Ƃɂ���āAAutoRestore�̑ΏۂƂ������j

	m_WindowClipper.Release();
//	m_Primary2.Release();
	m_Primary.Release();

	if (m_bFullScr) {	//	�����t���X�N���[�����[�h�Ȃ�΂����߂��K�v������
		if (GetDDraw()->SetCooperativeLevel(m_hWnd,DDSCL_NORMAL)){
			Err.Out("CFastDraw::BeginChangeDisplay��SetCooperativeLevel�Ɏ��s..");
		}
		// �E�B���h�D���[�h�ɂ���Ƃ��́A������s�Ȃ��K�v����
		if (GetDDraw()->RestoreDisplayMode()){
			Err.Out("CFastDraw::BeginChangeDisplay��RestoreDisplayMode�Ɏ��s..");
		}
	}
}

void CFastDraw::TestDisplayMode(int nSX,int nSY,bool bFullScr,int nColor){
	if (!m_bDisplayChanging) return ; // already changing!!
	if (GetDDraw()==NULL) return ;

	// �E�C���h�E���[�h�Ńf�B�X�v���C���[�h��ύX���Ȃ���
	m_bChangeDisplayMode |= (m_bFullScr!=bFullScr) || m_bFullScr;// �ǉ�
	if (m_bChangeDisplayMode) CAppManager::GetMyWindow()->SetResized(true);

	//	��ʃ��[�h�𔽉f������...
	m_bFullScr	= bFullScr;

	if (!bFullScr) {
		// Window���[�h�̎w�肪����Ȃ�Ύd�����Ȃ��B
		if (GetDDraw()->SetCooperativeLevel(m_hWnd,DDSCL_NORMAL)){
			Err.Out("CFastDraw::TestDisplayMode��SetCooperativeLevel��DDSC_NORMAL�Ŏ��s");
			return ; // ����ł����񂩂�����͂����茾���ăV�����Ȃ��̂₯��...
		}
	} else {
		if (GetDDraw()->SetCooperativeLevel(m_hWnd,DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN)!=DD_OK){
			Err.Out("CFastDraw::TestDisplayMode��SetCooperativeLevel��DDSC_EXCLUSIVE�Ŏ��s");
			return ; // ���̃A�v���g���Ă񂿂Ⴄ��H
		}
		if (GetDDraw()->SetDisplayMode(nSX,nSY,nColor)!=DD_OK){
			Err.Out("CFastDraw::TestDisplayMode��SetDisplayMode�Ɏ��s");
			return ; // ��Ή����H
		}
	}

	//	m_bUseFlip	: �t���b�v���g�p����̂��H�i���[�U�[�̈ӎv	�j
	//	m_bUseFlip2 : �t���b�v�����ۂɃn�[�h�E�F�A�I�Ɏg���āA
	//				�����Ďg���Ƃ���true

//	m_bUseFlip2 = m_bUseFlip;

	bool bUseFilp = false;
	//	�v���C�}���T�[�t�F�[�X�̍쐬
	if (m_Primary.CreatePrimary(bUseFilp,nSX,nSY)!=0) {
		Err.Out("CFastDraw::TestDisplayMode��CreatePrimary�Ɏ��s");
		return ;
	}

	//	�E�B���h�D���[�h�Ȃ�΃N���b�p���d�|����
	m_WindowClipper.Release();
	if (!bFullScr){
		if (m_WindowClipper.SetClipper(GetDDraw(),m_Primary.GetSurface(),m_hWnd)!=0) {
			Err.Out("CFastDraw::TestDisplayMode��Clip�Ɏ��s");
			return ;
		}
	}

/*
	if (m_bUseFlip2){
		if (m_Primary2.CreatePrimary2(&m_Primary,m_bUseFlip2)!=0){
			//	���ꎸ�s���Ă���A�t���b�v�g�p���Ȃ��A�Ƃ���
			m_bUseFlip2 = false;
		}
	}
*/

	bool b256 = m_Primary.GetSurfaceType()==2;
	if (b256) {
		m_Secondary.m_bSecondary256DIB = true;
		//	RGB555��DIB�ɂ��Ă��炤�ŁI
	}

	//	�Z�J���_���T�[�t�F�[�X�̍쐬
	if (m_Secondary.CreateSecondary(&m_Primary,bUseFilp)!=0) {
		Err.Out("CFastDraw::TestDisplayMode��CreateSecondary�Ɏ��s");
		return ;
	}

/*
	// 256�łȂ��Ȃ�΃p���b�g�̐ݒ�͖��Ӗ�
	if (b256) InstallPalette();
*/

	//	�����A�t���X�N���[�����[�h��256�F�ł��A�t���b�v���g�p����Ȃ�΁A
	//	Secondary256�͕s�v�Ȃ̂����D�D�D
	if (b256) {
		//	256�ϊ��p�T�[�t�F�[�X
		m_Secondary256.InnerCreateSurface(nSX,nSY,false,true);
	}

	//	�����ŃE�B���h�D���[�h�𔽉f���Ă���
	CAppManager::GetMyWindow()->ChangeScreen(bFullScr);
	CAppManager::GetMyWindow()->ChangeWindowStyle();

	m_bDisplayChanging	= false;	//	��ʃ��[�h�̕ύX�ɐ����I
	m_nScreenColor	= nColor;
	m_nScreenXSize	= nSX;
	m_nScreenYSize	= nSY;
	return ;
}

LRESULT CFastDraw::EndChangeDisplay(){
	struct tmpclass {
		tmpclass(CCriticalSection*c):c_(c){}
		~tmpclass() { c_->Leave();}
		CCriticalSection*c_;
	};
	tmpclass t(GetCriticalSection());
	//	������Ƃ���BeginChangeDisplay�ł������r�����b�N���������悤

	if (GetDDraw()==NULL) return 1;
	
	if (m_bDisplayChanging) { // Change����Ă��Ȃ��I�I
		Err.Out("CFastDraw::BeginChangeDisplay�`EndChangeDisplay�Ɏ��s");
		return 1;
	}

	// �E�C���h�E���[�h�ŁA�f�B�X�v���C���[�h���ς���ĂȂ��Ȃ�҂��Ȃ�
	if(m_bChangeDisplayMode) {// �ǉ�
		::Sleep(500);	//	�r�f�I���[�h�̕ύX�̗]�g���󂯂�\��������
	}

/*
	// �p���b�g�̍č\�z
	m_bBrightnessChanged = true;
*/

	// ���O�ŕ`�悷��Ȃ�Ί֌W�Ȃ��̂���...
	CAppManager::GetMyWindow()->ShowCursor(!m_bFullScr);
	// �E�B���h�D�T�C�Y�̕ύX
	CAppManager::GetMyWindow()->SetSize(m_nScreenXSize,m_nScreenYSize);
	// �E�B���h�D�X�^�C���̕ύX

	CAppManager::GetMyWindow()->ChangeScreen(m_bFullScr);
	//	�����̏����E�B���h�D�ɂ͍��m���Ă��B
	//	�i�{���́A���ׂẴE�B���h�E�Ƀu���[�h�L���X�g����K�v������j
	CAppManager::GetMyWindow()->ChangeWindowStyle();

	// �v���[���̍č\�z�i�����������Ă���΁j
	RestoreAll();

	//	�i�p���b�g�̃��A���C�Y��Ɂj
	// yaneGTL.h�Ŏg�p����ARGB555��256�̕ϊ��e�[�u���̏�����
	//	(�������A256�F���[�h�ȊO�̂Ƃ��́A���������ɖ߂��Ă���)
	CFastPlaneBlendTable::OnChangePalette();	

	return 0;
}

/////// �g�p��:p) ////////////////////////////////////////////////////////////

LRESULT CFastDraw::ChangeDisplay(bool bFullScr){
	if (GetDDraw()==NULL) return 1; //	���߂������...
	BeginChangeDisplay();
		if (bFullScr) {
			TestDisplayMode(m_nScreenXSize,m_nScreenYSize,true,m_nScreenColor);
		}
		TestDisplayMode(m_nScreenXSize,m_nScreenYSize); // �E�B���h�D���[�h
	if (EndChangeDisplay()) return 2;	//	�f�B�X�v���C���[�h�ύX�Ɏ��s
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//	�f�B�X�v���C���[�h�̐ݒ聨�ύX

LRESULT		CFastDraw::SetDisplay(bool bFullScr,int nSizeX,int nSizeY,int nColorDepth){
	if (nSizeX!=0) {
		m_nScreenXSize	=	nSizeX;
	}
	if (nSizeY!=0) {
		m_nScreenYSize	=	nSizeY;
	}
	if (nColorDepth!=0) {
		m_nScreenColor	=	nColorDepth;
	} else {
		m_nScreenColor	=	16; // fixed '01/04/08
	}
	ChangeDisplay(bFullScr);
	//	���̃p�����[�^�����͂����ɓn��
	//	�im_bFullScr�͌��݂̉�ʃ��[�h��ێ����Ă���̂Łj
	return 0;
}

void		CFastDraw::GetDisplay(bool&bFullScr,int &nSizeX,int &nSizeY,int &nColorDepth){
	bFullScr		=	m_bFullScr;
	nSizeX			=	m_nScreenXSize;
	nSizeY			=	m_nScreenYSize;
	nColorDepth		=	m_nScreenColor;
}

void		CFastDraw::GetSize(int &nSizeX,int &nSizeY){	// GetDisplayMode��x,y�����������
	nSizeX			=	m_nScreenXSize;
	nSizeY			=	m_nScreenYSize;
}
	
bool	CFastDraw::IsFullScreen(){
	return		m_bFullScr;
}

void	CFastDraw::CheckSurfaceLost(){
	LPDIRECTDRAWSURFACE lpSurface = GetPrimary()->GetSurface();
	//	�V�X�e����������̃T�[�t�F�[�X��Lost���Ȃ��̂ł���O�O�G
	if (lpSurface==NULL) return ;
	if (lpSurface->IsLost()){
		ChangeDisplay(m_bFullScr);
		//	�v���C�}���T�[�t�F�[�X�̍č\�z����s�Ȃ��ׂ�
	}
}

/*
int		CFastDraw::GetBpp(){	// ���݂�Bpp�̎擾
	return GetPrimary()->GetBpp();
}
*/

void	CFastDraw::SetBrightness(int nBright){
	if (m_nBrightness == nBright) return ;
	m_nBrightness = nBright;
}

//////////////////////////////////////////////////////////////////////////////

void	CFastDraw::SetOffset(int ox,int oy){
	//	�Z�J���_���̓]���I�t�Z�b�g
	m_nSecondaryOffsetX =	ox;
	m_nSecondaryOffsetY =	oy;
}

void	CFastDraw::OnDraw(LPCRECT lpRect,bool bLayerCallBack){

	//	�ŏ�������Ă���Ȃ�Ε`�悹���Ƀ��^�[��
	if (CAppManager::GetMyWindow()->IsMinimized()) return ;

	//	Menu��Popup����
	int nMenu = 0;
	//	FullScr�ŁA���j���[����ŁA�}�E�X����ʏ�̃��j���[�̈�ɂ������������̂Ȃ��...
	if (IsFullScreen() && ::GetActiveWindow()==m_hWnd) {
	//						������Ƀ��C���E�C���h�E���A�N�e�B�u(�_�C�A���O�Ȃǂ��Ăяo����ĂȂ�)
		POINT point;
		::GetCursorPos(&point);
		if (point.y<m_nMenu) {
			if (::GetMenu(m_hWnd)!=NULL){
//				�����̃��j���[�T�C�Y�𐳊m�ɎZ�o���邽�߁A���j���[�����𒲂ׂ�(c)yaneurao'00/12/15
				nMenu = m_nMenu;
				//	Paint���Ȃ������[
//				::SetMenu(m_hWnd,GetMenu(m_hWnd));	//	���j���[�̈�̍ĕ`��
				::DrawMenuBar(m_hWnd);
			}
		}
	}
	
	//	�����A�T�[�t�F�[�X���������Ă����...
	if (m_bLostSurface) {
		//	�X���b�h�������j�������̂Ȃ�΃��X�g�A�͖���...
		if (!CAppManager::GetMyApp()->IsThreadValid()) return ;
		CheckSurfaceLost();
		//	���̃t���O�́A���b�Z�[�W�|���v����
		//	���X�g�A�C�x���g�ɉ����Đݒ肳���
		m_bLostSurface = false;
		return ;
	}
	
	LPDIRECTDRAWSURFACE lpPrimary = GetPrimary()->GetSurface();
	if (lpPrimary==NULL) return ;

	//////////////////////////////////////////////////////////
	//	Layer callback
	if ( bLayerCallBack ){
		//	Layer���Z�J���_���ɕ`�悵�Ă��炤�O�O
		m_LayerList.OnDraw(GetSecondary());
		
		//	HDCLayer���Z�J���_���ɕ`�悵�Ă��炤�O�O
		if (!m_HDCLayerList.IsEmpty()){
			HDC hdc = GetSecondary()->GetDC();
			if (hdc!=NULL){
				m_HDCLayerList.OnDraw(hdc);
				GetSecondary()->ReleaseDC();
			}
//			Invalidate();	//	��������������ŁA�����t���O���Ă�
		}
		
		//	AfterLayer���Z�J���_���ɕ`�悵�Ă��炤�O�O
		m_AfterLayerList.OnDraw(GetSecondary());

		//	�u���C�g�l�X�ɉ����āA�Z�J���_��������
		RealizeBrightness(m_nBrightness);
	}
	//////////////////////////////////////////////////////////
	
	//	�v���C�}��������Ă��Ȃ���΃��^�[��
	//	if (!m_bDirty) return ;
	//	DirtyRect�Ǘ��͍s�Ȃ��Ă��Ȃ�
	//	�ǂ���FullScreen��Flip���g���ƍŏ��X�V�����ł͊Ǘ��ł��Ȃ�
	
	LPDIRECTDRAWSURFACE lpSecondary;
	CFastPlane* lpSecondarySurface;
	if (m_Primary.GetSurfaceType()==2){	//	8bpp��...
		//	Secondary�́A����RGB555�Ȃ̂œ]�����Ă��K�v����
		GetSecondary256()->BltFast(GetSecondary(),0,0);
		lpSecondary = GetSecondary256()->GetSurface();
		lpSecondarySurface = GetSecondary256();
	} else {
		lpSecondary = GetSecondary()->GetSurface();
		lpSecondarySurface = GetSecondary();
	}
	if (lpSecondary==NULL) return ;

	RECT r; // source rect
	::SetRect(&r,0,0,m_nScreenXSize,m_nScreenYSize);
	
	if (m_nSecondaryOffsetX || m_nSecondaryOffsetY) {
		//	�]����I�t�Z�b�g���������Ă���̂��H
		RECT r2; // distination rect
		if ( lpRect == NULL ){	// �����X�V���H
			::SetRect(&r2,0,0,m_nScreenXSize,m_nScreenYSize);	//	����T�C�Y�Ɖ��肵�ėǂ�
			//	a clipping algorithm
			//		between the same size rect (C) yaneurao'1999
			if (m_nSecondaryOffsetX>0) {
				r.right		-= m_nSecondaryOffsetX;
				r2.left		+= m_nSecondaryOffsetX;
			} else {
				r.left		-= m_nSecondaryOffsetX;
				r2.right	+= m_nSecondaryOffsetX;
			}
			if (m_nSecondaryOffsetY>0) {
				r.bottom	-= m_nSecondaryOffsetY;
				r2.top		+= m_nSecondaryOffsetY;
			} else {
				r.top		-= m_nSecondaryOffsetY;
				r2.bottom	+= m_nSecondaryOffsetY;
			}
			
			if (CWindow::IsFullScreen()){
				// �t���X�N���[���ł��ABlt�Ȃ̂�I
				DDBLTFX fx;
				ZERO(fx);	//	�ꉞ������
				fx.dwSize = sizeof(fx);
				fx.dwFillColor = lpSecondarySurface->GetFillColor();
				RECT br;
				::SetRect(&br,0,nMenu,m_nScreenXSize,r2.top);					//	��̈�
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				//	main�摜
				lpPrimary->BltFast(r2.left,r2.top,lpSecondary,&r,DDBLTFAST_WAIT);
				::SetRect(&br,0,r2.top,r2.left,r2.bottom);				//	���̈�
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				::SetRect(&br,r2.right,r2.top,m_nScreenXSize,r2.bottom);	//	�E�̈�
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				::SetRect(&br,0,r2.bottom,m_nScreenXSize,m_nScreenYSize);	//	���̈�
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				
			} else {
				// DirectDrawClipper�𗘗p����ɂ�Blt�ɂ��Ȃ��Ă͂Ȃ�Ȃ��B
				// �������A�Ȃ񂿂�[������B���������ɂ��ė~����>Clipper�N�I
				POINT cp;
				cp.x = cp.y = 0;
				::ClientToScreen(m_hWnd,&cp); // �N���C�A���g�E�B���h�D�̋�`������I
				
				DDBLTFX fx;
				ZERO(fx);	//	�ꉞ������
				fx.dwSize = sizeof(fx);
				fx.dwFillColor = lpSecondarySurface->GetFillColor();
				
				RECT br;
				::SetRect(&br,0,0,m_nScreenXSize,r2.top);					//	��̈�
				::OffsetRect(&br,cp.x,cp.y);
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				
				//	main�摜
				br = r2;
				::OffsetRect(&br,cp.x,cp.y);
				lpPrimary->Blt(&br,lpSecondary,&r,DDBLT_WAIT,NULL);
				
				::SetRect(&br,0,r2.top,r2.left,r2.bottom);					//	���̈�
				::OffsetRect(&br,cp.x,cp.y);
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				::SetRect(&br,r2.right,r2.top,m_nScreenXSize,r2.bottom);	//	�E�̈�
				::OffsetRect(&br,cp.x,cp.y);
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
				::SetRect(&br,0,r2.bottom,m_nScreenXSize,m_nScreenYSize);	//	���̈�
				::OffsetRect(&br,cp.x,cp.y);
				lpPrimary->Blt(&br,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&fx);
			}
		}else{
			// �����X�V
				r = *lpRect;	// source rest;
				r2 = *lpRect;	// dest	 rest;
			if (CWindow::IsFullScreen()){
				// from CPlane DRAW_CLIPPER
				RECT& sr = r;			
				int x = m_nSecondaryOffsetX;
				int y = m_nSecondaryOffsetY;
				{
					RECT clip;
					::SetRect(&clip,0,0,m_nScreenXSize,m_nScreenYSize);
					int x2,y2;													
					x2 = x + sr.right - sr.left; /* x + Width  */				
					y2 = y + sr.bottom - sr.top; /* y + Height */				
					if (x2<clip.left || y2<clip.top								
						|| x>=clip.right || y>=clip.bottom) return;	 /* ��ʊO */ 
					int t;														
					t = clip.left-x;										
					if (t>0)	{ sr.left	+= t;	x = clip.left;	}		
					t = clip.top -y;										
					if (t>0)	{ sr.top	+= t;	y = clip.top;	}		
					t = x2-clip.right;										
					if (t>0)	{ sr.right	-= t;	x2= clip.right; }		
					t = y2-clip.bottom;										
					if (t>0)	{ sr.bottom -= t;	y2= clip.bottom;}		
					if (sr.right<=sr.left || sr.bottom<=sr.top) return; // invalid rect
				}
				lpPrimary->BltFast(x,y,lpSecondary,&r,DDBLTFAST_WAIT);
			}else{
				// �I�t�Z�b�g�]���̕����X�V�Ȃ̂ŁA���ꂽ���w�i�͕`�悵�Ȃ�
				// DirectDrawClipper�𗘗p����ɂ�Blt�ɂ��Ȃ��Ă͂Ȃ�Ȃ��B
				// �������A�Ȃ񂿂�[������B���������ɂ��ė~����>Clipper�N�I
				POINT cp;
				cp.x = cp.y = 0;
				::ClientToScreen(m_hWnd,&cp); // �N���C�A���g�E�B���h�D�̋�`������I
				::OffsetRect(&r2,m_nSecondaryOffsetX,m_nSecondaryOffsetY);
				::OffsetRect(&r2,cp.x,cp.y); 
				lpPrimary->Blt(&r2,lpSecondary,&r,DDBLT_WAIT,NULL);
			}
			
		}
		
	} else {
		//	�]���I�t�Z�b�g���w�肳��Ă��Ȃ��t�c�[�̏ꍇ
		
		if (CWindow::IsFullScreen()){
			if (/*m_bUseFlip2*/ false && lpRect == NULL ){
				lpPrimary->Flip(NULL,DDFLIP_WAIT);
			} else {
				if ( lpRect != NULL ){
					r = *lpRect;
				}
				//	�����A���j���[�\���Ή��I(yane'02/01/13)
				if ( nMenu && nMenu > r.top ) {
					r.top = nMenu;
				}
				lpPrimary->BltFast(r.left,r.top,lpSecondary,&r,DDBLTFAST_WAIT);
			}
		} else {
			// DirectDrawClipper�𗘗p����ɂ�Blt�ɂ��Ȃ��Ă͂Ȃ�Ȃ��B
			// �������A�Ȃ񂿂�[������B���������ɂ��ė~����>Clipper�N�I
			POINT cp;
			cp.x = cp.y = 0;
			::ClientToScreen(m_hWnd,&cp); // �N���C�A���g�E�B���h�D�̋�`������I
			if ( lpRect != NULL ) {
				r = *lpRect;
			}
			RECT sr = r;
			::OffsetRect(&sr,cp.x,cp.y);
			lpPrimary->Blt(&sr,lpSecondary,&r,DDBLT_WAIT,NULL);
		}
	}
	
//	m_bDirty = false;
}

//////////////////////////////////////////////////////////////////////////////
void	CFastDraw::RealizeBrightness(int nBrightness){
	if (m_nBrightness==256) return ;
	//	���ꂾ���ł����񂩁O�O�G�@�������O�O�G
	GetSecondary()->FadeEffect(nBrightness);
}

//////////////////////////////////////////////////////////////////////////////

void	CFastDraw::RestoreAll(){ // �S�v���[���̃����[�h
	GetFastPlaneList()->for_each(&CFastPlane::Restore);
	return ;
}

//////////////////////////////////////////////////////////////////////////////
LRESULT		CFastDraw::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){

	switch(uMsg){
	case WM_PAINT : {
		//	dirty rect������
//		m_bDirty = true;
//		if (!CWindow::IsFullScreen()) OnDraw();	//	�ꉞ�A�ĕ`�悵�Ă܂��B
//		OnDraw();
		/*
			����O���Ă����̂����A�����R�����g���O���Ƃ��Ă�
			WM_PAINT�̏������ɂ�GetDC�͎g���Ȃ��̂Œ��ӂ��K�v�B
			�iGetDC�Ŏ擾�����f�o�C�X�R���e�L�X�g�ɕ`�悷���WM_PAINT���������邩��j
			WM_PAINT�̏�������WM_PAINT�����������ŁA�A�v���P�[�V������WM_PAINT��
			���������ł��Ȃ��Ȃ�I
		*/
		return 0;	//	DefWindowProc���Ăяo���ɍs���B
					//	������Ăяo���Ă����Ȃ��ƍēxWM_PAINT�������Ă���
					}

	case WM_ACTIVATEAPP: {
		UINT bActive = wParam;
		if(bActive) {
			//	���b�Z�[�W�|���v���ł͂Ȃ��A���[�J�[�X���b�h���Ń��X�g�A���ׂ�...
			m_bLostSurface = true;
		}
		return 0;
						 }

	case WM_PALETTECHANGED: {
		//	�i�p���b�g�̃��A���C�Y��Ɂj
		// yaneGTL.h�Ŏg�p����ARGB555��256�̕ϊ��e�[�u���̏�����
		CFastPlaneBlendTable::OnChangePalette();	
		return 0;
							}

	} // end switch


	return 0;
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif // USE_FastDraw
