
#include "stdafx.h"
#include "yaneSprite.h"
#include "yaneSurface.h"
#include "../Auxiliary/yaneFile.h"
#include "../Auxiliary/yaneStringScanner.h" // CLineParser
#include "yanePlane.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

//////////////////////////////////////////////////////////////////////////////
//	CSprite

void	CSimpleSprite::set(ISurface*lp){
	//	プレーン全域を一つのスプライトとする
	WARNING(lp==NULL,"CSimpleSprite::SetSurfaceでlp==NULL");

	pSurface = lp;
	int	sx,sy;
	pSurface->GetSize(sx,sy);
	::SetRect(&rcRect,0,0,sx,sy);
	nOx = nOy = 0;
	nHeight	= sy;
	bFast	= true;
}

void	CSimpleSprite::set(ISurface*lp,const RECT &rc){
	//	プレーン全域を一つのスプライトとする
	WARNING(lp==NULL,"CSimpleSprite::SetSurfaceでlp==NULL");

	pSurface = lp;
	rcRect = rc;
	nOx = nOy = 0;
	nHeight	= rc.bottom-rc.top;
	bFast	= true;
}

//////////////////////////////////////////////////////////////////////////////
//	CSprite

CSprite::CSprite() {
	m_nAnimation	= 0;
	m_nDirection	= 0;
	m_nOx			= 0;
	m_nOy			= 0;
	m_nPriority		= 0;	//	これがディフォルト（深い意味は無い）
	m_bVisible		= true;
	m_nHeight		= 0;	//	深い意味はない
	m_vSprtieVector.Add(new spritevectorofvector);
}

CSprite::~CSprite(){
}

void	CSprite::setOffset(int nOffsetX,int nOffsetY){
	m_nOx = nOffsetX;
	m_nOy = nOffsetY;
}

void	CSprite::getOffset(int& nOffsetX,int& nOffsetY){
	nOffsetX = m_nOx;
	nOffsetY = m_nOy;
}

void	CSprite::incMotion(){
	m_nAnimation++;
	if (m_nAnimation >= (int)getDirectionalSprite()->size()) {
		m_nAnimation = 0;
	}
}

void	CSprite::setDirection(int nDirection){
	//	方向の変更が無い場合はアニメカウンタをリセットしない（仕様）
	if (m_nDirection != nDirection) {
		m_nAnimation = 0;
	}
	m_nDirection = nDirection;
}

//////////////////////////////////////////////////////////////////////////////

void	CSpriteEx::Blt(ISurface*pSurface,LPRECT lpClip){
	Blt(pSurface,0,0,lpClip);
}

void	CSpriteEx::Blt(ISurface*pSurface,int x,int y,LPRECT lpClip){
	BltFix(pSurface,x,y,lpClip);
	incMotion();
}

void	CSpriteEx::BltFix(ISurface*pSurface,LPRECT lpClip){
	int x,y;
	getPos(x,y);
	BltFix(pSurface,x,y,lpClip);
}

void	CSpriteEx::BltFix(ISurface*pSurface,int x,int y,LPRECT lpClip){
	//	有効か？
	if (!isEnable()) return ;

	//	CDirectDrawで描画する場合Layer無視
	CSimpleSprite &s = *getSprite();

	int ox,oy;
	getOffset(ox,oy);
	ox+=x+s.nOx;
	oy+=y+s.nOy;

	//	そのまま委譲してまうとすっか～
	if (!s.bFast) {
		pSurface->Blt(s.pSurface,ox,oy,NULL,&s.rcRect,lpClip);
	} else {
		pSurface->BltFast(s.pSurface,ox,oy,NULL,&s.rcRect,lpClip);
	}
}

void	CSpriteEx::BltOnce(ISurface*pSurface,int x,int y,LPRECT lpClip){
	BltFix(pSurface,x,y,lpClip);
	
	//	ケツになっていたら、それ以上は加算しない
	int n=getMotion();
	incMotion();
	if (getMotion()==0) {
		setMotion(n);
	}
}

//////////////////////////////////////////////////////////////////////////////
///	yaneSDK2ndからのぶっこ抜き。あまりいい実装ではないのでマネしないでネ

void	CSpriteChara::Release()
{
	m_apSurface.clear();
	m_vSpriteVector.Delete();
}

LRESULT	CSpriteChara::Load(const string& strFileName)
{
	Release();
	m_vSpriteVector.Add(new CSprite::spritevectorofvector);
	CFile file;
	if (file.Read(strFileName)!=0) return 1;

	//	ReadLine()で一行ずつ解析してスプライト設定を行なう

	//	スプライトスライスの用意
	CSimpleSprite ssprite[256];
	ZERO(ssprite);

	int		nLine = 0;
	CHAR	szBuf[256];
	while (true) {
		nLine++;

		CLineParser		lineParser;
		if( file.ReadLine( szBuf ) !=0 ) break;
		lineParser.SetLine( szBuf );					//	ライン パーサーに文字列をセットする

		//////////////////////////////////////////////////
		// 各コマンド別に実行させる

		// スプライト定義数の設定
		if ( lineParser.IsMatch( "#SpriteMax" ) ){
			int		n;
			if ( lineParser.GetNum( n ) != 0 ) { Err.Out("CSpriteChara::Loadで#SpriteMaxに失敗"); return 2; }
//			m_lpSprite.resize(n);
//			m_nSpriteMax = n;
			m_vSpriteVector->resize(n);
			continue;
		}
		////	最大一致法 SpriteMax > SpriteF > Sprite
		//	スプライトxの矩形循環の設定 (ただし抜き色は無効)
		if ( lineParser.IsMatch( "#SpriteF" ) ){
			WARNING(m_vSpriteVector->empty(),"CSpriteChara::LoadでSpriteMaxなしに#Spriteが有る");
			int		n,m;
			lineParser.GetNum(n);
			while (true){
				if (lineParser.GetNum(m)) break;
				ssprite[m].bFast=true;

				//	高さは、足元
				ssprite[m].nHeight = 32;
					//	今回はこれで統一したほうがよさげ...
					//	もし変更が必要な場合は、各キャラクラスで変更すること

				(*m_vSpriteVector)[n].push_back(ssprite[m]);
			}
			continue;
		}
/*
	⇒　一括定義
	#SpritePlane 0,0,34					//	スプライト0～34は、プレーン0から35個目(プレーン34)とする。
	#SpritePlaneF 0,0,34					//	スプライト0～34は、プレーン0から35個目(プレーン34)とする。ただし抜き色は無効
*/

		//	非循環スプライトxの設定
		if ( lineParser.IsMatch( "#SpritePlaneF" ) ){
			WARNING(m_vSpriteVector->empty(),"CSpriteChara::LoadでSpriteMaxなしに#Spriteが有る");
			int		n,m,l;
			lineParser.GetNum(n);
			if (lineParser.GetNum(m)) break;
			if (lineParser.GetNum(l)) break;

			CSimpleSprite s = {0};
			for(int i=n;i<n+l;i++){
				ssprite[i].bFast=true;
				//	高さは、足元
				ssprite[i].nHeight = 32;
				//	今回はこれで統一したほうがよさげ...
				//	もし変更が必要な場合は、各キャラクラスで変更すること
				s.pSurface = m_apSurface[m+i].get();
				{
					int sx,sy;
					s.pSurface->GetSize(sx,sy);
					::SetRect(&s.rcRect,0,0,sx,sy);
				}
//				m_lpSprite[i].SetSpriteAdd(&s);
				(*m_vSpriteVector)[n].push_back(s);
			}
			continue;
		}

		//	非循環スプライトxの設定
		if ( lineParser.IsMatch( "#SpritePlane" ) ){
			WARNING(m_vSpriteVector->empty(),"CSpriteChara::LoadでSpriteMaxなしに#Spriteが有る");
			int		n,m,l;
			lineParser.GetNum(n);
			if (lineParser.GetNum(m)) break;
			if (lineParser.GetNum(l)) break;

			CSimpleSprite s = {0};
			for(int i=n;i<n+l;i++){
				ssprite[i].bFast=false;
				//	高さは、足元
				ssprite[i].nHeight = 32;
				//	今回はこれで統一したほうがよさげ...
				//	もし変更が必要な場合は、各キャラクラスで変更すること
				s.pSurface = m_apSurface[m+i].get();
				{
					int sx,sy;
					s.pSurface->GetSize(sx,sy);
					::SetRect(&s.rcRect,0,0,sx,sy);
				}
//				m_lpSprite[i].SetSpriteAdd(&s);
				(*m_vSpriteVector)[n].push_back(s);
			}
			continue;
		}


		//	スプライトxの矩形循環の設定
		if ( lineParser.IsMatch( "#Sprite" ) ){
			WARNING(m_vSpriteVector->empty(),"CSpriteChara::LoadでSpriteMaxなしに#Spriteが有る");
			int		n,m;
			lineParser.GetNum(n);
			while (true){
				if (lineParser.GetNum(m)) break;
				ssprite[m].bFast=false;

				//	高さは、足元
				ssprite[m].nHeight = 32;
					//	今回はこれで統一したほうがよさげ...
					//	もし変更が必要な場合は、各キャラクラスで変更すること
				
//				m_lpSprite[n].SetSpriteAdd(&ssprite[m]);
				(*m_vSpriteVector)[n].push_back(ssprite[m]);
			}
			continue;
		}

		// 指定したプレーンpにスプライトデータを読み込む
		if ( lineParser.IsMatch( "#Plane" ) ){
			int		n;
			lineParser.GetNum( n );
			//	不足分は、newする
			while ((int)m_apSurface.size()<=n){
				m_apSurface.insert(CPlane::GetFactory()->CreateInstance());
			}
			string szFile;
			lineParser.GetStr(szFile);
			m_apSurface[n]->Load(szFile);
			continue;
		}

		//	指定したプレーンpの抜き色を座標で指定
		if ( lineParser.IsMatch( "#ColorKeyA" ) ){
			int		n,m;
			lineParser.GetNum( n );
			lineParser.GetNum( m );
			//	不足分は、newする
			//	(ただし、ここでnewしたものに対してSetColorKeyは不正だが．．)
			while ((int)m_apSurface.size()<=m){
				m_apSurface.insert(CPlane::GetFactory()->CreateInstance());
			}
			int x,y;
			lineParser.GetNum( x );
			lineParser.GetNum( y );
			for(int i=n;i<=m;i++) {
				m_apSurface[i]->SetColorKeyPos(x,y);
			}
			continue;
		}

		//	指定したプレーンpの抜き色をRGBで指定
		if ( lineParser.IsMatch( "#ColorKeyB" ) ){
			int		n,m;
			lineParser.GetNum( n );
			lineParser.GetNum( m );
			//	不足分は、newする
			while ((int)m_apSurface.size()<=m){
				m_apSurface.insert(CPlane::GetFactory()->CreateInstance());
			}
			int r,g,b;
			lineParser.GetNum( r );
			lineParser.GetNum( g );
			lineParser.GetNum( b );
			for(int i=n;i<=m;i++) {
				m_apSurface[i]->SetColorKey(ISurface::makeRGB(r,g,b));
			}
			continue;
		}

		//	指定したプレーンpの抜き色を座標で指定
		if ( lineParser.IsMatch( "#ColorKey" ) ){
			int		n;
			lineParser.GetNum( n );
			//	不足分は、newする
			while ((int)m_apSurface.size()<=n){
				m_apSurface.insert(CPlane::GetFactory()->CreateInstance());
			}
			int x,y;
			lineParser.GetNum( x );
			lineParser.GetNum( y );
			m_apSurface[n]->SetColorKeyPos(x,y);
			continue;
		}

		//	最大一致法 (RectOffset > RectA > Rect)
		//	矩形aからbまでに、(x,y)のオフセットを与える
		if ( lineParser.IsMatch( "#RectOffset" ) ){
			int		a,b;
			lineParser.GetNum( a );
			lineParser.GetNum( b );
			int		x,y;
			lineParser.GetNum(x);
			lineParser.GetNum(y);
			for(int i=a;i<=b;i++){
				ssprite[i].nOx = x;
				ssprite[i].nOy = y;
			}
			continue;
		}

		//	矩形aから、プレーンpに対して、(x,y)からW×Hで横x列、縦y行の繰り替えし定義を行う
		if ( lineParser.IsMatch( "#RectA" ) ){
			int		n;
			lineParser.GetNum( n );
			int		nPlane;
			lineParser.GetNum( nPlane );

			int		x,y,w,h,xt,yt;
			lineParser.GetNum(x);
			lineParser.GetNum(y);
			lineParser.GetNum(w);
			lineParser.GetNum(h);
			lineParser.GetNum(xt);
			lineParser.GetNum(yt);
			for(int j=0;j<yt;j++){
				for(int i=0;i<xt;i++){
					::SetRect( &ssprite[n+i+j*xt].rcRect,x+w*i,y+h*j,x+w*(i+1),y+h*(j+1));
					ssprite[n+i+j*xt].pSurface = m_apSurface[nPlane].get();
				}
			}
			continue;
		}

		//	矩形aとしてプレーンpの(x,y)からW×Hの矩形とする
		if ( lineParser.IsMatch( "#Rect" ) ){
			int		n;
			lineParser.GetNum( n );
			WARNING(n<0||n>255,"CSpriteChara::Loadでn<0||n>255");
			int		nPlane;
			lineParser.GetNum( nPlane );
			int		x,y,sx,sy;
			lineParser.GetNum(x);
			lineParser.GetNum(y);
			lineParser.GetNum(sx);
			lineParser.GetNum(sy);
			::SetRect( &ssprite[n].rcRect,x,y,x+sx,y+sy);
			ssprite[n].pSurface = m_apSurface[nPlane].get();
			continue;
		}

		/*
		//	スプライトs1からs2はレイヤーLとして定義（ディフォルトで5）
		if ( lineParser.IsMatch( "#Layer" ) ){
			int		a,b;
			lineParser.GetNum( a );
			lineParser.GetNum( b );
			int		nLayer;
			lineParser.GetNum(nLayer);
			for(int i=a;i<=b;i++){
				m_lpSprite[i].SetLayer(nLayer);
			}
			continue;
		}
		*/
		if (lineParser.IsMatch("#")){
			Err.Out("CSpriteChara::Loadで%sの%d行に不明な命令",strFileName,nLine);
			return 1;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////

/*
	スプライト定義ファイルのフォーマット
	・大文字小文字は区別しない
	・１行は２５６文字まで
	・矩形定義は256まで
	・スプライトとは、ある一連の動き（CSimpleSprite）を定義する
	・スプライト定義は、矩形を順番に指定することによって行なう
	・各行、//以降はコメントとできる
	・プレーンは無限に利用できる
	・矩形とともに、足の位置（ベースライン）を指定することがある
	・矩形とともに、オフセットを指定することがある
	・スプライト・矩形番号は０から始まる
	#SpriteMax	4			//	スプライト定義数は４。これは先頭で指定する
	#Plane 0,"test.bmp"		//	プレーン0としてtest.bmpを読み込む
	#ColorKey 0,4,10		//	0番のプレーンの抜き色を位置で指定し、それは(4,10)とする(ディフォルトは(0,0))
	#Plane 1,"test2.bmp"	//	プレーン1としてtest2.bmpを読み込む
	#Rect 0,1,48,48,64,128	//	矩形0としてプレーン1の(48,48)からW64×H128の矩形とする
	#Rect 1,1,112,48,64,128	//	矩形1としてプレーン1の(112,48)からW64×H128の矩形とする
	#Sprite 1,0,1,1,0		//	スプライト1は矩形0→矩形1→矩形1→矩形0という循環とする
	#SpriteF 0,0,1,1,0		//	スプライト0は矩形0→矩形1→矩形1→矩形0という循環とする。ただし抜き色は無効


	⇒　一括定義
	#SpritePlane 0,0,34					//	スプライト0～34は、プレーン0から35個(プレーン0からプレーン34)とする。
	#SpritePlaneF 0,0,34				//	スプライト0～34は、プレーン0から35個(プレーン0からプレーン34)とする。ただし抜き色は無効
	#ColorKeyA 0,34,4,10				//	0～34のプレーンのカラーキーは位置で指定し、それは(4,10)とする(ディフォルトは(0,0))
	#ColorKeyB 0,107,0,255,0			
	//	0-107番のプレーンの抜き色をRGBで指定し、それは(0,255,0)とする

//	#SpriteA 0,0,34						//	スプライト0～34は、矩形0から35個目の矩形までとする。（未実装）

	#RectA	12,0,10,15,32,64,4,3	//	矩形12～は、プレーン0に対する、W32×H64のサイズであり、
									//	それは(10,15)に始まり、横4列、縦3行の12回分繰り替えし
									//	定義される。つまり矩形12～23までが一気に定義される
	#RectOffset 10,16,5,6	//	矩形10から16までに、(5,6)のオフセットを与える

//	#Layer 1,3,5			//	スプライト1から3はLayer5として定義（ディフォルトで5）
//

*/

}} // end of namespace
