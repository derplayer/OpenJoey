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

//	���킸�ƒm�ꂽWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	//	�}�C�N���X���b�h�p�̃��������m��
	const int stack_size = 1024*128; // 128k
	BYTE local_stack[stack_size]; // 128k
	CMemoryAllocatorPool pool;
	pool.SetMemory(&local_stack[0],stack_size);
	CMicroThread::SetDefaultMemoryAllocator(&pool);

	//	��CAppInitializer�̃f�X�g���N�^�őS�X���b�h�̏I����ҋ@����̂�
	//	CAppInitializer��艺�Ɂ��������ƁA���̃X�^�b�N��
	//	CAppInitializer�̃f�X�g���N�^����ɉ�����ꂩ�˂Ȃ��B
	//	�i���̃v���O�����ł́A�X���b�h�͈���������Ă��Ȃ��̂�
	//	���ɂ͂Ȃ�Ȃ����A�T�u�X���b�h���N������ꍇ�ɂ͒��ӂ��K�v�ł���j

	//	���ƁA�}�C�N���X���b�h�p�̃������Ƃ��ẮA�K�������̃X�^�b�N��
	//	���蓖�Ă�K�v������B����́AWindows�̍\������O�̐����ɂ����̂��B

	CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	�K�������Ă�

	CCharacter chara;
	for(int i=0;i<100;++i) {
		chara.OnMove();	CDbg().Out("loop");
	}

	return 0;
}
