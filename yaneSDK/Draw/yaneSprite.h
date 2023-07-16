//	yaneChara.h :
//
//		Simple Sprite Manager
//		programmed by yaneurao '00/07/20
//		rewritten by yaneurao '03/06/18
//

#ifndef __yaneSprite_h__
#define __yaneSprite_h__

namespace yaneuraoGameSDK3rd {
namespace Draw {

class ISurface;

struct CSimpleSprite {
/**
	１枚のスプライトを表現する
	転送元サーフェースと、転送元矩形の情報しかない。
*/
public:
	ISurface*	pSurface;	///	転送元プレーン
	RECT		rcRect;		///	転送元矩形
	// ---------- 使いたければ以下のを使ってもいい ----------
	int			nOx,nOy;	///	転送オフセット
	int			nHeight;	///	ベースライン算出用
	bool		bFast;		///	抜き色無し転送か？

	//	---補助
	void		set(ISurface*p);
	///	サーフェース全域を一つのこのスプライトとする
	///	nOx=nOy=0,bFast=true,nHeight=sy

	void		set(ISurface*p,const RECT&rc);
	///	サーフェースの一部をこのスプライトとする
	///	nOx=nOy=0,bFast=true,nHeight=sy

};

class CSprite {
/**
	CSimpleSpriteのvectorのvectorをメンバに持つ、スプライトクラス

//		スプライトとは、「プレーンにおける矩形」の集合として
//		定義できる論理構造であって、描画とは切り離して考えるべき
//
//		よって、このクラスは描画に関することを行なわない

*/
public:
	///	スプライトの実体
	typedef vector<CSimpleSprite> spritevector;
	typedef vector<spritevector> spritevectorofvector;

	///	スプライト実体の取得
	spritevectorofvector* getAllSprite() { return m_vSprtieVector.get(); }
	spritevector* getDirectionalSprite()
		{ return &((*getAllSprite())[getDirection()]); }
	CSimpleSprite* getSprite()
		{ return &((*getDirectionalSprite())[getMotion()]); }

	///	スマートポインタでの取得/設定(CSpriteCharaから取得して設定するべし)
	smart_ptr<spritevectorofvector> getMySprite() { return m_vSprtieVector; }
	void	setMySprite(const smart_ptr<spritevectorofvector>& v)
		{ m_vSprtieVector = v; 	m_nDirection = m_nAnimation	= 0;}
	/**
		(*getAllSprite())[getDirection()][getMotion()]
		を画面に描画するのである。

		m_nAnimation (getMotionで取得できる)はアニメーションカウンタなので
		毎フレーム自動的に加算される
		m_nDirection (getDirectionで取得できる)は、キャラクタの向きを変更する。
		（向きごとに一連のアニメーションが定義されていると考える)

		上記の表現は、
		(*getDirectionalSprite())[m_nAnimation]
		や、単に
		*getSprite()
		と書ける。これは、現在表示している（表示する）スプライト情報を
		取得するものである。

		setMySpriteしたとき、setDirection(0); setMotion(0); したことになる
	*/


	///	スプライト向きの設定/取得
	/**
		注:setDirectionでは、方向の変更が無い場合はアニメーションカウンタを
		リセットしない。方向の変更がある場合は、アニメーションカウンタを
		0にする。
	*/
	void	setDirection(int nDirection);
	int		getDirection() const { return m_nDirection; }

	///　追加オフセット量を設定/取得する（初期値(0,0)）
	void	setOffset(int nOffsetX,int nOffsetY);
	void	getOffset(int& nOffsetX,int& nOffsetY);

	///	現在のアニメーション番号を設定/取得/インクリメントする
	void	setMotion(int n) { m_nAnimation=n; }
	int		getMotion() const { return m_nAnimation; }
	void	incMotion();
	///	アニメーションが終端にまで達したか?
	bool	isEnd() const
	{ return (int)const_cast<CSprite*>(this)->getDirectionalSprite()->size()
		== m_nAnimation; }

	///	スプライトの表示座標を設定する
	void	setPos(int x,int y) { m_nX = x; m_nY = y; }
	void	getPos(int&x,int&y) const { x=m_nX; y=m_nY; }

	///	高さの設定(この情報を使おうが使うまいが構わない)
	void	setHeight(int nHeight) { m_nHeight=nHeight; }
	int		getHeight() const { return m_nHeight; }

	///	スプライトを有効にする/有効かどうかの状態を取得する
	///	(この情報を使おうが使うまいが構わない)
	void	enable(bool bEnable) { m_bVisible = bEnable; }
	bool	isEnable() const { return m_bVisible; }

	///	スプライトのプライオリティを設定する
	///	(この情報を使おうが使うまいが構わない)
	void	setPriority(int nPriority) { m_nPriority = nPriority; }
	int		getPriority() const { return m_nPriority;}

	CSprite();
	virtual ~CSprite();

protected:
	smart_ptr<spritevectorofvector>		m_vSprtieVector;
	int		m_nDirection;		//	向いている方向
	int		m_nAnimation;		//	アニメーションカウンタ
	/**
		m_vSprtieVector[m_nDirection][m_nAnimation]
		を画面に描画すると考える。

		m_nAnimationはアニメーションカウンタなので
		毎フレーム自動的に加算される

		m_nDirectionは、キャラクタの向きを変更する。
		（向きごとに一連のアニメーションが定義されていると考える)
	*/

	//	以下のメンバはディフォルトで0
	int		m_nX,m_nY;			//	表示位置
	//	以下は使いたくなければ使わなくて良い
	int		m_nOx,m_nOy;		//	オフセット座標
	int		m_nHeight;			//	高さ(キャラのベースライン算出のため)
	bool	m_bVisible;			//	表示・非表示
	int		m_nPriority;		//	描画プライオリティ
};

struct CSpriteEx : public CSprite {
/**
	描画機能つきのスプライトクラス
	必要に応じて、このクラスを派生させてメンバを足すか、
	CSpirteから別に派生させるかして使ってくんろ
*/
public:

	void	Blt(ISurface*pSurface,LPRECT lpClip=NULL);
	//	通常描画(アニメーションカウント加算される)

	void	Blt(ISurface*pSurface,int x,int y,LPRECT lpClip=NULL);
	//	座標指定つき描画(アニメーションカウント加算される)

	void	BltFix(ISurface*pSurface,LPRECT lpClip=NULL);
	//	アニメーションカウントは加算されない描画

	void	BltFix(ISurface*pSurface,int x,int y,LPRECT lpClip=NULL);
	//	座標指定つき描画(アニメーションカウントは加算されない)

	void	BltOnce(ISurface*pSurface,int x,int y,LPRECT lpClip=NULL);
	//	アニメーションカウントが最後になっていたらそれ以上は加算しない描画
	//	(それ以外の場合にはアニメーションカウントが加算される)
};

class CSpriteChara {
///	ファイルからスプライト定義を読み込むためのローダー
///	(yaneSDK2ndのCSpriteChara互換)
public:

	///	キャラパターン定義ファイルをテキストファイルから読み込む/解放する
	LRESULT	Load(const string& strFileName);
	void	Release();

	/**
		Loadするときに生成するサーフェースは
		CPlaneに設定してあるfactoryを用いる
	*/

	///	Loadで読み込んだスプライト定義を取得する
	///	(ここで得たものをCSprite::setMySpriteに渡すと良い)
	smart_ptr<CSprite::spritevectorofvector> getMySprite()
		{ return m_vSpriteVector; }
	void	setMySprite(const smart_ptr<CSprite::spritevectorofvector>& v)
		{ m_vSpriteVector = v; }

protected:

	///	Loadで読み込んだときのサーフェース
	smart_vector_ptr<ISurface>	m_apSurface;

	///	展開したスプライト実体
	smart_ptr<CSprite::spritevectorofvector> m_vSpriteVector;
};

}} // end of namespace

#endif
