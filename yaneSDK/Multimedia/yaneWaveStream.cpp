#include "stdafx.h"
#include "../Auxiliary/yaneFile.h"
#include "yaneSoundStream.h"
#include "yaneWaveStream.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

// CWaveStream
CWaveStream::CWaveStream() : m_hmmio(NULL)
{
	m_vFile.Add(new CFile);
	// ����
	Close();
}

CWaveStream::~CWaveStream()
{
	// ����
	Close();
}

LRESULT	CWaveStream::Open(const char* filename)
{
	// ����
	Close();

	// �Ȃ�ׂ��X�g���[�~���O�̕����ŃI�[�v��
	m_bStaticOpen = false;
	if (GetFile()->Open(filename, "rb")!=0){
		// ����[�Ȃ��ۂ��Ɠǂݍ�����
		m_bStaticOpen = true;
		if (GetFile()->Read(filename)) {
			Err.Out("CWaveStream::Load %s�̃t�@�C���ǂݍ��ݎ��̃G���[", filename);
			return 1; // �t�@�C���ǂݍ��݃G���[
		}
	}

	if (Initialize()!=0){ Close(); return 2; }
	return 0;
}

LRESULT	CWaveStream::Close()
{
	if (m_hmmio!=NULL) mmioClose(m_hmmio, 0);
	m_hmmio = NULL;
	GetFile()->Close();

	m_bStaticOpen = false;
	ZERO(m_vWaveFormat);
	m_dwDataOffset = 0;
	m_dwDataLength = 0;
	m_dwReadPosition = 0;

	return 0;
}

LONG CWaveStream::GetLength() const
{
	return ByteToMsec(m_dwDataLength);
}

WAVEFORMATEX* CWaveStream::GetFormat() const
{
	// �₾�Ȃ�����Ȃ���
	return &(const_cast<CWaveStream*>(this))->m_vWaveFormat;
}

LRESULT CWaveStream::Initialize()
{
	if (m_bStaticOpen){
		// �f�[�^�́A��������Af.fileadr����f.filesize����������B
		MMIOINFO mmioinfo; // �������t�@�C����mmioOpen����̂��I�I(C)yaneurao
		ZeroMemory(&mmioinfo, sizeof(mmioinfo));
		mmioinfo.pchBuffer = (LPSTR)GetFile()->GetMemory();	// �����Ƀt�@�C���o�b�t�@adr
		mmioinfo.fccIOProc = FOURCC_MEM;					// ����������I
		mmioinfo.cchBuffer = GetFile()->GetSize();			// �����Ƀt�@�C���T�C�Y

		// ����ƃI�[�v���ł���B�i��͒������[�j
		m_hmmio = mmioOpen(NULL, &mmioinfo, MMIO_READ);
		if (m_hmmio==NULL) {
			Err.Out("CWaveStream::Initialize mmioOpen�Ɏ��s");
			return 1;
		}
	} else {
		// �t�@�C�����I�[�v�����Ă��炤
		m_hmmio = mmioOpen(const_cast<LPSTR>(GetFile()->GetName().c_str()), NULL, MMIO_READ);
		if (m_hmmio==NULL) {
			Err.Out("CWaveStream::Initialize mmioOpen�Ɏ��s");
			return 1;
		}
	}

	MMCKINFO ciPC,ciSC; // �e�`�����N�ƃT�u�`�����N
	ciPC.fccType = mmioFOURCC('W','A','V','E');
	if (mmioDescend(m_hmmio,(LPMMCKINFO)&ciPC,NULL,MMIO_FINDRIFF)){
		Err.Out("CWaveStream::Initialize Wave�t�@�C���łȂ�");
		return 2;
	}
	ciSC.ckid = mmioFOURCC('f','m','t',' ');
	if (mmioDescend(m_hmmio,&ciSC,&ciPC,MMIO_FINDCHUNK)){
		Err.Out("CWaveStream::Initialize fmt�`�����N���w��ł��Ȃ�");
		return 3;
	}

	// �ꍇ�����߂�ǂ�����mmioRead
	mmioRead(m_hmmio, (HPSTR)GetFormat(), sizeof(WAVEFORMATEX));
	if (GetFormat()->wFormatTag != WAVE_FORMAT_PCM){
		Err.Out("CWaveStream::Initialize WAVE_FORMAT_PCM����Ȃ�");
		return 4;
	}

	mmioAscend(m_hmmio,&ciSC,0); // fmt�T�u�`�����N�O���ֈړ�
	ciSC.ckid = mmioFOURCC('d','a','t','a');
	if (mmioDescend(m_hmmio,&ciSC,&ciPC,MMIO_FINDCHUNK)){
		Err.Out("CWaveStream::Initialize �f�[�^�`�����N�ɍs���Ȃ�");
		return 5;
	}

	// �������Ɠ�����@�Ńf�[�^�ւ̃|�C���^�𓾂�
	m_dwDataLength   = ciSC.cksize; // �f�[�^�T�C�Y
	m_dwDataOffset   = mmioSeek(m_hmmio,0,SEEK_CUR);

	SetCurrentPos(0);

	return 0;
}

LRESULT	CWaveStream::SetCurrentPos(LONG lPosition)
{
	DWORD dwBytePosition = MsecToByte(lPosition);
	if (dwBytePosition==0xffffffff) return 1;	// �ǂݍ���łȂ��񂿂Ⴄ�H

	if (lPosition >= GetLength()) return 2;		// ��΂������Ȃ񂶂�Ȃ��́H

	m_dwReadPosition = dwBytePosition;
	if (m_bStaticOpen){
		// ���ɂ��鎖�͂Ȃ�
	} else {
		fseek(GetFile()->GetFilePtr(), m_dwDataOffset + m_dwReadPosition, SEEK_SET);
	}
	return 0;
}

LONG CWaveStream::GetCurrentPos() const
{
	LONG lMsecPosition = ByteToMsec(m_dwReadPosition);

	return lMsecPosition;
}

DWORD CWaveStream::Read(BYTE* lpDest, DWORD dwSize)
{
	DWORD readed;
	if (m_bStaticOpen){
		// �T�C�Y�����Ȃ����ȁH
		int mod = (m_dwReadPosition+dwSize) - m_dwDataLength;
		if (mod<0) mod = 0;

		readed = dwSize - mod;
		memcpy(lpDest, (BYTE*)GetFile()->GetMemory()+m_dwDataOffset+m_dwReadPosition, readed);
		m_dwReadPosition += readed;
	} else {
		readed = fread(lpDest, sizeof(BYTE), dwSize, GetFile()->GetFilePtr());
		m_dwReadPosition += readed;
	}
	return readed;
}

LONG CWaveStream::ByteToMsec(DWORD dwByte) const
{
	if (GetFormat() && GetFormat()->nAvgBytesPerSec) {
		return (LONG)( (DWORDLONG)dwByte * 1000  / GetFormat()->nAvgBytesPerSec);
	}

	return -1;
}

DWORD CWaveStream::MsecToByte(LONG lMsec) const
{
	if (GetFormat() && GetFormat()->nAvgBytesPerSec) {
		return (DWORD)( (DWORDLONG)lMsec * GetFormat()->nAvgBytesPerSec / 1000);
	}

	return 0xffffffff;
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
