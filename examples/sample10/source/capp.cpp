#include "stdafx.h"
#include "CApp.h"

class CTaskControlBase {
/**
	�^�X�N����̂��߂̊��N���X
*/
public:
	void	setPriority(int nPriority) { nPriority_ = nPriority;}
	int		getPriority() { return nPriority_; }
	/*
		�v���C�I���e�B�i�^�X�N�̗D��x�j�̐ݒ�^�擾
		�v���C�I���e�B�̐ݒ莩�̂́Aclass CTaskController �̑��ōs�Ȃ��̂�
		���̔h���N���X���s�Ȃ��K�v�͂Ȃ��B
	*/

	virtual void Task(void*) = 0;
	///	�Ăяo�����ׂ��^�X�N�B�����h���N���X�ŃI�[�o�[���C�h����
	///	���̈����ɂ́ACTaskController::callTask�œn�������̂����̂܂ܓ����Ă���

protected:
	int		nPriority_;
};

class CTaskController {
/**
	�^�X�N�̃R���g���[���[
	CTaskControlBase*��list�������Ă��ĊǗ�����
*/
public:
	typedef list<CTaskControlBase*>	tasklist;
	tasklist* getTaskList() { return& tasklist_; }

	///	���ׂẴ^�X�N���Ăяo��
	void	callTask(void*p){
		tasklist::iterator it = getTaskList()->begin();
		bKillTask_ = false;
		while (it!=getTaskList()->end()){
			(*it)->Task(p);
			if (bKillTask_) {
				//	���̃^�X�N�͎����������
				bKillTask_ = false;
				delete *it;
				it = getTaskList()->erase(it);
			} else {
				it++;
			}
		}
	}

	/**
		���������^�X�N���^�X�N���X�g�ɓo�^����
		new ����CTaskControlBase�h���N���X��n���ă`��
		�v���C�I���e�B�́A�^�X�N�̗D��x�B0���ƈ�ԗD�揇�ʂ������B
		������ɔ����ă}�C�i�X�̒l��n���Ă��ǂ��悤�ɂ͂Ȃ��Ă���B
	*/
	void	addTask(CTaskControlBase*p,int nPriority)
	{
		p->setPriority(nPriority);
		tasklist::iterator it = getTaskList()->begin();
		while (it!=getTaskList()->end()){
			if ((*it)->getPriority() > nPriority) break;
			it++;
		}
		getTaskList()->insert(it,p);
	}

	/**
		�D��x���w�肵�āA���̃^�X�N�����ł�����
		(�������A�����Ŏ����̃^�X�N���폜���邱�Ƃ͏o���Ȃ�)
	*/
	void	killTask(int nPriority){
		tasklist::iterator it = getTaskList()->begin();
		while (it!=getTaskList()->end()){
			int nMyPriority = (*it)->getPriority();
			if (nMyPriority > nPriority) break;
			// priority�Ɋւ��Đ���������ł���̂Œ����Ă���΂����ł����܂�
			if (nMyPriority == nPriority) {
				delete *it;
				it = getTaskList()->erase(it);
			}
		}
	}

	/**
		�D��x���w�肵�ă^�X�N���ꊇ�폜����
		���̃��\�b�h���Ăяo���Ƃ��A�����̃^�X�N���܂܂�Ă͂����Ȃ�
	*/
	void	killTask(int nStartPriority,int nEndPriority){
		tasklist::iterator it = getTaskList()->begin();
		while (it!=getTaskList()->end()
			&& ((*it)->getPriority()<nStartPriority)) ++it;
		while (it!=getTaskList()->end()
			&& ((*it)->getPriority()<nEndPriority)) {
			delete *it;
			it = getTaskList()->erase(it);
		}
	}

	/**
		callTask�ŌĂяo����Ă���^�X�N��
		�������g���폜���邽�߂̃��\�b�h
	*/
	void	killMe() { bKillTask_ = true; }

protected:
	tasklist tasklist_;
	bool	bKillTask_;
};

//	�����������܂ł͔ėp�I�ȃQ�[���^�X�N�ƃQ�[���^�X�N�R���g���[���[
//	�K���ɂԂ��������Ďg���ă`��
/////////////////////////////////////////////////////////////////
//	�������́A���̃Q�[����p�̃Q�[���^�X�N�R���g���[���[��info

/**
	�^�X�N�D�揇�ʂƂ��Ă̓^�X�N���ƂɈقȂ�l��p�ӂ���
*/
namespace taskPriority {
	enum TaskPriority__ { task1,task2,task3 };
}

struct CGameTaskInfo {
	CApp* getApp() { return pApp;}
	void  setApp(CApp*p){ pApp = p; }

	//	�`�悷��R���e�L�X�g���擾
	CFastPlane* getDrawContext()
	{ return getApp()->GetDraw()->GetSecondary(); }

	CTaskController* getTaskController() { return pTaskController;}
	void setTaskController(CTaskController*tc) { pTaskController=tc;}

	bool bMove;	//	�ړ�����̂�
	bool bDraw;	//	�`�悷��̂�

protected:
	CTaskController* pTaskController;
	CApp*		pApp;	//	�����̃A�v���N���X
};

//	���[�U�[��`�^�X�N���N���X
class CMyTask : public CTaskControlBase {
public:
	virtual void Task(void*p){
		CGameTaskInfo* pInfo = (CGameTaskInfo*)p;
		if (pInfo->bMove) OnMove(pInfo);
		if (pInfo->bDraw) OnDraw(pInfo);
	}
	virtual void OnMove(CGameTaskInfo* p){}
	virtual void OnDraw(CGameTaskInfo* p){}
	/*
		���[�U�[�͂��̃N���X��h�������A
		OnMove��OnDraw���I�[�o�[���C�h���Ďg��
	*/

	//	�������g�̃^�X�N������(�^�X�N�R���g���[���[�̗͂��؂��)
	virtual void killMe(CGameTaskInfo* p){
		p->getTaskController()->killMe();
	}

	//	�ق��A�^�X�N�R���g���[���[�ւ̃A�N�Z�X����
	//	���̃N���X�œK����wrap����Ɨǂ�
};

struct CMyTask2 : public CMyTask {
	CMyTask2() : n_(0) {}
	virtual void OnMove(CGameTaskInfo* p){
		CDbg().Out("CMyTask2::OnMove");
		n_++;
		if (n_==4) {
			CDbg().Out("���Ԃ������̂�Task2������Ȃ�ɂ�(�L�D`)");
			killMe(p);
		}
	}
	int n_;
};

//	���[�U�[��`�^�X�N���N���X
struct CMyTask1 : public CMyTask {
	CMyTask1() : n_(0) {}
	virtual void OnMove(CGameTaskInfo* p){
		CDbg().Out("CMyTask1::OnMove");
		n_ ++;
		if (n_==3) {
			CDbg().Out("Task1��Task2�𐶐�����ɂ偛(���ށ�)o");
			p->getTaskController()->addTask(new CMyTask2,taskPriority::task2);
		}
		if (n_==5) {
			CDbg().Out("���Ԃ������̂�Task1�͂���Ȃ�ɂ�(�L�D`)");
			killMe(p);
		}
	}
	int n_;
};

void	CApp::MainThread() {				 //	 ���ꂪ���s�����

	CTaskController tc;
	CGameTaskInfo info;
	info.setApp(this);
	info.setTaskController(&tc);
	//	��P�^�X�N���N��
	tc.addTask(new CMyTask1,taskPriority::task1);
	int count=0;
	while (IsThreadValid()){
		if (count<10) { count++; CDbg().Out("--callTask--"); }
		info.bMove = true;
		info.bDraw = false;
		tc.callTask(&info);
		//	����Ń^�X�N���Ăяo��
	}
}

//	���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
	virtual void MainThread(void){			//	���ꂪ���[�J�[�X���b�h
		CApp().Start();						//	CApp app; app.Start();�̈Ӗ���
	}
};

//	���킸�ƒm�ꂽWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	�K�������Ă�
	CSingleApp sapp;
	if (sapp.IsValid()) {
		CThreadManager::CreateThread(new CAppMainWindow());					//	��Œ�`�������C���̃E�B���h�D���쐬
	}
	return 0;
}
