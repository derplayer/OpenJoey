//
//	『Revolution』	2000/07	 Waffle 
//		programmed by yaneurao(M.Isozaki)
//

#ifndef __CMonologueView_h__
#define __CMonologueView_h__

#include "CApp.h"			//	メインアプリ

class CMonologueView {
public:
	//	モノローグ画面描画
	virtual LRESULT OnDraw(void);

	//	読み込み
	virtual	LRESULT Load(LPSTR lpszFile);

	bool	IsEof(void) { return m_bEof; }

	CMonologueView(CApp* lpApp);
	virtual ~CMonologueView();

protected:
	CApp*	GetApp(void) { return m_lpApp; }
	CApp*	m_lpApp;
	bool	m_bEof;
	bool	m_bFadeIn;			//	フェードイン中？
	bool	m_bFadeOut;			//	フェードアウト中？
	CTimer	m_Timer;
	void	GetNextMessage(void);
	virtual	void OnDrawBG(int nFadeLevel=256);
	bool	m_bBlack;
	CFile	m_File;
	CSound	m_BGMSound;

	CPlane		m_BGPlane[3];		//	BG*3
	int			m_nPosX,m_nPosY;	//	BG表示座標
	int			m_nScrollSpeed;		//	BGスクロールスピード
	CTextPlane	m_TextPlane[32];	//	循環Text
	int			m_nTextPos;			//	テキスト表示ポジション
	int			m_nTextNext;		//	次に使うm_TextPlane
	int			m_nTextLast;		//	最初のテキスト
	int			m_nMsgSpeed;		//	メッセージのスピード
	CPlaneFadeBlt m_FadeBlt;		//	フェードBlter
	void	SetText(LPSTR);			//	テキストの設定
	int			m_nReadMessageCount;//	次のメッセージ

	CPlane		m_CutPlane;			//	Cut in/outするプレーン
	int			m_nCutCount;		//	そのカウンタ
	int			m_nCutX,m_nCutY;

	int			m_nEnd;				//	終了カウンタ
};

#endif
