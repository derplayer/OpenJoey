//	yaneEvent.h :
//		programmed by yaneurao	'02/03/05

#ifndef __yaneEvent_h__
#define __yaneEvent_h__


namespace yaneuraoGameSDK3rd {
namespace Thread {

class IEvent {
public:
	virtual	void	SetEvent()=0;
	virtual void	ResetEvent()=0;
	virtual LRESULT	Wait(int nTime=-1)=0;
	virtual HANDLE	GetHandle() const =0;
	virtual bool	IsSignal() const =0;

	virtual ~IEvent(){}
};

class CEvent : public IEvent {
/**
	WIN32のイベントのwrapper

	何かのイベントが発生したことをあるスレッドから
	別のスレッドに通知できるようにする同期オブジェクトです。

	イベントは、スレッドがいつその作業を行うかを
	知る必要があるときに有用です。

	このクラスは、ディフォルトでResetEventされた状態だと考えてください。

*/
public:
	virtual	void	SetEvent();
	///	オブジェクトをシグナル状態（利用できる状態）にする

	virtual void	ResetEvent();
	///	オブジェクトをノンシグナル状態（利用できない状態）にする

	virtual LRESULT	Wait(int nTime=-1);
	/**
		オブジェクトがシグナル状態（利用できる状態）になるまで待つ
		nTime時間だけ待つ　　nTime==-1 ならば無限に待つ

		返し値	0 : オブジェクトがシグナル状態になった
				1 : 待ち時間タイムアウトになった
				2 : それ以外（ありえないはず）
	*/

	virtual HANDLE	GetHandle() const { return m_hHandle; }
	/**
		HANDLEを取得する
		通例取得する必要は無いのだが、
		::WaitForMultipleObject
		等で待つときにハンドルが必要になる

		ここで取得したハンドルに対して、
		直接::SetEvent,::ResetEventを呼び出して
		状態を変更した場合は、このクラスのIsSignalは以前の値を返す
	*/

	virtual bool	IsSignal() const;
	/**
		シグナル状態(SetEventされた状態)なのか、
		ノンシグナル状態(ResetEventされた状態)なのかを返す
	*/

	CEvent();
	virtual ~CEvent();

protected:
	HANDLE	m_hHandle;
	volatile bool	m_bSignal;
	//	イベントに対してsignal状態なのか否かを判定するAPIが
	//	存在しない．．．（どういうこっちゃねん）

	CCriticalSection* GetCriticalSection() { return &m_cs; }
	CCriticalSection m_cs;
};

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd

#endif
