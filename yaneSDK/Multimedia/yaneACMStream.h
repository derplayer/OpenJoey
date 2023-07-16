// enraACMStream.h
//		programmed by ENRA		'02/03/25

#ifndef __yaneACMStream_h__
#define __yaneACMStream_h__


#include "yaneSoundStream.h"
#include <msacm.h>

namespace yaneuraoGameSDK3rd { namespace Auxiliary { class IFile; }}

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class CACMStream : public ISoundStream {
/**
	ACM�𗘗p���ăT�E���h�f�[�^��ǂݍ��� class ISoundStream �h���N���X
	���k�t�@�C����HDD���璀���ǂݏo������̂ł͂Ȃ��A��C�ɓǂݍ��ށB
*/
public:
	CACMStream();
	virtual ~CACMStream();

	/// override from class ISoundStream
	LRESULT	Open(const char* filename);
	LRESULT	Close();
	LONG	GetLength() const;
	LRESULT	SetCurrentPos(LONG lPosition);
	LONG	GetCurrentPos() const;
	DWORD	Read(BYTE* lpDest, DWORD dwSize);
	WAVEFORMATEX* GetFormat() const { return m_pDestFormat; }

protected:
	LRESULT InnerRead(BYTE* lpDest, DWORD dwSize, DWORD& dwReaded);

	// �t�@�C������p��IFile�𓾂�
	smart_ptr<IFile>	GetFile() const { return m_vFile; }
	smart_ptr<IFile>	m_vFile;

	// Open�p�̃w���p�֐�
	// mmioOpen���AWAVEFORMATEX�̐ݒ�A�f�[�^�T�C�Y�̎擾�Ȃǂ��s��
	LRESULT	InitializeNormal();
	// MP3�t�@�C���𒼐ڃI�[�v������
	LRESULT	InitializeMP3();

	// �P�ʕϊ��p�w���p�֐��i0xffffffff�̓G���[�j
	LONG	ByteToMsec(DWORD dwByte) const;
	DWORD	MsecToByte(LONG lMsec) const;

	// HMMIO
	HMMIO	m_hmmio;
	// WAVEFORMATEX
	WAVEFORMATEX*		 m_pSrcFormat;
	WAVEFORMATEX*		 m_pDestFormat;		// const�֐��ł��Ԃ���悤��
	WAVEFORMATEX		 m_vDestFormat;		// ���̎��̂͂�����^^;
	MPEGLAYER3WAVEFORMAT m_vMP3Format;		// �����mp3�̃w�b�_�[��p�ӂ���
	// HACMSTREAM
	HACMSTREAM		 m_hAcm;
	// ACMSTREAMHEADER
	ACMSTREAMHEADER	 m_vAcmHeader;

	// �ϊ����f�[�^�̑S��(byte)
	DWORD	m_dwSrcLength;
	// �ϊ����f�[�^�ւ̃t�@�C���擪����̃I�t�Z�b�g
	DWORD	m_dwDataOffset;
	// �ϊ����f�[�^��̈ʒu
	DWORD	m_dwReadPosition;

	// �ϊ���f�[�^�̑S��(msec)
	DWORD	m_lDestLength;
	// ���܂łɏ������񂾃T�C�Y
	DWORD	m_dwDestTotalWritten;

	// ACM�̓u���b�N�����Z���o�b�t�@���󂯕t���Ȃ�
	DWORD m_dwSrcBlockLength;
	DWORD m_dwDstBlockLength;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneACMStream_h__
