#include "stdafx.h"

#include "yaneFPSTimer.h"
#include "yaneTimer.h"

namespace yaneuraoGameSDK3rd {
namespace Timer {

CFPSTimer::CFPSTimer() {
	SetFPS(60); // ƒfƒBƒtƒHƒ‹ƒg’l‚Ìİ’è

	// timeGetTime‚ÌüŠú‚ğã‚°‚é‚©Hi‚m‚s‚Å‚Í1ms‚ª•ÛØ‚³‚ê‚Ä‚¢‚È‚¢j
	// ‚Ü‚ AÅˆ«A‰e‹¿‚Ì—L‚è‚»‚¤‚È‚Ì‚ÍADrawScene‚¾‚¯‚È‚ñ‚¾‚¯‚Ç...
}

CFPSTimer::~CFPSTimer() {
	//	ƒfƒXƒgƒ‰ƒNƒ^‚Å‚Í“Á‚Éˆ—‚Í‚µ‚È‚¢
}

void CFPSTimer::SetFPS(DWORD fps){ // FPS’l
	ResetTimer();
	ResetElapseTimeCounter();

	m_dwCallCount = 0;
	m_nUnuseSleep = 0;
	m_nDrawCount  = 0;

	m_dwFPS = fps;
	if (fps==0) {	// non-wait mode
		return ;
	}
	
	// ‚PƒtƒŒ[ƒ€‚²‚Æ‚É‰½ms‘Ò‚Â•K—v‚ª‚ ‚é‚Ì‚©H~0x10000[ms]
	m_dwFPSWait = (1000*0x10000)/m_dwFPS;
	
	// ‚±‚¢‚Â‚ÍAdwFPSWait‚Ì¬”ˆÈ‰º‚ğ16ƒrƒbƒg‚Ì¸“x‚Å•Û‚·‚é‚½‚ß‚É‚ ‚é
	m_dwFPSWaitTT = 0; //@¡‰ñ‚ÌŠÔ‚Íƒ[ƒ(cf.DrawFlip)

}

DWORD CFPSTimer::GetFPS() {
	return m_dwFPS;
}

void CFPSTimer::ResetTimer(){
	m_dwLastDraw = GetTimer()->Get(); // ‘O‰ñ•`‰æŠÔ‚ÍA‚±‚±‚Åİ’è
	m_bFrameSkip = false;
	m_dwFrameSkipCounter = 0;
	m_dwFrameSkipCounterN = 0;
}

void CFPSTimer::ResetElapseTimeCounter(){	 // FPSTimeƒJƒEƒ“ƒ^‚ÌƒŠƒZƒbƒg
	m_dwElapsedTime = 0;
}

DWORD CFPSTimer::GetElapseTimeCounter(){	 // FPSTimeƒJƒEƒ“ƒ^‚Ìæ“¾
	return m_dwElapsedTime;
}

void CFPSTimer::ResetCallCounter(){	  // ŒÄo‚µƒJƒEƒ“ƒ^‚ÌƒŠƒZƒbƒg
	m_dwCallCount = 0;
}

DWORD CFPSTimer::GetCallCounter(){	 // ŒÄo‚µƒJƒEƒ“ƒ^‚Ìæ“¾
	return m_dwCallCount;
}
// ---------------------------------------------------------------------------

void CFPSTimer::WaitFrame(){	  // (C)yaneurao 1998-1999
	DWORD t = GetTimer()->Get(); // Œ»İ

	//	ƒXƒLƒbƒvƒŒ[ƒgƒJƒEƒ“ƒ^
	if (m_dwFPS!=0 && ((m_dwCallCount % m_dwFPS) == 0)) {
		m_dwFrameSkipCounter = m_dwFrameSkipCounterN;
		m_dwFrameSkipCounterN = 0;
	}

	m_dwDrawTime[m_nDrawCount & 31] = t;  // Draw‚µ‚½ŠÔ‚ğ‹L˜^‚·‚é‚±‚Æ‚ÅFPS‚ğZo‚·‚éè•‚¯‚É‚·‚é
	if (++m_nDrawCount == 64) m_nDrawCount = 32;
	// 8‚É–ß‚·‚±‚Æ‚É‚æ‚Á‚ÄA0`15‚È‚çA‚Ü‚¾16ƒtƒŒ[ƒ€‚Ì•`‰æ‚ªI‚í‚Á‚Ä‚¢‚È‚¢‚½‚ßA
	// FPS‚ÌZo‚ªo—ˆ‚È‚¢‚±‚Æ‚ğ’m‚é‚±‚Æ‚ªo—ˆ‚éB

	m_dwCallCount++; // ‚±‚¢‚Â‚ğFPS‘ª’è‚Ég‚¤‚±‚Æ‚ªo—ˆ‚éB

	// ‚©‚È‚èŒµl‚©‚Â³Šm‚©‚ÂŒø—¦—Ç‚­ŠÔ‘Ò‚¿‚ğ‚·‚é‚Í‚¸B
	if (m_dwFPS == 0) {
		m_dwElapseTime[m_nDrawCount & 31] = 0;
		return ; // Non-wait mode
	}

	m_dwFPSWaitTT = (m_dwFPSWaitTT & 0xffff) + m_dwFPSWait; // ¡‰ñ‚Ì‘Ò‚¿ŠÔ‚ğŒvZ
	// m_dwFPSWait‚ÍA‘Ò‚¿ŠÔ‚Ì¬”ˆÈ‰º‚ğ‚P‚Uƒrƒbƒg‚Ì¸“x‚Å‚Á‚Ä‚¢‚é‚Æl‚¦‚æ
	// ‚±‚ê‚É‚æ‚èAdoubleŒ^‚ğ‚¿o‚·•K—v‚ª‚È‚­‚È‚éB

	DWORD dwWait = m_dwFPSWaitTT >> 16; // Œ‹‹Ç‚Ì‚Æ‚±‚ëA¡‰ñ‚Í‰½ms‘Ò‚Â‚Ë‚ñH

	// ‚PƒtƒŒ[ƒ€ŠÔ‚ğŒo‰ß‚µ‚¿‚å‚éB‚½‚¾‚¿‚É•`‰æ‚µ‚È‚¿‚á‚¢I
	DWORD dwElp = (DWORD)(t - m_dwLastDraw); // ‘O‰ñ•`‰æ‚©‚ç‚¢‚­‚çŒo‰ß‚µ‚Æ‚ñ‚Ë‚ñH
	if (dwElp>=dwWait) { // ‰ß‚¬‚Ä‚é‚â‚ñI‰ß‚¬‚Ä‚é•ªA250ms‚Ü‚Å‚â‚Á‚½‚çŸ‰ñ‚É‚¿‰z‚·‚ÅI
		DWORD dwDelay = dwElp-dwWait;

		//	250ˆÈã’x‚ê‚Ä‚¢‚½‚çAƒtƒŒ[ƒ€ƒXƒLƒbƒv‚µ‚È‚¢i‰Šú‰»‚Ì‚½‚ßj
		//	‚»‚µ‚ÄA’x‚êŠÔ‚Í0‚Æ‚µ‚Äˆµ‚¤
		if (dwDelay >= 250) {
			dwDelay = 0;
		}

		//	‚QƒtƒŒ[ƒ€‚Ì•`‰æŠÔˆÈã‚È‚ç‚ÎŸƒtƒŒ[ƒ€‚ğƒXƒLƒbƒv‚·‚é
		m_bFrameSkip =	(dwDelay >= dwWait*3);
		if (m_bFrameSkip) m_dwFrameSkipCounterN++;

		if (dwDelay < 250) { t -= dwDelay; } else { t -= 250; }
		// ¡‰ñ‚Ì•`‰æ‚ğ‹U‚é‚±‚Æ‚ÅAŸ‰ñ‚Ì•`‰æŠJn‚ğ‘‚ß‚é

		m_dwLastDraw = t;
		m_dwElapseTime[m_nDrawCount & 31] = 0;
		return ;
	}

	// ‚Ù‚ÈAŠÔ‚ğ’×‚·‚Æ‚·‚Á‚©I

	m_dwElapsedTime += dwElp; // ŠÔ‘Ò‚¿‚µ‚½•ª‚Æ‚µ‚ÄŒvã
	m_dwElapseTime[m_nDrawCount & 31] = dwElp;

	m_bFrameSkip = false;	//	Ÿ‚ÍƒtƒŒ[ƒ€ƒXƒLƒbƒv‚µ‚È‚¢

/*
	// ‚Ü‚¾ŠÔ‚Í‚½‚Á‚Õ‚è‚ ‚é‚Ì‚©H
	// 4msˆÈãÁ”ï‚·‚é•K—v‚ª‚ ‚é‚Ì‚È‚ç‚ÎASleep‚·‚é
	// ‚¢‚ÜdwWait>dwElp‚È‚Ì‚ÅdwWait-dwElp>=0‚Æl‚¦‚Ä—Ç‚¢
	if (dwWait-dwElp >= 4) {
		if (m_nUnuseSleep) {
			m_nUnuseSleep--;
		} else {
			::Sleep(dwWait-dwElp-3);
		}
	}

	// 95/98/NT‚Å‘ª’è‚µ‚½‚Æ‚±‚ëSleep(1);‚Å1ms’PˆÊ‚ÅƒXƒŠ[ƒv‚·‚é‚Ì‚Í‰Â”\
	// ‚½‚¾‚µAÀ‘•ŒnˆË‘¶‚Ì‰Â”\«‚à‚ ‚é‚Ì‚ÅSleep‚Ì¸“x‚Í3msˆÈ“à‚Æ‰¼’è	

	if ((CTimeBase::timeGetTime()-m_dwLastDraw)>=dwWait) {
	// ‚â‚Á‚×[IIQ‰ß‚²‚µ‚Æ‚é‚â‚ñ‚¯IIiÎj
		m_nUnuseSleep = 60; // 60ƒtƒŒ[ƒ€ŠÔSleepg‚¤‚ñ‚Í‚â‚ß...
		// ‚µ‚©‚µA‚È‚ñ‚¿‚ã[_Œo¿‚ÈƒvƒƒOƒ‰ƒ€‚È‚ñ‚â...
		m_dwLastDraw += dwWait; // Q‰ß‚²‚µ‚Ä‚à[‚½‚¯‚Ç–³‹‚â:p
		return ;
	}

	while ((CTimeBase::timeGetTime()-m_dwLastDraw)<dwWait) ;
	// ƒ‹[ƒv‚ÅŠÔ‚ğ’×‚·i‚ ‚Ü‚èD‚«‚¶‚á‚È‚¢‚¯‚Çj
*/
	//	Ë@‚Æ‚¢‚¤‚©A‘¼‚ÌƒXƒŒƒbƒh‚ªASleep‚âƒƒbƒZ[ƒWˆ—‚ğs‚È‚í‚È‚¢
	//	ê‡A‚»‚ÌƒXƒŒƒbƒh‚©‚ç‹­§“I‚ÉŸ‚ÌƒXƒŒƒbƒh‚ÉØ‚è‘Ö‚¦‚ç‚ê‚é‚Ì‚Í
	//	–ñ20msŒã‚Å‚ ‚é‚©‚ç‚µ‚Ä..‚»‚Ì‚Ö‚ñ‚ğl—¶‚·‚é‚Æ...
	//	í‚ÉƒXƒŠ[ƒvg‚¤À‘•‚Å‚à‚¢‚¢‚æ‚¤‚È‹C‚ª‚·‚é...
	::Sleep(dwWait-dwElp);


	// ‚±‚ê‚ÅAŠÔ‚Â‚Ô‚µŠ®—¹I

	m_dwLastDraw += dwWait; // ‚Ò‚Á‚½‚è‚Å•`‰æ‚ªŠ®—¹‚µ‚½‰¼’è‚·‚éBi’[”‚ğ‚¿‚Ü‚È‚¢‚½‚ßj
}

bool	CFPSTimer::ToBeSkip() const {
	return m_bFrameSkip;
}

DWORD CFPSTimer::GetSkipFrame() const {
	return m_dwFrameSkipCounter;
}

DWORD CFPSTimer::GetRealFPS() {	//	FPS‚Ìæ“¾i‘ª’è’lj

	if (m_nDrawCount < 16) return 0; // ‚Ü‚¾16ƒtƒŒ[ƒ€Œv‘ª‚µ‚Ä‚¢‚È‚¢
	if (m_nDrawCount < 32) {
		DWORD t = m_dwDrawTime[(m_nDrawCount-1)]	// ‘O‰ñŠÔ
			-	m_dwDrawTime[(m_nDrawCount-16)];	// 15‰ñ‘O‚ÌŠÔ
		if (t==0) {
			return 0;	//	‘
		}	
		return (1000*15+t/2)/t;
		// •½‹Ï‚©‚çZo‚µ‚Ä’l‚ğ•Ô‚·i’[”‚ÍlÌŒÜ“ü‚·‚éj
	}
		DWORD t = m_dwDrawTime[(m_nDrawCount-1) & 31] // ‘O‰ñŠÔ
			-	m_dwDrawTime[m_nDrawCount & 31];	 // 31‰ñ‘O‚ÌŠÔ
		if (t==0) {
			return 0;	//	‘ª
		}	
		return (1000*31+t/2)/t;
}

DWORD CFPSTimer::GetCPUPower() {	 //	 CPU Power‚Ìæ“¾i‘ª’è’lj

	if (m_nDrawCount < 16) return 0; // ‚Ü‚¾16ƒtƒŒ[ƒ€Œv‘ª‚µ‚Ä‚¢‚È‚¢
	DWORD t=0;
	for(int i=0;i<16;i++) 
		t += m_dwElapseTime[i]; // ‚±‚±16ƒtƒŒ[ƒ€“à‚ÅFPS‚µ‚½ŠÔ
	// return 1-t/(1000*16/m_dwFPS)[%] ; // FPSƒmƒ‹ƒ}‚©‚çZo‚µ‚Ä’l‚ğ•Ô‚·
	return 100-(t*m_dwFPS/160);
}

} // end of namespace Timer
} // end of namespace yaneuraoGameSDK3rd
