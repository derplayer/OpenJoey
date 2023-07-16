// yaneWaveStreamSound.h
//		programmed by ENRA		 '02/03/25

#ifndef __yaneWaveStreamSound_h__
#define __yaneWaveStreamSound_h__


#include "yaneSound.h"
#include "../Thread/yaneThread.h"
#include "../Thread/yaneCriticalSection.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class IWaveOutput;
class ISoundBuffer;
class ISoundStream;
class CWaveStreamSound : public IWaveSound, public CThread {
/**
	Wave�Đ��̂��߂� class ISound �h���N���X
	���ۂ�class CWaveSound��ʂ��ė��p����
	class CWaveStaticSound �ƈႢ�A�t�@�C����W�J���Ȃ���Đ�����
*/
public:
	CWaveStreamSound(IWaveOutput* p);
	virtual ~CWaveStreamSound();

	///	overriden from class ISound
	LRESULT Open(const string& strFileName) { return Load(strFileName); }
	LRESULT Close()	{ return Release(); }
	LRESULT Play();
	LRESULT Replay();
	LRESULT Stop();
	LRESULT Pause();
	bool	IsPlay() const;
	LRESULT	SetLoopPlay(bool bLoop);
	bool	IsLoopPlay() const;
	LONG	GetLength() const;
	LRESULT SetCurrentPos(LONG lPos);
	LONG	GetCurrentPos() const;
	LRESULT SetVolume(LONG volume);
	LONG	GetVolume() const;
	string	GetFileName() const { return m_strFileName; }
	int	GetType() const { return 4; }

	///	overriden from class IWaveSound
	LRESULT Restore();

	///	�t�@�C����ǂݍ���
	LRESULT Load(const string& strFileName);
	///	�t�@�C�������
	LRESULT Release();
	///	�Đ����g���̐ݒ�E�擾
	LRESULT SetFrequency(DWORD freq);
	DWORD	GetFrequency() const;

	//////////////////////////////////////////////////////////////////////////

protected:
	//	�o�b�t�@��Byte�T�C�Y�𓾂�
	inline DWORD	GetBufferSize() const { return eBufferLength * m_nAvgBytesPerSec; }

	//	Stop()�̉�����
	LRESULT InnerStop();

	//	Secondary Buffer��������
	LRESULT InitBuffer();
	//	Secondary Buffer�Ƀf�[�^�𗬂�����
	LRESULT GetNextSoundData();

	//	Accessor
	inline IWaveOutput*				GetOutput() const { return m_pOutput; }
	inline ICriticalSection*		GetCS() const { return const_cast<CCriticalSection*>(&m_vCS); }
	inline smart_ptr<ISoundStream>	GetStream() const { return m_vStream; }
	inline smart_ptr<ISoundBuffer>	GetBuffer() const { return m_vBuffer; }
	inline void	SetStream(const smart_ptr<ISoundStream>& p) { m_vStream = p; }
	inline void	SetBuffer(smart_ptr<ISoundBuffer> p) { m_vBuffer = p; }

	//	overriden from class CThread
	void	ThreadProc();

private:
	//	Secondary Buffer
	smart_ptr<ISoundBuffer>	m_vBuffer;	
	//	Sound Stream
	smart_ptr<ISoundStream>	m_vStream;
	//	Critical Section Object
	CCriticalSection		m_vCS;
	//	Wave Output Device
	IWaveOutput*			m_pOutput;

	volatile DWORD	m_dwPlayPosition;	//	�t�@�C�����̍Đ��ʒu��Byte�Ŏ���

	enum	{ eBufferLength = 3 };	//	�Z�J���_���o�b�t�@�̕b��

	DWORD	m_dwPrevWriteCursor;//	�O��̏����J�[�\��
	DWORD	m_dwNextWriteCursor;//	����̏����J�[�\��
	DWORD	m_dwWriteDiffer;	//	�O��̏������߂Ȃ������T�C�Y
	DWORD	m_dwPrevPlayCursor;	//	�O��̍Đ��J�[�\��

	string	m_strFileName;		//	�t�@�C����
	int		m_bPaused;			//	�T�E���hpause�����H
	volatile bool	m_bLoop;	//	���[�v���[�h�ōĐ�����̂��H
	DWORD	m_nAvgBytesPerSec;	//	�P�b�Ԃ̃f�[�^�o�C�g���i�Đ��ʒu�Z�o�p�j
	DWORD	m_dwLength;			//	�Ȃ̒���[�P��: Byte]
	LONG	m_lVolume;			//	Volume
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneWaveStreamSound_h__
