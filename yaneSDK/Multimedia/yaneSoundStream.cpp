// Sound Stream Management

#include "stdafx.h"
#include "../Auxiliary/yaneFile.h"
#include "../Auxiliary/yaneStringMap.h"
#include "../AppFrame/yaneObjectCreater.h"

// Stream Family
#include "yaneWaveStream.h"
#include "yaneACMStream.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

// CSoundStreamManager
CSoundStreamFactory::CSoundStreamFactory()
{
	// plugin map ÇÃç\íz
	m_vPlugInMap.Add(new CStringMap);
}

CSoundStreamFactory::~CSoundStreamFactory()
{
}

smart_ptr<ISoundStream> CSoundStreamFactory::Open(const string& filename)
{
	// Ç‹Ç∏ägí£éqÇìæÇÈ
	string fullname = CFile::MakeFullName(filename);
	string ext = CFile::GetSuffixOf(fullname);
	CFile::ToLower(ext);

	// ägí£éqÇ…ÇÊÇ¡Çƒê∂ê¨Ç∑ÇÈ
	smart_ptr<ISoundStream> p;
	if (ext=="wav") {	// .wav
		p.Add(new CWaveStream);
		if (p->Open(fullname.c_str())==0) return p;
	}

	// mapÇ…ï∑Ç¢ÇƒÇ›ÇÊÇ©
	IStringMap::mymap::const_iterator it = GetPlugInMap()->GetMap()->begin();
	while (it!=GetPlugInMap()->GetMap()->end()){
		string registerd_ext = it->first;  CFile::ToLower(registerd_ext);
		if (registerd_ext==ext){
			p.Add((ISoundStream*)CObjectCreater::GetObj()->Create(it->second));
			if (!p.isNull() && p->Open(fullname.c_str())==0) return p;
		}
		it++;
	}

	// ACMÇ…ï∑Ç¢ÇƒÇ›ÇÊÇ©
	{
		p.Add(new CACMStream);
		if (p->Open(fullname.c_str())==0) return p;
	}

	// ñ≥Ç¢ÇÀ
	p.Add(new CNullSoundStream);

	return p;
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
