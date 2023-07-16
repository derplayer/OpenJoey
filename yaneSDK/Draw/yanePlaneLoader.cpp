#include "stdafx.h"
#include "yanePlaneLoader.h"

//////////////////////////////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Draw {

CPlaneLoader::CPlaneLoader()
{
	SetColorKey(ISurface::makeRGB(0,255,0));
	SetFactory(CPlane::GetFactory());	//	default��factory��ݒ�
}

/*smart_ptr<ISurface>*/
CPlane CPlaneLoader::GetPlane(int nNo){		///	�v���[���̎擾

	Load(nNo);

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0){
		//	���ꂪ������Ȃ����Ƃ͖����͂�����..
		return CPlane(new CSurfaceNullDevice);
	}

	smart_ptr<ISurface> p(
		smart_ptr_static_cast<ISurface>(smart_ptr_static_cast<CLoadCacheInfo>(obj)->pObj)
	);
	if (IsRGBColorKey()){
		p->SetColorKey(GetColorKey());
	} else {
		p->SetColorKeyPos(m_nColorKeyPosX,m_nColorKeyPosY);
	}
	return CPlane(p);
}

///	nNo�̃t�@�C�������擾
string	CPlaneLoader::GetFileName(int nNo){

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0){
		//	���ꂪ������Ȃ����Ƃ͖����͂�����..
		return "";
	}

	return (smart_ptr_static_cast<CLoadCacheInfo>(obj))->strFileName;
}

//////////////////////////////////////////////////////////////////////////////

LRESULT		CPlaneLoader::InnerLoad(const smart_obj& obj){

	CLoadCacheInfo& info = *smart_ptr_static_cast<CLoadCacheInfo>(obj);

	string strFileName(info.strFileName);

	info.pObj.Add(GetFactory()->CreateInstance());
	LRESULT lr = smart_ptr_static_cast<ISurface>(info.pObj)->Load(strFileName);

	//	�G���[���[�h�Ȃ�΁ANullDevice��Ԃ�
	if (lr!=0){
		info.pObj.Add(new CSurfaceNullDevice);
	}

	return lr;
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
