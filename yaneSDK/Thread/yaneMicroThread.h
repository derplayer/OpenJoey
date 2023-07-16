
#ifndef __yaneMicroThread_h__
#define __yaneMicroThread_h__


#include "../Auxiliary/yaneAllocator.h"
#include "../Thread/yaneThread.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

//#define __BORLANDC__
	//	��VC��BCB5�Œʂ邩�f�o�b�O����Ƃ��̃V���{����`

#ifdef USE_EXCEPTION
//	��O�������L���łȂ���΁A���̃N���X���̖����ɂ���

class CMicroThread {
/**
	���̃N���X��ʂ��ČĂяo�����֐��́A�֐��̓r����resume���A
	���ɌĂяo���ꂽ�Ƃ��ɁA�ȑO�̏�Ԃ��Č����邱�Ƃ��o����

	�v���O������

	struct CCharacter {
		void	OnMove(){
			if (!microthread_.isEnd()){
				if (!microthread_.isSuspended()) {
					smart_ptr<function_callback> fn(function_callback_v::Create(&CCharacter::MicroThread));
					microthread_.start(fn);
				} else {
					microthread_.resume();
				}
			}
		}
		void MicroThread(){
			int i,x=100,y=200;
			for(i=0;i<100;++i) { x+=2; suspend(); }
			for(i=0;i<100;++i) { y+=2; suspend(); }
		}
	protected:
		void suspend() { microthread_.suspend(); }
		CMicroThread microthread_;
	};

	���̂悤�ɂ���΁AOnMove���Ăяo�����ƂɁA
	x+=2��100��OnMove���Ăяo���ԁA�J��Ԃ�
	101��ڂ�OnMove�̌Ăяo������́Ay+=2�����s����B
	�����100��J��Ԃ��ă}�C�N���X���b�h�͏I�����܂��B
	(�ȍ~�AOnMove���Ăяo���Ă������N����܂���)

	���ӁF��O���������邽�߂ɂ́A�}�C�N���X���b�h�̃X�^�b�N��
		OS�̊Ǘ�����X�^�b�N�̈�ɂ����Ȃ���΂Ȃ�Ȃ��B
		(Win32�\������O�̐���)
		�����ŁA�X�^�b�N�̈��Ƀ}�C�N���X���b�h�̃X�^�b�N���m�ۂ���
		���K�v������B

		���̂��߂ɂ́ˁ@class CMemoryAllocatorPool
		���@�p����Ɨǂ��B
		��)
			const int stack_size = 1024*128; // 128k
			BYTE local_stack[stack_size]; // 128k
			CMemoryAllocatorPool pool;
			pool.SetMemory(&local_stack[0],stack_size);

			�X�^�b�N�̈��Ɋm�ۂ��ꂽ�z����A
			CMemoryAllocatorPool�ɐH�킹�āA�����
			CMicroThread�̃R���X�g���N�^�œn���B

		��������΁ACMicroThread�̃f�X�g���N�^�ŁAsuspend����
		�}�C�N���X���b�h����CMicroThreadQuitException��O�𔭐������A
		�J�n�n�_�܂ŃX�^�b�N�̊����߂�(�f�X�g���N�^�̌Ăяo��)��
		�s�Ȃ����Ƃ�ۏ؂ł���B

		�܂�A�}�C�N���X���b�h��suspend���ɏI��������ꍇ�ł��A
		��O�����ɂ���Đ���ɃX�^�b�N�̊����߂����N���邱�Ƃ��ۏ؂����B

		����āA���̃N���X�𗘗p����ɂ͗�O�������L���łȂ���΂Ȃ�Ȃ��I

*/
public:

	void	start(const smart_ptr<function_callback>& fn,int nStackSize=0x8000);
	/**
		�}�C�N���X���b�h�̎��s���J�n����B
		�X�^�b�N�T�C�Y�̓f�B�t�H���g��0x8000�o�C�g�B
		��������Ă��Ȃ���Ԃ�suspend�Ƃ�resume�Ƃ��Ăяo���Ă͂����Ȃ��B

		�����Ȋ֐��ł���������0x2000�ȏ�͕K�v�B
		debug�r���h���Ə璷�ȃR�[�h�ɂȂ�̂ŁA�ǂ����Ă�..
		�idebug�r���h��release�r���h�ŃT�C�Y��؂�ւ��Ă��ǂ��j
	*/

	void	suspend();		///	���s���̏����𒆒f����
	void	sleep(int n);	/// ���s���̏�����n�񒆒f����Bsuspend��n��Ăяo���ƍl����Ηǂ�
	void	resume();		///	���f�����������ĊJ����

	///	�T�X�y���h�����H
	bool	isSuspended() const { return bSuspended_;}

	///	�}�C�N���X���b�h�̎��s�͏I���������H
	bool	isEnd() const { return bEnd_; }

	/**
		���݃}�C�N���X���b�h���g�p���Ă���X�^�b�N�T�C�Y��Ԃ�
		(�}�C�N���X���b�h������ł��A�O����ł��Ăяo�����Ƃ��o����)
		�}�C�N���X���b�h������Ăяo���āA�ǂꂭ�炢�̃�������H���Ă���̂�
		�`�F�b�N���Ă������Ƃ������߂��܂��B(Debug�r���h����Release�r���h���Ƃ�
		���炭�Ⴄ�̂Œ��ӂ��K�v)
	*/
	int		GetUseStackSize() const;

	void	SetMemoryAllocator(IMemoryAllocator*pAllocator) {
		pAllocator_=pAllocator;
	}
	///	�R���X�g���N�^�Őݒ肵���тꂽ�A���P�[�^�����Ƃ���ݒ肷�邽�߂̊֐�

	static void	SetDefaultMemoryAllocator(IMemoryAllocator*pAllocator) {
		pDefaultAllocator_=pAllocator;
	}
	static IMemoryAllocator* GetDefaultMemoryAllocator(){
		return pDefaultAllocator_;
	}

	CMicroThread(IMemoryAllocator*pAllocator=NULL)
		:bSuspended_(false),bEnd_(false)
		,pAllocator_(pAllocator),abyStack_(NULL) {}

	virtual ~CMicroThread();

protected:
	BYTE*		abyStack_;	//	�X�^�b�N
	int			anStackSize_;
	void		ReleaseStack();

	smart_ptr<function_callback> fn_; // �R�[���o�b�N���function

	bool			bSuspended_;	//	�}�C�N���X���b�h�̃T�X�y���h
	bool			bEnd_;			//	�}�C�N���X���b�h�̏I���t���O
	BYTE*			register_esp_;			//	�L�^���Ă���esp
	IMemoryAllocator* pAllocator_;

	static void	QuitFunction();		//	�A�Z���u������Ăяo����O�𔭐�����֐�

//		BCB�ł́Ainline�֐�����_asm���g���Ȃ��̂�(;�L�D`)
#if defined(yaneSDK_BCC_Mode)||defined(yaneSDK_CodeWarrior_Mode)
	void	SwitchThread();
#else
	#ifdef yaneSDK_MSVC_Mode
	#pragma warning(disable:4731)	//	ebp�ύX�������炢�Ŷ����������Ȃ�(߄t�)��٧
	#pragma warning(disable:4733)	//	fs:[0]�ύX�������炢�Ŷ����������Ȃ�(߄t�)��٧
	#endif
	void	SwitchThread(){
		_asm {
			//	���݂̉��z�X�^�b�N�ɐς�
			push	offset Next
			//	resume�̂��Ƃ����ɖ߂��Ă���K�v������̂�

			push	ebx
			push	ebp
			push	esi
			push	edi

			push	dword ptr fs:[0]	//	��O�����̂��߂ɕK�v

			mov		eax,[this]
			xchg	esp,[eax].register_esp_

			pop		dword ptr fs:[0]	//	��O�����̂��߂ɕK�v

			pop		edi
			pop		esi
			pop		ebp
			pop		ebx

//			pop		eax
//			jmp		eax
			ret
Next:;
		}
	}
#endif
	class CMicroThreadQuitException : public CRuntimeException {};

	static void	InnerCallBack(CMicroThread*p);
	//	fn_�ɃR�[���o�b�N����

	static IMemoryAllocator* pDefaultAllocator_;
};
#endif


} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd

#endif
