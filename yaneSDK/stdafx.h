#ifndef __STDAFX_H__
#define __STDAFX_H__

//	最初に設定を読み込んどこう＾＾；
#include "config/yaneConfig.h"


#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
//	windows.hのmin/maxは、std::min , std::maxと衝突する
//	http://www.devx.com/free/tips/tipview.asp?content_id=3334
#include <windows.h>


// DirectSoundが使いよる。自分の使うヘッダーぐらい自分でincludeせーよ...
#pragma warning(disable:4201)	//	mmsystem.hのなかで使用している非標準の拡張機能対策
#include <mmsystem.h>
#pragma warning(default:4201)
#pragma comment(lib,"winmm.lib")

//	mpeg用の構造体があるのでプラットフォームSDKから最新のと差し替えてね！
#include <mmreg.h>


// ＮＴで動作させるので、バージョンはDirectX3だ！！
#define DIRECTINPUT_VERSION 0x0300
#define DIRECTDRAW_VERSION	0x0300
#define DIRECTSOUND_VERSION 0x0300
//	↑DirectX7からDSBUFFERDESC構造体のサイズが大きくなっているのでその対策
#include <dinput.h>			// DirectInput header
#include <ddraw.h>			// DirectDraw header
#include <dsound.h>			// DirectSound header
#include <dmusicc.h>		// DirectMusic header(directX6以降のSDKが必要にょ)
#include <dmusici.h>

//	DirectX関係のGUIDの設定のため
#if	defined(yaneSDK_MSVC_Mode)	//	BCCではimport32.libに含まれているため、リンクしなくてもいい
	#pragma comment(lib,"dxguid.lib")
#endif


//namespace yaneuraoGameSDK3rd {}
//using namespace yaneuraoGameSDK3rd;	//	yaneSDK3rd使うんでそ？？
namespace yaneuraoGameSDK3rd { namespace YTL {}}
using namespace yaneuraoGameSDK3rd::YTL;
namespace yaneuraoGameSDK3rd { namespace Thread {}}
using namespace yaneuraoGameSDK3rd::Thread;
namespace yaneuraoGameSDK3rd { namespace Draw {}}
using namespace yaneuraoGameSDK3rd::Draw;
namespace yaneuraoGameSDK3rd { namespace AppFrame {}}
using namespace yaneuraoGameSDK3rd::AppFrame;
namespace yaneuraoGameSDK3rd { namespace Math {}}
using namespace yaneuraoGameSDK3rd::Math;
namespace yaneuraoGameSDK3rd { namespace Auxiliary {}}
using namespace yaneuraoGameSDK3rd::Auxiliary;
namespace yaneuraoGameSDK3rd { namespace Timer {}}
using namespace yaneuraoGameSDK3rd::Timer;
namespace yaneuraoGameSDK3rd { namespace Thread {}}
using namespace yaneuraoGameSDK3rd::Thread;
namespace yaneuraoGameSDK3rd { namespace Window {}}
using namespace yaneuraoGameSDK3rd::Window;
namespace yaneuraoGameSDK3rd { namespace Multimedia {}}
using namespace yaneuraoGameSDK3rd::Multimedia;
namespace yaneuraoGameSDK3rd { namespace Input {}}
using namespace yaneuraoGameSDK3rd::Input;
namespace yaneuraoGameSDK3rd { namespace Timer {}}
using namespace yaneuraoGameSDK3rd::Timer;
namespace yaneuraoGameSDK3rd { namespace Dll {}}
using namespace yaneuraoGameSDK3rd::Dll;

//	plug-in dllを作成するのならば...
#ifdef COMPILE_YANE_PLUGIN_DLL
	//	YTL::stringを使うように強制しないとかち合う
	#undef USE_yaneString
	#define USE_yaneString
#endif

#if defined(USE_STL_OnVisualC) || defined(USE_STLPort)
	//	↓STLPortの場合はincludeパスに設定しておかないといかんらしい..
	#include <stdio.h>
	// STL関連
	#include <set>
	#include <list>
	#include <vector>
	#include <stack>
	#include <map>
	#include <algorithm>
	using std::set;
	using std::list;
	using std::vector;
	using std::stack;
	using std::map;
	using std::pair;
	//	usingをする奴は死刑？？
	//	ご、、ごめん(;´Д`)

	//	algorithmの関数どうすんねん..
	//	一個一個指定せんといかんのか？？最悪や..
	using std::find;

#ifndef USE_yaneString
	#include <string>
	using std::string;
#endif

#endif // defined(USE_STL_OnVisualC) || defined(USE_STLPort)


#ifdef USE_yaneString
	//	YTL/yaneString.hを読み込んで使う
	#include "YTL/string.h"				//	std::string互換クラス
	using namespace yaneuraoGameSDK3rd::YTL::YTLstring;
	//	#define string YTL::string
	//	↑この定義だと、再起的に見えるせいか、VS.NETのIntelliSenseではフリーズする
#endif


#include <stdlib.h>		// _MAX_PATH,qsort,rand..

// -------- あったら便利かな～＾＾ ----------------------------------
//	YTLは、あったら便利？

#include "YTL/index.h"				//	Yanurao Template Library
#include "Thread/yaneThreadLocal.h"	//	ThreadLocal template

//	Err出力に必要
#include "Auxiliary/yaneStream.h"

// -------- 必要なものをtypedefしとくかー ---------------------------

//	定数矩形ポインタなど(MFCで定義されているがBCBへの移植を考慮して定義)
typedef const RECT* LPCRECT;
typedef const POINT* LPCPOINT;
typedef const SIZE* LPCSIZE;

#endif
