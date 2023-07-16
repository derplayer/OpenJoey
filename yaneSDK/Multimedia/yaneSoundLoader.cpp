#include "stdafx.h"
#include "yaneSoundFactory.h"

#include "yaneSound.h"
//	CSoundFactory���K�v�Ƃ���
#include "../Multimedia/yaneSoundFactory.h"
#include "../Multimedia/yaneDirectMusic.h"
#include "../Multimedia/yaneDirectSound.h"
#include "../Multimedia/yaneWaveOutput.h"
#include "yaneSoundLoader.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

//////////////////////////////////////////////////////////////////////////////

smart_ptr<ISound>	CSoundLoader::GetSound(int nNo){

	Load(nNo);

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0){
		//	���ꂪ������Ȃ����Ƃ͖����͂�����..
		return smart_ptr<ISound>(new CSoundNullDevice);
	}

	//	CLoadCacheInfo
	return smart_ptr_static_cast<ISound>(
		(smart_ptr_static_cast<CLoadCacheInfo>(obj))->pObj);
}

///	nNo�̃t�@�C�������擾
string	CSoundLoader::GetFileName(int nNo ){

	smart_obj obj;
	if (GetIDObjectManager()->getObject(nNo,obj)!=0){
		//	���ꂪ������Ȃ����Ƃ͖����͂�����..
		return "";
	}

	return (smart_ptr_static_cast<CLoadCacheInfo>(obj))->strFileName;
}

//////////////////////////////////////////////////////////////////////////////

LRESULT		CSoundLoader::InnerLoad(const smart_obj& obj){
	if (m_sf.isNull()) return -1;	//	factory���ݒ肳��Ă��Ȃ�

	CLoadCacheInfo& info = *(smart_ptr_static_cast<CLoadCacheInfo>(obj)).get();

	string strFileName(info.strFileName);

	smart_ptr<ISound> p;
	if (!m_bEnforceStreamPlay){
		p = m_sf->Create(strFileName);
	} else {
		p = m_sf->Create(strFileName,m_bStreamPlay);
	}

	info.pObj = smart_ptr_static_cast<void>(p);
	//	Create����΁A�ǂݍ��݂Ɏ��s�����NullDevice���Ԃ�

	if (p->GetType() == 0) {
		//	CSoundNullDevice?
		return 1; // �ǂݍ��݂Ɏ��s
	}

	//	InnerLoad�ŃT�[�t�F�[�X��ǂݍ��ނƖ����I��
	//	������鏈�����o���Ȃ��Ȃ��Ă���..

	LRESULT lr = p->Open(strFileName);
/*
	//	�G���[���[�h�Ȃ�΁ANullDevice��Ԃ�
	//	(�������Ă����Ύ�����InnerLoad���Ăяo����Ȃ���..)
	if (lr!=0){
		(*(smart_ptr<ISound>*)info.pObj.get())
			= smart_ptr<ISound>(new CSoundNullDevice);
	}
*/
	return lr;
	return 0;
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
