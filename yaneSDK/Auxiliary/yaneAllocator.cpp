#include "stdafx.h"
#include "yaneAllocator.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

//	overridden from IMemoryAllocator
BYTE* CMemoryAllocatorPool::Alloc(int size){
	synchronized guard(cs_);

	mylist::iterator it = memory_.begin();
	while (it!=memory_.end()){
		//	先頭からサーチして空いていたところを割り当てる
		if (!(*it).bUsed_){
			int rest = (*it).size_ - size;
			if (rest >=0) {
				//	これを割りあててしまえ
				(*it).bUsed_ = true;
				BYTE* p = (*it).start_;
				//	このブロック、残りがまだあるのなら、分割してそちらは
				//	未使用領域としなければならない
				if (rest>0) {
					it++;
					memory_.insert(it,CMemoryTable(p+size,rest,false));
				}
				return p;
			}
		}
		it++;
	}
	//	メモリが確保できなかった
	#ifdef USE_EXCEPTION
		throw CRuntimeException(
			"CMemoryAllocatorPool::Allocで確保するメモリ不足"
		);
	#else
		return NULL; // failed
	#endif
}

void CMemoryAllocatorPool::Free(BYTE* pb){
	synchronized guard(cs_);

	mylist::iterator it = memory_.begin();
	while (it!=memory_.end()){
		if ((*it).start_==pb){
			//	これだ。解放汁！
			if (!(*it).bUsed_){
				//	もう解放したやつヤン?
				#ifdef USE_EXCEPTION
					throw CRuntimeException(
						"CMemoryAllocatorPool::Freeでメモリの二重解放"
					);
				#endif
			}
			(*it).bUsed_ = false;
			// この前後のブロックと結合できるならしてまう
			//	1.前方に対して結合(解放ごとに結合しているので、
			//		空ブロックが2つ並んでいることは考えなくてよい)
			if (it!=memory_.begin()){
				mylist::iterator it2 = it;
				it2--;
				//	ひとつ前が未使用領域かつ、現ブロックの先頭まで伸びている
				//	(断片化されたメモリも扱うことを想定しているため、
				//		後者のチェックも必要)
				if (!(*it2).bUsed_ &&
					((*it2).start_ + (*it2).size_ == (*it).start_)){
					(*it2).size_ += (*it).size_;
					memory_.erase(it);
					it = it2;
				}
			}
			//	2.後方に対して結合
			mylist::iterator it2 = it;
			it2++;
			if (it2!=memory_.end()){
				if (!(*it2).bUsed_ &&
					((*it).start_ + (*it).size_ == (*it2).start_)){
					(*it).size_ += (*it).size_;
					memory_.erase(it2);
				}
			}
			return ; // 解放処理終了
		}
		it++;
	}
	//	メモリが解放できなかった
	#ifdef USE_EXCEPTION
		throw CRuntimeException(
			"CMemoryAllocatorPool::Freeでメモリ解放に失敗"
		);
	#endif
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
