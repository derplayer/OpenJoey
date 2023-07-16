/**
	ISurfaceを256倍使いやすくする！
*/

#ifndef __yanePlane_h__
#define __yanePlane_h__


namespace yaneuraoGameSDK3rd {
namespace Draw {

class ISurface;
class IPlaneFactory {
public:
	virtual ISurface* CreateInstance() = 0;
};

class CPlane {
/**
	smart_ptr<ISurface> といちいち書くのは面倒極まりないので．．
*/
public:
	///	ポインタのふりをするための仕掛け
	ISurface& operator*() const	 {return *get(); }
	ISurface* operator->() const {return get();	 }

	///	ISurfaceへの暗黙の変換
	operator ISurface* () const { return get(); }

	ISurface* get() const { return p_.get(); }

	//	default factoryの設定/取得
	static void SetFactory(const smart_ptr<IPlaneFactory>& pFactory) { pFactory_ = pFactory; }
	static smart_ptr<IPlaneFactory> GetFactory() { return pFactory_; }

	void	CreateInstance(const smart_ptr<IPlaneFactory>& pFactory);
	///	factoryを渡せば明示的にサーフェースを生成する

	CPlane();
	/**
		こちらは、SetFactoryされているfactoryから自動的にsurfaceを生成する
	*/

	CPlane(const smart_ptr<IPlaneFactory>& pFactory);
	///	surfaceのfactoryを渡せば、それを用いて生成して保持する

	CPlane(const smart_ptr<ISurface>& surface);
	///	copy constructor

	CPlane(ISurface* surface);
	///　↑これは、所有権もCPlaneが保持するので、
	///		CPlane plane(new CTextFastPlane);
	///	のようにして使うこと！

protected:
	smart_ptr<ISurface> p_; // p_;　←って泣いてるの？
	static ThreadLocal<smart_ptr<IPlaneFactory> > pFactory_;	// default factory
};

class CFastDraw;
struct CPlaneFactoryFastPlane : public IPlaneFactory {
/**
	IPlaneFactory派生クラス。こいつはCFastPlaneを生成するためのfactory
*/
	CPlaneFactoryFastPlane(CFastDraw*p) : pDraw_(p) {}
	virtual ISurface* CreateInstance();
protected:
	CFastDraw* pDraw_;
};

class CFastPlaneFactory {
/**
	このクラスは、コンストラクタのなかで、
	IPlaneFactory派生クラスをCPlaneに対して設定する
	これにより、このクラスをCAppに持たせさえすれば、
	そのスレッドは、CPlaneは、すべてCFastPlaneを意味することになる
*/
public:
	CFastDraw*	GetDraw() { return m_vFastDraw.get(); }

	CFastPlaneFactory();

protected:
	smart_ptr<CFastDraw>	m_vFastDraw;	//	FastDraw持ってますねん
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
