/**
	simple allocator class
		STLのallocator classは、VC++6と7(.NET)で実装が異なるので
		自前で用意する。
		（STLPortも実装が不変とは限らないので）

	implemented by coLun(Y.Imamura) '03/06/19
*/
/**
	yaneSDK3rdではDLL側で実装されたクラスの呼び出しがサポートされています。
	また、EXE-DLL間で自由に文字列を行き来するためにYTL::stringが用意されています。
	更に、EXE-DLL間で自由にnew&deleteできるように、yaneSDKではnewとdeleteがオーバーライドされています。

	YTL::allocatorは、EXE-DLL間で、自由にallocate&deallocateできます。
	yaneSDKのオーバーライドされたnew&deleteを呼び出すだけの簡単なクラスです。
*/
/**
　なお、
	void destroy(pointer p){ p->~T(); }
　の行は以下のプログラムのVC6でのコンパイルを確認しているので、
　charやlongやintの場合でも問題なくコンパイルできると思われる・・・（警告もなしでした）
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
　もし警告やエラーを出すコンパイラがあったら、yaneSDK3rdの掲示板などへ
　書き込みしていただけると嬉しいです・・・
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
