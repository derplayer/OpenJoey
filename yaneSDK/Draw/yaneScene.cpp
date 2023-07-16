#include "stdafx.h"
#include "yaneScene.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

void	CSceneControl::CreateScene(int nScene){

	if (nScene==-1) return ; // ���ꎸ�s�Ȃ�
	//	�V�����V�[�����\�z
	CSceneInfo* pInfo = new CSceneInfo;
	pInfo->m_lpScene = GetSceneFactory()->CreateScene(nScene);
	pInfo->m_nScene = nScene;
	if (!pInfo->m_lpScene.isNull()){
		pInfo->m_lpScene->SetSceneControl(smart_ptr<ISceneControl>(this,false));
		pInfo->m_lpScene->OnInit();	//	����ŏ������B
	}
	//	�X�^�b�N�ɒǉ�
	GetSceneStack()->push_back(smart_ptr<CSceneInfo>(pInfo));
}

void	CSceneControl::OnMove(const smart_ptr<ISurface>& lpDrawContext) {// �ǉ�
	//	OnInit�̂Ȃ��Ŏ��X�ƃV�[���W�����v������̂Ő�Ƀ��b�Z�[�W���N���A
	do {
	//	�C���f���g����̖ʓ|�Ȃ̂ł����ŁO�O�G

	int nMessage = m_nMessage;
	m_nMessage = 0;

	//	���b�Z�[�W��dispatch
	switch (nMessage) {
		//	0:No Message 1:JumpScene 2:CallScene
		//	3:CallSceneFast 4:ReturnScene 5:ExitScene
	case 0: break;
	case 1:
		//	���݂̃V�[��������Ȃ�΁A�����j��
		if (!IsEnd()){
			GetSceneStack()->pop_back();
		}
		//	�V�����V�[�����\�z
		CreateScene(m_nNextScene);
		break;
	case 2: {
		//	���݂̃V�[����j��
		if (!IsEnd()){
			(*GetSceneStack()->rbegin())->m_lpScene.Delete();
		}
		//	�V�����V�[�����\�z
		CreateScene(m_nNextScene);
		break;
			}
	case 3:
		//	���݂̃V�[�����X�^�b�N��ɐς�ŁADelete���Ȃ��B
		//	�˃X�}�[�g�|�C���^�ɂȂ��Ă���̂�push���Ă�����Delete����Ȃ��B
		//	�V�����V�[�����\�z
		CreateScene(m_nNextScene);
		break;
	case 4:	{		//	���̃V�[���ɖ߂�B
		//	���݂̃V�[����j��
		if (IsEnd()){
#ifdef USE_EXCEPTION
			throw CSyntaxException("����ȏ�ReturnScene�o���Ȃ�");
#endif
		} else {
			GetSceneStack()->pop_back();
		}
		if (IsEnd()) break; //	�����߂�V�[�������˂�

		CSceneInfo& pInfo = **GetSceneStack()->rbegin();
		int nScene = pInfo.m_nScene;
		if (pInfo.m_lpScene.isNull()) {
			//	�V�[��������Ă��邩��Afactory�ŁA�܂��������ɂ�I
			GetSceneStack()->pop_back();
			CreateScene(nScene);
			//	�ꉞ�A�Ăяo�����O�O�G
			if (!(*GetSceneStack()->rbegin())->m_lpScene.isNull()) {
				(*GetSceneStack()->rbegin())->m_lpScene->OnComeBack(nScene);
			}
		} else {
			//	�A���Ă����Ł`�@�W�F�[���J���o�[�N�b�O�O�G
			(*GetSceneStack()->rbegin())->m_lpScene->OnComeBack(nScene);
		}
		break;
			}
	case 5:	return ;	//	�`�悹���ɔ�����
	}

	if (!IsEnd() && !(*GetSceneStack()->rbegin())->m_lpScene.isNull()) {
		(*GetSceneStack()->rbegin())->m_lpScene->OnMove(lpDrawContext);
	}

	//	�C���f���g����̖ʓ|�Ȃ̂ł����ŁO�O�G
	} while (m_nMessage!=0);
	//	��L��OnDraw�̂Ȃ��ł���Ɏ��̃V�[���ɔ�Ԃ��Ƃ�����B
	//	���b�Z�[�W�͖����Ȃ�܂ŏ�����������̂�����

	return ;
}

void CSceneControl::OnDraw(const smart_ptr<ISurface>&lpDrawContext) {// �ǉ�
	if (!IsEnd() && !(*GetSceneStack()->rbegin())->m_lpScene.isNull()) {
		(*GetSceneStack()->rbegin())->m_lpScene->OnDraw(lpDrawContext);
	}
	return ;
}

void	CSceneControl::PushScene(int nScene){
	//	�w�肳�ꂽ�V�[�������X�^�b�N��ɐςށB
	CSceneInfo* pInfo = new CSceneInfo;
	pInfo->m_nScene = nScene;
	GetSceneStack()->push_back(smart_ptr<CSceneInfo>(pInfo));
}

bool	CSceneControl::IsEnd() const{
	return const_cast<CSceneControl*>(this)->GetSceneStack()->size()==0;
}

int CSceneControl::GetSceneNo() const{
	if (IsEnd()) return -1;
	return (*(const_cast<CSceneControl*>(this))->GetSceneStack()->rbegin())->m_nScene;
}

smart_ptr<IScene> CSceneControl::GetScene() const{
	if (IsEnd()) return smart_ptr<IScene>();
	return (*(const_cast<CSceneControl*>(this))->GetSceneStack()->rbegin())->m_lpScene;
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
