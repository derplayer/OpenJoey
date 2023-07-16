// yaneWaveOutput.h
//		programmed by ENRA		 '02/03/28

#ifndef __yaneWaveOutput_h__
#define __yaneWaveOutput_h__


#include "../Window/yaneWinHook.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class ISound;
class IWaveSound;
class ISoundBuffer;
class ISoundStreamFactory;
class CDirectSound;
class CSoundParameter;

class IWaveOutput {
/**
	Wave出力を抽象化したクラス
	class IWaveSound 派生クラスの管理
	class ISoundBuffer と class ISoundStreamFactory の提供
	を受け持つ

	[使用方法]
	CWaveOutputDirectSound ds;　←IWaveOutput派生クラス
	CWaveSound sound(&ds);
	sound.Play();
*/
public:
	IWaveOutput();
	virtual ~IWaveOutput(){};

	virtual int	GetType() const = 0;
	/**
		RTTIもどき。派生クラスのタイプを返す
		0 : class CWaveNullOutput
		1 : class CWaveOutputDirectSound
		2 : かんがえちゅう
	*/

	/// class ISoundBuffer 派生クラスの生成
	virtual smart_ptr<ISoundBuffer> CreateBuffer()=0;

	/// class CSoundParameter の設定・取得
	virtual smart_ptr<CSoundParameter> GetSoundParameter() const { return m_pSoundParameter; }
	virtual void SetSoundParameter(const smart_ptr<CSoundParameter>& p) { m_pSoundParameter = p; }

	/// 管理リストの取得
	/// class IWaveSound 派生クラスの全インスタンスに対しての操作はこれで行う
	virtual list_chain<IWaveSound>* GetSoundList() { return &m_listSound; }

	/// 管理リストへの追加・削除
	/// class IWaveSound 派生クラスのコンストラクタ・デストラクタで呼び出されるので
	/// ユーザが明示的に呼び出す必要はない
	virtual LRESULT AddSound(IWaveSound* p);
	virtual LRESULT DelSound(IWaveSound* p);

protected:
	// class IWaveSound 派生クラスのリスト
	list_chain<IWaveSound> m_listSound;
	// class CSoundParameter を保持する
	smart_ptr<CSoundParameter> m_pSoundParameter;
};

class CWaveNullOutput : public IWaveOutput {
/**
	class IWaveOutput のNullDeviceクラス
*/
public:
	CWaveNullOutput(){};
	virtual ~CWaveNullOutput(){};

	/// override from class IWaveOutput
	virtual int	GetType() const { return 0; }
	virtual smart_ptr<ISoundBuffer> CreateBuffer();
};

class CWaveOutputDirectSound : public IWaveOutput, public IWinHook {
/**
	DirectSoundに特化した class IWaveOutput 派生クラス
	バッファのロスト時、管理している全ての class IWaveSound 派生クラスの
	Restore関数を自動的に呼びだす
*/
public:
	/// CDirectSoundへのポインタを取る（p==NULLだと即落ちる）
	/// 省略可能にしないとref_creater_and_mediatorのコンパイルが出来ない
	CWaveOutputDirectSound(CDirectSound* p=NULL);
	virtual ~CWaveOutputDirectSound();

	/// プライマリサウンドバッファのフォーマットを変更する
	/// [in] nFrequency - 11kHz(0) or 22kHz(1) or 33kHz(2) or 44kHz(3)
	/// [in] nBit　　　 - 8bit(0) or 16bit(1)
	/// [in] bStereo　　- trueならステレオ
	virtual LRESULT ChangePrimaryFormat(int nFrequency, int nBit, bool bStereo);

	/// セカンダリサウンドバッファを生成する
	/// 作成できなかったときはCNullSoundBufferを生成する
	virtual smart_ptr<ISoundBuffer> CreateSecondery();

	/// override from class IWaveOutput
	virtual int	GetType() const { return 1; }
	virtual smart_ptr<ISoundBuffer> CreateBuffer() { return CreateSecondery(); }

protected:
	// バッファのロスト監視用
	LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wparam,LPARAM lparam);
	// CDirectSoundへのポインタ
	CDirectSound* m_pDirectSound;
	CDirectSound* GetDirectSound() const { return m_pDirectSound; }
	// プライマリサウンドバッファ
	smart_ptr<ISoundBuffer> m_pPrimary;
	// プライマリサウンドバッファの種類
	int	m_nPrimaryType;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneWaveOutput_h__
