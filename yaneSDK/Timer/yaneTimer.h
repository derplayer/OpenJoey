// yaneTimer.h :
//
//	CTimer			:	�o�ߎ��Ԃ��J�E���g����^�C�}
//
//		programmed by yaneurao(M.Isozaki) '99/07/25
//		modified by yaneurao '00/02/28-'00/03/13
//

#ifndef __yaneTime_h__
#define __yaneTime_h__


namespace yaneuraoGameSDK3rd {
namespace Timer {

class CTimeGetTimeWrapper {
/**
	�����b��timeGetTime��wrapper

	timeGetTime���g���Ȃ��󋵂ɒu���ẮA
	GetTickCount���g���悤�ɐݒ肷��

	���̃R���X�g���N�^�`�f�X�g���N�^��
	timeBeginPeriodMin��timeEndPeriodMin��
	�Ăяo���Ă���
	
	���̃I�u�W�F�N�g��ref_creater�ŗp����B
	(singleton�ł��ǂ������̂���..)
*/
public:

	DWORD	GetTime() {
		///	���̃^�C�}�̎擾
		if (m_bUseTGT) {
			return ::timeGetTime();
		} else {
			return ::GetTickCount();
		}
	}

	CTimeGetTimeWrapper();
	virtual ~CTimeGetTimeWrapper();

	static	ref_creater<CTimeGetTimeWrapper>*	GetRefObj()
		{ return & m_vTimeGetTime; }

protected:
	bool		m_bUseTGT;		//	timeGetTime���g�p����̂��H
	int			m_nRef;			//	timeBeginPeriodMin�`timeEndPeriodMin�̎Q�ƃJ�E���g
	TIMECAPS	m_dwTimeCaps;	//	�^�C�}�[

	static	ref_creater<CTimeGetTimeWrapper>	m_vTimeGetTime;
};

//////////////////////////////////////////////////////////////////////////////

class ITimer {
public:
	virtual void	Reset()=0;			///	���݂̎������O��
	virtual DWORD	Get()=0;			///	���݂̎����̎擾
	virtual void	Set(DWORD)=0;		///	���݂̎����̐ݒ�
	virtual void	Pause()=0;			///	Pause�@
	virtual void	Restart()=0;		///	Pause����
	virtual ~ITimer() {}
};

class CTimer : public ITimer {
/**
	�����b���A�Ɨ��^�C�}�B
	Reset()����ƁA�^�C�}�����Z�b�g����A����ȍ~�AGet()���Ăяo���ƁA
	�O��AReset()���ꂽ�Ƃ�����̌o�ߎ��ԁi[ms]�P�ʁj���Ԃ��Ă���悤�ɂȂ�B
*/
public:
	virtual void	Reset();			///	���݂̎������O��
	virtual DWORD	Get() ;				///	���݂̎����̎擾
	virtual void	Set(DWORD);			///	���݂̎����̐ݒ�
	virtual void	Pause();			///	Pause�@�
	virtual void	Restart();			///	Pause����

	CTimer();
	virtual ~CTimer();

protected:
	DWORD	m_dwOffsetTime;					//	�I�t�Z�b�g�l
	DWORD	m_dwPauseTime;					//	Pause�������Ƃ���Time
	int		m_bPaused;						//	pause�����H

private:
	static	ref_creater<CTimeGetTimeWrapper>*	GetRefObj()
	{ return CTimeGetTimeWrapper::GetRefObj(); }
	static	CTimeGetTimeWrapper*	GetObj()
	{ return GetRefObj()->get(); }
};

//////////////////////////////////////////////////////////////////////////////

class CFixTimer : public ITimer {
/**
	�Q�[���� class CTimer ���g���ꍇ�A
	�P�t���[���̊Ԃ́A�Œ�l���Ԃ��Ă����ق����]�܂����B

	class CMouse �ɑ΂��� class CMouseEx �Ɠ����֌W�ł���B
	Flush�֐��̐�����ǂނ��ƁB
*/
public:
	virtual void	Reset();			///	���݂̎������O��
	virtual DWORD	Get();				///	���݂̎����̎擾
	virtual void	Set(DWORD);			///	���݂̎����̐ݒ�
	virtual void	Pause();			///	Pause�@�
	virtual void	Restart();			///	Pause����

	virtual	void	Flush();
	/**
		�������X�V����B
		����������u�Ԃ̎����l�Ɋ�Â���Get�Œl���Ԃ�悤�ɂȂ�B
		�ȍ~�A�ēx���̊֐����Ăяo���܂ŁAFlush�Ɠ����l���Ԃ�
		���̊֐��ȊO�́Aclass CTimer �Ɠ���
	*/

	CFixTimer();

protected:
	CTimer	m_vTimer;
	DWORD	m_dwTimeGetTime;				//	�O��Flush��������

	CTimer* GetTimer() { return& m_vTimer; }
};

} // end of namespace Timer
} // end of namespace yaneuraoGameSDK3rd

#endif
