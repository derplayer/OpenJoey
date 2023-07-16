//
//	yaneTextDIBitmap.h :
//

#ifndef __yaneTextDIBitmap_h__
#define __yaneTextDIBitmap_h__


#include "yaneFont.h"
#include "yaneDIBitmap.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CTextDIBitmap : public CDIBitmap {
public:

	CFont*	GetFont() { return &m_Font; }
	void	SetTextPos(int x,int y){ m_nTextX=x; m_nTextY=y; }
	void	GetTextPos(int&x,int&y){ x=m_nTextX; y=m_nTextY; }

	/// 縦書きの設定／取得
	void	SetVertical(bool bVertical = true);
	bool	IsVertical() const { return m_bVertical; }

	LRESULT	UpdateText();

	LRESULT	UpdateTextA();	///	アンチェリかけるバージョン
	///	→α値を持つようになるので描画はISurface::BlendBltFastAlphaで行なう
	/**
		注意:CDIBitmapはISurface派生クラスではないので
		実際はCSurfaceInfoのGeneralBltで転送しなくてはならない
	*/

	LRESULT	UpdateTextAA();	//	アンチェリかけるバージョン
	///	→α値を持つようになるので描画はISurface::BlendBltFastAlphaで行なう

	CTextDIBitmap();
	virtual ~CTextDIBitmap() {};	//	plane holder

protected:
	LRESULT UpdateTextHorizontal();		//　横書き UpdateText
	LRESULT UpdateTextHorizontalA();	//　横書き UpdateTextA
	LRESULT UpdateTextHorizontalAA();	//　横書き UpdateTextA
	LRESULT UpdateTextVertical();		//　縦書き UpdateText
	LRESULT UpdateTextVerticalA();		//　縦書き UpdateTextA
	LRESULT UpdateTextVerticalAA();		//　縦書き UpdateTextA

	CFont		m_Font;				//	こいつで描画
	int			m_nTextX,m_nTextY;	//	テキスト描画位置
	bool		m_bVertical;		//　縦書きかどうか
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
