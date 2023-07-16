
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

	//	システムメモリ上に確保するのだ！
	m_bUseSystemMemory	=	true;

	m_bYGA			= false;
	m_bYGAUse		= false;
	m_bMySurface	= false;
	m_bSecondary256 = false;
	//	256色モードならば、通常RGB555のサーフェースを作成するのだが、
	//	セカンダリだけは256色サーフェース
	m_bLoad256		= false;
	m_bSecondary256DIB=false;

	//	RGB555のMySurfaceに対して、DIB Sectionを使用するのか？
	m_hBitmap		= NULL;
	m_hDC			= NULL;

	//	自動修復サーフェース
	m_bAutoRestore = false;
	//	オーナードロー(Restoreが呼び出されない。Primary,Secondaryはこれ)
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
	//	256色モードの仮想セカンダリはDIBとして作成してたんか？
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
			Err.Out("CFastPlane::Releaseで不明サーフェースのリリース");
		}
		m_bMySurface = false;
	}

	RELEASE_SAFE(m_lpSurface);
	RELEASE_SAFE(m_lpPalette);
	m_strBitmapFile.erase();
	m_nSizeX	=	0;
	m_nSizeY	=	0;
	m_bLoad256	= false;
	GetSurfaceInfo()->SetInit(false);	//	サーフェース情報も初期化

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

//	サーフェースのロストに対する復帰処理
LRESULT	CFastPlane::Restore(){
	LRESULT lr = 0;
	m_bNowRestoring = true;	//	リストア中につき、Releaseでフック解除するの禁止！
	int nType = GetSurfaceInfo()->GetSurfaceType();
	if (nType!=0){	//	Surfaceが存在するかどうかは、こいつで判定する必要がある
		//	システムメモリ上のサーフェースはLostしないのだが＾＾；
		//	現在の画面bppと異なるサーフェースならば、解体して作りなおす必要がある
		bool bRestore = false;
		if (m_lpSurface!=NULL && m_lpSurface->IsLost()) {
			//	&& 論理演算子は、左から右への評価を保証するし、
			//	VCの最適化により、左が成立時に右が評価されないのも保証される
			bRestore = true;
		} else {
		//	現在の画面bppとコンパチで無いサーフェースならばリストアする
			int nType2 = GetMyFastDraw()->GetPrimary()->GetSurfaceType();
			switch (nType2){
			case 2:	//	256色のときは、RGB555とみなす
				if (nType==2 && m_bSecondary256) break; // 256色用セカンダリやん？
				if (nType==4 || nType == 11) break;
				bRestore = true; break;
			case 3:
				if (nType==3 || nType == 10) break;
				bRestore = true; break;
			case 4:
				//	256色モードの時に読み込んだビットマップなんか信用できない
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
			//	自動修復サーフェースならば、サーフェースを作成後、そいつをコピー
				LRESULT lrErr = 0;
				if (m_nSizeX!=0 && m_nSizeY!=0){
					CFastPlane plane;
					plane.CreateSurface(m_nSizeX,m_nSizeY,IsYGA());
					lrErr = plane.BltFast(this,0,0);
					// この変換可能やったか？
					if (lrErr==0){
						InnerCreateSurface(m_nSizeX,m_nSizeY,m_bYGA,m_bSecondary256);
						lrErr = BltFast(&plane,0,0);
					}
				}
				//	転送に失敗したら、しゃーないから自前でリストアしてんか．．
				if (lrErr!=0) goto RestoreRetry;

				//	現在の画面モードに合わせてColorKeyを設定しなおす
				UpdateColorKey();
			} else {
RestoreRetry:;
				//	ビットマップファイルならばそれを復元する
				if (IsLoaded()){
					lr = InnerLoad(m_strBitmapFile);
				} else {
					//	本当は、このとき、変換子を用意して変換ほうが良い
					//	しかし、復元のすべての組み合わせは膨大である．．
					//	一度、ARGB8888に変換して、そこからターゲット型に
					//	戻せば、そうでも無いのだが．．

					if (m_nSizeX!=0 && m_nSizeY!=0){
						//	とりあえず、サーフェース作りなおしとくか．．
						InnerCreateSurface(m_nSizeX,m_nSizeY,m_bYGA,m_bSecondary256);
					}
				}
				//	現在の画面モードに合わせてColorKeyを設定しなおす
				UpdateColorKey();
				//	オーナードローかも知れないので、それを復元する
				lr |= OnDraw();	//	委譲する
			}
		}
	}
	m_bNowRestoring = false;	//	リストア終了につき、Releaseでフック解除するの解除！
	return lr;
}


LRESULT CFastPlane::Load(const string& strBitmapFileName){
	Release();
	ResetColorKey();
	// あとでRestoreできるようにファイル名を格納しておく。
	LRESULT lr = InnerLoad(strBitmapFileName);
	m_strBitmapFile = strBitmapFileName;
	return lr;
}

//	ファイル名を返す
string	CFastPlane::GetFileName() const{
	return m_strBitmapFile;
}

////////////////////////////////////////////////////////////////////////////////////

//	ビットマップの内部的なロード。格納ファイル名には影響しない
LRESULT	CFastPlane::InnerLoad(const string& strFileName){
/**
	ISurface::Loadが実装されているので、基本的な読み込みは、
	サーフェース作成後に、ISurface::Loadに委譲すれば良い
*/

	/**
		InnerLoadは、
		ビットマップファイル名、抜き色の指定等を破壊しないことを保証する
		そのためRelease前にすべての設定を保存する
	*/
	string strBitmap(m_strBitmapFile);
	//	colorkeyの設定も保存

	bool bUsePosColorKey = m_bUsePosColorKey;
	int	nColorKeyX = m_nColorKeyX, nColorKeyY = m_nColorKeyY;
	ISurfaceRGB rgbColorKey = m_rgbColorKey;
	{
		Release();	//	解体
	}
	m_strBitmapFile = strBitmap;
	m_bUsePosColorKey = bUsePosColorKey;
	m_nColorKeyX = nColorKeyX; m_nColorKeyY = nColorKeyY;
	m_rgbColorKey = rgbColorKey;


	//	#notdefined#ならば、正常終了したとして帰る
	if (strFileName == "#notdefined#"){
		return 0;
	}

	//	nSurfaceTypeのサーフェースを作成し、そこに読み込む

	int nSurfaceType = GetMyFastDraw()->GetPrimary()->GetSurfaceType();

	m_bLoad256 = false;
	if (m_bYGAUse){
	//	αサーフェースを作成すんのか？
		nSurfaceType = GetYGASurfaceType(nSurfaceType);
		//	対応するαサーフェースの番号を得る
	} else {
		switch(nSurfaceType){
		case 2: nSurfaceType = 4; m_bLoad256=true; break;
		// ---- 8bppならば RGB555で作成。
		//	このとき読み込んだビットマップは、
		//	他の画面モードに移行したときにリストアすべき
		}
	}

	LRESULT lr = ISurface::LoadByType(strFileName,nSurfaceType);
	if (lr!=0) return lr;

	// 抜き色をリセットしとこかー
	// ResetColorKey();　←まちがい('02/11/01)yane.
	// 抜き色を更新しとこかー
	UpdateColorKey();

	return lr;
}

HDC CFastPlane::GetDC(){
	if (m_hBitmap!=NULL){
		return m_hDC;
	}
	
	if (m_hDC!=NULL) {
		Err.Out("CFastPlane::EndPaintが呼び出されていないのにBeginPaintが呼び出された");
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
		Err.Out("CFastPlane::BeginPaintが呼び出されていないのにEndPaintが呼び出された");
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
	case 4:	{ //	RGB555を作成
		CFastPlaneRGB555* lpSurface;
		if (m_bSecondary256DIB){
			//	256色用の仮想セカンダリであるならば、DIBSectionで作っておく
			//	(HDCを取得できるようにするため)
			BITMAPINFO bmi;
			ZERO(bmi);
			bmi.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth		=  m_nSizeX;
			bmi.bmiHeader.biHeight		= -m_nSizeX;
			bmi.bmiHeader.biPlanes		= 1;
			bmi.bmiHeader.biBitCount	= 16;
			bmi.bmiHeader.biCompression	= BI_RGB;
			HDC hdc = ::GetDC(NULL); // hWndのほうがええんか？
			m_hBitmap = ::CreateDIBSection(hdc /* NULL*/, &bmi , DIB_RGB_COLORS, (void**)&lpSurface, NULL, 0 );
			//	↑これで作られるDIBはRGB555のようだが？
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
		//	このとき、HDCは取得できない。ゴメンネ
	case 3:	{ //	RGB565を作成
		CFastPlaneRGB565* lpSurface = new CFastPlaneRGB565[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneRGB565),rc);
		break;
				}
	case 5: { //	RGB888を作成
		CFastPlaneRGB888* lpSurface = new CFastPlaneRGB888[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneRGB888),rc);
		break;
			}
	case 6: { //	BGR888を作成
		CFastPlaneBGR888* lpSurface = new CFastPlaneBGR888[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneBGR888),rc);
		break;
			}
	case 7: { //	XRGB8888を作成
		CFastPlaneXRGB8888* lpSurface = new CFastPlaneXRGB8888[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneXRGB8888),rc);
		break;
			}
	case 8: { //	XBGR8888を作成
		CFastPlaneXBGR8888* lpSurface = new CFastPlaneXBGR8888[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneXBGR8888),rc);
		break;
			}

	case 10:	{ //	ARGB4565を作成
		CFastPlaneARGB4565* lpSurface = new CFastPlaneARGB4565[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneARGB4565),rc);
		break;
				}
	case 11: { //	ARGB4555を作成
		CFastPlaneARGB4555* lpSurface = new CFastPlaneARGB4555[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneARGB4555),rc);
		break;
				}
	case 12:	//	ARGB8888
			{ //	ARGB8888を作成
		CFastPlaneARGB8888* lpSurface = new CFastPlaneARGB8888[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneARGB8888),rc);
		break;
				}
	case 13:	//	ABGR8888
			{ //	ABGR8888を作成
		CFastPlaneABGR8888* lpSurface = new CFastPlaneABGR8888[m_nSizeX * m_nSizeY];
		GetSurfaceInfo()->Init((void*)lpSurface,m_nSizeX*sizeof(CFastPlaneABGR8888),rc);
		break;
				}
	default:
		return 1;	//	サポートしてへんで＾＾；
	}

	m_bYGA = (nSurfaceType>=10);

	GetSurfaceInfo()->SetSurfaceType(nSurfaceType);
	m_bMySurface = true; // 自分でnewしたサーフェースであることを意味するマーカー

	//	クリアフラグ立ってないならクリアしない
	if (bClear) Clear();

	//	hook 開始する
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
		return 1;	//	こんなサーフェース、勘弁してくれー＾＾；
	}

	//	現在の画面モードに応じたものにする必要あり
	int nType = GetMyFastDraw()->GetPrimary()->GetSurfaceType();

	if (!bYGA) {
		//	YGA画像では無いので無条件でDirectDrawSurface
		//	ただし、8bppのときは、RGB555のサーフェースを作る
		if (nType == 2 && !bSecondary256) {
			return InnerCreateMySurface(sx,sy,4);
		} else {
			if (bSecondary256) m_bSecondary256 = true;
			//	このサーフェースは、256色用セカンダリ
		}

		///	現在のプライマリより大きなサーフェースは作成に失敗する
		///	(DirectX3の制限)
		int nPx,nPy;
		GetMyFastDraw()->GetPrimary()->GetSize(nPx,nPy);
		bool bCreateDirectDrawSurface = !(nPx < sx || nPx < sy);
		if (bCreateDirectDrawSurface) {

			DDSURFACEDESC ddsd;
			ZERO(ddsd);
			ddsd.dwSize = sizeof(ddsd);
			ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
			//	強制的にシステムメモリを使うオプション
			if (m_bUseSystemMemory) {
				ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			} else {
				ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
			}
			// サイズを保存しておく
			m_nSizeX = ddsd.dwWidth	 = sx;
			m_nSizeY = ddsd.dwHeight = sy;

			//	RGB888で、奇数バイトで終わっているとやらしいので、
			//	余分に確保するようにする。dWidthが奇数ならば偶数で確保するようにする
			if (GetMyFastDraw()->GetPrimary()->GetBpp()==24 && ((ddsd.dwWidth & 1)==1)){
				ddsd.dwWidth++;	//	強制的に偶数バイトにアライン
			}
			LPDIRECTDRAW lpDraw = GetMyFastDraw()->GetDDraw();
			if (lpDraw==NULL) return 1;
			if (lpDraw->CreateSurface(&ddsd,&m_lpSurface,NULL)!=DD_OK){
				Err.Out("CFastPlane::InnerCreateSurfaceのCreateSurfaceに失敗");
				return 2; // あじゃー
			}
		} else {
			return InnerCreateMySurface(sx,sy,nType);
		}

		UpdateSurfaceInfo();

		//	m_nSurfaceRef = 1;	//	参照カウントの設定＾＾
		Clear();	//	念のためクリアしておく(最上位を0にするため)
	} else {
		//	ＹＧＡ画像なので、現在の画面モードに応じたものにする必要あり
		int nType = GetMyFastDraw()->GetPrimary()->GetSurfaceType();
		switch (nType){
		case 3:	 //	ARGB4565を作成
			return InnerCreateMySurface(sx,sy,10);
			break;

		case 2:	 // ⇒8bppのときは、RGB555なので、それに対するYGAはARGB4555
		case 4:	 //	ARGB4555を作成
			return InnerCreateMySurface(sx,sy,11);
			break;

		//	RGB順ならばARGB8888でええんちゃう？
		case 5:	//	RGB888
		case 7:	//	XRGB8888
			return InnerCreateMySurface(sx,sy,12);
			break;
		//	BGR順ならば飽和加算のこととか考えてABGR8888にしとこか？
		case 6:	//	BGR888
		case 8:	//	XBGR8888
			return InnerCreateMySurface(sx,sy,13);
			break;
		}

		//	クリアしておかないとαにゴミが残ったままになる．．＾＾；
		Clear();
	}

	m_bYGA = bYGA;

	//	hook 開始する
	//	--- これら以外は、InnerCreateMySurfaceのなかでhookしているのでok
	if (GetMyFastDraw()!=NULL){
		GetMyFastDraw()->GetFastPlaneList()->insert(this);
	}
	return 0;
}

LRESULT CFastPlane::CreateSurface(int sx,int sy,bool bYGA){
	//	普通にCreateSurfaceすると、それは間違いなくオーナードロープレーンである
//	m_bOwnerDraw	= false;
//	m_bBitmap		= false;
	ResetColorKey();

	LRESULT lr = InnerCreateSurface(sx,sy,bYGA || m_bYGAUse);
	if (lr) return lr;

	UpdateColorKey();

	//	CreateSurfaceしてるんだから、FillColorはリセットすべき? '00/09/09
	//	⇒しかし、こうしてしまうと、画面を切り替えたあと、FillColorが変わってしまう．．

//	m_bOwnerDraw	= true;

	return 0;
}

//	プライマリサーフェースの生成
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
		Err.Out("CFastPlane::CreatePrimaryに失敗");
		if (bUseFlip) {
			bUseFlip=false;
			// Flipping Surfaceはビデオメモリ上に配置する必要があるため、
			// Createにミスしたとも考えられる
			goto sur_retry;
		}
		return 1; // あじゃー
	}
	//	ここでコピーしておく
	if (nSx && nSy) {
		m_nSizeX = nSx; m_nSizeY = nSy;	//	サイズはこれを採用
	} else {
		GetMyFastDraw()->GetSize(m_nSizeX,m_nSizeY);
	}

	UpdateSurfaceInfo();

	if (GetSurfaceType() == 2) {
		//	8bppやったら、これセカンダリ256同様、
		//	256色モードやけど、RGB555では無く、8bppとして用意された
		//	特殊なサーフェースとして申請する
		m_bSecondary256 = true;
	}

	//	このサーフェースに対しても、カラーキーは設定しとこか．．
	UpdateColorKey();

//	m_nSurfaceRef = 1;		//	参照カウント足しておかないとうまく解放されない
	m_bOwnerDraw = true;	//	これをOnにしないとRestoreされてしまう
	return 0;
}

//	セカンダリサーフェースの生成
LRESULT CFastPlane::CreateSecondary(CFastPlane*lpPrimary,bool& bUseFlip){
	Release();
	ResetColorKey();

	LPDIRECTDRAW lpDraw = GetMyFastDraw()->GetDDraw();
	if (lpDraw==NULL) return 1;

	//	↓flipルーチン書きかけ！未サポート！
sur_retry: ;
	if (bUseFlip) {
		// flipを使う以上、システムメモリにバッファを確保するのはまずい．．．
		DDSCAPS ddscaps;		
		ZERO(ddscaps);	//	いらないけど一応ね
		if (m_bUseSystemMemory) {
			ddscaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_SYSTEMMEMORY;
		} else {
			ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		}
		if (lpPrimary->GetSurface()->GetAttachedSurface(&ddscaps,&m_lpSurface)!=DD_OK){
			Err.Out("CFastPlane::CreateSecondaryのGetAttachedSurfaceに失敗");
			// ほやから言わんこっちゃない！
			bUseFlip = false;
			goto sur_retry;
		}
		lpPrimary->GetSize(m_nSizeX,m_nSizeY);
		//	一応クリアしとこか...
		Clear();
		//	サーフェースのチェック＆更新
		UpdateSurfaceInfo();
	} else {
		int nSizeX,nSizeY;
		lpPrimary->GetSize(nSizeX,nSizeY);
		if (InnerCreateSurface(nSizeX,nSizeY)!=0){
			Err.Out("CFastPlane::CreateSecondaryに失敗");
			return 0;
		}
		//	UpdateとClearは、InnerCreateSurfaceが面倒を見る
	}

	//	このサーフェースに対しても、カラーキーは設定しとこか．．
	UpdateColorKey();

//	m_nSurfaceRef = 1;		//	参照カウント足しておかないとうまく解放されない
	m_bOwnerDraw = true;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//	新しくサーフェースを作ったときには、この関数を呼び出すこと！
LRESULT	CFastPlane::UpdateSurfaceInfo(){
	int nSurfaceType = CDirectDrawSurfaceManager().GetSurfaceType(GetSurface());

	//	実際にLock作業が必要である
	DDSURFACEDESC ddsd = { sizeof (ddsd) };
	LRESULT hres;
	ddsd.dwSize = sizeof(ddsd);
	while ((hres = GetSurface()->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING)
		;
	SIZE rc = { m_nSizeX,m_nSizeY };

	if (hres != DD_OK) {
		if (IsPrimarySurface()){
			//	primaryかも知れん
			GetSurfaceInfo()->Init(NULL,0,rc,nSurfaceType);
			///	primaryは、Lockに失敗することがあるから初期化だけでもしとかにゃいかん
			GetSurfaceInfo()->SetLocker(smart_ptr<ISurfaceLocker>(new IDisableSurfaceLocker));
			///	SurfaceのLockは必ず失敗するようにしとかなくてはいかん
		} else {
			//	primaryでないのにlock失敗したらダメぽ
			GetSurfaceInfo()->SetInit(false);
		}
		return 1;	//	Lockに失敗
	}
	GetSurfaceInfo()->Init(ddsd.lpSurface,ddsd.lPitch,rc,nSurfaceType);
	//	↑アドレスを特定するために、これが必要

	//	unlockを忘れると大変なことに＾＾；
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
// ディスプレイの色数を調べるのにGetDisplayModeは使ってはいけない
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
