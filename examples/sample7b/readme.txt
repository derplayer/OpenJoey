yaneuraoGameSDK sample7	---	lambda processor

	���@yaneuraoGameSDK3rd��p���ă_�C�A���O�������T���v��

�E�����_�v���Z�b�T�Ƃ́H

	random_shuffle�̑�R�p�����[�^�Ƃ��Ċ֐��I�u�W�F�N�g���w�肵�܂��B
	�Ƃ��낪�A�����o�֐����֐��I�u�W�F�N�g�ɂ��悤�Ǝv���ƁA

		bind1st(mem_fun1_t<int,CApp,int>(CApp::myrand),this)

	�̂悤�ɁAbind1st��mem_fun1_t�Ƒg�ݍ��킹��K�v���o�Ă��܂��B
	���̂ւ񂪔��ɂ߂�ǂ��������B����ɁA���̂��Ɗ֐��������Ȃ���
	�����܂���B�ʂ̊֐��ɂ������̏�ŁA�������Ə������Ƃ͂ł���̂��H
	�Ƃ����̂��A�����_�v���Z�b�T�̐��쓮�@�ł��B

	��j
		int	(int x,int y) { return x+y; }
	�Ɠ��͂���΁A2�����Ƃ�A��������v�������̂�Ԃ��֐��I�u�W�F�N�g��
	�쐬���܂��B�i�ȉ��̂悤�ɏo�͂���܂��j


//////////////////////////////////////////////////////////////////////////
// lambda�N���X�̒�`
class lambda030416093804_0153 {
public:
		int	 operator()(int x,int y)
{ return x+y; }
};

// �֐��I�u�W�F�N�g(�R�s�y���Ė��ߍ���Ńl)
lambda030416093804_0153()

/** // ���\�[�X
		int	(int x,int y) { return x+y; }

*/
//////////////////////////////////////////////////////////////////////////

	���̂悤�ɏo�͂���܂��B

	���̊֐���p���āA
	int c =	lambda030416093804_0153()(10,25);
	�Ƃ���΁A10+25��35��c�ɑ������܂��B

	���@���̃N���X���́A���̖��O�ɕύX���Ă��\���܂���B
	unique�Ȗ��O�ɂȂ�悤�ɁA����+4���̗����ɂȂ��Ă��܂��B

	���̊֐��I�u�W�F�N�g�̑��݂���X�R�[�v�ɂ���ϐ����A���̊֐��I�u�W�F�N�g����	���R�ɗp���邱�Ƃ��o���܂��B�i�R���X�g���N�^�œn���Ă��邾���ł����j

	��j
		int (int r) { return pThis->Rand() % r; }
		CApp* pThis:this;
		���@CApp*�^��pThis�Ƃ��������o�B�Ăяo�����ł́Athis�������Ƃ��ēn���B
		�i�ϊ���̃\�[�X�����Q�l�ɂ��Ă��������j
		�|�C���^�̏ꍇ�ACApp* �̂悤�ɏ����ACApp *�ƃX�y�[�X���͓���Ȃ����ƁB

//////////////////////////////////////////////////////////////////////////
// lambda�N���X�̒�`
class lambda030416094214_7376 {
public:
lambda030416094214_7376(CApp* Arg1):
	pThis(Arg1){}
int	 operator()(int r)
{ return pThis->Rand() % r; }
protected:
	CApp*	pThis;
};

// �֐��I�u�W�F�N�g(�R�s�y���Ė��ߍ���Ńl)
lambda030416094214_7376(this)

/** // ���\�[�X
int (int r) { return pThis->Rand() % r; }
CApp* pThis:this;

*/
//////////////////////////////////////////////////////////////////////////

	�萔�����Z����I�u�W�F�N�g�𐶐�����
		int (int n) { return n+c; }
		int c;
	�Ƃ���΁A�R���X�g���N�^�Œ萔c���Ƃ�֐��I�u�W�F�N�g�𐶐����܂��B

	�܂�A�֐��I�u�W�F�N�g�̏o�͂��A
//////////////////////////////////////////////////////////////////////////
// �֐��I�u�W�F�N�g(�R�s�y���Ė��ߍ���Ńl)
lambda030416094701_3931(c)
//////////////////////////////////////////////////////////////////////////
	���̂悤�ɂȂ�܂��B

		int (int n) { return n+c; }
		int c:c2;
	���������΁A�R���X�g���N�^�Œ萔c2���Ƃ�֐��I�u�W�F�N�g�𐶐����܂��B

//////////////////////////////////////////////////////////////////////////
// �֐��I�u�W�F�N�g(�R�s�y���Ė��ߍ���Ńl)
lambda030416094734_1538(c2)
//////////////////////////////////////////////////////////////////////////

	�R���X�g���N�^�̈����͖����ɏ����܂��B

	int () { return a+b+c+d; }
	int a;
	int b;
	int c;
	int	d;
	ine e;
	�Ə�����

//////////////////////////////////////////////////////////////////////////
// lambda�N���X�̒�`
class lambda030416094958_2421 {
public:
lambda030416094958_2421(int Arg1,int Arg2,int Arg3,int Arg4,int Arg5):
	a(Arg1),b(Arg2),c(Arg3)
	,d(Arg4),e(Arg5){}
int	 operator()()
{ return a+b+c+d+e; }
protected:
	int	a;
	int	b;
	int	c;
	int	d;
	int	e;
};

// �֐��I�u�W�F�N�g(�R�s�y���Ė��ߍ���Ńl)
lambda030416094958_2421(a,b,c,d,e)

/** // ���\�[�X
int () { return a+b+c+d+e; }
int a;
int b;
int c;
int d;
int e;

*/
//////////////////////////////////////////////////////////////////////////

	�Əo�͂���܂��B

	�^���̂Ƃ���ɁA�Q�ƂƂ�const�ɑΉ����܂����B
	�z��͂ł��܂���B�������..

	�ȉ��ၫ

//////////////////////////////////////////////////////////////////////////
void test(){
// lambda�N���X�̒�`
class lambda030416125256_6729 {
public:
typedef	int*	first_argument_type;
typedef	int*	second_argument_type;
typedef void  result_type;
lambda030416125256_6729(const CApp* Arg1):
	pThis(Arg1){}
void  operator()(int* p ,int* q) const 
{ *p = *p + *q; }
protected:
	const CApp*	pThis;
};

// �֐��I�u�W�F�N�g(�R�s�y���Ė��ߍ���Ńl)
//lambda030416125256_6729(this)

/** // ���\�[�X
void (int* p ,int* q) { *p = *p + *q; }
const CApp* pThis:this;

*/

}
//////////////////////////////////////////////////////////////////////////

