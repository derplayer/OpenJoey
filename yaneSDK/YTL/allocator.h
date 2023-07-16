/**
	simple allocator class
		STL��allocator class�́AVC++6��7(.NET)�Ŏ������قȂ�̂�
		���O�ŗp�ӂ���B
		�iSTLPort���������s�ςƂ͌���Ȃ��̂Łj

	implemented by coLun(Y.Imamura) '03/06/19
*/
/**
	yaneSDK3rd�ł�DLL���Ŏ������ꂽ�N���X�̌Ăяo�����T�|�[�g����Ă��܂��B
	�܂��AEXE-DLL�ԂŎ��R�ɕ�������s�������邽�߂�YTL::string���p�ӂ���Ă��܂��B
	�X�ɁAEXE-DLL�ԂŎ��R��new&delete�ł���悤�ɁAyaneSDK�ł�new��delete���I�[�o�[���C�h����Ă��܂��B

	YTL::allocator�́AEXE-DLL�ԂŁA���R��allocate&deallocate�ł��܂��B
	yaneSDK�̃I�[�o�[���C�h���ꂽnew&delete���Ăяo�������̊ȒP�ȃN���X�ł��B
*/
/**
�@�Ȃ��A
	void destroy(pointer p){ p->~T(); }
�@�̍s�͈ȉ��̃v���O������VC6�ł̃R���p�C�����m�F���Ă���̂ŁA
�@char��long��int�̏ꍇ�ł����Ȃ��R���p�C���ł���Ǝv����E�E�E�i�x�����Ȃ��ł����j
> 	template<class T>
> 	void _dest(T * t){
> 		t->~T();
> 	}
> 
> 	void main(){
> 		char *c=new char('c');
> 		std::cout << *c;
> 		_dest(c);
> 		std::cout << *c;
> 		delete c;
> 	}
�@�����x����G���[���o���R���p�C������������AyaneSDK3rd�̌f���Ȃǂ�
�@�������݂��Ă���������Ɗ������ł��E�E�E
*/
#ifndef __YTLAllocator_h__
#define __YTLAllocator_h__

#include <memory>

namespace yaneuraoGameSDK3rd {
namespace YTL {
namespace YTLallocator {

template<class T>
class allocator {
public:
	typedef size_t		size_type;
	typedef ptrdiff_t	difference_type;
	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;
	typedef T			value_type;

	template <class U>
		struct rebind { typedef allocator<U> other; };

	pointer address(reference x) const{ return &x; }
	const_pointer address(const_reference x) const{ return &x; }
	allocator(){}
	allocator(const allocator<T> &){}
	template<class T2> allocator<T>& operator=(const allocator<T2> &){}
	pointer allocate(size_type n, const void *hint){ return (pointer)(operator new(n * sizeof(T))); }
	void deallocate(pointer p, size_type n){ operator delete(p); }
	void construct(pointer p, const T& val){ new ((void *)p) T(val); }
	void destroy(pointer p){ p->~T(); }
	size_type max_size() const{
		size_type c = (size_type)(-1) / sizeof(T);
		return (0<c ? c : 1);
	}
};

}	//	end of namespace YTLstring
}	//	end of namespace YTL
}	//	end of namespace yaneuraoGameSDK3rd

#endif
