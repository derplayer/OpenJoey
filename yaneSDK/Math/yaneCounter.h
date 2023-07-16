//
//		ゲーム用、汎用カウンタ
//

#ifndef __yaneCounter_h__
#define __yaneCounter_h__


#include "../Auxiliary/yaneSerialize.h"

namespace yaneuraoGameSDK3rd {
namespace Math {

class ICounter;

class ICounterFactory {
/**
	ユーザーで独自にカウンタ(ICounter派生クラス)を用意する場合、
	GetTypeで、1000以上のuniqueな数字を返すようにして、
	このFactory派生クラスで、CreateInstanceをオーバーライドして、
	その数字を受け取ったときに、そのクラスをnewして返すようにして、
	そのFactoryを、ICounter::SetFactoryすれば良い。

	そうすれば、CProxyCounterによる、その新しく作ったカウンタの
	シリアライズが可能となる。
*/
public:
	virtual ICounter* CreateInstance(int n)=0;

	virtual ~ICounterFactory(){}
};

class ICounter : public IArchive {
/**
		カウンタ用の基底クラス
*/
public:
	///		intとの親和性
	virtual operator int () const=0;
	virtual const int operator = (int n)=0;

	///		加減算
	virtual void	Inc()=0;
	virtual void	Dec()=0;

	///		初期位置か？
	virtual bool IsBegin() const = 0;
	///		終端に達したのか？
	virtual bool IsEnd() const = 0;

	virtual int GetType() const = 0;
	static	ICounter* CreateInstance(int nType);
	/**
		手動RTTI
			CNullCounter		:	0
			CRootCounter		:	1
			CSaturationCounter	:	2
			CInteriorCounter	:	3
		GetTypeは、動的な型を番号で取得する
		CreateInstanceは、番号から、そのオブジェクトを生成するためのfactory
		上記の番号以外であれば、SetFactoryで設定されたfactoryを用いて生成する
	*/
	static	void	SetFactory(const smart_ptr<ICounterFactory>& v)
		{	m_vFactory = v; }
	static	smart_ptr<ICounterFactory>	GetFactory()
		{ return m_vFactory; }

	virtual ~ICounter(){}

protected:
	static	smart_ptr<ICounterFactory> m_vFactory;
};

class CNullCounter : public ICounter {
/**
		class ICounter のNull Device
*/
public:
	virtual operator int () const { return 0; }
	virtual const int operator = (int n) { return n; }
	virtual void	Inc() {}
	virtual void	Dec() {}
	virtual bool IsBegin() const { return false;}
	virtual bool IsEnd() const { return false; }
	virtual int GetType() const { return 0; }
	virtual void Serialize(ISerialize&) {}
};

/////////////////////////////////////////////////////////////////////////

class CProxyCounter : public ICounter {
/**
	ICounterのための代理母。こいつは、デシリアライズのときに
	ICounterのfactoryを用いて、そのオブジェクトを再現する
*/
public:
	CProxyCounter() : m_vCounter(new CNullCounter) {}
	CProxyCounter(const smart_ptr<ICounter>& p) : m_vCounter(p){}

	///		intとの親和性
	virtual operator int () const { return *m_vCounter.get(); }
	virtual const int operator = (int n) { return ((*m_vCounter.get()) = n);}

	///		加減算
	virtual void	Inc() { m_vCounter->Inc(); }
	virtual void	Dec() { m_vCounter->Dec(); }

	///		初期位置か？
	virtual bool IsBegin() const { return m_vCounter->IsBegin(); }
	///		終端に達したのか？
	virtual bool IsEnd() const { return m_vCounter->IsEnd(); }

protected:
	smart_ptr<ICounter>	m_vCounter;
	ICounter* GetCounter() { return m_vCounter.get(); }

	//	overriden from IArchive
	virtual void Serialize(ISerialize&);
};

/////////////////////////////////////////////////////////////////////////

class CRootCounter : public ICounter {
/**
	初期値(nStart)・終了値(nEnd)、そして必要ならば増加値(nStep)を設定します。
	そのあと、Inc関数を実行すると、1ずつ増えます。(nStepが設定されていれば、
	その数だけ増えます)　そして、加算した結果、nEnd以上になると自動的に
	nStartに戻ります。(nEndにはなりません)

	Reset関数は、カウンタ値を初期値nStartに戻します。
	あるいは、SetInit関数でReset後の初期値が設定されていれば、その値にします。

	また、int型との相互変換が可能ですので、あたかもint型の変数であるかのように
	使えます。

	例）
	CRootCounter r;
	r.Set ( 0, 256 , -5 );
	// nStart == 0 , nEnd == 256というようにカウンタを設定する
	r = 128;

	この状態で、r++; を５回実行すると５回目で r == 129になる。r--;
	を５回実行すると５回目で r == 127になる。

	また、nStart≦nEndでなくて良いのです。
	つまり、加算( Inc / ++ )では、nEnd方向へインクリメントします。
	減算( Dec / -- )では、nStart方向へインクリメントします。

	また、Stepがマイナスである場合は、
	その絶対値回数のIncメンバ関数の呼び出しによって、1だけnEnd方向に進みます。
*/
public:
	///	nStepは一回の増分の絶対値。マイナスは1/nStepの意味
	///	nStart≦nEndでなくて良い
	void	Set(int nStart,int nEnd,int nStep=1)
		{ m_nStart=nStart; m_nEnd=nEnd; m_nStep=nStep; Reset(); }
	void	SetStep(int nStep) { m_nStep = nStep; }
	void	SetStart(int nStart) { m_nStart = nStart; }
	void	SetEnd(int nEnd) { m_nEnd = nEnd; }

	///	取得
	int		GetStep() const { return m_nStep; }
	int		GetStart() const { return m_nStart; }
	int		GetEnd() const { return m_nEnd; }

	///	カウンタのリセット
	void	Reset() { m_nRootCount= m_nStart; m_nRate=0; }

	///	property..
	virtual bool	IsBegin() const { return m_nRootCount == m_nStart; }
	virtual bool	IsEnd() const { return m_nRootCount == m_nEnd; }

	CRootCounter();
	CRootCounter(int nEnd);
	CRootCounter(int nStart,int nEnd,int nStep=1);

	//	intとの相互変換
	operator int () const { return m_nRootCount; }
	const int operator = (int n) { m_nRootCount = n; return n; }
	int		Get () const { return m_nRootCount; }

	//	カウンタのインクリメント(終端まで達すると、そこで停止する)
	void	Inc() { inc(true); }
	void	Dec() { inc(false); }
	//	加算（＝End方向へインクリメント）／減算（＝Start方向へのインクリメント）
	CRootCounter& operator++()
		{ Inc(); return (*this); }
	CRootCounter operator++(int)
		{ CRootCounter _Tmp = *this; Inc(); return (_Tmp); }
	CRootCounter& operator--()
		{ Dec(); return (*this); }
	CRootCounter operator--(int)
		{ CRootCounter _Tmp = *this; Dec(); return (_Tmp); }

	virtual int GetType() const { return 1; }

protected:
	void	inc(bool bAdd=true);

	int		m_nRootCount;
	int		m_nStart;
	int		m_nEnd;
	int		m_nStep;
	int		m_nRate;	//	nStep<0のときは、ｎ回のInc()で+1される

	virtual void Serialize(ISerialize&s) {
		s << m_nRootCount << m_nStart << m_nEnd << m_nStep << m_nRate;
	}
};

class CSaturationCounter : public ICounter {
/**
	飽和カウンタ。class CRootCounter のバリエーション。
	「カウンタを++していき、終了値になったときに、そこでカウンタは
	停止する」という部分だけが異なる。
*/
public:
	///	nStepは一回の増分の絶対値。マイナスは1/nStepの意味
	///	nStart≦nEndでなくて良い
	void	Set(int nStart,int nEnd,int nStep=1)
		{ m_nStart=nStart; m_nEnd=nEnd; m_nStep=nStep; Reset(); }
	void	SetStep(int nStep) { m_nStep = nStep; }
	void	SetStart(int nStart) { m_nStart = nStart; }
	void	SetEnd(int nEnd) { m_nEnd = nEnd; }

	///	取得
	int		GetStep() const { return m_nStep; }
	int		GetStart() const { return m_nStart; }
	int		GetEnd() const { return m_nEnd; }

	///	カウンタのリセット
	void	Reset() { m_nRootCount= m_nStart; m_nRate=0; }

	///	property..
	virtual bool	IsBegin() const { return m_nRootCount == m_nStart; }
	virtual bool	IsEnd() const { return m_nRootCount == m_nEnd; }

	CSaturationCounter();
	CSaturationCounter(int nEnd);
	CSaturationCounter(int nStart,int nEnd,int nStep=1);

	//	intとの相互変換
	operator int () const { return m_nRootCount; }
	const int operator = (int n) { m_nRootCount = n; return n; }
	int		Get () const { return m_nRootCount; }

	//	カウンタのインクリメント(終端まで達すると、そこで停止する)
	void	Inc() { inc(true); }
	void	Dec() { inc(false); }
	//	加算（＝End方向へインクリメント）／減算（＝Start方向へのインクリメント）
	CSaturationCounter& operator++()
		{ Inc(); return (*this); }
	CSaturationCounter operator++(int)
		{ CSaturationCounter _Tmp = *this; Inc(); return (_Tmp); }
	CSaturationCounter& operator--()
		{ Dec(); return (*this); }
	CSaturationCounter operator--(int)
		{ CSaturationCounter _Tmp = *this; Dec(); return (_Tmp); }

	virtual int GetType() const { return 2; }

protected:
	void	inc(bool bAdd=true);

	int		m_nRootCount;
	int		m_nStart;
	int		m_nEnd;
	int		m_nStep;
	int		m_nRate;	//	nStep<0のときは、ｎ回のInc()で+1される

	virtual void Serialize(ISerialize&s) {
		s << m_nRootCount << m_nStart << m_nEnd << m_nStep << m_nRate;
	}
};

class CInteriorCounter : public ICounter {
/**
	内分カウンタを提供します。

	Setメンバ関数で、初期値(nStart)・終了値(nEnd)、
	そして分割数(nFrames)を設定します。

	そうすれば、このカウンタはnStartから始まり、
	nFrame回のIncメンバ関数の呼び出しによってnEndに到達するような
	内分カウンタとなります。

	また、Incは、operator++としても定義してあります。
	Incの逆操作であるDecもあり、それはoperator--としても定義してあります。
*/
public:
	CInteriorCounter();

	///	intとの相互変換
	virtual operator int () const { return m_nNow; }
	virtual const int operator = (int n) { m_nNow = m_nStart = n; m_nFramesNow = 0; return n; }

	virtual void	Inc();		///	加算
	CInteriorCounter& operator++() { Inc(); return (*this); }
	CInteriorCounter operator++(int) { CInteriorCounter _Tmp = *this; Inc(); return (_Tmp); }

	virtual void	Dec();		///	減算
	CInteriorCounter& operator--() { Dec(); return (*this); }
	CInteriorCounter operator--(int) { CInteriorCounter _Tmp = *this; Dec(); return (_Tmp); }

	void	Set(int nStart,int nEnd,int nFrames);
	/**
		初期値(nStart)・終了値(nEnd)、そして分割数(nFrames)を設定します。
	*/

	///	現在の値を（一時的に）変更する。次のInc/Decで、正常な値に戻る
	void	Set(int nNow) { *this = nNow; }

	virtual bool	IsBegin() const { return (m_nNow == m_nStart);}
	virtual bool	IsEnd() const { return (m_nNow == m_nEnd);}

	///	property
	int		GetFrame() const { return m_nFrames; }
	int		GetStart() const { return m_nStart;}
	int		GetEnd() const { return m_nEnd;}
	///		現在のフレーム数(incをした回数)の取得。
	int		GetFrameNow() const { return m_nFramesNow; }

	virtual int GetType() const { return 3; }

protected:
	int		m_nNow;			//	現在の値
	int		m_nStart;		//	初期値
	int		m_nEnd;			//	終了値
	int		m_nFrames;		//	フレーム分割数（終了値になるまで何回Incをすればいいのか）
	int		m_nFramesNow;	//	現在、何フレーム目か？

	virtual void Serialize(ISerialize&s) {
		s << m_nNow << m_nStart << m_nEnd << m_nFrames << m_nFramesNow;
	}
};

/////////////////////////////////////////////////////////////////////////

} // end of namespace Math
} // end of namespace yaneuraoGameSDK3rd

#endif
