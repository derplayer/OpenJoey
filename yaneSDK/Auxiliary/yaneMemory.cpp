#include "stdafx.h"
#include "yaneMemory.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

//	new,deleteごとにログ記録を残すか？
#ifdef	USE_MEMORY_CHECK

/*
	newとdeleteで、ログを吐き出す。この吐き出されたログのうち、
	newしてdeleteしていない部分を突き止めるには、たとえばPerlで
	次のようなプログラムを書けば良い。

	# memcheck.pl
	while (<>) {
		if		(m/new[\s]*([a-f\d]+)[\s]*:[\s]*([\d]+)/){
			$dat{$1} = $2;
		} elsif (m/del[\s]*([a-f\d]+)/){
			delete $dat{$1};
		}
	}

	foreach (keys %dat) {
		print "$_ : $dat{$_}\n";
	}

		上記のプログラムは、標準入力からテキストを得て、
		標準入力に出力しているので、実行は、

	jperl memcheck.pl < debug_memory.txt > result.txt

	のようにして行なう。

	そして、newしてdeleteされていないメモリ／サイズが特定できれば、
	今度は、以下のoperator newの関数内で、どのメモリであるかを
	チェックするif文を書いて、その条件成立時のところにブレーク
	ポイントを仕掛けて走らせてみる。そうすれば、どこで確保された
	メモリが解放されていなかったかがわかる。

*/

void* operator new (size_t t){
	void *p = ::malloc(t);

	FILE*fp=fopen("debug_memory.txt","aw+");
	fprintf(fp,"new %.8x :%d\n",p,t);
	fclose(fp);

	return p;
}

void operator delete(void*p){
	if (p==NULL) return ;

	FILE*fp=fopen("debug_memory.txt","aw+");
	fprintf(fp,"del %.8x\n",p);
	fclose(fp);

	::free(p);
}

#endif

///////////////////////////////////////////////////////////////////////////////

#ifdef USE_MEMORY_STATE	//	メモリのデバッグクラスを利用するか
CMemoryState* CMemoryState::g_lpMemoryState = NULL;

void* operator new (size_t t){
	CMemoryState* lpMemoryState = CMemoryState::g_lpMemoryState;
	static bool bRecursive = false;
	// この関数内のset::insertで再帰的に呼び出されるのをガードする

/*
	if (lpMemoryState!=NULL && lpMemoryState->m_lpaby!=NULL){
		// 排他的アクセス
		lpMemoryState->m_vCS.Enter();
		//	高速なnew / deleteが必要
		if (lpMemoryState->m_nMax == 0 || lpMemoryState->m_nBlockSize < t) {
			//	空きエリアあらへん or 規定サイズ以上のメモリ確保要求
			return (void *)::malloc(t);
		} else {
			// 排他的アクセス
			lpMemoryState->m_vCS.Enter();
			//	空きブロックを返す
			const LPVOID pTemp = (void*)(lpMemoryState->m_lpaby +
				lpMemoryState->m_lpanEmptyBlock[--(lpMemoryState->m_nMax)]
					* lpMemoryState->m_nBlockSize);
			// 排他的アクセス
			lpMemoryState->m_vCS.Leave();
			return pTemp;
		}
	}
*/
	if (lpMemoryState!=NULL && lpMemoryState->m_lpaby!=NULL){
		//	高速なnew / deleteが必要
		if (lpMemoryState->m_nBlockSize < t) {
			//	規定サイズ以上のメモリ確保要求
			return (void *)::malloc(t);
		} else {
			// 排他的アクセス
			lpMemoryState->m_vCS.Enter();
			if (lpMemoryState->m_nMax == 0) {
				//	空きエリアあらへん
				lpMemoryState->m_vCS.Leave();
				return (void *)::malloc(t);
			}
			//	空きブロックを返す
			const LPVOID pTemp = (void*)(lpMemoryState->m_lpaby +
				lpMemoryState->m_lpanEmptyBlock[--(lpMemoryState->m_nMax)]
					* lpMemoryState->m_nBlockSize);
			lpMemoryState->m_vCS.Leave();
			return pTemp;
		}
	}


	LPVOID p = (LPVOID)::malloc(t);
	// 排他的アクセス
	if (lpMemoryState!=NULL) lpMemoryState->m_vCS.Enter();
	if (bRecursive){
		if (lpMemoryState!=NULL) lpMemoryState->m_vCS.Leave();
		return p;
	}
	//	再帰的に呼び出されているならば、このまま帰る

	if (lpMemoryState!=NULL) {
		bRecursive = true;
		lpMemoryState->m_alpMemory.insert(pair<LPVOID,size_t>(p,t));
		bRecursive = false;
		lpMemoryState->m_vCS.Leave();
	}
	return p;
}

void operator delete(void*p){
	CMemoryState* lpMemoryState = CMemoryState::g_lpMemoryState;

	if (lpMemoryState!=NULL && lpMemoryState->m_lpaby!=NULL){
		//	高速なnew / deleteが必要

		//	newが一度も呼び出されていないのにdeleteが呼び出されることは無い
		//	（と仮定している）

		int n = (BYTE*)p - (BYTE*)lpMemoryState->m_lpaby;

		//	自分のヒープ上のメモリか？
		if ((0 <= n) &&
			(n < lpMemoryState->m_nBlockNum*lpMemoryState->m_nBlockSize)) {
			// 排他的アクセス
			lpMemoryState->m_vCS.Enter();
			n /= lpMemoryState->m_nBlockSize;
			lpMemoryState->m_lpanEmptyBlock[++lpMemoryState->m_nMax] = n;
			//	メモリの解放
			lpMemoryState->m_vCS.Leave();
		} else {
			::free(p);
		}
		return;	// ここで抜けないと２重にfreeしてしまう
	}

	::free(p);
	if (CMemoryState::g_lpMemoryState!=NULL) {
		// 排他的アクセス
		lpMemoryState->m_vCS.Enter();
		CMemoryState::g_lpMemoryState->m_alpMemory.erase(p);
		lpMemoryState->m_vCS.Leave();
	}
	//	↑これに失敗するようなら、それはnewしていないメモリのdeleteなのだが、
	//	そのようなバグは稀なので気にしないことにする
}

LRESULT	CMemoryState::Dump(ITextOutputStream*pDump) {
	LRESULT lr = 0;
	if (m_lpaby!=NULL){
		//	高速なnew / deleteを使用しているときのメモリリーク！
		for(int i=m_nMax;i<m_nBlockNum;i++){
			pDump->Out("解放されていないメモリを発見 %.8x (%d bytes未満)\n",
				(LONG)((BYTE*)m_lpaby + m_lpanEmptyBlock[i]*m_nBlockSize),m_nBlockSize);
		}
	} else {
		map<LPVOID,size_t>::iterator it;
		it = this->m_alpMemory.begin();
		pDump->Clear();
		while (it != this->m_alpMemory.end()) {
			pDump->Out("解放されていないメモリを発見 %.8x (%d bytes)\n",(LONG)(it->first),it->second);
			it++; lr ++;
		}
	}
	return lr;
}

void	CMemoryState::BeginSnap() {
	Release();
	GetCallBack()->clear();
	g_lpMemoryState = this;
}
///	スナップを開始する

void	CMemoryState::BeginSnap(int nBlockSize,int nBlockNum) {
	Release();
	m_nBlockSize = nBlockSize;
	m_nBlockNum	 = nBlockNum;
	m_nMax	= nBlockNum; // 全部空き
	m_lpaby = new BYTE[nBlockSize*nBlockNum];
	m_lpanEmptyBlock = new int[nBlockNum];
	for(int i=0;i<nBlockNum;i++){ m_lpanEmptyBlock[i] = i; }
	GetCallBack()->clear();
	g_lpMemoryState = this;
}
///	スナップを開始する

void	CMemoryState::EndSnap() {
	smart_vector_ptr<function_callback>::iterator it=GetCallBack()->begin();
	while (it!=GetCallBack()->end()){
		(*it)->run(); it++;
	}
	GetCallBack()->clear();
	g_lpMemoryState = NULL;
}
///	スナップを終了する

#endif

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

