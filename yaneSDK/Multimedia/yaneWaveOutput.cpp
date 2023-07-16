#include "stdafx.h"

#include "yaneSound.h"
#include "yaneSoundBuffer.h"
#include "yaneSoundStream.h"
#include "yaneSoundParameter.h"
#include "yaneWaveOutput.h"

#include "yaneDirectSound.h"
#include "../Window/yaneWinHook.h"
#include "../AppFrame/yaneAppManager.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

// IWaveOutput
IWaveOutput::IWaveOutput()
{
	// default parameter
	m_pSoundParameter.Add();
}

LRESULT IWaveOutput::AddSound(IWaveSound* p)
{
	if (p->GetType()!=3) return 1;
	GetSoundList()->insert(p);

	return 0;
}

LRESULT IWaveOutput::DelSound(IWaveSound* p)
{
	return GetSoundList()->erase(p) ? 0 : 1;
}


// CWaveNullOutput
smart_ptr<ISoundBuffer> CWaveNullOutput::CreateBuffer()
{
	smart_ptr<ISoundBuffer> p;
	p.Add(new CNullSoundBuffer);

	return p;
}

// CWaveOutputDirectSound
CWaveOutputDirectSound::CWaveOutputDirectSound(CDirectSound* p) : m_pDirectSound(p)
{
#ifdef USE_EXCEPTION
	// NULL�ɂ��Ă������Ă킯����Ȃ�
	if (m_pDirectSound==NULL) { CSyntaxException("������NULL�Ȃ̂�CWaveOutputDirectSound�̃R���X�g���N�g���o���܂���"); }
#endif

	// �v���C�}�������(�������������͂��Ȃ�)
	m_pPrimary.Add(new CDirectSoundPrimaryBuffer(GetDirectSound()));

	// �v���C�}����Create���I����Ă��Ȃ�
	m_nPrimaryType = -1;

	// Hook�J�n
	CAppManager::Hook(this);
}

CWaveOutputDirectSound::~CWaveOutputDirectSound()
{
	// Hook�I��
	CAppManager::Unhook(this);
}

LRESULT CWaveOutputDirectSound::ChangePrimaryFormat(int nFrequency, int nBit, bool bStereo)
{
	// �ύX�����Ȃ琬���ɂ��Ă���
	int nType = ((bStereo)?1:0) + nBit*2 + nFrequency*4;
	if (m_nPrimaryType==nType) return 0;

	if (GetDirectSound()->GetStatus()==0){
		// �������x����ύX����
		if (GetDirectSound()->SetCooperativeLevel(DSSCL_PRIORITY)!=0) return 1;
		// ���������ĂȂ������珉��������
		if (m_nPrimaryType==-1) m_pPrimary->Create(NULL, 0, false);
		// �v���C�}���̃t�H�[�}�b�g��ς���
		WAVEFORMATEX pcmwf = {0};
		pcmwf.wFormatTag		= WAVE_FORMAT_PCM;
		pcmwf.nChannels			= (bStereo) ? 2 : 1;		// 1 or 2 channel
		pcmwf.wBitsPerSample	= (nBit + 1) * 8;			// 8 or 16 bits
		pcmwf.nSamplesPerSec	= (nFrequency + 1) * 11025; // 11KHz or 22KHz or 33KHz or 44KHz
		pcmwf.nBlockAlign		= 4;
		pcmwf.nAvgBytesPerSec	= pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
		if (m_pPrimary->SetFormat(&pcmwf)!=0) return 2;
		m_nPrimaryType = nType;

		// �Z�J���_�������X�g����͂�
		GetSoundList()->for_each(IWaveSound::Restore);
	}

	// �ǂ�����A�f�t�H���g�Ŏ��s�ɂ��Ƃ���
	return 3;
}

smart_ptr<ISoundBuffer> CWaveOutputDirectSound::CreateSecondery()
{
	smart_ptr<ISoundBuffer> p;
	if (GetDirectSound()->GetStatus()==0){
		p.Add(new CDirectSoundSeconderyBuffer(GetDirectSound()));
	}
	// �ʖڂȂ�NullDevice��������ł���
	if (p.isNull()){
		p.Add(new CNullSoundBuffer);
	}

	return p;
}

LRESULT CWaveOutputDirectSound::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg){
	case WM_ACTIVATEAPP:
		{
			UINT bActive = wParam;
			if(bActive) {
				GetSoundList()->for_each(IWaveSound::Restore);
			}
			return 0;
		}
	}
	return 0;
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
