/**
	Javaのobject lock/synchronizedをサポートするためのクラス

		programmed by M.Isozaki(yaneurao) '02/07/15

*/
#ifndef __yaneLockObject_h__
#define __yaneLockObject_h__


#include "../Thread/yaneEvent.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

class CLockObject {
/**
	javaのobject lock/synchonizedをサポートするためのクラス

	実際の使い方についてはJavaのThread関係の本を読むか、
	class CThreadManager の実装を参考にすべし。
*/
public:

	LRESULT	wait(int nTimeOut=-1);
	/**
		wait キューに並ぶ
		１．ただし、呼び出す前にはcritical lockを獲得していなければ
			ならない。すなわち、GetCriticalSection()->Enter()
			を呼び出してある状態でなければならない。

			もしロックを持っていなければ
				CIllegalMonitorStateException
			例外が発生。

		２．並ぶ前にスレッドは、
			いったん自分のcritical lockを解除する。
		３．待ちは、他スレッドから
			notify / notifyAllが呼び出されるまで続く
			CThreadに対して、interruptが呼び出されたときも
			待ちから抜ける　⇒　CThread::interrupt参照）
		４．抜ける前に、再度自分のcritical lockを行なう

		５．nTimeは、そのタイムアウト時間を指定できる。[ms]単位。
			-1ならばendless（notify / notifyAllされるまで待つ）

		返し値：
			0 : 他スレッドによってnotify / notifyAllされた
			1 : タイムアウトになった
		//	2 : ありえないはずだが、CEventが返す可能性あり
			(例：メモリ不足など)
	*/

	void	notify();
	/// waitで待っているスレッドを叩き起こす(1つのみ)

	void	notifyAll();
	/// waitで待っているすべてのスレッドを叩き起こす

	///////////////////////////////////////////////////////////////////
	//	おまけ

	void	eraseThreadInWaitList(DWORD dwThreadID);
	/**
		待ちスレッドリストのなかから、指定されたスレッドと同じ
		スレッドIDを持つものを消去する
		（この呼び出しに先行してロックを獲得していなければならない）
			獲得していない場合はCIllegalMonitorStateExceptionが発生

		ふつうは、呼び出すことは無い（はず）
	*/

	void	notifyThreadInWaitList(DWORD dwThreadID);
	/**
		待ちスレッドリストのなかから、指定されたすレッドと同じ
		スレッドIDを持つものに対して、SetEventを行なう
	*/

public:
	ICriticalSection* GetCriticalSection() { return& m_cs;}
	/**
		このオブジェクトに対するcritical lock
		例）
			CLockObject o;
			CCriticalLock cl(o.GetCriticalSection());
			{
				なんやかや
			}
		とすれば、「なんやかや」の部分の処理はatomic（他スレッドに
		邪魔されない）になる
	*/

	struct CWaitState {
	/**
		waitで待っているスレッドを表現するための構造体
	*/
		void	SetThreadId(DWORD dwThreadID) { m_dwThreadID = dwThreadID; }
		DWORD	GetThreadId() const { return m_dwThreadID; }
		/**
			その待っているスレッドを設定／取得する
			ここで設定される値は、uniqueであれば別に::GetThreadIdで取得
			出来る値でなくとも良い。
		*/

		IEvent* GetEvent() { return &m_vEvent; }
		///	待ちのためのイベント

		CWaitState() : m_bInterrupt(false),m_dwThreadID((DWORD)-1) {}
	private:
		CEvent	m_vEvent;	//	待っているイベントリスト
		bool	m_bInterrupt;
		DWORD	m_dwThreadID;
	};

	smart_vector_ptr<CWaitState>* GetWaitThreadList()
		{ return &m_WaitThreadList; }

protected:
	CCriticalSection m_cs;
	smart_vector_ptr<CWaitState> m_WaitThreadList;
	//	↑これを扱うときは、CriticalLockを必ずかけるので、
	//	volatileである必要は無い

};

struct synchronized {
/**
	CLockObjectに対してjavaのsynchronizedを実現する
	使いかた

	void test(){
		synchronized g(&lock);
		//	このメソッドは擬似的にsynchronizedになる
	}
	CLockObject lock;
*/
	synchronized(CLockObject*lock) {
		cs_ = lock->GetCriticalSection();
		cs_->Enter();
	}
	synchronized(CLockObject& lock) {
		cs_ = lock.GetCriticalSection();
		cs_->Enter();
	}	
	synchronized(ICriticalSection*cs) {
		cs_ = cs;
		cs_->Enter();
	}
	synchronized(CCriticalSection& cs) {
		cs_ = &cs;
		cs_->Enter();
	}

	///	CriticalSectionのenter～leaveを明示的に行なう用のメソッド
	void lock(){ cs_->Enter(); }
	void unlock() { cs_->Leave(); }

	~synchronized(){
		cs_->Leave();
	}

protected:
	ICriticalSection* cs_;
};

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd

#endif
