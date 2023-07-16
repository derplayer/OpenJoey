// yaneSoundStream.h
//		programmed by ENRA		'02/03/25

#ifndef __yaneSoundStream_h__
#define __yaneSoundStream_h__


namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class ISoundStream {
/**
	�T�E���h�f�[�^��PCM�`���œǂݏo�����߂̃C���^�[�t�F�[�X
*/
public:
	ISoundStream(){};
	virtual ~ISoundStream(){};

	/// �I�[�v������
	virtual LRESULT	Open(const char* filename) = 0;

	/// �N���[�Y����
	virtual LRESULT	Close() = 0;

	/// �T�E���h�f�[�^�̒�����msec�P�ʂŕԂ�
	virtual LONG	GetLength() const = 0;

	/// �Đ��ʒu��msec�P�ʂŐݒ肷��
	virtual LRESULT	SetCurrentPos(LONG lPosition) = 0;

	/// �Đ��ʒu��msec�P�ʂŕԂ�
	virtual LONG	GetCurrentPos() const = 0;

	/// lpDest�ɁAdwSize����PCM�`���œW�J����
	/// [in] lpDest - �W�J�o�b�t�@�ւ̃|�C���^
	/// [in] dwSize - �W�J�T�C�Y
	virtual DWORD	Read(BYTE* lpDest, DWORD dwSize) = 0;

	/// WAVEFORMATEX�\���̂�Ԃ�
	virtual WAVEFORMATEX* GetFormat() const = 0;
};

class CNullSoundStream : public ISoundStream {
/**
	class ISoundStream ��NullDevice�N���X
*/
public:
	CNullSoundStream(){};
	virtual ~CNullSoundStream(){};

	/// override from class ISoundStream
	LRESULT	Open(const char* filename) { return -1; }
	LRESULT	Close() { return -1; }
	LONG	GetLength() const { return -1; }
	LRESULT	SetCurrentPos(LONG dwPosition) { return -1; }
	LONG	GetCurrentPos() const { return -1; }
	DWORD	Read(BYTE* lpDest, DWORD dwSize) { return 0; }
	WAVEFORMATEX* GetFormat() const { return NULL; }
};

}}

namespace yaneuraoGameSDK3rd {namespace Auxiliary { class IStringMap; }}

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class ISoundStreamFactory {
/**
	class ISoundStream �h���N���X�𐶐�����factory �̃C���^�[�t�F�[�X
*/
public:
	ISoundStreamFactory(){};
	virtual ~ISoundStreamFactory(){};

	/// ISoundStream�𐶐�����
	virtual smart_ptr<ISoundStream> Open(const string& filename) = 0;

	IStringMap* GetPlugInMap() { return m_vPlugInMap.get(); }
	/**
		�g���q�ː�������T�E���hPlugIn�̃N���X��
		�ւ�map�ł��B
		ISoundStreamFactory::GetPlugInMap()->Write("ogg", "CVorbisStream");
		�̂悤�ɂ��ăv���O�C����ǉ����Ă����܂��B
	*/

protected:
	/// �g���q����PlugIn�̃N���X���ւ�map
	smart_ptr<IStringMap>	m_vPlugInMap;
};

class CSoundStreamFactory : public ISoundStreamFactory {
/**
	class ISoundStream �h���N���X�𐶐�����factory
*/
public:
	CSoundStreamFactory();
	virtual ~CSoundStreamFactory();

	/// override from class ISoundStreamFactory
	smart_ptr<ISoundStream> Open(const string& filename);
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd


#endif // __yaneSoundStream_h__
