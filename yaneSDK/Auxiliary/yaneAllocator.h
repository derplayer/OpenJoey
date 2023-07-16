
#ifndef __yaneAllocator_h__
#define __yaneAllocator_h__
#include "../Thread/yaneLockObject.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class IMemoryAllocator {
/**
	メモリ割り当て用
*/
public:
	virtual	BYTE* Alloc(int size) = 0;
	/**
		sizeバイトのメモリをallocして、その先頭アドレスを返す
		size==0のときは、NULLポインタを返す
	*/

	virtual void Free(BYTE* pb) = 0;
	/**
		Allocで確保したメモリを返却する
		返却できなかったときの動作は未定義
	*/

	virtual ~IMemoryAllocator() {}
};

class CMemoryAllocatorPool : public IMemoryAllocator {
/**
	メモリ割り当て用　汎用クラス
	スタック領域上に確保したメモリに対するアロケータを作りたいときなどに使う
	メンバは一応thread safeである。

	使用例）
		BYTE stack1024[1024];
		CMemoryAllocatorPool pool;
		pool.SetMemory(&stack1024[0],1024); // stack1024のメモリをプールする

		BYTE* p = pool.Alloc(1024);	//	プールされたメモリから割り当て

*/
public:
	void	SetMemory(BYTE*pb,int size) {
		synchronized guard(cs_);
		Release();
		memory_.push_back(CMemoryTable(pb,size,false));
	}
	/**
		予め確保されたメモリを、セットしておけば、以降の
		Allocではそのメモリ上から領域を確保する。
		SetMemoryしていない状態でAllocを行なったときや、
		Allocで十分なメモリが確保できないときはCRuntimeExceptionをthrowする
	*/

	void	Release() { synchronized guard(cs_); memory_.clear(); }
	/**
		確保しているメモリ情報等をリセットする
	*/

	//	overridden from IMemorAllocator
	virtual	BYTE* Alloc(int size);
	virtual void Free(BYTE* pb);

	CMemoryAllocatorPool() { Release(); }
	virtual ~CMemoryAllocatorPool() {}
	// 未解放の領域があれば例外スローするとかしてもいいが?


protected:
	struct CMemoryTable {
		CMemoryTable(BYTE*start,int size,bool bUsed) :
			start_(start),size_(size),bUsed_(bUsed){}
		BYTE* start_;	//	先頭
		int	  size_;	//	サイズ(単位はバイト)
		bool  bUsed_;	//	使用しているかどうかのフラグ
	};

	typedef list<CMemoryTable>mylist;
	mylist memory_;
	//	これが確保したメモリを表現する

	CCriticalSection cs_;
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
