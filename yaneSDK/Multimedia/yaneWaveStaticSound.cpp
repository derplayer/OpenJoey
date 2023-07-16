//	 re-programmed by yaneurao(M.Isozaki) '99/07/07
//	 re-programmed by ENRA				  '02/03/23

#include "stdafx.h"
#include "../Auxiliary/yaneFile.h"

#include "yaneWaveOutput.h"
#include "yaneSoundParameter.h"
#include "yaneSoundBuffer.h"
#include "yaneSoundStream.h"

#include "yaneWaveStaticSound.h"
//#include "../Window/yaneDebugWindow.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

#define sign(val) (val>0?1:(val<0?-1:0))	//	������Ԃ�

//////////////////////////////////////////////////////////////////////////

CWaveStaticSound::CWaveStaticSound(IWaveOutput* p) : m_pOutput(p)
{
	m_bPaused = 0;			//	�Q�ƃJ�E���g����
	m_nAvgBytesPerSec = 0;

	// �f�t�H���g�Ń��[�v���Ȃ�
	SetLoopPlay(false);

	// NullSoundBuffer��������ǂ�
	SetBuffer(smart_ptr<ISoundBuffer>(new CNullSoundBuffer));

	// �ʓ|���Ă�������
	GetOutput()->AddSound(this);
}

CWaveStaticSound::~CWaveStaticSound(){
	Release();

	// ����Ȃ�[
	GetOutput()->DelSound(this);
}

/*
//////////////////////////////////////////////////////////////////////////
//--- �C�� '02/01/10  by ENRA ---
LRESULT CWaveStaticSound::Load(string filename){
	DWORD dwDataLen = 0;

	// ���̊֐��ADirectSound�̂Ȃ��ŁA����������ԂȂ񂿂Ⴄ�H
	if (GetDirectSound() == NULL) return 1;	// ����ꂳ��I

	Release(); // �܂��́A������悤�ˁI
	m_szFileName.erase();
	if (filename.empty()) return 0;

	// ���΃p�X����
	filename = CFile::MakeFullName(filename);

	// Factory����ւ���(;�L�D`)
	if(m_lpReaderFactory==NULL){ m_lpReaderFactory.Add(); }
	// �I�[�v���ł������邩�[�H
	mtknLib::IkmPCMstream* pReader = m_lpReaderFactory->CreateReader(filename.c_str());
	if (pReader==NULL) {
		Err.Out("CWaveStaticSound::Load " + filename + "�̃t�@�C���ǂݍ��ݎ��̃G���[");
		return 1; // �t�@�C���ǂݍ��݃G���[
	}

	// �K�v�ȃo�b�t�@���𓾂�
	dwDataLen = pReader->GetPCMLength();

	// ����Ƃ�DirectSound�̏�����
	DSBUFFERDESC  dsbdDesc;
	ZeroMemory(&dsbdDesc, sizeof(DSBUFFERDESC));
	dsbdDesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY; // �{�����[�������Ō��\�I�I
	// dsbdDesc.dwFlags = DSBCAPS_LOCSOFTWARE;
	// ��ʂ̃f�[�^�������ꍇ�A�\�t�g�������ł�����ق�����肪���Ȃ��̂���
	// �i�n�[�h�E�F�A�ɂ���Ƃ������ă~�L�V���O�Ɏ��Ԃ�������\��������j
	// �����������Ȃ�\������B
	// ���ƁA�{�����[���R���g���[���\�͂��������Ă����K�v����B
	// �n�[�h�E�F�A�o�b�t�@���R���g���[���\�͂����ƁA
	// �����Ɏ��Ԃ������邩��...

	// | DSBCAPS_GETCURRENTPOSITION2;
	// ���������ƁA���t�����ǂ����̃X�e�[�^�X�������x���Ȃ�̂�
	// �X�e�[�^�X�����Ă̘A���Đ����ł��Ȃ��Ȃ�I

	// | DSBCAPS_GLOBALFOCUS
	// ����������ƁABackGround�ł������葱����

	dsbdDesc.dwBufferBytes = dwDataLen;
	dsbdDesc.lpwfxFormat = pReader->GetFormat();

	// DirectSound�o�b�t�@�̍쐬
	if (GetDirectSound()->CreateSoundBuffer(&dsbdDesc,&m_lpDSBuffer, NULL) != DS_OK){
		//	���g���ύX�̃R���g���[�����܂��������񂩁H
		dsbdDesc.dwFlags = DSBCAPS_CTRLVOLUME; // �{�����[�������Ȃ�ǂ���I�H
		if (GetDirectSound()->CreateSoundBuffer(&dsbdDesc,&m_lpDSBuffer, NULL) != DS_OK){
			Err.Out("CWaveStaticSound::Load��CreateSoundBuffer()�Ŏ��s�I");
			return 7;
		}
	}

	// DirectSound�o�b�t�@��Lock
	BYTE* lpDSBuffData;
	HRESULT hr;
	hr = m_lpDSBuffer->Lock(0, dwDataLen, (void**)&lpDSBuffData,
		&dwDataLen, NULL, 0, 0);
		// ����́A���́A���s���邱�Ƃ͑��X�L��̂�:p

	if (hr==DSERR_BUFFERLOST){
		m_lpDSBuffer->Restore(); // ����ŃI�b�P!�i�΁j
		hr = m_lpDSBuffer->Lock(0, dwDataLen, (void**)&lpDSBuffData,
			&dwDataLen, NULL, 0, 0);
		// ��ŁA�����������g���C����́I�I
	}

	if (hr!=DS_OK) {
		// ����Ń_���Ȃ�A�����������̂��Ⴄ�H
		Err.Out("CWaveStaticSound::Load��Lock()�Ɏ��s�I");
		return 8;
	}

	// �t�@�C����W�J
	pReader->SetLoop(false);
	pReader->Read(lpDSBuffData, dsbdDesc.dwBufferBytes);
	// DLL����o�����Ȃ̂�smart_ptr�͎g�����Ⴞ�߂��߁B
	pReader->DeleteSelf();  pReader=NULL;

	// DirectSound�o�b�t�@��Unlock...
	if (m_lpDSBuffer->Unlock(lpDSBuffData, dwDataLen, NULL, 0)
		!= DS_OK) {
		// ����Ȃ�ӂ[�A���s���邩��...�ǂȂ����[����[�񂶂�
		Err.Out("CWaveStaticSound::Load��Unlock()�Ɏ��s�I");
		return 10;
	}

	//	�b�Ԃ̍Đ��b��
	WAVEFORMATEX wfe;
	if( (m_lpDSBuffer->GetFormat( &wfe, sizeof(wfe), NULL )) == DS_OK &&
		(wfe.nAvgBytesPerSec)){
		m_nAvgBytesPerSec = wfe.nAvgBytesPerSec;
	} else {
		//	���������Ȃ��B����G���[�ɂȂ�񂩂��H
		m_nAvgBytesPerSec = 0;
	}

	m_szFileName = filename;
	m_lLength = LONG(((DWORDLONG)dwDataLen * 1000) / m_nAvgBytesPerSec);

	return 0;
}
//-------------------------------
*/

LRESULT CWaveStaticSound::Load(const string& filename){
	Release(); // �܂��́A������悤�ˁI
	m_strFileName.erase();
	if (filename.empty()) return 0;

	smart_ptr<ISoundStream> stream = GetOutput()->GetSoundParameter()->GetStreamFactory()->Open(filename);
	LPCWAVEFORMATEX pWFormat = stream->GetFormat();
	if (pWFormat==NULL) return 0;
	const DWORD dwDataLen = (LONG)((DWORDLONG)stream->GetLength() * pWFormat->nAvgBytesPerSec /1000);
	// DirectSound�o�b�t�@�̍쐬
	SetBuffer(GetOutput()->CreateBuffer());
	if (GetBuffer()->Create(pWFormat, dwDataLen, false, GetOutput()->GetSoundParameter().get()) != 0){
		Err.Out("CWaveStaticSound::Load �T�E���h�o�b�t�@�̍쐬�Ɏ��s�I");
		// NullSoundBuffer��������ǂ�
		SetBuffer(smart_ptr<ISoundBuffer>(new CNullSoundBuffer));
		return 1;
	}

	// DirectSound�o�b�t�@��Lock
	BYTE* lpLockedBuffer;
	DWORD dwLockedSize;
	if (GetBuffer()->Lock(0, dwDataLen, &lpLockedBuffer, &dwLockedSize) != 0) {
		// ����Ń_���Ȃ�A�����������̂��Ⴄ�H
		Err.Out("CWaveStaticSound::Load GetBuffer()->Lock()�Ɏ��s�I");
		return 2;
	}

	// �ǂݍ��݁`
	const DWORD dwReadedSize = stream->Read(lpLockedBuffer, dwLockedSize);
	if ( dwReadedSize > dwLockedSize ) {	//	�G���[�ƍl������
		Err.Out("CWaveStaticSound::Load stream->Read()�Ɏ��s�I");
		//	�Y�ꂸ��Unlock����Ƃ�
		GetBuffer()->Unlock(lpLockedBuffer, dwLockedSize);
		return 3;
	}

	// DirectSound�o�b�t�@��Unlock...
	if (GetBuffer()->Unlock(lpLockedBuffer, dwLockedSize) != 0) {
		// ����Ȃ�ӂ[�A���s���邩��...�ǂȂ����[����[�񂶂�
		Err.Out("CWaveStaticSound::Load GetBuffer()->Unlock()�Ɏ��s�I");
		return 4;
	}

	//	�b�Ԃ̍Đ��b��
	WAVEFORMATEX wfe;
	if( GetBuffer()->GetFormat(&wfe)==0 ) {
		m_nAvgBytesPerSec = wfe.nAvgBytesPerSec;
	} else {
		//	���������Ȃ��B����G���[�ɂȂ�񂩂��H
		m_nAvgBytesPerSec = 0;
	}

	m_strFileName = filename;
	m_lLength = (LONG)( ((DWORDLONG)dwDataLen*1000)/m_nAvgBytesPerSec );

	return 0;
}

LRESULT CWaveStaticSound::Release(){
	if (IsPlay()) Stop();
	// �Đ����Ȃ�Ƃ߂�Ƃ������I �i�ꉞ�ˁj

	// NullSoundBuffer��������ǂ�
	SetBuffer(smart_ptr<ISoundBuffer>(new CNullSoundBuffer));
	m_strFileName.erase();

	m_nAvgBytesPerSec = 0;

	return 0;
}

LRESULT CWaveStaticSound::Restore(){
	DWORD dwStatus = GetBuffer()->GetStatus();
	if (dwStatus==1){
		GetBuffer()->Restore();
		if (m_strFileName!="") {
			const string temp(m_strFileName); // �R�s�[���ēn�����Release���ꂿ�Ⴄ:p
			return Load(temp);
		}
	}

	return 0;
}

LRESULT CWaveStaticSound::Play()
{
	m_bPaused = 0;

	// �Đ����Ȃ�A�~�߂Ăł��Đ����Ȃ����B
	// �{���Ȃ�A���t���Ȃ�~�߂��ɃJ�����g�|�W�V���������ύX���Ă�����
	// �ǂ��͂��Ȃ̂����A���̏u�Ԃ�PLAY���I������Ƃ����\��������B
	// �i�Ȃ񂩁ADirectSound�̃o�O�̂悤�ȋC�����Ȃ����Ȃ��j

	// �����ŁA�J�����g�|�W�V������擪�ɖ߂��āA�v���C���~�܂���
	// ����΁A�Đ��Ƃ����A���S���Y���ɕύX����B
	
	GetBuffer()->SetCurrentPosition(0);
	// ���ɖ߂��Ă���̂ف[��Play���萳�m��� (C) yaneurao
	if (GetBuffer()->GetStatus()==3) {		
		GetBuffer()->Stop(); // �ˑR�~�߂��班���m�C�Y���邯�ǂȁB�d���Ȃ���ȁB
		GetBuffer()->SetCurrentPosition(0);
	}

	if (GetBuffer()->Play(m_bLoop)!=0){
		if (GetBuffer()->GetStatus()==1) {		
			// ���炢��������B�o�b�t�@LOST�������Brestore����i���I
			return Restore();
		}
		Err.Out("CWaveStaticSound::Play GetBuffer()->Play()�Ɏ��s�I");
		return 1;
	}

	return 0;
}

	//////////////////////////////////////////////////////////////////////////

LRESULT CWaveStaticSound::Stop()
{
	if (!IsPlay()) return 0; // �����~�܂��Ă������[�ɁI

	LRESULT ret = GetBuffer()->Stop();
	GetBuffer()->SetCurrentPosition(0); // ���ǂ��Ƃ��ȁI

	return ret;
}

bool CWaveStaticSound::IsPlay() const
// ���̃`�����l�����Đ���(=3)�Ȃ�true
{
	return GetBuffer()->GetStatus()==3;
}

LRESULT CWaveStaticSound::Pause()
 // ����`�����l����pause
{
	m_bPaused += sign(m_bPaused);	//	�K�E�Z:p
	if (!IsPlay()) {
		return 0; // �����I����Ă������[�ɁI
	}
	//	�Đ����̂�pause������Ȃ��
	m_bPaused = 1;

	// �����ɔ����Ă���������[���Ƃ́Am_lpDSBuffer!=NULL
	return GetBuffer()->Stop();
}

LRESULT CWaveStaticSound::Replay()
// pause�Ŏ~�߂Ă����A���̃`�����l���̍Đ����ĊJ�B
{
	if (!m_bPaused) return 0; // pause���Ăւ�āI
	
	if (--m_bPaused==0) {	//	�Q�ƃJ�E���g����
		return GetBuffer()->Play(m_bLoop);
	}
	return 0;
}

LRESULT CWaveStaticSound::SetVolume(LONG volume)
{
	// �͈͊O�̃G���[�`�F�b�N���[�ւ񂩂�A����ɂ���Ă�[
	return GetBuffer()->SetVolume(volume);

	// DSBVOLUME_MAX�i�����Ȃ��j :		0
	// DSBVOLUME_MIN�i�����j	 : -10000
	// �̊ԂŎw�肷��̂��B
}

LONG CWaveStaticSound::GetVolume() const
// �擾�́A����`�����l���ɑ΂��Ă����ł��Ȃ�...
{
	LONG volume;
	if ((GetBuffer()->GetVolume(volume))!=DS_OK){
		Err.Out("CWaveStaticSound::GetVolume�Ɏ��s");
		return 0; // volume��DSBVOLUME_MAX��Ԃ��Ƃ���?
	}
	return volume;
}

LONG CWaveStaticSound::GetCurrentPos() const
{
	if (m_nAvgBytesPerSec==0) return -1;	//	Load�̏��擾�Ŏ��s���Ă���
	
	DWORD pos, dm;
	if( (GetBuffer()->GetCurrentPosition( pos, dm )) != 0 ) return -1;
	return LONG(((DWORDLONG) pos * 1000 / m_nAvgBytesPerSec ) );
	//	DWORDLONG�ɃL���X�g���Ȃ��ƈ�ꂿ�Ⴄ:p
}

LRESULT CWaveStaticSound::SetCurrentPos(LONG lPos){
	if (m_nAvgBytesPerSec==0) return -1;	//	Load�̏��擾�Ŏ��s���Ă���

	DWORD pos;
	pos = (DWORD)((DWORDLONG)lPos * m_nAvgBytesPerSec / 1000);
	return GetBuffer()->SetCurrentPosition(pos);
}

//	bmd����̒��
LONG CWaveStaticSound::GetFrequency() const
{
	DWORD dwFreq;
	if( GetBuffer()->GetFrequency( dwFreq ) != 0 ) return -1;
	return( (LONG)dwFreq );
}

LRESULT CWaveStaticSound::SetFrequency( DWORD freq )
{
	if((DSBFREQUENCY_MIN<=freq && freq<=DSBFREQUENCY_MAX) || freq==DSBFREQUENCY_ORIGINAL)
		if( GetBuffer()->SetFrequency( freq ) == 0 ) return 0;
	return 1;
}

LRESULT	CWaveStaticSound::SetLoopPlay(bool bLoop)
{
	m_bLoop = bLoop;
	return 0;
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
