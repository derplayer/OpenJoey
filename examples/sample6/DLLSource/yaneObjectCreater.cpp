#include "stdafx.h"
#include "yaneObjectCreater.h"

//	static member..
IObjectCreater* IObjectCreater::m_p;

//	���̊֐����ADLL PlugIn�Ƃ��āA�R�[���o�b�N���Ă��炤
void	YaneDllInitializer(void*p){
	IObjectCreater* pObjectCreater = static_cast<IObjectCreater*>(p);
	IObjectCreater::SetObj(pObjectCreater);	//	������o�^

	//	�v���g�^�C�v�錾�O�O�G
	void	YaneRegistPlugIn(IObjectCreater*);
	YaneRegistPlugIn(pObjectCreater);
}

//	new��delete�́AMain���̂��̂��Ă�
void* operator new (size_t t){
	IObjectCreater* pp = IObjectCreater::GetObj();
	if (pp!=NULL){
		return pp->New(t);
	} else {
		return (void*)malloc(t);
	}
}

void operator delete(void*p){
	IObjectCreater* pp = IObjectCreater::GetObj();
	if (pp!=NULL){
		pp->Delete(p);
	} else {
		::free(p);
	}
}

