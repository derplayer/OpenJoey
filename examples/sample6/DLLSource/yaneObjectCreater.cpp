#include "stdafx.h"
#include "yaneObjectCreater.h"

//	static member..
IObjectCreater* IObjectCreater::m_p;

//	この関数を、DLL PlugInとして、コールバックしてもらう
void	YaneDllInitializer(void*p){
	IObjectCreater* pObjectCreater = static_cast<IObjectCreater*>(p);
	IObjectCreater::SetObj(pObjectCreater);	//	こいつを登録

	//	プロトタイプ宣言＾＾；
	void	YaneRegistPlugIn(IObjectCreater*);
	YaneRegistPlugIn(pObjectCreater);
}

//	newとdeleteは、Main側のものを呼ぶ
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

