// DirectMusic Wrapper

#include "stdafx.h"
#include "../Auxiliary/yaneCOMBase.h"
#include "../AppFrame/yaneAppManager.h"
#include "../Multimedia/yaneDirectSound.h"
#include "../Multimedia/yaneDirectMusic.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

///////////////////////////////////////////////////////////////////////////

bool CDirectMusic::CanUseDirectMusic(){
	//	DirectMusic���g������Ȃ̂��ǂ����𒲂ׂĕԂ�
	static bool bFirst = true;
	static bool bUse   = false;

	if (bFirst){	//	�ŏ��̂P��̂ݒ��ׂĕԂ�
		bFirst = false;
		CCOMObject<IDirectMusicPerformance*> obj;
		bUse = obj.CreateInstance(
			CLSID_DirectMusicPerformance,IID_IDirectMusicPerformance)==0;
	}
	return bUse;
}

///////////////////////////////////////////////////////////////////////////

CDirectMusic::CDirectMusic(CDirectSound* p/*=NULL*/) :
	m_nStatus(0),m_lpDMusic(0)
{
	if (GetDirectMusicPerformance()->CreateInstance(
		CLSID_DirectMusicPerformance,IID_IDirectMusicPerformance)!=0){

		m_nStatus = 1;
		return ;
	}

	// DirectSound��Attach�����݂�
	if (p&&p->Get()){
		m_pDirectSound = p->Get();
	}

	//	�����DirectSound���珉��������
	//	m_lpDMusic = NULL;	//	���������Ă����Ȃ���bug��B
		//	�Ԃ��l�̂��߂ɓn���ϐ��̏�������v�����邾�Ȃ�āA
		//	�Ƃ�ł��Ȃ���>DirectMusic
	if (FAILED(GetDirectMusicPerformance()->get()->
			Init(&m_lpDMusic,GetDirectSound(),NULL))){
		m_nStatus = 2;
		m_lpDMusic = NULL;	//	NULL��ۏ؂���
		return ;
	}

	//	�������A�|�[�g�i�V���Z�T�C�U�j�̑I���͍s�Ȃ�
	if (FAILED(GetDirectMusicPerformance()->get()->AddPort(NULL))){
	//	���m_lpDMusic���󂯂Ă���ꍇ�́A
	//	�������������|�[�g�łȂ���΂Ȃ�Ȃ�
			//	�T�[�h�p�[�e�B���̃\�t�g�V���Z�̓f�B�t�H���g��
			//	�I������邱�Ƃ͖����̂ł��̏ꍇ�A���s����
			//	�����ǃh���}�C�i�΁j
	}

	if (GetDirectMusicLoader()->CreateInstance(
		CLSID_DirectMusicLoader,IID_IDirectMusicLoader)!=0){
		m_nStatus = 3;
	}

	//	�������Đ����s�Ȃ��̂ŃZ�O�����g�̃L���b�V���ݒ��؂�
	//	�����؂��Ă��o�O��悤���O�O�G
	GetDirectMusicLoader()->get()->EnableCache(GUID_DirectMusicAllTypes,false);
	GetDirectMusicLoader()->get()->EnableCache(CLSID_DirectMusicSegment,false);
}

CDirectMusic::~CDirectMusic(){
	GetDirectMusicLoader()->Release();	//	Loader���ɉ�̂��Ȃ���΂Ȃ�Ȃ�

	if (GetDirectMusicPerformance()!=NULL){
		GetDirectMusicPerformance()->get()->CloseDown();
		GetDirectMusicPerformance()->Release();
	}
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
