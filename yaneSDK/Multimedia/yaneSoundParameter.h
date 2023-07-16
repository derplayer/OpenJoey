// yaneSoundParameter.h
//		programmed by ENRA		 '02/03/28

#ifndef __yaneSoundParameter_h__
#define __yaneSoundParameter_h__


namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class ISoundStreamFactory;
class CSoundParameter
/**
	グローバルフォーカス、外部プラグインクラスなどの指定を保持するクラス
	変更は、このクラスを共有している全てのISoundインスタンスに影響する
*/
{
public:
	CSoundParameter();
	virtual ~CSoundParameter(){};

	/**
		サウンドバッファが、グローバルフォーカスを持つのか設定・取得
		これをtrueにしておけば、フォーカスがこのスレッドが生成した
		ウィンドゥから離れてもサウンドは聞こえ続ける
		ただし、サウンドバッファを生成し直さなければ反映されない
		デフォルトではfalse
	*/
	virtual void SetGlobalFocus(bool b) { m_bGlobalFocus = b; }
	virtual	bool IsGlobalFocus() const { return m_bGlobalFocus; }

	/**
		サウンドバッファが、パンコントロール機能を持つのか設定・取得
		デフォルトではfalse
	*/
	virtual void SetPanControl(bool b) { m_bPanControl = b; }
	virtual	bool IsPanControl() const { return m_bPanControl; }

	/**
		サウンドバッファが、周波数コントロール機能を持つのか設定・取得
		デフォルトではfalse
	*/
	virtual void SetFrequencyControl(bool b) { m_bFrequencyControl = b; }
	virtual	bool IsFrequencyControl() const { return m_bFrequencyControl; }

	/**
		ISoundStreamFactory派生クラスの設定・取得
		外部プラグインなどの指定はこのクラスに対して行う
		デフォルトではCSoundStreamFactory
	*/
	virtual void SetStreamFactory(const smart_ptr<ISoundStreamFactory>& p) { m_pStreamFactory = p; }
	virtual smart_ptr<ISoundStreamFactory> GetStreamFactory() const { return m_pStreamFactory; }

protected:
	// サウンドバッファがグローバルフォーカスを持つのか
	bool	m_bGlobalFocus;
	// サウンドバッファがパンコントロール機能を持つのか
	bool	m_bPanControl;
	// サウンドバッファが周波数コントロール機能を持つのか
	bool	m_bFrequencyControl;
	// ISoundStreamのfactory
	smart_ptr<ISoundStreamFactory> m_pStreamFactory;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif // __yaneSoundParameter_h__
