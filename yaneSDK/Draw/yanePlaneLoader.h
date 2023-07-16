//
//	yanePlaneLoader.h :
//
//		プレーンの統括的ローディング
//

#ifndef __yanePlaneLoader_h__
#define __yanePlaneLoader_h__

#include "../Auxiliary/yaneLoadCache.h"
#include "../Draw/yaneSurface.h"
#include "../Draw/yanePlane.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CPlaneLoader : public CLoadCache {
/**
	class ISurface 派生クラスのための LoadCache

	ISurfaceのためのfactoryは、
		CPlane::SetFactoryで設定されたものを自動的に使用する
		(このCPlaneLoaderのコンストラクタでCPlane::GetFactoryする)
		CPlane::SetFactoryで設定されたもの以外のを用いたいならば、
		CPlaneLoader::SetFactoryであとから設定すれば良い。

	使用例)
	CPlaneLoader pl;
	pl.SetReadDir("testplane/");	//	読み込みpathの設定
	pl.Set("planedefine.txt",true);

*/
public:

//	virtual smart_ptr<ISurface> GetPlane(int nNo);		///	プレーンの取得
	virtual CPlane GetPlane(int nNo);		///	プレーンの取得
	//	↑こっちのほうが使いやすい気がするのだが。

	///	nNoのファイル名を取得
	string	GetFileName(int nNo );

	/**
		抜き色の設定／取得
		この色をディフォルトの抜き色とする
		（ディフォルト= RGB(0,255,0)すなわち緑)

		注意：通例、サーフェースの左上(0,0)の点を抜き色とするが、
			利便性からCPlaneLoaderでは、色で抜き色を決める。
	*/
	void	SetColorKey(ISurfaceRGB rgb)
		{ m_rgbColorKey = rgb; m_bRGBColorKey = true; }
	ISurfaceRGB GetColorKey() const
		{ return m_rgbColorKey; }

	///	抜き色は、RGBで指定されているか？
	bool	IsRGBColorKey() const { return m_bRGBColorKey; }

	///	抜き色を、位置指定で設定する
	void	SetColorKeyPos(int x,int y)
	{	m_bRGBColorKey=false; m_nColorKeyPosX=x; m_nColorKeyPosY=y; }

	///	サーフェースのfactoryを設定/取得する(defaultではfactoryには
	///	CPlane::GetFactoryしたものが代入されている)
	void SetFactory(const smart_ptr<IPlaneFactory>& pFactory) { m_pPlaneFactory = pFactory; }
	smart_ptr<IPlaneFactory> GetFactory() { return m_pPlaneFactory; }

	CPlaneLoader();
protected:
	virtual	LRESULT InnerLoad(const smart_obj& obj);
	ISurfaceRGB m_rgbColorKey;
	bool	m_bRGBColorKey;		//	抜き色は、RGBで指定なのか？
	int		m_nColorKeyPosX;
	int		m_nColorKeyPosY;
	smart_ptr<IPlaneFactory>  m_pPlaneFactory;
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif