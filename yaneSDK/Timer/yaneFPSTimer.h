// yaneFPSTimer.h
//
//	���m�����ʂȂ��E�F�C�g���|����
//
//	SetFPS��FPS���Z�b�g���āA���Ƃ́A�`��O��ɂł��AWait()���邾���ŗǂ��B
//

#ifndef __yaneFPSTimer_h__
#define __yaneFPSTimer_h__


#include "yaneTimer.h"

namespace yaneuraoGameSDK3rd {
namespace Timer {

class IFPSTimer {
public:
	virtual void SetFPS(DWORD fps)=0;
	virtual DWORD GetFPS()=0;
	virtual DWORD GetRealFPS()=0;
	virtual DWORD GetCPUPower()=0;
	virtual DWORD GetSkipFrame() const =0;
	virtual void ResetTimer()=0;
	virtual void WaitFrame()=0;
	virtual bool ToBeSkip() const=0;
	virtual void ResetElapseTimeCounter()=0;
	virtual DWORD GetElapseTimeCounter()=0;
	virtual void ResetCallCounter()=0;
	virtual DWORD GetCallCounter()=0;

	virtual ~IFPSTimer(){}
};

class CFPSTimer : public IFPSTimer {
/**
	���Ԃ������I�ɑ҂��߂̃^�C�}�[�B
	�t���[�����[�g�i��b�Ԃ̕`�文���j���U�O�e�o�r�iFrames Par Second�j��
	�������鎞�ȂǂɎg���B

*/
public:
	virtual void SetFPS(DWORD fps);
	/**
		FPS�̐ݒ�i�C�j�V�����C�Y�����˂�j
		�f�B�t�H���g�ł͂U�O�e�o�r�B
	*/

	virtual DWORD GetFPS();
	///	 FPS�̎擾

	virtual DWORD GetRealFPS();
	/**
		 FPS�̎擾�i����l�j
		1�b�Ԃɉ���WaitFrame���Ăяo�������A
		�O��32��̌Ăяo�����Ԃ̕��ς���Z�o����B
	*/

	virtual DWORD GetCPUPower();
	/**
		 CPU�ғ����̎擾�i����l�j
		�b�o�t�̉ғ����ɍ��킹�āA0�`100�̊Ԃ̒l���Ԃ�B
		����������́AWaitFrame��Sleep�������Ԃ���Z�o���ꂽ���̂ł����āA
		�����܂ŎQ�l�l�ł���B
	*/

	virtual DWORD GetSkipFrame() const;
	/**
	SetFPS���ꂽ�l�܂ł̕`��ɁAToBeSkip��true�ɂȂ��Ă����t���[������Ԃ��B
	�������A�����Ō����t���[�����Ƃ́AWaitFrame�̌Ăяo�����Ƃ�
	�P�t���[���ƌv�Z�B
	*/

	virtual void ResetTimer();
	/**
		�҂����Ԃ̒[���̃��Z�b�g�i���ʁA�g���K�v�͂Ȃ��j
		WaitFrame�ł́A�O���WaitFrame���Ăяo�������Ԃ��A����̑҂����Ԃ�
		�Z�o����̂Ɏg���iSetFPS�Őݒ肵���l�ɋ߂Â��悤�Ƃ��邽�߁A�O��A
		�s�����Ă������Ԃ�����̑҂����Ԃ��猸�炷�j�̂ŁA�O��̌Ăяo�����Ԃ�
		���ݎ��ɂ��邱�Ƃɂ���āA����̑҂����Ԃւ̏C�����������̂����̊֐��B
		�ʏ�A�g�����Ƃ͖����B
	*/

	virtual void WaitFrame();
	/**
		�P�t���[�����̎��Ԃ�����܂ő҂�
	�@	���C�����[�v�̂Ȃ��ł́A�`�揈�����s�Ȃ������ƁA
		����WaitFrame���Ăяo���΁ASetFPS�Őݒ肵��
		�t���[�����[�g�Ɏ����I�ɒ��������B
	*/

	virtual bool ToBeSkip() const;
	/**	
		���̃t���[�����X�L�b�v���ׂ����H
	�����A���ꂪtrue��Ԃ��Ă���Ȃ�΁A���̃t���[���̕`����܂邲��
	�X�L�b�v���Ă��܂��΁i�������A�L�����̈ړ����̌v�Z�����͍s�Ȃ��j�A
	�b�Ԃ̂e�o�r�͒x���}�V���ł����Ɖ��肵�ăv���O�������邱�Ƃ��o����B
	�������A���̂Ƃ���WaitFrame�͌Ăяo�����ƁB
	�ڂ�������́A��˂��炨�̃z�[���y�[�W
		http://www.sun-inet.or.jp/~yaneurao/
	�̓V�˃Q�[���v���O���}�{���M�v�X��Q�P�͂��Q�Ƃ̂��ƁB

	�܂��A�t���[�����X�L�b�v���邩�ǂ����́AWaitFrame�̒i�K�Ŋm�肷��B
	���̂��߁A����WaitFrame���Ăяo���܂ŁAToBeSkip�͓����^�U�l��Ԃ�������B
	*/


	/**
		WaitFrame�ŏ�������Ԃ�݌v�ŃJ�E���g���Ă���B
		���̃J�E���^�̂�Z�b�g�Ǝ擾����̂����̊֐��ł���B
	*/
	virtual void ResetElapseTimeCounter();
	///		����Ԃ̃��Z�b�g
	virtual DWORD GetElapseTimeCounter();
	///		����Ԃ̎擾

	/**
		WaitFrame���Ăяo�����Ƃɓ����I�ȌĂяo���J�E���^��
		�C���N�������g�����B���̃J�E���^�����Z�b�g����̂Ǝ擾����̂�
		���̊֐��ł���B
	*/
	virtual void ResetCallCounter();
	///		�ďo���J�E���^�̃��Z�b�g
	virtual DWORD GetCallCounter();
	///		�ďo���J�E���^�̎擾

	CFPSTimer();
	virtual ~CFPSTimer();

protected:
	DWORD m_dwFPS;		  //  FPS(�f�B�t�H���g��60)
	DWORD m_dwFPSWait;	  // =1000*0x10000/FPS; // 60FPS�Ɋ�Â��E�F�C�g
	DWORD m_dwLastDraw;	  // �O��̕`�掞��
	DWORD m_dwFPSWaitTT;  // �O�񔭐������[�����Ԃ̕ێ��̂���
	DWORD m_dwElapsedTime;	  // �O�񃊃Z�b�g����Ă���Elapse���ꂽ����
	int	  m_nUnuseSleep;	  // Wait�֐�����Sleep�𗘗p���邩
	DWORD m_dwDrawTime[32];	   // FPS����p�̕`�掞�Ԍv�Z�p
	DWORD m_dwElapseTime[32];  // CPU Power����p
	DWORD m_nDrawCount;		   // �i����32��̌ďo�����Ԃ̕��ς���FPS�����߂�j
	DWORD m_dwCallCount;	// WaitFrame���Ăяo���ꂽ��

	bool	m_bFrameSkip;		 //	���̃t���[���̓X�L�b�v����̂��H
	DWORD	m_dwFrameSkipCounter; // �@�t���[���X�L�b�v�J�E���^
	DWORD	m_dwFrameSkipCounterN; // �v�����̃t���[���X�L�b�v�J�E���^

	CTimer	m_vTimer;
	CTimer* GetTimer() { return &m_vTimer; }
};

} // end of namespace Timer
} // end of namespace yaneuraoGameSDK3rd

#endif
