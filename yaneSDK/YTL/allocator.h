/**
simple allocator class
The STL allocator class has different implementations between VC++6 and 7 (.NET)
Prepare by yourself.
(Because the implementation of STLPort is not always immutable)

implemented by coLun(Y.Imamura) '03/06/19
*/
/**
yaneSDK3rd supports calling classes implemented on the DLL side.
In addition, YTL::string is provided for freely passing strings between EXE-DLLs.
Furthermore, new and delete are overridden in yaneSDK so that new&delete can be freely performed between EXE-DLLs.

YTL::allocator can be freely allocated and deallocated between EXE-DLLs.
It is a simple class that just calls overridden new&delete of yaneSDK.
*/
/**
 note that,
void destroy(pointer p){ p->~T();
The line confirms the compilation of the following program with VC6, so
It seems that you can compile without problems even for char, long and int... (There was no warning)
> template<class T>
> void _dest(T * t){
> t->~T();
> }
>
> void main(){
> char *c=new char('c');
> std::cout << *c;
> _dest(c);
> std::cout << *c;
> delete c;
> }
If there is a compiler that issues warnings or errors, go to the yaneSDK3rd bulletin board, etc.
"I would appreciate it if you could write it down..."
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
