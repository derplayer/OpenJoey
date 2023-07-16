//	yaneFont.h :
//
//		Font Wrapper
//

#ifndef __yaneFont_h__
#define __yaneFont_h__


namespace yaneuraoGameSDK3rd {
namespace Draw {

class CFont {
/**
	文字フォントのためのクラス
		class CTextDIBitmap
		class CTextFastPlane
	も参考にすること
*/
public:

	///	---- テキストの設定と描画

	void	SetText(const string& s);
	/// 表示文字列の指定（\nで改行が使用できる）

	void	__cdecl SetText(LPSTR fmt, ... );
	///	↑と同じ。printf出力書式が使える。
	///	ただしbufは512文字まで

	///	数字を表示文字列として指定
	void	SetText(int i);

	///	表示文字列の取得
	string	GetText() { return m_String; }

	/**
		描画サイズを得る
		このメソッドは、プロパティ設定後に呼び出すと良い
	*/
	LRESULT	GetSize(int& sx,int& sy);

	///	HDCを渡して、そこに描画してもらう
	void	OnDraw(HDC hdc,int x=0,int y=0);

	///	------	property	（説明の右にある括弧数字は、ディフォルト値）

	///	フォントクオリティ				(0）
	void	SetQuality(int nQuality);
	int		GetQuality() const { return m_nQuality; }

	/// Fontサイズをpt数(ポイント数)で設定/取得			(16)
	void	SetSize(int nSize);
	int		GetSize() const { return m_nSize; }

	/// テキストカラー設定／取得			(255,255,255)
	void	SetColor(COLORREF rgb);
	COLORREF GetColor() const { return m_nRgb; }

	/// 文字の影つけ						(64,64,64)
	void	SetBackColor(COLORREF rgb);
	COLORREF GetBackColor() const { return m_nBkRgb; }

	/// 背景色							(CLR_INVALID)
	void	SetBGColor(COLORREF rgb);
	COLORREF GetBGColor() const { return m_nBGRgb; }

	/// 次行までの行間					(20)
	void	SetHeight(int nHeight);
	int		GetHeight() const { return m_nHeight; }

	/// フォントセレクト（番号で）		(0) : MSゴシック
	void	SetFont(int nFontNo);

	/// フォントセレクト（フォント名で）
	void	SetFont(const string& fontname);
	/**
		ただし、縦書きフォント(@)は、これで設定するのではなく、
		SetVerticalFontメソッドで縦書き設定する
	*/

	/**
		設定されているフォント名の取得
		ただし、SetVerticalFont(true)で、縦書きフォントが
		選択されている場合は、フォント名の先頭に '@' を
		付与したものを返す
	*/
	string	GetFont() const;

	/// 文字の太さ						(300:FW_LIGHT) , FW_BOLDは700
	void	SetWeight(int nWeight);

	/// 斜体								(false)
	void	SetItalic(bool b);

	/// 下線の有無						(false)
	void	SetUnderLine(bool b);

	/// 打ち消し線の有無					(false)
	void	SetStrikeOut(bool b);

	/// 影のオフセット				(2,2)
	void	SetShadowOffset(int nOx,int nOy);
	void	GetShadowOffset(int &nOx,int &nOy) const { nOx=m_nShadowOffsetX; nOy=m_nShadowOffsetY; }

	/**
		縦書きフォントを設定(default:false)
		@ を先頭に追加する
	*/
	void	SetVerticalFont(bool b) { m_bVerticalFont = b; }
	bool	IsVerticalFont() const { return m_bVerticalFont; }

	CFont();
	virtual ~CFont();

protected:
	string	m_String;			//	表示する文字列
	int		m_nQuality;			//	フォントクオリティ
	COLORREF m_nRgb;			//	文字色
	COLORREF m_nBkRgb;			//	文字の影
	COLORREF m_nBGRgb;			//	背景色
	int		m_nSize;			//	選択されているフォントサイズ
	string	m_FontName;			//	選択されているフォント名
	int		m_nHeight;			//	次の行までの距離
	int		m_nWeight;			//	文字の太さ
	bool	m_bItalic;			//	斜体
	bool	m_bUnderLine;		//	下線
	bool	m_bStrikeOut;		//	打ち消し線
	bool	m_bVerticalFont;	//	縦書きフォントの選択

	int		m_nShadowOffsetX;	//	影のオフセットX座標
	int		m_nShadowOffsetY;	//	影のオフセットY座標

	//	Auxiliary
	void	TextOut(HDC hdc,int x,int y,const string& s);
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
