#include "stdafx.h"

#include "yaneSound.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

LRESULT ISound::ConvertToMealTime(LONG nPos,int&nHour,int&nMin,int&nSec,int&nMS){
	if (nPos<0) return -1;
	nMS = nPos % 1000;
	nPos /= 1000;
	nSec = nPos % 60;
	nPos /= 60;
	nMin = nPos % 60;
	nPos /= 60;
	nHour = nPos;
	return 0;
}

LRESULT ISound::ConvertFromMealTime(LONG&nPos,int nHour,int nMin,int nSec,int nMS){
	if ( nHour < 0 || nMin < 0 || nSec < 0 || nMS < 0 ) return -1;
	nPos = nMS;
	nPos += nSec * 1000;
	nPos += nMin * 60 * 1000;
	nPos += nHour * 24*60*1000;
	return 0;
}

}}

#include "yaneSoundFactory.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

//	static members..
ThreadLocal<smart_ptr<ISoundFactory> > CSound::pFactory_;

////////////////////////////////////////////////////////////

LRESULT CSound::Load(const string& strFileName){
	///	ª‚±‚Ìƒtƒ@ƒCƒ‹(wav,mp3,mid,ogg..)‚ğ“Ç‚İ‚Ş

	p_.Delete();

	smart_ptr<ISoundFactory> factory = GetFactory();
	if (factory.isNull()){
		//	factory‚ªİ’è‚³‚ê‚Ä‚È‚¢
#ifdef USE_EXCEPTION
		throw CRuntimeException("sound-factory‚ªİ’è‚³‚ê‚Ä‚¢‚È‚¢");
#else
		return 1;
#endif
	}

	p_ = factory->Create(strFileName);
	if (p_.isNull()) return 1;

	return 0;
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
