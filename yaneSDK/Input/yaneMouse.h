// MouseInput.h :
//	�}�E�X���͗p
//		programmed by yaneurao(M.Isozaki) '99/7/31
//
//	���������N���X�ł͂Ȃ��B:p
//
//		���@�_�u���N���b�N�̊��m�͂��Ȃ��B
//

#ifndef __yaneMouseInput_h__
#define __yaneMouseInput_h__


#include "../Window/yaneWinHook.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

class IMouse {
public:
	virtual LRESULT GetXY(int &x,int &y)const=0;
	virtual bool	RButton()const=0;
	virtual bool	LButton()const=0;
	virtual LRESULT GetInfo(int &x,int &y,int &b)const=0;
	virtual void	GetButton(bool&bL,bool&bR)=0;
	virtual void	ResetButton()=0;
	virtual LRESULT SetXY(int x,int y)=0;
	virtual void	SetOutScreenInput(bool bEnable)=0;

	virtual ~IMouse(){}
};

class CMouse : public IWinHook,public IMouse {
/**
	�}�E�X�̃��A���^�C����Ԏ擾�p�̃N���X�ł��B

	��s���ăE�B���h�D���������Ă���K�v������̂�
	class CAppFrame �h���N���X���Ŏg�p����悤�ɂ��Ă��������B
*/
public:
	virtual LRESULT GetXY(int &x,int &y)const;
	///		�}�E�X�|�W�V�����𓾂�i�N���C�A���g���W�n�ɂāj

	virtual bool	RButton()const;
	///		�E�{�^����Ԃ𓾂�i���݂̃��A���^�C���̏��j

	virtual bool	LButton()const;
	///		���{�^����Ԃ𓾂�i���݂̃��A���^�C���̏��j

	virtual LRESULT GetInfo(int &x,int &y,int &b)const;
	///	�}�E�X�|�W�V�����ƃ{�^����Ԃ�Ԃ�
	///	(b:�E�{�^�������Ȃ��+1,���{�^�������Ȃ��+2 �����Ȃ��+1+2==+3)

	virtual void	GetButton(bool&bL,bool&bR);
	///	�O���GetButton���牟���ꂽ���H
	virtual void	ResetButton();
	///	GetButton���Ŏ擾�ł���{�^����Ԃ̃��Z�b�g

	virtual LRESULT SetXY(int x,int y);
	///	�}�E�X���w��̃|�W�V�����Ɉړ��i�N���C�A���g���W�n�ɂāj

	virtual void	SetOutScreenInput(bool bEnable);
	/**
		�������܂܁A�}�E�X����ʊO�ɂ�����Ƃ��A�ǂ��Ȃ�̂��H
			true  == �{�^���͉������܂܂��ƌ��Ȃ����
			false == �{�^���͗����ꂽ���̂ƌ��Ȃ����
			default�ł�false
	*/

	CMouse();
	virtual ~CMouse();

protected:
	LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam); // ���b�Z�[�W�̃R�[���o�b�N

	bool	m_bRB;					//�@�}�E�X�{�^�����
	bool	m_bLB;
	bool	m_bHistRB;				//	����
	bool	m_bHistLB;
	bool	m_bOutScreenInput;		//	��ʊO�ł̓���
};

class CFixMouse : public IMouse {
/**
	�}�E�X�̃��A���^�C�����̎擾�̂��߂̃N���X�ł�

	�Q�[����CMouse���g���ꍇ�A�P�t���[���̊Ԃ́A
	�Œ�l���Ԃ��Ă����ق����]�܂����B

	class CTimer �ɑ΂��� CFixTimer �̊֌W�Ɠ����ł���B
*/
public:
	///	�t���b�V��������
	virtual LRESULT	Flush();
	/**
		�}�E�X�̍��W�A�{�^����Ԃ��X�V����B
		����������u�Ԃ̏�ԂɊ�Â��Ċe�����o�֐��Œl���Ԃ�悤�ɂȂ�B
		���Ƃ͂قƂ�ǁAclass CMouse �Ɠ����B�������A
	*/

	virtual LRESULT GetXY(int &x,int &y)const;
	virtual bool	RButton()const;
	virtual bool	LButton()const;
	virtual LRESULT GetInfo(int &x,int &y,int &b)const;

	///	�O���Flush���牟���ꂽ���H
	virtual void	GetButton(bool&bL,bool&bR);
	virtual bool	IsPushRButton()const;
	virtual bool	IsPushLButton()const;

	///	�O���Flush���牟���グ��ꂽ���H
	virtual void	GetUpButton(bool&bL,bool&bR);
	virtual bool	IsPushUpRButton()const;
	virtual bool	IsPushUpLButton()const;

	virtual void	ResetButton();
	///	���{�^����ԃ��Z�b�g

	virtual LRESULT SetXY(int x,int y);
	///	�}�E�X���w��̃|�W�V�����Ɉړ��i�N���C�A���g���W�n�ɂāj
	///	����ō��W���ړ��������ꍇ�AFlush���Ȃ��Ƃ�GetXY����΂��̍��W���Ԃ�B

	/**
		�K�[�h�^�C���Ƃ́A�V�[���Ǘ������Ă����肷��Ƃ��A�{�^����������āA
		���̃V�[���Ɉړ����āA���̃V�[���ł����Ȃ�{�^���������ꂽ�Ɣ��肳���
		���܂����Ƃ�h�~���邽�߂ɁA��莞�ԁu���͂������v�ƉR��Ԃ����߂�
		�@�\�ł��BSetGuardTime�Őݒ肵��������Flush�����o�֐����Ăяo���܂ł́A
		GetButton/IsPushRButton/IsPushLButton�ł͂��ׂă{�^����
		������Ă��Ȃ��ƕԂ�܂��B
	*/
	///	�K�[�h�^�C�������ǂ�����Ԃ�
	virtual bool	IsGuardTime()const;
	///	�K�[�h�^�C����ݒ肷��
	void	SetGuardTime(int nTime);

	virtual void	SetOutScreenInput(bool bEnable)
		{ GetMouse()->SetOutScreenInput(bEnable);}

	CFixMouse();
	virtual ~CFixMouse();

protected:
	bool	m_bRBN;					//�@Flush�����Ƃ��̃}�E�X�{�^�����
	bool	m_bLBN;
	int		m_nRLBN;				//	Flush�����Ƃ��̃}�E�X�{�^�����
	int		m_nX,m_nY;				//	Flush�����Ƃ��̃}�E�X�̃|�W�V����
	int		m_nGuardTime;			//	�K�[�h�^�C��
	bool	m_bHistRB;				//	����
	bool	m_bHistLB;

	CMouse	m_vMouse;				//	�����ɈϏ�
	CMouse* GetMouse() { return &m_vMouse; }
};

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd

#endif
