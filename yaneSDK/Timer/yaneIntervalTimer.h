// yaneIntervalTimer.h :
//
//		�C���^�[�o���^�C�}
//		�i�������A�蓮���R�[���o�b�N�Ŏg���j
//
//		programmed & designed by yaneurao(M.Isozaki) '02/03/20
//

#ifndef __yaneIntervalTime_h__
#define __yaneIntervalTime_h__


#include "yaneTimer.h"

namespace yaneuraoGameSDK3rd {
namespace Timer {

class IIntervalTimer {
public:
	virtual void	RegistCallBack(void*pVoid,DWORD dwNext,
			DWORD dwInterval,int nRepeat,
			const smart_ptr<function_callback>&fn)=0;
	virtual void	DeleteCallBack(void*pVoid)=0;
	virtual void	CallBack()=0;
	virtual void	SetTimer(const smart_ptr<ITimer>& pTimer)=0;

	virtual ~IIntervalTimer() {}
};

class CIntervalTimer : public IIntervalTimer {
/**
	�C���^�[�o���^�C�}�𓝍��I�ɊǗ����邽�߂̃N���X
		�i�������A�蓮���R�[���o�b�N�Ŏg���j
	�R�[���o�b�N���̂́Aclass CAppBase ���AIsThreadValid�̂Ȃ���
	�s�Ȃ��Ă����̂ŁACAppBase�̗��p�҂́A�����I�ɍs�Ȃ��K�v�͖���

	�g�p��F
	//	CAppFrame::MainThread���ɂāD�D
		//	�R�[���o�b�N�̊֐���`
		smart_ptr<function_callback> fn(
			function_callback_v::Create(&CTest::Test,&test)
		);
		//	�R�[���o�b�N�̓o�^
		GetMyApp()->GetIntervalTimer()->RegistCallBack(
			&test,	//	�R�[���o�b�N�o�^����Ă�����̂�
					//	�폜���邽�߂̎��ʎq
			3000,	//	3000ms��Ɉ��ڂ̃R�[���o�b�N
			1000,	//	1000ms���Ƃ�
			10,		//	10��R�[���o�b�N
			fn
		);
*/
public:

	virtual void	RegistCallBack(void*pVoid,DWORD dwNext,
			DWORD dwInterval,int nRepeat,
			const smart_ptr<function_callback>&fn);
	/**
		�R�[���o�b�N�̓o�^���s�Ȃ��BpVoid�́A�^�C�}���ʎq�ł���A
		���[�U�[�����̃N���X��this�|�C���^��n���Ă����΁A���ƂŁA
		��������ʎq�Ƃ��ėp���āA�o�^�����R�[���o�b�N���폜���邱�Ƃ��o����

		dwNext	�́A�P��ڂ̃R�[���o�b�N�͉��~���b��Ȃ̂��B
			���̎������������Ƃ��ɁA�R�[���o�b�N����������

		dwInterval�́A�P��ڈȍ~�̃R�[���o�b�N�̊Ԋu�B
			nRepeat��1�̂Ƃ��͊֌W�Ȃ��B

		nRepeat�́A����R�[���o�b�N���s�Ȃ��̂��̎w��B
		������w�肵���񐔂����AnCallBack�Ԋu�̃R�[���o�b�N���s�Ȃ���
		0�Ȃ�΁A�����񐔁A�Ăяo��

		fn�́A�R�[���o�b�N���ׂ��֐��B
	*/

	virtual void	DeleteCallBack(void*pVoid);
	///		RegistCallBack�Ŏw�肵��pVoid�ƈ�v����R�[���o�b�N�����ׂď���

	virtual void	CallBack();
	/**
		�o�^����Ă���R�[���o�b�N�̂����A���ԂɂȂ��Ă�����̂�
		���ԂɌĂяo���B

		class CAppBase �́AIsThreadValid�ŁA���̊֐�����o���Ă���̂ŁA
		CAppBase��MainThread���ł́A�����I�ɂ��̊֐����Ăяo���K�v�͖����B
	*/

	virtual void	SetTimer(const smart_ptr<ITimer>& pTimer);
	/**
		CFixTimer�����g���̂Ȃ�΁A
		���O�ɗp�ӂ���Ă���^�C�}�[�������Œu��������Ɨǂ��B
		�f�B�t�H���g�ł�CTimer���g����
	*/

	CIntervalTimer();
	virtual ~CIntervalTimer();

protected:
	//	�^�C�}�[�R�[���o�b�N�\����
	struct CTimerInfo {
	void*					m_pVoid;
	/**
		�^�C�}�[���ʎq�i���[�U�[�����̃N���X��this�|�C���^���j
		�n���Ă����΁A���̃N���X�����������^�C�}�݂̂��������邱�Ƃ��o����
	*/
	DWORD					m_dwCallBackTimer;
	//	���^�C�}�[���A���̎������߂���΁A�R�[���o�b�N����

	DWORD					m_dwInterval;
	//	�R�[���o�b�N�̊Ԋu[ms]

	int						m_nRepeat;
	//	�c��A�Ăяo����(0�Ȃ�΁A�����ɌĂяo��)

	smart_ptr<function_callback>		m_fnCallBack;
	///	�R�[���o�b�N���ׂ��֐�

	bool					m_bDelete;
	///	���̃I�u�W�F�N�g�͎��̃t�F�C�Y�ŏ������̂��H
	///	iterator��smart_iterator�ł͖����̂ŁA�t�F�C�Y��x�点��
	///	�폜���Ă��K�v������
	bool	IsValid() const { return! m_bDelete; }
	///	�ˁ@smart_list_ptr�̃C�e���[�^���ێg���̂ɕK�v

	};
	typedef smart_list_ptr<CTimerInfo> CTimerList;

	CTimerList	m_listTimerInfo;
	CTimerList* GetTimerInfo(){ return& m_listTimerInfo;}

	smart_ptr<ITimer>	m_pTimer;
	smart_ptr<ITimer>	GetTimer() const { return m_pTimer; }
	//	�^�C�}�[�J�E���g�́A�����Ŏ擾����
};

} // end of namespace Timer
} // end of namespace yaneuraoGameSDK3rd

#endif
