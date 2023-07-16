//	 re-programmed by yaneurao(M.Isozaki) '99/07/07
//	 re-programmed by ENRA				  '02/03/23

#include "stdafx.h"
#include "../Auxiliary/yaneFile.h"

#include "yaneWaveOutput.h"
#include "yaneSoundParameter.h"
#include "yaneSoundBuffer.h"
#include "yaneSoundStream.h"

#include "yaneWaveStaticSound.h"
//#include "../Window/yaneDebugWindow.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

#define sign(val) (val>0?1:(val<0?-1:0))	//	符号を返す

//////////////////////////////////////////////////////////////////////////

CWaveStaticSound::CWaveStaticSound(IWaveOutput* p) : m_pOutput(p)
{
	m_bPaused = 0;			//	参照カウント方式
	m_nAvgBytesPerSec = 0;

	// デフォルトでループしない
	SetLoopPlay(false);

	// NullSoundBufferをつっこんどく
	SetBuffer(smart_ptr<ISoundBuffer>(new CNullSoundBuffer));

	// 面倒見てください
	GetOutput()->AddSound(this);
}

CWaveStaticSound::~CWaveStaticSound(){
	Release();

	// さよならー
	GetOutput()->DelSound(this);
}

/*
//////////////////////////////////////////////////////////////////////////
//--- 修正 '02/01/10  by ENRA ---
LRESULT CWaveStaticSound::Load(string filename){
	DWORD dwDataLen = 0;

	// この関数、DirectSoundのなかで、いっちゃん手間なんちゃう？
	if (GetDirectSound() == NULL) return 1;	// お疲れさん！

	Release(); // まずは、解放しようね！
	m_szFileName.erase();
	if (filename.empty()) return 0;

	// 相対パス厳禁
	filename = CFile::MakeFullName(filename);

	// Factoryあらへんやん(;´Д`)
	if(m_lpReaderFactory==NULL){ m_lpReaderFactory.Add(); }
	// オープンできるやつおるかー？
	mtknLib::IkmPCMstream* pReader = m_lpReaderFactory->CreateReader(filename.c_str());
	if (pReader==NULL) {
		Err.Out("CWaveStaticSound::Load " + filename + "のファイル読み込み時のエラー");
		return 1; // ファイル読み込みエラー
	}

	// 必要なバッファ長を得る
	dwDataLen = pReader->GetPCMLength();

	// やっとこDirectSoundの初期化
	DSBUFFERDESC  dsbdDesc;
	ZeroMemory(&dsbdDesc, sizeof(DSBUFFERDESC));
	dsbdDesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY; // ボリュームだけで結構！！
	// dsbdDesc.dwFlags = DSBCAPS_LOCSOFTWARE;
	// 大量のデータを扱う場合、ソフトメモリでやったほうが問題が少ないのだが
	// （ハードウェアにあるとかえってミキシングに時間がかかる可能性がある）
	// 音質が悪くなる可能性あり。
	// あと、ボリュームコントロール能力を持たせておく必要あり。
	// ハードウェアバッファがコントロール能力を持つと、
	// 処理に時間がかかるかも...

	// | DSBCAPS_GETCURRENTPOSITION2;
	// これを入れると、演奏中かどうかのステータスが少し遅くなるので
	// ステータスを見ての連続再生ができなくなる！

	// | DSBCAPS_GLOBALFOCUS
	// こいつを入れると、BackGroundでも音が鳴り続ける

	dsbdDesc.dwBufferBytes = dwDataLen;
	dsbdDesc.lpwfxFormat = pReader->GetFormat();

	// DirectSoundバッファの作成
	if (GetDirectSound()->CreateSoundBuffer(&dsbdDesc,&m_lpDSBuffer, NULL) != DS_OK){
		//	周波数変更のコントロールがまずかったんか？
		dsbdDesc.dwFlags = DSBCAPS_CTRLVOLUME; // ボリュームだけならどうよ！？
		if (GetDirectSound()->CreateSoundBuffer(&dsbdDesc,&m_lpDSBuffer, NULL) != DS_OK){
			Err.Out("CWaveStaticSound::LoadのCreateSoundBuffer()で失敗！");
			return 7;
		}
	}

	// DirectSoundバッファをLock
	BYTE* lpDSBuffData;
	HRESULT hr;
	hr = m_lpDSBuffer->Lock(0, dwDataLen, (void**)&lpDSBuffData,
		&dwDataLen, NULL, 0, 0);
		// これは、実は、失敗することは多々有るのだ:p

	if (hr==DSERR_BUFFERLOST){
		m_lpDSBuffer->Restore(); // これでオッケ!（笑）
		hr = m_lpDSBuffer->Lock(0, dwDataLen, (void**)&lpDSBuffData,
			&dwDataLen, NULL, 0, 0);
		// んで、もっかいリトライするの！！
	}

	if (hr!=DS_OK) {
		// これでダメなら、メモリ足りんのちゃう？
		Err.Out("CWaveStaticSound::LoadのLock()に失敗！");
		return 8;
	}

	// ファイルを展開
	pReader->SetLoop(false);
	pReader->Read(lpDSBuffData, dsbdDesc.dwBufferBytes);
	// DLLから出張中なのでsmart_ptrは使っちゃだめだめ。
	pReader->DeleteSelf();  pReader=NULL;

	// DirectSoundバッファのUnlock...
	if (m_lpDSBuffer->Unlock(lpDSBuffData, dwDataLen, NULL, 0)
		!= DS_OK) {
		// こんなんふつー、失敗するかぁ...どないせーちゅーんじゃ
		Err.Out("CWaveStaticSound::LoadのUnlock()に失敗！");
		return 10;
	}

	//	秒間の再生秒数
	WAVEFORMATEX wfe;
	if( (m_lpDSBuffer->GetFormat( &wfe, sizeof(wfe), NULL )) == DS_OK &&
		(wfe.nAvgBytesPerSec)){
		m_nAvgBytesPerSec = wfe.nAvgBytesPerSec;
	} else {
		//	おかしいなぁ。これエラーになるんかい？
		m_nAvgBytesPerSec = 0;
	}

	m_szFileName = filename;
	m_lLength = LONG(((DWORDLONG)dwDataLen * 1000) / m_nAvgBytesPerSec);

	return 0;
}
//-------------------------------
*/

LRESULT CWaveStaticSound::Load(const string& filename){
	Release(); // まずは、解放しようね！
	m_strFileName.erase();
	if (filename.empty()) return 0;

	smart_ptr<ISoundStream> stream = GetOutput()->GetSoundParameter()->GetStreamFactory()->Open(filename);
	LPCWAVEFORMATEX pWFormat = stream->GetFormat();
	if (pWFormat==NULL) return 0;
	const DWORD dwDataLen = (LONG)((DWORDLONG)stream->GetLength() * pWFormat->nAvgBytesPerSec /1000);
	// DirectSoundバッファの作成
	SetBuffer(GetOutput()->CreateBuffer());
	if (GetBuffer()->Create(pWFormat, dwDataLen, false, GetOutput()->GetSoundParameter().get()) != 0){
		Err.Out("CWaveStaticSound::Load サウンドバッファの作成に失敗！");
		// NullSoundBufferをつっこんどく
		SetBuffer(smart_ptr<ISoundBuffer>(new CNullSoundBuffer));
		return 1;
	}

	// DirectSoundバッファをLock
	BYTE* lpLockedBuffer;
	DWORD dwLockedSize;
	if (GetBuffer()->Lock(0, dwDataLen, &lpLockedBuffer, &dwLockedSize) != 0) {
		// これでダメなら、メモリ足りんのちゃう？
		Err.Out("CWaveStaticSound::Load GetBuffer()->Lock()に失敗！");
		return 2;
	}

	// 読み込み～
	const DWORD dwReadedSize = stream->Read(lpLockedBuffer, dwLockedSize);
	if ( dwReadedSize > dwLockedSize ) {	//	エラーと考えられる
		Err.Out("CWaveStaticSound::Load stream->Read()に失敗！");
		//	忘れずにUnlockせんとな
		GetBuffer()->Unlock(lpLockedBuffer, dwLockedSize);
		return 3;
	}

	// DirectSoundバッファのUnlock...
	if (GetBuffer()->Unlock(lpLockedBuffer, dwLockedSize) != 0) {
		// こんなんふつー、失敗するかぁ...どないせーちゅーんじゃ
		Err.Out("CWaveStaticSound::Load GetBuffer()->Unlock()に失敗！");
		return 4;
	}

	//	秒間の再生秒数
	WAVEFORMATEX wfe;
	if( GetBuffer()->GetFormat(&wfe)==0 ) {
		m_nAvgBytesPerSec = wfe.nAvgBytesPerSec;
	} else {
		//	おかしいなぁ。これエラーになるんかい？
		m_nAvgBytesPerSec = 0;
	}

	m_strFileName = filename;
	m_lLength = (LONG)( ((DWORDLONG)dwDataLen*1000)/m_nAvgBytesPerSec );

	return 0;
}

LRESULT CWaveStaticSound::Release(){
	if (IsPlay()) Stop();
	// 再生中ならとめんといかんよ！ （一応ね）

	// NullSoundBufferをつっこんどく
	SetBuffer(smart_ptr<ISoundBuffer>(new CNullSoundBuffer));
	m_strFileName.erase();

	m_nAvgBytesPerSec = 0;

	return 0;
}

LRESULT CWaveStaticSound::Restore(){
	DWORD dwStatus = GetBuffer()->GetStatus();
	if (dwStatus==1){
		GetBuffer()->Restore();
		if (m_strFileName!="") {
			const string temp(m_strFileName); // コピーして渡さんとReleaseされちゃう:p
			return Load(temp);
		}
	}

	return 0;
}

LRESULT CWaveStaticSound::Play()
{
	m_bPaused = 0;

	// 再生中なら、止めてでも再生しなおす。
	// 本来なら、演奏中なら止めずにカレントポジションだけ変更しておけば
	// 良いはずなのだが、その瞬間にPLAYが終了するという可能性もある。
	// （なんか、DirectSoundのバグのような気もしなくもない）

	// そこで、カレントポジションを先頭に戻して、プレイが止まって
	// いれば、再生というアルゴリズムに変更する。
	
	GetBuffer()->SetCurrentPosition(0);
	// 頭に戻してからのほーがPlay判定正確やね (C) yaneurao
	if (GetBuffer()->GetStatus()==3) {		
		GetBuffer()->Stop(); // 突然止めたら少しノイズ入るけどな。仕方ないわな。
		GetBuffer()->SetCurrentPosition(0);
	}

	if (GetBuffer()->Play(m_bLoop)!=0){
		if (GetBuffer()->GetStatus()==1) {		
			// えらいこっちゃ。バッファLOSTしちょる。restoreするナリ！
			return Restore();
		}
		Err.Out("CWaveStaticSound::Play GetBuffer()->Play()に失敗！");
		return 1;
	}

	return 0;
}

	//////////////////////////////////////////////////////////////////////////

LRESULT CWaveStaticSound::Stop()
{
	if (!IsPlay()) return 0; // もう止まってるっちゅーに！

	LRESULT ret = GetBuffer()->Stop();
	GetBuffer()->SetCurrentPosition(0); // もどしとかな！

	return ret;
}

bool CWaveStaticSound::IsPlay() const
// そのチャンネルが再生中(=3)ならtrue
{
	return GetBuffer()->GetStatus()==3;
}

LRESULT CWaveStaticSound::Pause()
 // 特定チャンネルのpause
{
	m_bPaused += sign(m_bPaused);	//	必殺技:p
	if (!IsPlay()) {
		return 0; // もう終わってるっちゅーに！
	}
	//	再生中のをpauseしたんならば
	m_bPaused = 1;

	// ここに抜けてきたっちゅーことは、m_lpDSBuffer!=NULL
	return GetBuffer()->Stop();
}

LRESULT CWaveStaticSound::Replay()
// pauseで止めていた、そのチャンネルの再生を再開。
{
	if (!m_bPaused) return 0; // pauseしてへんて！
	
	if (--m_bPaused==0) {	//	参照カウント方式
		return GetBuffer()->Play(m_bLoop);
	}
	return 0;
}

LRESULT CWaveStaticSound::SetVolume(LONG volume)
{
	// 範囲外のエラーチェックせーへんから、勝手にやってやー
	return GetBuffer()->SetVolume(volume);

	// DSBVOLUME_MAX（減衰なし） :		0
	// DSBVOLUME_MIN（無音）	 : -10000
	// の間で指定するのよん。
}

LONG CWaveStaticSound::GetVolume() const
// 取得は、特定チャンネルに対してしかできない...
{
	LONG volume;
	if ((GetBuffer()->GetVolume(volume))!=DS_OK){
		Err.Out("CWaveStaticSound::GetVolumeに失敗");
		return 0; // volumeはDSBVOLUME_MAXを返しとこか?
	}
	return volume;
}

LONG CWaveStaticSound::GetCurrentPos() const
{
	if (m_nAvgBytesPerSec==0) return -1;	//	Loadの情報取得で失敗している
	
	DWORD pos, dm;
	if( (GetBuffer()->GetCurrentPosition( pos, dm )) != 0 ) return -1;
	return LONG(((DWORDLONG) pos * 1000 / m_nAvgBytesPerSec ) );
	//	DWORDLONGにキャストしないと溢れちゃう:p
}

LRESULT CWaveStaticSound::SetCurrentPos(LONG lPos){
	if (m_nAvgBytesPerSec==0) return -1;	//	Loadの情報取得で失敗している

	DWORD pos;
	pos = (DWORD)((DWORDLONG)lPos * m_nAvgBytesPerSec / 1000);
	return GetBuffer()->SetCurrentPosition(pos);
}

//	bmdさんの提案
LONG CWaveStaticSound::GetFrequency() const
{
	DWORD dwFreq;
	if( GetBuffer()->GetFrequency( dwFreq ) != 0 ) return -1;
	return( (LONG)dwFreq );
}

LRESULT CWaveStaticSound::SetFrequency( DWORD freq )
{
	if((DSBFREQUENCY_MIN<=freq && freq<=DSBFREQUENCY_MAX) || freq==DSBFREQUENCY_ORIGINAL)
		if( GetBuffer()->SetFrequency( freq ) == 0 ) return 0;
	return 1;
}

LRESULT	CWaveStaticSound::SetLoopPlay(bool bLoop)
{
	m_bLoop = bLoop;
	return 0;
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
