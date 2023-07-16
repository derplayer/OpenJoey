#include "stdafx.h"
#include "yaneSoundStream.h"
#include "yaneSoundParameter.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

CSoundParameter::CSoundParameter()
{
	// デフォルトで持たない
	m_bGlobalFocus		= false;
	m_bPanControl		= false;
	m_bFrequencyControl	= false;
	// デフォルトでCSoundStreamFactoru
	m_pStreamFactory.Add(new CSoundStreamFactory);
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
