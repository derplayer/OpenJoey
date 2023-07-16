In this folder is the YTL (Yaneurao Template Library)

	exception : set of exceptions

	ref_deleter : reference counted destructor (required for smart_ptr implementation)
	ref_callback_deleter: callback destructor with reference count

	smart_ptr : real smart pointer.
	Arrays and non-arrays can be fully integrated
	Smart pointers more than smart pointers in Java
	smart_vector_ptr : std::vector version of smart pointer
	Å® Refer to Yaneurao HP "Genius Game Programmer Training Cast Chapter 11" smart_list_ptr: std::list version of smart pointer

	list_chain : custom version of std::list

	proxy_ptr : A pointer that will be new only when needed
	// With this, static proxy_ptr cannot be used as singleton...

	singleton : A pointer that starts new when needed
	// use instead of proxy_ptr

	mediator : a virtual implementation of a (Java-ish) inner class
	Å® Refer to Yaneurao HP "Genius Game Programmer Training Cast Chapter 11"
	factory_permutation : template for class permutation
	Å® Please refer to the Yaneurao HP "Genius Game Programmer Training Cast Chapter 3"

	function_callback : Callback object for functions and member functions

	ref_creater : reference counted creator

	string : std::string compatible class

	yaneMacro : Simple macro collection