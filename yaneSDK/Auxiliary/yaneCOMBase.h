// yaneCOM.h :
//	initialize/terminate COM
//
//	COMを使用する最初と最後で初期化／終了処理が必要なので、
// こんなもんが必要になってくる...
//

#ifndef __yaneCOMBase_h__
#define __yaneCOMBase_h__


//	#include "../YTL/yaneThreadLocal.h"
//	↑これはstdafx.hで読み込んでいる
#include "../Thread/yaneCriticalSection.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class CCOMBase {
/**
	COMは、スレッドベースで初期化しなければならない。
	よって、COMを使う前には、inc_ref関数を呼び出し、
	COMを使い終わったときには、dec_refを呼び出せば良いクラス
*/
public:
	static LRESULT inc_ref();			// COMの参照カウントの加算
	static LRESULT dec_ref();			// COMの参照カウントの減算

protected:
	static ThreadLocal<int> m_nCount;	//	参照カウントを保持している
	static CCriticalSection m_cr;
	static CCriticalSection* GetCriticalSection() { return& m_cr; }
};

template <class T>
class CCOMObject {
/**
	CoCreateInstance～ReleaseInstanceのwrapper
	T はCOMオブジェクト
*/
public:
	LRESULT CreateInstance(REFCLSID clsid,REFIID guid){
		CCOMBase::inc_ref();
		Release();	//	前のを消滅させる
		if (FAILED(::CoCreateInstance(clsid,NULL,CLSCTX_INPROC_SERVER,
			guid, (VOID**)&pObj))){
			pObj = NULL;
			CCOMBase::dec_ref();
		}
		//	CoCreateInstanceの関数の失敗と、pObjがNULLならば失敗とみなす
		return pObj==NULL?1:0;
	}
	///	インスタンスを生成する

	LRESULT Release(){
		LRESULT lr = 0;
		if (pObj!=NULL){
			pObj->Release();
			pObj = NULL;
			lr = CCOMBase::dec_ref();
		}
		return lr;
	}
	///	インスタンスを解放する

	T	get() const { return pObj; }
	///	生成したオブジェクトを取得する

	operator T* () const { return get(); }
	///	暗黙変換子
	T& operator*() const  {return *get(); }
	T* operator->() const {return get(); }
	///	-> 演算子も定義しておく

	CCOMObject() : pObj(0) {}
	virtual ~CCOMObject() { Release(); }

protected:
	T		pObj;	//	COMオブジェクト
};

class CLoadLibraryWrapper {
/**
	LoadLibrary～ReleaseLibraryのwrapper
*/
public:
	///	DLLのLoad
	LRESULT Load(const string& libname) {
		m_hDLL = LoadLibrary(libname.c_str());
		return m_hDLL==NULL?1:0;
	}

	///	DLLのRelease
	void Release(){
		if (m_hDLL!=NULL){
			::FreeLibrary(m_hDLL);
			m_hDLL = NULL;
		}
	}

	///	DLL内の関数アドレスの取得
	void* GetProc(const string& funcname){
		return ::GetProcAddress(m_hDLL,funcname.c_str());
	}

	///	読み込んだDLLのインスタンスを返す
	HINSTANCE Get() const { return m_hDLL; }

	CLoadLibraryWrapper() { m_hDLL = NULL; }
	virtual ~CLoadLibraryWrapper() { Release(); }
protected:
	HINSTANCE m_hDLL;
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
