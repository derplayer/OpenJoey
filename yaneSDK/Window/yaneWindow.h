// yaneWindow.h
//	window base class
//

#ifndef __yaneWindow_h__
#define __yaneWindow_h__


#include "yaneWinHook.h"
#include "../Thread/yaneCriticalSection.h"

namespace yaneuraoGameSDK3rd {
namespace Window {

class CWindowOption {
/**
	class CWindow で指定するためのウィンドゥオプション
*/
public:
	string	caption;	///	キャプション
	string	classname;	///	クラス名(captionと同じでも良い)
	LPCTSTR dialog;		///	ダイアログの場合は、ここでリソース名を指定すればok
		/**
			これをstringにしていないのは、
			MAKEINTRESOURCEマクロが、単にint型を強制的にLPCTSTRにキャストして
			::DialogBox関数に渡す仕様になっているため、stringにすると
			MAKEINTERSOURCEマクロが使えないから。

			IWindow派生クラスのOnPreCreateをオーバーライドして、

			virtual LRESULT OnPreCreate(CWindowOption &opt){
				opt.dialog = MAKEINTRESOURCE(IDD_DIALOG1);	//	ダイアログなのだ！
				return 0;
			}
			のようにすれば、ダイアログを表示できる
	
		*/

	int		size_x;		///	横方向のサイズ
	int		size_y;		///	縦方向のサイズ
	LONG	style;		///	ウィンドゥスタイルの追加指定	

	bool	bCentering;	///	ウィンドゥは画面全体に対してセンタリングして表示か

	CWindowOption() {
		/**
			ディフォルトでこれ。必要があれば書き換えるべし
			caption = "あぷりちゃん";
			classname = "YANEAPPLICATION";
			style = WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
			size_x = 640; size_y = 480;
			bCentering = true;
		*/
		caption = "あぷりちゃん";
		classname = "YANEAPPLICATION";
		style = WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
		size_x = 640; size_y = 480;
		bCentering = true;
		dialog = NULL;
	}
};

class IWindow {
public:
	virtual LRESULT		Create(CWindowOption& opt,HWND lpParent=NULL)=0;
	virtual HWND		GetHWnd()const=0;
	virtual LRESULT		SetWindowPos(int x,int y)=0;
	virtual void		ChangeWindowStyle()=0;
	virtual LRESULT		Resize(int sx,int sy)=0;
	virtual void		SetSize(int sx,int sy)=0;
	virtual void		UseMouseLayer(bool bUse)=0;
	virtual void		ShowCursor(bool bShow)=0;
	virtual bool		IsShowCursor()=0;
	virtual CWindowOption* GetWindowOption()=0;
	volatile virtual bool		IsMinimized()=0;
	virtual CWinHookList* GetHookList()=0;
	virtual void		ClearAllHook()=0;
	
	virtual void		SetResized(bool bResized)=0;
	// DirectDrawによるフルスクリーンモード化したときに
	//	ウィンドゥが勝手にリサイズされるので、そのときに通知しておかないと
	//	ウィンドゥクラスの内部状態と異なるものになってしまう

	virtual void ChangeScreen(bool bFullScr)=0;
	virtual bool IsDialog() const=0;

	virtual ~IWindow(){}
};

class CWindow : public IWinHook , public IWindow{
/**
	窓を生成するクラスです。このクラスのインスタンス一つが、
	窓一つに対応します。マルチウィンドゥのサポートはやや甘いです。
	（どうせフルスクリーンになるとDirectDrawを用いて描画している以上、
	どうしようも無いという話もある…）

	☆　描画のために

	class CFastDrawを使った描画を行なう場合、
	そちらも参照してください。CFastDrawを使っているときに
	画面サイズを変更する場合、CFastDraw::ChangeDisplayModeを
	呼び出してください。

*/
public:
	/// 窓を作る。親ウィンドウが存在するのならば最後のパラメータで指定すること
	LRESULT		Create(CWindowOption& opt,HWND lpParent=NULL);

	HWND		GetHWnd()const { return m_hWnd; }	///	HWNDを返す

	LRESULT		SetWindowPos(int x,int y);			///	ウィンドゥを移動

	void		ChangeWindowStyle();
	///	現在のスクリーンモード（フルスクリーン or ウィンドゥモード）用に
	///	WindowStyleを変更する

	LRESULT		Resize(int sx,int sy);				///	窓のリサイズ

	//	本来はstaticな関数なのだがIWindow経由でアクセスする必要があるため、こうなる
	virtual void ChangeScreen(bool bFullScr){
		g_bFullScreen = bFullScr;
	}
	/**
		フルスクリーン⇔ウィンドゥモードの切り替えが発生した場合、
		この関数を呼び出したあとそれぞれのウィンドゥに関して
		ChangeWindowStyleを呼び出すこと
		ただし、フルスクリーン時のマルチウィンドゥはDirectDrawを使う関係上、
		非サポート
	*/

	void		SetSize(int sx,int sy);
	/**
		窓のリサイズ（設定のみで実際に変更はしない）
		なぜこんなものが必要になるかというと、Resize⇒ChangeWindowStyleと
		実行すると２回ウィンドゥサイズの変更を行なうことになって、
		そのモーションが見えて困るから。
		SetSize　⇒　ChangeWindowStyleならば安全。
	*/

	///		MouseLayer
	void	UseMouseLayer(bool bUse);
	///	ソフトウェアカーソルのためにカーソルを消す
	void	ShowCursor(bool bShow);
	///	ハードウェアマウスカーソルの表示／非表示
	bool	IsShowCursor();
	///	ハードウェアマウスカーソルの表示／非表示を取得

	///	生成しているのはダイアログか？
	virtual bool IsDialog() const
	{	return const_cast<CWindow*>(this)->GetWindowOption()->dialog!=NULL; }

	///		Property
	DWORD		m_dwFillColor;		///	背景色

	CWindowOption* GetWindowOption() { return &m_opt; }
	///	Windowオプションの取得。ウィンドゥ生成前ならば書き換えても良い。

	volatile bool	IsMinimized() { return m_bMinimized; }
	///	最小化されているか？

	///	メッセージをフックするためのポインタリスト
	///	（class IWinHook のベクタ）
	CWinHookList* GetHookList() { return &m_HookPtrList; }

	///	メッセージをフックするためのポインタリストをクリアする
	void	ClearAllHook() { m_HookPtrList.Clear(); }

	CWindow();
	virtual ~CWindow();

	/**
		 メニューの存在をチェックし、メニューがあるのならば
		内部フラグ(m_bUseMenu)を更新する。
		動的にメニューを取り外ししたときに、このクラスに
		それを反映させるのに使う
	*/
	void CheckMenu() {
		if(m_hWnd!=NULL&&::GetMenu(m_hWnd)!=NULL){
			m_bUseMenu = true;
		}else{
			m_bUseMenu = true;
		}
	}

	///		その他
	static void	GetScreenSize(int &x,int &y);
	///	現在の画面全体のサイズの取得

	static bool	IsFullScreen() { return g_bFullScreen; }
	///	フルスクリーンか？

	virtual void		SetResized(bool bResized) { m_bResized = true; }
	/**
		DirectDrawによるフルスクリーンモード化したときに
		ウィンドゥが勝手にリサイズされるので、そのときに通知しておかないと
		ウィンドゥクラスの内部状態と異なるものになってしまう
		(CFastDrawから呼び出して使う)
	*/

protected:
	HWND		m_hWnd;				//	ウィンドゥハンドル
	bool		m_bFullScreen;		//	現在どちらのモードに合わせて窓を作っているのか？
	CWindowOption	m_opt;			//	ウィンドゥオプション
	bool		m_bCentering;		//	ウィンドゥは画面全体に対してセンタリングして表示か(default:true)

	static LRESULT CALLBACK gWndProc(HWND,UINT,WPARAM,LPARAM);
	static LRESULT CALLBACK gDlgProc(HWND,UINT,WPARAM,LPARAM);
	LRESULT Dispatch(HWND,UINT,WPARAM,LPARAM);	//	windows message dispatcher

	LRESULT		Initializer();	//	起動後、一度だけウィンドゥクラスを登録する
	//	フックしているすべてのインスタンスへのチェイン
	CWinHookList	m_HookPtrList;

	////////////////////////////////////////////////////////////////////////////
	//	マウスカーソルのOn/Offは、ウィンドゥに対する属性なので
	//	ウィンドゥクラスが担うべき
//	void	InnerShowCursor(bool bShow);
	bool	m_bShowCursor;			//	マウスカーソルの表示状態
	bool	m_bUseMouseLayer;		//	ソフトウェアマウスカーソルを使うか？

	//	ウィンドゥサイズのadjust
	void	InnerAdjustWindow(RECT&,CWindowOption&);

	//	メニュー付きウィンドゥか？
	bool	m_bUseMenu;

	//	ChangeWindowStyle⇔SetSizeのフラグ
	bool	m_bResized;

	//	Windows基本窓タイプを作成したときに、メッセージハンドラを
	//	フックするので、その関数ポインタを保存しておく必要がある。
	WNDPROC	m_pWndProc;

	//	最小化されているか？
	volatile bool	m_bMinimized;

	bool IsWindowsClassName(const string& strClassName);
	//	Windowsで用意されているWindowクラス名かどうかを調べる

/*
	//	メニューが確実に存在することを事前に伝えておけば
	//	生成時に正確なウィンドゥサイズがいきなり求まる
	void		UseMenu(bool bUseMenu) { m_bUseMenu = bUseMenu; }
*/	//	自動的に判定するようにした

	static	bool g_bFullScreen;			//	フルスクリーンモードなのか？

	static CCriticalSection m_cs;
	static CCriticalSection* GetCriticalSection() { return &m_cs; }
	//	↑この上のstaticメンバ群にアクセスするためのクリティカルセクション

	// override from CWinHook
	virtual LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

class CDialogHelper {
/**
	ダイアログを扱うときのヘルパクラス
*/
public:
	CDialogHelper(IWindow *pWin=NULL) : m_pWindow(pWin){ hook(pWin); }
	~CDialogHelper() { unhook(GetWindow()); }

	///	コンストラクタで渡しそびれた時のためのsetter / getter
	IWindow* GetWindow() { return m_pWindow; }
	void SetWindow(IWindow*pWin) { unhook(m_pWindow); m_pWindow = pWin; hook(pWin); }

	///	HWND取得
	HWND	GetHWnd() { return GetWindow()->GetHWnd();}

	///	あるコントロールのHWndを得る
	HWND	GetHWnd(int nEditControlID) { return ::GetDlgItem(GetHWnd(),nEditControlID);}

	///	エディットコントロールetc..から、そこに表示されているテキストを取得する
	///	(StaticText,Edit Control)
	string	GetText(int nEditControlID);

	///	エディットコントロールetc..に対して、そこにテキスト文字列を設定する
	///	(StaticText,Edit Control)
	LRESULT	SetText(int nEditControlID,const string& str);

	///	ボタン状態の取得
	///	(CheckBox,RadioButton)
	/**
		-1:そのボタンは存在しない
		0:ボタンのチェックはオフになっています。 
		1:ボタンのチェックはオンになっています。 
		2:ボタンはグレイ表示（不確定）の状態です。
		ボタンが、BS_3STATE スタイルまたは BS_AUTO3STATE スタイルを持つときにだけ適用されます。 
	*/
	LRESULT	GetCheck(int nEditControlID);

	///	ボタンにチェックを入れる
	///	(CheckBox,RadioButton)
	/**
		nCheck:
		0:ボタンのチェックはオフに
		1:ボタンのチェックはオンに
		2:ボタンはグレイ表示（不確定）の状態に
	*/
	LRESULT SetCheck(int nEditControlID,int nCheck=1);

	/**
		ボタン等は押し下げ情報が来るので、特定のメッセージを
		hookする形で情報を取得する

		例)
		ボタンならば

			uMsg = WM_COMMAND , wParam = nEditControl
		だから、このように
		事前にuMsgとwParamをペアにしてメッセージdispatcherに
		登録しておき、あとで解析キューのデータを調べると良い

		⇒ class CMessagePoolerも参照のこと。
	*/
	CMessagePooler* GetPooler() { return &m_vPooler; }

	/**
		Hook系は、識別子が戻るので、GetPoolInfoの引数として
		渡して、プールされている情報を取得して使う
	*/

	int	HookButtonOnClick(int nEditControl)
	{	return GetPooler()->SetPoolData(WM_COMMAND,nEditControl,true,0,false); }
	/**
			ボタン情報のhook
		例)
			int nID = dialog.HookButtonOnClick(IDC_BUTTON1);
			while (IsThreadValid()){
				if (dialog.GetPoolInfo(nID)->isPool()){
				//	ボタンおされとる！
					dialog.GetPoolInfo(nID)->reset();
					//	押し下げ情報のクリア
				}
			}
	*/

	int	HookHScroolBar(int nEditControl)
	{	return GetPooler()->SetPoolData(WM_HSCROLL,0,false,(LPARAM)GetHWnd(nEditControl),true);}
	/**
		水平スクロールバーのhook

		メッセージはwParamに入る
			nScrollCode = (int) LOWORD(wParam); // スクロールコード
			nPos = (short int) HIWORD(wParam);	// スクロールボックス（つまみ）の位置
	*/

	int	HookVScroolBar(int nEditControl)
	{	return GetPooler()->SetPoolData(WM_VSCROLL,0,false,(LPARAM)GetHWnd(nEditControl),true);}
	/**
		垂直スクロールバーのhook

		メッセージはwParamに入る
			nScrollCode = (int) LOWORD(wParam); // スクロールコード
			nPos = (short int) HIWORD(wParam);	// スクロールボックス（つまみ）の位置
	*/

	///	スクロールバーには、さらにヘルパ関数必要だろう（作りかけ）

	CMessagePooler::CInfo* GetPoolInfo(int nID)
	{	return GetPooler()->GetInfo(nID); }

	///	---- コンボボックスとリストボックス用の情報保持クラス ---
	/**
		備考：コンボボックスは以下の３種類ある
			１．標準 　　　　　　　　常に垂れている 　　　　　エディット可能
			２．ドロップダウン 　　　↓ボックスで垂れる 　　　エディット可能 
			３．ドロップダウンリスト フォーカスが来ると垂れる エディット禁止（リスト中から選択のみ）

		ドロップダウンの高さ(垂れ具合)の設定：
			リソースエディタのコンボボックスコントロールの絵の↓を
			クリックして枠を出す。この枠を下方向に引っ張って垂れ具合を調整。
			注意！：デフォルトのままだと高さが０なのでリスト項目が見えない。
	*/
	struct CListBoxInfo {
	typedef vector<string> mylist;

	mylist* GetList() { return& m_list;}
	const mylist* GetConstList() const { return& m_list; }

	///	リストボックスであるかを設定／取得する(リストボックスでないならコンボボックス）
	bool	IsListBox() const { return m_bListBox; }
	void	SetListBox(bool bListBox) { m_bListBox = bListBox; }

	///	文字列をリストに追加
	/// nPosは追加個所。指定した直前に追加される。nPos==-1(default)では、最終行に追加される
	void	AddString(const string& str,int nPos = -1){
		if (nPos==-1){
			GetList()->push_back(str);
			nPos = (int)GetList()->size()-1;
		} else {
			//	挿入するんで1個ずつずらす
			GetList()->push_back("");
			int size = (int)GetList()->size();
			for(int i=size-1;i>=nPos+1;--i){
				(*GetList())[i] = (*GetList())[i-1];
			}
		}
		::SendMessage(GetHWnd(),IsListBox()?LB_INSERTSTRING:CB_INSERTSTRING
			,(WPARAM)nPos,reinterpret_cast<LPARAM>(str.c_str()));

	}

	///	現在の選択行を設定／取得する (0 origin)
	LRESULT	SetCurSel(int nSel) {
		if ((int)GetList()->size() <= nSel) return -1; // 範囲外
		::SendMessage(GetHWnd(),IsListBox()?LB_SETCURSEL:CB_SETCURSEL
			, (WPARAM)nSel, 0L);
		return 0;
	}
	///	（取得する場合は、選択行がなければ-1が返る)
	int GetCurSel() const {
		int nPos = ::SendMessage(GetHWnd(),IsListBox()?LB_GETCURSEL:CB_GETCURSEL
			,0L, 0L);
		return nPos;
	}

	///	現在選択している場所にある文字列を取得
	string	GetSelectedString() const {
		int nPos = GetCurSel();
		if ((int)GetConstList()->size()<=nPos) {
			return "";
		} else {
			return (*GetConstList())[nPos];
		}
	}

	///	設定されている文字列を全クリア
	LRESULT	Clear(){
		HWND hWnd = GetHWnd();
		if (hWnd==NULL) return -1;
		::SendMessage(hWnd,
			IsListBox()?LB_RESETCONTENT:CB_RESETCONTENT,0,0L);
		GetList()->clear();
		return 0;
	}

	CListBoxInfo(HWND hWnd = NULL) : m_hWnd(hWnd),m_bListBox(true) {}
	
	///	コンストラクタで渡しそびれたときのためにコンボボックス(リストボックス)
	///	のhWndを渡してやるためのメンバ
	void	SetHWnd(HWND hWnd) { m_hWnd = hWnd; }
	HWND	GetHWnd() const { return m_hWnd; }

	protected:
	mylist m_list;		//	保持している情報リスト
	HWND   m_hWnd;
	bool	m_bListBox; // true:listbox false:combobox
	};

	/**
		リストボックスorコンボボックスを、CListBoxInfoと関連づける
		bListBox : true == リストボックスである　false == コンボボックスである
	*/
	void	Attach(CDialogHelper::CListBoxInfo& info,int nEditControl,bool bListBox=true)
	{
		info.SetHWnd(GetHWnd(nEditControl));
		info.SetListBox(bListBox);
	}
	///--------------------------------------------------------

	///	---- そのほか、欲しかったら追加するデ？？ ----

protected:
	IWindow* m_pWindow;	//	親Windowポインタを設定しておく
	CMessagePooler m_vPooler; // メッセージPooler

	void	hook(IWindow*pWin);
	void	unhook(IWindow*pWin);
};

class CMsgDlg {
/**
	メッセージダイアログ出力用(主にデバッグ用)
*/
public:
	void	Out(const string& caption,const string& message);
};

} // end of namespace Window
} // end of namespace yaneuraoGameSDK3rd

#endif
