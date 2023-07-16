//
// yaneThreadPooler.h :
//
//	CThreadPool,CThreadManagerはatl等で定義されている(;´Д`)
//	

#ifndef __yaneThreadPooler_h__
#define __yaneThreadPooler_h__


#include "../Thread/yaneThread.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

class CThreadPoolerRequestQueue {
/**
	CThreadPoolerが用いるコールバックのためのqueue
	これ単独でも使用することは出来る
*/
public:
	///	いま関数を実行しているスレッドの数
	int	getInvokeCount() const { return m_nInvokeCount; }
	void setInvokeCount(int nInvokeCount){
		synchronized g(getLock());
		m_nInvokeCount = nInvokeCount;
		getLock()->notifyAll();	//	これを待ってるやつがいるかも知れん
	}
	void incInvokeCount() {
		synchronized g(getLock());
		m_nInvokeCount++;
		getLock()->notifyAll();	//	これを待ってるやつがいるかも知れん
	}
	void decInvokeCount() {
		synchronized g(getLock());
		m_nInvokeCount--;
		getLock()->notifyAll();	//	これを待ってるやつがいるかも知れん
	}

	///	プールされている全スレッドの数の設定/取得
	int getThreadCount() const { return m_nThreadCount; }
	void setThreadCount(int nThreadNum) {
		synchronized g(getLock());
		m_nThreadCount = nThreadNum;
		getLock()->notifyAll();	//	これを待ってるやつがいるかも知れん
	}
	void decThreadCount() {
		synchronized g(getLock());
		m_nThreadCount--;
		getLock()->notifyAll();	//	これを待ってるやつがいるかも知れん
	}
	void incThreadCount() {
		synchronized g(getLock());
		m_nThreadCount++;
		getLock()->notifyAll();
	}

	///	requestのget/set
	smart_ptr<function_callback> getRequest() {
		synchronized g(getLock());
		smart_ptr<function_callback> fn;
		if (m_bInvalidate) return fn; // ダメやん

		while (m_aInvokeList.size()==0){
			getLock()->wait();
			if (m_bInvalidate) return fn; // ダメやん
		}
		fn = *m_aInvokeList.begin();
		m_aInvokeList.pop_front();
		return fn;
	}
	void setRequest(const smart_ptr<function_callback>& req){
		synchronized g(getLock());
		m_aInvokeList.push_back(req);
		getLock()->notifyAll();
	}

	///	getRequestで待機しているスレッドに対してinvalidateを通知
	void invalidateAllThread(){
		synchronized g(getLock());
		//	WorkerThreadのなかでwaitしている全スレッドに通知
		m_bInvalidate = true;
		getLock()->notifyAll();
	}
	void setValid(bool bValid) {
		m_bInvalidate = !bValid;
	}

	bool isDone(){
		synchronized g(getLock());
	//	終了条件＝受注している仕事がなくて、かつ、実行している仕事が無い
		return	getInvokeCount()==0 && getInvokeList()->size()==0;
	}

	LRESULT waitAllThreadEnded(int nTimeOut){
		synchronized g(getLock());
		while (!isDone()){
			LRESULT lr = getLock()->wait(nTimeOut);
			if (lr!=0) return lr;
		}
		return 0;
	}

	///	受注している仕事のリスト
	smart_list_ptr<function_callback>* getInvokeList()
		{ return &m_aInvokeList;}

	///	↑をいじるときは、オブジェクトのlockが必要
	CLockObject* getLock() { return& m_vLockObject; }

	CThreadPoolerRequestQueue()
		:m_nInvokeCount(0),m_nThreadCount(0),m_bInvalidate(false){}

private:
	CLockObject m_vLockObject;	//	未処理の仕事のカウントのためのLockObject	
	smart_list_ptr<function_callback> m_aInvokeList;
	int m_nInvokeCount;
	int m_nThreadCount;
	bool m_bInvalidate;
};

class CThreadPoolerThread : public CThread {
/**
	CThreadPoolerで使用する待機スレッド
*/
public:
	CThreadPoolerThread(const shared_ptr<CThreadPoolerRequestQueue>& queue_)
		: queue(queue_){
		queue->incThreadCount();	//	実行中のスレッドカウントを加算する
	//	実行中のスレッドカウントは、コンストラクタでincして、
	//	ThreadProcを抜けるときにdecする
	}

	virtual void ThreadProc();
	~CThreadPoolerThread(){ StopThread(); }
	//	スレッドクラスから派生させる場合は、
	//	必ず派生クラス側でStopThreadを行なう必要がある

private:
	shared_ptr<CThreadPoolerRequestQueue> queue;
};

class CThreadPooler {
public:
	///	スレッドを指定した数だけ起動して待機させる
	LRESULT	StartThread(int nThreadNum);

	///	起動しているスレッドに指定した関数を実行させる
	void	Invoke(const smart_ptr<function_callback>&fn);

	///	保持しているスレッドをすべて停止させる
	void	ClearAllThread();

	///	Invokeしたのに処理されていない仕事の数が0になればtrue
	bool	IsDone() const;

	///	処理されていない仕事の数が0になるのを待つ
	///	0:ok  1:timeout
	LRESULT	wait(int nTimeOut=-1);

	CThreadPooler::CThreadPooler(){ m_pQueue.Add();}
	virtual ~CThreadPooler(){ ClearAllThread(); }

private:
	smart_vector_ptr<CThreadPoolerThread> m_aThread;
	shared_ptr<CThreadPoolerRequestQueue> m_pQueue;
};

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd

#endif
