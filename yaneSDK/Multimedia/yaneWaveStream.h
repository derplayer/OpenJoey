// enraWaveStream.h
//		programmed by ENRA		'02/03/25

#ifndef __yaneWaveStream_h__
#define __yaneWaveStream_h__


#include "yaneSoundStream.h"

namespace yaneuraoGameSDK3rd { namespace Auxiliary { class IFile;}}

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class CWaveStream : public ISoundStream {
/**
	wav�`���̃T�E���h�f�[�^��ǂݍ��� class ISoundStream �h���N���X
*/
public:
	CWaveStream();
	virtual ~CWaveStream();

	/// override from class ISoundStream
	LRESULT	Open(const char* filename);
	LRESULT	Close();
	LONG	GetLength() const;
	LRESULT	SetCurrentPos(LONG lPosition);
	LONG	GetCurrentPos() const;
	DWORD	Read(BYTE* lpDest, DWORD dwSize);
	WAVEFORMATEX* GetFormat() const;

protected:
	/// �t�@�C������p��IFile�𓾂�
	smart_ptr<IFile>	GetFile() const { return m_vFile; }
	smart_ptr<IFile>	m_vFile;

	/// Open�p�̃w���p�֐�
	/// mmioOpen���AWAVEFORMATEX�̐ݒ�A�f�[�^�T�C�Y�̎擾�Ȃǂ��s��
	LRESULT	Initialize();

	/// �P�ʕϊ��p�w���p�֐��i0xffffffff�̓G���[�j
	LONG	ByteToMsec(DWORD dwByte) const;
	DWORD	MsecToByte(LONG lMsec) const;

	// �t�@�C�����ۂ��ƃ������ɓǂݍ���ł���Ȃ�true
	bool	m_bStaticOpen;
	// WAVEFORMATEX
	WAVEFORMATEX m_vWaveFormat;
	// �g�`�f�[�^�ւ̃t�@�C���擪����̃I�t�Z�b�g
	DWORD	m_dwDataOffset;
	// �g�`�f�[�^�ւ̒���
	DWORD	m_dwDataLength;
	// ���݂̔g�`�f�[�^��̈ʒu
	DWORD	m_dwReadPosition;
	HMMIO	m_hmmio;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneWaveStream_h__
