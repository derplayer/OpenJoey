
#ifndef __yaneAllocator_h__
#define __yaneAllocator_h__
#include "../Thread/yaneLockObject.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class IMemoryAllocator {
/**
	���������蓖�ėp
*/
public:
	virtual	BYTE* Alloc(int size) = 0;
	/**
		size�o�C�g�̃�������alloc���āA���̐擪�A�h���X��Ԃ�
		size==0�̂Ƃ��́ANULL�|�C���^��Ԃ�
	*/

	virtual void Free(BYTE* pb) = 0;
	/**
		Alloc�Ŋm�ۂ�����������ԋp����
		�ԋp�ł��Ȃ������Ƃ��̓���͖���`
	*/

	virtual ~IMemoryAllocator() {}
};

class CMemoryAllocatorPool : public IMemoryAllocator {
/**
	���������蓖�ėp�@�ėp�N���X
	�X�^�b�N�̈��Ɋm�ۂ����������ɑ΂���A���P�[�^����肽���Ƃ��ȂǂɎg��
	�����o�͈ꉞthread safe�ł���B

	�g�p��j
		BYTE stack1024[1024];
		CMemoryAllocatorPool pool;
		pool.SetMemory(&stack1024[0],1024); // stack1024�̃��������v�[������

		BYTE* p = pool.Alloc(1024);	//	�v�[�����ꂽ���������犄�蓖��

*/
public:
	void	SetMemory(BYTE*pb,int size) {
		synchronized guard(cs_);
		Release();
		memory_.push_back(CMemoryTable(pb,size,false));
	}
	/**
		�\�ߊm�ۂ��ꂽ���������A�Z�b�g���Ă����΁A�ȍ~��
		Alloc�ł͂��̃������ォ��̈���m�ۂ���B
		SetMemory���Ă��Ȃ���Ԃ�Alloc���s�Ȃ����Ƃ���A
		Alloc�ŏ\���ȃ��������m�ۂł��Ȃ��Ƃ���CRuntimeException��throw����
	*/

	void	Release() { synchronized guard(cs_); memory_.clear(); }
	/**
		�m�ۂ��Ă��郁������񓙂����Z�b�g����
	*/

	//	overridden from IMemorAllocator
	virtual	BYTE* Alloc(int size);
	virtual void Free(BYTE* pb);

	CMemoryAllocatorPool() { Release(); }
	virtual ~CMemoryAllocatorPool() {}
	// ������̗̈悪����Η�O�X���[����Ƃ����Ă�������?


protected:
	struct CMemoryTable {
		CMemoryTable(BYTE*start,int size,bool bUsed) :
			start_(start),size_(size),bUsed_(bUsed){}
		BYTE* start_;	//	�擪
		int	  size_;	//	�T�C�Y(�P�ʂ̓o�C�g)
		bool  bUsed_;	//	�g�p���Ă��邩�ǂ����̃t���O
	};

	typedef list<CMemoryTable>mylist;
	mylist memory_;
	//	���ꂪ�m�ۂ�����������\������

	CCriticalSection cs_;
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
