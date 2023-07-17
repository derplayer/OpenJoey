/**
	《　Plug In DLL 作成方法　》

	このファイルは、yaneuraoGameSDK3rd用Plug In DLLを作るための
	ヘッダファイル

	DLL側のプロジェクトで、このファイルをincludeして使うこと！

	１．このヘッダをincludeする
	２．main側に公開したいオブジェクト（のfactory）を
		以下のようなYaneRegistPlugIn関数を作って登録する

		void	YaneRegistPlugIn(IObjectCreater*p){
			p->RegistClass("CHoge",new factory<CHoge>);
			p->RegistClass("CHoee",new factory<CHoee>);
						//	↑このクラス名は、あくまで例
		}

	３．オブジェクトを名前で生成するにはIObjectCreaterを使う
		（使いかたは、yaneSDK3rd/AppFrame/yaneObjectCreaterと同じ）

		例：
			//	使う前にLoad
			IObjectCreater::GetObj()->LoadPlugIn("enraSound.dll");
			IVorbis* p = (IVorbis*)CObjectCreater::GetObj()->Create("CVorbis");
			if (p!=NULL){
				p->Play();
				delete p;
			}
			//	使い終わったら、解放
			IObjectCreater::GetObj()->ReleasePlugIn("enraSound.dll");

*/

#ifndef __ObjectCreater_h__
#define __ObjectCreater_h__

/////////////////////////////////////////////////////////////////////////////
#ifndef __YTLfactory__
#define __YTLfactory__

class factory_base {
/**
	factory の基底クラス
*/
public:
	virtual void* Create()=0;
	virtual ~factory_base(){}
};

template <class T>
class factory : public factory_base {
/**
	factory
*/
public:
	virtual T* CreateInstance() { return new T; }
	virtual void* Create() { return new T; }
};

template <class T,class S>
class factory2 : public factory<T> {
///	Tは基底クラス。Sはその派生クラス。
	virtual T* CreateInstance() { return new S; }
	virtual void* Create() { return new S; }
};

#endif

/////////////////////////////////////////////////////////////////////////////

#include "string.h"	//	YTL/string
using YTL::string;

class IObjectCreater {
public:
	virtual void* Create(const string& strClassName)=0;
	virtual LRESULT LoadPlugIn(const string& strFileName)=0;
	virtual LRESULT ReleasePlugIn(const string& strFileName)=0;

	//	DLL側がオブジェクトを登録するのに使う
	virtual void RegistClass(const string& strClassName,factory_base*)=0;

	//	Main側のnewとdelete
	virtual void* New (size_t t)=0;
	virtual void  Delete(void* p)=0;

	static IObjectCreater* GetObj() { return m_p; }
	static void SetObj(IObjectCreater*p) { m_p = p; }

	virtual ~IObjectCreater()=0;
protected:
	static IObjectCreater* m_p;
};

//	この関数を、DLL PlugInとして、コールバックしてもらえる
extern "C"	__declspec( dllexport ) void _cdecl YaneDllInitializer(void*);

#endif
