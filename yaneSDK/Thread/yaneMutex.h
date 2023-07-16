// yaneMutex.h :
//
//	programmed by M.Isozaki '99/06/19
//	毎回ハンドルを取得するので遅いでやんす:p

#ifndef __yaneMutex_h__
#define __yaneMutex_h__


namespace yaneuraoGameSDK3rd {
namespace Thread {

class IMutex {
public:
	virtual LRESULT Open(const string& strMutexName)=0;
	virtual void Wait()=0;
	virtual void Release()=0;
	virtual ~IMutex(){}
};

class CMutex : public IMutex {
/**
	Mutexクラスです。ＯＳレベルで１つしか存在しない相互排他を目的とします。
	２重起動の防止などに使います。
	２重起動防止は、class CSingleApp も参照すること。

	他スレッドとの相互排他ならば class CCriticalSection を用いること。
	そちらのほうが処理は断然軽いです。

*/
public:
	virtual LRESULT Open(const string& strMutexName);
	/**
	　strMutexNameは、ミューテックス名（任意につけて良い）

	　返し値は
	　0 : 他に同名のものは存在しない→所有権獲得成功！
	　1 : 他に同名のものが存在する。(error)⇒このあとWaitで待つと良い
	  2 : 既存のセマフォ、ミューテックス、待機可能なタイマ、ジョブ、
		ファイルマッピングオブジェクトのいずれかの名前と一致したのでエラー
	*/

	virtual void Wait();
	/**
		Openで１が返ってきているときに呼び出して、
		所有権が獲得できるのを待つ。
	*/
	virtual void Release();
	/**
		Mutexを解放する
		デストラクタも自動的にこのReleaseを呼び出すので、
		すぐに解体するならば、明示的に呼び出す必要は無い。
	*/

	CMutex();
	virtual ~CMutex();
protected:
	HANDLE	m_hMutex; // Mutex ハンドル 
};

class IMutexSection {
public:
	virtual LRESULT	Enter(const string& strMutexName)=0;
	virtual void	Leave()=0;
	virtual ~IMutexSection(){}
};

class CMutexSection : public IMutexSection {
/**
		MutexによるLock～unlockです
		class CCriticalSectionと同じ感覚で使えます
*/
public:
	virtual LRESULT	Enter(const string& strMutexName);	///	MutexSectionに入る
	/**
		返し値：
　　　　　０：成功
		　１：既存のセマフォ、ミューテックス、待機可能なタイマ、ジョブ、
		ファイルマッピングオブジェクトのいずれかの名前と一致したのでエラー
	*/

	virtual void	Leave();					///	MutexSectionから出る

	/**
		デストラクタではCMutexを解放するので、そのときに
		自動的にMutexは解放される。
	*/

protected:
	CMutex m_vMutex;
};

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd

#endif
