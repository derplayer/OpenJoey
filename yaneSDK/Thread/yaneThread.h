//	yaneThread.h :
//		programmed by yaneurao	'00/02/26
//		“¯ŠúƒIƒuƒWƒFƒNƒg‚Ì“±“ü '01/11/19

#ifndef __yaneThread_h__
#define __yaneThread_h__


#include "../Thread/yaneEvent.h"
#include "../Thread/yaneLockObject.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

class IThread {
public:
	virtual LRESULT CreateThread()=0;
	virtual LRESULT CreateThread(const smart_ptr<function_callback>& fn)=0;
	virtual void SetThreadFunction(const smart_ptr<function_callback>& fn)=0;
	virtual LRESULT StopThread()=0;
	virtual LRESULT JumpToThread()=0;
	virtual bool IsThreadExecute()const=0;
	virtual bool IsThreadValid()const=0;
	virtual void InvalidateThread()=0;
	virtual DWORD	GetThreadId() const=0;

	//	javaŒİŠ·ƒXƒŒƒbƒh
	virtual LRESULT	wait(int nTimeOut=-1)
		#ifdef USE_EXCEPTION
			throw(CInterruptedException)
		#endif
			=0;
	virtual	LRESULT sleep(int nTime)
		#ifdef USE_EXCEPTION
			throw(CInterruptedException)
		#endif
			=0;
	virtual void	notify()=0;
	virtual void	notifyAll()=0;
	virtual void	interrupt()=0;
	virtual bool	isInterrupted() const=0;
	virtual bool	interrupted()=0;

	virtual ~IThread(){}

	virtual CEvent* GetExceptionEvent()=0;
	//	—áŠO”­¶‚Ésignaló‘Ô‚É‚È‚éƒCƒxƒ“ƒg

	//	‚»‚ÌƒXƒŒƒbƒh‚É‘Î‰‚·‚éƒXƒŒƒbƒhƒNƒ‰ƒX‚ğ“¾‚é
	//	(NULL‚Å‚ ‚é‚±‚Æ‚à‚ ‚è‚¤‚é)
	static IThread* getThread() { return pThread; }
	static void setThread(IThread*p) { pThread = p; }
		//	ª‚±‚ê‚ÍCThread‚ªCreateThread‚µ‚½‚Æ‚«‚ÉŸè‚É‚â‚è‚æ‚é
	static void unsetThread() { pThread.Clear(); }

protected:
	static ThreadLocal<IThread*> pThread;
};

}}

//	‘¼‚Ìnamespace‚É‚ ‚é‚à‚Ì‚ğ‘O•ûéŒ¾‚·‚é
namespace yaneuraoGameSDK3rd { namespace AppFrame { class IAppBase; }}

namespace yaneuraoGameSDK3rd {
namespace Thread {

class CThread : public IThread {
/**
	ƒXƒŒƒbƒh‚ğ¶¬iŠÇ—j‚µ‚Ü‚·B

	ƒXƒŒƒbƒh¶¬‚ÉŠÖ‚µ‚Ä‚ÍA
	class CThreadManager ‚Ì‚Ù‚¤‚àQÆ‚µ‚Ä‚­‚¾‚³‚¢B

	//	ƒXƒŒƒbƒhƒNƒ‰ƒX‚©‚ç”h¶‚³‚¹‚éê‡‚ÍA
	//	•K‚¸”h¶ƒNƒ‰ƒX‘¤‚ÅStopThread‚ğs‚È‚¤•K—v‚ª‚ ‚é‚Ì‚Å’ˆÓ
	(¨‚â‚Ë–{‚Q‚ğQÆ‚Ì‚±‚Æ)
*/
public:
	virtual void ThreadProc() {}
	/**
		ƒ[ƒJ[ƒXƒŒƒbƒh‚Ìˆ—‚ğ‹Lq‚·‚é‚½‚ß‚ÉA
		‚±‚ÌŠÖ”‚ğƒI[ƒo[ƒ‰ƒCƒh‚µ‚Ü‚·B
		i‚±‚ÌƒNƒ‰ƒX‚ğ”h¶‚³‚¹‚Äg‚¤‚Æ‚«j
	*/

	virtual LRESULT CreateThread();
	virtual LRESULT CreateThread(const smart_ptr<function_callback>& fn);
	virtual void SetThreadFunction(const smart_ptr<function_callback>& fn);
	/**
		CreateThread‚ÍAƒXƒŒƒbƒh‚ğì¬‚µAThreadProc‚ğÀs‚µ‚Í‚¶‚ß‚Ü‚·B
		CreateThread‚Ìˆø”‚Æ‚µ‚ÄAƒR[ƒ‹ƒoƒbƒN‚·‚éŠÖ”‚ğw’è‚µ‚È‚¢ê‡‚ÍA
		ThreadProc‚ªŒÄ‚Ño‚³‚ê‚é‚Ì‚ÅThreadProc‚ğƒI[ƒo[ƒ‰ƒCƒh‚µ‚Ä‚¨‚­‚©A
		SetThreadFunction‚ÅAƒXƒŒƒbƒhŠÖ”‚Æ‚µ‚ÄƒR[ƒ‹ƒoƒbƒN‚·‚éŠÖ”‚ğ
		İ’è‚µ‚Ä‚¨‚­‚±‚ÆB
	*/

	virtual LRESULT StopThread();
	/**
		ƒXƒŒƒbƒh‚ÌƒXƒgƒbƒvB’â~‚·‚é‚Ü‚Å‘Ò‚¿‚Ü‚·
		•Ô‚µ’lF
			‚OF³íI—¹
			‚PF‚·‚Å‚ÉƒXƒŒƒbƒh‚ª‘¶İ‚µ‚Ä‚¢‚È‚©‚Á‚½i³íI—¹j
	*/

	virtual LRESULT JumpToThread();	///	ƒXƒŒƒbƒh‚ğì‚ç‚¸‚ÉƒWƒƒƒ“ƒv

	virtual bool IsThreadExecute()const; /// ƒXƒŒƒbƒh‚ÍÀs’†‚©H

	virtual bool IsThreadValid()const;
	/**
		ƒXƒŒƒbƒh‚ÌStop‚Í‚©‚©‚Á‚Ä‚¢‚È‚¢‚©
		‚±‚ÌŠÖ”‚ªfalse‚É‚È‚Á‚½‚Æ‚«‚ÉAThreadProc‚©‚ç’Eo‚·‚é‚æ‚¤‚É
		ƒR[ƒfƒBƒ“ƒO‚µ‚Ü‚·B
	*/

	virtual void InvalidateThread();
	/**
		ƒXƒŒƒbƒh‚É’â~M†‚ğ‘—‚é
		i‚»‚Ì’â~‚ğ‘Ò‚Â‚í‚¯‚Å‚Í‚È‚¢j
		ƒ[ƒJ[ƒXƒŒƒbƒh‚ÍŸ‚ÉIsThreadValid‚ğƒ`ƒFƒbƒN‚µ‚½‚Æ‚«‚É
		false‚ª•Ô‚Á‚Ä‚«‚Ü‚·‚Ì‚ÅA‚»‚Ì‚Æ‚«‚ÉThreadProc‚©‚ç”²‚¯o‚é‚Å‚µ‚å‚¤B
		‚ ‚é‚¢‚ÍThreadSleep‚µ‚½‚Æ‚«‚É”ñ‚O‚ª•Ô‚é‚Ì‚ÅA‚»‚±‚ÅƒXƒŒƒbƒh‚ğ
		”²‚¯‚éƒR[ƒh‚ğ‘‚¢‚Ä‚à—Ç‚¢‚Å‚·B
	*/

	/// ----- Java‚ÌƒXƒŒƒbƒh‹@”\‚ğ‚¢‚­‚Â‚©À‘•
	/**
		‚»‚ê‚¼‚ê‚Ìà–¾‚É‚Â‚¢‚ÄÚ‚µ‚­‚Í class CLockObject ‚Ì“¯–¼‚ÌŠÖ”‚Æ
		Java‚ÌThread‚Ì“¯–¼‚ÌŠÖ”‚ğQÆ‚Ì‚±‚ÆB
	*/

	virtual LRESULT	wait(int nTimeOut=-1)
		#ifdef USE_EXCEPTION
			throw(CInterruptedException)
		#endif
			;
	///		wait ƒLƒ…[‚É•À‚Ô

	virtual void	notify();
	/// wait‚Å‘Ò‚Á‚Ä‚¢‚éƒXƒŒƒbƒh‚ğ’@‚«‹N‚±‚·(1‚Â‚Ì‚İ)

	virtual void	notifyAll();
	/// wait‚Å‘Ò‚Á‚Ä‚¢‚é‚·‚×‚Ä‚ÌƒXƒŒƒbƒh‚ğ’@‚«‹N‚±‚·

	virtual void	interrupt();
	/**
		Š„‚è‚İ‚ğ‚©‚¯‚éB‚±‚ÌŠÖ”‚ÍA‘¼ƒXƒŒƒbƒh‚©‚çŒÄ‚Ño‚·B
		‚»‚¤‚·‚é‚ÆA‚±‚ÌƒXƒŒƒbƒh‚ÍAŸ‚Ìwait‚©sleep‚Å
			CInterruptedException—áŠO‚ğ”­¶‚³‚¹‚éB
	*/

	virtual bool	isInterrupted() const;
	///	Š„‚è‚İó‘Ô‚É‚È‚Á‚Ä‚¢‚é‚©

	virtual bool	interrupted();
	///	Š„‚è‚İó‘Ô‚É‚È‚Á‚Ä‚¢‚é‚©‚ğ•Ô‚·‚ªA‚»‚ÌŒãA
	///	Š„‚è‚İó‘Ô‚ÍƒŠƒZƒbƒg‚·‚éB

	virtual	LRESULT sleep(int nTime)
		#ifdef USE_EXCEPTION
			throw(CInterruptedException)
		#endif
			;
	/**
		nTime [ms]‘Ò‚ÂB
	•Ô‚µ’lF
		0F³íI—¹@
		1 : ‘Ò‚Á‚Ä‚¢‚éÅ’†‚ÉA•ÊƒXƒŒƒbƒh‚©‚çStopThread‚ªŒÄ‚Ño‚³‚ê‚ê‚Î
		“¯ŠúƒIƒuƒWƒFƒNƒg‚ªƒVƒOƒiƒ‹ó‘ÔiƒXƒŒƒbƒh‚ğI—¹‚³‚¹‚È‚³‚¢A
		‚Æ‚¢‚¤ó‘Ôj‚É‚È‚èA‚½‚¾‚¿‚ÉŠÖ”‚ğ”²‚¯‚éB‚»‚Ìê‡A”ñ0‚ª•Ô‚éB
		‘¼ƒXƒŒƒbƒh‚©‚çA‚±‚ÌƒXƒŒƒbƒh‚Ìinterruptƒƒ\ƒbƒh‚ªŒÄ‚Ño‚³‚ê‚½
		ê‡Asleep‚Å‚Ì‘Ò‹@‚ğ‘¦À‚ÉI—¹‚µACInterruptedException—áŠO‚ğ
		”­¶‚³‚¹‚éB

	’Ê—áAƒ[ƒJ[ƒXƒŒƒbƒh‚ÍA
@@while ( IsThreadValid( ) ) {
@@@	ˆ— ;
@@@	Sleep( 20 );
@@} 
	‚Æ‚¢‚¤‚æ‚¤‚ÈƒR[ƒfƒBƒ“ƒO‚É‚æ‚Á‚ÄA‰½‚©‚Ìˆ—‚ğs‚È‚¤ˆ—‚ğ‘‚«‚Ü‚·‚ªA
	‚±‚Ì‚Æ‚«‚ÉSleep(20)‚ª‚ ‚é‚½‚ßAƒXƒŒƒbƒh‚ªInvalidate‚³‚ê‚ÄiI—¹‚Ì‡}j
	‚©‚çA‚±‚Ì•ª‚¾‚¯ƒŒƒXƒ|ƒ“ƒX‚ª’á‰º‚µ‚Ü‚·B‚»‚±‚ÅA‚±‚±‚ğSleep‚Å‚Í‚È‚­A
	if (thread.sleep( 20 )) break; ‚È‚Ç‚Æ‚·‚ê‚ÎA‚±‚Ì•ª‚ÌƒŒƒXƒ|ƒ“ƒX‚ğ
	Œüã‚³‚¹‚é‚±‚Æ‚ªo—ˆ‚Ü‚·Bthread.sleep‚ÌÀ‘•‚ÍA“¯ŠúƒIƒuƒWƒFƒNƒg‚ğ‘Ò‚Â
	(::WaitForSingleObject)‚æ‚¤‚É‚È‚Á‚Ä‚¢‚é‚Ì‚ÅA‚±‚Ì•”•ª‚É‚¨‚¢‚ÄCPUƒpƒ[‚ğA
	‚ ‚Ü‚èg—p‚¹‚¸AŒø—¦“I‚ÉSleep‚·‚é‚±‚Æ‚ªo—ˆ‚Ü‚·B
	*/

	/// ----- property

	virtual DWORD	GetThreadId() const { return m_dwRunningThreadID; }
	/**
		‹N“®‚µ‚Ä‚¢‚éƒXƒŒƒbƒh‚ÌAGetCurrentThreadId‚Å•Ô‚³‚ê‚é’l‚ğ•Ô‚·B
		ƒXƒŒƒbƒh‚ª‹N“®‚µ‚Ä‚¢‚È‚¢ê‡‚ÍA-1‚ª•Ô‚é
	*/
	virtual HANDLE	GetThreadHandle() const { return m_dwThreadHandle; }
	/**
		‹N“®‚µ‚Ä‚¢‚éƒXƒŒƒbƒh‚Ìƒnƒ“ƒhƒ‹‚ğ•Ô‚·
	*/

	//	ƒXƒŒƒbƒh‚ÌƒXƒ^ƒbƒNƒTƒCƒY‚Ìw’è
	void	SetStackSize(size_t size) { m_nStackSize = size; }
	size_t	GetStackSize() const { return m_nStackSize; }

	CThread();
	virtual ~CThread();

protected:
	volatile HANDLE	m_dwThreadHandle;		//	ƒXƒŒƒbƒhƒnƒ“ƒhƒ‹
	volatile DWORD	m_dwRunningThreadID;	//	ƒXƒŒƒbƒh‚ÌID(ã‹L‚Æ“¯‚¶‚¾‚Æv‚¤‚ªOSˆË‘¶‚Ì‰Â”\«‚à‚ ‚é‚Ì‚Åˆê‰•Êˆµ‚¢‚É‚·‚é)

//	static void ThreadCallBack(LPVOID lpVoid);
	static unsigned __stdcall ThreadCallBack( void * lpVoid);

	void	InitEvent();
	//	ˆÈ‰º‚Ì‚Q‚Â‚ÌƒXƒŒƒbƒh‚ÌƒXƒe[ƒ^ƒX‚ğ‰Šú‰»‚·‚é

	CEvent	m_vExistEvent;
	//	‚±‚ê‚ÍAƒXƒŒƒbƒh‚ª¶‚«‚Ä‚¢‚éŠÔ‚ÍAƒmƒ“ƒVƒOƒiƒ‹ó‘Ô
	//	ƒVƒOƒiƒ‹ó‘Ô‚É‚È‚Á‚½‚Æ‚«‚ÍAƒXƒŒƒbƒh‚ğI—¹‚³‚¹‚È‚³‚¢‚Æ‚¢‚¤‡}
	CEvent* GetExistEvent() { return &m_vExistEvent; }
	//	ËƒVƒOƒiƒ‹ó‘Ô‚É‚È‚ê‚ÎAIsThreadValid‚ªfalse‚ğ•Ô‚·

	CEvent m_vThreadExecute;
	///	ƒXƒŒƒbƒh‚ª¶¬‚³‚êAƒXƒŒƒbƒhƒR[ƒ‹ƒoƒbƒNŠÖ”‚ğŠJn‚µ‚½‚©H
	CEvent* GetThreadExecute() { return &m_vThreadExecute; }
	//	ªÀs’†‚È‚ç‚ÎƒVƒOƒiƒ‹ó‘Ô‚É‚È‚é

	//	sleep‚·‚é‚Æ‚«‚È‚Ç‚É‘Ò‚Â‚×‚«—áŠOƒCƒxƒ“ƒg
	CEvent	m_vExceptionEvent;
	virtual CEvent* GetExceptionEvent() { return &m_vExceptionEvent; }

	//	thread‚ªstart‚·‚é‚Ü‚Å‚Ìó‘Ô
	CEvent	m_vThreadStarting;
	CEvent*	GetThreadStarting() { return &m_vThreadStarting; }
	//	ªŠJn‚·‚é‚Ü‚Å‚Ì‘Ò‚¿ó‘Ôƒmƒ“ƒVƒOƒiƒ‹ó‘Ô

	//	Create‚ÆStop“®ì‚ğ”r‘¼‚·‚é‚½‚ß‚ÌƒNƒŠƒeƒBƒJƒ‹ƒZƒNƒVƒ‡ƒ“
	CCriticalSection m_vCriticalSection;
	CCriticalSection* GetCriticalSection() { return& m_vCriticalSection;}

	smart_ptr<function_callback> m_fn;
	smart_ptr<function_callback> GetCallBack() const { return m_fn; }

	CLockObject	m_vLockObject;
	/*volatile*/ CLockObject* GetLockObject() { return &m_vLockObject;}
	//	‚±‚êƒXƒŒƒbƒh‚Ì‚½‚ß‚ÌLock Object

	bool	m_bCreatedThread;
	/*
		CreateThread‚Å¶¬‚³‚ê‚½ƒXƒŒƒbƒh‚È‚Ì‚©‚ğ¦‚·ƒtƒ‰ƒO
		JumpThread‚Å¶¬‚³‚ê‚½ƒXƒŒƒbƒh‚ÍAI—¹‚ÉWaitForSingleObject‚µ‚Ä‚Í
		‚¢‚¯‚È‚¢‚Ì‚ÅAƒtƒ‰ƒO‚ğ—§‚Ä‚Ä‚¨‚­
	*/

	size_t	m_nStackSize;
};

class IThreadManager {
public:
	///	ƒXƒŒƒbƒh‚Ì’Ç‰Á‚Æíœ
	virtual void	AddThread(IThread* pThread)=0;
	virtual void	DelThread(IThread* pThread)=0;
	virtual bool	StopAllThread()=0;
	virtual void	WaitAllThreadEnded()=0;
	virtual ~IThreadManager(){}
};

class CThreadManager : public IThreadManager {
/**
	Thread‚ğ“Š‡‚·‚éThreadManager
	‚±‚ê‚Íå‚Éclass CThread ‚©‚ç“à•”“I‚ÉŒÄ‚Ño‚³‚ê‚é

	ƒ†[ƒU[‚ªg‚¤‚Æ‚µ‚½‚çArunƒƒ\ƒbƒh‚®‚ç‚¢
	‚±‚ê‚ÍA
		CThreadManager::RunThread(new CThreadXXX);
	‚Æ‚¢‚¤‚æ‚¤‚Ég‚¤B‚ ‚Æ‚ÍAŠ—LŒ ‚Í‚±‚ÌƒXƒŒƒbƒhƒNƒ‰ƒX‚ª
	–Ê“|‚ğŒ©‚Ä‚­‚ê‚éB
	
*/
public:
	static	CThreadManager* GetObj() { return m_obj.get(); }
	///	ª‚±‚¢‚Â‚ÅAsingleton‚È“z‚ğ“¾‚é

	///	ƒXƒŒƒbƒh‚Ì’Ç‰Á‚Æíœ
	virtual void	AddThread(IThread* pThread);
	virtual void	DelThread(IThread* pThread);

	///	‚·‚×‚Ä‚ÌƒXƒŒƒbƒh‚ğI—¹‚³‚¹‚éiI—¹‚ğ‘Ò‚Âj
	virtual bool	StopAllThread();
	/*
		‚·‚Å‚É‘¼ƒXƒŒƒbƒh‚ªStopAllThread‚ğŒÄ‚Ño‚µAI—¹ˆ—’†‚Å
		‚ ‚ê‚ÎAbalk‚µ‚ÄA‚»‚Ì‚Æ‚«‚Ífasle‚ª•Ô‚é
	*/

	///	‚·‚×‚Ä‚ÌƒXƒŒƒbƒh‚ªI—¹‚·‚é‚Ì‚ğ‘Ò‚ÂiI—¹‚³‚¹‚é‚í‚¯‚Å‚Í‚È‚¢j
	virtual void	WaitAllThreadEnded();

	/**
		ƒXƒŒƒbƒh‚ğ•Û‚·‚éBŠ—LŒ ‚ÍA‚±‚ÌƒNƒ‰ƒX‚ª–Ê“|‚ğŒ©‚é
		ƒXƒŒƒbƒh‚ğ‘–‚ç‚¹‚é‘O‚ÉA‚±‚¢‚Â‚ÉŠ—LŒ ‚ğ‚½‚¹‚Ä‚¨‚¯‚ÎA
		‚»‚ÌƒXƒŒƒbƒh‚ªI—¹‚·‚é‚Æ‚«‚ÉƒCƒ“ƒXƒ^ƒ“ƒX‚ª©“®“I‚Éíœ‚³‚ê‚é
	*/
	static void	RegistThread(IThread* pThread)
		{ GetObj()->RegistThread(shared_ptr<IThread>(pThread)); }
//	static void RegistThread(const smart_ptr<IThread>& pThread)
//		{ GetObj()->_RegistThread(pThread); }
	static void RegistThread(const shared_ptr<IThread>& pThread)
		{ GetObj()->_RegistThread(pThread); }

	/**
		RegistThread‚Æ“¯‚¶‚­Š—LŒ ‚Ü‚Å‚±‚ÌƒNƒ‰ƒX‚ª–Ê“|‚ğŒ©‚é‚ªA‚³‚ç‚É
		IAppBase‚ÌRun‚ğŒÄ‚Ño‚·‚±‚Æ‚ğ•ÛØ‚·‚é
	*/
	static LRESULT RunThread(IAppBase*pAppBase);
//	static LRESULT RunThread(const smart_ptr<IAppBase>& pAppBase);
	static LRESULT RunThread(const shared_ptr<IAppBase>& pAppBase);

	/**	
		RegistThread‚Æ“¯‚¶‚­Š—LŒ ‚Ü‚Å‚±‚ÌƒNƒ‰ƒX‚ª–Ê“|‚ğŒ©‚é‚ªA‚³‚ç‚É
		IThread‚ÌCreateThread‚ğŒÄ‚Ño‚·‚±‚Æ‚ğ•ÛØ‚·‚é
	*/
	static LRESULT CreateThread(IThread*pThread);
//	static LRESULT CreateThread(const smart_ptr<IThread>& pThread);
	static LRESULT CreateThread(const shared_ptr<IThread>& pThread);

protected:
	//	ªƒWƒƒƒ“ƒv‘ä‚É‚µ‚Ä«‚ÉƒWƒƒƒ“ƒv‚·‚é
	virtual void _RegistThread(
		const shared_ptr<IThread>& pThread,
			//	‚±‚ÌƒXƒŒƒbƒh‚ª’â~‚·‚é‚Æ‚«‚ÉA
		const shared_obj& obj=shared_obj()
			//	‚±‚ÌƒIƒuƒWƒFƒNƒg‚ğíœ‚·‚é
		//	ª‚±‚ê“n‚·‚Æ‚«‚ÍAQÆƒJƒEƒ“ƒg‚ª‹¶‚í‚È‚¢‚æ‚¤‚É
		//	smart_ptr::inc_ref‚µ‚Ä‚©‚çA‚»‚Ìƒ|ƒCƒ“ƒ^‚ğ“n‚µ‚Ä‚â‚é•K—v‚ ‚è
	);
public:

	CThreadManager():m_bStopAllThread(false) {}

	typedef vector<IThread*> threadlist; 
protected:

	/**
		ŠÇ—‚µ‚Ä‚¢‚éƒXƒŒƒbƒhˆê——
	*/

	threadlist m_apThreadList;
	threadlist* GetThreadList() { return& m_apThreadList; }
		//	‚±‚¢‚Â‚ç–{“–‚Ívolatile‚È‚Ì‚¾‚ªAVC++‚ÍA
		//	ƒƒ“ƒo•Ï”‚Ö‚ÌƒAƒNƒZƒX‚ÍƒŒƒWƒXƒ^Š„•t‚µ‚È‚¢

	/**
		‚±‚ê‚ÍŠ—L‚µ‚Ä‚¢‚éƒXƒŒƒbƒhˆê——
	*/
	struct CThreadList {
		shared_ptr<IThread> pThread;	//	‚±‚ÌƒXƒŒƒbƒh‚ª’â~‚·‚é‚Æ‚«‚ÉA
		shared_obj obj;		//	‚±‚ÌƒIƒuƒWƒFƒNƒg‚ğíœ‚·‚é
		bool	bValid;					//	‚±‚Ìª‚Q‚Â‚Ìƒf[ƒ^ƒƒ“ƒo‚ª“K³‚©‚ğ¦‚·
			//	•ÊƒXƒŒƒbƒh‚©‚çíœ‚µ‚È‚¢‚ÆA©•ª‚Å©•ª‚ÌƒXƒŒƒbƒh‚ğíœ‚·‚é‚±‚Æ‚Ío—ˆ‚È‚¢‚Ì‚ÅDD
		CThreadList():bValid(true){}
	};
	smart_list_ptr<CThreadList> m_apMyThreadList;
	smart_list_ptr<CThreadList>* GetMyThreadList()
		{ return& m_apMyThreadList; }
	void	GarbageThread();
	//	deleteƒ}[ƒN(CThreadList::bValid==false)‚Ì‚Â‚¢‚Ä‚¢‚é
	//	ƒXƒŒƒbƒh‚ğíœ‚·‚é(ThreadList‚É‘Î‚·‚éCriticalLock‚ÍA
	//	Š|‚¯‚½ó‘Ô‚ÅŒÄ‚Ño‚·‚±‚Æ)

	CLockObject m_lockobject;
	CLockObject* GetLockObject() { return &m_lockobject; }

	/////////////////////////////////////////////////////////////
	///	‘SƒXƒŒƒbƒh‚Ì’â~‚Ì‚ß‚Ì§Œä‹
	volatile bool	m_bStopAllThread;
	CCriticalSection m_csStopAllThread;
	CCriticalSection* GetStopAllThreadLock() { return &m_csStopAllThread;}
	/**
		‘SƒXƒŒƒbƒh‚Ì’â~Š©‚ªo‚Ä‚¢‚é‚©‚Ç‚¤‚©‚Ìƒtƒ‰ƒO‚Æ
		‚»‚ê‚É‘Î‚·‚éƒNƒŠƒeƒBƒJƒ‹ƒZƒNƒVƒ‡ƒ“
		(latch)
	*/
	bool	isStopAllThread() {
	///	‚±‚Ìƒƒ\ƒbƒh‚ÍAˆê“xŒÄ‚Ño‚·‚ÆAˆÈ~‚Ítrue‚ğ•Ô‚·
		CCriticalLock cl(GetStopAllThreadLock());
		bool b = m_bStopAllThread;
		m_bStopAllThread = true;
		return b;
	}

	/////////////////////////////////////////////////////////////

	///	singletonƒpƒ^[ƒ“
	static	singleton<CThreadManager> m_obj;
};

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd

#endif
