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
	// 閉じる
	Close();
}

CWaveStream::~CWaveStream()
{
	// 閉じる
	Close();
}

LRESULT	CWaveStream::Open(const char* filename)
{
	// 閉じる
	Close();

	// なるべくストリーミングの方向でオープン
	m_bStaticOpen = false;
	if (GetFile()->Open(filename, "rb")!=0){
		// しゃーない丸ごと読み込もう
		m_bStaticOpen = true;
		if (GetFile()->Read(filename)) {
			Err.Out("CWaveStream::Load %sのファイル読み込み時のエラー", filename);
			return 1; // ファイル読み込みエラー
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
	// やだなぁこんなやり方
	return &(const_cast<CWaveStream*>(this))->m_vWaveFormat;
}

LRESULT CWaveStream::Initialize()
{
	if (m_bStaticOpen){
		// データは、メモリ上、f.fileadrからf.filesize分だけある。
		MMIOINFO mmioinfo; // メモリファイルをmmioOpenするのだ！！(C)yaneurao
		ZeroMemory(&mmioinfo, sizeof(mmioinfo));
		mmioinfo.pchBuffer = (LPSTR)GetFile()->GetMemory();	// こいつにファイルバッファadr
		mmioinfo.fccIOProc = FOURCC_MEM;					// メモリから！
		mmioinfo.cchBuffer = GetFile()->GetSize();			// こいつにファイルサイズ

		// やっとオープンできる。（先は長いぞー）
		m_hmmio = mmioOpen(NULL, &mmioinfo, MMIO_READ);
		if (m_hmmio==NULL) {
			Err.Out("CWaveStream::Initialize mmioOpenに失敗");
			return 1;
		}
	} else {
		// ファイルをオープンしてもらう
		m_hmmio = mmioOpen(const_cast<LPSTR>(GetFile()->GetName().c_str()), NULL, MMIO_READ);
		if (m_hmmio==NULL) {
			Err.Out("CWaveStream::Initialize mmioOpenに失敗");
			return 1;
		}
	}

	MMCKINFO ciPC,ciSC; // 親チャンクとサブチャンク
	ciPC.fccType = mmioFOURCC('W','A','V','E');
	if (mmioDescend(m_hmmio,(LPMMCKINFO)&ciPC,NULL,MMIO_FINDRIFF)){
		Err.Out("CWaveStream::Initialize Waveファイルでない");
		return 2;
	}
	ciSC.ckid = mmioFOURCC('f','m','t',' ');
	if (mmioDescend(m_hmmio,&ciSC,&ciPC,MMIO_FINDCHUNK)){
		Err.Out("CWaveStream::Initialize fmtチャンクが指定できない");
		return 3;
	}

	// 場合分けめんどいからmmioRead
	mmioRead(m_hmmio, (HPSTR)GetFormat(), sizeof(WAVEFORMATEX));
	if (GetFormat()->wFormatTag != WAVE_FORMAT_PCM){
		Err.Out("CWaveStream::Initialize WAVE_FORMAT_PCMじゃない");
		return 4;
	}

	mmioAscend(m_hmmio,&ciSC,0); // fmtサブチャンク外部へ移動
	ciSC.ckid = mmioFOURCC('d','a','t','a');
	if (mmioDescend(m_hmmio,&ciSC,&ciPC,MMIO_FINDCHUNK)){
		Err.Out("CWaveStream::Initialize データチャンクに行けない");
		return 5;
	}

	// さっきと同じ手法でデータへのポインタを得る
	m_dwDataLength   = ciSC.cksize; // データサイズ
	m_dwDataOffset   = mmioSeek(m_hmmio,0,SEEK_CUR);

	SetCurrentPos(0);

	return 0;
}

LRESULT	CWaveStream::SetCurrentPos(LONG lPosition)
{
	DWORD dwBytePosition = MsecToByte(lPosition);
	if (dwBytePosition==0xffffffff) return 1;	// 読み込んでないんちゃう？

	if (lPosition >= GetLength()) return 2;		// 飛ばしすぎなんじゃないの？

	m_dwReadPosition = dwBytePosition;
	if (m_bStaticOpen){
		// 特にする事はない
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
		// サイズ超えないかな？
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
