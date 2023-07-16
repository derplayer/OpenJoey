
#ifndef __yaneMemory_h__
#define __yaneMemory_h__


#ifdef USE_MEMORY_STATE	//	メモリのデバッグクラスを利用するか

#include "../Auxiliary/yaneStream.h"
#include "../Thread/yaneCriticalSection.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class CMemoryState {
/**

	///	！！！ＤＡＮＧＥＲ！！！
	///	－－－－このクラスは作りかけ－－－－
	///	！！！！使わないでね！！！


	１．現在newしているメモリのスナップショットを取得して、
	メモリリークを検出するためのクラス。

	２．あるいは、自前の高速なnew /deleteを定義する。

	１．２．の両方に使える。

	yaneConfig.hでUSE_MEMORY_STATEをdefineしているときにのみ有効

	使い方１．

	このクラスを利用して、

	CTextOutputStreamFile dump;
	dump.SetFileName("leaklog.txt");
	CMemoryState s;

	s.BeginSnap();

	{
		BYTE* lpabyLeak1 = new BYTE[100];
		DWORD* lpabyLeak2 = new DWORD[100];

	}
	s.EndSnap();

	s.Dump(&dump);

	のようにすれば、この２箇所のメモリリークが存在することが検出できます。

	使い方２．

	CMemoryState s;

	s.BeginSnap(16,4096);	//	16バイトのブロックを4096個用意する
	{
		//	実処理をここに書く

		//	16バイト以下のnewが発生した場合、このクラスの確保している
		//	エリアからブロック単位で優先して使用していく

	}
	s.EndSnap();
	s.Dump(&dump);
	//	割り当てられたブロックのうち未解放のエリアがあったならば
	//	診断のために表示させることも出来る

	☆　この場合、BeginSnap～EndSnapの間でnewしてdeleteされていないような
		インスタンスが有ってはいけません。

	⇒	s.BeginSnap(16,4096);	//	16バイトのブロックを4096個用意する
	の部分を
		s.BeginSnap(); // 16,4096);	//	16バイトのブロックを4096個用意する
	というようにコメントアウトして、メモリリークを検出しておき、最後に
	この高速版なnew/deleteに置き換えるという使いかたが結構いいかも．．

*/
public:
	void	BeginSnap();
	///	スナップを開始する

	void	BeginSnap(int nBlockSize,int nBlockNum);
	///	スナップを開始する

	void	EndSnap();
	///	スナップを終了する

	LRESULT	Dump(ITextOutputStream*pDump);
	///	メモリリークの状況をIDumpContextに出力。
	///	リークがなければ、何も出力せずに0が返る

	CMemoryState() { m_lpaby = NULL; }
	virtual ~CMemoryState() { Release(); }

	static bool	IsActive() { return g_lpMemoryState!=NULL; }
	///	このクラスがメモリのnew/deleteをオーバーロードしているときは
	///	このフラグがtrueになるので、そのときは、コールバックオブジェクトを
	///	仕掛けるなり、何なりする。
	static void RegistCallBack(const smart_ptr<function_callback>& fn){
		if (g_lpMemoryState!=NULL){
			g_lpMemoryState->GetCallBack()->insert(fn);
		}
	}

public:	//	面倒だからpublicでいいや＾＾；
	map<LPVOID,size_t>	m_alpMemory;
	//	new しているメモリ列

	BYTE*	m_lpaby;			//	こいつがメモリを保持
	int*	m_lpanEmptyBlock;	//	空きブロック管理用
	int		m_nBlockSize;		//	1ブロックのサイズ(byte)
	int		m_nBlockNum;		//	この数だけ一気にブロック確保
	int		m_nMax;				//	メモリブロックの次に空いている番号

	static CMemoryState* g_lpMemoryState;
	//	こいつにスナップ中である

	//	終了時にCallBackするリスト
	smart_vector_ptr<function_callback> m_afnCallBack;
	smart_vector_ptr<function_callback>* GetCallBack() { return & m_afnCallBack; }

	CCriticalSection m_vCS;

protected:
	void Release(){
		g_lpMemoryState = NULL; // いまカスタムバージョンのnew/delteを使われては困る
		if (m_lpaby!=NULL) {
			delete [] m_lpaby;
			delete [] m_lpanEmptyBlock;
			m_lpaby = NULL;
		}
	}
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif

#endif
