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
	ACMを利用してサウンドデータを読み込む class ISoundStream 派生クラス
	圧縮ファイルはHDDから逐次読み出しするのではなく、一気に読み込む。
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

	// ファイル操作用のIFileを得る
	smart_ptr<IFile>	GetFile() const { return m_vFile; }
	smart_ptr<IFile>	m_vFile;

	// Open用のヘルパ関数
	// mmioOpenし、WAVEFORMATEXの設定、データサイズの取得などを行う
	LRESULT	InitializeNormal();
	// MP3ファイルを直接オープンする
	LRESULT	InitializeMP3();

	// 単位変換用ヘルパ関数（0xffffffffはエラー）
	LONG	ByteToMsec(DWORD dwByte) const;
	DWORD	MsecToByte(LONG lMsec) const;

	// HMMIO
	HMMIO	m_hmmio;
	// WAVEFORMATEX
	WAVEFORMATEX*		 m_pSrcFormat;
	WAVEFORMATEX*		 m_pDestFormat;		// const関数でも返せるように
	WAVEFORMATEX		 m_vDestFormat;		// ↑の実体はこいつ^^;
	MPEGLAYER3WAVEFORMAT m_vMP3Format;		// 無理矢理mp3のヘッダーを用意する
	// HACMSTREAM
	HACMSTREAM		 m_hAcm;
	// ACMSTREAMHEADER
	ACMSTREAMHEADER	 m_vAcmHeader;

	// 変換元データの全長(byte)
	DWORD	m_dwSrcLength;
	// 変換元データへのファイル先頭からのオフセット
	DWORD	m_dwDataOffset;
	// 変換元データ上の位置
	DWORD	m_dwReadPosition;

	// 変換先データの全長(msec)
	DWORD	m_lDestLength;
	// 今までに書き込んだサイズ
	DWORD	m_dwDestTotalWritten;

	// ACMはブロック長より短いバッファを受け付けない
	DWORD m_dwSrcBlockLength;
	DWORD m_dwDstBlockLength;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneACMStream_h__
