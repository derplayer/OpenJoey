
#ifndef __yaneMicroThread_h__
#define __yaneMicroThread_h__


#include "../Auxiliary/yaneAllocator.h"
#include "../Thread/yaneThread.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

//#define __BORLANDC__
	//	↑VCでBCB5で通るかデバッグするときのシンボル定義

#ifdef USE_EXCEPTION
//	例外処理が有効でなければ、このクラス自体無効にする

class CMicroThread {
/**
	このクラスを通じて呼び出した関数は、関数の途中でresumeし、
	次に呼び出されたときに、以前の状態を再現することが出来る

	プログラム例

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

	このようにすれば、OnMoveを呼び出すごとに、
	x+=2を100回OnMoveを呼び出す間、繰り返し
	101回目のOnMoveの呼び出しからは、y+=2を実行する。
	それを100回繰り返してマイクロスレッドは終了します。
	(以降、OnMoveを呼び出しても何も起こりません)

	注意：例外を処理するためには、マイクロスレッドのスタックを
		OSの管理するスタック領域におかなければならない。
		(Win32構造化例外の制限)
		そこで、スタック領域上にマイクロスレッドのスタックを確保して
		やる必要がある。

		このためには⇒　class CMemoryAllocatorPool
		を　用いると良い。
		例)
			const int stack_size = 1024*128; // 128k
			BYTE local_stack[stack_size]; // 128k
			CMemoryAllocatorPool pool;
			pool.SetMemory(&local_stack[0],stack_size);

			スタック領域上に確保された配列を、
			CMemoryAllocatorPoolに食わせて、それを
			CMicroThreadのコンストラクタで渡す。

		そうすれば、CMicroThreadのデストラクタで、suspend中の
		マイクロスレッド内にCMicroThreadQuitException例外を発生させ、
		開始地点までスタックの巻き戻し(デストラクタの呼び出し)を
		行なうことを保証できる。

		つまり、マイクロスレッドをsuspend中に終了させる場合でも、
		例外処理によって正常にスタックの巻き戻しが起きることが保証される。

		よって、このクラスを利用するには例外処理が有効でなければならない！

*/
public:

	void	start(const smart_ptr<function_callback>& fn,int nStackSize=0x8000);
	/**
		マイクロスレッドの実行を開始する。
		スタックサイズはディフォルトで0x8000バイト。
		これをしていない状態でsuspendとかresumeとか呼び出してはいけない。

		小さな関数でもだいたい0x2000以上は必要。
		debugビルドだと冗長なコードになるので、どうしても..
		（debugビルドとreleaseビルドでサイズを切り替えても良い）
	*/

	void	suspend();		///	実行中の処理を中断する
	void	sleep(int n);	/// 実行中の処理をn回中断する。suspendをn回呼び出すと考えれば良い
	void	resume();		///	中断した処理を再開する

	///	サスペンド中か？
	bool	isSuspended() const { return bSuspended_;}

	///	マイクロスレッドの実行は終了したか？
	bool	isEnd() const { return bEnd_; }

	/**
		現在マイクロスレッドが使用しているスタックサイズを返す
		(マイクロスレッド内からでも、外からでも呼び出すことが出来る)
		マイクロスレッド内から呼び出して、どれくらいのメモリを食っているのか
		チェックしておくことをお勧めします。(Debugビルド時とReleaseビルド時とで
		えらく違うので注意が必要)
	*/
	int		GetUseStackSize() const;

	void	SetMemoryAllocator(IMemoryAllocator*pAllocator) {
		pAllocator_=pAllocator;
	}
	///	コンストラクタで設定しそびれたアロケータをあとから設定するための関数

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
	BYTE*		abyStack_;	//	スタック
	int			anStackSize_;
	void		ReleaseStack();

	smart_ptr<function_callback> fn_; // コールバック先のfunction

	bool			bSuspended_;	//	マイクロスレッドのサスペンド
	bool			bEnd_;			//	マイクロスレッドの終了フラグ
	BYTE*			register_esp_;			//	記録しているesp
	IMemoryAllocator* pAllocator_;

	static void	QuitFunction();		//	アセンブラから呼び出す例外を発生する関数

//		BCBでは、inline関数内で_asmが使えないのだ(;´Д`)
#if defined(yaneSDK_BCC_Mode)||defined(yaneSDK_CodeWarrior_Mode)
	void	SwitchThread();
#else
	#ifdef yaneSDK_MSVC_Mode
	#pragma warning(disable:4731)	//	ebp変更したぐらいでｶﾞﾀｶﾞﾀ言うなや(ﾟдﾟ)ｺﾞﾙｧ
	#pragma warning(disable:4733)	//	fs:[0]変更したぐらいでｶﾞﾀｶﾞﾀ言うなや(ﾟдﾟ)ｺﾞﾙｧ
	#endif
	void	SwitchThread(){
		_asm {
			//	現在の仮想スタックに積む
			push	offset Next
			//	resumeのあとここに戻ってくる必要があるので

			push	ebx
			push	ebp
			push	esi
			push	edi

			push	dword ptr fs:[0]	//	例外処理のために必要

			mov		eax,[this]
			xchg	esp,[eax].register_esp_

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
	}
#endif
	class CMicroThreadQuitException : public CRuntimeException {};

	static void	InnerCallBack(CMicroThread*p);
	//	fn_にコールバックする

	static IMemoryAllocator* pDefaultAllocator_;
};
#endif


} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd

#endif
