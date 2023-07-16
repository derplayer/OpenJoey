#include "stdafx.h"
#include "yaneAllocator.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

//	overridden from IMemoryAllocator
BYTE* CMemoryAllocatorPool::Alloc(int size){
	synchronized guard(cs_);

	mylist::iterator it = memory_.begin();
	while (it!=memory_.end()){
		//	�擪����T�[�`���ċ󂢂Ă����Ƃ�������蓖�Ă�
		if (!(*it).bUsed_){
			int rest = (*it).size_ - size;
			if (rest >=0) {
				//	��������肠�ĂĂ��܂�
				(*it).bUsed_ = true;
				BYTE* p = (*it).start_;
				//	���̃u���b�N�A�c�肪�܂�����̂Ȃ�A�������Ă������
				//	���g�p�̈�Ƃ��Ȃ���΂Ȃ�Ȃ�
				if (rest>0) {
					it++;
					memory_.insert(it,CMemoryTable(p+size,rest,false));
				}
				return p;
			}
		}
		it++;
	}
	//	���������m�ۂł��Ȃ�����
	#ifdef USE_EXCEPTION
		throw CRuntimeException(
			"CMemoryAllocatorPool::Alloc�Ŋm�ۂ��郁�����s��"
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
			//	���ꂾ�B����`�I
			if (!(*it).bUsed_){
				//	����������������?
				#ifdef USE_EXCEPTION
					throw CRuntimeException(
						"CMemoryAllocatorPool::Free�Ń������̓�d���"
					);
				#endif
			}
			(*it).bUsed_ = false;
			// ���̑O��̃u���b�N�ƌ����ł���Ȃ炵�Ă܂�
			//	1.�O���ɑ΂��Č���(������ƂɌ������Ă���̂ŁA
			//		��u���b�N��2����ł��邱�Ƃ͍l���Ȃ��Ă悢)
			if (it!=memory_.begin()){
				mylist::iterator it2 = it;
				it2--;
				//	�ЂƂO�����g�p�̈悩�A���u���b�N�̐擪�܂ŐL�тĂ���
				//	(�f�Љ����ꂽ���������������Ƃ�z�肵�Ă��邽�߁A
				//		��҂̃`�F�b�N���K�v)
				if (!(*it2).bUsed_ &&
					((*it2).start_ + (*it2).size_ == (*it).start_)){
					(*it2).size_ += (*it).size_;
					memory_.erase(it);
					it = it2;
				}
			}
			//	2.����ɑ΂��Č���
			mylist::iterator it2 = it;
			it2++;
			if (it2!=memory_.end()){
				if (!(*it2).bUsed_ &&
					((*it).start_ + (*it).size_ == (*it2).start_)){
					(*it).size_ += (*it).size_;
					memory_.erase(it2);
				}
			}
			return ; // ��������I��
		}
		it++;
	}
	//	������������ł��Ȃ�����
	#ifdef USE_EXCEPTION
		throw CRuntimeException(
			"CMemoryAllocatorPool::Free�Ń���������Ɏ��s"
		);
	#endif
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
