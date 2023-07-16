#include "stdafx.h"
#include "../Draw/yanePlane.h"
#include "../Draw/yaneFastDraw.h"
#include "../Draw/yaneFastPlane.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

CFastPlaneFactory::CFastPlaneFactory():
	m_vFastDraw(new CFastDraw)
{
	CPlane::SetFactory(smart_ptr<IPlaneFactory>(new CPlaneFactoryFastPlane(GetDraw())));
}

ISurface* CPlaneFactoryFastPlane::CreateInstance(){
	return new CFastPlane(pDraw_);
}

CPlane::CPlane(){
	CreateInstance(GetFactory());
}

CPlane::CPlane(const smart_ptr<IPlaneFactory>& pFactory){
	CreateInstance(pFactory);
}

void	CPlane::CreateInstance(const smart_ptr<IPlaneFactory>& pFactory)
{
	if (pFactory.isNull()){
#ifdef USE_EXCEPTION
		throw CRuntimeException("CPlane::CreateInstanceでfactoryがNULL");
#else
		return ;	//	これエラーとちゃうんか．．
#endif
	}
	p_.Add(pFactory->CreateInstance());
}

CPlane::CPlane(const smart_ptr<ISurface>& surface)
	: p_(surface){}

CPlane::CPlane(ISurface* surface)
	: p_(surface){}

ThreadLocal<smart_ptr<IPlaneFactory> > CPlane::pFactory_;

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
