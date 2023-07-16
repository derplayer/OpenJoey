// yaneTimer.h :
//
//	CTimer			:	Œo‰ßŠÔ‚ğƒJƒEƒ“ƒg‚·‚éƒ^ƒCƒ}
//
//		programmed by yaneurao(M.Isozaki) '99/07/25
//		modified by yaneurao '00/02/28-'00/03/13
//

#ifndef __yaneTime_h__
#define __yaneTime_h__


namespace yaneuraoGameSDK3rd {
namespace Timer {

class CTimeGetTimeWrapper {
/**
	‘‚¢˜b‚ªtimeGetTime‚Ìwrapper

	timeGetTime‚ªg‚¦‚È‚¢ó‹µ‚É’u‚¢‚Ä‚ÍA
	GetTickCount‚ğg‚¤‚æ‚¤‚Éİ’è‚·‚é

	‚±‚ÌƒRƒ“ƒXƒgƒ‰ƒNƒ^`ƒfƒXƒgƒ‰ƒNƒ^‚Å
	timeBeginPeriodMin‚ÆtimeEndPeriodMin‚ğ
	ŒÄ‚Ño‚µ‚Ä‚¢‚é
	
	‚±‚ÌƒIƒuƒWƒFƒNƒg‚Íref_creater‚Å—p‚¢‚éB
	(singleton‚Å‚à—Ç‚©‚Á‚½‚Ì‚¾‚ª..)
*/
public:

	DWORD	GetTime() {
		///	¶‚Ìƒ^ƒCƒ}‚Ìæ“¾
		if (m_bUseTGT) {
			return ::timeGetTime();
		} else {
			return ::GetTickCount();
		}
	}

	CTimeGetTimeWrapper();
	virtual ~CTimeGetTimeWrapper();

	static	ref_creater<CTimeGetTimeWrapper>*	GetRefObj()
		{ return & m_vTimeGetTime; }

protected:
	bool		m_bUseTGT;		//	timeGetTime‚ğg—p‚·‚é‚Ì‚©H
	int			m_nRef;			//	timeBeginPeriodMin`timeEndPeriodMin‚ÌQÆƒJƒEƒ“ƒg
	TIMECAPS	m_dwTimeCaps;	//	ƒ^ƒCƒ}[

	static	ref_creater<CTimeGetTimeWrapper>	m_vTimeGetTime;
};

//////////////////////////////////////////////////////////////////////////////

class ITimer {
public:
	virtual void	Reset()=0;			///	Œ»İ‚Ì‚ğ‚O‚É
	virtual DWORD	Get()=0;			///	Œ»İ‚Ì‚Ìæ“¾
	virtual void	Set(DWORD)=0;		///	Œ»İ‚Ì‚Ìİ’è
	virtual void	Pause()=0;			///	Pause‹@
	virtual void	Restart()=0;		///	Pause‰ğœ
	virtual ~ITimer() {}
};

class CTimer : public ITimer {
/**
	‘‚¢˜b‚ªA“Æ—§ƒ^ƒCƒ}B
	Reset()‚·‚é‚ÆAƒ^ƒCƒ}‚ªƒŠƒZƒbƒg‚³‚êA‚»‚êˆÈ~AGet()‚ğŒÄ‚Ño‚·‚ÆA
	‘O‰ñAReset()‚³‚ê‚½‚Æ‚«‚©‚ç‚ÌŒo‰ßŠÔi[ms]’PˆÊj‚ª•Ô‚Á‚Ä‚­‚é‚æ‚¤‚É‚È‚éB
*/
public:
	virtual void	Reset();			///	Œ»İ‚Ì‚ğ‚O‚É
	virtual DWORD	Get() ;				///	Œ»İ‚Ì‚Ìæ“¾
	virtual void	Set(DWORD);			///	Œ»İ‚Ì‚Ìİ’è
	virtual void	Pause();			///	Pause‹@”
	virtual void	Restart();			///	Pause‰ğœ

	CTimer();
	virtual ~CTimer();

protected:
	DWORD	m_dwOffsetTime;					//	ƒIƒtƒZƒbƒg’l
	DWORD	m_dwPauseTime;					//	Pause‚©‚¯‚½‚Æ‚«‚ÌTime
	int		m_bPaused;						//	pause’†‚©H

private:
	static	ref_creater<CTimeGetTimeWrapper>*	GetRefObj()
	{ return CTimeGetTimeWrapper::GetRefObj(); }
	static	CTimeGetTimeWrapper*	GetObj()
	{ return GetRefObj()->get(); }
};

//////////////////////////////////////////////////////////////////////////////

class CFixTimer : public ITimer {
/**
	ƒQ[ƒ€‚Å class CTimer ‚ğg‚¤ê‡A
	‚PƒtƒŒ[ƒ€‚ÌŠÔ‚ÍAŒÅ’è’l‚ª•Ô‚Á‚Ä‚«‚½‚Ù‚¤‚ª–]‚Ü‚µ‚¢B

	class CMouse ‚É‘Î‚·‚é class CMouseEx ‚Æ“¯‚¶ŠÖŒW‚Å‚ ‚éB
	FlushŠÖ”‚Ìà–¾‚ğ“Ç‚Ş‚±‚ÆB
*/
public:
	virtual void	Reset();			///	Œ»İ‚Ì‚ğ‚O‚É
	virtual DWORD	Get();				///	Œ»İ‚Ì‚Ìæ“¾
	virtual void	Set(DWORD);			///	Œ»İ‚Ì‚Ìİ’è
	virtual void	Pause();			///	Pause‹@”
	virtual void	Restart();			///	Pause‰ğœ

	virtual	void	Flush();
	/**
		‚ğXV‚·‚éB
		‚±‚ê‚ğ‚µ‚½uŠÔ‚Ì’l‚ÉŠî‚Ã‚¢‚ÄGet‚Å’l‚ª•Ô‚é‚æ‚¤‚É‚È‚éB
		ˆÈ~AÄ“x‚±‚ÌŠÖ”‚ğŒÄ‚Ño‚·‚Ü‚ÅAFlush‚Æ“¯‚¶’l‚ª•Ô‚é
		‚±‚ÌŠÖ”ˆÈŠO‚ÍAclass CTimer ‚Æ“¯‚¶
	*/

	CFixTimer();

protected:
	CTimer	m_vTimer;
	DWORD	m_dwTimeGetTime;				//	‘O‰ñFlush‚µ‚½ŠÔ

	CTimer* GetTimer() { return& m_vTimer; }
};

} // end of namespace Timer
} // end of namespace yaneuraoGameSDK3rd

#endif
