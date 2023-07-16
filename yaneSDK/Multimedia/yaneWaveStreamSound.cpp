#include "stdafx.h"

#include "../Auxiliary/yaneFile.h"
#include "../Thread/yaneCriticalSection.h"
#include "yaneWaveOutput.h"
#include "yaneSoundParameter.h"
#include "yaneSoundBuffer.h"
#include "yaneSoundStream.h"

#include "yaneWaveStreamSound.h"
//#include "../Window/yaneDebugWindow.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

//////////////////////////////////////////////////////////////////////////

void CWaveStreamSound::ThreadProc()
{
	bool bWait = false;
	const DWORD dwBufferSize = GetBufferSize();
	while (true) {
		//	既に用無し…？
		if ( !IsThreadValid() ) return ;

		//	ファイル全体を通しての再生位置を検出する
		//	GetNextSoundData関数内のCThread::sleep関数の関係上、20msec程度の分解能しかない
		{	CCriticalLock lock(GetCS());
			const DWORD dwPrevPlayCursor = m_dwPrevPlayCursor;
			DWORD dwPlayCursor, dwWriteCursor;
			GetBuffer()->GetCurrentPosition(dwPlayCursor, dwWriteCursor);
			if ( dwPlayCursor >= dwPrevPlayCursor ) {
				m_dwPlayPosition += dwPlayCursor - dwPrevPlayCursor;
			} else {
				m_dwPlayPosition += (dwPlayCursor + dwBufferSize) - dwPrevPlayCursor;
			}
			m_dwPrevPlayCursor = dwPlayCursor;
			m_dwPlayPosition %= m_dwLength;
		}

		//	データを流し込む
		DWORD ret = GetNextSoundData();
		if ( ret==1 ) {	//	EOF
			if (IsLoopPlay()){
				//	ループ再生なら、ストリームを巻き戻して再生続行
				GetStream()->SetCurrentPos(0);
				continue ;
			} else {
				//	非ループ再生なら、終端までの再生の面倒を見る
				bWait = true;
				break ;
			}
		}
		ef ( ret>=2 ) {	//	エラー
			//	Stop()を呼ぶとスレッドの終了を待つのでデッドロックになっちまう
			InnerStop();
			return ;
		}
	}

	if ( bWait ) {
		//	再生カーソルが停止位置より前方にあると、
		//	dwPlayCursor >= m_dwNextWriteCursor という条件では
		//	正しい位置で停止出来ない
		//	（バッファの一周期分だけ早く停止してしまう）
		bool bForward = false;
		DWORD dwLimit = 0;
		{	CCriticalLock lock(GetCS());
			//dwLimit = m_dwNextWriteCursor - (m_nAvgBytesPerSec / 50);//	20msec手前で止める
			dwLimit = m_dwNextWriteCursor;
			DWORD dwPlayCursor, dwWriteCursor;
			GetBuffer()->GetCurrentPosition(dwPlayCursor, dwWriteCursor);
			if ( dwPlayCursor >= dwLimit ) {
				bForward = true;	//	再生カーソルは停止位置より前方
			}

			//	ごみの除去
			//	bForward ==	true  -> 停止位置～再生位置までクリア
			//				false -> 停止位置～終わりまでクリア
			const DWORD dwClearSize = (bForward) ? (dwPlayCursor - dwLimit) : (dwBufferSize - dwLimit);
			BYTE* lpLockedBuffer;
			DWORD dwLockedSize;
			//	ラップアラウンドすることはあり得ない
			if ( GetBuffer()->Lock(dwLimit, dwClearSize, &lpLockedBuffer, &dwLockedSize) ) {
				Err.Out("CWaveStreamSound::ThreadProc GetBuffer()->Lock()に失敗！");
				//	Stop()を呼ぶとスレッドの終了を待つのでデッドロックになっちまう
				InnerStop();
				return ;
			}
			memset(lpLockedBuffer, 0, dwLockedSize);
			if ( GetBuffer()->Unlock(lpLockedBuffer, dwLockedSize) ){
				Err.Out("CWaveStreamSound::ThreadProc GetBuffer()->Unlock()に失敗！");
				//	Stop()を呼ぶとスレッドの終了を待つのでデッドロックになっちまう
				InnerStop();
				return ;
			}
		}

		while (true) {
			//	既に用無し…？
			if ( !IsThreadValid() ) return ;

			//	ファイル全体を通しての再生位置を検出する
			//	GetNextSoundData関数内のCThread::sleep関数の関係上、20msec程度の分解能しかない
			{	CCriticalLock lock(GetCS());
				const DWORD dwPrevPlayCursor = m_dwPrevPlayCursor;
				DWORD dwPlayCursor, dwWriteCursor;
				GetBuffer()->GetCurrentPosition(dwPlayCursor, dwWriteCursor);
				if ( !bForward && dwPlayCursor >= dwLimit ){
					//	Stop()を呼ぶとスレッドの終了を待つのでデッドロックになっちまう
					InnerStop();
					return ;
				}
				if ( dwPlayCursor == dwPrevPlayCursor ) {
					lock.Leave();						//	ロック解除
					if ( CThread::sleep(20) ) return ;	//	しばし休息。Invalidateされたらすぐ終了
					continue ;
				} else {
					if ( dwPlayCursor > dwPrevPlayCursor ) {
						m_dwPlayPosition += dwPlayCursor - dwPrevPlayCursor;
					} else {
						m_dwPlayPosition += (dwPlayCursor + dwBufferSize) - dwPrevPlayCursor;
						//	再生カーソルは停止位置より後方に位置している事になる
						bForward = false;
					}
				}
				m_dwPrevPlayCursor = dwPlayCursor;
				m_dwPlayPosition %= m_dwLength;
			}
		}
	}
}

LRESULT CWaveStreamSound::GetNextSoundData()
{
	//	今から弄るんだから邪魔すんなよー
	CCriticalLock lock(GetCS());

	const DWORD dwBufferSize = GetBufferSize();

	//	消費したサイズを得る
	DWORD dwUsedSize = 0;
	DWORD dwPlayCursor, dwWriteCursor;
	GetBuffer()->GetCurrentPosition(dwPlayCursor, dwWriteCursor);
	{
		const DWORD dwPrevWriteCursor = m_dwPrevWriteCursor;
		if ( dwWriteCursor == dwPrevWriteCursor ) {	//	消費してないの？
			lock.Leave();		//	ロック解除
			CThread::sleep(20);	//	しばし休息
			return 0;
		} else {
			//dwUsedSize = ((dwBufferSize + dwWriteCursor) - dwPrevWriteCursor) % dwBufferSize;
			if ( dwWriteCursor > dwPrevWriteCursor ) {
				dwUsedSize = dwWriteCursor - dwPrevWriteCursor;
			} else {
				dwUsedSize = (dwWriteCursor + dwBufferSize) - dwPrevWriteCursor;
			}
		}
	}
	//	前回補充出来なかった分を上乗せ
	dwUsedSize += m_dwWriteDiffer;
	//	使用量が0.5秒分より小さいなら何もしない（CPUパワーの節約）
	if ( dwUsedSize < m_nAvgBytesPerSec/2 ) {
		lock.Leave();		//	ロック解除
		CThread::sleep(20);	//	しばし休息
		return 0;
	}
	m_dwWriteDiffer = 0;	//	忘れずにクリアー

	//	再生カーソルでクリッピングする
	{
		const DWORD dwNextWriteCursor = m_dwNextWriteCursor;
		//	ラップアラウンドを考慮する
		if ( (dwNextWriteCursor + dwUsedSize) > dwBufferSize ) {
			//	書き込み開始位置からバッファ終端までの長さ
			const DWORD dwWriteSize1 = dwBufferSize - dwNextWriteCursor;
			//	バッファ先端から書き込み終了位置までの長さ
			const DWORD dwWriteSize2 = dwUsedSize - dwWriteSize1;
			if ( dwWriteSize2 > dwPlayCursor ) {
				dwUsedSize = dwWriteSize1 + dwPlayCursor;
			}
		} else {
			if ( dwNextWriteCursor < dwPlayCursor&&dwPlayCursor < (dwNextWriteCursor + dwUsedSize) ) {
				dwUsedSize = dwPlayCursor - dwNextWriteCursor;
			}
		}
	}
	//	4バイト境界に合わせる
	dwUsedSize &= ~3;

	//	現在の書込カーソルを覚えておく
	m_dwPrevWriteCursor = dwWriteCursor;

	//	ストリームから読み込む
	smart_ptr<BYTE> pBuffer;
	pBuffer.AddArray(dwUsedSize);
	const DWORD readed = GetStream()->Read(pBuffer.get(), dwUsedSize);
	if ( readed==0 ) {
		return 1;	//	EOFでっせ
	}
	ef ( readed > dwUsedSize ) {	//	エラーと考えられる
		Err.Out("CWaveStreamSound::GetNextSoundData GetStream()->Read()に失敗！");
		return 2;
	}

	//	サウンドバッファに書き込む
	BYTE* lpLockedBuffer;
	DWORD dwLockedSize;
	BYTE* lpLockedBuffer2;
	DWORD dwLockedSize2;
	if ( GetBuffer()->Lock(m_dwNextWriteCursor, dwUsedSize, &lpLockedBuffer, &dwLockedSize, &lpLockedBuffer2, &dwLockedSize2) ) {
		Err.Out("CWaveStreamSound::GetNextSoundData GetBuffer()->Lock()に失敗！");
		return 3;
	}
	if ( readed < dwLockedSize ) {
		//	少し読めなかったらしい
		memcpy(lpLockedBuffer, pBuffer.get(), readed);
	} else {
		memcpy(lpLockedBuffer, pBuffer.get(), dwLockedSize);
	}
	if ( lpLockedBuffer2!=NULL && readed > dwLockedSize ) {
		//	ラップアラウンドした分をコピー
		if ( readed - dwLockedSize < dwLockedSize2 ) {
			//	少し読めなかったらしい
			memcpy(lpLockedBuffer2, pBuffer.get()+dwLockedSize, readed - dwLockedSize);
		} else {
			//	残りをきっちりコピー
			memcpy(lpLockedBuffer2, pBuffer.get()+dwLockedSize, dwLockedSize2);
		}
	}
	if ( GetBuffer()->Unlock(lpLockedBuffer, dwLockedSize, lpLockedBuffer2, dwLockedSize2) ) {
		Err.Out("CWaveStreamSound::GetNextSoundData GetBuffer()->Unlock()に失敗！");
		return 4;
	}

	//	次回書き込みカーソルを更新
	m_dwNextWriteCursor += readed;
	m_dwNextWriteCursor %= dwBufferSize;
	//	今回読み込めなかったサイズは次回にまわす
	if ( readed < dwUsedSize ) {
		m_dwWriteDiffer += dwUsedSize - readed;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

CWaveStreamSound::CWaveStreamSound(IWaveOutput* p) : m_pOutput(p)
{
	//	デフォルトでループしない
	SetLoopPlay(false);

	//	NullSoundBufferをつっこんどく
	SetBuffer(smart_ptr<ISoundBuffer>(new CNullSoundBuffer));
	//	なんも読み込んでないから０
	m_dwLength = 0;
	m_nAvgBytesPerSec = 0;
	//	参照カウント方式
	m_bPaused = 0;
	//	Volumeを初期化
	m_lVolume = 0;

	//	面倒見てください
	GetOutput()->AddSound(this);
}

CWaveStreamSound::~CWaveStreamSound(){
	Release();
	//	さよならー
	GetOutput()->DelSound(this);
}

LRESULT CWaveStreamSound::InitBuffer()
{
	//	今から弄るんだから邪魔すんなよー
	CCriticalLock lock(GetCS());

	//	ファイル読みこんでるの？
	if ( m_strFileName==""||m_nAvgBytesPerSec==0 ) {
		Err.Out("CWaveStreamSound::InitBuffer ファイルが読み込めてない");
		return 1;
	}

	//	読み込めてるのかチェック
	if ( GetStream()->GetFormat()==NULL ) {
		Err.Out("CWaveStreamSound::InitBuffer GetStream()->GetFormat()==NULL");
		return 2;
	}

	//	DirectSoundバッファの作成
	SetBuffer( GetOutput()->CreateBuffer() );
	if ( GetBuffer()->Create(GetStream()->GetFormat(), GetBufferSize(), true, GetOutput()->GetSoundParameter().get())!=0 ) {
		Err.Out("CWaveStreamSound::InitBuffer サウンドバッファの作成に失敗！");
		return 3;
	}

	//	カーソルを全て初期化
	m_dwPrevWriteCursor = 0;
	m_dwNextWriteCursor = 0;
	m_dwWriteDiffer = 0;
	m_dwPrevPlayCursor = 0;
	GetBuffer()->SetCurrentPosition(0);
	//	バッファにVolumeを反映させておく
	GetBuffer()->SetVolume(m_lVolume);

	//	バッファサイズの7/8くらいを読み込む
	DWORD dwUsedSize = (GetBufferSize()*7)/8;
	//	4バイト境界に合わせる
	dwUsedSize &= ~3;

	//	DirectSoundバッファをLock
	BYTE* lpLockedBuffer;
	DWORD dwLockedSize;
	if ( GetBuffer()->Lock(m_dwNextWriteCursor, dwUsedSize, &lpLockedBuffer, &dwLockedSize) ) {
		//	これでダメなら、メモリ足りんのちゃう？
		Err.Out("CWaveStreamSound::InitBuffer GetBuffer()->Lock()に失敗！");
		return 4;
	}
	const DWORD dwReadedSize = GetStream()->Read(lpLockedBuffer, dwLockedSize);
	if ( dwReadedSize > dwLockedSize ) {	//	エラーと考えられる
		Err.Out("CWaveStreamSound::InitBuffer GetStream()->Read()に失敗！");
		//	忘れずにUnlockせんとな
		GetBuffer()->Unlock(lpLockedBuffer, dwLockedSize);
		return 5;
	}
	if ( GetBuffer()->Unlock(lpLockedBuffer, dwLockedSize) ) {
		//	こんなんふつー、失敗するかぁ...どないせーちゅーんじゃ
		Err.Out("CWaveStreamSound::InitBuffer GetBuffer()->Unlock()に失敗！");
		return 6;
	}
	//	次回書き込みカーソルを更新
	m_dwNextWriteCursor += dwReadedSize;
	//	読み込みできなかったサイズは次回にまわす
	if ( dwReadedSize < dwUsedSize ) { m_dwWriteDiffer = dwUsedSize - dwReadedSize; }

	return 0;
}

LRESULT CWaveStreamSound::Load(const string& filename)
{
	Release(); //	まずは、解放しようね！
	m_strFileName.erase();
	if ( filename.empty() ) { return 0; }

	//	ファイルを開く
	SetStream( GetOutput()->GetSoundParameter()->GetStreamFactory()->Open(filename) );

	//	秒間の再生秒数
	{	LPCWAVEFORMATEX pWaveFormat = GetStream()->GetFormat();
		if ( pWaveFormat!=NULL ) {
			m_nAvgBytesPerSec = pWaveFormat->nAvgBytesPerSec;
		} else {
			//	おかしいなぁ。これエラーになるんかい？
			m_nAvgBytesPerSec = 0;
		}
	}

	m_strFileName = filename;
	m_dwLength = (DWORD)((LONGLONG)GetStream()->GetLength() * m_nAvgBytesPerSec / 1000);

	//	頭出し
	SetCurrentPos(0);

	return 0;
}

LRESULT CWaveStreamSound::Release(){
	if (IsPlay()) Stop();
	//	再生中ならとめんといかんよ！ （一応ね）

	//	今から弄るんだから邪魔すんなよー
	CCriticalLock lock(GetCS());

	//	NullSoundBufferをつっこんどく
	SetBuffer(smart_ptr<ISoundBuffer>(new CNullSoundBuffer));
	m_strFileName.erase();

	return 0;
}

LRESULT CWaveStreamSound::Restore(){
	//	今から弄るんだから邪魔すんなよー
	DWORD status;
	{	CCriticalLock lock(GetCS());
		status = GetBuffer()->GetStatus();
	}

	if (m_strFileName!="" && status==1) {
		//	うわーロストしてるよ(;´Д`)
		bool bPlay = IsPlay();
		Stop();
		string temp = m_strFileName; //	コピーして渡さんとReleaseされちゃう:p
		Load(temp);
		return (bPlay) ? Play() : 0;
	}

	return 0;
}

LRESULT CWaveStreamSound::Play()
{
	//	再生中ならとめんといかんよ！
	if ( IsPlay() ) Stop();

	m_bPaused = 0;

	//	バッファを初期化する
	if ( InitBuffer()!=0 ) {
		Stop();
		return 1;
	}

	//	バッファを再生状態にする
	if ( GetBuffer()->Play(true)!=0 ) {
		Err.Out("CWaveStreamSound::InitBuffer GetBuffer()->Play()に失敗！");
		Stop();
		return 2;
	}

	//	あとはワーカスレッドに任せる
	if ( CreateThread()!=0 ) {
		Err.Out("CWaveStreamSound::InitBuffer CreateThread()に失敗！");
		Stop();
		return 3;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CWaveStreamSound::Stop()
{
	//	今から弄るんだから邪魔すんなよー
	{
		CCriticalLock lock(GetCS());
		if ( m_bPaused!=0 ) return 0; //	ポーズ中なら既にStopされている
	}

	//	スレッド止める->安全のため止まるまで待つStopThread関数を使う
	StopThread();

	return InnerStop();
}

LRESULT CWaveStreamSound::InnerStop()
{
	//	スレッド内から呼ばれる事もあるのでLockが必要
	CCriticalLock lock(GetCS());

	//	バッファ止めて初期化する
	LRESULT ret = GetBuffer()->Stop();
	GetBuffer()->SetCurrentPosition(0);
	if (!GetStream().isNull()){
		//	これはファイル読み込みに失敗していればnullでありうる
		GetStream()->SetCurrentPos(0);
	}
	m_dwPlayPosition = 0;
	m_bPaused = 0;

	return ret;
}

LONG CWaveStreamSound::GetCurrentPos() const
{
	//	今から弄るんだから邪魔すんなよー
	CCriticalLock lock(GetCS());

	if (m_nAvgBytesPerSec==0) return -1;	//	Loadの情報取得で失敗している

	LONG pos = LONG(((DWORDLONG) m_dwPlayPosition * 1000 / m_nAvgBytesPerSec) );
	//	DWORDLONGにキャストしないと溢れちゃう:p
	return pos;
}

LRESULT CWaveStreamSound::SetCurrentPos(LONG lPos){
	if (m_nAvgBytesPerSec==0) return 1;	//	Loadの情報取得で失敗している

	//	きっちり位置を変えるには止めるしかない
	bool bPlay = IsPlay();
	Stop();

	//	Stop関数により、スレッドは止まっている
	//	よってCriticalSectionは必要ない
	GetStream()->SetCurrentPos(lPos);
	m_dwPlayPosition = LONG(((DWORDLONG) lPos * m_nAvgBytesPerSec / 1000 ) );

	return (bPlay) ? Play() : 0;
}

bool CWaveStreamSound::IsPlay() const
//	そのチャンネルが再生中(=3)ならtrue
{
	//	今から弄るんだから邪魔すんなよー
	CCriticalLock lock(GetCS());

	DWORD ret = GetBuffer()->GetStatus();
	return ret==3;
}

LRESULT CWaveStreamSound::Pause()
//	特定チャンネルのpause
{
	//	今から弄るんだから邪魔すんなよー
	CCriticalLock lock(GetCS());

	int& bPaused = m_bPaused;
	bPaused += (bPaused>0?1:(bPaused<0?-1:0));	//	必殺技:p
	if ( !IsPlay() ) {
		return 0; //	もう終わってる
	}
	//	再生中のをpauseしたんならば
	bPaused = 1;

	LRESULT ret = GetBuffer()->Stop();
	return ret;
}

LRESULT CWaveStreamSound::Replay()
//	pauseで止めていた、そのチャンネルの再生を再開。
{
	//	今から弄るんだから邪魔すんなよー
	CCriticalLock lock(GetCS());

	if (!m_bPaused) return 0; //	pauseしてへんて！

	if (--m_bPaused==0) {	//	参照カウント方式
		LRESULT ret = GetBuffer()->Play(true);
		return ret;
	}
	return 0;
}

LRESULT CWaveStreamSound::SetVolume(LONG volume)
{
	//	今から弄るんだから邪魔すんなよー
	CCriticalLock lock(GetCS());

	//	範囲外のエラーチェックせーへんから、勝手にやってやー
	m_lVolume = volume;
	LRESULT ret = GetBuffer()->SetVolume(m_lVolume);
	return ret;

	//	DSBVOLUME_MAX（減衰なし） :		0
	//	DSBVOLUME_MIN（無音）	 : -10000
	//	の間で指定するのよん。
}

LONG CWaveStreamSound::GetVolume() const
//	取得は、特定チャンネルに対してしかできない...
{
	//	今から弄るんだから邪魔すんなよー
	CCriticalLock lock(GetCS());

	return m_lVolume;
}

//	bmdさんの提案
DWORD CWaveStreamSound::GetFrequency() const
{
	//	今から弄るんだから邪魔すんなよー
	CCriticalLock lock(GetCS());

	DWORD dwFreq;
	LRESULT ret = GetBuffer()->GetFrequency(dwFreq);

	if ( ret!=0 ) return 0xffffffff;
	return (LONG)dwFreq;
}

LRESULT CWaveStreamSound::SetFrequency(DWORD freq)
{
	//	今から弄るんだから邪魔すんなよー
	CCriticalLock lock(GetCS());

	if((DSBFREQUENCY_MIN<=freq && freq<=DSBFREQUENCY_MAX) || freq==DSBFREQUENCY_ORIGINAL){
		LRESULT ret = GetBuffer()->SetFrequency(freq);
		if(ret==0) return 0;
	}
	return 1;
}

LRESULT	CWaveStreamSound::SetLoopPlay(bool bLoop)
{
	//	今から弄るんだから邪魔すんなよー
	CCriticalLock lock(GetCS());

	m_bLoop = bLoop;
	return 0;
}
bool CWaveStreamSound::IsLoopPlay() const
{
	//	今から弄るんだから邪魔すんなよー
	CCriticalLock lock(GetCS());

	bool ret = m_bLoop;
	return ret;
}

LONG CWaveStreamSound::GetLength() const
{
	//	今から弄るんだから邪魔すんなよー
	CCriticalLock lock(GetCS());

	LONG len =	GetStream()->GetLength();
	return len;
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
