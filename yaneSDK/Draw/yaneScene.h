//
//	yaneScene.h :
//
//		シーン管理クラス
//		⇒　やねうらおホームページ「天才ゲームプログラマ養成ギプス」の
//			第８，９，１１，１３章を参考のこと。
//

#ifndef __yaneScene_h__
#define __yaneScene_h__


/**
	シーン管理クラス。ゲーム等で必須。詳しくは、
	やねうらおホームページ「天才ゲームプログラマ養成ギプス」の
	第８，９，１１，１３章を参考のこと。

	１．ISceneクラスとmediatorから多重継承してシーンクラスを作って使う。
	２．ISceneFactoryとmediatorと多重継承して使う。
		それのsmart_ptrをCSceneControlに渡してやる。

	あとは、１．で作成したIScene派生クラスのなかで、
	次に行きたいシーン名を指定すれば自由にシーン間を移動（ジャンプ・コール）が
	出来ます。
*/

#include "../Auxiliary/yaneSerialize.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

class IScene;
class ISurface;		//	抽象サーフェース

class ISceneFactory {
/**
	シーンの構築のためのparameterized factory
	は、このクラスを派生させて、そこに登録する。
*/
public:
	virtual smart_ptr<IScene> CreateScene(int nScene) = 0;
	///	ナンバーnSceneのシーンクラスを生成して返す
};

class ISceneParam {
/**
	シーン間でキャッチボールするためのパラメータ基底クラス。
	こいつを派生させて、キャッチボールすると良い。
*/
public:
	virtual ~ISceneParam(){}
};

class ISceneControl {
/**
	シーンのコントロールの基底クラス
*/
public:
	/// ------ シーンの移動関連。次回のOnDrawタイミングで切り替わる。
	///	（次のOnDrawでは、このシーンの描画はされない）
	///	次に行くシーンを設定する
	virtual void	JumpScene(int eScene)=0;
	///	シーンをコールする（ReturnSceneで、このクラスに戻ってくる）
	///		ただし、このクラスはいったんdeleteされる。
	virtual void	CallScene(int eScene)=0;
	///	シーンをコールする（ReturnSceneで、このクラスに戻ってくる）
	///		ただし、このクラスはdeleteされずに残る。
	///		そのシーンに移行するときに、OnCallSceneFastが呼び出される。
	virtual void	CallSceneFast(int eScene)=0;
	///	CallScene/CallSceneFastで呼ばれたシーンに復帰する
	virtual void	ReturnScene()=0;
	///	シーンを終了する
	virtual void	ExitScene()=0;
	/// --------------------------------------------------------------
	///	シーンをスタックに積む。
	///	(ReturnSceneしたときに、ここに積まれた順（スタックなので逆順）で
	///	シーンが呼び出される)
	virtual	void	PushScene(int eScene) =0;
	///	PopSceneもできたら面白いかも知れないけど、
	///	それでは正しいシーン管理とは言えない気もする。(大域Jump禁止の論理)
	/// --------------------------------------------------------------
	///	--- 描画と移動は次の２つのフェーズに分解される↓
	///	オブジェクトの描画を行なう
	virtual void	OnDraw(const smart_ptr<ISurface>& lp)=0;
	///	オブジェクトの移動を行う
	virtual void	OnMove(const smart_ptr<ISurface>& lp)=0;

	virtual void SetSceneFactory(const smart_ptr<ISceneFactory>& pv)=0;
	virtual smart_ptr<ISceneFactory> GetSceneFactory() const=0;
	virtual bool IsEnd() const=0;
	virtual int GetSceneNo() const=0;
	virtual smart_ptr<IScene> GetScene() const=0;
	virtual ~ISceneControl(){}
};

class CSceneControl : public ISceneControl , public IArchive {
/**
	class IScene （シーンクラス）のためのコントローラー
*/
public:
	CSceneControl(const smart_ptr<ISceneFactory>& pvSceneFactory)
		{ SetSceneFactory(pvSceneFactory); }
	CSceneControl(){}
	/**
		コンストラクタでは、ファクトリーを渡す
		渡し損ねた場合は、SetSceneFactoryで渡す
	*/
	virtual void SetSceneFactory(const smart_ptr<ISceneFactory>& pv)
		{ m_pvSceneFactory = pv;}
	virtual smart_ptr<ISceneFactory> GetSceneFactory() const
		{ return m_pvSceneFactory; }

	///	---	以下の制御関数の意味はISceneと同じね
	virtual void	JumpScene(int nScene)
		{ m_nNextScene = nScene; m_nMessage=1; }
	virtual void	CallScene(int nScene)
		{ m_nNextScene = nScene; m_nMessage=2; }
	virtual void	CallSceneFast(int nScene)
		{ m_nNextScene = nScene; m_nMessage=3; }
	virtual void	ReturnScene(){ m_nMessage=4; }
	virtual void	ExitScene()	 { m_nMessage=5; }
	virtual void	PushScene(int nScene);
	virtual void	OnDraw(const smart_ptr<ISurface>& lp);
	virtual void	OnMove(const smart_ptr<ISurface>& lp);

	///	factoryを設定／取得できる。CScene派生クラス内から、
	///		smart_ptr<IScene> p =
	///			GetSceneControl()->GetSceneFactory()->CreateScene(CSCENE1);
	///	のようにしてもうひとつのシーンを生成してそのなかで子シーンを
	///	呼び出すことも出来る。

	///	----- property
	///	スタックに呼び出し元シーンが無い状態で
	///	ReturnSceneされるとこれがtrueになる
	virtual bool IsEnd() const;		//	終了したのか？

	///	現在のシーンナンバーを返す
	///	もし、IsEnd()==trueならば、-1
	virtual int GetSceneNo() const;

	///	現在のシーンを返す
	///	もし、IsEnd()==trueならば、NULLなsmart_ptrが戻る
	virtual smart_ptr<IScene> GetScene() const;

protected:
	//	シーンのfactory
	smart_ptr<ISceneFactory> m_pvSceneFactory;

	virtual void	CreateScene(int nScene);	//	シーンの生成（内部的に使用）
	//	-----移動要求メッセージ
	int		m_nMessage;
		//	0:No Message 1:JumpScene 2:CallScene
		//	3:CallSceneFast 4:ReturnScene 5:ExitScene
	int			m_nNextScene;
		//	次に移動すべきシーン

	class CSceneInfo {
	public:
		smart_ptr<IScene>	m_lpScene;		//	シーンのポインタ
		int					m_nScene;		//	シーンナンバー
		CSceneInfo() {
			m_nScene = -1;	//	何もCreateしない。
		}
	};
	//	呼び出し元シーンのスタック
	smart_vector_ptr<CSceneInfo>	m_SceneInfoStack;
	smart_vector_ptr<CSceneInfo>*	GetSceneStack()
		{ return &m_SceneInfoStack;}

	///	シーン間のデータのやりとりに使う。
	smart_ptr<ISceneParam>	GetParam() const
		{ return m_vParam; }
	void	SetParam(const smart_ptr<ISceneParam>& vParam)
		{ m_vParam = vParam;}

	virtual void Serialize(ISerialize&){}
	///	ToDo:まってくれーー。時間なくて、まだ実装してないんよー。

private:
	///	シーン間のちょっとしたデータのやりとりに使う。（と便利＾＾）
	smart_ptr<ISceneParam>	m_vParam;
};


class IScene : public IArchive {
/**
	シーン基底クラス

シーン定義例
enum SCENE {
	SCENE1,			//	シーン１
	SCENE2,			//	シーン２
};
*/
public:
	///	これは、ユーザーが必要に応じてオーバーライドすること
	virtual void	OnDraw(const smart_ptr<ISurface>& lp) {}
	virtual void	OnMove(const smart_ptr<ISurface>& lp) {}

	///	初期化は、次の関数で行なう。（コンストラクタではouterが使えないため）
	virtual void	OnInit() {}

	///	CallScene/CallSceneFastで呼び出し、
	///	ReturnSceneで戻ってきたときに呼び出される
	///	nSceneには、どのシーンから戻ってきたかが入る。
	virtual void	OnComeBack(int nScene){}

	///	シーンコントローラーの設定／取得
	void	SetSceneControl(const smart_ptr<ISceneControl>& lp) { m_lpSceneControl = lp; }
	smart_ptr<ISceneControl> GetSceneControl() const { return m_lpSceneControl; }
	/// --------------------------------------------------------------
	virtual void Serialize(ISerialize&){}
	///	シリアライズが必要なクラスは、これをオーバーライドしておけば
	///	CSceneControlがシリアライズ機構を提供してくれる

	IScene(const smart_ptr<ISceneControl>& lp) : m_lpSceneControl(lp){}
	IScene() {}
	
	virtual ~IScene(){} // place holder

protected:

	smart_ptr<ISceneControl> m_lpSceneControl;
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
