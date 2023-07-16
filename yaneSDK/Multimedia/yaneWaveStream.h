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
	wav形式のサウンドデータを読み込む class ISoundStream 派生クラス
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
	/// ファイル操作用のIFileを得る
	smart_ptr<IFile>	GetFile() const { return m_vFile; }
	smart_ptr<IFile>	m_vFile;

	/// Open用のヘルパ関数
	/// mmioOpenし、WAVEFORMATEXの設定、データサイズの取得などを行う
	LRESULT	Initialize();

	/// 単位変換用ヘルパ関数（0xffffffffはエラー）
	LONG	ByteToMsec(DWORD dwByte) const;
	DWORD	MsecToByte(LONG lMsec) const;

	// ファイルを丸ごとメモリに読み込んでいるならtrue
	bool	m_bStaticOpen;
	// WAVEFORMATEX
	WAVEFORMATEX m_vWaveFormat;
	// 波形データへのファイル先頭からのオフセット
	DWORD	m_dwDataOffset;
	// 波形データへの長さ
	DWORD	m_dwDataLength;
	// 現在の波形データ上の位置
	DWORD	m_dwReadPosition;
	HMMIO	m_hmmio;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneWaveStream_h__
