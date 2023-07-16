///
///		参照カウント付きcreater
///
#ifndef __YTLRefCreater_h__
#define __YTLRefCreater_h__

namespace yaneuraoGameSDK3rd {
namespace YTL {

template <class T>
class ref_creater {
	/**
		参照カウント付きオブジェクト

		class CSoundManager {
		ref_creater<CDirectSound> m_vCreater;
		public:
		ref_creater<CDirectSound>* GetCreater() { return &m_vCreater;}
		//	以下略
		};

		１．
		GetCreater()->inc_ref();
		//	使う前に参照カウント加算

		２．
		CDirectSound* p = GetCreater()->get();
		//	インスタンスの取得

		３．
		GetCreater()->dec_ref();
		//	使い終わったら参照カウント減算
		（このとき自動的にオブジェクトは解放される）

	*/
public:

	bool	inc_ref() {
		if (nRef++==0) {
			p = CreateInstance(); return true;
		}
		return false;
	}
	///	参照カウントの加算
	///	最初に加算したときに、オブジェクトが生成される
	///	オブジェクトが生成されたのならばtrue

	bool	dec_ref() {
		if (--nRef==0) {
			Release();
			return true;
		}
		return false;
	}
	///	参照カウントの減算
	///	０になったときに、オブジェクトが解体される
	///	オブジェクトが解体されたのならばtrue

	T*	get() const { return p; }
	///	オブジェクトTの取得

	void	Release() { if (p) { delete p; p=0; } }
	///	オブジェクトの強制解体

	virtual T*	CreateInstance() { return new T; }
	///	AddRefのときにオブジェクト生成する部分は
	///	オーバーライドできる

	ref_creater() : nRef(0),p(0) {}
	virtual ~ref_creater() { Release(); }

protected:
	T*		p;
	int		nRef;
};

template <class T,class S>
class ref_creater_and_mediator :
	public ref_creater<T> , public mediator<S> {
/**
	class ref_creater のローカライズ版

	T のコンストラクタでS*を渡して、それをmediatorとして
	使いたいクラスのために使う

	例:
	ref_creater_and_mediator<CDirectSound,ISoundManager>	m_vDirectSound;
	//	DirectSoundは必要になったときにのみ生成される
	とやっておけば、
		m_vDirectSound.inc_ref()
	と外部から生成を要求されたときに、
		new CDirectSound(pSoundManager);
	と、このオブジェクトの生成時に渡しておいたS*を
	コンストラクタに指定して生成する。

	inc_refするまでに
		mediator::SetOutClassすれば、
	m_vDirectSound.inc_ref()
	でオブジェクトが生成されるとき、このSetOutClassで設定した
	ポインタがコンストラクタに渡ります。

*/
public:
	ref_creater_and_mediator(S*p=NULL) : mediator<S>(p) {}
protected:
	virtual T* CreateInstance() { return new T(&GetOutClass()); }
};

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
