//
//	yaneTextFastPlane.h :
//
#ifdef USE_FastDraw

#ifndef __yaneTextFastPlane_h__
#define __yaneTextFastPlane_h__


#include "yaneFont.h"
#include "yaneFastPlane.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CTextFastPlane : public CFastPlane {
public:

	CFont*	GetFont() { return &m_Font; }
	void	SetTextPos(int x,int y){ m_nTextX=x; m_nTextY=y; }
	LRESULT	UpdateText();
	LRESULT	UpdateTextA();	//	アンチェリかけるバージョン
										//	→α値を持つようになるので描画はBlendBltFastAlphaで行なう
	LRESULT	UpdateTextAA(); //	精細なアンチェリをかけるバージョン
										//	UpdateTextAAよりずいぶん遅い＾＾；；
	void	SetVertical(bool bVertical = true) { m_bVertical = bVertical; }
	bool	IsVertical() { return m_bVertical; }
	CTextFastPlane(CFastDraw* pFastDraw=NULL);
	virtual ~CTextFastPlane() {};	//	plane holder

protected:
	CFont		m_Font;				//	こいつで描画
	int			m_nTextX,m_nTextY;	//	テキスト描画位置
	bool		m_bVertical;		//　縦書きかどうか

	virtual		LRESULT OnDraw();	//	リストア用関数
	int			m_nUpdateType;		//	リストアするときの種類
									//	0:リストアしない 1:UpdateText 2:UpdateTextA 3:UpdateTextAA
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif	//	__yaneTextFastPlane_h__ 

#endif // USE_FastDraw
