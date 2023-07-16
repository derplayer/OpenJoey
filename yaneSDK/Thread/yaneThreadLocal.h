/*
	スレッド固有のオブジェクトを用意するためのテンプレート

		programmed & designed by yaneurao '02/03/10
*/

#ifndef __YTLThreadLocal_h__
#define __YTLThreadLocal_h__


#include "../Thread/yaneCriticalSection.h"
#include "../YTL/singleton.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

class ThreadLocalBase;
class ThreadLocalBaseHelper {
/**
	class ThreadLocalBase のためのSingletonオブジェクト
	（非localなオブジェクトの初期化順序は不定のためこういうのが必要になる）

	class ThreadLocalBase から呼び出されるので、
	これらの関数は直接呼び出して使うというものではない。
*/
public:
	static ThreadLocalBaseHelper* GetObj(){ return m_obj.get(); }

	///	チェインに追加／チェインから消去
	void	insert(ThreadLocalBase*p);
	void	erase(ThreadLocalBase*p);

	///	スレッドが停止するときにそのスレッドから呼び出される
	void	OnThreadEnd();

	///	WinMainから抜けるときに、CAppInitializerからコールバックされる
	void	OnExit();

	//	こいつが、CAppInitalizerに、OnExitのコールバックを依頼
	ThreadLocalBaseHelper();

protected:
	static singleton<ThreadLocalBaseHelper> m_obj;

	///	チェインの取得
	list_chain<ThreadLocalBase>* GetChain(){ return &m_threadlocal_list;}
	list_chain<ThreadLocalBase>	m_threadlocal_list;

	//	↑このチェインをいじるときのためのクリティカルセクション
	CCriticalSection	ms_cr;
	CCriticalSection*	GetCriticalSection() { return &ms_cr; }
};

class ThreadLocalBase {
/**
	class ThreadLocal テンプレートのための基底クラス
	class ThreadLocal も参照のこと。
*/
public:
	virtual void	Clear()=0;
	virtual void	ClearAll()=0;
	virtual void	SetExit(bool)=0;
	virtual ~ThreadLocalBase(){} // merely place holder

	static void	OnThreadEnd()
	{	ThreadLocalBaseHelper::GetObj()->OnThreadEnd(); }
	/**
		スレッドは終了するときに必ずこの関数を呼び出して、現存する
		すべてのThreadLocalのインスタンスのClearを呼び出さなければ
		ならない
	*/

	///	チェインに追加／チェインから消去
	static void	insert(ThreadLocalBase*p)
	{	ThreadLocalBaseHelper::GetObj()->insert(p); }
	static void	erase(ThreadLocalBase*p)
	{	ThreadLocalBaseHelper::GetObj()->erase(p);	}

	///	WinMainから抜ける前に、この関数を呼び出して、
	///	チェインを断ち切っておく必要がある
	///	ただし、この関数はCAppInitializerから自動的に
	///	コールバックされるのでユーザーは呼び出す必要は無い
	static void OnExit()
	{	ThreadLocalBaseHelper::GetObj()->OnExit(); }

};

template <class T>
class ThreadLocal : public ThreadLocalBase {
/**
	スレッド固有のオブジェクトを用意するためのテンプレートクラス

	ThreadLocal<HWND> hWnd;

	とやれば、
	hWnd = GetHWnd();
	というようにスレッド固有の変数が自動的に用意されて、
	こいつに代入が出来る。

	あるいは、
	ThreadLocal<HWND> hWnd;
	HWND	h = hWnd;
	というように、こいつからの代入も出来る。

	すべてのスレッドは消滅するときに、
	class CThreadLocalBase::OnThreadEnd() を呼び出して、現存する
	ThreadLocalのインスタンスすべてのClearを呼び出さなければならない。

	（最後のスレッドは、このクラスのデストラクタが保有するオブジェクトを
	解体してくれるので、この関数を呼び出す必要は無い）

	※　参考にすると良いクラス
	class CCOMBase
*/
public:
	///	データの設定／取得
	void	Set(const T& t);
	T*		Get();

	///	要素がまだ代入されていないかをテストする
	bool	isEmpty();

	///	暗黙変換子
	operator T() { return *Get(); }

	///	代入演算子
	const T operator = (const T& t){
		Set(t);
		return *Get();
	}

	///	そのスレッドに属するデータをクリア
	virtual void	Clear();

	///	全スレッドに属するデータをクリア
	virtual void	ClearAll();

	///	終了処理が終わっているのならば、チェインから外す等の操作は不要
	virtual void	SetExit(bool b) { m_bExit = b; }

	ThreadLocal(): m_bExit(false)
		{ ThreadLocalBase::insert(this); }
	virtual ~ThreadLocal() {
		ClearAll();
		if (!m_bExit) { // 終了処理が終わっていないときのみ
			ThreadLocalBase::erase(this);
		}
	}

	//	processId ⇒ HWNDのmap
	typedef map <DWORD,T*> mymap;

protected:
	mymap	m_map;
	mymap* GetMap() { return &m_map; }

	CCriticalSection	m_cr;
	CCriticalSection*	GetCriticalSection() { return &m_cr; }

	bool	m_bExit;
};

///////////////////////////////////////////////////////////////////////

template <class T>
T*	ThreadLocal<T>::Get() {
	/// そのスレッドに対応するデータの取得
	DWORD	dwThreadID = ::GetCurrentThreadId();

	CCriticalLock cl(GetCriticalSection());

	mymap::iterator it;
	it = GetMap()->find(dwThreadID);
	T* p;
	if (it==GetMap()->end()){
		//	無いやん？？newするか？
		p = new T; // newしてまうで！
		GetMap()->insert(pair<DWORD,T*>(dwThreadID,p));
		//	insertしとこっと。

		//	getで存在しないのは問題のような気もするなぁ..
	} else {
		p = it->second;
	}

	return p;
}

template <class T>
void	ThreadLocal<T>::Set(const T& t){
	///	そのスレッドに対応するTの設定
	DWORD	dwThreadID = ::GetCurrentThreadId();

	CCriticalLock cl(GetCriticalSection());

	mymap::iterator it;
	it = GetMap()->find(dwThreadID);

	T* p;
	if (it==GetMap()->end()){
		//	無いやん？？
		p = new T; // newしてまうで！
		*p = t;	//	コピってしまう。
		GetMap()->insert(pair<DWORD,T*>(dwThreadID,p));
	} else {
		*(it->second) = t;
	}

}

template <class T>
void	ThreadLocal<T>::Clear() {
	/// そのスレッドに対応するデータの取得
	DWORD	dwThreadID = ::GetCurrentThreadId();

	CCriticalLock cl(GetCriticalSection());

	//	テンプレート中ではusing std::mapしてあるのが無効らしい > VC++6(;´Д`)
	mymap::iterator it;
	it = GetMap()->find(dwThreadID);
	if (it==GetMap()->end()){
		//	無いやん？？
	} else {
		delete it->second;		//	この要素消して
		GetMap()->erase(it);	//	消してまうで！
	}
}

template <class T>
void	ThreadLocal<T>::ClearAll() {
	CCriticalLock cl(GetCriticalSection());
	mymap::iterator it;
	it = GetMap()->begin();
	while (it!=GetMap()->end()){
		delete it->second;
		it++;
	}
	GetMap()->clear();
}

template <class T>
bool	ThreadLocal<T>::isEmpty() {
	DWORD	dwThreadID = ::GetCurrentThreadId();
	CCriticalLock cl(GetCriticalSection());

	mymap::iterator it;
	it = GetMap()->find(dwThreadID);
	bool b = (it==GetMap()->end());	//	未作成データ

	return b;
}

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd

#endif
