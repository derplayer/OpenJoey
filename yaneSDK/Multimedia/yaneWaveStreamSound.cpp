#include "stdafx.h"

#include "../Auxiliary/yaneFile.h"
#include "../Thread/yaneCriticalSection.h"
#include "yaneWaveOutput.h"
#include "yaneSoundParameter.h"
#include "yaneSoundBuffer.h"
#include "yaneSoundStream.h"

#include "yaneWaveStreamSound.h"
//#include "../Window/yaneDebugWindow.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

//////////////////////////////////////////////////////////////////////////

void CWaveStreamSound::ThreadProc()
{
	bool bWait = false;
	const DWORD dwBufferSize = GetBufferSize();
	while (true) {
		//	���ɗp�����c�H
		if ( !IsThreadValid() ) return ;

		//	�t�@�C���S�̂�ʂ��Ă̍Đ��ʒu�����o����
		//	GetNextSoundData�֐�����CThread::sleep�֐��̊֌W��A20msec���x�̕���\�����Ȃ�
		{	CCriticalLock lock(GetCS());
			const DWORD dwPrevPlayCursor = m_dwPrevPlayCursor;
			DWORD dwPlayCursor, dwWriteCursor;
			GetBuffer()->GetCurrentPosition(dwPlayCursor, dwWriteCursor);
			if ( dwPlayCursor >= dwPrevPlayCursor ) {
				m_dwPlayPosition += dwPlayCursor - dwPrevPlayCursor;
			} else {
				m_dwPlayPosition += (dwPlayCursor + dwBufferSize) - dwPrevPlayCursor;
			}
			m_dwPrevPlayCursor = dwPlayCursor;
			m_dwPlayPosition %= m_dwLength;
		}

		//	�f�[�^�𗬂�����
		DWORD ret = GetNextSoundData();
		if ( ret==1 ) {	//	EOF
			if (IsLoopPlay()){
				//	���[�v�Đ��Ȃ�A�X�g���[���������߂��čĐ����s
				GetStream()->SetCurrentPos(0);
				continue ;
			} else {
				//	�񃋁[�v�Đ��Ȃ�A�I�[�܂ł̍Đ��̖ʓ|������
				bWait = true;
				break ;
			}
		}
		ef ( ret>=2 ) {	//	�G���[
			//	Stop()���ĂԂƃX���b�h�̏I����҂̂Ńf�b�h���b�N�ɂȂ����܂�
			InnerStop();
			return ;
		}
	}

	if ( bWait ) {
		//	�Đ��J�[�\������~�ʒu���O���ɂ���ƁA
		//	dwPlayCursor >= m_dwNextWriteCursor �Ƃ��������ł�
		//	�������ʒu�Œ�~�o���Ȃ�
		//	�i�o�b�t�@�̈����������������~���Ă��܂��j
		bool bForward = false;
		DWORD dwLimit = 0;
		{	CCriticalLock lock(GetCS());
			//dwLimit = m_dwNextWriteCursor - (m_nAvgBytesPerSec / 50);//	20msec��O�Ŏ~�߂�
			dwLimit = m_dwNextWriteCursor;
			DWORD dwPlayCursor, dwWriteCursor;
			GetBuffer()->GetCurrentPosition(dwPlayCursor, dwWriteCursor);
			if ( dwPlayCursor >= dwLimit ) {
				bForward = true;	//	�Đ��J�[�\���͒�~�ʒu���O��
			}

			//	���݂̏���
			//	bForward ==	true  -> ��~�ʒu�`�Đ��ʒu�܂ŃN���A
			//				false -> ��~�ʒu�`�I���܂ŃN���A
			const DWORD dwClearSize = (bForward) ? (dwPlayCursor - dwLimit) : (dwBufferSize - dwLimit);
			BYTE* lpLockedBuffer;
			DWORD dwLockedSize;
			//	���b�v�A���E���h���邱�Ƃ͂��蓾�Ȃ�
			if ( GetBuffer()->Lock(dwLimit, dwClearSize, &lpLockedBuffer, &dwLockedSize) ) {
				Err.Out("CWaveStreamSound::ThreadProc GetBuffer()->Lock()�Ɏ��s�I");
				//	Stop()���ĂԂƃX���b�h�̏I����҂̂Ńf�b�h���b�N�ɂȂ����܂�
				InnerStop();
				return ;
			}
			memset(lpLockedBuffer, 0, dwLockedSize);
			if ( GetBuffer()->Unlock(lpLockedBuffer, dwLockedSize) ){
				Err.Out("CWaveStreamSound::ThreadProc GetBuffer()->Unlock()�Ɏ��s�I");
				//	Stop()���ĂԂƃX���b�h�̏I����҂̂Ńf�b�h���b�N�ɂȂ����܂�
				InnerStop();
				return ;
			}
		}

		while (true) {
			//	���ɗp�����c�H
			if ( !IsThreadValid() ) return ;

			//	�t�@�C���S�̂�ʂ��Ă̍Đ��ʒu�����o����
			//	GetNextSoundData�֐�����CThread::sleep�֐��̊֌W��A20msec���x�̕���\�����Ȃ�
			{	CCriticalLock lock(GetCS());
				const DWORD dwPrevPlayCursor = m_dwPrevPlayCursor;
				DWORD dwPlayCursor, dwWriteCursor;
				GetBuffer()->GetCurrentPosition(dwPlayCursor, dwWriteCursor);
				if ( !bForward && dwPlayCursor >= dwLimit ){
					//	Stop()���ĂԂƃX���b�h�̏I����҂̂Ńf�b�h���b�N�ɂȂ����܂�
					InnerStop();
					return ;
				}
				if ( dwPlayCursor == dwPrevPlayCursor ) {
					lock.Leave();						//	���b�N����
					if ( CThread::sleep(20) ) return ;	//	���΂��x���BInvalidate���ꂽ�炷���I��
					continue ;
				} else {
					if ( dwPlayCursor > dwPrevPlayCursor ) {
						m_dwPlayPosition += dwPlayCursor - dwPrevPlayCursor;
					} else {
						m_dwPlayPosition += (dwPlayCursor + dwBufferSize) - dwPrevPlayCursor;
						//	�Đ��J�[�\���͒�~�ʒu������Ɉʒu���Ă��鎖�ɂȂ�
						bForward = false;
					}
				}
				m_dwPrevPlayCursor = dwPlayCursor;
				m_dwPlayPosition %= m_dwLength;
			}
		}
	}
}

LRESULT CWaveStreamSound::GetNextSoundData()
{
	//	������M��񂾂���ז�����Ȃ�[
	CCriticalLock lock(GetCS());

	const DWORD dwBufferSize = GetBufferSize();

	//	������T�C�Y�𓾂�
	DWORD dwUsedSize = 0;
	DWORD dwPlayCursor, dwWriteCursor;
	GetBuffer()->GetCurrentPosition(dwPlayCursor, dwWriteCursor);
	{
		const DWORD dwPrevWriteCursor = m_dwPrevWriteCursor;
		if ( dwWriteCursor == dwPrevWriteCursor ) {	//	����ĂȂ��́H
			lock.Leave();		//	���b�N����
			CThread::sleep(20);	//	���΂��x��
			return 0;
		} else {
			//dwUsedSize = ((dwBufferSize + dwWriteCursor) - dwPrevWriteCursor) % dwBufferSize;
			if ( dwWriteCursor > dwPrevWriteCursor ) {
				dwUsedSize = dwWriteCursor - dwPrevWriteCursor;
			} else {
				dwUsedSize = (dwWriteCursor + dwBufferSize) - dwPrevWriteCursor;
			}
		}
	}
	//	�O���[�o���Ȃ�����������悹
	dwUsedSize += m_dwWriteDiffer;
	//	�g�p�ʂ�0.5�b����菬�����Ȃ牽�����Ȃ��iCPU�p���[�̐ߖ�j
	if ( dwUsedSize < m_nAvgBytesPerSec/2 ) {
		lock.Leave();		//	���b�N����
		CThread::sleep(20);	//	���΂��x��
		return 0;
	}
	m_dwWriteDiffer = 0;	//	�Y�ꂸ�ɃN���A�[

	//	�Đ��J�[�\���ŃN���b�s���O����
	{
		const DWORD dwNextWriteCursor = m_dwNextWriteCursor;
		//	���b�v�A���E���h���l������
		if ( (dwNextWriteCursor + dwUsedSize) > dwBufferSize ) {
			//	�������݊J�n�ʒu����o�b�t�@�I�[�܂ł̒���
			const DWORD dwWriteSize1 = dwBufferSize - dwNextWriteCursor;
			//	�o�b�t�@��[���珑�����ݏI���ʒu�܂ł̒���
			const DWORD dwWriteSize2 = dwUsedSize - dwWriteSize1;
			if ( dwWriteSize2 > dwPlayCursor ) {
				dwUsedSize = dwWriteSize1 + dwPlayCursor;
			}
		} else {
			if ( dwNextWriteCursor < dwPlayCursor&&dwPlayCursor < (dwNextWriteCursor + dwUsedSize) ) {
				dwUsedSize = dwPlayCursor - dwNextWriteCursor;
			}
		}
	}
	//	4�o�C�g���E�ɍ��킹��
	dwUsedSize &= ~3;

	//	���݂̏����J�[�\�����o���Ă���
	m_dwPrevWriteCursor = dwWriteCursor;

	//	�X�g���[������ǂݍ���
	smart_ptr<BYTE> pBuffer;
	pBuffer.AddArray(dwUsedSize);
	const DWORD readed = GetStream()->Read(pBuffer.get(), dwUsedSize);
	if ( readed==0 ) {
		return 1;	//	EOF�ł���
	}
	ef ( readed > dwUsedSize ) {	//	�G���[�ƍl������
		Err.Out("CWaveStreamSound::GetNextSoundData GetStream()->Read()�Ɏ��s�I");
		return 2;
	}

	//	�T�E���h�o�b�t�@�ɏ�������
	BYTE* lpLockedBuffer;
	DWORD dwLockedSize;
	BYTE* lpLockedBuffer2;
	DWORD dwLockedSize2;
	if ( GetBuffer()->Lock(m_dwNextWriteCursor, dwUsedSize, &lpLockedBuffer, &dwLockedSize, &lpLockedBuffer2, &dwLockedSize2) ) {
		Err.Out("CWaveStreamSound::GetNextSoundData GetBuffer()->Lock()�Ɏ��s�I");
		return 3;
	}
	if ( readed < dwLockedSize ) {
		//	�����ǂ߂Ȃ������炵��
		memcpy(lpLockedBuffer, pBuffer.get(), readed);
	} else {
		memcpy(lpLockedBuffer, pBuffer.get(), dwLockedSize);
	}
	if ( lpLockedBuffer2!=NULL && readed > dwLockedSize ) {
		//	���b�v�A���E���h���������R�s�[
		if ( readed - dwLockedSize < dwLockedSize2 ) {
			//	�����ǂ߂Ȃ������炵��
			memcpy(lpLockedBuffer2, pBuffer.get()+dwLockedSize, readed - dwLockedSize);
		} else {
			//	�c�����������R�s�[
			memcpy(lpLockedBuffer2, pBuffer.get()+dwLockedSize, dwLockedSize2);
		}
	}
	if ( GetBuffer()->Unlock(lpLockedBuffer, dwLockedSize, lpLockedBuffer2, dwLockedSize2) ) {
		Err.Out("CWaveStreamSound::GetNextSoundData GetBuffer()->Unlock()�Ɏ��s�I");
		return 4;
	}

	//	���񏑂����݃J�[�\�����X�V
	m_dwNextWriteCursor += readed;
	m_dwNextWriteCursor %= dwBufferSize;
	//	����ǂݍ��߂Ȃ������T�C�Y�͎���ɂ܂킷
	if ( readed < dwUsedSize ) {
		m_dwWriteDiffer += dwUsedSize - readed;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

CWaveStreamSound::CWaveStreamSound(IWaveOutput* p) : m_pOutput(p)
{
	//	�f�t�H���g�Ń��[�v���Ȃ�
	SetLoopPlay(false);

	//	NullSoundBuffer��������ǂ�
	SetBuffer(smart_ptr<ISoundBuffer>(new CNullSoundBuffer));
	//	�Ȃ���ǂݍ���łȂ�����O
	m_dwLength = 0;
	m_nAvgBytesPerSec = 0;
	//	�Q�ƃJ�E���g����
	m_bPaused = 0;
	//	Volume��������
	m_lVolume = 0;

	//	�ʓ|���Ă�������
	GetOutput()->AddSound(this);
}

CWaveStreamSound::~CWaveStreamSound(){
	Release();
	//	����Ȃ�[
	GetOutput()->DelSound(this);
}

LRESULT CWaveStreamSound::InitBuffer()
{
	//	������M��񂾂���ז�����Ȃ�[
	CCriticalLock lock(GetCS());

	//	�t�@�C���ǂ݂���ł�́H
	if ( m_strFileName==""||m_nAvgBytesPerSec==0 ) {
		Err.Out("CWaveStreamSound::InitBuffer �t�@�C�����ǂݍ��߂ĂȂ�");
		return 1;
	}

	//	�ǂݍ��߂Ă�̂��`�F�b�N
	if ( GetStream()->GetFormat()==NULL ) {
		Err.Out("CWaveStreamSound::InitBuffer GetStream()->GetFormat()==NULL");
		return 2;
	}

	//	DirectSound�o�b�t�@�̍쐬
	SetBuffer( GetOutput()->CreateBuffer() );
	if ( GetBuffer()->Create(GetStream()->GetFormat(), GetBufferSize(), true, GetOutput()->GetSoundParameter().get())!=0 ) {
		Err.Out("CWaveStreamSound::InitBuffer �T�E���h�o�b�t�@�̍쐬�Ɏ��s�I");
		return 3;
	}

	//	�J�[�\����S�ď�����
	m_dwPrevWriteCursor = 0;
	m_dwNextWriteCursor = 0;
	m_dwWriteDiffer = 0;
	m_dwPrevPlayCursor = 0;
	GetBuffer()->SetCurrentPosition(0);
	//	�o�b�t�@��Volume�𔽉f�����Ă���
	GetBuffer()->SetVolume(m_lVolume);

	//	�o�b�t�@�T�C�Y��7/8���炢��ǂݍ���
	DWORD dwUsedSize = (GetBufferSize()*7)/8;
	//	4�o�C�g���E�ɍ��킹��
	dwUsedSize &= ~3;

	//	DirectSound�o�b�t�@��Lock
	BYTE* lpLockedBuffer;
	DWORD dwLockedSize;
	if ( GetBuffer()->Lock(m_dwNextWriteCursor, dwUsedSize, &lpLockedBuffer, &dwLockedSize) ) {
		//	����Ń_���Ȃ�A�����������̂��Ⴄ�H
		Err.Out("CWaveStreamSound::InitBuffer GetBuffer()->Lock()�Ɏ��s�I");
		return 4;
	}
	const DWORD dwReadedSize = GetStream()->Read(lpLockedBuffer, dwLockedSize);
	if ( dwReadedSize > dwLockedSize ) {	//	�G���[�ƍl������
		Err.Out("CWaveStreamSound::InitBuffer GetStream()->Read()�Ɏ��s�I");
		//	�Y�ꂸ��Unlock����Ƃ�
		GetBuffer()->Unlock(lpLockedBuffer, dwLockedSize);
		return 5;
	}
	if ( GetBuffer()->Unlock(lpLockedBuffer, dwLockedSize) ) {
		//	����Ȃ�ӂ[�A���s���邩��...�ǂȂ����[����[�񂶂�
		Err.Out("CWaveStreamSound::InitBuffer GetBuffer()->Unlock()�Ɏ��s�I");
		return 6;
	}
	//	���񏑂����݃J�[�\�����X�V
	m_dwNextWriteCursor += dwReadedSize;
	//	�ǂݍ��݂ł��Ȃ������T�C�Y�͎���ɂ܂킷
	if ( dwReadedSize < dwUsedSize ) { m_dwWriteDiffer = dwUsedSize - dwReadedSize; }

	return 0;
}

LRESULT CWaveStreamSound::Load(const string& filename)
{
	Release(); //	�܂��́A������悤�ˁI
	m_strFileName.erase();
	if ( filename.empty() ) { return 0; }

	//	�t�@�C�����J��
	SetStream( GetOutput()->GetSoundParameter()->GetStreamFactory()->Open(filename) );

	//	�b�Ԃ̍Đ��b��
	{	LPCWAVEFORMATEX pWaveFormat = GetStream()->GetFormat();
		if ( pWaveFormat!=NULL ) {
			m_nAvgBytesPerSec = pWaveFormat->nAvgBytesPerSec;
		} else {
			//	���������Ȃ��B����G���[�ɂȂ�񂩂��H
			m_nAvgBytesPerSec = 0;
		}
	}

	m_strFileName = filename;
	m_dwLength = (DWORD)((LONGLONG)GetStream()->GetLength() * m_nAvgBytesPerSec / 1000);

	//	���o��
	SetCurrentPos(0);

	return 0;
}

LRESULT CWaveStreamSound::Release(){
	if (IsPlay()) Stop();
	//	�Đ����Ȃ�Ƃ߂�Ƃ������I �i�ꉞ�ˁj

	//	������M��񂾂���ז�����Ȃ�[
	CCriticalLock lock(GetCS());

	//	NullSoundBuffer��������ǂ�
	SetBuffer(smart_ptr<ISoundBuffer>(new CNullSoundBuffer));
	m_strFileName.erase();

	return 0;
}

LRESULT CWaveStreamSound::Restore(){
	//	������M��񂾂���ז�����Ȃ�[
	DWORD status;
	{	CCriticalLock lock(GetCS());
		status = GetBuffer()->GetStatus();
	}

	if (m_strFileName!="" && status==1) {
		//	����[���X�g���Ă��(;�L�D`)
		bool bPlay = IsPlay();
		Stop();
		string temp = m_strFileName; //	�R�s�[���ēn�����Release���ꂿ�Ⴄ:p
		Load(temp);
		return (bPlay) ? Play() : 0;
	}

	return 0;
}

LRESULT CWaveStreamSound::Play()
{
	//	�Đ����Ȃ�Ƃ߂�Ƃ������I
	if ( IsPlay() ) Stop();

	m_bPaused = 0;

	//	�o�b�t�@������������
	if ( InitBuffer()!=0 ) {
		Stop();
		return 1;
	}

	//	�o�b�t�@���Đ���Ԃɂ���
	if ( GetBuffer()->Play(true)!=0 ) {
		Err.Out("CWaveStreamSound::InitBuffer GetBuffer()->Play()�Ɏ��s�I");
		Stop();
		return 2;
	}

	//	���Ƃ̓��[�J�X���b�h�ɔC����
	if ( CreateThread()!=0 ) {
		Err.Out("CWaveStreamSound::InitBuffer CreateThread()�Ɏ��s�I");
		Stop();
		return 3;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CWaveStreamSound::Stop()
{
	//	������M��񂾂���ז�����Ȃ�[
	{
		CCriticalLock lock(GetCS());
		if ( m_bPaused!=0 ) return 0; //	�|�[�Y���Ȃ����Stop����Ă���
	}

	//	�X���b�h�~�߂�->���S�̂��ߎ~�܂�܂ő҂�StopThread�֐����g��
	StopThread();

	return InnerStop();
}

LRESULT CWaveStreamSound::InnerStop()
{
	//	�X���b�h������Ă΂�鎖������̂�Lock���K�v
	CCriticalLock lock(GetCS());

	//	�o�b�t�@�~�߂ď���������
	LRESULT ret = GetBuffer()->Stop();
	GetBuffer()->SetCurrentPosition(0);
	if (!GetStream().isNull()){
		//	����̓t�@�C���ǂݍ��݂Ɏ��s���Ă����null�ł��肤��
		GetStream()->SetCurrentPos(0);
	}
	m_dwPlayPosition = 0;
	m_bPaused = 0;

	return ret;
}

LONG CWaveStreamSound::GetCurrentPos() const
{
	//	������M��񂾂���ז�����Ȃ�[
	CCriticalLock lock(GetCS());

	if (m_nAvgBytesPerSec==0) return -1;	//	Load�̏��擾�Ŏ��s���Ă���

	LONG pos = LONG(((DWORDLONG) m_dwPlayPosition * 1000 / m_nAvgBytesPerSec) );
	//	DWORDLONG�ɃL���X�g���Ȃ��ƈ�ꂿ�Ⴄ:p
	return pos;
}

LRESULT CWaveStreamSound::SetCurrentPos(LONG lPos){
	if (m_nAvgBytesPerSec==0) return 1;	//	Load�̏��擾�Ŏ��s���Ă���

	//	��������ʒu��ς���ɂ͎~�߂邵���Ȃ�
	bool bPlay = IsPlay();
	Stop();

	//	Stop�֐��ɂ��A�X���b�h�͎~�܂��Ă���
	//	�����CriticalSection�͕K�v�Ȃ�
	GetStream()->SetCurrentPos(lPos);
	m_dwPlayPosition = LONG(((DWORDLONG) lPos * m_nAvgBytesPerSec / 1000 ) );

	return (bPlay) ? Play() : 0;
}

bool CWaveStreamSound::IsPlay() const
//	���̃`�����l�����Đ���(=3)�Ȃ�true
{
	//	������M��񂾂���ז�����Ȃ�[
	CCriticalLock lock(GetCS());

	DWORD ret = GetBuffer()->GetStatus();
	return ret==3;
}

LRESULT CWaveStreamSound::Pause()
//	����`�����l����pause
{
	//	������M��񂾂���ז�����Ȃ�[
	CCriticalLock lock(GetCS());

	int& bPaused = m_bPaused;
	bPaused += (bPaused>0?1:(bPaused<0?-1:0));	//	�K�E�Z:p
	if ( !IsPlay() ) {
		return 0; //	�����I����Ă�
	}
	//	�Đ����̂�pause������Ȃ��
	bPaused = 1;

	LRESULT ret = GetBuffer()->Stop();
	return ret;
}

LRESULT CWaveStreamSound::Replay()
//	pause�Ŏ~�߂Ă����A���̃`�����l���̍Đ����ĊJ�B
{
	//	������M��񂾂���ז�����Ȃ�[
	CCriticalLock lock(GetCS());

	if (!m_bPaused) return 0; //	pause���Ăւ�āI

	if (--m_bPaused==0) {	//	�Q�ƃJ�E���g����
		LRESULT ret = GetBuffer()->Play(true);
		return ret;
	}
	return 0;
}

LRESULT CWaveStreamSound::SetVolume(LONG volume)
{
	//	������M��񂾂���ז�����Ȃ�[
	CCriticalLock lock(GetCS());

	//	�͈͊O�̃G���[�`�F�b�N���[�ւ񂩂�A����ɂ���Ă�[
	m_lVolume = volume;
	LRESULT ret = GetBuffer()->SetVolume(m_lVolume);
	return ret;

	//	DSBVOLUME_MAX�i�����Ȃ��j :		0
	//	DSBVOLUME_MIN�i�����j	 : -10000
	//	�̊ԂŎw�肷��̂��B
}

LONG CWaveStreamSound::GetVolume() const
//	�擾�́A����`�����l���ɑ΂��Ă����ł��Ȃ�...
{
	//	������M��񂾂���ז�����Ȃ�[
	CCriticalLock lock(GetCS());

	return m_lVolume;
}

//	bmd����̒��
DWORD CWaveStreamSound::GetFrequency() const
{
	//	������M��񂾂���ז�����Ȃ�[
	CCriticalLock lock(GetCS());

	DWORD dwFreq;
	LRESULT ret = GetBuffer()->GetFrequency(dwFreq);

	if ( ret!=0 ) return 0xffffffff;
	return (LONG)dwFreq;
}

LRESULT CWaveStreamSound::SetFrequency(DWORD freq)
{
	//	������M��񂾂���ז�����Ȃ�[
	CCriticalLock lock(GetCS());

	if((DSBFREQUENCY_MIN<=freq && freq<=DSBFREQUENCY_MAX) || freq==DSBFREQUENCY_ORIGINAL){
		LRESULT ret = GetBuffer()->SetFrequency(freq);
		if(ret==0) return 0;
	}
	return 1;
}

LRESULT	CWaveStreamSound::SetLoopPlay(bool bLoop)
{
	//	������M��񂾂���ז�����Ȃ�[
	CCriticalLock lock(GetCS());

	m_bLoop = bLoop;
	return 0;
}
bool CWaveStreamSound::IsLoopPlay() const
{
	//	������M��񂾂���ז�����Ȃ�[
	CCriticalLock lock(GetCS());

	bool ret = m_bLoop;
	return ret;
}

LONG CWaveStreamSound::GetLength() const
{
	//	������M��񂾂���ז�����Ȃ�[
	CCriticalLock lock(GetCS());

	LONG len =	GetStream()->GetLength();
	return len;
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
