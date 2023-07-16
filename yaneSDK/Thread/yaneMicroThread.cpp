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

//	念のためにコードを分離しておこう...
#if !defined(yaneSDK_CantUseInlineAssembler)
	void	CMicroThread::ReleaseStack(){
	//	いま、マイクロスレッド実行中?
		if (bSuspended_){
			//	スタックを切り替えたあと、例外をthrowするか?
			void *fQuitFunction = (void*)&CMicroThread::QuitFunction;
			//	VC++6のインラインアセンブラがstaticなメンバ関数のcallを
			//	許可していないため(VC++7では可能)
		_asm {
			push	offset Next
			//	resumeのあとここに戻ってくる必要があるので

			push	ebx
			push	ebp
			push	esi
			push	edi

			push	dword ptr fs:[0]	//	例外処理のために必要

			mov		eax,[this]
			xchg	esp,[eax].register_esp_
			mov		eax,fQuitFunction

			//↓↓↓ ここ↑で例外が発生するので、実際は、この以下の部分は実行されることはない

			pop		dword ptr fs:[0]	//	例外処理のために必要

			pop		edi
			pop		esi
			pop		ebp
			pop		ebx
//	VC++6ではオーバーロードされていないグローバル関数しかインラインアセンブラから呼び出せない(;´Д`)
			jmp		eax

			//↑↑↑ と、いうことでここまでは、コピペの残骸(^^;
Next:;
			}
		}

		if (abyStack_!=NULL){
			if (pAllocator_!=NULL) {	//	アロケータで割り当てたやつか?
				pAllocator_->Free(abyStack_);
			} else if (pDefaultAllocator_!=NULL) {	//	ディフォルトアロケータで割り当てたやつか?
				pDefaultAllocator_->Free(abyStack_);
			} else {
				delete [] abyStack_;
			}
			abyStack_ = NULL;
		}
	}

#else	// !defined(yaneSDK_CantUseInlineAssembler)

	//	BCC5.5用にハンドアセンブルバージョンを用意
	void	CMicroThread::ReleaseStack(){
	//	いま、マイクロスレッド実行中?
		if (bSuspended_){
			//	スタックを切り替えたあと、例外をthrowするか?

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
			//	resumeのあとここに戻ってくる必要があるので

			push	ebx
			push	ebp
			push	esi
			push	edi

			push	dword ptr fs:[0]	//	例外処理のために必要

//			mov		eax,[this]
			mov		ecx,[eax]
			xchg	esp,[ecx]
//			mov		eax,fQuitFunction
			mov		eax,[eax+4]

			//↓↓↓ ここ↑で例外が発生するので、実際は、この以下の部分は実行されることはない

			pop		dword ptr fs:[0]	//	例外処理のために必要

			pop		edi
			pop		esi
			pop		ebp
			pop		ebx
//	VC++6ではオーバーロードされていないグローバル関数しかインラインアセンブラから呼び出せない(;´Д`)
			jmp		eax

			//↑↑↑ と、いうことでここまでは、コピペの残骸(^^;
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
			if (pAllocator_!=NULL) {	//	アロケータで割り当てたやつか?
				pAllocator_->Free(abyStack_);
			} else if (pDefaultAllocator_!=NULL) {	//	ディフォルトアロケータで割り当てたやつか?
				pDefaultAllocator_->Free(abyStack_);
			} else {
				delete [] abyStack_;
			}
			abyStack_ = NULL;
		}
	}

#endif	// !defined(yaneSDK_CantUseInlineAssembler)

	CMicroThread::~CMicroThread(){
		//	例外を発生させて、強制的に抜け出すべきか?
		ReleaseStack();
	}

//	念のためにコードを分離しておこう...
#if !defined(yaneSDK_CantUseInlineAssembler)
	void	CMicroThread::start(const smart_ptr<function_callback>& fn,int nStackSize)
	{
#ifdef USE_EXCEPTION
		if (nStackSize<=32){
			throw CRuntimeException("CMicroThread::startでスタックが用意されなかった");
		}
#endif
		bEnd_ = false;
		ReleaseStack();
		if (pAllocator_!=NULL) {	//	アロケータあるんか??
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
		//	VC++6のインラインアセンブラがstaticなメンバ関数のcallを
		//	許可していないため(VC++7では可能)

		_asm {
			mov		eax,[this]

			push	offset Next	//	戻り値のpush

			push	ebx
			push	ebp
			push	esi
			push	edi

			push	dword ptr fs:[0]	//	例外処理のために必要

			xchg	[eax].register_esp_ , esp

			//	bpを切り替えているので、this自体が不正になるので
			//	ここ以降、メンバにアクセスすら出来ない
			push	eax	//	InnerCallBackの第１パラメータとして渡す
//			call	InnerCallBack
//	VC++6ではオーバーロードされていないグローバル関数しかインラインアセンブラから呼び出せない(;´Д`)
			call	[fInnerCallBack]

		//	ここに到達したということは、その関数の実行が終了したということを意味するので
		//	スタックを元の状態に戻してやる必要がある

			pop		eax

			xchg	[eax].register_esp_ , esp
			mov		[eax].bEnd_,1	//	bEnd_ = true;

			pop		dword ptr fs:[0]	//	例外処理のために必要

			pop		edi
			pop		esi
			pop		ebp
			pop		ebx

//			pop		eax	//	最初に積んだラベルも
//			jmp		eax
			ret
Next:
		}

	}

#else	// !defined(yaneSDK_CantUseInlineAssembler)

	//	BCC5.5用にハンドアセンブル by yaneurao('03/07/06)
	void	CMicroThread::start(const smart_ptr<function_callback>& fn,int nStackSize)
	{
#ifdef USE_EXCEPTION
		if (nStackSize<=32){
			throw CRuntimeException("CMicroThread::startでスタックが用意されなかった");
		}
#endif
		bEnd_ = false;
		ReleaseStack();
		if (pAllocator_!=NULL) {	//	アロケータあるんか??
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
//			push	offset Next	//	戻り値のpush

			push	ebx
			push	ebp
			push	esi
			push	edi

			push	dword ptr fs:[0]	//	例外処理のために必要

//			xchg	[eax].register_esp_ , esp
			mov		ecx,[eax]
			xchg	esp,[ecx]


			//	bpを切り替えているので、this自体が不正になるので
			//	ここ以降、メンバにアクセスすら出来ない
//			push	eax	//	InnerCallBackの第１パラメータとして渡す
			push	[eax+12]
			push	[eax]

			mov		ecx,[eax+8]
			mov		eax,[eax+4]

			//			call	InnerCallBack
//	VC++6ではオーバーロードされていないグローバル関数しかインラインアセンブラから呼び出せない(;´Д`)
//			call	[fInnerCallBack]
//			call	[eax+8]
			push	eax
			call	ecx

		//	ここに到達したということは、その関数の実行が終了したということを意味するので
		//	スタックを元の状態に戻してやる必要がある
			pop		eax
			mov		[eax],1	//	bEnd_ = true;
			pop		eax

//			xchg	[eax].register_esp_ , esp
			xchg	esp,[eax]

			pop		dword ptr fs:[0]	//	例外処理のために必要

			pop		edi
			pop		esi
			pop		ebp
			pop		ebx

//			pop		eax	//	最初に積んだラベルも
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

			0x58,//		              pop         eax  //	直前pushの分
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

	void	CMicroThread::suspend()	///	実行中の処理を中断する
	{
		bSuspended_ = true;

		SwitchThread();
	}


	void	CMicroThread::resume()		///	中断した処理を再開する
	{
		if (!isSuspended()) return ;	//	中断（･Ａ･）ｼﾃﾅｲ!!

		bSuspended_ = false;

		SwitchThread();
	}

	void	CMicroThread::sleep(int n){
		while (n-->0){
			suspend();
		}
	}

	///	現在マイクロスレッドが使用しているスタックサイズを返す
	///	(マイクロスレッド内からでも、外からでも呼び出すことが出来る)
	int		CMicroThread::GetUseStackSize() const{
		BYTE* pStack;
		pStack = reinterpret_cast<BYTE*>(&pStack);

		//	マイクロスレッド内から呼び出したのか？
		if (abyStack_ <= pStack && 
			pStack <= abyStack_+ anStackSize_){
			//	yes⇒この現在のスタック位置をスタックボトムからの相対位置を求める
		} else {
			//	no ⇒マイクロスレッドに保存されているスタック位置を調べる
			pStack = register_esp_;
		}
		return (abyStack_+ anStackSize_) - pStack;
	}


	void	CMicroThread::InnerCallBack(CMicroThread*p)
	{
		//	fn_にコールバックする
		if (p->pAllocator_!=NULL || p->pDefaultAllocator_!=NULL){
		//	スタック上の領域なら例外が有効なので、
		//	例外処理をしておく
			try {
				p->fn_->run();	//	fn_にコールバックする
			} catch(CMicroThreadQuitException&) {
			}
			//	何も処理する必要はない
		} else {
			p->fn_->run();	//	fn_にコールバックする
		}
	}	


//		BCBでは、inline関数内で_asmが使えないのだ(;´Д`)
#if defined(yaneSDK_BCC_Mode)||defined(yaneSDK_CodeWarrior_Mode)
///	さらに、BCC5.5だとTASMもついてないのだ(;´Д`)
	void	CMicroThread::SwitchThread(){
	/*
		_asm {
			//	現在の仮想スタックに積む
//			push	offset Next
			//	resumeのあとここに戻ってくる必要があるので

//			mov		ecx,30		//	offset Next - Now
//			add		ecx,
//			push	ecx
//Now:
			//	↑この関数を呼び出すということはスタックに
			//	戻り先が積まれていると考えて良い

			push	ebx
			push	ebp
			push	esi
			push	edi

			push	dword ptr fs:[0]	//	例外処理のために必要

//			mov		eax,[this]
//			xchg	esp,[eax].register_esp_	//	CodeWarriorだとここが通らないのだ(;´Д`)
			xchg	esp,[eax]

			pop		dword ptr fs:[0]	//	例外処理のために必要

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
