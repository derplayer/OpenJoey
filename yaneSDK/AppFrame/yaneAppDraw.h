//	Application Draw Frame

#ifndef __yaneAppDraw_h__
#define __yaneAppDraw_h__

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

class IAppDraw {
/**
	アプリケーションにおける描画用基底クラス
	class CAppManager を介して、自分のスレッドが
	使っている描画クラスを取得できる
*/
public:
	virtual int GetType() const = 0;
	/**
		擬似RTTI(以下の値が返る)

		CFastDraw		: 1
		CDirectDraw3D	: 2
	*/


	virtual ~IAppDraw(){}
};

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

#endif
