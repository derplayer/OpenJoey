//	yaneCriticalSection.h :
//		programmed by yaneurao	'00/03/02

#ifndef __yaneCriticalSection_h__
#define __yaneCriticalSection_h__


namespace yaneuraoGameSDK3rd {
namespace Thread {

class ICriticalSection {
public:
	virtual void	Enter()=0;
	virtual void	Leave()=0;
	virtual bool	IsLockedByThisThread() const =0;
	virtual ~ICriticalSection(){}
};

class CCriticalSection : public ICriticalSection {
/**
　スレッド間の排他を制御します。
　あるスレッドでEnter関数を呼び出したあと、Leave関数を呼び出すまでの間は、
　クリティカルセクションとなります。
　クリティカルセクションを、同時に２つ以上のスレッドが実行することは出来ません。
　同一スレッドからならば、多重にEnterすることが出来ます。
　（詳しくは、マルチスレッド関係の文献を参照すること）

  class CMutex に比較して、非常に高速なのが特徴です。

  注意：
  　CCriticalSectionの実体は、グローバルスコープに置くか、
  　staticなメンバ変数にするかしてください。
*/
public:
	virtual void	Enter();	///	CriticalSectionに入る
	virtual void	Leave();	///	CriticalSectionから出る

	virtual bool	IsLockedByThisThread() const;
	/**
		このスレッドによってLockされていればtrue
	*/

	CCriticalSection();
	virtual ~CCriticalSection();
protected:
	CRITICAL_SECTION	m_csCriticalSection;
	DWORD				m_dwLockedThread;	//	LockしているThreadId(0:非Lock)
	int					m_nLockCount;		//	Lockされている回数
			//	(同一スレッドならば複数回Lockできるので)
};

class CCriticalLock {
/**
	クリティカルセクションの使用補助クラス
		CCriticalSectionでEnterに入ったときに例外が発生すると
		その関数から抜けてしまうので、Leaveされなくて困ることがある。
	そこで、
		{
			CCriticalLock guard(GetCriticalSection());
			//	ここにその処理を書く
		}
	というように使えば、例外が発生したとしても、このクラスの
	デストラクタで、CCriticalSection::Leaveが呼び出されることが
	保証されます。

	また、Leave/Enterで、明示的にLeave/Enterすることができる。

		例１．
		{
			CCriticalLock guard(GetCriticalSection());
			guard.Leave();	//	CriticalSectionから抜ける
			//	このままスコープアウトした場合、CCriticalSection::Leaveは
			//	もう呼び出されない
		}

		例２．
		{
			CCriticalLock guard(GetCriticalSection());
			guard.Enter();	//	CriticalSectionに二重に入る
			//	このままスコープアウトした場合、CCriticalSection::Leaveは
			//	２度呼び出される
		}

		※　Enter/Leaveは、このCCriticalLockのインスタンスを生成した
			スレッド以外のスレッドが呼び出すことは想定していない。


			//	synchronizedクラス(→yaneLockObject.h)も参照のこと
*/
public:
	CCriticalLock(ICriticalSection* cs);

	///	明示的にCriticalSectionから抜ける
	void	Leave();

	///	明示的にCriticalSectionに入る
	void	Enter();

	~CCriticalLock();

protected:
	ICriticalSection* m_cs;
	int m_nLockCount;	//	Lockされている回数
	//	このCCriticalLockのデストラクタでは、Lock(Enter)された回数だけUnlock(Leave)することを保証する
};

namespace ThreadingModels
{

//	何もしないダミーのロック機構を提供する
class SingleThread
{
protected:
	struct Lock {
		Lock(const SingleThread&) {};
	};
};

//	オブジェクト単位のロック機構を提供する
class MultiThread
{
protected:
	struct Lock {
		Lock(const MultiThread& o)
			: lock_object( &(const_cast<MultiThread&>(o).cs) ) {};
	private:
		CCriticalLock lock_object;
	};
private:
	friend struct Lock;
	CCriticalSection cs;
};

} // end of namespace LockPolicies
} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd

#endif
