//
//	yaneScene.h :
//
//		�V�[���Ǘ��N���X
//		�ˁ@��˂��炨�z�[���y�[�W�u�V�˃Q�[���v���O���}�{���M�v�X�v��
//			��W�C�X�C�P�P�C�P�R�͂��Q�l�̂��ƁB
//

#ifndef __yaneScene_h__
#define __yaneScene_h__


/**
	�V�[���Ǘ��N���X�B�Q�[�����ŕK�{�B�ڂ����́A
	��˂��炨�z�[���y�[�W�u�V�˃Q�[���v���O���}�{���M�v�X�v��
	��W�C�X�C�P�P�C�P�R�͂��Q�l�̂��ƁB

	�P�DIScene�N���X��mediator���瑽�d�p�����ăV�[���N���X������Ďg���B
	�Q�DISceneFactory��mediator�Ƒ��d�p�����Ďg���B
		�����smart_ptr��CSceneControl�ɓn���Ă��B

	���Ƃ́A�P�D�ō쐬����IScene�h���N���X�̂Ȃ��ŁA
	���ɍs�������V�[�������w�肷��Ύ��R�ɃV�[���Ԃ��ړ��i�W�����v�E�R�[���j��
	�o���܂��B
*/

#include "../Auxiliary/yaneSerialize.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

class IScene;
class ISurface;		//	���ۃT�[�t�F�[�X

class ISceneFactory {
/**
	�V�[���̍\�z�̂��߂�parameterized factory
	�́A���̃N���X��h�������āA�����ɓo�^����B
*/
public:
	virtual smart_ptr<IScene> CreateScene(int nScene) = 0;
	///	�i���o�[nScene�̃V�[���N���X�𐶐����ĕԂ�
};

class ISceneParam {
/**
	�V�[���ԂŃL���b�`�{�[�����邽�߂̃p�����[�^���N���X�B
	������h�������āA�L���b�`�{�[������Ɨǂ��B
*/
public:
	virtual ~ISceneParam(){}
};

class ISceneControl {
/**
	�V�[���̃R���g���[���̊��N���X
*/
public:
	/// ------ �V�[���̈ړ��֘A�B�����OnDraw�^�C�~���O�Ő؂�ւ��B
	///	�i����OnDraw�ł́A���̃V�[���̕`��͂���Ȃ��j
	///	���ɍs���V�[����ݒ肷��
	virtual void	JumpScene(int eScene)=0;
	///	�V�[�����R�[������iReturnScene�ŁA���̃N���X�ɖ߂��Ă���j
	///		�������A���̃N���X�͂�������delete�����B
	virtual void	CallScene(int eScene)=0;
	///	�V�[�����R�[������iReturnScene�ŁA���̃N���X�ɖ߂��Ă���j
	///		�������A���̃N���X��delete���ꂸ�Ɏc��B
	///		���̃V�[���Ɉڍs����Ƃ��ɁAOnCallSceneFast���Ăяo�����B
	virtual void	CallSceneFast(int eScene)=0;
	///	CallScene/CallSceneFast�ŌĂ΂ꂽ�V�[���ɕ��A����
	virtual void	ReturnScene()=0;
	///	�V�[�����I������
	virtual void	ExitScene()=0;
	/// --------------------------------------------------------------
	///	�V�[�����X�^�b�N�ɐςށB
	///	(ReturnScene�����Ƃ��ɁA�����ɐς܂ꂽ���i�X�^�b�N�Ȃ̂ŋt���j��
	///	�V�[�����Ăяo�����)
	virtual	void	PushScene(int eScene) =0;
	///	PopScene���ł�����ʔ��������m��Ȃ����ǁA
	///	����ł͐������V�[���Ǘ��Ƃ͌����Ȃ��C������B(���Jump�֎~�̘_��)
	/// --------------------------------------------------------------
	///	--- �`��ƈړ��͎��̂Q�̃t�F�[�Y�ɕ�������遫
	///	�I�u�W�F�N�g�̕`����s�Ȃ�
	virtual void	OnDraw(const smart_ptr<ISurface>& lp)=0;
	///	�I�u�W�F�N�g�̈ړ����s��
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
	class IScene �i�V�[���N���X�j�̂��߂̃R���g���[���[
*/
public:
	CSceneControl(const smart_ptr<ISceneFactory>& pvSceneFactory)
		{ SetSceneFactory(pvSceneFactory); }
	CSceneControl(){}
	/**
		�R���X�g���N�^�ł́A�t�@�N�g���[��n��
		�n�����˂��ꍇ�́ASetSceneFactory�œn��
	*/
	virtual void SetSceneFactory(const smart_ptr<ISceneFactory>& pv)
		{ m_pvSceneFactory = pv;}
	virtual smart_ptr<ISceneFactory> GetSceneFactory() const
		{ return m_pvSceneFactory; }

	///	---	�ȉ��̐���֐��̈Ӗ���IScene�Ɠ�����
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

	///	factory��ݒ�^�擾�ł���BCScene�h���N���X������A
	///		smart_ptr<IScene> p =
	///			GetSceneControl()->GetSceneFactory()->CreateScene(CSCENE1);
	///	�̂悤�ɂ��Ă����ЂƂ̃V�[���𐶐����Ă��̂Ȃ��Ŏq�V�[����
	///	�Ăяo�����Ƃ��o����B

	///	----- property
	///	�X�^�b�N�ɌĂяo�����V�[����������Ԃ�
	///	ReturnScene�����Ƃ��ꂪtrue�ɂȂ�
	virtual bool IsEnd() const;		//	�I�������̂��H

	///	���݂̃V�[���i���o�[��Ԃ�
	///	�����AIsEnd()==true�Ȃ�΁A-1
	virtual int GetSceneNo() const;

	///	���݂̃V�[����Ԃ�
	///	�����AIsEnd()==true�Ȃ�΁ANULL��smart_ptr���߂�
	virtual smart_ptr<IScene> GetScene() const;

protected:
	//	�V�[����factory
	smart_ptr<ISceneFactory> m_pvSceneFactory;

	virtual void	CreateScene(int nScene);	//	�V�[���̐����i�����I�Ɏg�p�j
	//	-----�ړ��v�����b�Z�[�W
	int		m_nMessage;
		//	0:No Message 1:JumpScene 2:CallScene
		//	3:CallSceneFast 4:ReturnScene 5:ExitScene
	int			m_nNextScene;
		//	���Ɉړ����ׂ��V�[��

	class CSceneInfo {
	public:
		smart_ptr<IScene>	m_lpScene;		//	�V�[���̃|�C���^
		int					m_nScene;		//	�V�[���i���o�[
		CSceneInfo() {
			m_nScene = -1;	//	����Create���Ȃ��B
		}
	};
	//	�Ăяo�����V�[���̃X�^�b�N
	smart_vector_ptr<CSceneInfo>	m_SceneInfoStack;
	smart_vector_ptr<CSceneInfo>*	GetSceneStack()
		{ return &m_SceneInfoStack;}

	///	�V�[���Ԃ̃f�[�^�̂��Ƃ�Ɏg���B
	smart_ptr<ISceneParam>	GetParam() const
		{ return m_vParam; }
	void	SetParam(const smart_ptr<ISceneParam>& vParam)
		{ m_vParam = vParam;}

	virtual void Serialize(ISerialize&){}
	///	ToDo:�܂��Ă���[�[�B���ԂȂ��āA�܂��������ĂȂ����[�B

private:
	///	�V�[���Ԃ̂�����Ƃ����f�[�^�̂��Ƃ�Ɏg���B�i�ƕ֗��O�O�j
	smart_ptr<ISceneParam>	m_vParam;
};


class IScene : public IArchive {
/**
	�V�[�����N���X

�V�[����`��
enum SCENE {
	SCENE1,			//	�V�[���P
	SCENE2,			//	�V�[���Q
};
*/
public:
	///	����́A���[�U�[���K�v�ɉ����ăI�[�o�[���C�h���邱��
	virtual void	OnDraw(const smart_ptr<ISurface>& lp) {}
	virtual void	OnMove(const smart_ptr<ISurface>& lp) {}

	///	�������́A���̊֐��ōs�Ȃ��B�i�R���X�g���N�^�ł�outer���g���Ȃ����߁j
	virtual void	OnInit() {}

	///	CallScene/CallSceneFast�ŌĂяo���A
	///	ReturnScene�Ŗ߂��Ă����Ƃ��ɌĂяo�����
	///	nScene�ɂ́A�ǂ̃V�[������߂��Ă�����������B
	virtual void	OnComeBack(int nScene){}

	///	�V�[���R���g���[���[�̐ݒ�^�擾
	void	SetSceneControl(const smart_ptr<ISceneControl>& lp) { m_lpSceneControl = lp; }
	smart_ptr<ISceneControl> GetSceneControl() const { return m_lpSceneControl; }
	/// --------------------------------------------------------------
	virtual void Serialize(ISerialize&){}
	///	�V���A���C�Y���K�v�ȃN���X�́A������I�[�o�[���C�h���Ă�����
	///	CSceneControl���V���A���C�Y�@�\��񋟂��Ă����

	IScene(const smart_ptr<ISceneControl>& lp) : m_lpSceneControl(lp){}
	IScene() {}
	
	virtual ~IScene(){} // place holder

protected:

	smart_ptr<ISceneControl> m_lpSceneControl;
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
