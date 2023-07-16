#include "stdafx.h"
#include "../Thread/yaneThread.h"
#include "../AppFrame/yaneAppBase.h"
#include <process.h> 

namespace yaneuraoGameSDK3rd {
namespace Thread {

//////////////////////////////////////////////////////////////////////////////
//	static members..
ThreadLocal<IThread*> IThread::pThread;

//////////////////////////////////////////////////////////////////////////////

CThread::CThread(){
	m_dwThreadHandle	 = (HANDLE)0;
	m_dwRunningThreadID	 = (DWORD)-1;
	//	-1はエラー値。(スレッドハンドルとしてはありえない値)
	//	（_beginthreadは理論上は０が返ってくることがある）

	m_bCreatedThread = false;
	m_nStackSize = 0; // default
}

CThread::~CThread(){
	StopThread();
}

//	イベントステータスの初期化
void	CThread::InitEvent(){
	GetExistEvent()->ResetEvent();	//	ノンシグナル状態にする
	//	スレッドが生きている状態のときはノンシグナル状態

	GetThreadStarting()->ResetEvent();
	//	スレッドが開始していない状態のときはノンシグナル状態

	GetExceptionEvent()->ResetEvent();	//	ノンシグナル状態にする
	//	例外が発生していないときは、ノンシグナル状態

	GetThreadExecute()->ResetEvent();	//	ノンシグナル状態にする
	//	スレッドが実行されていないときは、ノンシグナル状態

	m_bCreatedThread = false;
	//	CreateThreadで生成されたスレッドならばtrue
}

bool CThread::IsThreadValid()const{
	///	割り込み状態になっているか
	CThread* pThis = const_cast<CThread*>(this);
	return !pThis->GetExistEvent()->IsSignal();
}

bool CThread::IsThreadExecute()const /// スレッドは実行中か？
{
	CThread* pThis = const_cast<CThread*>(this);
	return pThis->GetThreadExecute()->IsSignal();
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CThread::CreateThread(const smart_ptr<function_callback>& fn) {
	CCriticalLock cs(GetCriticalSection());
	m_fn = fn;
	return CreateThread();
}

void CThread::SetThreadFunction(const smart_ptr<function_callback>& fn){
	CCriticalLock cs(GetCriticalSection());
	m_fn = fn;
}

LRESULT CThread::CreateThread() {
	CCriticalLock cs(GetCriticalSection());

	StopThread();
	InitEvent();
	m_bCreatedThread = true;

//	return !::CreateThread(NULL,0,ThreadCallBack,this,NULL,&m_dwThreadHandle);
//	m_dwThreadHandle = ::_beginthread(0,ThreadCallBack,0,(void*)this);
	HANDLE dwThreadHandle =
		(HANDLE)::_beginthreadex(0,GetStackSize(),ThreadCallBack,(void*)this,0
			,(unsigned int*)&m_dwRunningThreadID);
	if (dwThreadHandle ==0 ){
	/*
					↑ここでコンパイルエラーが出るとしたら、
		リンクするライブラリがシングルスレッドになっているからです。
		プロジェクトの設定⇒「C/C++」⇒「コード生成」で、
		「使用するランタイムライブラリ」は、
		マルチスレッド用のものを使用してください。
	*/
		return -1;	//	スレッド起動に失敗
	}
	m_dwThreadHandle = dwThreadHandle;

	/**
		スレッドが起動するのを待つ？
	*/
	return GetThreadStarting()->Wait();
		//	⇒_beginthreadexは、エラー時、-1が返ってくる
	/*
		CreateThreadではなく_beginthreadでスレッドを生成しないと、
		C言語のランタイムを呼び出している場合、小規模なメモリリークが発生する。
		(80バイト程度)
		あと、コンパイルするときは、マルチスレッド用のライブラリを
		リンクするようにしてください。

	また、

   Thread.cファイルを覗いてみると、
1. _beginthread 内で CreateThread 後、直ちに ResumeThread
   をコールする。
2. すると、_threadstart がコールされる。
3. _threadstart内でコールバック関数がコールされる。
4. 上記コールバック関数終了後、直ちに 
		_endthread がコール
　　される。

　結果、スレッドハンドルが無効になる。
　したがって、WaitForSingleObjectよりも先にスレッドの
コールバック関数が終了した場合、無効なスレッドハンドル
に対し待機するようになる。

	よって、_beginthreadexで待つべき

	*/
}

LRESULT CThread::StopThread() {
	CCriticalLock cs(GetCriticalSection());

	InvalidateThread();			//	止まって欲しいねん！

	if (m_dwThreadHandle==(HANDLE)0) {
		return 0;
		//	スレッドもとから動いてへんで？
	}

	//	これスレッド、自分自身だったら、こんなところで自分のスレッドの終了を待ったところで
	//	デッドロックになるだけ
	if (GetThreadId() == ::GetCurrentThreadId()) return 2;

	//	強制的に、このスレッドを削除するコードは危険なので投入しない

	DWORD dwResult = 0;	
	HANDLE dwThreadHandle = m_dwThreadHandle;
	m_dwThreadHandle = (HANDLE)0;
	m_dwRunningThreadID = (DWORD)-1;
	//	↑ここでハンドルをコピーして、クリアしておかないと、Leaveした途端に
	//	あらたなスレッドがCreateされて、ハンドルが上書きされてしまう
	//	可能性がある

	if (m_bCreatedThread){
		cs.Leave();
			//　このlockをしたまま待っていたら、ワーカースレッドがLockを獲得できずに
			//	dead lockに成り得る
		dwResult = ::WaitForSingleObject(dwThreadHandle,INFINITE);
		cs.Enter();
	}
	//	↑これを待つのは一見、堅牢なコードだが、デッドロックになりうる
	//	(嘘を返すことがあるのか？？＞_beginthread 。_beginthreadexならok)
	//	※　_beginthreadのほうは、自動的にスレッドハンドルを閉じるようだ

	if (dwResult != WAIT_OBJECT_0) {
		//	きっと、このスレッドハンドルの所有権がすでに
		//	破棄されていたのだと思う..これは気にしないで良い
		return 1;
	}
	::CloseHandle(dwThreadHandle);
	// このハンドル明示的に閉じてリソース解放しとこっと

	//	終了したので．．
	return 0;
}

LRESULT CThread::JumpToThread() {
	StopThread();
	InitEvent();

	m_bCreatedThread = false;

	m_dwRunningThreadID = ::GetCurrentThreadId();
	//	スレッドIDの保存
	//	(実はこれ、スレッドハンドルと同じものであるのは保証されているのだが．．)

	{
		//	一応スレッドマネージャに登録しておく
		CThreadManager::GetObj()->AddThread(this);
		GetThreadExecute()->SetEvent();

		ThreadProc();				//	これを実行するのだが

		GetThreadExecute()->ResetEvent();
		//	スレッドマネージャから削除する
		CThreadManager::GetObj()->DelThread(this);

	}

	m_dwRunningThreadID = (DWORD)-1;	//	スレッド終了したぽん
	return 0;
}

//void CThread::ThreadCallBack(LPVOID lpVoid){
unsigned __stdcall CThread::ThreadCallBack( void * lpVoid){
	//	↑_beginthreadexを用いるので変更しました
	
	CThread* pThread = reinterpret_cast<CThread*>(lpVoid);

	pThread->m_dwRunningThreadID = ::GetCurrentThreadId();
	//	スレッドIDの保存

	//	スレッドマネージャに登録しておく
	CThreadManager::GetObj()->AddThread(pThread);

	//	このフラグは読み取り専用なのでここで書き換えて良い
	const smart_ptr<function_callback> fn(pThread->GetCallBack());
	pThread->GetThreadExecute()->SetEvent();
		//	↑このフラグをたおすと、このスレッドを生成したスレッドが
		//	CreateThreadから抜けて、そのあとSetFunctionでCallBackすべき
		//	関数を書き換えてしまい、ここで衝突することがある
	pThread->GetThreadStarting()->SetEvent();

	IThread::setThread(pThread);	//	スレッドを設定しておく

	if (fn.isNull()){
		pThread->ThreadProc();
	} else {
		fn->run();	//	コールバックする
		pThread->m_fn.Delete();			//	function_callbackを解体する
	}

	IThread::unsetThread();		//	設定しておいたスレッド*を削除

	/**
		Threadを念のためにInvalidateして、
		このスレッドに対してwaitしているスレッドに通知してやる
	*/
	pThread->InvalidateThread();

	//	スレッドマネージャから削除する
	CThreadManager::GetObj()->DelThread(pThread);
		//	DeleteThreadにおいてStopThreadされる

	return 0;
}

void	CThread::InvalidateThread(){
	CCriticalLock cs(GetCriticalSection());
	{
		CCriticalLock cl(GetLockObject()->GetCriticalSection());

		//	待ちリストに入っている奴を、解放してやる
		GetLockObject()->notifyThreadInWaitList(GetThreadId());

		//	同期オブジェクトもシグナル状態にしてやる
		GetExistEvent()->SetEvent();		//	存在しない
//		GetThreadExecute()->ResetEvent();	//	実行されていない
	}
}

//////////////////////////////////////////////////////////////////////////////
//	Java互換Thread機能も、楽じゃないよ．．

LRESULT	CThread::wait(int nTimeOut /*=-1*/)
		#ifdef USE_EXCEPTION
			throw(CInterruptedException)
		#endif
{
	///		wait キューに並ぶ
	if (interrupted()){
		#ifdef USE_EXCEPTION
			throw CInterruptedException();
		#endif
	}
	LRESULT lr = GetLockObject()->wait(nTimeOut);
	/*	//	これは心配せんでもCLockObjectが投げよる
	if (interrupted()){
		#ifdef USE_EXCEPTION
			throw CInterruptedException();
		#endif
	}
	*/
	return lr;
}

LRESULT CThread::sleep(int nTime)
		#ifdef USE_EXCEPTION
			throw(CInterruptedException)
		#endif
{
	//	nTime [ms]待つ。待っている最中に、同期オブジェクトがシグナル状態
	//	（スレッドを終了させなさい、という状態）になれば、非0が返る。

	if (interrupted()){
		#ifdef USE_EXCEPTION
			throw CInterruptedException();
		#endif
	}

	const HANDLE h[] = {
		//	例外イベントと生存イベントとの両方を監視する必要がある
		GetExistEvent()->GetHandle(),
		GetExceptionEvent()->GetHandle()
	};

	DWORD dwResult =
		::WaitForMultipleObjects(
			2,&h[0],FALSE,nTime
		);

	if (interrupted()){
		#ifdef USE_EXCEPTION
			throw CInterruptedException();
		#endif
	}
	return dwResult == WAIT_TIMEOUT ? 0:1;
}

void	CThread::notify(){
	/// waitで待っているスレッドを叩き起こす(1つのみ)
	GetLockObject()->notify();
}

void	CThread::notifyAll(){
	/// waitで待っているすべてのスレッドを叩き起こす
	GetLockObject()->notifyAll();
}

void	CThread::interrupt(){
	/**
		割り込みをかける。この関数は、他スレッドから呼び出す。
		そうすると、このスレッドは、次のwaitかsleepで
			CInterruptedException例外を発生させる。
	*/
	CCriticalLock cl(GetLockObject()->GetCriticalSection());
	GetLockObject()->eraseThreadInWaitList(GetThreadId());
		//	待っているならば、その待ちを削除する
	GetExceptionEvent()->SetEvent();
	//	sleepしている可能性もあるので、それも有効にしてやる

}

bool	CThread::isInterrupted() const{
	///	割り込み状態になっているか
	CThread* pThis = const_cast<CThread*>(this);
	return pThis->GetExceptionEvent()->IsSignal();
}

bool	CThread::interrupted() {
	///	割り込み状態になっているかを返すが、その後、
	///	割り込み状態はリセットする。
	CCriticalLock cl(GetLockObject()->GetCriticalSection());
		//	一応Lockしておこう。

	bool bInterrupted = GetExceptionEvent()->IsSignal();
	GetExceptionEvent()->ResetEvent();
	return bInterrupted;
}

//////////////////////////////////////////////////////////////////////////////
//	ThreadManager
//////////////////////////////////////////////////////////////////////////////
singleton<CThreadManager> CThreadManager::m_obj;

void	CThreadManager::AddThread(IThread* pThread){
	CCriticalLock cl(GetLockObject()->GetCriticalSection());
	//	重複登録まで心配せんでええでしょ
	GetThreadList()->push_back(pThread);
	GetLockObject()->notifyAll();
}

void	CThreadManager::DelThread(IThread* pThread){
	CCriticalLock cl(GetLockObject()->GetCriticalSection());

	{
		threadlist::iterator it =
			find(GetThreadList()->begin(),GetThreadList()->end(),pThread);
		if (it!=GetThreadList()->end()){
		//	見つからんはずが無いんやけどなぁ．．
			GetThreadList()->erase(it);
		}
	}

	//	このあと、自分が所有権を持っているスレッドが終了したならば、
	//	それをeraseしておく
	{
		smart_list_ptr<CThreadList>::iterator it =
			GetMyThreadList()->begin();
		while (it!=GetMyThreadList()->end()){
			if ((*it)->pThread.get() == pThread){
			//	所持している実体が同じであれば．．
//				it = GetMyThreadList()->erase(it);
				//	自分のスレッドを自分が削除することは出来ない
				(*it)->bValid = false; // deleteマークをつける
				it++;
			} else {
				it++;
			}
		}
	}

	GetLockObject()->notifyAll();
}

bool	CThreadManager::StopAllThread() {
	if (isStopAllThread()) return false;
	/*
		全スレッドの停止勧告がすでに出ているのならば、
		このスレッドが停止させようとしても、
		その停止勧告を出しているスレッドを停止させることは
		出来ないので、デッドロックに陥る。
		よって、ここでは、balkすべき
	*/

	CCriticalLock cl(GetLockObject()->GetCriticalSection());

	GarbageThread();	//	ガーベジした結果、すべて終了しているかも知れない

			//	↓自分自身のスレッドも数に入るはずなので、1以下を条件とする
	while (!(
				(
					(GetThreadList()->empty())
				||
					(
						(GetThreadList()->size()==1)
					&&
						((*GetThreadList()->begin())->GetThreadId()
							==GetCurrentThreadId())
					)
				)
			&&
			//	管理しているスレッドが無いか、１つあったとしても、それがこのスレッド自身であるか
				(
					(GetMyThreadList()->empty())
				||
					(
						(GetMyThreadList()->size()==1)
					&&
						((*GetMyThreadList()->begin())->pThread->GetThreadId()
							==GetCurrentThreadId())
					)
				)
			)){

				/** /	//	debug用テストコード
				int n1 = GetThreadList()->size();
				int n2 = GetMyThreadList()->size();
				DWORD n3 = (*GetThreadList()->begin())->GetThreadId();
				DWORD n4 = (*GetMyThreadList()->begin())->pThread->GetThreadId();
				DWORD n5 = GetCurrentThreadId();
				//*/

	///	GetMyThreadListは、これから実行されるべきスレッドも含まれる
	///	要するに、GetThreadListで取得されるスレッドが仮にemptyであったとしても
	///	GetMyThreadListがemptyでなければ、終了条件を満たしていないことになる
	//	全スレッドに停止命令を勧告
		threadlist::iterator it = GetThreadList()->begin();
		while (it!=GetThreadList()->end()){
//			(*it)->StopThread();
			/**
				このスレッドが他のスレッドを停止させようにも、
				そのスレッドが終了するときに、上記のスレッドリストから
				自分を取り除こうとするが、そのため上記チェインに
				critical lockをかけることになって、そこでデッドロックに陥る。
			*/
			(*it)->InvalidateThread();	//	こっちか？
			it++;
		}
		//	↑毎回これを行なわないと、
		//	新たなスレッドが作成される可能性がある
		GetLockObject()->wait();
		GarbageThread();
	}
	return true;
}

//	すべてのスレッドが終了するのを待つ
void	CThreadManager::WaitAllThreadEnded() {
	CCriticalLock cl(GetLockObject()->GetCriticalSection());

	GarbageThread();	//	ガーベジした結果、すべて終了しているかも知れない
	while (!(GetThreadList()->empty() && GetMyThreadList()->empty())){
	///	GetMyThreadListは、これから実行されるべきスレッドも含まれる
	///	要するに、GetThreadListで取得されるスレッドが仮にemptyであったとしても
	///	GetMyThreadListがemptyでなければ、終了条件を満たしていないことになる
		GetLockObject()->wait();
		//	このwaitを呼び出せばbusy waitにはならない
		GarbageThread();
	}
}

void	CThreadManager::_RegistThread(
	const shared_ptr<IThread>& pThread,
	const shared_obj&	obj/*=NULL*/
){
	CCriticalLock cl(GetLockObject()->GetCriticalSection());
	CThreadList* p = new CThreadList;
	p->pThread	= pThread;
	p->obj = obj;
	GetMyThreadList()->push_back(smart_ptr<CThreadList>(p));
}

void	CThreadManager::GarbageThread(){
	//	lockはかけないから、外部でlockしといてね
	//	このあと、自分が所有権を持っているスレッドが終了したならば、
	//	それをeraseしておく
	{
		smart_list_ptr<CThreadList>::iterator it =
			GetMyThreadList()->begin();
		while (it!=GetMyThreadList()->end()){
			if (!(*it)->bValid){
			//	deleteマークがついているならば
				it = GetMyThreadList()->erase(it);
			} else {
				it++;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////

///	RunThreadの場合は、CAppFrameのRunを呼び出すことを保証する
LRESULT CThreadManager::RunThread(IAppBase*pAppBase){
	return RunThread(shared_ptr<IAppBase>(pAppBase));
}

LRESULT CThreadManager::RunThread(const shared_ptr<IAppBase>& pAppBase){
	shared_ptr<IAppBase>* p = new shared_ptr<IAppBase>(pAppBase);

	shared_ptr<IThread> pThread(pAppBase->GetWorkerThread(),false);
		//	↑このスレッドの停止を、このオブジェクトの解体フェイズとする

	GetObj()->_RegistThread(pThread,shared_obj(p));
	//	smart_ptr_baseにキャストされる

	return pAppBase->Run();	//	このあと実行する
}

///	CreateThreadの場合は、IThreadのCreateThreadを呼び出すことを保証する
LRESULT CThreadManager::CreateThread(IThread*pThread){
	return CreateThread(shared_ptr<IThread>(pThread));
}

LRESULT CThreadManager::CreateThread(const shared_ptr<IThread>& pThread){
	RegistThread(pThread);
	return pThread->CreateThread();	//	スレッドの作成
}

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd
