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
	// バッファ無いやん
	if ( !GetBuffer() ) return 0;

	DWORD dwStatus;
	GetBuffer()->GetStatus(&dwStatus);

	// ロストしてるやん
	if ( dwStatus & DSBSTATUS_BUFFERLOST ) return 1;
	// 再生中
	ef ( dwStatus & DSBSTATUS_PLAYING ) return 3;

	// 止まってます
	return 2;
}

LRESULT CDirectSoundBuffer::Lock(DWORD dwWriteCursor, DWORD dwLockBytes, BYTE** lplpLockedBuffer1, DWORD* lpdwLockedBytes1, BYTE** lplpLockedBuffer2/*=NULL*/, DWORD* lpdwLockedBytes2/*=NULL*/)
{
	// バッファ無いやん
	if (!GetBuffer()) return 1;

	// DirectSoundバッファをLock
	HRESULT hr = GetBuffer()->Lock(dwWriteCursor, dwLockBytes,
									(void**)lplpLockedBuffer1, lpdwLockedBytes1,
									(void**)lplpLockedBuffer2, lpdwLockedBytes2, 0);
	// これは、実は、失敗することは多々有るのだ:p
	if ( hr==DSERR_BUFFERLOST ) {
		GetBuffer()->Restore(); // これでオッケ!（笑）
		hr = GetBuffer()->Lock(dwWriteCursor, dwLockBytes, 
								(void**)lplpLockedBuffer1, lpdwLockedBytes1,
								(void**)lplpLockedBuffer2, lpdwLockedBytes2, 0);
		// んで、もっかいリトライするの！！
	}

	if ( hr!=DS_OK ) {
		// これでダメなら、メモリ足りんのちゃう？
		return 2;
	}

	return 0;
}

LRESULT CDirectSoundBuffer::Unlock(BYTE* lpLockedBuffer1, DWORD dwLockedBytes1, BYTE* lpLockedBuffer2/*=NULL*/, DWORD dwLockedBytes2/*=0*/)
{
	// バッファ無いやん
	if ( !GetBuffer() ) return 1;

	// DirectSoundバッファのUnlock...
	if ( DS_OK!=GetBuffer()->Unlock(lpLockedBuffer1, dwLockedBytes1, lpLockedBuffer2, dwLockedBytes2) ) {
		// こんなんふつー、失敗するかぁ...どないせーちゅーんじゃ
		return 2;
	}

	return 0;
}

LRESULT CDirectSoundBuffer::Play(bool bLoop/*=false*/)
{
	// バッファ無いやん
	if (!GetBuffer()) return 1;
	HRESULT hr = GetBuffer()->Play(0, 0, ((bLoop) ? DSBPLAY_LOOPING : 0));

	// うはー、ロストしてる
	if (hr==DSERR_BUFFERLOST) return 2;

	if (hr!=DS_OK) return 3;
	return 0;
}

LRESULT CDirectSoundBuffer::Stop()
{
	// バッファ無いやん
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->Stop()) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::Restore()
{
	// バッファ無いやん
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->Restore()) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::SetFormat(LPCWAVEFORMATEX lpFormat)
{
	// バッファ無いやん
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->SetFormat(lpFormat)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::GetFormat(LPWAVEFORMATEX lpFormat) const
{
	// バッファ無いやん
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->GetFormat(lpFormat, sizeof(WAVEFORMATEX), NULL)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::SetCurrentPosition(DWORD dwPlayCursor)
{
	// バッファ無いやん
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->SetCurrentPosition(dwPlayCursor)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::GetCurrentPosition(DWORD& dwPlayCursor, DWORD& dwWriteCursor) const
{
	// バッファ無いやん
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->GetCurrentPosition(&dwPlayCursor, &dwWriteCursor)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::SetVolume(LONG lVolume)
{
	// バッファ無いやん
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->SetVolume(lVolume)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::GetVolume(LONG& lVolume) const
{
	// バッファ無いやん
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->GetVolume(&lVolume)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::SetPan(LONG lPan)
{
	// バッファ無いやん
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->SetPan(lPan)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::GetPan(LONG& lPan) const
{
	// バッファ無いやん
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->GetPan(&lPan)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::SetFrequency(DWORD dwFrequency)
{
	// バッファ無いやん
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->SetFrequency(dwFrequency)) return 2;
	return 0;
}

LRESULT CDirectSoundBuffer::GetFrequency(DWORD& dwFrequency) const
{
	// バッファ無いやん
	if (!GetBuffer()) return 1;
	if (DS_OK!=GetBuffer()->GetFrequency(&dwFrequency)) return 2;
	return 0;
}


// CDirectSoundPrimaryBuffer
LRESULT CDirectSoundPrimaryBuffer::Create(LPCWAVEFORMATEX lpWFormat, DWORD dwSize, bool bZeroClear/*=false*/, const CSoundParameter* pParam/*=NULL*/)
{
	DSBUFFERDESC dsbdDesc = { sizeof(DSBUFFERDESC) };
	// プライマリ
	dsbdDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

	// バッファ作れやー
	HRESULT ret = GetDirectSound()->Get()->CreateSoundBuffer(&dsbdDesc, &m_lpBuffer, NULL);

	// 失敗したー(;´Д`)
	if ( ret!=DS_OK||!GetBuffer() ) return 1;

	return 0;
}


// CDirectSoundSeconderyBuffer
LRESULT CDirectSoundSeconderyBuffer::Create(LPCWAVEFORMATEX lpWFormat, DWORD dwSize, bool bZeroClear/*=false*/, const CSoundParameter* pParam/*=NULL*/)
{
	DSBUFFERDESC dsbdDesc = { sizeof(DSBUFFERDESC) };
	//	デフォルトでボリュームコントロール能力を持つ
	dsbdDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_LOCSOFTWARE/* | DSBCAPS_GETCURRENTPOSITION2*/;
	//	グローバルフォーカスあり？
	if ( pParam&&pParam->IsGlobalFocus() ) dsbdDesc.dwFlags |= DSBCAPS_GLOBALFOCUS;
	//	パンコントロールあり？
	if ( pParam&&pParam->IsPanControl() ) dsbdDesc.dwFlags |= DSBCAPS_CTRLPAN;
	//	周波数コントロールあり？
	if ( pParam&&pParam->IsFrequencyControl() ) dsbdDesc.dwFlags |= DSBCAPS_CTRLFREQUENCY;

	//	LPWAVEFORMATEXとサイズを指定する
	dsbdDesc.lpwfxFormat = const_cast<LPWAVEFORMATEX>( lpWFormat );
	dsbdDesc.dwBufferBytes = dwSize;

	//	バッファ作れやー
	HRESULT ret = GetDirectSound()->Get()->CreateSoundBuffer(&dsbdDesc, &m_lpBuffer, NULL);
	if ( ret!=DS_OK||!GetBuffer() ) return 1;	//	失敗したー(;´Д`)

	//	ゼロクリアする
	if ( bZeroClear ) {
		BYTE* lpBuf = NULL;
		DWORD dwLockedSize = dsbdDesc.dwBufferBytes;
		ret = GetBuffer()->Lock(0, dwLockedSize, (void**)&lpBuf, &dwLockedSize, NULL, NULL, 0);
		if ( ret!=DS_OK ) return 2;		//	失敗したー(;´Д`)
		memset(lpBuf, 0, dwLockedSize);
		ret = GetBuffer()->Unlock(lpBuf, dwLockedSize, NULL, NULL);
		if ( ret!=DS_OK ) return 3;		//	失敗したー(;´Д`)
	}

	return 0;
}

smart_ptr<ISoundBuffer> CDirectSoundSeconderyBuffer::Clone()
{
	smart_ptr<CDirectSoundSeconderyBuffer> pNewBuffer(new CDirectSoundSeconderyBuffer(GetDirectSound()));

	//	DirectSoundにDuplicateしてもらう
	HRESULT ret = GetDirectSound()->Get()->DuplicateSoundBuffer(GetBuffer(), &(pNewBuffer->m_lpBuffer));

	//	うまいこといってるかな？
	if ( ret==DS_OK&&pNewBuffer->GetBuffer()!=NULL ) {
		smart_ptr<ISoundBuffer> tmp;
		tmp.UpCast(pNewBuffer);
		return tmp;
	}

	//	失敗したー(;´Д`)
	return smart_ptr<ISoundBuffer>(new CNullSoundBuffer);
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
