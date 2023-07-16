/*
	function_callback	: versatile function callbacker
		reprogrammed & desinged by yaneurao(M.Isozaki) '02/03/08

	参考記事：
		『InsideWindows』1999年8月号
		επιστημη氏「任意の関数をスレッドに」
*/

#ifndef __YTLFunctionCallback_h__
#define __YTLFunctionCallback_h__

namespace yaneuraoGameSDK3rd {
namespace YTL {

class function_callback {
/**
	関数のコールバック用基底クラス
	こいつの、smart_ptrを相手に渡して、
	相手からは、run()を呼び出してもらえればそれで良い

	例)
		smart_ptr<function_callback_r>
			s(function_callback_r<int>::Create(&test,1,10));
		//	コールバックオブジェクトが完成
		int nResult = s->run();
		//	int test(int n1,int n2)という関数をtest(1,10)と呼び出す

		function_callback_rg2のrg2とは、
			r : result有り	vならばresult無し
			g : グローバルな関数 mならばmember関数
			2 : 関数の引数は２つ
		の意味です。

		引数は5つのテンプレートまで用意してあります。
		それ以上については、自前で用意してください。

		例：
		smart_ptr<function_callback_v>
			s(function_callback_v::Create(&CHoge::test,this,1,10));
		//	コールバックオブジェクトが完成
		s->run();
		//	void CHoge::test(int n1,int n2)という関数をtest(1,10)と呼び出す

		function_callback_vもfunction_callback_rも、
		このfunction_callback派生クラスなので、戻り値はどうでも良いのならば
		受け取るのは、smart_ptr<function_callback>でも構いません。

		void test(int n)という関数をtest(m)とコールバックするならば、
		smart_ptr<function_callback> f(function_callback_v::Create(&test,m));
		f->run();

		//	int CHoge::test(int n1,int n2,int n3)という関数をtest(1,10,100)と呼び出す
		smart_ptr<function_callback_r<int> >
			s(function_callback_r<int>::Create(&CHoge::test,this,1,10,100));
		s->run();
		int r = s->GetResult();	//	これで返し値を取得

		//	グローバルな関数int	SukiSukiSakura(int n1,int n2)を呼び出す場合
		smart_ptr<function_callback_r<int> >
			s(function_callback_r<int>::Create(&SukiSukiSakura,2,3));
		s->run();
		int r = s->GetResult();	//	これで返し値を取得

	ただし、非const参照を受け取る関数の場合、function_callbackは値をコピーで保持するため
	期待される動作は行われない。
	例)
		//	void onCallback(int& i) { i++; } という関数があるとする。
		int n;
		int& rn = n;
		function_callback_v::Create(&onCallback, rn)->run();	//	nは変更されない


	class ITask{
		void kicker(const Hoge& arg) { Run(arg); }
	};
	このようなconst&メンバも呼び出せるように修正。('03/07/02)


	thisだけbindしなおすことが出来る機構を追加('03/07/06)
*/
public:
	virtual	void run() = 0;
	virtual ~function_callback(){}

	/**
		コールバックするthisポインタだけあとで変更する機能
		・非staticなメンバ関数のコールバックを保持していないときは
		CRuntimeException例外が発生する
		・それ以上の型チェックは行なわない
	*/

	template <class T>
	void bind(T* t){ inner_bind((void*)t); }

protected:
	//	ディフォルトではCRuntimeExceptionを投げる
	virtual void inner_bind(void*){
#ifdef USE_EXCEPTION
		throw CRuntimeException(
			"メンバ関数へのfunction_callbackに対してbindを行なった"
		);
#endif
	}
};

class function_callback_v : public function_callback {
///		返し値を持たないバージョン
public:
	//	ヘルパ関数
	static function_callback_v* Create(void (*f)());

	template <class Arg1,class A1>
	static function_callback_v* Create(void (*f)(A1), const Arg1& a1){
			return new function_callback_vg1<A1,Arg1>(f,a1);
	}
	template <class Arg1,class Arg2,class A1,class A2>
	static function_callback_v* Create(void (*f)(A1,A2), const Arg1& a1, const Arg2& a2){
			return new function_callback_vg2<A1,A2,Arg1,Arg2>(f,a1,a2);
	}
	template <class Arg1,class Arg2,class Arg3,class A1,class A2,class A3>
	static function_callback_v* Create(void (*f)(A1,A2,A3),
		 const Arg1& a1, const Arg2& a2, const Arg3& a3){
			return new function_callback_vg3<A1,A2,A3,Arg1,Arg2,Arg3>(f,a1,a2);
	}
	template <class Arg1,class Arg2,class Arg3,class Arg4
			,class A1,class A2,class A3,class A4>
	static function_callback_v* Create(void (*f)(A1,A2,A3,A4),
		 const Arg1& a1, const Arg2& a2, const Arg3& a3, const Arg4& a4){
			return new function_callback_vg4<A1,A2,A3,A4,Arg1,Arg2,Arg3,Arg4>
				(f,a1,a2,a3,a4);
	}
	template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5
			,class A1,class A2,class A3,class A4,class A5>
	static function_callback_v* Create(void (*f)(A1,A2,A3,A4,A5),
		 const Arg1& a1, const Arg2& a2, const Arg3& a3,
		 const Arg4& a4, const Arg5& a5){
			return new function_callback_vg5<A1,A2,A3,A4,A5,Arg1
				,Arg2,Arg3,Arg4,Arg5>
				(f,a1,a2,a3,a4,a5);
	}
	template <class obj>
	static function_callback_v* Create(void (obj::*f)(),obj* o){
			return new function_callback_vm0<obj>(f,o);
	}
	template <class obj,class Arg1,class A1>
	static function_callback_v* Create(void (obj::*f)(A1),obj* o, const Arg1& a1){
			return new function_callback_vm1<obj,A1,Arg1>(f,o,a1);
	}
	template <class obj,class Arg1,class Arg2,class A1,class A2>
	static function_callback_v* Create(void (obj::*f)(A1,A2),obj* o,
		 const Arg1& a1, const Arg2& a2){
			return new function_callback_vm2<obj,A1,A2,Arg1,Arg2>(f,o,a1,a2);
	}
	template <class obj,class Arg1,class Arg2,class Arg3,
			class A1,class A2,class A3>
	static function_callback_v* Create(void (obj::*f)(A1,A2,A3),obj* o,
		 const Arg1& a1, const Arg2& a2, const Arg3& a3){
			return new function_callback_vm3<obj,A1,A2,A3,Arg1,Arg2,Arg3>
				(f,o,a1,a2,a3);
	}
	template <class obj,class Arg1,class Arg2,class Arg3,class Arg4,
			class A1,class A2,class A3,class A4>
	static function_callback_v* Create(void (obj::*f)(A1,A2,A3,A4),
		obj* o, const Arg1& a1, const Arg2& a2, const Arg3& a3, const Arg4& a4){
			return new function_callback_vm4<obj,A1,A2,A3,A4,Arg1,Arg2,Arg3,Arg4>
				(f,o,a1,a2,a3,a4);
	}
	template <class obj,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,
			class A1,class A2,class A3,class A4,class A5>
	static function_callback_v* Create(void (obj::*f)(A1,A2,A3,A4,A5),
		obj* o, const Arg1& a1, const Arg2& a2, const Arg3& a3, const Arg4& a4, const Arg5& a5){
			return new function_callback_vm5<obj,A1,A2,A3,A4,A5
				,Arg1,Arg2,Arg3,Arg4,Arg5>(f,o,a1,a2,a3,a4,a5);
	}
};

template <class Result>
class function_callback_r : public function_callback {
///		返し値をテンプレート化されたバージョン
public:
	Result GetResult() const { return _r; }

	//	ヘルパ関数
	static function_callback_r* Create(Result (*f)());
	template <class Arg1,class A1>
	static function_callback_r* Create(Result (*f)(A1), const Arg1& a1){
			return new function_callback_rg1<Result,A1,Arg1>(f,a1);
	}
	template <class Arg1,class Arg2,class A1,class A2>
	static function_callback_r* Create(Result (*f)(A1,A2),
		 const Arg1& a1, const Arg2& a2){
			return new function_callback_rg2<Result,A1,A2,Arg1,Arg2>(f,a1,a2);
	}
	template <class Arg1,class Arg2,class Arg3,
		class A1,class A2,class A3>
	static function_callback_r* Create(Result (*f)(A1,A2,A3),
		 const Arg1& a1, const Arg2& a2, const Arg3& a3){
			return new function_callback_rg3<Result,A1,A2,A3
			,Arg1,Arg2,Arg3>(f,a1,a2);
	}
	template <class Arg1,class Arg2,class Arg3,class Arg4,
		class A1,class A2,class A3,class A4>
	static function_callback_r* Create(Result (*f)(A1,A2,A3,A4),
		 const Arg1& a1, const Arg2& a2, const Arg3& a3, const Arg4& a4){
			return new function_callback_rg4<Result,A1,A2,A3,A4
			,Arg1,Arg2,Arg3,Arg4>(f,a1,a2,a3,a4);
	}
	template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,
		class A1,class A2,class A3,class A4,class A5>
	static function_callback_r* Create(Result (*f)(A1,A2,A3,A4,A5),
		 const Arg1& a1, const Arg2& a2, const Arg3& a3, const Arg4& a4, const Arg5& a5){
			return new function_callback_rg5<Result,A1,A2,A3,A4,A5
			,Arg1,Arg2,Arg3,Arg4,Arg5>(func f,Arg1 a1,Arg2 a2,Arg3 a3,Arg4 a4,Arg5 a5);
	}
	template <class obj>
	static function_callback_r* Create(Result (obj::*f)(),obj* o){
			return new function_callback_rm0<Result,obj>(f,o);
	}
	template <class obj,class Arg1,class A1>
	static function_callback_r* Create(Result (obj::*f)(A1),obj* o, const Arg1& a1){
			return new function_callback_rm1<Result,obj,A1,Arg1>(f,o,a1);
	}
	template <class obj,class Arg1,class Arg2,class A1,class A2>
	static function_callback_r* Create(Result (obj::*f)(A1,A2),obj* o,
		 const Arg1& a1, const Arg2& a2){
			return new function_callback_rm2<Result,obj,A1,A2,Arg1,Arg2>
				(f,o,a1,a2);
	}
	template <class obj,class Arg1,class Arg2,class Arg3,
		class A1,class A2,class A3>
	static function_callback_r* Create(Result (obj::*f)(A1,A2,A3),obj* o,
		 const Arg1& a1, const Arg2& a2, const Arg3& a3){
			return new function_callback_rm3<Result,obj,A1,A2,A3
			,Arg1,Arg2,Arg3>(f,o,a1,a2,a3);
	}
	template <class obj,class Arg1,class Arg2,class Arg3,class Arg4,
		class A1,class A2,class A3,class A4>
	static function_callback_r* Create(Result (obj::*f)(A1,A2,A3,A4),
		obj* o, const Arg1& a1, const Arg2& a2, const Arg3& a3, const Arg4& a4){
			return new function_callback_rm4<Result,obj,A1,A2,A3,A4
			,Arg1,Arg2,Arg3,Arg4>(f,o,a1,a2,a3,a4);
	}
	template <class obj,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,
		class A1,class A2,class A3,class A4,class A5>
	static function_callback_r* Create(Result (obj::*f)(A1,A2,A3,A4,A5),
		obj* o, const Arg1& a1, const Arg2& a2, const Arg3& a3, const Arg4& a4, const Arg5& a5){
			return new function_callback_rm5<Result,obj,A1,A2,A3,A4,A5
			,Arg1,Arg2,Arg3,Arg4,Arg5>(f,o,a1,a2,a3,a4,a5);
	}

protected:
	Result _r;
};

////////////////////////////////////////////////////////
//	あとは、上記の、
//		リザルトの有無×引数の数(0-5)×メンバ関数or非メンバ関数
//	のバリエーション　＝　２４個　(;´Д`) 

//---------	リザルト無し	グローバル関数
class function_callback_vg0 : public function_callback_v {
//	void グローバル関数　引数0
public:
	typedef void (*function_type) ();
	function_callback_vg0(function_type f) : _f(f) {}
	virtual void run(){ _f(); }
protected:
	function_type _f;
};

template <class A1,class Arg1>
class function_callback_vg1 : public function_callback_v {
//	void グローバル関数　引数1
public:
	typedef void (*function_type) (A1);
	function_callback_vg1(function_type f,Arg1 a1) : _f(f),_a1(a1) {}
	virtual void run(){ _f(_a1); }
protected:
	function_type _f;
	Arg1 _a1;
};

template <class A1,class A2,class Arg1,class Arg2>
class function_callback_vg2 : public function_callback_v {
//	void グローバル関数　引数2
public:
	typedef void (*function_type) (A1,A2);
	function_callback_vg2(function_type f,Arg1 a1,Arg2 a2)
		: _f(f),_a1(a1),_a2(a2) {}
	virtual void run(){ _f(_a1,_a2); }
protected:
	function_type _f;
	Arg1 _a1;
	Arg2 _a2;
};

template <class A1,class A2,class A3,class Arg1,class Arg2,class Arg3>
class function_callback_vg3 : public function_callback_v {
//	void グローバル関数　引数3
public:
	typedef void (*function_type) (A1,A2,A3);
	function_callback_vg3(function_type f,Arg1 a1,Arg2 a2,Arg3 a3)
		: _f(f),_a1(a1),_a2(a2),_a3(a3) {}
	virtual void run(){ _f(_a1,_a2,_a3); }
protected:
	function_type _f;
	Arg1 _a1;
	Arg2 _a2;
	Arg3 _a3;
};

template <class A1,class A2,class A3,class A4,
	class Arg1,class Arg2,class Arg3,class Arg4>
class function_callback_vg4 : public function_callback_v {
//	void グローバル関数　引数4
public:
	typedef void (*function_type) (A1,A2,A3,A4);
	function_callback_vg4(function_type f,Arg1 a1,Arg2 a2,Arg3 a3,Arg4 a4)
		: _f(f),_a1(a1),_a2(a2),_a3(a3),_a4(a4) {}
	virtual void run(){ _f(_a1,_a2,_a3,_a4); }
protected:
	function_type _f;
	Arg1 _a1;
	Arg2 _a2;
	Arg3 _a3;
	Arg4 _a4;
};

template <class A1,class A2,class A3,class A4,class A5,
	class Arg1,class Arg2,class Arg3,class Arg4,class Arg5>
class function_callback_vg5 : public function_callback_v {
//	void グローバル関数　引数5
public:
	typedef void (*function_type) (A1,A2,A3,A4,A5);
	function_callback_vg5(function_type f,Arg1 a1,Arg2 a2,Arg3 a3
		,Arg4 a4,Arg5 a5)
		: _f(f),_a1(a1),_a2(a2),_a3(a3),_a4(a4),_a5(a5) {}
	virtual void run(){ _f(_a1,_a2,_a3,_a4,_a5); }
protected:
	function_type _f;
	Arg1 _a1;
	Arg2 _a2;
	Arg3 _a3;
	Arg4 _a4;
	Arg5 _a5;
};

//---------	リザルト有り	グローバル関数
template <class Result>
class function_callback_rg0 : public function_callback_r<Result> {
//	void グローバル関数　引数無し
public:
	typedef Result (*function_type) ();
	function_callback_rg0(function_type f) : _f(f) {}
	virtual void run(){ _r = _f(); }
protected:
	function_type _f;
};

template <class Result,class A1,class Arg1>
class function_callback_rg1 : public function_callback_r<Result> {
//	void グローバル関数　引数1
public:
	typedef Result (*function_type) (A1);
	function_callback_rg1(function_type f,Arg1 a1) : _f(f),_a1(a1) {}
	virtual void run(){ _r = _f(_a1); }
protected:
	function_type _f;
	Arg1 _a1;
};

template <class Result,class A1,class A2,class Arg1,class Arg2>
class function_callback_rg2 : public function_callback_r<Result> {
//	void グローバル関数　引数2
public:
	typedef Result (*function_type) (A1,A2);
	function_callback_rg2(function_type f,Arg1 a1,Arg2 a2)
		: _f(f),_a1(a1),_a2(a2) {}
	virtual void run(){ _r = _f(_a1,_a2); }
protected:
	function_type _f;
	Arg1 _a1;
	Arg2 _a2;
};

template <class Result,class A1,class A2,class A3,class Arg1,class Arg2,class Arg3>
class function_callback_rg3 : public function_callback_r<Result> {
//	void グローバル関数　引数3
public:
	typedef Result (*function_type) (A1,A2,A3);
	function_callback_rg3(function_type f,Arg1 a1,Arg2 a2,Arg3 a3)
		: _f(f),_a1(a1),_a2(a2),_a3(a3) {}
	virtual void run(){ _r=_f(_a1,_a2,_a3); }
protected:
	function_type _f;
	Arg1 _a1;
	Arg2 _a2;
	Arg3 _a3;
};

template <class Result,class A1,class A2,class A3,class A4,
	class Arg1,class Arg2,class Arg3,class Arg4>
class function_callback_rg4 : public function_callback_r<Result> {
//	void グローバル関数　引数4
public:
	typedef Result (*function_type) (A1,A2,A3,A4);
	function_callback_rg4(function_type f,Arg1 a1,Arg2 a2,Arg3 a3,Arg4 a4)
		: _f(f),_a1(a1),_a2(a2),_a3(a3),_a4(a4) {}
	virtual void run(){ _r=_f(_a1,_a2,_a3,_a4); }
protected:
	function_type _f;
	Arg1 _a1;
	Arg2 _a2;
	Arg3 _a3;
	Arg4 _a4;
};

template <class Result,class A1,class A2,class A3,class A4,class A5,
	class Arg1,class Arg2,class Arg3,class Arg4,class Arg5>
class function_callback_rg5 : public function_callback_r<Result> {
//	void グローバル関数　引数5
public:
	typedef Result (*function_type) (A1,A2,A3,A4,A5);
	function_callback_rg5(function_type f,Arg1 a1,Arg2 a2,Arg3 a3
		,Arg4 a4,Arg5 a5)
		: _f(f),_a1(a1),_a2(a2),_a3(a3),_a4(a4),_a5(a5) {}
	virtual void run(){ _r=_f(_a1,_a2,_a3,_a4,_a5); }
protected:
	function_type _f;
	Arg1 _a1;
	Arg2 _a2;
	Arg3 _a3;
	Arg4 _a4;
	Arg5 _a5;
};

//---------	リザルト無し	メンバ関数
template <class ObjectType>
class function_callback_vm0 : public function_callback_v {
//	void メンバ関数　引数0
public:
	typedef void (ObjectType::*function_type) ();
	function_callback_vm0(function_type f,ObjectType*o) : _f(f),_o(o) {}
	virtual void run(){ (_o->*_f)(); }
protected:
	virtual void inner_bind(void*p){ _o = (ObjectType*)p; }
	ObjectType* _o;	//	こいつが先頭でないと書き換えできない
	function_type _f;
};

template <class ObjectType,class A1,class Arg1>
class function_callback_vm1 : public function_callback_v {
//	void メンバ関数　引数1
public:
	typedef void (ObjectType::*function_type) (A1);
	function_callback_vm1(function_type f,ObjectType*o,Arg1 a1)
		: _f(f),_o(o),_a1(a1) {}
	virtual void run(){ (_o->*_f)(_a1); }
protected:
	virtual void inner_bind(void*p){ _o = (ObjectType*)p; }
	ObjectType* _o;
	function_type _f;
	Arg1 _a1;
};

template <class ObjectType,class A1,class A2,class Arg1,class Arg2>
class function_callback_vm2 : public function_callback_v {
//	void メンバ関数　引数2
public:
	typedef void (ObjectType::*function_type) (A1,A2);
	function_callback_vm2(function_type f,ObjectType*o,Arg1 a1,Arg2 a2)
		: _f(f),_o(o),_a1(a1),_a2(a2) {}
	virtual void run(){ (_o->*_f)(_a1,_a2); }
protected:
	virtual void inner_bind(void*p){ _o = (ObjectType*)p; }
	ObjectType* _o;
	function_type _f;
	Arg1 _a1;
	Arg2 _a2;
};

template <class ObjectType,class A1,class A2,class A3,
	class Arg1,class Arg2,class Arg3>
class function_callback_vm3 : public function_callback_v {
//	void メンバ関数　引数3
public:
	typedef void (ObjectType::*function_type) (A1,A2,A3);
	function_callback_vm3(function_type f,ObjectType*o,Arg1 a1,Arg2 a2,Arg3 a3)
		: _f(f),_o(o),_a1(a1),_a2(a2),_a3(a3) {}
	virtual void run(){ (_o->*_f)(_a1,_a2,_a3); }
protected:
	virtual void inner_bind(void*p){ _o = (ObjectType*)p; }
	ObjectType* _o;
	function_type _f;
	Arg1 _a1;
	Arg2 _a2;
	Arg3 _a3;
};

template <class ObjectType,class A1,class A2,class A3,class A4,
	class Arg1,class Arg2,class Arg3,class Arg4>
class function_callback_vm4 : public function_callback_v {
//	void メンバ関数　引数4
public:
	typedef void (ObjectType::*function_type) (A1,A2,A3,A4);
	function_callback_vm4(function_type f,ObjectType*o,Arg1 a1,Arg2 a2
		,Arg3 a3,Arg4 a4)
		: _f(f),_o(o),_a1(a1),_a2(a2),_a3(a3),_a4(a4) {}
	virtual void run(){ (_o->*_f)(_a1,_a2,_a3,_a4); }
protected:
	virtual void inner_bind(void*p){ _o = (ObjectType*)p; }
	ObjectType* _o;
	function_type _f;
	Arg1 _a1;
	Arg2 _a2;
	Arg3 _a3;
	Arg4 _a4;
};

template <class ObjectType,class A1,class A2,class A3,class A4,class A5,
	class Arg1,class Arg2,class Arg3,class Arg4,class Arg5>
class function_callback_vm5 : public function_callback_v {
//	void メンバ関数　引数5
public:
	typedef void (ObjectType::*function_type) (A1,A2,A3,A4,A5);
	function_callback_vm5(function_type f,ObjectType*o,Arg1 a1,Arg2 a2
		,Arg3 a3,Arg4 a4,Arg5 a5)
		: _f(f),_o(o),_a1(a1),_a2(a2),_a3(a3),_a4(a4),_a5(a5) {}
	virtual void run(){ (_o->*_f)(_a1,_a2,_a3,_a4,_a5); }
protected:
	virtual void inner_bind(void*p){ _o = (ObjectType*)p; }
	ObjectType* _o;
	function_type _f;
	Arg1 _a1;
	Arg2 _a2;
	Arg3 _a3;
	Arg4 _a4;
	Arg5 _a5;
};

//---------	リザルト有り	メンバ関数
template <class Result,class ObjectType>
class function_callback_rm0 : public function_callback_r<Result> {
//	void グローバル関数　引数無し
public:
	typedef Result (ObjectType::*function_type) ();
	function_callback_rm0(function_type f,ObjectType*o) : _f(f),_o(o) {}
	virtual void run(){ _r = (_o->*_f)(); }
protected:
	virtual void inner_bind(void*p){ _o = (ObjectType*)p; }
	ObjectType* _o;
	function_type _f;
};

template <class Result,class ObjectType,class A1,class Arg1>
class function_callback_rm1 : public function_callback_r<Result> {
//	void メンバ関数　引数1
public:
	typedef Result (ObjectType::*function_type) (A1);
	function_callback_rm1(function_type f,ObjectType*o,Arg1 a1)
		: _f(f),_o(o),_a1(a1) {}
	virtual void run(){ _r = (_o->*_f)(_a1); }
protected:
	virtual void inner_bind(void*p){ _o = (ObjectType*)p; }
	ObjectType* _o;
	function_type _f;
	Arg1 _a1;
};

template <class Result,class ObjectType,class A1,class A2,class Arg1,class Arg2>
class function_callback_rm2 : public function_callback_r<Result> {
//	void メンバ関数　引数2
public:
	typedef Result (ObjectType::*function_type) (A1,A2);
	function_callback_rm2(function_type f,ObjectType*o,Arg1 a1,Arg2 a2)
		: _f(f),_o(o),_a1(a1),_a2(a2) {}
	virtual void run(){ _r = (_o->*_f)(_a1,_a2); }
protected:
	virtual void inner_bind(void*p){ _o = (ObjectType*)p; }
	ObjectType* _o;
	function_type _f;
	Arg1 _a1;
	Arg2 _a2;
};

template <class Result,class ObjectType,class A1,class A2,class A3,
	class Arg1,class Arg2,class Arg3>
class function_callback_rm3 : public function_callback_r<Result> {
//	void メンバ関数　引数3
public:
	typedef Result (ObjectType::*function_type) (A1,A2,A3);
	function_callback_rm3(function_type f,ObjectType*o,Arg1 a1,Arg2 a2,Arg3 a3)
		: _f(f),_o(o),_a1(a1),_a2(a2),_a3(a3) {}
	virtual void run(){ _r=(_o->*_f)(_a1,_a2,_a3); }
protected:
	virtual void inner_bind(void*p){ _o = (ObjectType*)p; }
	ObjectType* _o;
	function_type _f;
	Arg1 _a1;
	Arg2 _a2;
	Arg3 _a3;
};

template <class Result,class ObjectType,class A1,class A2,class A3,class A4,
	class Arg1,class Arg2,class Arg3,class Arg4>
class function_callback_rm4 : public function_callback_r<Result> {
//	void メンバ関数　引数4
public:
	typedef Result (ObjectType::*function_type) (A1,A2,A3,A4);
	function_callback_rm4(function_type f,ObjectType*o,
		Arg1 a1,Arg2 a2,Arg3 a3,Arg4 a4)
		: _f(f),_o(o),_a1(a1),_a2(a2),_a3(a3),_a4(a4) {}
	virtual void run(){ _r=(_o->*_f)(_a1,_a2,_a3,_a4); }
protected:
	virtual void inner_bind(void*p){ _o = (ObjectType*)p; }
	ObjectType* _o;
	function_type _f;
	Arg1 _a1;
	Arg2 _a2;
	Arg3 _a3;
	Arg4 _a4;
};

template <class Result,class ObjectType,class A1,class A2,class A3,class A4,
	class A5,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5>
class function_callback_rm5 : public function_callback_r<Result> {
//	void メンバ関数　引数5
public:
	typedef Result (ObjectType::*function_type) (A1,A2,A3,A4,A5);
	function_callback_rm5(function_type f,ObjectType*o,
		Arg1 a1,Arg2 a2,Arg3 a3,Arg4 a4,Arg5 a5)
		: _f(f),_o(o),_a1(a1),_a2(a2),_a3(a3),_a4(a4),_a5(a5) {}
	virtual void run(){ _r=(_o->*_f)(_a1,_a2,_a3,_a4,_a5); }
protected:
	virtual void inner_bind(void*p){ _o = (ObjectType*)p; }
	ObjectType* _o;
	function_type _f;
	Arg1 _a1;
	Arg2 _a2;
	Arg3 _a3;
	Arg4 _a4;
	Arg5 _a5;
};

////////////////////////////////////////////////////////

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
