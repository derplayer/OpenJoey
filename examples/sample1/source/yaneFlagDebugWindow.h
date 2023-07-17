//////////////////////////////////////////////////////////////////////////////
//
//	フラグデバッグ用GUI
//
//		programmed by yaneurao('02/11/06)

#ifndef __yaneFlagDebugWindow_h__
#define __yaneFlagDebugWindow_h__

class CFlagDebugApp : public CAppFrame {
public:

	///	与えられたフラグ番号に対して、そのフラグへのポインタを返す関数(delegate)
	void SetFlagDelegate(const delegate<int*,int>& func)
		{ m_funcFlag = func;}

	///	与えられたフラグ番号に対して、そのフラグの説明返す関数(delegate)
	void SetFlagExplanationDelegate(const delegate<string,int>& func)
		{ m_funcFlagExplanation = func;}

	/**
		↑上記のふたつは、引数が範囲外の数字であれば、
			NULLを返すようにコーディングしてください
	*/

	string	GetDebugBmpFile() const { return m_strDebugBmp; }
	void	SetDebugBmpFile(const string& str) { m_strDebugBmp=str; }

protected:
	virtual void MainThread();

	CFastDraw* GetDraw() { return m_vDraw.GetDraw(); }
	CPlane GetPlane() const { return m_plane;}
	CMouse* GetMouse() { return& m_mouse;}
	CKeyInput* GetInput() { return& m_input; }

	void	DrawNum(int nNum,int x,int y,int nFigure,int bNot=16);
	//	数字を(x,y)に nFigure桁で描画する nNot進数で表記
	
private:
	delegate<int*,int> m_funcFlag;
	//	与えられたフラグ番号に対して、そのフラグへのポインタを返す関数(delegate)

	delegate<string,int> m_funcFlagExplanation;
	//	与えられたフラグ番号に対して、そのフラグの説明返す関数(delegate)

	CFastPlaneFactory m_vDraw;
	string m_strDebugBmp;
	CPlane m_plane;
	CMouse m_mouse;
	CKeyInput m_input;
};

//	これがmain windowのためのクラス。
class CFlagDebugWindow : public CAppBase {	//	アプリケーションクラスから派生
	string m_strDebugBmp;
	virtual void MainThread(){			   //  これがワーカースレッド
		flagdebugger.Add();
		flagdebugger->SetDebugBmpFile(m_strDebugBmp);
		/*
				ここでnewしないといけない。
				ここでnewせず、このクラスがCFlagDebugWindowを保有すると、
				このクラスを生成するスレッドがCFlagDebugWindowを生成することになり、
				CFlagDebugWindowで利用しているCFastPlaneFactoryのコンストラクタを
				生成するスレッドが、CFlagDebugWindow::MainThreadを実行しているスレッドと
				異なることになるのでまずい
			*/
		flagdebugger->SetFlagDelegate(m_funcFlag);
		flagdebugger->SetFlagExplanationDelegate(m_funcFlagExplanation);
		flagdebugger->Start();
		flagdebugger.Delete();
			/*
				そして、MainThreadから抜けるまでに解体しておかなければ、
				CFastDrawがメッセージをフックする関係上、まずい
			*/
	}
	virtual LRESULT OnPreCreate(CWindowOption& opt){
		opt.caption = "FlagDebugWindow";
		opt.style = WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
		opt.size_x = 320; opt.size_y = 240;
		opt.bCentering = false;
		return 0;
	}
	smart_ptr<CFlagDebugApp> flagdebugger;
	delegate<int*,int> m_funcFlag;
	delegate<string,int> m_funcFlagExplanation;
public:
	CFlagDebugApp* GetFlagDebugger() { return flagdebugger.get(); }
	void	SetDebugBmpFile(const string& str) { m_strDebugBmp = str; }
		///	数字書いたbmpを用意↑するのだ

	CFlagDebugWindow() { SetDebugBmpFile("debug_nums.bmp"); }
		//	ディフォルトのファイルを設定しとこか...

	///	与えられたフラグ番号に対して、そのフラグへのポインタを返す関数(delegate)
	void SetFlagDelegate(const delegate<int*,int>& func)
		{ m_funcFlag = func;}

	///	与えられたフラグ番号に対して、そのフラグの説明返す関数(delegate)
	void SetFlagExplanationDelegate(const delegate<string,int>& func)
		{ m_funcFlagExplanation = func;}

	/**
		↑上記のふたつは、引数が範囲外の数字であれば、
			NULLを返すようにコーディングしてください
	*/
};

#endif
