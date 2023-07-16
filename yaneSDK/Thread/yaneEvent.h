//	yaneEvent.h :
//		programmed by yaneurao	'02/03/05

#ifndef __yaneEvent_h__
#define __yaneEvent_h__


namespace yaneuraoGameSDK3rd {
namespace Thread {

class IEvent {
public:
	virtual	void	SetEvent()=0;
	virtual void	ResetEvent()=0;
	virtual LRESULT	Wait(int nTime=-1)=0;
	virtual HANDLE	GetHandle() const =0;
	virtual bool	IsSignal() const =0;

	virtual ~IEvent(){}
};

class CEvent : public IEvent {
/**
	WIN32�̃C�x���g��wrapper

	�����̃C�x���g�������������Ƃ�����X���b�h����
	�ʂ̃X���b�h�ɒʒm�ł���悤�ɂ��铯���I�u�W�F�N�g�ł��B

	�C�x���g�́A�X���b�h�������̍�Ƃ��s������
	�m��K�v������Ƃ��ɗL�p�ł��B

	���̃N���X�́A�f�B�t�H���g��ResetEvent���ꂽ��Ԃ��ƍl���Ă��������B

*/
public:
	virtual	void	SetEvent();
	///	�I�u�W�F�N�g���V�O�i����ԁi���p�ł����ԁj�ɂ���

	virtual void	ResetEvent();
	///	�I�u�W�F�N�g���m���V�O�i����ԁi���p�ł��Ȃ���ԁj�ɂ���

	virtual LRESULT	Wait(int nTime=-1);
	/**
		�I�u�W�F�N�g���V�O�i����ԁi���p�ł����ԁj�ɂȂ�܂ő҂�
		nTime���Ԃ����҂@�@nTime==-1 �Ȃ�Ζ����ɑ҂�

		�Ԃ��l	0 : �I�u�W�F�N�g���V�O�i����ԂɂȂ���
				1 : �҂����ԃ^�C���A�E�g�ɂȂ���
				2 : ����ȊO�i���肦�Ȃ��͂��j
	*/

	virtual HANDLE	GetHandle() const { return m_hHandle; }
	/**
		HANDLE���擾����
		�ʗ�擾����K�v�͖����̂����A
		::WaitForMultipleObject
		���ő҂Ƃ��Ƀn���h�����K�v�ɂȂ�

		�����Ŏ擾�����n���h���ɑ΂��āA
		����::SetEvent,::ResetEvent���Ăяo����
		��Ԃ�ύX�����ꍇ�́A���̃N���X��IsSignal�͈ȑO�̒l��Ԃ�
	*/

	virtual bool	IsSignal() const;
	/**
		�V�O�i�����(SetEvent���ꂽ���)�Ȃ̂��A
		�m���V�O�i�����(ResetEvent���ꂽ���)�Ȃ̂���Ԃ�
	*/

	CEvent();
	virtual ~CEvent();

protected:
	HANDLE	m_hHandle;
	volatile bool	m_bSignal;
	//	�C�x���g�ɑ΂���signal��ԂȂ̂��ۂ��𔻒肷��API��
	//	���݂��Ȃ��D�D�D�i�ǂ�������������˂�j

	CCriticalSection* GetCriticalSection() { return &m_cs; }
	CCriticalSection m_cs;
};

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd

#endif
