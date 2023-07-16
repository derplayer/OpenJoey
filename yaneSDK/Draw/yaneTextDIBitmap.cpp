#include "stdafx.h"
#include "yaneSurface.h"
#include "yaneTextDIBitmap.h"
/**
	このクラスは、yaneSDK2ndのものを突貫工事でコピペしたもの
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
	if (!IsVertical()) {	// 横書きの場合
		lr = UpdateTextHorizontal();
	} else {				// 縦書きの場合
		//	fontは90゜倒す
		GetFont()->SetVerticalFont(!GetFont()->IsVerticalFont());
		lr = UpdateTextVertical();
		GetFont()->SetVerticalFont(!GetFont()->IsVerticalFont());
	}
	return lr;
}

LRESULT	CTextDIBitmap::UpdateTextA(){	//	アンチェリ付き描画
	LRESULT lr;
	if (!IsVertical()) {	// 横書きの場合
		lr = UpdateTextHorizontalA();
	} else {				// 縦書きの場合
		//	fontは90゜倒す
		GetFont()->SetVerticalFont(!GetFont()->IsVerticalFont());
		lr = UpdateTextVerticalA();
		GetFont()->SetVerticalFont(!GetFont()->IsVerticalFont());
	}
	return lr;
}

//	↑上のコピペした＾＾；
LRESULT	CTextDIBitmap::UpdateTextAA(){	//	アンチェリ付き描画
	LRESULT lr;
	if (!IsVertical()) {	// 横書きの場合
		lr = UpdateTextHorizontalAA();
	} else {				// 縦書きの場合
		//	fontは90゜倒す
		GetFont()->SetVerticalFont(!GetFont()->IsVerticalFont());
		lr = UpdateTextVerticalAA();
		GetFont()->SetVerticalFont(!GetFont()->IsVerticalFont());
	}
	return lr;
}

//////////////////////////////////////////////////////////
/*

	::GetGlyphOutlineは、
		Win9x系では、bold指定すると化ける
		(猪名川で行こうは、このせいで一部文字化けする)
		また、ServicePackを当てていないNT4.0だと、
			回転させたときに、OSが落ちる
		あと、一文字ずつしか変換できないので、
		途中にイタリック文字がまじっているとその調整を
		自前でやらないといけない。

	以上を理解して使うのならば、悪くはないのだけど．．
*/
//////////////////////////////////////////////////////////
// 横書き

inline LRESULT CTextDIBitmap::UpdateTextHorizontal(){
	int sx,sy;
	m_Font.GetSize(sx,sy);
	// m_Fontのテキストが空ならプレーンは作らない
	//	(どうせBltterがチェックして不正なプレーンならば転送しないので)
	if (CreateSurface(sx,sy,32)) return 1;
	// 背景色で塗りつぶす
	Clear();

	m_Font.OnDraw(GetDC(),m_nTextX,m_nTextY);
	return 0;
}



inline LRESULT CTextDIBitmap::UpdateTextHorizontalA(){	//	アンチェリ付き描画
	int sx,sy;
	m_Font.GetSize(sx,sy);
	if (CreateSurface(sx,sy,32,true)) return 1;
	//　背景色で塗りつぶす
	//	こうしないと縮小したサーフェスをコピーした時にゴミが乗るのだ
	Clear();

	COLORREF rgb = m_Font.GetColor();
	COLORREF bk	 = m_Font.GetBackColor();

	m_Font.SetBackColor(CLR_INVALID);
	m_Font.SetColor(RGB(255,255,255));	//	白にして
	int	nQuality = m_Font.GetQuality();
	m_Font.SetQuality(4);	//	アンチェリ無しで書く
	int nFontSize = m_Font.GetSize();
	int nFontHeight = m_Font.GetHeight();
	m_Font.SetSize(nFontSize<<1);	//	2倍サイズで描画
	m_Font.SetHeight(nFontHeight<<1); //　行間も2倍サイズで描画
	//	SetSizeは、行間も変更してしまうので、
	//	SetSizeする前に、行間を取得しておき、それを２倍しなければならない

	CDIBitmap d_dib;
	int sx2,sy2;
	m_Font.GetSize(sx2,sy2);
	// m_Fontのテキストが空でもプレーンを作る．
	if ((sx2 == 0) || (sy2 == 0)) {
		sx2 = sy2 = 1;
	}
	if (d_dib.CreateSurface(sx2,sy2,32,true)) return 2;
	// 背景色で塗りつぶす
	d_dib.GetSurfaceInfo()->SetFillColor(GetSurfaceInfo()->GetFillColor());
	d_dib.Clear();
	//	d_dib.GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	m_Font.OnDraw(d_dib.GetDC(),m_nTextX<<1,m_nTextY<<1);	//	2倍のフォントサイズで描画

	//	２×２→等倍に縮小
	{
	  sx2>>=1; sy2>>=1;
	  if (sx2<sx) { sx = sx2; }	//　なんや？生成されたプレーン／２のほうが小さいで...
	  if (sy2<sy) { sy = sy2; }
	  DWORD dwColor = ((rgb & 0xff) << 16) + (rgb & 0xff00) + ((rgb & 0xff0000)>>16);	//	転置して
	  ULONG lDstWidth = d_dib.GetWidth();
	  for(int y=0;y<sy;y++){
		DWORD *lpdw		= (DWORD*)(GetPtr() + 4*GetWidth() * y);
		DWORD *lpdw2a	= (DWORD*)(d_dib.GetPtr() + (4*lDstWidth * (y*2)));
		DWORD *lpdw2b	= lpdw2a + lDstWidth; // 次ライン
		//	得られた明るさをαチャンネルに持っていく
		for(int x=0;x<sx;x++){
			DWORD dwAlpha;
			dwAlpha =  ((*lpdw2a)&0xff) + ((*(lpdw2a+1))&0xff);
			dwAlpha += ((*lpdw2b)&0xff) + ((*(lpdw2b+1))&0xff);
			*lpdw = ((dwAlpha << (24-2)) & 0xff000000) + dwColor;	//	色は、rgbで指定された色
			lpdw++; lpdw2a+=2; lpdw2b+=2;
		}
	  }
	}

	//	復帰させる＾＾；　なんぎやなー（笑）
	m_Font.SetSize(nFontSize);
	m_Font.SetHeight(nFontHeight);
	//	↑ここで戻さないと、文字の影を生成するときに嘘のフォントサイズになっちまう＾＾；

	//	文字の影
	if (bk!=CLR_INVALID) {
	//	影つき文字やんか．．．やらしいなぁ＾＾；
		CTextDIBitmap dib;
		*dib.GetFont() = *GetFont();	//	フォントまるごとコピーしてまえー！！＾＾；
		dib.GetFont()->SetColor(bk);
		dib.GetFont()->SetBackColor(CLR_INVALID);	//	これをCLR_INVALIDにしておけば、この再帰は２度目で停止する
//		dib.GetFont()->SetText(m_Font.GetText());
//		dib.SetTextPos(2,2);	//	ずらして描画！
		{
			int nOx, nOy;
			dib.GetFont()->GetShadowOffset(nOx,nOy);
			dib.SetTextPos(nOx,nOy); // ずらして描画！
		}
		dib.UpdateTextA();
		//	PhotoShopのレイヤ的不透明度付き合成

		int alpha;
		int beta;
		int total;
		DWORD dwRGB;

		//	このdibのサイズは、同サイズと仮定できないかも知れない．．
//		int nSize = GetRect()->right*GetRect()->bottom;
//		for (int i=0;i<nSize;i++){

		//	dibにあらたに描画させたやつとサイズが異なるかも知れない．．
		ULONG lSrcWidth = GetWidth();
		ULONG lDstWidth = dib.GetWidth();
		for(int y=0;y<sy;y++){
			DWORD *lpdw = (DWORD*)(GetPtr() + (4* lSrcWidth * y));
			DWORD *lpdw2 = (DWORD*)(dib.GetPtr() + (4 * lDstWidth * y));
			//	得られた明るさをαチャンネルに持っていく
			for(int x=0;x<sx;x++){
				/*
				このアルゴリズムについては、やねうらおのホームページの
				スーパープログラマーへの道　「第CB回　PhotoShopのレイヤ合成アルゴリズムについて（YGAフォーマットを提唱する) 00/10/18」
				を参照のこと
				*/
				alpha = *lpdw >> 24;
				beta  = *lpdw2 >> 24;
				beta  = (255-alpha)*beta / 255;	//	エネルギー残量の計算
				//	分配比率が決まったので、これに基づいて分配
				total = alpha + beta;
				if (total != 0){	//	ゼロのときは、当然何もしなくて良い
					if (beta == 0){		//転送先が０なら、そのまま転送
						*lpdw2 = *lpdw;
					}else{
						dwRGB = ((*lpdw & 0xff) * alpha + (*lpdw2 & 0xff) * beta) / total;
						dwRGB += (((*lpdw & 0xff00) * alpha + (*lpdw2 & 0xff00) * beta) / total) & 0xff00;
						dwRGB += (((*lpdw & 0xff0000) * alpha + (*lpdw2 & 0xff0000) * beta) / total) & 0xff0000;
						dwRGB += total << 24;	//	alpha値は、エネルギー総量
						*lpdw = dwRGB;
					}
				}
				lpdw++; lpdw2++;
			}
		}
	}

	//	復帰させる＾＾；　なんぎやなー２（笑）
	m_Font.SetQuality(nQuality);
	m_Font.SetColor(rgb);
	m_Font.SetBackColor(bk);

//	*GetYGA() = true;	//	for BltNatural

	return 0;
}

//	↑上のコピペした＾＾；
inline LRESULT CTextDIBitmap::UpdateTextHorizontalAA(){	//	アンチェリ付き描画
	int sx,sy;
	m_Font.GetSize(sx,sy);
	if (CreateSurface(sx,sy,32,true)) return 1;
	//　背景色で塗りつぶす
	//	こうしないと縮小したサーフェスをコピーした時にゴミが乗るのだ

	//	GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	Clear();

	COLORREF rgb = m_Font.GetColor();
	COLORREF bk	 = m_Font.GetBackColor();

	m_Font.SetBackColor(CLR_INVALID);
	m_Font.SetColor(RGB(255,255,255));	//	白にして
	int	nQuality = m_Font.GetQuality();
	m_Font.SetQuality(4);	//	アンチェリ無しで書く
	int nFontSize = m_Font.GetSize();
	int nFontHeight = m_Font.GetHeight();
	m_Font.SetSize(nFontSize<<2);	//	4倍サイズで描画
	m_Font.SetHeight(nFontHeight<<2); //　行間も4倍サイズで描画

	CDIBitmap d_dib;
	int sx2,sy2;
	m_Font.GetSize(sx2,sy2);
	// m_Fontのテキストが空でもプレーンを作る．
	if ((sx2 == 0) || (sy2 == 0)) {
		sx2 = sy2 = 1;
	}
	if (d_dib.CreateSurface(sx2,sy2,32,true)) return 2;
	// 背景色で塗りつぶす
	d_dib.GetSurfaceInfo()->SetFillColor(GetSurfaceInfo()->GetFillColor());
	//	d_dib.GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	d_dib.Clear();
	m_Font.OnDraw(d_dib.GetDC(),m_nTextX<<2,m_nTextY<<2);	//	4倍のフォントサイズで描画

	//	４×４→等倍に縮小
	{
	  sx2>>=2; sy2>>=2;
	  if (sx2<sx) { sx = sx2; }	//　なんや？生成されたプレーン／２のほうが小さいで...
	  if (sy2<sy) { sy = sy2; }

	  ULONG dstWidth = d_dib.GetWidth();
	  ULONG srcWidth = GetWidth();
	  DWORD dwColor = ((rgb & 0xff) << 16) + (rgb & 0xff00) + ((rgb & 0xff0000)>>16);	//	転置して
	  for(int y=0;y<sy;y++){
		DWORD *lpdw = (DWORD*)(GetPtr() + (4 * srcWidth * y));
		DWORD *lpdw2a =(DWORD*)(d_dib.GetPtr() + (4*dstWidth * (y*4	 )));
		DWORD *lpdw2b = lpdw2a		   +  dstWidth; // 次ライン
		DWORD *lpdw2c = lpdw2b		   +  dstWidth; // 次ライン
		DWORD *lpdw2d = lpdw2c		   +  dstWidth; // 次ライン
		//	得られた明るさをαチャンネルに持っていく
		for(int x=0;x<sx;x++){
			DWORD dwAlpha;
			dwAlpha =	((*lpdw2a)&0xff) + ((*(lpdw2a+1))&0xff) + ((*(lpdw2a+2))&0xff) + ((*(lpdw2a+3))&0xff);
			dwAlpha +=	((*lpdw2b)&0xff) + ((*(lpdw2b+1))&0xff) + ((*(lpdw2b+2))&0xff) + ((*(lpdw2b+3))&0xff);
			dwAlpha +=	((*lpdw2c)&0xff) + ((*(lpdw2c+1))&0xff) + ((*(lpdw2c+2))&0xff) + ((*(lpdw2c+3))&0xff);
			dwAlpha +=	((*lpdw2d)&0xff) + ((*(lpdw2d+1))&0xff) + ((*(lpdw2d+2))&0xff) + ((*(lpdw2d+3))&0xff);
			*lpdw = ((dwAlpha << (24-4)) & 0xff000000) + dwColor;	//	色は、rgbで指定された色
			lpdw++; lpdw2a+=4; lpdw2b+=4; lpdw2c+=4; lpdw2d+=4;
		}
	  }
	}

	//	復帰させる＾＾；　なんぎやなー（笑）
	m_Font.SetSize(nFontSize);
	m_Font.SetHeight(nFontHeight);
	//	↑ここで戻さないと、文字の影を生成するときに嘘のフォントサイズになっちまう＾＾；

	//	文字の影
	if (bk!=CLR_INVALID) {
	//	影つき文字やんか．．．やらしいなぁ＾＾；
		CTextDIBitmap dib;
		*dib.GetFont() = *GetFont();	//	フォントまるごとコピーしてまえー！！＾＾；
		dib.GetFont()->SetColor(bk);
		dib.GetFont()->SetBackColor(CLR_INVALID);	//	これをCLR_INVALIDにしておけば、この再帰は２度目で停止する
//		dib.GetFont()->SetText(m_Font.GetText());
//		dib.SetTextPos(2,2);	//	ずらして描画！
		{
			int nOx, nOy;
			dib.GetFont()->GetShadowOffset(nOx,nOy);
			dib.SetTextPos(nOx,nOy); // ずらして描画！
		}
		dib.UpdateTextAA();

		//	PhotoShopのレイヤ的不透明度付き合成
		int alpha;
		int beta;
		int total;
		DWORD dwRGB;

		//	dibにあらたに描画させたやつとサイズが異なるかも知れない．．
		ULONG lSrcWidth = GetWidth();
		ULONG lDstWidth = dib.GetWidth();

		for(int y=0;y<sy;y++){
			DWORD *lpdw = (DWORD*)(GetPtr() + (4*lSrcWidth * y));
			DWORD *lpdw2 = (DWORD*)(dib.GetPtr() + (4*lDstWidth * y));
			//	得られた明るさをαチャンネルに持っていく
			for(int x=0;x<sx;x++){
				/*
				このアルゴリズムについては、やねうらおのホームページの
				スーパープログラマーへの道　「第CB回　PhotoShopのレイヤ合成アルゴリズムについて（YGAフォーマットを提唱する) 00/10/18」
				を参照のこと
				*/
				alpha = *lpdw >> 24;
				beta  = *lpdw2 >> 24;
				beta  = (255-alpha)*beta / 255;	//	エネルギー残量の計算
				//	分配比率が決まったので、これに基づいて分配
				total = alpha + beta;
				if (total != 0){	//	ゼロのときは、当然何もしなくて良い
					if (beta == 0){		//転送先が０なら、そのまま転送
						*lpdw2 = *lpdw;
					}else{
						dwRGB = ((*lpdw & 0xff) * alpha + (*lpdw2 & 0xff) * beta) / total;
						dwRGB += (((*lpdw & 0xff00) * alpha + (*lpdw2 & 0xff00) * beta) / total) & 0xff00;
						dwRGB += (((*lpdw & 0xff0000) * alpha + (*lpdw2 & 0xff0000) * beta) / total) & 0xff0000;
						dwRGB += total << 24;	//	alpha値は、エネルギー総量
						*lpdw = dwRGB;
					}
				}
				lpdw++; lpdw2++;
			}
		}
	}

	//	復帰させる＾＾；　なんぎやなー２（笑）
	m_Font.SetQuality(nQuality);
	m_Font.SetColor(rgb);
	m_Font.SetBackColor(bk);

//	*GetYGA() = true;	//	for BltNatural

	return 0;
}


//////////////////////////////////////////////////////////
// 縦書き

inline LRESULT CTextDIBitmap::UpdateTextVertical(){
	int sx,sy;

	m_Font.GetSize(sx,sy);
//----------------------------変更
	if (CreateSurface(sy,sx,32)) return 1;	// 逆になる
	// 背景色で塗りつぶす
	//	こうしないと縮小したサーフェスをコピーした時にゴミが乗るのだ
	//GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	Clear();

	// 影の向きを縦書き用に変更
	int nOx, nOy;
	m_Font.GetShadowOffset(nOx, nOy);
	m_Font.SetShadowOffset(nOy, -nOx);

	// 縦書き変換用にもう一つサーフェスを作成
	CDIBitmap d_dib2;
	if (d_dib2.CreateSurface(sx, sy,32)) return 2;
	// 背景色で塗りつぶす
	d_dib2.GetSurfaceInfo()->SetFillColor(GetSurfaceInfo()->GetFillColor());
	// d_dib2.GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	d_dib2.Clear();

 //	m_Font.OnDraw(GetDC(),m_nTextX,m_nTextY);
	m_Font.OnDraw(d_dib2.GetDC(),m_nTextX,m_nTextY);

	// 横→縦変換
	{
		LONG width, height;
		GetSize(width, height);
//		ULONG lSrcWidth = GetWidth();
		ULONG lDstWidth = d_dib2.GetWidth();
		for (int y=0;y<sy;y++) {
			DWORD *lpdw = (DWORD*)(GetPtr() + 4*(width - y -1));	// 横書きの上→下は縦書きの右→左
			DWORD *lpdw2 = (DWORD*)(d_dib2.GetPtr() + 4*(lDstWidth * y));
			for (int x=0;x<sx;x++) {
				*lpdw = *lpdw2;
				lpdw+=width;	// １つ下へ
				lpdw2++;
			}
		}
	}

	// 影の向きを元に戻す
	m_Font.SetShadowOffset(nOx, nOy);
//----------------------------


//	*GetYGA() = false;	//	for BltNatural
	return 0;
}

inline LRESULT CTextDIBitmap::UpdateTextVerticalA(){	//	アンチェリ付き描画
	int sx,sy;
	m_Font.GetSize(sx,sy);
//----------------------------変更
	if (CreateSurface(sy,sx,32,true)) return 1;	// 逆になる
	// 背景色で塗りつぶす
	//	こうしないと縮小したサーフェスをコピーした時にゴミが乗るのだ
	//GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	Clear();
//----------------------------

	COLORREF rgb = m_Font.GetColor();
	COLORREF bk	 = m_Font.GetBackColor();

	m_Font.SetBackColor(CLR_INVALID);
	m_Font.SetColor(RGB(255,255,255));	//	白にして
	int	nQuality = m_Font.GetQuality();
	m_Font.SetQuality(4);	//	アンチェリ無しで書く
	int nFontSize = m_Font.GetSize();
	int nFontHeight = m_Font.GetHeight();
	m_Font.SetSize(nFontSize<<1);	//	2倍サイズで描画
	m_Font.SetHeight(nFontHeight<<1); //　行間も2倍サイズで描画
	//	SetSizeは、行間も変更してしまうので、
	//	SetSizeする前に、行間を取得しておき、それを２倍しなければならない

	CDIBitmap d_dib;
	int sx2,sy2;
	m_Font.GetSize(sx2,sy2);
	// m_Fontのテキストが空でもプレーンを作る．
	if ((sx2 == 0) || (sy2 == 0)) {
		sx2 = sy2 = 1;
	}
	if (d_dib.CreateSurface(sx2,sy2,32,true)) return 2;
	// 背景色で塗りつぶす
	d_dib.GetSurfaceInfo()->SetFillColor(GetSurfaceInfo()->GetFillColor());
	//d_dib.GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	d_dib.Clear();
	m_Font.OnDraw(d_dib.GetDC(),m_nTextX<<1,m_nTextY<<1);	//	2倍のフォントサイズで描画


//----------------------------追加
	// 影の向きを縦書き用に変更
	int nOx, nOy;
	m_Font.GetShadowOffset(nOx, nOy);
	m_Font.SetShadowOffset(nOy, -nOx);

	// 縦書き変換用にもう一つサーフェスを作成
	CDIBitmap d_dib2;
	if (d_dib2.CreateSurface(sx, sy,32,true)) return 2;
	// 背景色で塗りつぶす
	d_dib2.GetSurfaceInfo()->SetFillColor(GetSurfaceInfo()->GetFillColor());
	//d_dib2.GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	d_dib2.Clear();
//----------------------------


/*
	//	等倍のときの転送はこれで良いが．．．
	DWORD *lpdw = GetPtr();
	DWORD dwColor = ((rgb & 0xff) << 16) + (rgb & 0xff00) + ((rgb & 0xff0000)>>16);	//	転置して
	//	明るさをαチャンネルに持っていく
	int nSize = GetRect()->right * GetRect()->bottom;
	for (int i=0;i<nSize;i++){
		*lpdw = ((*lpdw << 8)&0xff000000) + dwColor;	//	色は、rgbで指定された色
		lpdw++;
	}
*/

	//	２×２→等倍に縮小
	{
	  sx2>>=1; sy2>>=1;
	  if (sx2<sx) { sx = sx2; }	//　なんや？生成されたプレーン／２のほうが小さいで...
	  if (sy2<sy) { sy = sy2; }
	  DWORD dwColor = ((rgb & 0xff) << 16) + (rgb & 0xff00) + ((rgb & 0xff0000)>>16);	//	転置して

	  ULONG lDstWidth = d_dib.GetWidth();
	  ULONG lDstWidth2 = d_dib2.GetWidth();

	  for(int y=0;y<sy;y++){
//----------------------------変更
	//	DWORD *lpdw = GetPtr() + (GetRect()->right * y);
		DWORD *lpdw = (DWORD*)(d_dib2.GetPtr() + (4*lDstWidth2 * y));	// 縦書き変換用のものを使う
//----------------------------
		DWORD *lpdw2a = (DWORD*)(d_dib.GetPtr() + 4*(lDstWidth * (y*2	 )));
		DWORD *lpdw2b = lpdw2a		   + lDstWidth; // 次ライン
		//	得られた明るさをαチャンネルに持っていく
		for(int x=0;x<sx;x++){
			DWORD dwAlpha;
			dwAlpha =  ((*lpdw2a)&0xff) + ((*(lpdw2a+1))&0xff);
			dwAlpha += ((*lpdw2b)&0xff) + ((*(lpdw2b+1))&0xff);
			*lpdw = ((dwAlpha << (24-2)) & 0xff000000) + dwColor;	//	色は、rgbで指定された色
			lpdw++; lpdw2a+=2; lpdw2b+=2;
		}
	  }
	}

	//	復帰させる＾＾；　なんぎやなー（笑）
	m_Font.SetSize(nFontSize);
	m_Font.SetHeight(nFontHeight);
	//	↑ここで戻さないと、文字の影を生成するときに嘘のフォントサイズになっちまう＾＾；

	//	文字の影
	if (bk!=CLR_INVALID) {
	//	影つき文字やんか．．．やらしいなぁ＾＾；
		CTextDIBitmap dib;
		*dib.GetFont() = *GetFont();	//	フォントまるごとコピーしてまえー！！＾＾；
		dib.GetFont()->SetColor(bk);
		dib.GetFont()->SetBackColor(CLR_INVALID);	//	これをCLR_INVALIDにしておけば、この再帰は２度目で停止する
//		dib.GetFont()->SetText(m_Font.GetText());
//		dib.SetTextPos(2,2);	//	ずらして描画！
		{
			int nOx, nOy;
			dib.GetFont()->GetShadowOffset(nOx,nOy);
			dib.SetTextPos(nOx,nOy); // ずらして描画！
		}
		dib.UpdateTextA();
		//	PhotoShopのレイヤ的不透明度付き合成

		int alpha;
		int beta;
		int total;
		DWORD dwRGB;

		//	このdibのサイズは、同サイズと仮定できないかも知れない．．
//		int nSize = GetRect()->right*GetRect()->bottom;
//		for (int i=0;i<nSize;i++){

		ULONG lDstWidth = dib.GetWidth();
		ULONG lDstWidth2 = d_dib2.GetWidth();

		//	dibにあらたに描画させたやつとサイズが異なるかも知れない．．
		for(int y=0;y<sy;y++){
//----------------------------変更
	//		DWORD *lpdw = GetPtr() + (GetRect()->right * y);
			DWORD *lpdw = (DWORD*)(d_dib2.GetPtr() + (4*lDstWidth2 * y));	// 縦書き変換用のものを使う
//----------------------------
			DWORD *lpdw2 = (DWORD*)(dib.GetPtr() + (4*lDstWidth * y));
			//	得られた明るさをαチャンネルに持っていく
			for(int x=0;x<sx;x++){
				/*
				このアルゴリズムについては、やねうらおのホームページの
				スーパープログラマーへの道　「第CB回　PhotoShopのレイヤ合成アルゴリズムについて（YGAフォーマットを提唱する) 00/10/18」
				を参照のこと
				*/
				alpha = *lpdw >> 24;
				beta  = *lpdw2 >> 24;
				beta  = (255-alpha)*beta / 255;	//	エネルギー残量の計算
				//	分配比率が決まったので、これに基づいて分配
				total = alpha + beta;
				if (total != 0){	//	ゼロのときは、当然何もしなくて良い
					if (beta == 0){		//転送先が０なら、そのまま転送
						*lpdw2 = *lpdw;
					}else{
						dwRGB = ((*lpdw & 0xff) * alpha + (*lpdw2 & 0xff) * beta) / total;
						dwRGB += (((*lpdw & 0xff00) * alpha + (*lpdw2 & 0xff00) * beta) / total) & 0xff00;
						dwRGB += (((*lpdw & 0xff0000) * alpha + (*lpdw2 & 0xff0000) * beta) / total) & 0xff0000;
						dwRGB += total << 24;	//	alpha値は、エネルギー総量
						*lpdw = dwRGB;
					}
				}
				lpdw++; lpdw2++;
			}
		}
	}

	//	復帰させる＾＾；　なんぎやなー２（笑）
	m_Font.SetQuality(nQuality);
	m_Font.SetColor(rgb);
	m_Font.SetBackColor(bk);


//----------------------------追加
	// 横→縦変換
	{
		LONG width, height;
		GetSize(width, height);
		ULONG lDstWidth2 = d_dib2.GetWidth();
		for (int y=0;y<sy;y++) {
			DWORD *lpdw = (DWORD*)(GetPtr() + 4*(width - y -1));	// 横書きの上→下は縦書きの右→左
			DWORD *lpdw2 = (DWORD*)(d_dib2.GetPtr() + 4*(lDstWidth2 * y));
			for (int x=0;x<sx;x++) {
				*lpdw = *lpdw2;
				lpdw+=width;	// １つ下へ
				lpdw2++;
			}
		}
	}

	// 影の向きを元に戻す
	m_Font.SetShadowOffset(nOx, nOy);
//----------------------------


//	*GetYGA() = true;	//	for BltNatural

	return 0;
}

//	↑上のコピペした＾＾；
inline LRESULT CTextDIBitmap::UpdateTextVerticalAA(){	//	アンチェリ付き描画
	int sx,sy;
	m_Font.GetSize(sx,sy);
//----------------------------変更
	//	if (CreateSurface(sx,sy)) return 1;
	if (CreateSurface(sy,sx,32,true)) return 1;	// 逆になる
	//　背景色で塗りつぶす
	//	こうしないと縮小したサーフェスをコピーした時にゴミが乗るのだ
	//GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	Clear();
//----------------------------

	COLORREF rgb = m_Font.GetColor();
	COLORREF bk	 = m_Font.GetBackColor();

	m_Font.SetBackColor(CLR_INVALID);
	m_Font.SetColor(RGB(255,255,255));	//	白にして
	int	nQuality = m_Font.GetQuality();
	m_Font.SetQuality(4);	//	アンチェリ無しで書く
	int nFontSize = m_Font.GetSize();
	int nFontHeight = m_Font.GetHeight();
	m_Font.SetSize(nFontSize<<2);	//	4倍サイズで描画
	m_Font.SetHeight(nFontHeight<<2); //　行間も4倍サイズで描画

	CDIBitmap d_dib;
	int sx2,sy2;
	m_Font.GetSize(sx2,sy2);
	// m_Fontのテキストが空でもプレーンを作る．
	if ((sx2 == 0) || (sy2 == 0)) {
		sx2 = sy2 = 1;
	}
	if (d_dib.CreateSurface(sx2,sy2,32,true)) return 2;
	// 背景色で塗りつぶす
	d_dib.GetSurfaceInfo()->SetFillColor(GetSurfaceInfo()->GetFillColor());
	//d_dib.GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	d_dib.Clear();
	m_Font.OnDraw(d_dib.GetDC(),m_nTextX<<2,m_nTextY<<2);	//	4倍のフォントサイズで描画


//----------------------------追加
	// 影の向きを縦書き用に変更
	int nOx, nOy;
	m_Font.GetShadowOffset(nOx, nOy);
	m_Font.SetShadowOffset(nOy, -nOx);

	// 縦書き変換用にもう一つサーフェスを作成
	CDIBitmap d_dib2;
	if (d_dib2.CreateSurface(sx,sy,32,true)) return 2;
	// 背景色で塗りつぶす
	d_dib2.GetSurfaceInfo()->SetFillColor(GetSurfaceInfo()->GetFillColor());
	//d_dib2.GetSurfaceInfo()->GeneralEffect(CSurfaceInfo::eSurfaceFill, NULL);
	d_dib2.Clear();
//----------------------------


	//	４×４→等倍に縮小
	{
	  sx2>>=2; sy2>>=2;
	  if (sx2<sx) { sx = sx2; }	//　なんや？生成されたプレーン／２のほうが小さいで...
	  if (sy2<sy) { sy = sy2; }
	  DWORD dwColor = ((rgb & 0xff) << 16) + (rgb & 0xff00) + ((rgb & 0xff0000)>>16);	//	転置して
	
	  ULONG lDstWidth = d_dib.GetWidth();
	  ULONG lDstWidth2 = d_dib2.GetWidth();
	  for(int y=0;y<sy;y++){
//----------------------------変更
	//	DWORD *lpdw = GetPtr() + (GetRect()->right * y);
		DWORD *lpdw = (DWORD*)(d_dib2.GetPtr() + (4*lDstWidth2 * y));	// 縦書き変換用のものを使う
//----------------------------
		DWORD *lpdw2a = (DWORD*)(d_dib.GetPtr() + (4*lDstWidth * (y*4	 )));
		DWORD *lpdw2b = (DWORD*)(lpdw2a		   +  lDstWidth); // 次ライン
		DWORD *lpdw2c = (DWORD*)(lpdw2b		   +  lDstWidth); // 次ライン
		DWORD *lpdw2d = (DWORD*)(lpdw2c		   +  lDstWidth); // 次ライン
		//	得られた明るさをαチャンネルに持っていく
		for(int x=0;x<sx;x++){
			DWORD dwAlpha;
			dwAlpha =	((*lpdw2a)&0xff) + ((*(lpdw2a+1))&0xff) + ((*(lpdw2a+2))&0xff) + ((*(lpdw2a+3))&0xff);
			dwAlpha +=	((*lpdw2b)&0xff) + ((*(lpdw2b+1))&0xff) + ((*(lpdw2b+2))&0xff) + ((*(lpdw2b+3))&0xff);
			dwAlpha +=	((*lpdw2c)&0xff) + ((*(lpdw2c+1))&0xff) + ((*(lpdw2c+2))&0xff) + ((*(lpdw2c+3))&0xff);
			dwAlpha +=	((*lpdw2d)&0xff) + ((*(lpdw2d+1))&0xff) + ((*(lpdw2d+2))&0xff) + ((*(lpdw2d+3))&0xff);
			*lpdw = ((dwAlpha << (24-4)) & 0xff000000) + dwColor;	//	色は、rgbで指定された色
			lpdw++; lpdw2a+=4; lpdw2b+=4; lpdw2c+=4; lpdw2d+=4;
		}
	  }
	}

	//	復帰させる＾＾；　なんぎやなー（笑）
	m_Font.SetSize(nFontSize);
	m_Font.SetHeight(nFontHeight);
	//	↑ここで戻さないと、文字の影を生成するときに嘘のフォントサイズになっちまう＾＾；

	//	文字の影
	if (bk!=CLR_INVALID) {
	//	影つき文字やんか．．．やらしいなぁ＾＾；
		CTextDIBitmap dib;
		*dib.GetFont() = *GetFont();	//	フォントまるごとコピーしてまえー！！＾＾；
		dib.GetFont()->SetColor(bk);
		dib.GetFont()->SetBackColor(CLR_INVALID);	//	これをCLR_INVALIDにしておけば、この再帰は２度目で停止する
//		dib.GetFont()->SetText(m_Font.GetText());
//		dib.SetTextPos(2,2);	//	ずらして描画！
		{
			int nOx, nOy;
			dib.GetFont()->GetShadowOffset(nOx,nOy);
			dib.SetTextPos(nOx,nOy); // ずらして描画！
		}
		dib.UpdateTextAA();

		//	PhotoShopのレイヤ的不透明度付き合成
		int alpha;
		int beta;
		int total;
		DWORD dwRGB;

		//	dibにあらたに描画させたやつとサイズが異なるかも知れない．．

		ULONG lDstWidth = dib.GetWidth();
		ULONG lDstWidth2 = d_dib2.GetWidth();
		for(int y=0;y<sy;y++){
//----------------------------変更
		//	DWORD *lpdw = GetPtr() + (GetRect()->right * y);
			DWORD *lpdw = (DWORD*)(d_dib2.GetPtr() + (4*lDstWidth2 * y));	// 縦書き変換用のものを使う
//----------------------------
			DWORD *lpdw2 = (DWORD*)(dib.GetPtr() + (4*lDstWidth * y));
			//	得られた明るさをαチャンネルに持っていく
			for(int x=0;x<sx;x++){
				/*
				このアルゴリズムについては、やねうらおのホームページの
				スーパープログラマーへの道　「第CB回　PhotoShopのレイヤ合成アルゴリズムについて（YGAフォーマットを提唱する) 00/10/18」
				を参照のこと
				*/
				alpha = *lpdw >> 24;
				beta  = *lpdw2 >> 24;
				beta  = (255-alpha)*beta / 255;	//	エネルギー残量の計算
				//	分配比率が決まったので、これに基づいて分配
				total = alpha + beta;
				if (total != 0){	//	ゼロのときは、当然何もしなくて良い
					if (beta == 0){		//転送先が０なら、そのまま転送
						*lpdw2 = *lpdw;
					}else{
						dwRGB = ((*lpdw & 0xff) * alpha + (*lpdw2 & 0xff) * beta) / total;
						dwRGB += (((*lpdw & 0xff00) * alpha + (*lpdw2 & 0xff00) * beta) / total) & 0xff00;
						dwRGB += (((*lpdw & 0xff0000) * alpha + (*lpdw2 & 0xff0000) * beta) / total) & 0xff0000;
						dwRGB += total << 24;	//	alpha値は、エネルギー総量
						*lpdw = dwRGB;
					}
				}
				lpdw++; lpdw2++;
			}
		}
	}

	//	復帰させる＾＾；　なんぎやなー２（笑）
	m_Font.SetQuality(nQuality);
	m_Font.SetColor(rgb);
	m_Font.SetBackColor(bk);


//----------------------------追加
	// 横→縦変換
	{
		LONG width, height;
		GetSize(width, height);

		ULONG lDstWidth2 = d_dib2.GetWidth();

		for (int y=0;y<sy;y++) {
			DWORD *lpdw = (DWORD*)(GetPtr() + 4*(width - y -1));	// 横書きの上→下は縦書きの右→左
			DWORD *lpdw2 = (DWORD*)(d_dib2.GetPtr() + 4*(lDstWidth2 * y));
			for (int x=0;x<sx;x++) {
				*lpdw = *lpdw2;
				lpdw+=width;	// １つ下へ
				lpdw2++;
			}
		}
	}

	// 影の向きを元に戻す
	m_Font.SetShadowOffset(nOx, nOy);
//----------------------------


//	*GetYGA() = true;	//	for BltNatural

	return 0;
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
