#include "stdafx.h"
#include "yaneScene.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

void	CSceneControl::CreateScene(int nScene){

	if (nScene==-1) return ; // これ失敗なり
	//	新しいシーンを構築
	CSceneInfo* pInfo = new CSceneInfo;
	pInfo->m_lpScene = GetSceneFactory()->CreateScene(nScene);
	pInfo->m_nScene = nScene;
	if (!pInfo->m_lpScene.isNull()){
		pInfo->m_lpScene->SetSceneControl(smart_ptr<ISceneControl>(this,false));
		pInfo->m_lpScene->OnInit();	//	これで初期化。
	}
	//	スタックに追加
	GetSceneStack()->push_back(smart_ptr<CSceneInfo>(pInfo));
}

void	CSceneControl::OnMove(const smart_ptr<ISurface>& lpDrawContext) {// 追加
	//	OnInitのなかで次々とシーンジャンプしうるので先にメッセージをクリア
	do {
	//	インデントするの面倒なのでここで＾＾；

	int nMessage = m_nMessage;
	m_nMessage = 0;

	//	メッセージのdispatch
	switch (nMessage) {
		//	0:No Message 1:JumpScene 2:CallScene
		//	3:CallSceneFast 4:ReturnScene 5:ExitScene
	case 0: break;
	case 1:
		//	現在のシーンがあるならば、それを破棄
		if (!IsEnd()){
			GetSceneStack()->pop_back();
		}
		//	新しいシーンを構築
		CreateScene(m_nNextScene);
		break;
	case 2: {
		//	現在のシーンを破棄
		if (!IsEnd()){
			(*GetSceneStack()->rbegin())->m_lpScene.Delete();
		}
		//	新しいシーンを構築
		CreateScene(m_nNextScene);
		break;
			}
	case 3:
		//	現在のシーンをスタック上に積んで、Deleteしない。
		//	⇒スマートポインタになっているのでpushしておけばDeleteされない。
		//	新しいシーンを構築
		CreateScene(m_nNextScene);
		break;
	case 4:	{		//	元のシーンに戻る。
		//	現在のシーンを破棄
		if (IsEnd()){
#ifdef USE_EXCEPTION
			throw CSyntaxException("これ以上ReturnScene出来ない");
#endif
		} else {
			GetSceneStack()->pop_back();
		}
		if (IsEnd()) break; //	もう戻るシーン無いねん

		CSceneInfo& pInfo = **GetSceneStack()->rbegin();
		int nScene = pInfo.m_nScene;
		if (pInfo.m_lpScene.isNull()) {
			//	シーン解放してあるから、factoryで、また作ったらにゃ！
			GetSceneStack()->pop_back();
			CreateScene(nScene);
			//	一応、呼び出そか＾＾；
			if (!(*GetSceneStack()->rbegin())->m_lpScene.isNull()) {
				(*GetSceneStack()->rbegin())->m_lpScene->OnComeBack(nScene);
			}
		} else {
			//	帰ってきたで～　ジェーンカンバークッ＾＾；
			(*GetSceneStack()->rbegin())->m_lpScene->OnComeBack(nScene);
		}
		break;
			}
	case 5:	return ;	//	描画せずに抜ける
	}

	if (!IsEnd() && !(*GetSceneStack()->rbegin())->m_lpScene.isNull()) {
		(*GetSceneStack()->rbegin())->m_lpScene->OnMove(lpDrawContext);
	}

	//	インデントするの面倒なのでここで＾＾；
	} while (m_nMessage!=0);
	//	上記のOnDrawのなかでさらに次のシーンに飛ぶことがある。
	//	メッセージは無くなるまで処理し続けるのが原則

	return ;
}

void CSceneControl::OnDraw(const smart_ptr<ISurface>&lpDrawContext) {// 追加
	if (!IsEnd() && !(*GetSceneStack()->rbegin())->m_lpScene.isNull()) {
		(*GetSceneStack()->rbegin())->m_lpScene->OnDraw(lpDrawContext);
	}
	return ;
}

void	CSceneControl::PushScene(int nScene){
	//	指定されたシーン名をスタック上に積む。
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
