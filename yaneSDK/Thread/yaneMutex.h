// yaneMutex.h :
//
//	programmed by M.Isozaki '99/06/19
//	����n���h�����擾����̂Œx���ł��:p

#ifndef __yaneMutex_h__
#define __yaneMutex_h__


namespace yaneuraoGameSDK3rd {
namespace Thread {

class IMutex {
public:
	virtual LRESULT Open(const string& strMutexName)=0;
	virtual void Wait()=0;
	virtual void Release()=0;
	virtual ~IMutex(){}
};

class CMutex : public IMutex {
/**
	Mutex�N���X�ł��B�n�r���x���łP�������݂��Ȃ����ݔr����ړI�Ƃ��܂��B
	�Q�d�N���̖h�~�ȂǂɎg���܂��B
	�Q�d�N���h�~�́Aclass CSingleApp ���Q�Ƃ��邱�ƁB

	���X���b�h�Ƃ̑��ݔr���Ȃ�� class CCriticalSection ��p���邱�ƁB
	������̂ق��������͒f�R�y���ł��B

*/
public:
	virtual LRESULT Open(const string& strMutexName);
	/**
	�@strMutexName�́A�~���[�e�b�N�X���i�C�ӂɂ��ėǂ��j

	�@�Ԃ��l��
	�@0 : ���ɓ����̂��̂͑��݂��Ȃ������L���l�������I
	�@1 : ���ɓ����̂��̂����݂���B(error)�˂��̂���Wait�ő҂Ɨǂ�
	  2 : �����̃Z�}�t�H�A�~���[�e�b�N�X�A�ҋ@�\�ȃ^�C�}�A�W���u�A
		�t�@�C���}�b�s���O�I�u�W�F�N�g�̂����ꂩ�̖��O�ƈ�v�����̂ŃG���[
	*/

	virtual void Wait();
	/**
		Open�łP���Ԃ��Ă��Ă���Ƃ��ɌĂяo���āA
		���L�����l���ł���̂�҂B
	*/
	virtual void Release();
	/**
		Mutex���������
		�f�X�g���N�^�������I�ɂ���Release���Ăяo���̂ŁA
		�����ɉ�̂���Ȃ�΁A�����I�ɌĂяo���K�v�͖����B
	*/

	CMutex();
	virtual ~CMutex();
protected:
	HANDLE	m_hMutex; // Mutex �n���h�� 
};

class IMutexSection {
public:
	virtual LRESULT	Enter(const string& strMutexName)=0;
	virtual void	Leave()=0;
	virtual ~IMutexSection(){}
};

class CMutexSection : public IMutexSection {
/**
		Mutex�ɂ��Lock�`unlock�ł�
		class CCriticalSection�Ɠ������o�Ŏg���܂�
*/
public:
	virtual LRESULT	Enter(const string& strMutexName);	///	MutexSection�ɓ���
	/**
		�Ԃ��l�F
�@�@�@�@�@�O�F����
		�@�P�F�����̃Z�}�t�H�A�~���[�e�b�N�X�A�ҋ@�\�ȃ^�C�}�A�W���u�A
		�t�@�C���}�b�s���O�I�u�W�F�N�g�̂����ꂩ�̖��O�ƈ�v�����̂ŃG���[
	*/

	virtual void	Leave();					///	MutexSection����o��

	/**
		�f�X�g���N�^�ł�CMutex���������̂ŁA���̂Ƃ���
		�����I��Mutex�͉�������B
	*/

protected:
	CMutex m_vMutex;
};

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd

#endif
