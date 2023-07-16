// yaneLayer.h

#ifndef __yaneLayer_h__
#define __yaneLayer_h__


#if defined(yaneSDK_CodeWarrior_Mode)
#include "yaneSurface.h" //	クラスの定義ファイルをincludeしてやらないとだめ
#endif

namespace yaneuraoGameSDK3rd {
namespace Draw {

class ISurface;
class ILayerList;

class ILayerBase {
/**
	レイヤの基底クラス。
	これを、描画ユニット(CFastDraw等)のレイヤコールバックに登録して使用する。

*/
public:
	///		描画するコールバック(以下のどちらかをオーバーライドする)
	virtual	void	OnDraw(ISurface*lpDraw) {}
	virtual	void	OnDraw(HDC) {}

	///		ポジション指定/取得
	void	SetPos(int x,int y)	 { m_nX = x; m_nY = y; }
	void	GetPos(int& x,int& y) const { x = m_nX; y = m_nY; }

	///		描画を有効／無効化する(ディフォルトで有効)
	void	Enable(bool bEnable) { m_bEnable = bEnable; }
	bool	IsEnable() const { return m_bEnable; }

	///		LayerListをコンストラクタで渡せば、自動的にフックする
	ILayerBase(ILayerList*pLayerList=NULL);

	void	SetLayerList(ILayerList*pLayerList);
	/**
		LayerListを設定する。(その瞬間にフックする)
	*/

	///		デストラクタで、LayerListからフックしていたのを外す
	virtual ~ILayerBase();

protected:
	int		m_nX,m_nY;	//	オフセット座標
	bool	m_bEnable;

	ILayerList* m_pLayerList;
	/*
		自分がフックしているレイヤリストを保持しておき、デストラクタで
		フックを解除する
	*/
};

class ILayerList {
public:
	virtual void	Add(ILayerBase*)=0;
	virtual void	Del(ILayerBase*)=0;
	virtual void	Clear()=0;
	virtual bool	IsEmpty() const=0;
	
	virtual void	OnDraw(ISurface*)=0;
	virtual void	OnDraw(HDC)=0;

	virtual ~ILayerList(){}
};

class CLayerList : public ILayerList {
public:
	virtual void	Add(ILayerBase*);			///	自分自身をフックに加える
	virtual void	Del(ILayerBase*);			///	自分自身をフックから外す
	virtual void	Clear();					///	すべてをクリアする
	virtual bool	IsEmpty() const;			///	空であるか？

	///	自分の保持しているフックリストに対してOnDrawを呼び出す
	virtual void	OnDraw(ISurface*);
	virtual void	OnDraw(HDC);

protected:
	list_chain<ILayerBase>	m_listLayer;	//	フックリスト
	list_chain<ILayerBase>* GetList() { return& m_listLayer; }
};

////////////////////////////////////////////////////////////////////////////

class CSurfaceLayer : public ILayerBase {
/**
	ISurface*を保持していて、OnDraw(ISurface)において、
	それをBltNaturalで描画するだけのレイヤ
*/
public:

	CSurfaceLayer(ISurface* pSurface,ILayerList*pLayerList=NULL)
		: m_pSurface(pSurface) , ILayerBase(pLayerList) {}
	CSurfaceLayer(const smart_ptr<ISurface>& pSurface,ILayerList*pLayerList=NULL)
		: m_pSurface(pSurface),ILayerBase(pLayerList) {}

protected:
	virtual void	OnDraw(ISurface*);
	smart_ptr<ISurface>	m_pSurface;
};

}}

#include "yaneFont.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CTextLayer : public ILayerBase {
/**
	CFontを持って、OnDrawにおいて、CFont::OnDrawを呼び出して描画する
*/
public:
	///	これでフォントを取得して設定を行なうこと
	CFont* GetFont() { return& m_vFont; }

	CTextLayer(ILayerList*pLayerList=NULL):ILayerBase(pLayerList){}
	///	HDC経由で描画しているので、HDCLayerにフックさせること！

protected:
	virtual void	OnDraw(HDC);
	CFont	m_vFont;
};

} namespace Timer { class CFPSTimer; } namespace Draw {

class CFPSLayer : public CTextLayer {
public:

	CFPSLayer(CFPSTimer*pFPSTimer,ILayerList*pLayerList=NULL);

	CFPSTimer* GetFPSTimer() { return m_pFPSTimer; }

protected:
	virtual void	OnDraw(HDC);
	CFPSTimer* m_pFPSTimer;
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
