#include "stdafx.h"
#include "yaneMicroThread.h"
#include <malloc.h>

namespace yaneuraoGameSDK3rd {
namespace Thread {

IMemoryAllocator* CMicroThread::pDefaultAllocator_=NULL;

#ifdef USE_EXCEPTION

	void	CMicroThread::QuitFunction(){
			throw CMicroThreadQuitException();
	}

//	�O�̂��߂ɃR�[�h�𕪗����Ă�����...
#if !defined(yaneSDK_CantUseInlineAssembler)
	void	CMicroThread::ReleaseStack(){
	//	���܁A�}�C�N���X���b�h���s��?
		if (bSuspended_){
			//	�X�^�b�N��؂�ւ������ƁA��O��throw���邩?
			void *fQuitFunction = (void*)&CMicroThread::QuitFunction;
			//	VC++6�̃C�����C���A�Z���u����static�ȃ����o�֐���call��
			//	�����Ă��Ȃ�����(VC++7�ł͉\)
		_asm {
			push	offset Next
			//	resume�̂��Ƃ����ɖ߂��Ă���K�v������̂�

			push	ebx
			push	ebp
			push	esi
			push	edi

			push	dword ptr fs:[0]	//	��O�����̂��߂ɕK�v

			mov		eax,[this]
			xchg	esp,[eax].register_esp_
			mov		eax,fQuitFunction

			//������ �������ŗ�O����������̂ŁA���ۂ́A���̈ȉ��̕����͎��s����邱�Ƃ͂Ȃ�

			pop		dword ptr fs:[0]	//	��O�����̂��߂ɕK�v

			pop		edi
			pop		esi
			pop		ebp
			pop		ebx
//	VC++6�ł̓I�[�o�[���[�h����Ă��Ȃ��O���[�o���֐������C�����C���A�Z���u������Ăяo���Ȃ�(;�L�D`)
			jmp		eax

			//������ �ƁA�������Ƃł����܂ł́A�R�s�y�̎c�[(^^;
Next:;
			}
		}

		if (abyStack_!=NULL){
			if (pAllocator_!=NULL) {	//	�A���P�[�^�Ŋ��蓖�Ă����?
				pAllocator_->Free(abyStack_);
			} else if (pDefaultAllocator_!=NULL) {	//	�f�B�t�H���g�A���P�[�^�Ŋ��蓖�Ă����?
				pDefaultAllocator_->Free(abyStack_);
			} else {
				delete [] abyStack_;
			}
			abyStack_ = NULL;
		}
	}

#else	// !defined(yaneSDK_CantUseInlineAssembler)

	//	BCC5.5�p�Ƀn���h�A�Z���u���o�[�W������p��
	void	CMicroThread::ReleaseStack(){
	//	���܁A�}�C�N���X���b�h���s��?
		if (bSuspended_){
			//	�X�^�b�N��؂�ւ������ƁA��O��throw���邩?

			struct CMyTmp {
				BYTE **my_esp;
				void *fQuitFunction;
			};
			CMyTmp mytmp;
			mytmp.my_esp = &register_esp_;
			mytmp.fQuitFunction = (void*)&CMicroThread::QuitFunction;

		//	handassembled by yaneurao('03/07/06)		
/*
		_asm {
		//	push	offset Next
			//	resume�̂��Ƃ����ɖ߂��Ă���K�v������̂�

			push	ebx
			push	ebp
			push	esi
			push	edi

			push	dword ptr fs:[0]	//	��O�����̂��߂ɕK�v

//			mov		eax,[this]
			mov		ecx,[eax]
			xchg	esp,[ecx]
//			mov		eax,fQuitFunction
			mov		eax,[eax+4]

			//������ �������ŗ�O����������̂ŁA���ۂ́A���̈ȉ��̕����͎��s����邱�Ƃ͂Ȃ�

			pop		dword ptr fs:[0]	//	��O�����̂��߂ɕK�v

			pop		edi
			pop		esi
			pop		ebp
			pop		ebx
//	VC++6�ł̓I�[�o�[���[�h����Ă��Ȃ��O���[�o���֐������C�����C���A�Z���u������Ăяo���Ȃ�(;�L�D`)
			jmp		eax

			//������ �ƁA�������Ƃł����܂ł́A�R�s�y�̎c�[(^^;
Next:;
			}
*/
			static const BYTE code_area [] ={
				0x8B,0x44,0x24,0x04,//      mov         eax,dword ptr [esp+4] 
				0x53,//               push        ebx  
				0x55,//               push        ebp  
				0x56,//               push        esi  
				0x57,//               push        edi  
				0x64,0xFF,0x35,0,0,0,0,// push        dword ptr fs:[0] 
				0x8B,0x08,//            mov         ecx,dword ptr [eax] 
				0x87,0x21,//            xchg        esp,dword ptr [ecx] 
				0x8B,0x40,0x04,//       mov         eax,dword ptr [eax+4] 
				0x64,0x8F,0x05,0,0,0,0,// pop         dword ptr fs:[0] 
				0x5F,//               pop         edi  
				0x5E,//               pop         esi  
				0x5D,//               pop         ebp  
				0x5B,//               pop         ebx  
				0xFF,0xE0//            jmp         eax  
			};
			//reinterpret_cast<void(*)(CMyTmp*)>(&code_area)(&mytmp);
			void(*f)(CMyTmp*) = (void(*)(CMyTmp*))&code_area;
			f(&mytmp);
		}

		if (abyStack_!=NULL){
			if (pAllocator_!=NULL) {	//	�A���P�[�^�Ŋ��蓖�Ă����?
				pAllocator_->Free(abyStack_);
			} else if (pDefaultAllocator_!=NULL) {	//	�f�B�t�H���g�A���P�[�^�Ŋ��蓖�Ă����?
				pDefaultAllocator_->Free(abyStack_);
			} else {
				delete [] abyStack_;
			}
			abyStack_ = NULL;
		}
	}

#endif	// !defined(yaneSDK_CantUseInlineAssembler)

	CMicroThread::~CMicroThread(){
		//	��O�𔭐������āA�����I�ɔ����o���ׂ���?
		ReleaseStack();
	}

//	�O�̂��߂ɃR�[�h�𕪗����Ă�����...
#if !defined(yaneSDK_CantUseInlineAssembler)
	void	CMicroThread::start(const smart_ptr<function_callback>& fn,int nStackSize)
	{
#ifdef USE_EXCEPTION
		if (nStackSize<=32){
			throw CRuntimeException("CMicroThread::start�ŃX�^�b�N���p�ӂ���Ȃ�����");
		}
#endif
		bEnd_ = false;
		ReleaseStack();
		if (pAllocator_!=NULL) {	//	�A���P�[�^�����??
			abyStack_ = pAllocator_->Alloc(nStackSize);
		} else if (pDefaultAllocator_!=NULL){
			abyStack_ = pDefaultAllocator_->Alloc(nStackSize);
		} else {
			abyStack_ = new BYTE[nStackSize];
		}
		anStackSize_ = nStackSize;
		fn_ = fn;
		register_esp_ = abyStack_+nStackSize;

		void *fInnerCallBack = (void*)&CMicroThread::InnerCallBack;
		//	VC++6�̃C�����C���A�Z���u����static�ȃ����o�֐���call��
		//	�����Ă��Ȃ�����(VC++7�ł͉\)

		_asm {
			mov		eax,[this]

			push	offset Next	//	�߂�l��push

			push	ebx
			push	ebp
			push	esi
			push	edi

			push	dword ptr fs:[0]	//	��O�����̂��߂ɕK�v

			xchg	[eax].register_esp_ , esp

			//	bp��؂�ւ��Ă���̂ŁAthis���̂��s���ɂȂ�̂�
			//	�����ȍ~�A�����o�ɃA�N�Z�X����o���Ȃ�
			push	eax	//	InnerCallBack�̑�P�p�����[�^�Ƃ��ēn��
//			call	InnerCallBack
//	VC++6�ł̓I�[�o�[���[�h����Ă��Ȃ��O���[�o���֐������C�����C���A�Z���u������Ăяo���Ȃ�(;�L�D`)
			call	[fInnerCallBack]

		//	�����ɓ��B�����Ƃ������Ƃ́A���̊֐��̎��s���I�������Ƃ������Ƃ��Ӗ�����̂�
		//	�X�^�b�N�����̏�Ԃɖ߂��Ă��K�v������

			pop		eax

			xchg	[eax].register_esp_ , esp
			mov		[eax].bEnd_,1	//	bEnd_ = true;

			pop		dword ptr fs:[0]	//	��O�����̂��߂ɕK�v

			pop		edi
			pop		esi
			pop		ebp
			pop		ebx

//			pop		eax	//	�ŏ��ɐς񂾃��x����
//			jmp		eax
			ret
Next:
		}

	}

#else	// !defined(yaneSDK_CantUseInlineAssembler)

	//	BCC5.5�p�Ƀn���h�A�Z���u�� by yaneurao('03/07/06)
	void	CMicroThread::start(const smart_ptr<function_callback>& fn,int nStackSize)
	{
#ifdef USE_EXCEPTION
		if (nStackSize<=32){
			throw CRuntimeException("CMicroThread::start�ŃX�^�b�N���p�ӂ���Ȃ�����");
		}
#endif
		bEnd_ = false;
		ReleaseStack();
		if (pAllocator_!=NULL) {	//	�A���P�[�^�����??
			abyStack_ = pAllocator_->Alloc(nStackSize);
		} else if (pDefaultAllocator_!=NULL) {
			abyStack_ = pDefaultAllocator_->Alloc(nStackSize);
		} else {
			abyStack_ = new BYTE[nStackSize];
		}
		anStackSize_ = nStackSize;
		fn_ = fn;
		register_esp_ = abyStack_+nStackSize;

			struct CMyTmp {
				BYTE **my_esp;
				CMicroThread*pThis;
				void *fInnerCallBack;
				bool *bEnd;
			};
			CMyTmp mytmp;
			mytmp.my_esp = &register_esp_;
			mytmp.pThis = this;
			mytmp.fInnerCallBack = (void*)&CMicroThread::InnerCallBack;
			mytmp.bEnd = &bEnd_;

/*
		_asm {
//			mov		eax,[this]
//			push	offset Next	//	�߂�l��push

			push	ebx
			push	ebp
			push	esi
			push	edi

			push	dword ptr fs:[0]	//	��O�����̂��߂ɕK�v

//			xchg	[eax].register_esp_ , esp
			mov		ecx,[eax]
			xchg	esp,[ecx]


			//	bp��؂�ւ��Ă���̂ŁAthis���̂��s���ɂȂ�̂�
			//	�����ȍ~�A�����o�ɃA�N�Z�X����o���Ȃ�
//			push	eax	//	InnerCallBack�̑�P�p�����[�^�Ƃ��ēn��
			push	[eax+12]
			push	[eax]

			mov		ecx,[eax+8]
			mov		eax,[eax+4]

			//			call	InnerCallBack
//	VC++6�ł̓I�[�o�[���[�h����Ă��Ȃ��O���[�o���֐������C�����C���A�Z���u������Ăяo���Ȃ�(;�L�D`)
//			call	[fInnerCallBack]
//			call	[eax+8]
			push	eax
			call	ecx

		//	�����ɓ��B�����Ƃ������Ƃ́A���̊֐��̎��s���I�������Ƃ������Ƃ��Ӗ�����̂�
		//	�X�^�b�N�����̏�Ԃɖ߂��Ă��K�v������
			pop		eax
			mov		[eax],1	//	bEnd_ = true;
			pop		eax

//			xchg	[eax].register_esp_ , esp
			xchg	esp,[eax]

			pop		dword ptr fs:[0]	//	��O�����̂��߂ɕK�v

			pop		edi
			pop		esi
			pop		ebp
			pop		ebx

//			pop		eax	//	�ŏ��ɐς񂾃��x����
//			jmp		eax
			ret
Next:
		}
/**/

		static const BYTE code_area[]={
			0x8B,0x44,0x24,0x04,//      mov         eax,dword ptr [esp+4] 
			0x53,//               push        ebx  
			0x55,//               push        ebp  
			0x56,//               push        esi  
			0x57,//               push        edi  
			0x64,0xFF,0x35,0,0,0,0,// push        dword ptr fs:[0] 
			0x8B,0x08,//            mov         ecx,dword ptr [eax] 
			0x87,0x21,//            xchg        esp,dword ptr [ecx] 

			0xFF,0x30,//            push        dword ptr [eax] 
			0xFF,0x70,0x0C,//         push        dword ptr [eax+0Ch] 

			0x8B,0x48,0x08,//       mov         ecx,dword ptr [eax+8] 
			0x8B,0x40,0x04,//       mov         eax,dword ptr [eax+4] 
			0x50,//                 push        eax  
			0xFF,0xD1,     //       call        ecx  

			0x58,//		              pop         eax  //	���Opush�̕�
			0x58,//		              pop         eax  
			0xC6,0x00,0x01,//    mov         byte ptr [eax],1 

			0x58,//		              pop         eax  
			0x87,0x20,//            xchg        esp,dword ptr [eax] 
 
			0x64,0x8F,0x05,0,0,0,0,// pop         dword ptr fs:[0] 
			0x5F,//               pop         edi  
			0x5E,//               pop         esi  
			0x5D,//               pop         ebp  
			0x5B,//               pop         ebx  
			0xC3 //               ret           	
		};
		//reinterpret_cast<void(*)(CMyTmp*)>(&code_area)(&mytmp);
		void(*f)(CMyTmp*) = (void(*)(CMyTmp*))&code_area;
		f(&mytmp);
	}
#endif	// !defined(yaneSDK_CantUseInlineAssembler)

	void	CMicroThread::suspend()	///	���s���̏����𒆒f����
	{
		bSuspended_ = true;

		SwitchThread();
	}


	void	CMicroThread::resume()		///	���f�����������ĊJ����
	{
		if (!isSuspended()) return ;	//	���f�i��`��j��Ų!!

		bSuspended_ = false;

		SwitchThread();
	}

	void	CMicroThread::sleep(int n){
		while (n-->0){
			suspend();
		}
	}

	///	���݃}�C�N���X���b�h���g�p���Ă���X�^�b�N�T�C�Y��Ԃ�
	///	(�}�C�N���X���b�h������ł��A�O����ł��Ăяo�����Ƃ��o����)
	int		CMicroThread::GetUseStackSize() const{
		BYTE* pStack;
		pStack = reinterpret_cast<BYTE*>(&pStack);

		//	�}�C�N���X���b�h������Ăяo�����̂��H
		if (abyStack_ <= pStack && 
			pStack <= abyStack_+ anStackSize_){
			//	yes�˂��̌��݂̃X�^�b�N�ʒu���X�^�b�N�{�g������̑��Έʒu�����߂�
		} else {
			//	no �˃}�C�N���X���b�h�ɕۑ�����Ă���X�^�b�N�ʒu�𒲂ׂ�
			pStack = register_esp_;
		}
		return (abyStack_+ anStackSize_) - pStack;
	}


	void	CMicroThread::InnerCallBack(CMicroThread*p)
	{
		//	fn_�ɃR�[���o�b�N����
		if (p->pAllocator_!=NULL || p->pDefaultAllocator_!=NULL){
		//	�X�^�b�N��̗̈�Ȃ��O���L���Ȃ̂ŁA
		//	��O���������Ă���
			try {
				p->fn_->run();	//	fn_�ɃR�[���o�b�N����
			} catch(CMicroThreadQuitException&) {
			}
			//	������������K�v�͂Ȃ�
		} else {
			p->fn_->run();	//	fn_�ɃR�[���o�b�N����
		}
	}	


//		BCB�ł́Ainline�֐�����_asm���g���Ȃ��̂�(;�L�D`)
#if defined(yaneSDK_BCC_Mode)||defined(yaneSDK_CodeWarrior_Mode)
///	����ɁABCC5.5����TASM�����ĂȂ��̂�(;�L�D`)
	void	CMicroThread::SwitchThread(){
	/*
		_asm {
			//	���݂̉��z�X�^�b�N�ɐς�
//			push	offset Next
			//	resume�̂��Ƃ����ɖ߂��Ă���K�v������̂�

//			mov		ecx,30		//	offset Next - Now
//			add		ecx,
//			push	ecx
//Now:
			//	�����̊֐����Ăяo���Ƃ������Ƃ̓X�^�b�N��
			//	�߂�悪�ς܂�Ă���ƍl���ėǂ�

			push	ebx
			push	ebp
			push	esi
			push	edi

			push	dword ptr fs:[0]	//	��O�����̂��߂ɕK�v

//			mov		eax,[this]
//			xchg	esp,[eax].register_esp_	//	CodeWarrior���Ƃ������ʂ�Ȃ��̂�(;�L�D`)
			xchg	esp,[eax]

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
	*/
		//	hand assembled by yaneurao('03/07/06)
		static const BYTE code_area[] ={
			0x8B,0x44,0x24,0x04,//      mov         eax,dword ptr [esp+4] 
			0x53,	//               push        ebx  
			0x55,	//               push        ebp  
			0x56,	//	             push        esi  
			0x57,	//               push        edi  
			0x64,0xFF,0x35,0,0,0,0, // push        dword ptr fs:[0] 
			0x87,0x20,//            xchg        esp,dword ptr [eax] 
			0x64,0x8F,0x05,0,0,0,0, // pop         dword ptr fs:[0] 
			0x5F,	//               pop         edi  
			0x5E,	//               pop         esi  
			0x5D,	//               pop         ebp  
			0x5B,	//               pop         ebx  
			0xC3,   //	             ret           
		};
		//reinterpret_cast<void(*)(BYTE**)>(&code_area)(&register_esp_);
		void(*f)(BYTE**) = (void(*)(BYTE**))&code_area;
		f(&register_esp_);
	}
#endif

#endif


} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd
