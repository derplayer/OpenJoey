//	yaneDebugWindow.h :
//		programmed by yaneurao	'00/02/25

#ifndef __yaneDebugWindow_h__
#define __yaneDebugWindow_h__


#include "../AppFrame/yaneAppBase.h"
#include "../Auxiliary/yaneStream.h"

namespace yaneuraoGameSDK3rd {
namespace Window {

class CDebugWindow : public CAppBase , public ITextOutputStream {
/**
	エラーログの出力のためのウィンドゥクラス。
	class CAppBase から派生させたアプリクラスなので、Run()関数を呼び出し、
	窓を生成してから使うこと。また、CAppBaseから派生させているので、
	class CAppFrame のMainThreadのなかで使うこと。

	デバッグのときの変数のウオッチ等に使うならば、
	class CDbg のほうを使うほうが断然便利。

	使用例：
	CDebugWindow dbg;
	dbg.Run();
　	dbg.Out("This may be a bug...");
*/
public:
	virtual void	Clear();						//	文字列をクリアする
	virtual void __cdecl Out(LPSTR fmt, ... );		//	文字列を表示する
	virtual void	Out(int);						//	数字を表示する
	virtual void	Out(const string&);
	virtual void	Out(LONG*);		//	可変引数を取るバージョン
protected:
	virtual LRESULT OnPreCreate(CWindowOption&);		//	overriden from CAppBase
};

//	デバッグウィンドゥのローカライズ版
class CDbg : public ITextOutputStream {
/**
	CDebugWindowを、ローカライズしたバージョン。
	CDebugWindowと同じメンバ関数を持つ。
	このクラスの場合、Run()は不要で、Outしたときに必要とあらば
	自動的にデバッグウィンドゥを開く。デバッグ用途には、これが便利。

	例
	CDbg().Out("x = %d",x);
*/
public:
	//	関数インターフェースは同じ。すべてCDebugWindowに委譲してしまう
	virtual void	Clear()					{
		CCriticalLock guard(GetCriticalSection());
		CheckValid(); GetWindow()->Clear();
	}

	virtual void __cdecl Out(LPSTR fmt, ... );
	virtual void	Out(int i);
	virtual void	Out(const string&s);
	virtual void	Out(LONG*lpl);
	
	static void	SelectWindow(int nDebugWindowNo){
		//if (0<=nDebugWindowNo && nDebugWindowNo<10) m_nTarget = nDebugWindowNo;
		SetTargetWindowNo(nDebugWindowNo);
	}
	/**
		デバッグウィンドゥのナンバーを指定する。0～9。defaultでは0。
		こいつを変更することで、出力先を変更できる。
		（複数のデバッグウィンドゥを作成できる。）
	*/
	static int	GetTargetWindowNo() {
		CCriticalLock guard(GetCriticalSection());
		return m_nTarget;
	}
	static void	SetTargetWindowNo(int nDebugWindowNo) {
		CCriticalLock guard(GetCriticalSection());
		if (0<=nDebugWindowNo && nDebugWindowNo<10) m_nTarget = nDebugWindowNo;
	}

protected:
	void	CheckValid();
	CDebugWindow* GetWindow(){ return m_lpDebugWindow[m_nTarget]; }

	static	CDebugWindow* m_lpDebugWindow[10];
		//	最大10個（笑）
	static	int m_nTarget;	//	現在対象としているデバッグウィンドゥ

	static	CCriticalSection	m_cs;
	static	CCriticalSection*	GetCriticalSection() { return &m_cs;}
	//	↑なんやかやするときの、排他制御用

	//	これ、CAppManagerから終了時にコールバックしてもらう
	static void		Release(int nTarget);
};

} // end of namespace Window
} // end of namespace yaneuraoGameSDK3rd

#endif
