#include "stdafx.h"
#include "capp.h"

struct CCharacter {
	void	OnMove(){
		if (!microthread_.isEnd()){
			if (!microthread_.isSuspended()) {
				smart_ptr<function_callback>
					fn(function_callback_v::Create(
						&CCharacter::MicroThread,this));
				microthread_.start(fn);
			} else {
				microthread_.resume();
			}
		}
	}
	void MicroThread(){
		int i,x=100,y=200;
		for(i=0;i<50;++i) { x+=2; suspend(); CDbg().Out("suspend1"); }
		for(i=0;i<50;++i) { y+=2; suspend(); CDbg().Out("suspend2"); }
	}
protected:
	void suspend() { microthread_.suspend(); }
	CMicroThread	microthread_;
};

//	言わずと知れたWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	//	マイクロスレッド用のメモリを確保
	const int stack_size = 1024*128; // 128k
	BYTE local_stack[stack_size]; // 128k
	CMemoryAllocatorPool pool;
	pool.SetMemory(&local_stack[0],stack_size);
	CMicroThread::SetDefaultMemoryAllocator(&pool);

	//	↓CAppInitializerのデストラクタで全スレッドの終了を待機するので
	//	CAppInitializerより下に↑を書くと、このスタックが
	//	CAppInitializerのデストラクタより先に解放されかねない。
	//	（このプログラムでは、スレッドは一つしか走っていないので
	//	問題にはならないが、サブスレッドを起動する場合には注意が必要である）

	//	あと、マイクロスレッド用のメモリとしては、必ず自分のスタックを
	//	割り当てる必要がある。これは、Windowsの構造化例外の制限によるものだ。

	CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	必ず書いてね

	CCharacter chara;
	for(int i=0;i<100;++i) {
		chara.OnMove();	CDbg().Out("loop");
	}

	return 0;
}
