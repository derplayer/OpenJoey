�@boost::regex++���AyaneSDK3rd���g����DLL�����ɌĂяo����悤�ɂ������̂ł��B
�@�r���h�i�R���p�C���j�ɂ�boost���C���X�g�[�����Ă���K�v������܂��B

�@�܂��Aboost::regex++�́Astatic�ȃN���X��new���P�񂾂��g���̂ŁA
�����CObjectCreater��DLL�Ăяo���̏������N���A�ł��܂���B
�@�����ŁA����DLL���R���p�C������ۂ́A[yaneObjectCreater.cpp]����
new�����delete���ȉ��̂悤�ɏ���������Ȃǂ̍H�v���K�v�ł��B
�inew��static�ȃN���X�ɂ���ĂP��̂ݎg����̂ŁA�P�����o���Ă������OK�j


static void * dmy_local_new_pointer=NULL;
//	new��delete�́AMain���̂��̂��Ă�
void* operator new (size_t t){
	IObjectCreater* pp = IObjectCreater::GetObj();
	if (pp!=NULL){
		return pp->New(t);
	} else {
		//WARNING(true,"operator new��IObjectCreater������������O�ɌĂяo����Ă���");
		return (dmy_local_new_pointer=(void*)malloc(t));	//	�Ƃ��Ă�Ͻް(߄D�)
	}
}

void operator delete(void*p){
	IObjectCreater* pp = IObjectCreater::GetObj();
	if (pp!=NULL){
		if(p==dmy_local_new_pointer){
			::free(p);
		}
		else{
			pp->Delete(p);
		}
	} else {
		//WARNING(true,"operator delete��IObjectCreater���I����ɌĂяo����Ă���");
		::free(p);					//	�Ƃ��Ă�Ͻް(߄D�)
	}
}


�@����[yaneObjectCreater.cpp]�̏��������́ADLL���ł̂ݗL���ɂȂ�̂ŁA
�Ăяo�����ł͂����̕ύX���s���K�v�͂���܂���B
�@�W����[yaneObjectCreater.cpp]�Ŗ��Ȃ��Ăяo�����Ƃ��\�ł��B
