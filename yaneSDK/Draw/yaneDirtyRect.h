//
//	Dirty Rect (更新矩形)　管理クラス
//
//		programmed and designed by yaneurao '02/01/16

#ifndef __yaneDirtyRect_h__
#define __yaneDirtyRect_h__


namespace yaneuraoGameSDK3rd {
namespace Draw {

class IDirtyRect {
public:
	virtual void	Clear()=0;
	virtual void	Refresh()=0;
	virtual void	AddRect(int left,int top,int right,int bottom)=0;
	virtual void	AddRectSize(int x,int y,int sx,int sy)=0;
	virtual void	AddRect(const RECT& rc)=0;
	virtual int		GetRectNum() const=0;
	virtual LPCRECT	GetRect() const=0;

	virtual ~IDirtyRect(){}
};

class CDirtyRect : public IDirtyRect {
/**
	使いかた。
		Clearでクリア。
		AddRectなんとかで、矩形を加算していく
		（描画ごとに幾度となく繰り返す）
		そのとき、このクラスの内部で、最大更新矩形(dirty rect)と
		なるように結合されていく。ただし、画面左上の少しと
		右下の少しを更新しただけなのに画面全体が最大更新矩形(dirty rect)
		として選択されてしまうのはまずいので、そのようなケースにそなえて
		最大5つのdirty rectを持っても良いことにする。

		最後、Refreshを呼び出すことにより、重複する（あるいは完全に含まれる）
		dirty rectを消去する。

		そして、GetRectNum,GetRectで、dirty rectの数と矩形が取得できる。
*/
public:

	///	すべてをクリアする
	void	Clear(){ m_nRectLast = 0; }

	///	リフレッシュする
	void	Refresh();

	///	矩形の加算
	///		範囲指定タイプ
	void	AddRect(int left,int top,int right,int bottom);
	///		座標＋大きさ指定タイプ
	void	AddRectSize(int x,int y,int sx,int sy)
	{	AddRect(x,y,x+sx,y+sy);}
	///		矩形で加算
	void	AddRect(const RECT& rc)
	{	AddRect(rc.left,rc.top,rc.right,rc.bottom); }

	//	--- property

	///	結果の取得
	///		生成された矩形合計数(0:なし)
	int		GetRectNum() const { return m_nRectLast; }
	///		生成された矩形へのポインタを取得
	LPCRECT	GetRect() const { return &m_arcRect[0]; }

	//	-----

	CDirtyRect() { Clear(); }

protected:
	enum	{ nRectSize = 5 };
	//	この数だけ矩形を確保
	//	まあ、あまり細かくとると、ガーベジの効率が落ちるので
	//	5個ぐらいにしておく。

	RECT	m_arcRect[nRectSize+1];	   //	矩形情報
	int		m_anRectArea[nRectSize+1]; //	矩形面積
	int		m_nRectLast;	//	↑をいくつまで使っているかのカウンタ
							//	初期状態==0 : 未使用
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
