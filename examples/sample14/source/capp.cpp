#include "stdafx.h"
#include "capp.h"

//
//	結城先生の「マルチスレッドのデザインパターン」本を
//	副読本として読むこと！
//

/////////////////////////////////////////////////////////////////
//	sample 1 : single thread executionパターン

namespace test1 {

struct CCommonResource {
	void test1(int nThreadNo){
		synchronized g(cs);
		//	共通リソースへのアクセス
		CDbg().Out("test1 : threadNo = "
			+CStringScanner::NumToString(nThreadNo));
	}
	void test2(int nThreadNo){
		synchronized g(cs);
		//	共通リソースへのアクセス
		CDbg().Out("test2 : threadNo = "
			+CStringScanner::NumToString(nThreadNo));
	}
protected:
	CCriticalSection cs;
};

struct CUserThread : public CThread {
	CUserThread(const shared_ptr<CCommonResource>& cr
		,int nThreadNo) : cr_(cr),nThreadNo_(nThreadNo) {}

	virtual void ThreadProc() {
		for(int i=0;i<100;++i){
			cr_->test1(nThreadNo_);
			cr_->test2(nThreadNo_);
		}
	}

protected:
	shared_ptr<CCommonResource> cr_;
	int nThreadNo_;
};

void Sample()
{
	shared_ptr<CCommonResource> cr(new CCommonResource);
	CThreadManager::CreateThread(new CUserThread(cr,0));
	CThreadManager::CreateThread(new CUserThread(cr,1));
	//	2つのスレッドからひとつの共用リソースに対してアクセスする
}

} // end of namespace

/////////////////////////////////////////////////////////////////
//	sample 2 : immutableパターンはwrite動作のないresourceクラス
//	サンプルは割愛
/////////////////////////////////////////////////////////////////
//	sample 3 : guarded suspension
namespace test3 {

struct CRequest { string strData; };

struct CRequestQueue {
/**
	CClientThreadからはputRequest(キューに積む)される。
	CServerThreadからはgetRequest(キューからデータを取得する)される。
	もし、CServerThreadからgetRequestされたときにキューにデータが
	存在しなければ、CClientThreadからputRequestされるのを待つ
*/
	typedef smart_list_ptr<CRequest> request_queue;
		//　　↑list<smart_ptr<CRequest> >の意味

	smart_ptr<CRequest> getRequest(){
		synchronized g(&m_lock);
		while (m_queue.size() ==0){
			m_lock.wait();	//	データがまだ存在しないので待つ
		}
		smart_ptr<CRequest> req = *m_queue.begin();
		m_queue.pop_front();
		return req;
	}
	void putRequest(const smart_ptr<CRequest>& req){
		synchronized g(&m_lock);
		m_queue.push_back(req);
		m_lock.notifyAll();
		//	このキューに対するpush_backを待っていたかも
		//	知れないすべてのスレッドに通知する
	}

protected:
	CLockObject m_lock;
	request_queue m_queue;
};

struct CClientThread : public CThread {
	CClientThread(const shared_ptr<CRequestQueue>& rq):rq_(rq){}

	virtual void ThreadProc() {
		for(int i=0;i<10;++i){
			smart_ptr<CRequest> req(new CRequest);
			req->strData = CStringScanner::NumToString(i);
			getRequestQueue()->putRequest(req);
			IThread::getThread()->sleep(2000);
			//	2秒ずつ待つとしよう
		}
	}
protected:
	shared_ptr<CRequestQueue> getRequestQueue() { return rq_;}
	shared_ptr<CRequestQueue> rq_;
};

struct CServerThread : public CThread {
	CServerThread(const shared_ptr<CRequestQueue>& rq):rq_(rq){}
	virtual void ThreadProc() {
		for(int i=0;i<10;++i){
			smart_ptr<CRequest> req = getRequestQueue()->getRequest();
			//	リクエストキューからデータを取得

			CDbg().Out(req->strData);

			//	仮にここでwaitを入れたとしても、RequestQueueが
			//	バッファリングしてくれるので安全に取り出せる
			//	IThread::getThread()->sleep(4000);
		}
	}
protected:
	shared_ptr<CRequestQueue> getRequestQueue() { return rq_;}
	shared_ptr<CRequestQueue> rq_;
};

void Sample()
{
	shared_ptr<CRequestQueue> rq(new CRequestQueue);
	CThreadManager::CreateThread(new CClientThread(rq));
	CThreadManager::CreateThread(new CServerThread(rq));
	//	2つのスレッドからひとつのリクエストキューに対してアクセスする
}

} // end of namespace

/////////////////////////////////////////////////////////////////
//	sample 4 : balking
//	balkは処理をせず帰るパターン。サンプル割愛。
/////////////////////////////////////////////////////////////////
//	sample 5 : producer-consumer
//		guarded suspention パターンの、requestqueueのqueueサイズに
//		制限があるものと考えるとわかりやすい。
//
//		イベントディスパッチングには
//		producerが複数あって、consumerが単一というモデルを使うことが多い
//		（こうすることにより、consumerはシングルスレッド処理が可能になる）

namespace test5 {

struct CData { string strData; };

struct CChannel {
/**
	CProducerThreadからはputData(データを置く)される。
		置かれたデータが、queue構造で管理しているかどうかは実装による。
	CConsumerThreadからはgetData(データを取り出す)される。

	もし、CConsumerThreadからgetDataされたときにデータが
	存在しなければ、CProducerThreadからputDataされるのを待つ
*/
	typedef smart_list_ptr<CData> data_queue;
		//　　↑list<smart_ptr<CRequest> >の意味
	enum { bufferlength = 3 };

	smart_ptr<CData> getData(){
		synchronized g(&m_lock);
		while (m_queue.size() ==0){
			m_lock.wait();	//	データがまだ存在しないので待つ
		}
		smart_ptr<CData> data = *m_queue.begin();
		m_queue.pop_front();

		//	取り出したので通知しておく
		m_lock.notifyAll();
		return data;
	}
	void putData(const smart_ptr<CData>& req){
		synchronized g(&m_lock);
		while (m_queue.size() >= bufferlength){
			m_lock.wait();
			//	データの置き場所がいっぱいなので空きが出来るのを待つ
		}

		m_queue.push_back(req);
		m_lock.notifyAll();
		//	このキューに対するpush_backを待っていたかも
		//	知れないすべてのスレッドに通知する
	}

protected:
	CLockObject m_lock;
	data_queue m_queue;
};

struct CProducerThread : public CThread {
	CProducerThread(const shared_ptr<CChannel>& cn,
		const string& name):cn_(cn),name_(name){}

	virtual void ThreadProc() {
		for(int i=0;i<10;++i){
			smart_ptr<CData> data(new CData);
			CDbg().Out(name_+"が出来たわよ！");
			data->strData = name_;
			getChannel()->putData(data);

			IThread::getThread()->sleep(1000);	//	1秒ずつ待つとしよう
		}
	}
protected:
	shared_ptr<CChannel> getChannel() { return cn_;}
	shared_ptr<CChannel> cn_;
	string name_;
};

struct CConsumerThread : public CThread {
	CConsumerThread(const shared_ptr<CChannel>& cn):cn_(cn){}
	virtual void ThreadProc() {
		for(int i=0;i<20;++i){
			smart_ptr<CData> data = getChannel()->getData();
			//	channel(仲介役)からデータを取得

			CDbg().Out("そんなの食えねぇよ！ヽ(`Д´)ノ＞"+data->strData);

			//	仮にここでwaitを入れたとしても、Channel役が
			//	バッファリングしてくれるので安全に取り出せる

			// IThread::getThread()->sleep(4000);
		}
	}
protected:
	shared_ptr<CChannel> getChannel() { return cn_;}
	shared_ptr<CChannel> cn_;
};

void Sample()
{
	shared_ptr<CChannel> rq(new CChannel);
	CThreadManager::CreateThread(new CConsumerThread(rq));
	CThreadManager::CreateThread(new CProducerThread(rq,"殺人カレー"));
	CThreadManager::CreateThread(new CProducerThread(rq,"激マズインスタントラーメン"));
	//	2つのproducerスレッドから
	//	ひとつのリクエストキューに対してアクセスする
}

} // end of namespace

/////////////////////////////////////////////////////////////////
//	sample 6 : read-write lock
//	read-readでは衝突しないことを利用して、
//	writeするときだけlockする。読み込みに対してはlockせずに
//	パフォーマンスを上げる。読み込みが頻繁に行なわれるときは、
//	読み込みに対する衝突が生じないのでパフォーマンスがあがる

//	結城先生のマルチスレッド本を持っていれば
//	そのp.187のサンプルと比較せよ
namespace test6 {

struct CReadWriteLock {
//	read-write lockを提供するクラス。このクラス自体は汎用性があるので
//	このまま流用できるだろう
	CReadWriteLock():
		readingReaders(0),waitingWriters(0),
		writingWriters(0),preferWriter(true){}

	void readLock() {
		synchronized guard(getLockObject());
		while (writingWriters > 0 || (preferWriter && waitingWriters>0)){
			getLockObject()->wait();
		}
		readingReaders++;
	}

	void readUnlock() {
		synchronized guard(getLockObject());
		readingReaders--;
		preferWriter = true;
		getLockObject()->notifyAll();
	}

	void writeLock() {
		synchronized guard(getLockObject());
		waitingWriters++;
		try {
			while (readingReaders > 0 || writingWriters >0){
				getLockObject()->wait();
			}
		} catch (...){
		//	wait()でインタラプト例外が発生したときにwaitingWritersを
		//	デクリメントする必要がある。
		//	finallyが欲しいんだが..
				waitingWriters--;
				throw ;
		}
		waitingWriters--;
		writingWriters++;
	}
	void writeUnlock(){
		synchronized guard(getLockObject());
		writingWriters--;
		preferWriter = false;
		getLockObject()->notifyAll();
	}

protected:
	int readingReaders;	//	読み込み中のスレッドの数
	int waitingWriters;	//	書くのを待っているスレッドの数
	int writingWriters;	//	書き込み中のスレッドの数
	bool preferWriter;	//	書き込みを優先するならばtrue
		//	(スレッドの生存性を下げないための工夫)

	CLockObject* getLockObject(){ return &lockobject;}
	CLockObject lockobject;
};

struct CCommonResource {
	string read(){
		//	読み込みに時間かかるんだ
		::Sleep(500);
		return str_;
	}
	void write(const string& str){
		//	書き込みにも時間かかるんだ
		::Sleep(3000);
		str_ = str;
	}

protected:
	string str_;
};

struct CReaderThread : public CThread {
	CReaderThread(const shared_ptr<CReadWriteLock>& rwlock,
		const shared_ptr<CCommonResource>& cr,const string&threadname):
			rwlock_(rwlock),cr_(cr),threadname_(threadname){}

	virtual void ThreadProc() {
		for(int i=0;i<30;++i){
			
			string str;
			getLock()->readLock();
				CDbg().Out(threadname_+"読み込み中");
				str = getResource()->read();
				//	読み込みの前後にreadLock～readUnlockが必要
				CDbg().Out(threadname_+":読み込み文字→"+str);
			getLock()->readUnlock();

			//	仮にここでwaitを入れたとしても、Channel役が
			//	バッファリングしてくれるので安全に取り出せる
			//	::Sleep(4000);
		}
	}
protected:
	shared_ptr<CReadWriteLock> getLock() { return rwlock_; }
	shared_ptr<CCommonResource> getResource() { return cr_; }
	string threadname_;

	shared_ptr<CReadWriteLock> rwlock_;
	shared_ptr<CCommonResource> cr_;
};

struct CWriterThread : public CThread {
	CWriterThread(const shared_ptr<CReadWriteLock>& rwlock,
		const shared_ptr<CCommonResource>& cr):rwlock_(rwlock),cr_(cr){}

	virtual void ThreadProc() {
		for(int i=0;i<5;++i){
			
			string str;
			getLock()->writeLock();
				CDbg().Out("書き込み中");
				if (i&1) {
					getResource()->write("るぅりん、何かくださいみゅ～☆");
				} else {
					getResource()->write("レイホウさん、あびばびば～！");
				}
				//	書き込みの前後にwriteLock～writeUnlockが必要
			getLock()->writeUnlock();

			::Sleep(3000);	//	3秒に1回書き込み
		}
	}
protected:
	shared_ptr<CReadWriteLock> getLock() { return rwlock_; }
	shared_ptr<CCommonResource> getResource() { return cr_; }

	shared_ptr<CReadWriteLock> rwlock_;
	shared_ptr<CCommonResource> cr_;
};

void Sample()
{
	shared_ptr<CReadWriteLock> rwlock(new CReadWriteLock);
	shared_ptr<CCommonResource> cr(new CCommonResource);
	CThreadManager::CreateThread(new CWriterThread(rwlock,cr));
	//	読み込みスレッドを3つ用意する
	CThreadManager::CreateThread(new CReaderThread(rwlock,cr,"あゆみ㌧"));
	CThreadManager::CreateThread(new CReaderThread(rwlock,cr,"もなみ"));
	CThreadManager::CreateThread(new CReaderThread(rwlock,cr,"れな"));
}

} // end of namespace

/////////////////////////////////////////////////////////////////
//	sample 7 : thread-per-message
//	メッセージ(要求)ごとにスレッドをひとつ作ってそいつに委譲して
//	応答性をあげる
namespace test7 {

struct CHelper : public CThread {
	CHelper(const string& str) : str_(str) {}
	virtual void ThreadProc() {
		Slowly(); // すごく処理に時間のかかる関数だと思いねぇ
		CDbg().Out(str_);
	}
	void Slowly() { ::Sleep(3000); }
protected:
	string str_;
};

struct CHost {
	void	doWork(const string& str){
		//	これを処理するために、スレッドをひとつ作って、そいつに委譲する
		CThreadManager::CreateThread(new CHelper(str));
		//	inner classが定義できればここに書けば済むのだが..
	}
};

void Sample()
{
	CHost host;
	host.doWork("仕事をしろにょ！");
	host.doWork("馬車馬のように働けにょ！");
	host.doWork("こっぺパン一個で末永く暮らせにょ！");
	CDbg().Out("---終了---");
	//	このサンプルを実行すると、終了の文字が先に表示され、
	//	仕事をしろにょ！以下の３つのメッセージは３秒後に表示される
	//	これは、CHost::doWorkがスレッドを作成して(そちらに処理を委譲して)
	//	応答を即座に返したことを意味する
}

} // end of namespace
/////////////////////////////////////////////////////////////////
//	sample 8 : worker thread(thread pool)
//	スレッドを事前に生成しておき、使いまわすことで生成～解体コストを削減する
//	このクラスはCThreadPoolとしてyaneSDK3rd/Thread/yaneThreadPooler.hにある。
//	実装のほうは、そちらを参考にしていただきたい。

namespace test8 {

void work(int n){
	string id = CStringScanner::NumToString(::GetCurrentThreadId());
	CDbg().Out("1から"+CStringScanner::NumToString(n)
		+"までの和を求めるぜ:Thread ID="+id);
	int nTotal = 0;
	for(int i=1;i<=n;i++) {
		nTotal+=i;
		::Sleep(10); // ちょっと計算するのに時間がかかるとしよう
	}
	CDbg().Out("1から"+CStringScanner::NumToString(n)+"までの和は"
		+CStringScanner::NumToString(nTotal)+"だぜ:Thread ID="+id);
}

void Sample()
{
	CThreadPooler pool;
	pool.StartThread(3);	// 3個のスレッドを待機させる
	//				↑この数字を変更して、どう変わるか違いを見よ。
	for(int i=0;i<10;++i){
		smart_ptr<function_callback>
			fn(function_callback_v::Create(&work,i*100+10));
		pool.Invoke(fn);	//	待機させているスレッドに処理させる
	}
	pool.wait(); // すべてのスレッドの実行が完了するのを待つ
	CDbg().Out("ひと仕事終わったあとのビールはうまいにょ○(≧∇≦)o");
}

} // end of namespace
/////////////////////////////////////////////////////////////////
//	sample 9 : future
//		スレッドに仕事をさせるとき、その引換券だけを先に渡すパターン
//		thread-per-message等では必ずこのパターンで引数の受け渡しを行なう

namespace test9 {

struct CData {	//	これが引換券の役割を果たす
	void setData(const string& str){
		synchronized guard(lock);
		if (bReady) return ; // balk
		str_ = str;
		bReady = true;
		lock.notifyAll(); // setData待ちの奴を叩き起こす
	}
	string getData(){
		synchronized guard(lock);
		while (!bReady){
			lock.wait();
			// データが用意されていないので
			//	setDataされるのを待つ
		}
		return str_;
	}

	CData():bReady(false){}
protected:
	bool bReady;
	string str_; // これが保持されているデータだと思いねぇ

	CLockObject lock;
};

//	ここ以下は、thread-per-messageパターンと同じ
struct CHelper : public CThread {
	CHelper(const string& str,shared_ptr<CData>& data)
		: str_(str),data_(data) {}
	virtual void ThreadProc() {
		Slowly(); // すごく処理に時間のかかる関数だと思いねぇ
		data_->setData("データが用意でけたーヽ（´ー｀）ノ");
		//	この文字列を設定した瞬間、メインスレッドがgetDataに成功する

		IThread::getThread()->sleep(2000); // 2秒待つ
		CDbg().Out(str_); // 「仕事をしろにょ」が表示される
	}
	void Slowly() { ::Sleep(3000); }
protected:
	shared_ptr<CData> data_;
	string str_;
};

struct CHost {
	void	doWork(const string& str,shared_ptr<CData>& data){
		//	これを処理するために、スレッドをひとつ作って、そいつに委譲する
		CThreadManager::CreateThread(new CHelper(str,data));
		//	inner classが定義できればここに書けば済むのだが..
	}
};

void Sample()
{
	CHost host;
	shared_ptr<CData> data(new CData); // ここにデータが入ってくる
	host.doWork("仕事をしろにょ！",data);
	CDbg().Out("--応答はすぐ帰る--");

	string str = data->getData();
	//	getDataすると、このデータがサブスレッドによって
	//	用意されるまで待たされる。

	CDbg().Out("仕事結果→"+str);
}

} // end of namespace

/////////////////////////////////////////////////////////////////
//	sample 9a:
//	futureパターンのCDataを引数ではなく返し値にする場合。
//	(CHost::doWork関数を、sample9と比較せよ)

namespace test9a {

struct CData {	//	これが引換券の役割を果たす
	void setData(const string& str){
		synchronized guard(lock);
		if (bReady) return ; // balk
		str_ = str;
		bReady = true;
		lock.notifyAll(); // setData待ちの奴を叩き起こす
	}
	string getData(){
		synchronized guard(lock);
		while (!bReady){
			lock.wait();
			// データが用意されていないので
			//	setDataされるのを待つ
		}
		return str_;
	}

	CData():bReady(false){}
protected:
	bool bReady;
	string str_; // これが保持されているデータだと思いねぇ

	CLockObject lock;
};

//	ここ以下は、thread-per-messageパターンと同じ
struct CHelper : public CThread {
	CHelper(const string& str,shared_ptr<CData>& data)
		: str_(str),data_(data) {}
	virtual void ThreadProc() {
		Slowly(); // すごく処理に時間のかかる関数だと思いねぇ
		data_->setData("データが用意でけたーヽ（´ー｀）ノ");
		//	この文字列を設定した瞬間、メインスレッドがgetDataに成功する

		IThread::getThread()->sleep(2000); // 2秒待つ
		CDbg().Out(str_); // 「仕事をしろにょ」が表示される
	}
	void Slowly() { ::Sleep(3000); }
protected:
	shared_ptr<CData> data_;
	string str_;
};

struct CHost {
	shared_ptr<CData> doWork(const string& str){
		//	これを処理するために、スレッドをひとつ作って、そいつに委譲する
		shared_ptr<CData> data(new CData);
		CThreadManager::CreateThread(new CHelper(str,data));
		//	inner classが定義できればここに書けば済むのだが..
		return data;
	}
};

void Sample()
{
	CHost host;
	shared_ptr<CData> data = host.doWork("仕事をしろにょ！");
	CDbg().Out("--応答はすぐ帰る--");

	string str = data->getData();
	//	getDataすると、このデータがサブスレッドによって
	//	用意されるまで待たされる。

	CDbg().Out("仕事結果→"+str);
}

} // end of namespace
/////////////////////////////////////////////////////////////////
//	sample 10 : two phase termination
//	終了処理を2段階で行ないたいときに使う

namespace test10 {

struct CCountUpThread : public CThread {
	CCountUpThread():nCount(0),bShutDownInterrupt(false){}

	virtual void ThreadProc() {
		try { 
			try {
				while (!bShutDownInterrupt){
					CDbg().Out(++nCount);
					sleep(5000);	//	このsleep中に割り込み例外が発生する
				}
			} catch(CInterruptedException&){
				CDbg().Out("--sleep中に割り込みが発生した--");
			}
		} catch (...) {		//	finallyが欲しい..
			terminate(); throw ;
		}
		terminate();
	}

	void terminate(){	//	終了処理用の関数
		CDbg().Out("終了のあとかたづけちゅう");
		::Sleep(3000);
		CDbg().Out("終了のあとかたづけ完了!!");
	}

	void shutDownRequest(){
		//	シャットダウン要求によりスレッドがinvalidateされる
		bShutDownInterrupt=true;
		interrupt();	//	CThread::sleepで待ってたら、たたき起こす
	}

protected:
	bool	bShutDownInterrupt;
	int		nCount;
};

void Sample()
{
	CCountUpThread* pThread = new CCountUpThread;
	pThread->CreateThread();
	::Sleep(1000);	//	1秒待つ
	CDbg().Out("終了要求を出します");
	pThread->shutDownRequest(); // シャットダウンを要求
	CDbg().Out("スレッドの終了を待ちます");
	pThread->StopThread(); // スレッドの停止を待つ
	CDbg().Out("スレッド終了");
}

} // end of namespace

/////////////////////////////////////////////////////////////////
//	sample 11 : thread-specific storage
//	yaneSDK3rdのThreadLocalなので使用例は割愛
/////////////////////////////////////////////////////////////////
//	sample 12 : active object(actor)
//	非同期な要求を受理する能動的オブジェクト
//	バックグラウンドでworker threadが1つ走っていて、
//	そいつが処理を行なう

namespace test12 {

//	スマートポインタいちいち書くの面倒
#define p(T) shared_ptr<T>

// 引数いちいち書くの面倒
#define p_(T) const shared_ptr<T>&

//	Java風にしておこう
#define Object shared_ptr<void>
#define Object_ const shared_ptr<void>&

struct IResult {
//	futureパターンで返し値をいただくための基底クラス
	virtual Object getResultValue()=0;
};

struct CRealResult : public IResult {
//	返し値を保持するためのクラス
	CRealResult(Object_ resultValue){
		this->resultValue = resultValue;
	}
    Object getResultValue() {
        return resultValue;
    }
protected:
	Object resultValue;
};

struct CFutureResult : public IResult {
//	futureパターンでResultを返す
	CFutureResult():ready(false){}
    void setResult(p_(IResult) result) {
		synchronized g(&lock);
		this->result = result;
        this->ready = true;
		lock.notifyAll();
    }
	Object getResultValue() {
		synchronized g(&lock);
        while (!ready) {
            try {
                lock.wait();
            } catch (CInterruptedException&) {
            }
        }
        return result->getResultValue();
    }
protected:
	CLockObject lock;
	p(IResult) result;
    bool ready;
};

/*	//	↑のfutureクラスを使うためのサンプル

void test1(){
	Object	obj(new string("あびばびば～"));
	p(IResult) real(new CRealResult(obj));

	p(CFutureResult) result(new CFutureResult);

	//	このresultに対してgetResultValueを行なうと
	//	他のスレッドがsetResultするまで待たされる

	//	a.「引換券」と交換する「データ」を渡す
	result->setResult(real);

	//	b.「引換券」と「データ」を交換する
	obj = result->getResultValue();

	//	「データ」をunboxingする
	string str = *smart_ptr_static_cast<string>(obj);
	CDbg().Out(str);
}
*/

struct IActiveObject {
//	このクラスが、active object基底クラス
//	このクラスのメソッドは非同期に呼び出される
	virtual void displayString(const string&) = 0;
	//	要求１．

	virtual p(IResult) makeString(int count, char fillchar) = 0;
	//	要求２．結果は、futureパターンでもらう

	virtual void shutdown() =0;
	//	シャットダウンリクエスト(これが来たらactive objectを停止する)
};

struct CServant : public IActiveObject {
//	これは、アクティブオブジェクトの実装例
	virtual p(IResult) makeString(int count, char fillchar) {
		smart_ptr<char> buffer;
		buffer.AddArray(count+1);
		buffer[count] = '\0';
        for (int i = 0; i < count; i++) {
            buffer[i] = fillchar;
            try {
				IThread::getThread()->sleep(100);
            } catch (CInterruptedException&) {
            }
        }
		Object obj(new string(buffer.get()));
			//	stringをObject型にboxing

        return p(IResult)(new CRealResult(obj));
    }	
	virtual void displayString(const string&str) {
		try {
			CDbg().Out("displayString: " + str);
//			CThread()->getMyThread()->sleep(10);
        } catch (CInterruptedException &) {
        }
    }
	virtual void shutdown() {}
};

struct IMethodRequest {
//	active objectに投げられる要求を保存しておくための基底クラス
	virtual void execute()=0;
	//	これで実行する

protected:
	IMethodRequest(p_(CServant) servant,p_(CFutureResult) future) {
        this->servant = servant;
        this->future = future;
    }

	p(CServant) servant;
    p(CFutureResult) future;
};

struct CActivationQueue {
//	スケジュールを保持するためのキュー
	enum { MAX_METHOD_REQUEST = 100 }; // 最大リクエスト数

	void putRequest(const shared_ptr<IMethodRequest>& request) {
		synchronized g(&lock);
        while (MAX_METHOD_REQUEST < requestQueue.size()) {
			//	これ以上queuing不可能なら仕方ないので待つ
            lock.wait();
        }
		requestQueue.push_back(request);
        lock.notifyAll();
    }
	p(IMethodRequest) takeRequest() {
		synchronized g(&lock);
        while (requestQueue.size() <= 0) {
			//	仕事がないなら仕方ないので待つ
            lock.wait();
        }
        p(IMethodRequest) request = *requestQueue.begin();
		requestQueue.pop_front();
        lock.notifyAll();
        return request;
    }
protected:
	CLockObject lock;
	list<p(IMethodRequest) >	requestQueue;
};

struct CSchedulerThread : public CThread {
//	スケジュールを保持しておき順番に実行するスレッド

	CSchedulerThread(p_(CActivationQueue) queue) {
        this->queue = queue;
    }
	
	//	スケジューラーに要求を積む
	void invoke(p_(IMethodRequest) request) {
        queue->putRequest(request);
    }

	virtual void ThreadProc() {
		try {
			while (true) {
				p(IMethodRequest) request = queue->takeRequest();
				request->execute();
				//	キューに積まれている内容をひとつずつ実行するだけで良い
			}	//	終了するときはこのスレッドに対して割り込みがかかる
		} catch (CInterruptedException&){
			//	shutdownによってIThread::interruptが呼び出され、その結果
			//	このスレッドがCLockObject::waitしていればそこでinterrupt例外が
			//	発生する。（よってここに抜けてくる）
		}
		CDbg().Out("ActiveObjectを終了しました");
    }

	virtual void shutdown() {
		interrupt();
		//	このスレッドに対してCInterruptedExceptionを発生させる
	}
protected:
	p(CActivationQueue) queue;
};

struct CMakeStringRequest : public IMethodRequest {
//	要求1をIMethodRequest派生クラスにしたもの
    CMakeStringRequest(p_(CServant) servant,p_(CFutureResult) future,
		int count, char fillchar)
		: IMethodRequest(servant, future)
	{
        this->count = count;
        this->fillchar = fillchar;
    }
    void execute() {
        p(IResult) result = servant->makeString(count, fillchar);
        future->setResult(result);
		//	futureパターンで受け渡しする
    }
protected:
	int count;
    char fillchar;
};

struct CDisplayStringRequest : public IMethodRequest {
	CDisplayStringRequest(p_(CServant) servant,const string& str)
		: IMethodRequest(servant,p(CFutureResult)())
	{
        this->str = str;
    }
    void execute() {
        servant->displayString(str);
    }
protected:
    string str;
};

struct CProxy : public IActiveObject {
    CProxy(p_(CSchedulerThread) scheduler,p_(CServant) servant) {
        this->scheduler = scheduler;
        this->servant = servant;
    }
	virtual p(IResult) makeString(int count, char fillchar) {
		p(CFutureResult) future(new CFutureResult());
        scheduler->invoke(p(IMethodRequest)
			(new CMakeStringRequest(servant, future, count, fillchar))
		);
		//	スケジューラーにお願いして、futureパターンで
		//	引換券だけ渡して制御をすぐに戻す
		return smart_ptr_static_cast<IResult>(future);
		// IResultに変換して戻す必要あり
    }
    virtual void displayString(const string& str) {
        scheduler->invoke(p(IMethodRequest)
			(new CDisplayStringRequest(servant, str))
		);
		//	スケジューラーにお願いしてすぐに制御を戻す
    }
	virtual void shutdown(){
		//	スケジューラにシャットダウン要請
		scheduler->shutdown();
	}
protected:
	p(CSchedulerThread) scheduler;
	p(CServant) servant;
};

struct CMakerClientThread : public CThread {
	CMakerClientThread(const string&name,p_(IActiveObject)actor)
		:name_(name),actor_(actor){}
	virtual void ThreadProc() {
		try { 
			for(int i=1;i<=5;++i){
				p(IResult) result = actor_->makeString(i,'C');
				//	戻り値つきのactive objectの呼び出し
				Object obj = result->getResultValue();
				const string& str = *smart_ptr_static_cast<string>(obj);
				//	unboxing
				//	(Object型に封入されたstring型オブジェクトを取り出す)
				//	string str = **(smart_ptr<string>*)(obj.get());
				//	でもいいのだがコピーコストがもったいないので

				CDbg().Out(name_ + "は" + str + "を作った");
				IThread::getThread()->sleep(200);
			}
		} catch(CInterruptedException&){
			//	sleep中に割り込み例外が発生した?
		}
	}
protected:
	string	name_;
	p(IActiveObject) actor_;
};

struct CDisplayClientThread : public CThread {
	CDisplayClientThread(const string&name,p_(IActiveObject)actor)
		:name_(name),actor_(actor){}
	virtual void ThreadProc() {
		try { 
			for(int i=1;i<=10;++i){
				string data = name_ + "は"
					+ CStringScanner::NumToString(i) + "を表示する";
				actor_->displayString(data);
				IThread::getThread()->sleep(200);
				//	このsleep中に割り込み例外が発生する(かも)
			}
		} catch(CInterruptedException&){
			//	sleep中に割り込み例外が発生した?
		}
	}
protected:
	string	name_;
	p(IActiveObject) actor_;
};

struct CActiveObjectFactory {
	static p(IActiveObject) createActiveObject() {
        p(CServant) servant(new CServant());
		//	active object
		
		p(CActivationQueue) queue(new CActivationQueue());
		// 要求queue
        
		p(CSchedulerThread) scheduler(new CSchedulerThread(queue));
		//	要求を順番に保持しておくためのスレッド

		//	proxyオブジェクトが、スケジューラ(とactive object)に橋渡しする
		p(IActiveObject) proxy(new CProxy(scheduler, servant));
		CThreadManager::CreateThread(
			smart_ptr_static_cast<IThread>(scheduler));
		//	↑shared_ptr<CSchedulerThread>をshared_ptr<IThread>にupcastする
        return proxy;
    }
};

struct CShutdownRequester : public CThread {
//	10秒後にshutdown requestを出す
	CShutdownRequester(p_(IActiveObject) actor){
		this->actor = actor;
	}
	virtual void ThreadProc() {
		sleep(10000);
		CDbg().Out("shutdown要求をだします");
		actor->shutdown();
	}
protected:
	p(IActiveObject) actor;
};

void Sample()
{
	p(IActiveObject) actor(CActiveObjectFactory::createActiveObject());
	CThreadManager::CreateThread(new CMakerClientThread("デジコ",actor));
	CThreadManager::CreateThread(new CMakerClientThread("シュガー",actor));
	CThreadManager::CreateThread(new CDisplayClientThread("ゲマゲマ",actor));
	CThreadManager::CreateThread(new CShutdownRequester(actor));
}

//	変なdefineしたのをはずしておこう
#undef p
#undef p_
#undef Object
#undef Object_

} // end of namespace test12

/////////////////////////////////////////////////////////////////

namespace test13 {

void Sample(){
	CDbg().Out("Thread making test -- start");
	CTimer t; t.Reset();
	for(int i=0;i<1000;++i){
		CThread th;
		th.CreateThread();
		th.StopThread();
	}
	DWORD d = t.Get();
	CDbg().Out("Thread making test -- end : time ="
		+ CStringScanner::NumToString(d));
	//	スレッド生成～消滅テスト。1個3msほどかかっとるな(Pentium4 2.53GHz)
	//	まあ、スレッド終了を待つ時間とかもあるしな..
}

} // end of namespace test13

/////////////////////////////////////////////////////////////////

//	function_callbackを用いて書き直した例
namespace test12a {

//	スマートポインタいちいち書くの面倒
#define p(T) shared_ptr<T>

// 引数いちいち書くの面倒
#define p_(T) const shared_ptr<T>&

//	Java風にしておこう
#define Object shared_ptr<void>
#define Object_ const shared_ptr<void>&

struct IResult {
//	futureパターンで返し値をいただくための基底クラス
	virtual Object getResultValue()=0;
};

struct CRealResult : public IResult {
//	返し値を保持するためのクラス
	CRealResult(Object_ resultValue){
		this->resultValue = resultValue;
	}
    Object getResultValue() {
        return resultValue;
    }
protected:
	Object resultValue;
};

struct CFutureResult : public IResult {
//	futureパターンでResultを返す
	CFutureResult():ready(false){}
    void setResult(p_(IResult) result) {
		synchronized g(&lock);
		this->result = result;
        this->ready = true;
		lock.notifyAll();
    }
	Object getResultValue() {
		synchronized g(&lock);
        while (!ready) {
            try {
                lock.wait();
            } catch (CInterruptedException&) {
            }
        }
        return result->getResultValue();
    }
protected:
	CLockObject lock;
	p(IResult) result;
    bool ready;
};

struct IActiveObject {
//	このクラスが、active object基底クラス
//	このクラスのメソッドは非同期に呼び出される
	virtual void displayString(const string&) = 0;
	//	要求１．

	virtual p(IResult) makeString(int count, char fillchar) = 0;
	//	要求２．結果は、futureパターンでもらう

	virtual void shutdown() =0;
	//	シャットダウンリクエスト(これが来たらactive objectを停止する)
};

struct CServant : public IActiveObject {
//	これは、アクティブオブジェクトの実装例
	virtual p(IResult) makeString(int count, char fillchar) {
		smart_ptr<char> buffer;
		buffer.AddArray(count+1);
		buffer[count] = '\0';
        for (int i = 0; i < count; i++) {
            buffer[i] = fillchar;
            try {
				IThread::getThread()->sleep(100);
            } catch (CInterruptedException&) {
            }
        }
		Object obj(new string(buffer.get()));
			//	stringをObject型にboxing

        return p(IResult)(new CRealResult(obj));
    }	
	virtual void displayString(const string&str) {
		try {
			CDbg().Out("displayString: " + str);
//			CThread()->getMyThread()->sleep(10);
        } catch (CInterruptedException &) {
        }
    }
	virtual void shutdown() {}
};

typedef function_callback IMethodRequest;

struct CActivationQueue {
//	スケジュールを保持するためのキュー
	enum { MAX_METHOD_REQUEST = 100 }; // 最大リクエスト数

	void putRequest(const shared_ptr<IMethodRequest>& request) {
		synchronized g(&lock);
        while (MAX_METHOD_REQUEST < requestQueue.size()) {
			//	これ以上queuing不可能なら仕方ないので待つ
            lock.wait();
        }
		requestQueue.push_back(request);
        lock.notifyAll();
    }
	p(IMethodRequest) takeRequest() {
		synchronized g(&lock);
        while (requestQueue.size() <= 0) {
			//	仕事がないなら仕方ないので待つ
            lock.wait();
        }
        p(IMethodRequest) request = *requestQueue.begin();
		requestQueue.pop_front();
        lock.notifyAll();
        return request;
    }
protected:
	CLockObject lock;
	list<p(IMethodRequest) >	requestQueue;
};

struct CSchedulerThread : public CThread {
//	スケジュールを保持しておき順番に実行するスレッド

	CSchedulerThread(p_(CActivationQueue) queue) {
        this->queue = queue;
    }
	
	//	スケジューラーに要求を積む
	void invoke(p_(IMethodRequest) request) {
        queue->putRequest(request);
    }

	virtual void ThreadProc() {
		try {
			while (true) {
				p(IMethodRequest) request = queue->takeRequest();
				// request->execute();
				request->run();
				//	キューに積まれている内容をひとつずつ実行するだけで良い
			}	//	終了するときはこのスレッドに対して割り込みがかかる
		} catch (CInterruptedException&){
			//	shutdownによってIThread::interruptが呼び出され、その結果
			//	このスレッドがCLockObject::waitしていればそこでinterrupt例外が
			//	発生する。（よってここに抜けてくる）
		}
		CDbg().Out("ActiveObjectを終了しました");
    }

	virtual void shutdown() {
		interrupt();
		//	このスレッドに対してCInterruptedExceptionを発生させる
	}
protected:
	p(CActivationQueue) queue;
};

struct CProxy : public IActiveObject {
    CProxy(p_(CSchedulerThread) scheduler,p_(CServant) servant) {
        this->scheduler = scheduler;
        this->servant = servant;
    }
	virtual p(IResult) makeString(int count, char fillchar) {
		p(CFutureResult) future(new CFutureResult());
        scheduler->invoke(p(IMethodRequest)
			(function_callback_v::Create(
				&CProxy::inner_makeString,
				servant,future,count,fillchar)
			)
		);
		//	スケジューラーにお願いして、futureパターンで
		//	引換券だけ渡して制御をすぐに戻す
		return smart_ptr_static_cast<IResult>(future);
    }
	//↑の関数のコールバックからのジャンプ台
	static void inner_makeString(p_(CServant) s,p(CFutureResult)& future
		,int count,char fillchar){
        p(IResult) result = s->makeString(count, fillchar);
        future->setResult(result);
	}

    virtual void displayString(const string& str) {
        scheduler->invoke(p(IMethodRequest)
			(function_callback_v::Create(
				&CProxy::inner_displayString,servant,str)
			)
		);
		//	スケジューラーにお願いしてすぐに制御を戻す
    }
	//	↑からのジャンプ台
	static void inner_displayString(p_(CServant) s,const string& str){
		s->displayString(str);
	}

	virtual void shutdown(){
		//	スケジューラにシャットダウン要請
		scheduler->shutdown();
	}
protected:
	p(CSchedulerThread) scheduler;
	p(CServant) servant;
};

struct CMakerClientThread : public CThread {
	CMakerClientThread(const string&name,p_(IActiveObject)actor)
		:name_(name),actor_(actor){}
	virtual void ThreadProc() {
		try { 
			for(int i=1;i<=5;++i){
				p(IResult) result = actor_->makeString(i,'C');
				//	戻り値つきのactive objectの呼び出し
				Object obj = result->getResultValue();
				const string& str = *smart_ptr_static_cast<string>(obj);
				//	unboxing
				//	(Object型に封入されたstring型オブジェクトを取り出す)
				//	string str = **(smart_ptr<string>*)(obj.get());
				//	でもいいのだがコピーコストがもったいないので

				CDbg().Out(name_ + "は" + str + "を作った");
				IThread::getThread()->sleep(200);
			}
		} catch(CInterruptedException&){
			//	sleep中に割り込み例外が発生した?
		}
	}
protected:
	string	name_;
	p(IActiveObject) actor_;
};

struct CDisplayClientThread : public CThread {
	CDisplayClientThread(const string&name,p_(IActiveObject)actor)
		:name_(name),actor_(actor){}
	virtual void ThreadProc() {
		try { 
			for(int i=1;i<=10;++i){
				string data = name_ + "は"
					+ CStringScanner::NumToString(i) + "を表示する";
				actor_->displayString(data);
				IThread::getThread()->sleep(200);
				//	このsleep中に割り込み例外が発生する(かも)
			}
		} catch(CInterruptedException&){
			//	sleep中に割り込み例外が発生した?
		}
	}
protected:
	string	name_;
	p(IActiveObject) actor_;
};

struct CActiveObjectFactory {
	static p(IActiveObject) createActiveObject() {
        p(CServant) servant(new CServant());
		//	active object
		
		p(CActivationQueue) queue(new CActivationQueue());
		// 要求queue
        
		p(CSchedulerThread) scheduler(new CSchedulerThread(queue));
		//	要求を順番に保持しておくためのスレッド

		//	proxyオブジェクトが、スケジューラ(とactive object)に橋渡しする
		p(IActiveObject) proxy(new CProxy(scheduler, servant));
		CThreadManager::CreateThread(
			smart_ptr_static_cast<IThread>(scheduler));
		//	↑shared_ptr<CSchedulerThread>をshared_ptr<IThread>にupcastする
        return proxy;
    }
};

struct CShutdownRequester : public CThread {
//	10秒後にshutdown requestを出す
	CShutdownRequester(p_(IActiveObject) actor){
		this->actor = actor;
	}
	virtual void ThreadProc() {
		sleep(10000);
		CDbg().Out("shutdown要求をだします");
		actor->shutdown();
	}
protected:
	p(IActiveObject) actor;
};

void Sample()
{
	p(IActiveObject) actor(CActiveObjectFactory::createActiveObject());
	CThreadManager::CreateThread(new CMakerClientThread("デジコ",actor));
	CThreadManager::CreateThread(new CMakerClientThread("シュガー",actor));
	CThreadManager::CreateThread(new CDisplayClientThread("ゲマゲマ",actor));
	CThreadManager::CreateThread(new CShutdownRequester(actor));
}

//	変なdefineしたのをはずしておこう
#undef p
#undef p_
#undef Object
#undef Object_

} // end of namespace test12a

/////////////////////////////////////////////////////////////////////////////

//	言わずと知れたWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	必ず書いてね

	test8::Sample();

//	test12a::Sample();
	//	↑呼び出すサンプルのナンバーを指定してチョ
	//	例)test3::Sample();

	return 0;
}
