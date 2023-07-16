#include "stdafx.h"
#include "../Multimedia/yaneDirectSound.h"
#include "../Multimedia/yaneSoundParameter.h"
#include "../Multimedia/yaneSoundBuffer.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

//////////////////////////////////////////////////////////////////////////////

// CDirectSoundBuffer
CDirectSoundBuffer::CDirectSoundBuffer(CDirectSound* p) : m_lpDirectSound(p)
{
	m_lpBuffer = NULL;
}

CDirectSoundBuffer::~CDirectSoundBuffer()
{
	RELEASE_SAFE(m_lpBuffer);
}

LRESULT CDirectSoundBuffer::GetStatus() const
{
	// �o�b�t�@�������
	if ( !GetBuffer() ) return 0;

	DWORD dwStatus;
	GetBuffer()->GetStatus(&dwStatus);

	// ���X�g���Ă���
	if ( dwStatus & DSBSTATUS_BUFFERLOST ) return 1;
	// �Đ���
	ef ( dwStatus & DSBSTATUS_PLAYING ) return 3;

	// �~�܂��Ă܂�
	return 2;
}

LRESULT CDirectSoundBuffer::Lock(DWORD dwWriteCursor, DWORD dwLockBytes, BYTE** lplpLockedBuffer1, DWORD* lpdwLockedBytes1, BYTE** lplpLockedBuffer2/*=NULL*/, DWORD* lpdwLockedBytes2/*=NULL*/)
{
	// �o�b�t�@�������
	if (!GetBuffer()) return 1;

	// DirectSound�o�b�t�@��Lock
	HRESULT hr = GetBuffer()->Lock(dwWriteCursor, dwLockBytes,
									(void**)lplpLockedBuffer1, lpdwLockedBytes1,
									(void**)lplpLockedBuffer2, lpdwLockedBytes2, 0);
	// ����́A���́A���s���邱�Ƃ͑��X�L��̂�:p
	if ( hr==DSERR_BUFFERLOST ) {
		GetBuffer()->Restore(); // ����ŃI�b�P!�i�΁j
		hr = GetBuffer()->Lock(dwWriteCursor, dwLockBytes, 
								(void**)lplpLockedBuffer1, lpdwLockedBytes1,
								(void**)lplpLockedBuffer2, lpdwLockedBytes2, 0);
		// ��ŁA�����������g���C����́I�I
	}

	if ( hr!=DS_OK ) {
		// ����Ń_���Ȃ�A�����������̂��Ⴄ�H
		return 2;
	}

	return 0;
}

LRESULT CDirectSoundBuffer::Unlock(BYTE* lpLockedBuffer1, DWORD dwLockedBytes1, BYTE* lpLockedBuffer2/*=NULL*/, DWORD dwLockedBytes2/*=0*/)
{
	// �o�b�t�@�������
	if ( !GetBuffer() ) return 1;

	// DirectSound�o�b�t�@��Unlock...
	if ( DS_OK!=GetBuffer()->Unlock(lpLockedBuffer1, dwLockedBytes1, lpLockedBuffer2, dwLockedBytes2) ) {
		// ����Ȃ�ӂ[�A���s���邩��...�ǂȂ����[����[�񂶂�
		return 2;
	}

	return 0;
}

LRESULT CDirectSoundBuffer::Play(bool bLoop/*=false*/)
{
	// �o�b�t�@�������
	if (!GetBuffer()) return 1;
	HRESULT hr = GetBuffer()->Play(0, 0, ((bLoop) ? DSBPLAY_LOOPING : 0));

	// ���́[�A���X�g���Ă�
	if (hr==DSERR_BUFFERLOST) return 2;

	if (hr!=DS_OK) return 3;
	return 0;
}

LRESULT CDirectSoundBuffer::Stop()
{
	// �o�b�t�@�������
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->Stop()) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::Restore()
{
	// �o�b�t�@�������
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->Restore()) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::SetFormat(LPCWAVEFORMATEX lpFormat)
{
	// �o�b�t�@�������
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->SetFormat(lpFormat)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::GetFormat(LPWAVEFORMATEX lpFormat) const
{
	// �o�b�t�@�������
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->GetFormat(lpFormat, sizeof(WAVEFORMATEX), NULL)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::SetCurrentPosition(DWORD dwPlayCursor)
{
	// �o�b�t�@�������
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->SetCurrentPosition(dwPlayCursor)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::GetCurrentPosition(DWORD& dwPlayCursor, DWORD& dwWriteCursor) const
{
	// �o�b�t�@�������
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->GetCurrentPosition(&dwPlayCursor, &dwWriteCursor)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::SetVolume(LONG lVolume)
{
	// �o�b�t�@�������
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->SetVolume(lVolume)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::GetVolume(LONG& lVolume) const
{
	// �o�b�t�@�������
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->GetVolume(&lVolume)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::SetPan(LONG lPan)
{
	// �o�b�t�@�������
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->SetPan(lPan)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::GetPan(LONG& lPan) const
{
	// �o�b�t�@�������
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->GetPan(&lPan)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::SetFrequency(DWORD dwFrequency)
{
	// �o�b�t�@�������
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->SetFrequency(dwFrequency)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::GetFrequency(DWORD& dwFrequency) const
{
	// �o�b�t�@�������
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->GetFrequency(&dwFrequency)) return 2;
	return 0;
}


// CDirectSoundPrimaryBuffer
LRESULT CDirectSoundPrimaryBuffer::Create(LPCWAVEFORMATEX lpWFormat, DWORD dwSize, bool bZeroClear/*=false*/, const CSoundParameter* pParam/*=NULL*/)
{
	DSBUFFERDESC dsbdDesc = { sizeof(DSBUFFERDESC) };
	// �v���C�}��
	dsbdDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

	// �o�b�t�@����[
	HRESULT ret = GetDirectSound()->Get()->CreateSoundBuffer(&dsbdDesc, &m_lpBuffer, NULL);

	// ���s�����[(;�L�D`)
	if ( ret!=DS_OK||!GetBuffer() ) return 1;

	return 0;
}


// CDirectSoundSeconderyBuffer
LRESULT CDirectSoundSeconderyBuffer::Create(LPCWAVEFORMATEX lpWFormat, DWORD dwSize, bool bZeroClear/*=false*/, const CSoundParameter* pParam/*=NULL*/)
{
	DSBUFFERDESC dsbdDesc = { sizeof(DSBUFFERDESC) };
	//	�f�t�H���g�Ń{�����[���R���g���[���\�͂�����
	dsbdDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_LOCSOFTWARE/* | DSBCAPS_GETCURRENTPOSITION2*/;
	//	�O���[�o���t�H�[�J�X����H
	if ( pParam&&pParam->IsGlobalFocus() ) dsbdDesc.dwFlags |= DSBCAPS_GLOBALFOCUS;
	//	�p���R���g���[������H
	if ( pParam&&pParam->IsPanControl() ) dsbdDesc.dwFlags |= DSBCAPS_CTRLPAN;
	//	���g���R���g���[������H
	if ( pParam&&pParam->IsFrequencyControl() ) dsbdDesc.dwFlags |= DSBCAPS_CTRLFREQUENCY;

	//	LPWAVEFORMATEX�ƃT�C�Y���w�肷��
	dsbdDesc.lpwfxFormat = const_cast<LPWAVEFORMATEX>( lpWFormat );
	dsbdDesc.dwBufferBytes = dwSize;

	//	�o�b�t�@����[
	HRESULT ret = GetDirectSound()->Get()->CreateSoundBuffer(&dsbdDesc, &m_lpBuffer, NULL);
	if ( ret!=DS_OK||!GetBuffer() ) return 1;	//	���s�����[(;�L�D`)

	//	�[���N���A����
	if ( bZeroClear ) {
		BYTE* lpBuf = NULL;
		DWORD dwLockedSize = dsbdDesc.dwBufferBytes;
		ret = GetBuffer()->Lock(0, dwLockedSize, (void**)&lpBuf, &dwLockedSize, NULL, NULL, 0);
		if ( ret!=DS_OK ) return 2;		//	���s�����[(;�L�D`)
		memset(lpBuf, 0, dwLockedSize);
		ret = GetBuffer()->Unlock(lpBuf, dwLockedSize, NULL, NULL);
		if ( ret!=DS_OK ) return 3;		//	���s�����[(;�L�D`)
	}

	return 0;
}

smart_ptr<ISoundBuffer> CDirectSoundSeconderyBuffer::Clone()
{
	smart_ptr<CDirectSoundSeconderyBuffer> pNewBuffer(new CDirectSoundSeconderyBuffer(GetDirectSound()));

	//	DirectSound��Duplicate���Ă��炤
	HRESULT ret = GetDirectSound()->Get()->DuplicateSoundBuffer(GetBuffer(), &(pNewBuffer->m_lpBuffer));

	//	���܂����Ƃ����Ă邩�ȁH
	if ( ret==DS_OK&&pNewBuffer->GetBuffer()!=NULL ) {
		smart_ptr<ISoundBuffer> tmp;
		tmp.UpCast(pNewBuffer);
		return tmp;
	}

	//	���s�����[(;�L�D`)
	return smart_ptr<ISoundBuffer>(new CNullSoundBuffer);
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
