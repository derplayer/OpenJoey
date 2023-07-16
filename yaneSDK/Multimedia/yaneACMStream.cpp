#include "stdafx.h"
#include "../Auxiliary/yaneFile.h"
#include "yaneSoundStream.h"
#include "yaneACMStream.h"
#pragma comment(lib, "msacm32.lib")
//#include "../Window/yaneDebugWindow.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

// CACMStream
CACMStream::CACMStream() : m_hmmio(NULL), m_hAcm(NULL), m_pDestFormat(&m_vDestFormat)
{
	m_vFile.Add(new CFile);
	// ����
	Close();
}

CACMStream::~CACMStream()
{
	// ����
	Close();
}

LRESULT	CACMStream::Open(const char* filename)
{
	// ����
	Close();

	// ���k�t�@�C���Ȃ񂾂���ۂ��Ɠǂݍ���
	if ( LRESULT ret = GetFile()->Read(filename) ) {
		Err.Out("CACMStream::Open �t�@�C���ǂݍ��݂Ɏ��s(ret=%d) - %s", ret, filename);
		return 1; // �t�@�C���ǂݍ��݃G���[
	}

	// RIFF�`���Ɖ��肵�ăI�[�v��
	LRESULT ret = InitializeNormal();
	// ����[�HMP3�`���Ȃ񂩂Ȃ�
	if (ret==2){
		ret = InitializeMP3();
	}
	// �����񂩂���
	if (ret!=0){
		Err.Out("CACMStream::Open �I�[�v���Ɏ��s(ret=%d) - %s", ret, filename);
		Close();
		return 2;
	}

	// ACM������������
	m_vDestFormat.wFormatTag = WAVE_FORMAT_PCM;	//	PCM�ɂȂ��ė~�����˂�I
	if (acmFormatSuggest(NULL,m_pSrcFormat, m_pDestFormat, sizeof(WAVEFORMATEX), ACM_FORMATSUGGESTF_WFORMATTAG)!=0){
		Err.Out("CACMStream::Open acmFormatSuggest()�Ɏ��s");
		Close();
		return 3;	//	acm������Ƃ��Ⴄ�H
	}
	if (acmStreamOpen(&m_hAcm, NULL, m_pSrcFormat, m_pDestFormat, NULL, NULL, NULL, 0/*ACM_STREAMOPENF_NONREALTIME*/)!=0){
		Err.Out("CACMStream::Open acmStreamOpen()�Ɏ��s");
		Close();
		return 4;	//	acm��������Ƃ��Ⴄ�H
	}
	// �ŏ��u���b�N�������߂�
	if (acmStreamSize(m_hAcm, m_dwSrcBlockLength, &m_dwDstBlockLength, ACM_STREAMSIZEF_SOURCE) !=0 ){
		Err.Out("CACMStream::Open acmStreamSize()�Ɏ��s");
		Close();
		return 5;	//	�Ȃ�ł�˂�ƌ��������i�΁j
	}

	return 0;
}

LRESULT	CACMStream::Close()
{
	if (m_hmmio!=NULL) mmioClose(m_hmmio, 0);
	m_hmmio = NULL;
	GetFile()->Close();

	m_pSrcFormat = NULL;
	ZERO(m_vMP3Format);
	ZERO(m_vDestFormat);

	m_dwSrcLength = 0;
	m_dwDataOffset = 0;
	m_dwReadPosition = 0;

	m_lDestLength = 0;
	m_dwDestTotalWritten = 0;

	m_dwSrcBlockLength = 0;
	m_dwDstBlockLength = 0;

	if (m_hAcm!=NULL) acmStreamClose(m_hAcm, 0);
	m_hAcm = NULL;
	ZERO(m_vAcmHeader);

	return 0;
}

LONG CACMStream::GetLength() const
{
	return m_lDestLength;
}

LRESULT CACMStream::InitializeNormal()
{
	// �f�[�^�́A��������Af.fileadr����f.filesize����������B
	MMIOINFO mmioinfo; // �������t�@�C����mmioOpen����̂��I�I(C)yaneurao
	ZeroMemory(&mmioinfo, sizeof(mmioinfo));
	mmioinfo.pchBuffer = (LPSTR)GetFile()->GetMemory();	// �����Ƀt�@�C���o�b�t�@adr
	mmioinfo.fccIOProc = FOURCC_MEM;					// ����������I
	mmioinfo.cchBuffer = GetFile()->GetSize();			// �����Ƀt�@�C���T�C�Y

	// ����ƃI�[�v���ł���B�i��͒������[�j
	m_hmmio = mmioOpen(NULL, &mmioinfo, MMIO_READ);
	if (m_hmmio==NULL) {
		Err.Out("CACMStream::InitializeNormal mmioOpen�Ɏ��s");
		return 1;
	}

	MMCKINFO ciPC,ciSC; // �e�`�����N�ƃT�u�`�����N
	ciPC.fccType = mmioFOURCC('W','A','V','E');
	if (mmioDescend(m_hmmio,(LPMMCKINFO)&ciPC,NULL,MMIO_FINDRIFF)){
		Err.Out("CACMStream::InitializeNormal Wave�t�@�C���łȂ�");
		return 2;
	}
	ciSC.ckid = mmioFOURCC('f','m','t',' ');
	if (mmioDescend(m_hmmio,&ciSC,&ciPC,MMIO_FINDCHUNK)){
		Err.Out("CACMStream::InitializeNormal fmt�`�����N���w��ł��Ȃ�");
		return 3;
	}

	// WAVEFORMATEX���σT�C�Y�Ȃ̂ŁA�|�C���^�𒼐ړ˂�����
	m_pSrcFormat = (WAVEFORMATEX*)((BYTE*)GetFile()->GetMemory() + mmioSeek(m_hmmio,0,SEEK_CUR));
	if (m_pSrcFormat->cbSize==0) { m_pSrcFormat->cbSize = sizeof(WAVEFORMATEX); }
	if (m_pSrcFormat->wFormatTag == WAVE_FORMAT_PCM){
		Err.Out("CACMStream::InitializeNormal ACM���g���܂ł��Ȃ�");
		return 4;
	}

	mmioAscend(m_hmmio,&ciSC,0); // fmt�T�u�`�����N�O���ֈړ�
	ciSC.ckid = mmioFOURCC('d','a','t','a');
	if (mmioDescend(m_hmmio,&ciSC,&ciPC,MMIO_FINDCHUNK)){
		Err.Out("CACMStream::InitializeNormal �f�[�^�`�����N�ɍs���Ȃ�");
		return 4;
	}

	// �������Ɠ�����@�Ńf�[�^�ւ̃|�C���^�𓾂�
	m_dwSrcLength = ciSC.cksize; // �f�[�^�T�C�Y
	m_dwDataOffset = mmioSeek(m_hmmio,0,SEEK_CUR);
	m_dwSrcBlockLength = m_pSrcFormat->nBlockAlign;
	m_lDestLength = m_dwSrcLength*1000/m_pSrcFormat->nAvgBytesPerSec;

	SetCurrentPos(0);

	return 0;
}

LRESULT CACMStream::InitializeMP3()
{
	const DWORD dwFileSize = GetFile()->GetSize();
	DWORD availSize = dwFileSize;
	DWORD dwSkipLen = 0;
	if (dwFileSize <= 128) {
		Err.Out("CACMStream::InitializeMP3 �t�@�C���T�C�Y���ُ�(dwFileSize=%d)", dwFileSize);
		return 1; // ����ȏ������킯����ւ񂪂�:p
	}

	//	�t���[���w�b�_����WAVEFORMATEX�̃f�[�^��p�ӂ���
	const unsigned char* const src_begin	= (unsigned char*)GetFile()->GetMemory();
	const unsigned char* const src_end		= src_begin + dwFileSize;
	const unsigned char* src				= src_begin;

	//  ID3v2�^�O�����Ă���Ȃ�΁A�ǂݔ�΂�
	if ((src[0] == 'I') && (src[1] == 'D') && (src[2] == '3')) {
		DWORD dwID3Size = src[9] + (src[8]<<7) + (src[7]<<14) + (src[6]<<21);
		// �o�[�W�����`�F�b�N
		if(src[3]>=0x04)
		{
			// ID3v2.4.0�ȍ~
			if(src[5]&0x10){	// �t�b�^�̗L��
				dwID3Size+=20; // �t�b�^����
			}else{
				dwID3Size+=10; // �t�b�^�Ȃ�
			}
		}else{
			// ID3v2.3.0�ȑO
			dwID3Size+=10; // �t�b�^�Ȃ�
		}
		
		if (availSize <= dwID3Size + 128) {
			Err.Out("CACMStream::InitializeMP3 ID3�w�b�_�����ُ�(availSize=%d, dwID3Size=%d)", availSize, dwID3Size);
			return 1;
		}
		
		src += dwID3Size;
		availSize -= dwID3Size;
		dwSkipLen += dwID3Size;
	}

	//	MP3�`�F�b�N
	if ( src[0]!=0xff ) {
		Err.Out("CACMStream::InitializeMP3 MP3�`�F�b�Nphase1�Ɏ��s(src[0]=0x%X)", src[0]);
		return 1;
	}
	if ( (src[1]&0xf8)!=0xf8 ) {
		Err.Out("CACMStream::InitializeMP3 MP3�`�F�b�Nphase2�Ɏ��s(src[1]=0x%X)", src[1]);
		return 1;
	}

	const int anBitrate[2][3][16] = {
		{
			// MPEG-1
			{ 0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,0 },	//	32000Hz(layer1)
			{ 0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,0 },	//	44100Hz(layer2)
			{ 0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,0 },	//	48000Hz(layer3)
		},
		{
				// MPEG-2
			{ 0,32,48,56, 64, 80, 96,112,128,144,160,176,192,224,256,0 },	//	32000Hz(layer1)
			{ 0, 8,16,24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,0 },	//	44100Hz(layer2)
			{ 0, 8,16,24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,0 },	//	48000Hz(layer3)
			},
	};
	const int anFreq[2][4] = {
		{ 44100,48000,32000,0 },
		{ 22050,24000,16000,0 },
	};

	const int nLayer = 4-((src[1] >> 1) & 3);
	if ( nLayer > 3 ) {
		Err.Out("CACMStream::InitializeMP3 MP3�������ȃ��C��");
		return 1;
	}

	const int nMpeg		 = ((src[1] & 8) == 0) ? 1 : 0;
	const int nBitrate	 = anBitrate[nMpeg][nLayer-1][ src[2]>>4 ];
	const int nFreq		 = anFreq[nMpeg][(src[2] >> 2) & 3];
	const int nChannel	 = ((src[3] >> 6) == 3) ? 1 : 2;
	const int nFrameSize = (72*(nMpeg+1)*1000) * nBitrate / nFreq;

	//	�����MP3�̃^�O��p�ӂ���
	ZERO(m_vMP3Format);
	m_vMP3Format.wfx.cbSize				= MPEGLAYER3_WFX_EXTRA_BYTES;
	m_vMP3Format.wfx.wFormatTag			= WAVE_FORMAT_MPEGLAYER3;
	m_vMP3Format.wfx.nChannels			= nChannel;
	m_vMP3Format.wfx.nSamplesPerSec		= nFreq;
	m_vMP3Format.wfx.nAvgBytesPerSec	= nBitrate * 1000 / 8;
	m_vMP3Format.wfx.nBlockAlign		= 1;
	m_vMP3Format.wfx.wBitsPerSample		= 0;
	m_vMP3Format.wID					= MPEGLAYER3_ID_MPEG;
	m_vMP3Format.fdwFlags				= MPEGLAYER3_FLAG_PADDING_OFF;
	m_vMP3Format.nBlockSize				= nFrameSize;
	m_vMP3Format.nFramesPerBlock		= 1;
	m_vMP3Format.nCodecDelay			= 0x0571;
	//	����ID3�^�O�����Ă���Ȃ�΁A���̕������O����
	//	dwFileSize > 128 �ł��邱�Ƃ͕ۏ؂���Ă���
	if ((src_end[-128] == 'T') && (src_end[-127] == 'A') && (src_end[-126] == 'G')) {
		availSize -= 128;
	}
	availSize -= 4;
	dwSkipLen += 4;

	m_dwSrcLength = availSize;
	m_dwDataOffset = dwSkipLen;

	// ����WAVEFORMATEX���g���Ă���
	m_pSrcFormat = &m_vMP3Format.wfx;
	m_dwSrcBlockLength = m_vMP3Format.nBlockSize;

	m_lDestLength = (m_dwSrcLength*8/nBitrate);

	SetCurrentPos(0);

	return 0;
}

LRESULT	CACMStream::SetCurrentPos(LONG lPosition)
{
	DWORD dwBytePosition = MsecToByte(lPosition);
	if (dwBytePosition==0xffffffff) return 1;	// �ǂݍ���łȂ��񂿂Ⴄ�H

	if (lPosition >= GetLength()) return 2;		// ��΂������Ȃ񂶂�Ȃ��́H

	// ACM���ď���������
	if (m_hAcm!=NULL) {
		acmStreamClose(m_hAcm, 0);
		m_hAcm = NULL;
		ZERO(m_vAcmHeader);
		if (acmStreamOpen(&m_hAcm, NULL, m_pSrcFormat, m_pDestFormat, NULL, NULL, NULL, ACM_STREAMOPENF_NONREALTIME)!=0){
			Err.Out("CACMStream::SetCurrentPos acmStreamOpen()�Ɏ��s");
			return 3;	//	acm��������Ƃ��Ⴄ�H
		}
	}	
	DWORD dwSrcPos = 0;
	acmStreamSize(m_hAcm, dwBytePosition, &dwSrcPos, ACM_STREAMSIZEF_DESTINATION);
	m_dwReadPosition = dwSrcPos;

	// �����܂ŏ������񂾎��ɂ���
	m_dwDestTotalWritten = dwBytePosition;

	return 0;
}

LONG CACMStream::GetCurrentPos() const
{
	LONG lMsecPosition = ByteToMsec(m_dwDestTotalWritten);

	return lMsecPosition;
}

DWORD CACMStream::Read(BYTE* lpDest, DWORD dwSize)
{
	// �ǂ�łȂ����
	if ( GetFile()->GetMemory()==NULL || m_hAcm==NULL ) return 0xffffffff;

	// �I�[���
	if ( m_dwReadPosition==m_dwSrcLength ) return 0;

	DWORD dwReadedSize = 0;
	InnerRead(lpDest, dwSize, dwReadedSize);

	// ���񂾂��������񂾂�
	m_dwDestTotalWritten += dwReadedSize;

	return dwReadedSize;
}

LRESULT CACMStream::InnerRead(BYTE* lpDest, DWORD dwSize, DWORD& dwReaded)
{
	// ACMSTREAMHEADER������������
	ZERO(m_vAcmHeader);

	m_vAcmHeader.cbStruct		 = sizeof(ACMSTREAMHEADER);
	m_vAcmHeader.pbSrc			 = (BYTE*)GetFile()->GetMemory() + m_dwDataOffset+m_dwReadPosition;
	m_vAcmHeader.cbSrcLength	 = m_dwSrcLength - m_dwReadPosition;
	m_vAcmHeader.cbSrcLengthUsed = 0;
	m_vAcmHeader.pbDst			 = lpDest;
	m_vAcmHeader.cbDstLength	 = dwSize;
	m_vAcmHeader.cbDstLengthUsed = 0;

	smart_ptr<BYTE> p;
	if (0<m_vAcmHeader.cbSrcLength&&m_vAcmHeader.cbSrcLength<500){
		p.AddArray(1000);
		memset(p.get(),0,1000);
		memcpy(p.get(),m_vAcmHeader.pbSrc,m_vAcmHeader.cbSrcLength);
		m_vAcmHeader.pbSrc = p.get();
		m_vAcmHeader.cbSrcLength = 1000;
	}

	// �ݒ肷��
	if(acmStreamPrepareHeader(m_hAcm, &m_vAcmHeader, NULL)!=0){
		Err.Out("CACMStream::Read acmStreamPrepareHeader()�Ɏ��s");
		return 3;
	}
	// �ϊ�����
//	if(acmStreamConvert(m_hAcm, &m_vAcmHeader, 0)!=0){
	if(acmStreamConvert(m_hAcm, &m_vAcmHeader, ACM_STREAMCONVERTF_BLOCKALIGN)!=0){
		Err.Out("CACMStream::Read acmStreamConvert()�Ɏ��s");
		return 4;
	}
	// �ݒ����������
	if(acmStreamUnprepareHeader(m_hAcm, &m_vAcmHeader, NULL)!=0){
		Err.Out("CACMStream::Read acmStreamPrepareHeader()�Ɏ��s");
		return 5;
	}
	// �ϊ��o�����T�C�Y�𒲂ׂ�
	dwReaded = m_vAcmHeader.cbDstLengthUsed;
	if (dwReaded==0) {
		// ���̉R���I�ϊ����ĂȂ��̂�Src�g�������Č����₪��R(`�D�L)�m
	} else {
		m_dwReadPosition += m_vAcmHeader.cbSrcLengthUsed;
	}

	if (m_dwReadPosition > m_dwSrcLength) {
		m_dwReadPosition = m_dwSrcLength;
	}

	return 0;
}

LONG CACMStream::ByteToMsec(DWORD dwByte) const
{
	if (GetFormat() && GetFormat()->nAvgBytesPerSec) {
		return (LONG)( (DWORDLONG)dwByte * 1000 / GetFormat()->nAvgBytesPerSec );
	}

	return -1;
}

DWORD CACMStream::MsecToByte(LONG lMsec) const
{
	if (GetFormat() && GetFormat()->nAvgBytesPerSec) {
		return (DWORD)( (DWORDLONG)lMsec * GetFormat()->nAvgBytesPerSec / 1000 );
	}

	return 0xffffffff;
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
