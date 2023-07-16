// yaneSoundBuffer.h
//		programmed by ENRA		'02/03/23

#ifndef __yaneSoundBuffer_h__
#define __yaneSoundBuffer_h__


namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class CSoundParameter;
class ISoundBuffer {
/**
	SoundBuffer�𒊏ۉ������C���^�[�t�F�[�X�N���X
	class CDirectSoundBuffer, class CNullSoundBuffer �͂�������h��
*/
public:
	ISoundBuffer(){};
	virtual ~ISoundBuffer(){};

	virtual int GetType() const = 0;
	/**
		RTTI���ǂ��B�h���N���X�̃^�C�v��Ԃ�
		0 : class CNullSoundBuffer
		1 : class CDirectSoundBuffer
		2 : ���񂪂����イ
	*/

	/// �o�b�t�@���쐬����
	/// [in] lpWFormat �@- WAVEFORMATEX�ւ̃|�C���^���w�肷��
	/// [in] dwLockBytes - �o�b�t�@�̃T�C�Y���w�肷��
	/// [in] bZeroClear�@- �o�b�t�@�̒��g���[���N���A���邩�ǂ���
	/// [in] pParam�@�@�@- CSoundParameter�ւ̃|�C���^���w�肷��
	virtual LRESULT Create(LPCWAVEFORMATEX lpWFormat, DWORD dwSize, bool bZeroClear=false, const CSoundParameter* pParam=NULL) = 0;

	/// �����̕�����Ԃ�
	virtual smart_ptr<ISoundBuffer> Clone() = 0;

	/// �o�b�t�@�̌��݂̏�Ԃ𓾂�
	/// �@0 - �o�b�t�@������
	/// �@1 - �o�b�t�@�����X�g���Ă���
	/// �@2 - ��~��
	/// �@3 - �Đ���
	virtual LRESULT GetStatus() const = 0;

	/// �o�b�t�@�̃t�H�[�}�b�g��WAVEFORMATEX�Ƃ��āA�ݒ�E�擾����
	virtual LRESULT SetFormat(LPCWAVEFORMATEX lpFormat) = 0;
	virtual LRESULT GetFormat(LPWAVEFORMATEX lpFormat) const = 0;

	/// �o�b�t�@�̍Đ��|�W�V������ݒ�E�擾����
	/// [in/out] dwPlayCursor�@- �Đ��ʒu���w�肷��/�Đ��ʒu���Ԃ�
	/// [out]�@�@dwWriteCursor - �������݈ʒu���Ԃ�
	virtual LRESULT SetCurrentPosition(DWORD dwPlayCursor) = 0;
	virtual LRESULT GetCurrentPosition(DWORD& dwPlayCursor, DWORD& dwWriteCursor) const = 0;

	/// �o�b�t�@�̃{�����[����ݒ�E�擾����
	/// [in/out] lVolume - �{�����[���l(0�`-10000)
	virtual LRESULT SetVolume(LONG lVolume) = 0;
	virtual LRESULT GetVolume(LONG& lVolume) const = 0;

	/// �o�b�t�@�̃p����ݒ�E�擾����
	/// [in/out] lPan - �p���l(-100000�`+10000)
	virtual LRESULT SetPan(LONG lPan) = 0;
	virtual LRESULT GetPan(LONG& lPan) const = 0;

	/// �o�b�t�@�̍Đ����g����ݒ�E�擾����
	/// [in/out] dwFrequency - ���g���l(100�`100000)
	virtual LRESULT SetFrequency(DWORD dwFrequency) = 0;
	virtual LRESULT GetFrequency(DWORD& dwFrequency) const = 0;

	/// �o�b�t�@��Lock�EUnlock����
	/// [in]�@dwWriteCursor �@- Lock����ʒu���w�肷��
	/// [in]�@dwLockBytes �@�@- Lock����T�C�Y���w�肷��
	/// [out] lpLockedBuffer1 - Lock���ꂽ�o�b�t�@�ւ̃|�C���^���Ԃ�
	/// [out] dwLockedBytes1�@- Lock���ꂽ�T�C�Y���Ԃ�
	/// [out] lpLockedBuffer2 - Lock�̈悪�o�b�t�@�̈���݂͂ł��ꍇ�A�o�b�t�@�擪�ւ̃|�C���^���Ԃ�
	/// [out] dwLockedBytes2�@- �͂ݏo�����̗̈�̃T�C�Y���Ԃ�
	virtual LRESULT Lock(DWORD dwWriteCursor, DWORD dwLockBytes, BYTE** lplpLockedBuffer1, DWORD* lpdwLockedBytes1, BYTE** lplpLockedBuffer2=NULL, DWORD* lpdwLockedBytes2=NULL) = 0;
	virtual LRESULT Unlock(BYTE* lpLockedBuffer1, DWORD dwLockedBytes1, BYTE* lpLockedBuffer=NULL, DWORD dwLockedBytes2=0) = 0;

	/// �o�b�t�@���Đ�����
	/// [in] bLoop - ���[�v�Đ�����ꍇ��true���w�肷��
	virtual LRESULT Play(bool bLoop = false) = 0;

	/// �o�b�t�@���~����
	virtual LRESULT Stop() = 0;

	/// �o�b�t�@�����X�g���Ă����烊�X�g�A����
	virtual LRESULT Restore() = 0;
};

class CDirectSound;
class CDirectSoundBuffer : public ISoundBuffer {
/**
	DirectSound�ɓ������� class ISoundBuffer �h���N���X
	class CDirectSoundPrimaryBuffer �@��
	class CDirectSoundSeconderyBuffer �́A��������h��

	[���ӓ_]�@DirectSound�́A�{�����[���l��-1000���w�肵���Ƃ���ƁA
	�@�@�@�@�@-10dB�̃{�����[���l�ɂȂ�B
*/
public:
	CDirectSoundBuffer(CDirectSound* lpDirectSound);
	virtual ~CDirectSoundBuffer();

	/// override from class ISoundBuffer
	virtual int GetType() const { return 1; }
	virtual LRESULT Create(LPCWAVEFORMATEX lpWFormat, DWORD dwSize, bool bZeroClear=false, const CSoundParameter* pParam=NULL) = 0;
	virtual smart_ptr<ISoundBuffer> Clone() = 0;
	virtual LRESULT GetStatus() const;
	virtual LRESULT SetFormat(LPCWAVEFORMATEX lpFormat);
	virtual LRESULT GetFormat(LPWAVEFORMATEX lpFormat) const;
	virtual LRESULT SetCurrentPosition(DWORD dwPlayCursor);
	virtual LRESULT GetCurrentPosition(DWORD& dwPlayCursor, DWORD& dwWriteCursor) const;
	virtual LRESULT SetVolume(LONG lVolume);
	virtual LRESULT GetVolume(LONG& lVolume) const;
	virtual LRESULT SetPan(LONG lPan);
	virtual LRESULT GetPan(LONG& lPan) const;
	virtual LRESULT SetFrequency(DWORD dwFrequency);
	virtual LRESULT GetFrequency(DWORD& dwFrequency) const;
	virtual LRESULT Lock(DWORD dwWriteCursor, DWORD dwLockBytes, BYTE** lplpLockedBuffer1, DWORD* lpdwLockedBytes1, BYTE** lplpLockedBuffer2=NULL, DWORD* lpdwLockedBytes2=NULL);
	virtual LRESULT Unlock(BYTE* lpLockedBuffer, DWORD dwLockedBytes, BYTE* lpLockedBuffer2=NULL, DWORD dwLockedBytes2=0);
	virtual LRESULT Play(bool bLoop = false);
	virtual LRESULT Stop();
	virtual LRESULT Restore();

protected:
	/// LPDIRECTSOUNDBUFFER��Ԃ�
	LPDIRECTSOUNDBUFFER GetBuffer() const { return m_lpBuffer; }
	LPDIRECTSOUNDBUFFER m_lpBuffer;

	/// CDirectSound��Ԃ�
	CDirectSound* GetDirectSound() const { return m_lpDirectSound; }
	CDirectSound* m_lpDirectSound;
};

class CDirectSoundPrimaryBuffer : public CDirectSoundBuffer {
/**
	DirectSoundBuffer��PrimaryBuffer�̂��߂̃N���X
*/
public:
	CDirectSoundPrimaryBuffer(CDirectSound* lpDirectSound) : CDirectSoundBuffer(lpDirectSound) {};
	virtual ~CDirectSoundPrimaryBuffer() {};

	/// override from class ISoundBuffer
	/// �����͑S�Ė��������
	virtual LRESULT Create(LPCWAVEFORMATEX lpWFormat, DWORD dwSize, bool bZeroClear=false, const CSoundParameter* pParam=NULL);

	/// override from class ISoundBuffer
	virtual smart_ptr<ISoundBuffer> Clone() { return smart_ptr<ISoundBuffer>(); }
};

class CDirectSoundSeconderyBuffer : public CDirectSoundBuffer {
/**
	DirectSoundBuffer��SeconderyBuffer�̂��߂̃N���X
*/
public:
	CDirectSoundSeconderyBuffer(CDirectSound* lpDirectSound) : CDirectSoundBuffer(lpDirectSound) {};
	virtual ~CDirectSoundSeconderyBuffer() {};

	/// override from class ISoundBuffer
	virtual LRESULT Create(LPCWAVEFORMATEX lpWFormat, DWORD dwSize, bool bZeroClear=false, const CSoundParameter* pParam=NULL);
	virtual smart_ptr<ISoundBuffer> Clone();
};

class CNullSoundBuffer : public ISoundBuffer {
/**
	class ISoundBuffer ��NullDevice�N���X
*/
public:
	CNullSoundBuffer(){};
	virtual ~CNullSoundBuffer(){};

	/// override from class ISoundBuffer
	virtual int GetType() const { return 0; }
	virtual LRESULT Create(LPCWAVEFORMATEX lpWFormat, DWORD dwSize, bool bZeroClear=false, const CSoundParameter* pParam=NULL) { return -1; }
	virtual smart_ptr<ISoundBuffer> Clone() { return smart_ptr<ISoundBuffer>(new CNullSoundBuffer); }
	virtual LRESULT GetStatus() const { return -1; }
	virtual LRESULT SetFormat(LPCWAVEFORMATEX lpFormat) { return -1; }
	virtual LRESULT GetFormat(LPWAVEFORMATEX lpFormat) const { return -1; }
	virtual LRESULT SetCurrentPosition(DWORD dwPlayCursor) { return -1; }
	virtual LRESULT GetCurrentPosition(DWORD& dwPlayCursor, DWORD& dwWriteCursor) const { return -1; }
	virtual LRESULT SetVolume(LONG lVolume) { return -1; }
	virtual LRESULT GetVolume(LONG& lVolume) const { return -1; }
	virtual LRESULT SetPan(LONG lPan) { return -1; }
	virtual LRESULT GetPan(LONG& lPan) const { return -1; }
	virtual LRESULT SetFrequency(DWORD dwFrequency) { return -1; }
	virtual LRESULT GetFrequency(DWORD& dwFrequency) const { return -1; }
	virtual LRESULT Lock(DWORD dwWriteCursor, DWORD dwLockBytes, BYTE** lplpLockedBuffer1, DWORD* lpdwLockedBytes1, BYTE** lplpLockedBuffer2=NULL, DWORD* lpdwLockedBytes2=NULL) { return -1; }
	virtual LRESULT Unlock(BYTE* lpLockedBuffer1, DWORD dwLockedBytes1, BYTE* lpLockedBuffer2=NULL, DWORD dwLockedBytes2=0) { return -1; }
	virtual LRESULT Play(bool bLoop = false) { return -1; }
	virtual LRESULT Stop() { return -1; }
	virtual LRESULT Restore() { return -1; }
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneSoundBuffer_h__
