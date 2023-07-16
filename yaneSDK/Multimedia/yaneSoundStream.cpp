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
	// plugin map の構築
	m_vPlugInMap.Add(new CStringMap);
}

CSoundStreamFactory::~CSoundStreamFactory()
{
}

smart_ptr<ISoundStream> CSoundStreamFactory::Open(const string& filename)
{
	// まず拡張子を得る
	string fullname = CFile::MakeFullName(filename);
	string ext = CFile::GetSuffixOf(fullname);
	CFile::ToLower(ext);

	// 拡張子によって生成する
	smart_ptr<ISoundStream> p;
	if (ext=="wav") {	// .wav
		p.Add(new CWaveStream);
		if (p->Open(fullname.c_str())==0) return p;
	}

	// mapに聞いてみよか
	IStringMap::mymap::const_iterator it = GetPlugInMap()->GetMap()->begin();
	while (it!=GetPlugInMap()->GetMap()->end()){
		string registerd_ext = it->first;  CFile::ToLower(registerd_ext);
		if (registerd_ext==ext){
			p.Add((ISoundStream*)CObjectCreater::GetObj()->Create(it->second));
			if (!p.isNull() && p->Open(fullname.c_str())==0) return p;
		}
		it++;
	}

	// ACMに聞いてみよか
	{
		p.Add(new CACMStream);
		if (p->Open(fullname.c_str())==0) return p;
	}

	// 無いね
	p.Add(new CNullSoundStream);

	return p;
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
