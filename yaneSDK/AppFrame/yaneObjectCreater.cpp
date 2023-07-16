#include "stdafx.h"
#include "yaneObjectCreater.h"
#include "../Auxiliary/yaneFile.h"

//////////////////////////////////////////////////////////////////////////////

#if !defined(NOT_USE_DEFAULT_DLLMAIN) && defined(COMPILE_YANE_PLUGIN_DLL)
namespace {
CAppInitializer* g_pAppInit;
}

BOOL APIENTRY DllMain( HANDLE hModule,  // HINSTANCEでも可
					   DWORD  ul_reason_for_call, 
					   LPVOID lpReserved
					 )
{
	//	２．DllMainでは、CAppInitializerで初期化するヨロシ！

	switch (ul_reason_for_call) {
		case DLL_THREAD_ATTACH :
			   {
				g_pAppInit = new CAppInitializer(hModule,ul_reason_for_call,lpReserved);
				//	初期化処理が必要ならここに書くなりヨ
				//	注意事項
				//	ａ．この時点ではまだnew/deleteは使えない(以下の5.の関数で行なう)
				//	ｂ．Errデバイスは未初期化(必要ならば、以下の5.の関数で行なう)
			   }
			break;
		case DLL_THREAD_DETACH :
			   {
				if (g_pAppInit!=NULL) { delete g_pAppInit; g_pAppInit=NULL; }
			    //	終了処理はここに書いてチョ
				//	たとえば、5.の関数内でnewしたstaticなポインタ
				//	をdeleteする処理とか。
			   }
		   break;
	};
	return TRUE;
}
#endif

#ifdef COMPILE_YANE_PLUGIN_DLL

#include "../AppFrame/yaneAppInitializer.h"
#include "../Auxiliary/yaneStream.h"
//	↑↑は、↓↓の初期化部分でyaneSDK3rdライブラリ全体に対する初期化が必要なので
//	includeしてあるやつです。CObjectCreaterだけをぶっこぬくときは、消してください。

//	プロトタイプ宣言＾＾；
void	YaneRegistPlugIn(IObjectCreater*);
//	この関数を、DLL PlugInとして、コールバックしてもらう
extern "C"	__declspec( dllexport ) void _cdecl YaneDllInitializer(void*p){
	IObjectCreater* pObjectCreater = static_cast<IObjectCreater*>(p);
	IObjectCreater::SetObj(pObjectCreater);	//	こいつを登録

	//	yaneSDK3rdのなかで初期化(staticな変数)が必要なものはここで初期化する
	//	DLL側では、↑のSetObjをやるまでnew/deleteが使えないので仕方ないのです
	//	CObjectCreaterのソースだけをぶっこぬくときは削除してくらはい
	///////////////////////////////////////////////////////////////////
	CFile::SetCurrentDir();	//	CFileのpath初期化
#ifdef USE_ErrorLog		//	CErrorLogクラスを有効にする。
	//	エラー出力デバイスをひとつ用意
	Err.SelectDevice(smart_ptr<ITextOutputStream>(new CTextOutputStreamNullDevice));
#endif
	///////////////////////////////////////////////////////////////////

	//	user定義↓
	YaneRegistPlugIn(pObjectCreater);
}

//	newとdeleteは、Main側のものを呼ぶ
void* operator new (size_t t){
	IObjectCreater* pp = IObjectCreater::GetObj();
	if (pp!=NULL){
		return pp->New(t);
	} else {
		WARNING(true,"operator newがIObjectCreaterを初期化する前に呼び出されている");
		return (void*)malloc(t);	//	とってもﾏｽﾞｰ(ﾟДﾟ)
	}
}

void operator delete(void*p){
	IObjectCreater* pp = IObjectCreater::GetObj();
	if (pp!=NULL){
		pp->Delete(p);
	} else {
		WARNING(true,"operator deleteがIObjectCreaterを終了後に呼び出されている");
		::free(p);					//	とってもﾏｽﾞｰ(ﾟДﾟ)
	}
}
#endif

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

//	DLL側?
#ifdef COMPILE_YANE_PLUGIN_DLL
//	static member..
IObjectCreater* IObjectCreater::m_p;
#endif

CDllPlugInLoader::CDllPlugInLoader(){
	m_hDll = NULL;
}
CDllPlugInLoader::~CDllPlugInLoader(){
	Release();
}

LRESULT CDllPlugInLoader::Load(const string& szDllName){
	m_strFileName = szDllName;
	// DLLを読み込む
	m_hDll = ::LoadLibrary(m_strFileName.c_str());
	if(m_hDll==NULL){
		Err.Out("CDllPlugInLoader::Load \"%s\"のロードに失敗", m_strFileName.c_str());
		return 1;
	}
	return 0;
}

LRESULT CDllPlugInLoader::Release(){
	LRESULT lr=0;
	if(m_hDll!=NULL){
		lr = !::FreeLibrary(m_hDll);
		m_hDll = NULL;
	}
	return lr;
}

//////////////////////////////////////////////////////////////////////////////

//	static member..
singleton<CObjectCreater> CObjectCreater::m_vObj;

CObjectCreater::CObjectCreater(): m_pPlugInInfo(0){}

void* CObjectCreater::Create(LPCSTR strClassName){
	void* pObj=NULL;
	CCriticalLock cl(GetCriticalSection());
	{
		smart_vector_ptr<CObjectInfo>& info = *GetObjectInfo();
		smart_vector_ptr<CObjectInfo>::iterator it = info.begin();
		while (it!=info.end()){
			if ((*it)->strClassName==strClassName){
			//	こいつやー！！
			//	こいつのfactory呼び出してオブジェクト作って返したるで！！
				//if (it->factory_base!=NULL){
				//	↑これNULLであることはありえない
					pObj = (*it)->factory->Create();
					break;
				//}
			}
			++it;
		}
	}
	return pObj;
}

LRESULT CObjectCreater::LoadPlugIn(LPCSTR strFileName){
	LRESULT lr=0;
	const string str(CFile::MakeFullName(strFileName));
	//	これ、MakeFullNameより、FindFileのほうが良いのだが...

	CCriticalLock cl(GetCriticalSection());
		//	同名のplug inを探す
		smart_vector_ptr<CPlugInInfo>::iterator	it = FindPlugIn(str);
		if (it == GetPlugInInfo()->end()) {
			//	読み込んで、そのクラス（のfactory）を登録する
			m_pPlugInInfo = new CPlugInInfo;
			//	この変数は、DLL側から参照されうる
			if (m_pPlugInInfo->vPlugIn.Load(str)==0){	
			//	plug-in読み込みに成功
				HINSTANCE hDll = m_pPlugInInfo->vPlugIn.GetInstance();
				typedef void (_cdecl *Func)(void*);
				Func YaneDllInitializer = (Func)::GetProcAddress(hDll, "YaneDllInitializer");
				if(YaneDllInitializer==NULL){
					//	初期化失敗ゴン
					DELETE_SAFE(m_pPlugInInfo);
				} else {
					//	所有権の移動
					YaneDllInitializer(static_cast<void*>(this));
					GetPlugInInfo()->insert(m_pPlugInInfo);
					m_pPlugInInfo = NULL;	//	deleteしてはいかん
				}
			} else {
				DELETE_SAFE(m_pPlugInInfo);
			}
		} else {
			//	既に読み込んでいるので、その参照カウントのみを加算する
			(*it)->nRef++;
		}
	return lr;
}

LRESULT CObjectCreater::ReleasePlugIn(LPCSTR strFileName){
	LRESULT lr=0;

	const string str(CFile::MakeFullName(strFileName));
	//	これ、MakeFullNameより、FindFileのほうが良いのだが...

	CCriticalLock cl(GetCriticalSection());
		//	同名のplug inを探す
		smart_vector_ptr<CPlugInInfo>::iterator	it = FindPlugIn(str);
		if (it == GetPlugInInfo()->end()) {
			//	無いとは、どういうこっちゃ？
			Err.Out("CObjectCreater::ReleasePlugIn 登録されていないPlugInの削除");
		} else {
			//	参照カウントを減算する
			if (--(*it)->nRef == 0){
				//	0になったので、このオブジェクトをdeleteする
				CPlugInInfo* pInfo = (*it).get();
				//	このpInfoを用いているオブジェクトの
				//	factoryを登録抹消しなければならない
				smart_vector_ptr<CObjectInfo>::iterator it2	= GetObjectInfo()->begin();
				while (it2!=GetObjectInfo()->end()){
					if ((*it2)->pPlugIn == pInfo){
						it2 = GetObjectInfo()->erase(it2);
					} else {
						++it2;
					}
				}
				GetPlugInInfo()->erase(it);	//	smart_ptrなので、これで消せる
			}
		}
	return lr;
}

smart_vector_ptr<CObjectCreater::CPlugInInfo>::iterator
	CObjectCreater::FindPlugIn(const string& strPlugInName){
/*
	この関数は、
		LRESULT CObjectCreater::LoadPlugIn と ReleasePlugIn
	から呼び出されるので、CriticalSection中であると仮定して良い
*/
	smart_vector_ptr<CPlugInInfo>::iterator	it = GetPlugInInfo()->begin();
	smart_vector_ptr<CPlugInInfo>::iterator it_end	= GetPlugInInfo()->end();
	while (it!=it_end){
		if (strPlugInName == (*it)->vPlugIn.GetFileName())
			break;
		++it;
	}
	return it;
}

void CObjectCreater::RegistClass(LPCSTR strClassName,factory_base*f){
	CObjectInfo* pInfo = new CObjectInfo;
	pInfo->pPlugIn		= m_pPlugInInfo;
	pInfo->strClassName = strClassName;
	pInfo->factory		= f;
	GetObjectInfo()->insert(pInfo);
}


void* CObjectCreater::New(size_t t){
	return new BYTE[t];
}

void  CObjectCreater::Delete(void* p){
	delete [] p;
}

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd
