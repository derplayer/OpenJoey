#include "stdafx.h"

#include "yaneFont.h"

//////////////////////////////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Draw {

CFont::CFont(){
	SetSize(16);
	SetHeight(20);
	SetFont(0);	// MS ゴシック
//	SetColor(RGB(128,192,128));
//	SetBackColor(RGB(64,128,64));
	SetColor(RGB(255,255,255));
	SetBackColor(RGB(128,128,128));
	SetBGColor(CLR_INVALID);
	SetQuality(2);
	SetVerticalFont(false);
	SetWeight(FW_LIGHT);
	SetItalic(false);
	SetUnderLine(false);
	SetStrikeOut(false);
	SetShadowOffset(2,2);
}

CFont::~CFont(){
}

//////////////////////////////////////////////////////////////////////////////

void	CFont::SetQuality(int nQuality){
	switch(nQuality) {
	case 0: m_nQuality = DEFAULT_QUALITY; break;
	case 1: m_nQuality = DRAFT_QUALITY; break;
	case 2: m_nQuality = PROOF_QUALITY; break;
	case 3: m_nQuality = ANTIALIASED_QUALITY; break;
	case 4: m_nQuality = NONANTIALIASED_QUALITY; break;
	default: WARNING(true,"CFont::SetQuality 規定外の数値");
	}
}

void	CFont::SetSize(int nSize){
	m_nSize = nSize;
	m_nHeight = nSize;	//	こいつも更新する価値がある？
}

void	CFont::SetColor(COLORREF rgb){
	m_nRgb = rgb;
}

void	CFont::SetBackColor(COLORREF rgb){
	m_nBkRgb	= rgb;
}

void	CFont::SetBGColor(COLORREF rgb){
	m_nBGRgb	= rgb;
}

void	CFont::SetHeight(int nHeight){
	m_nHeight = nHeight;
}

void	CFont::SetWeight(int nWeight){
	m_nWeight = nWeight;
}

////////////////////////////////////////////////////////////////////
//	font設定～取得

void	CFont::SetFont(int nFontNo){
	string name;
	switch (nFontNo) {
	case 0: name ="ＭＳ ゴシック"; break;
	case 1: name ="ＭＳ Ｐゴシック"; break;
	case 2: name ="ＭＳ 明朝"; break;
	case 3: name ="ＭＳ Ｐ明朝"; break;
	default: name="";
	}
	if (m_FontName==name) return ;
	m_FontName=name;
}
void	CFont::SetFont(const string& fontname){
	if (m_FontName==fontname) return ;
	m_FontName=fontname;
}

string	CFont::GetFont() const {
	if (!IsVerticalFont()){
	} else {
		if (!m_FontName.empty() && m_FontName[0]=='@'){
			//	最初から縦書きフォントなので何もしない
		} else {
			return '@' + m_FontName;
		}
	}
	return m_FontName;
}

////////////////////////////////////////////////////////////////////

void	CFont::SetText(const string& s){
	m_String = s;
}

void	__cdecl CFont::SetText(LPSTR fmt, ... ){
	CHAR buf[512];
	wvsprintf(buf,fmt,(LPSTR)(&fmt+1));
	SetText((string)buf);
}

void CFont::SetText(int i){
	CHAR buf[16];
	wsprintf(buf,"%d",i);
	SetText(buf);
}

void	CFont::SetItalic(bool b){
	m_bItalic = b;
}

void	CFont::SetUnderLine(bool b){
	m_bUnderLine = b;
}

void	CFont::SetStrikeOut(bool b){
	m_bStrikeOut = b;
}

void	CFont::SetShadowOffset(int nOx,int nOy){
	m_nShadowOffsetX = nOx;
	m_nShadowOffsetY = nOy;
}

/////////////////////////////////////////////////////////////////////////////////////////

//	drawing

void	CFont::OnDraw(HDC hdc,int x,int y){

	//	文字列が設定されていなければ帰る
	if (m_String.empty()) return ;

	string strFontName = GetFont();
	HFONT hFont = ::CreateFont(m_nSize,0,0,0,m_nWeight,m_bItalic,m_bUnderLine,m_bStrikeOut,SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, m_nQuality,FF_MODERN,strFontName.c_str());

	if (hFont==NULL) return ; // メモリ足りんのだがや！？

	HFONT hFontLast = (HFONT)::SelectObject(hdc,hFont);

	//	もし背景色付きで描画するならば、
	//	最初の一回はopaque,二回目以降がtransparent,
	bool bFirst = true;

	if (m_nBkRgb!=CLR_INVALID){
		//	これ、重ねる価値がなさそう．．
		POINT point[5] = {
			{m_nShadowOffsetX,m_nShadowOffsetY},		// 1
//			{2,2},		// 1
			{0,1},
			{2,1},		// 2
			{1,2},
			{2,2}		// 3
		};

		::SetTextColor(hdc,m_nBkRgb);
		//	色の変更

		//	背景色の設定
		if (m_nBGRgb==CLR_INVALID) {
			::SetBkMode(hdc, TRANSPARENT);
		} else {
			::SetBkMode(hdc, OPAQUE);
			::SetBkColor(hdc,m_nBGRgb);
		}
		bFirst = false;

		for(int i=0;i<1 /* 5 */;i++){
			TextOut(hdc,x + point[i].x,y + point[i].y,m_String);
			//	文字の表示（WIN32APIではなく、このクラスのstatic関数であることに注意）
		}
	}
	if (m_nRgb!=CLR_INVALID){
		//	背景色の設定
		if (bFirst){
			if (m_nBGRgb==CLR_INVALID) {
				::SetBkMode(hdc, TRANSPARENT);
			} else {
				::SetBkMode(hdc, OPAQUE);
				::SetBkColor(hdc,m_nBGRgb);
			}
		} else if (m_nBGRgb!=CLR_INVALID) {
			//	２回目以降の描画なので透過設定に戻しておかなくては．．
			::SetBkMode(hdc, TRANSPARENT);
		}

		::SetTextColor(hdc,m_nRgb);
		TextOut(hdc,x,y,m_String);	//	文字の表示
	}
	::SelectObject(hdc,hFontLast);
	::DeleteObject(hFont);		// これ一回でええんか？
}

LRESULT	CFont::GetSize(int& sx,int& sy){
	sx = 0; sy = 0;	// fail safe対策

	//	文字列が設定されていなければ帰る
	if (m_String.empty()) return 1;

	string strFontName = GetFont();
	HFONT hFont = ::CreateFont(m_nSize,0,0,0,m_nWeight,m_bItalic,m_bUnderLine,m_bStrikeOut,SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, m_nQuality,FF_MODERN,strFontName.c_str());
	if (hFont==NULL) return 1; // あじゃー

	SIZE size;
	HDC hdc = ::CreateCompatibleDC(NULL);
	HFONT hFontLast = (HFONT)::SelectObject(hdc,hFont);

	LPCSTR p = m_String.c_str();
	CHAR buf[256];	//	この長さまでしか計測不可＾＾；
	LPSTR q=buf;
	size.cx=0; size.cy=0;
	SIZE size2;
	for(;;){
		*q = *(p++);
		if (*q=='\n') {
			*q=0;	// 改行コードならば潰す 
			::GetTextExtentPoint32(hdc,buf,::lstrlen(buf),&size2);
			//GetTextExtentPointは特定の状況で１ドットずれが発生するそうな^^;
//			size.cy+=size2.cy;
			size.cy+=m_nHeight;	//	fixed by あきら
			if (m_bItalic) size2.cx+=m_nSize/4;	//	APIが斜体のサイズ測定を誤るbug対策
			if (size.cx<size2.cx) size.cx=size2.cx;
			q=buf;
			continue;	//	このときはqを加算されては困る
		} else if (*q=='\0') {
			::GetTextExtentPoint32(hdc,buf,::lstrlen(buf),&size2);
			//GetTextExtentPointは特定の状況で１ドットずれが発生するそうな^^;
			size.cy+=size2.cy;
			if (m_bItalic) size2.cx+=m_nSize/4;	//	APIが斜体のサイズ測定を誤るbug対策
			if (size.cx<size2.cx) size.cx=size2.cx;
			break;	// これにて終了～
		}
		q++;
	}
	sx = size.cx + 3;	//	文字には影があるので
	sy = size.cy + 3;	//

	::SelectObject(hdc,hFontLast);
	::DeleteObject(hFont);	// これ一回でええんか？

	::DeleteDC(hdc);	//	これ最後にせんとあかんで！(by TearDrop_Stone)

	return 0;
}

void	CFont::TextOut(HDC hdc,int x,int y,const string& s){

	//	文字列が設定されていなければ帰る
	if (m_String.empty()) return ;

	//	最大の利点は、文字列中に\nを埋め込むことで改行が出来ることである。

	CHAR buf[256];
	LPCSTR p = s.c_str();
	LPSTR q=buf;
	for(;;){
		*q = *(p++);
		if (*q=='\n') {
			*q=0;	// 改行コードならば潰す 
			::TextOut(hdc,x,y,buf,::lstrlen(buf));
			y+=m_nHeight;
			q=buf;
			continue;	//	このときはqを加算されては困る
		} else if (*q=='\0') {
			::TextOut(hdc,x,y,buf,::lstrlen(buf));
			break;	// これにて終了～
		}
		q++;
	}
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
