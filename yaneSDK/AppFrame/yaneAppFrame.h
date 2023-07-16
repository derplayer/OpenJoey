//	yaneAppFrame.h :
//			programmed by yaneurao	'00/06/16

#ifndef __yaneAppFrame_h__
#define __yaneAppFrame_h__

#include "yaneAppBase.h"
#include "yaneAppManager.h"

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

class IAppFrame {
public:
	virtual void	Start()=0;
	virtual void	MesSleep(int nTime)=0;
	virtual void	InvalidateThread()=0;
	virtual IAppBase* GetMyApp()const=0;
	virtual bool	IsThreadValid()const=0;
	virtual void	MainThread() = 0;
	virtual bool	IsMainApp() const =0;
	virtual void	SetMainApp(bool b) =0;

	//	�R���X�g���N�^�`�f�X�g���N�^�ŁACAppManager�ɓo�^���Ă���
	IAppFrame();
	virtual ~IAppFrame();
};

class CAppFrame : public IAppFrame {
/**
	�A�v���P�[�V�����̒��j�ƂȂ�N���X�ł��B

	�A�v���P�[�V�����Ƃ́A�A�v���P�[�V�����N���X(class CAppBase�̔h���N���X)
	�̂��Ƃł��B�A�v���P�[�V�����N���X�́A��̃E�B���h�D��
	��̃X���b�h���[�v�A�P�̕`��N���X��ۗL���܂��B

	���̃��[�J�[�X���b�h�̂Ȃ�����Ăяo���̂�CAppFrame�Ƃ����킯�ł��B

	�g�����F

	���[�U�[�́ACAppFrame��h�����A�����Ƀ����o�Ƃ��Ď����̎g������
	�R���|�[�l���g�������o�Ƃ��Ď������܂��B

	virtual void MainThread() = 0;
	���̊֐����I�[�o�[���C�h���āAStart�����o�֐����Ăяo���Ă��������B
	��������΁A��������A���̊֐������s����܂��B

	���̃N���X�̊��N���X�ł���Aclass IAppFrame �̃R���X�g���N�^��
	CAppManager�ɓo�^���Ă���̂ŁA����X���b�h�Ȃ�΂ǂ�����ł�
	CAppManager::GetMyFrame�Ŏ擾���邱�Ƃ��o���܂��B
	�K�v�ɉ����āA�������_�E���L���X�g���Ďg���Ɨǂ��ł��傤�B

*/
public:
	///	�X���b�h�̐�����
	virtual bool IsThreadValid()const { return GetMyApp()->IsThreadValid(); }
	/**
		MainThread�̂Ȃ��ł́A���̊֐���true�̊ԁA�܂�葱����悤�ɂ��܂��B
		�i�������A���ꂪtrue�ł����Ă������I�ɔ����Ă��\���܂���j
	*/

	///	�X���b�h�̖�����
	virtual void	InvalidateThread() { GetMyApp()->InvalidateThread(); }
	/**
		���̊֐������s����ƁAIsThreadValid��false��Ԃ��悤�ɂȂ�܂��B
		CAppManager::GetMyFrame�Ɨ��߂āA�O�����炱�̃t���[���X���b�h��
		��~�v�����o���̂Ɏg���ƕ֗��ł��B
	*/

	///	�Ăяo�����̃A�v���N���X�̎擾
	virtual IAppBase* GetMyApp()const { return m_lpMyApp; }
	/**
		���̊֐����Ăяo�����ƂŁA�����̑�����A�v���P�[�V�����N���X��
		�擾���o���܂��B�i�قƂ�ǎg���K�v�͂Ȃ��͂��ł����j
	*/

	virtual void	Start();
	/**
		���̃N���X�ł̎��s�ɍۂ��ẮAMainThread���I�[�o�[���C�h���Ă���
		���̃��\�b�h���Ăяo���Ă��������B��������Ώ��������s�Ȃ�ꂽ����
		MainThread���Ăяo����܂��B
	*/

	virtual void	MesSleep(int nTime);
	/**
		Windows���b�Z�[�W���������Ȃ��ƁA
		IntervalTimer�ł̃R�[���o�b�N������Ȃ��̂ŁA
		WindowMessage�������s�Ȃ��Ȃ���Sleep����֐��ł��B
		nTime[ms]�̊ԁASleep���܂��B
		�܂��A���Ȃ��Ƃ��AnTime[ms]�͌o�߂���܂ł́A���̊֐����甲���܂���B
		class CVolumeFader�Ńt�F�[�h������Ƃ��ȂǂɌĂяo����
		���ʓI�����m��܂���B
	*/

	virtual void	SetMainApp(bool b) { GetMyApp()->SetMainApp(b); }
	virtual bool	IsMainApp() const { return GetMyApp()->IsMainApp(); }
	/**
		���C���A�v���ł��邩��ݒ�^�擾����
		�idefault : false�j

		���C���A�v���ɐݒ肵�Ă����΂���CAppFrame�h���N���X��
		�I������Ƃ��ɁA�����I�ɑ���CAppBase�h���N���X�̃C���X�^���X��
		�I��������B
	*/

private:
	IAppBase* m_lpMyApp;
};

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

#endif
