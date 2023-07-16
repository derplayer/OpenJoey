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
	// 閉じる
	Close();
}

CACMStream::~CACMStream()
{
	// 閉じる
	Close();
}

LRESULT	CACMStream::Open(const char* filename)
{
	// 閉じる
	Close();

	// 圧縮ファイルなんだから丸ごと読み込む
	if ( LRESULT ret = GetFile()->Read(filename) ) {
		Err.Out("CACMStream::Open ファイル読み込みに失敗(ret=%d) - %s", ret, filename);
		return 1; // ファイル読み込みエラー
	}

	// RIFF形式と仮定してオープン
	LRESULT ret = InitializeNormal();
	// あれー？MP3形式なんかなぁ
	if (ret==2){
		ret = InitializeMP3();
	}
	// あかんかった
	if (ret!=0){
		Err.Out("CACMStream::Open オープンに失敗(ret=%d) - %s", ret, filename);
		Close();
		return 2;
	}

	// ACMを初期化する
	m_vDestFormat.wFormatTag = WAVE_FORMAT_PCM;	//	PCMになって欲しいねん！
	if (acmFormatSuggest(NULL,m_pSrcFormat, m_pDestFormat, sizeof(WAVEFORMATEX), ACM_FORMATSUGGESTF_WFORMATTAG)!=0){
		Err.Out("CACMStream::Open acmFormatSuggest()に失敗");
		Close();
		return 3;	//	acm無いんとちゃう？
	}
	if (acmStreamOpen(&m_hAcm, NULL, m_pSrcFormat, m_pDestFormat, NULL, NULL, NULL, 0/*ACM_STREAMOPENF_NONREALTIME*/)!=0){
		Err.Out("CACMStream::Open acmStreamOpen()に失敗");
		Close();
		return 4;	//	acmおかしんとちゃう？
	}
	// 最小ブロック長を求める
	if (acmStreamSize(m_hAcm, m_dwSrcBlockLength, &m_dwDstBlockLength, ACM_STREAMSIZEF_SOURCE) !=0 ){
		Err.Out("CACMStream::Open acmStreamSize()に失敗");
		Close();
		return 5;	//	なんでやねんと言いたい（笑）
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
	// データは、メモリ上、f.fileadrからf.filesize分だけある。
	MMIOINFO mmioinfo; // メモリファイルをmmioOpenするのだ！！(C)yaneurao
	ZeroMemory(&mmioinfo, sizeof(mmioinfo));
	mmioinfo.pchBuffer = (LPSTR)GetFile()->GetMemory();	// こいつにファイルバッファadr
	mmioinfo.fccIOProc = FOURCC_MEM;					// メモリから！
	mmioinfo.cchBuffer = GetFile()->GetSize();			// こいつにファイルサイズ

	// やっとオープンできる。（先は長いぞー）
	m_hmmio = mmioOpen(NULL, &mmioinfo, MMIO_READ);
	if (m_hmmio==NULL) {
		Err.Out("CACMStream::InitializeNormal mmioOpenに失敗");
		return 1;
	}

	MMCKINFO ciPC,ciSC; // 親チャンクとサブチャンク
	ciPC.fccType = mmioFOURCC('W','A','V','E');
	if (mmioDescend(m_hmmio,(LPMMCKINFO)&ciPC,NULL,MMIO_FINDRIFF)){
		Err.Out("CACMStream::InitializeNormal Waveファイルでない");
		return 2;
	}
	ciSC.ckid = mmioFOURCC('f','m','t',' ');
	if (mmioDescend(m_hmmio,&ciSC,&ciPC,MMIO_FINDCHUNK)){
		Err.Out("CACMStream::InitializeNormal fmtチャンクが指定できない");
		return 3;
	}

	// WAVEFORMATEXが可変サイズなので、ポインタを直接突っ込む
	m_pSrcFormat = (WAVEFORMATEX*)((BYTE*)GetFile()->GetMemory() + mmioSeek(m_hmmio,0,SEEK_CUR));
	if (m_pSrcFormat->cbSize==0) { m_pSrcFormat->cbSize = sizeof(WAVEFORMATEX); }
	if (m_pSrcFormat->wFormatTag == WAVE_FORMAT_PCM){
		Err.Out("CACMStream::InitializeNormal ACMを使うまでもない");
		return 4;
	}

	mmioAscend(m_hmmio,&ciSC,0); // fmtサブチャンク外部へ移動
	ciSC.ckid = mmioFOURCC('d','a','t','a');
	if (mmioDescend(m_hmmio,&ciSC,&ciPC,MMIO_FINDCHUNK)){
		Err.Out("CACMStream::InitializeNormal データチャンクに行けない");
		return 4;
	}

	// さっきと同じ手法でデータへのポインタを得る
	m_dwSrcLength = ciSC.cksize; // データサイズ
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
		Err.Out("CACMStream::InitializeMP3 ファイルサイズが異常(dwFileSize=%d)", dwFileSize);
		return 1; // そんな小さいわけあらへんがな:p
	}

	//	フレームヘッダからWAVEFORMATEXのデータを用意する
	const unsigned char* const src_begin	= (unsigned char*)GetFile()->GetMemory();
	const unsigned char* const src_end		= src_begin + dwFileSize;
	const unsigned char* src				= src_begin;

	//  ID3v2タグがついているならば、読み飛ばす
	if ((src[0] == 'I') && (src[1] == 'D') && (src[2] == '3')) {
		DWORD dwID3Size = src[9] + (src[8]<<7) + (src[7]<<14) + (src[6]<<21);
		// バージョンチェック
		if(src[3]>=0x04)
		{
			// ID3v2.4.0以降
			if(src[5]&0x10){	// フッタの有無
				dwID3Size+=20; // フッタあり
			}else{
				dwID3Size+=10; // フッタなし
			}
		}else{
			// ID3v2.3.0以前
			dwID3Size+=10; // フッタなし
		}
		
		if (availSize <= dwID3Size + 128) {
			Err.Out("CACMStream::InitializeMP3 ID3ヘッダ長が異常(availSize=%d, dwID3Size=%d)", availSize, dwID3Size);
			return 1;
		}
		
		src += dwID3Size;
		availSize -= dwID3Size;
		dwSkipLen += dwID3Size;
	}

	//	MP3チェック
	if ( src[0]!=0xff ) {
		Err.Out("CACMStream::InitializeMP3 MP3チェックphase1に失敗(src[0]=0x%X)", src[0]);
		return 1;
	}
	if ( (src[1]&0xf8)!=0xf8 ) {
		Err.Out("CACMStream::InitializeMP3 MP3チェックphase2に失敗(src[1]=0x%X)", src[1]);
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
		Err.Out("CACMStream::InitializeMP3 MP3が無効なレイヤ");
		return 1;
	}

	const int nMpeg		 = ((src[1] & 8) == 0) ? 1 : 0;
	const int nBitrate	 = anBitrate[nMpeg][nLayer-1][ src[2]>>4 ];
	const int nFreq		 = anFreq[nMpeg][(src[2] >> 2) & 3];
	const int nChannel	 = ((src[3] >> 6) == 3) ? 1 : 2;
	const int nFrameSize = (72*(nMpeg+1)*1000) * nBitrate / nFreq;

	//	無理矢理MP3のタグを用意する
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
	//	後ろにID3タグがついているならば、その分を除外する
	//	dwFileSize > 128 であることは保証されている
	if ((src_end[-128] == 'T') && (src_end[-127] == 'A') && (src_end[-126] == 'G')) {
		availSize -= 128;
	}
	availSize -= 4;
	dwSkipLen += 4;

	m_dwSrcLength = availSize;
	m_dwDataOffset = dwSkipLen;

	// このWAVEFORMATEXを使ってちょ
	m_pSrcFormat = &m_vMP3Format.wfx;
	m_dwSrcBlockLength = m_vMP3Format.nBlockSize;

	m_lDestLength = (m_dwSrcLength*8/nBitrate);

	SetCurrentPos(0);

	return 0;
}

LRESULT	CACMStream::SetCurrentPos(LONG lPosition)
{
	DWORD dwBytePosition = MsecToByte(lPosition);
	if (dwBytePosition==0xffffffff) return 1;	// 読み込んでないんちゃう？

	if (lPosition >= GetLength()) return 2;		// 飛ばしすぎなんじゃないの？

	// ACMを再初期化する
	if (m_hAcm!=NULL) {
		acmStreamClose(m_hAcm, 0);
		m_hAcm = NULL;
		ZERO(m_vAcmHeader);
		if (acmStreamOpen(&m_hAcm, NULL, m_pSrcFormat, m_pDestFormat, NULL, NULL, NULL, ACM_STREAMOPENF_NONREALTIME)!=0){
			Err.Out("CACMStream::SetCurrentPos acmStreamOpen()に失敗");
			return 3;	//	acmおかしんとちゃう？
		}
	}	
	DWORD dwSrcPos = 0;
	acmStreamSize(m_hAcm, dwBytePosition, &dwSrcPos, ACM_STREAMSIZEF_DESTINATION);
	m_dwReadPosition = dwSrcPos;

	// ここまで書き込んだ事にする
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
	// 読んでないやん
	if ( GetFile()->GetMemory()==NULL || m_hAcm==NULL ) return 0xffffffff;

	// 終端やん
	if ( m_dwReadPosition==m_dwSrcLength ) return 0;

	DWORD dwReadedSize = 0;
	InnerRead(lpDest, dwSize, dwReadedSize);

	// こんだけ書き込んだよ
	m_dwDestTotalWritten += dwReadedSize;

	return dwReadedSize;
}

LRESULT CACMStream::InnerRead(BYTE* lpDest, DWORD dwSize, DWORD& dwReaded)
{
	// ACMSTREAMHEADERを初期化する
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

	// 設定する
	if(acmStreamPrepareHeader(m_hAcm, &m_vAcmHeader, NULL)!=0){
		Err.Out("CACMStream::Read acmStreamPrepareHeader()に失敗");
		return 3;
	}
	// 変換する
//	if(acmStreamConvert(m_hAcm, &m_vAcmHeader, 0)!=0){
	if(acmStreamConvert(m_hAcm, &m_vAcmHeader, ACM_STREAMCONVERTF_BLOCKALIGN)!=0){
		Err.Out("CACMStream::Read acmStreamConvert()に失敗");
		return 4;
	}
	// 設定を解除する
	if(acmStreamUnprepareHeader(m_hAcm, &m_vAcmHeader, NULL)!=0){
		Err.Out("CACMStream::Read acmStreamPrepareHeader()に失敗");
		return 5;
	}
	// 変換出来たサイズを調べる
	dwReaded = m_vAcmHeader.cbDstLengthUsed;
	if (dwReaded==0) {
		// この嘘つき！変換してないのにSrc使ったって言いやがるヽ(`Д´)ノ
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
