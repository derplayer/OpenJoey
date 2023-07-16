/*
	function_callback	: versatile function callbacker
		reprogrammed & desinged by yaneurao(M.Isozaki) '02/03/08

	�Q�l�L���F
		�wInsideWindows�x1999�N8����
		�Ã΃ǃЃуŃʃŎ��u�C�ӂ̊֐����X���b�h�Ɂv
*/

#ifndef __YTLFunctionCallback_h__
#define __YTLFunctionCallback_h__

namespace yaneuraoGameSDK3rd {
namespace YTL {

class function_callback {
/**
	�֐��̃R�[���o�b�N�p���N���X
	�����́Asmart_ptr�𑊎�ɓn���āA
	���肩��́Arun()���Ăяo���Ă��炦��΂���ŗǂ�

	��)
		smart_ptr<function_callback_r>
			s(function_callback_r<int>::Create(&test,1,10));
		//	�R�[���o�b�N�I�u�W�F�N�g������
		int nResult = s->run();
		//	int test(int n1,int n2)�Ƃ����֐���test(1,10)�ƌĂяo��

		function_callback_rg2��rg2�Ƃ́A
			r : result�L��	v�Ȃ��result����
			g : �O���[�o���Ȋ֐� m�Ȃ��member�֐�
			2 : �֐��̈����͂Q��
		�̈Ӗ��ł��B

		������5�̃e���v���[�g�܂ŗp�ӂ��Ă���܂��B
		����ȏ�ɂ��ẮA���O�ŗp�ӂ��Ă��������B

		��F
		smart_ptr<function_callback_v>
			s(function_callback_v::Create(&CHoge::test,this,1,10));
		//	�R�[���o�b�N�I�u�W�F�N�g������
		s->run();
		//	void CHoge::test(int n1,int n2)�Ƃ����֐���test(1,10)�ƌĂяo��

		function_callback_v��function_callback_r���A
		����function_callback�h���N���X�Ȃ̂ŁA�߂�l�͂ǂ��ł��ǂ��̂Ȃ��
		�󂯎��̂́Asmart_ptr<function_callback>�ł��\���܂���B

		void test(int n)�Ƃ����֐���test(m)�ƃR�[���o�b�N����Ȃ�΁A
		smart_ptr<function_callback> f(function_callback_v::Create(&test,m));
		f->run();

		//	int CHoge::test(int n1,int n2,int n3)�Ƃ����֐���test(1,10,100)�ƌĂяo��
		smart_ptr<function_callback_r<int> >
			s(function_callback_r<int>::Create(&CHoge::test,this,1,10,100));
		s->run();
		int r = s->GetResult();	//	����ŕԂ��l���擾

		//	�O���[�o���Ȋ֐�int	SukiSukiSakura(int n1,int n2)���Ăяo���ꍇ
		smart_ptr<function_callback_r<int> >
			s(function_callback_r<int>::Create(&SukiSukiSakura,2,3));
		s->run();
		int r = s->GetResult();	//	����ŕԂ��l���擾

	�������A��const�Q�Ƃ��󂯎��֐��̏ꍇ�Afunction_callback�͒l���R�s�[�ŕێ����邽��
	���҂���铮��͍s���Ȃ��B
	��)
		//	void onCallback(int& i) { i++; } �Ƃ����֐�������Ƃ���B
		int n;
		int& rn = n;
		function_callback_v::Create(&onCallback, rn)->run();	//	n�͕ύX����Ȃ�


	class ITask{
		void kicker(const Hoge& arg) { Run(arg); }
	};
	���̂悤��const&�����o���Ăяo����悤�ɏC���B('03/07/02)


	this����bind���Ȃ������Ƃ��o����@�\��ǉ�('03/07/06)
*/
public:
	virtual	void run() = 0;
	virtual ~function_callback(){}

	/**
		�R�[���o�b�N����this�|�C���^�������ƂŕύX����@�\
		�E��static�ȃ����o�֐��̃R�[���o�b�N��ێ����Ă��Ȃ��Ƃ���
		CRuntimeException��O����������
		�E����ȏ�̌^�`�F�b�N�͍s�Ȃ�Ȃ�
	*/

	template <class T>
	void bind(T* t){ inner_bind((void*)t); }

protected:
	//	�f�B�t�H���g�ł�CRuntimeException�𓊂���
	virtual void inner_bind(void*){
#ifdef USE_EXCEPTION
		throw CRuntimeException(
			"�����o�֐��ւ�function_callback�ɑ΂���bind���s�Ȃ���"
		);
#endif
	}
};

class function_callback_v : public function_callback {
///		�Ԃ��l�������Ȃ��o�[�W����
public:
	//	�w���p�֐�
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
///		�Ԃ��l���e���v���[�g�����ꂽ�o�[�W����
public:
	Result GetResult() const { return _r; }

	//	�w���p�֐�
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
//	���Ƃ́A��L�́A
//		���U���g�̗L���~�����̐�(0-5)�~�����o�֐�or�񃁃��o�֐�
//	�̃o���G�[�V�����@���@�Q�S�@(;�L�D`) 

//---------	���U���g����	�O���[�o���֐�
class function_callback_vg0 : public function_callback_v {
//	void �O���[�o���֐��@����0
public:
	typedef void (*function_type) ();
	function_callback_vg0(function_type f) : _f(f) {}
	virtual void run(){ _f(); }
protected:
	function_type _f;
};

template <class A1,class Arg1>
class function_callback_vg1 : public function_callback_v {
//	void �O���[�o���֐��@����1
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
//	void �O���[�o���֐��@����2
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
//	void �O���[�o���֐��@����3
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
//	void �O���[�o���֐��@����4
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
//	void �O���[�o���֐��@����5
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

//---------	���U���g�L��	�O���[�o���֐�
template <class Result>
class function_callback_rg0 : public function_callback_r<Result> {
//	void �O���[�o���֐��@��������
public:
	typedef Result (*function_type) ();
	function_callback_rg0(function_type f) : _f(f) {}
	virtual void run(){ _r = _f(); }
protected:
	function_type _f;
};

template <class Result,class A1,class Arg1>
class function_callback_rg1 : public function_callback_r<Result> {
//	void �O���[�o���֐��@����1
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
//	void �O���[�o���֐��@����2
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
//	void �O���[�o���֐��@����3
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
//	void �O���[�o���֐��@����4
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
//	void �O���[�o���֐��@����5
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

//---------	���U���g����	�����o�֐�
template <class ObjectType>
class function_callback_vm0 : public function_callback_v {
//	void �����o�֐��@����0
public:
	typedef void (ObjectType::*function_type) ();
	function_callback_vm0(function_type f,ObjectType*o) : _f(f),_o(o) {}
	virtual void run(){ (_o->*_f)(); }
protected:
	virtual void inner_bind(void*p){ _o = (ObjectType*)p; }
	ObjectType* _o;	//	�������擪�łȂ��Ə��������ł��Ȃ�
	function_type _f;
};

template <class ObjectType,class A1,class Arg1>
class function_callback_vm1 : public function_callback_v {
//	void �����o�֐��@����1
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
//	void �����o�֐��@����2
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
//	void �����o�֐��@����3
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
//	void �����o�֐��@����4
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
//	void �����o�֐��@����5
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

//---------	���U���g�L��	�����o�֐�
template <class Result,class ObjectType>
class function_callback_rm0 : public function_callback_r<Result> {
//	void �O���[�o���֐��@��������
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
//	void �����o�֐��@����1
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
//	void �����o�֐��@����2
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
//	void �����o�֐��@����3
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
//	void �����o�֐��@����4
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
//	void �����o�֐��@����5
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
