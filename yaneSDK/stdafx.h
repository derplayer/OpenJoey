#ifndef __STDAFX_H__
#define __STDAFX_H__

//	Let's load the settings first.
#include "config/yaneConfig.h"


#ifndef yaneSDK_GCC_Mode_NOWIN

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
//	min/max in windows.h conflicts with std::min, std::max
//	http://www.devx.com/free/tips/tipview.asp?content_id=3334
#include <windows.h>

// DirectSound is used. Just include the headers you use yourself...
#pragma warning(disable:4201)	//	Measures against non-standard extensions used in mmsystem.h
#include <mmsystem.h>
#pragma warning(default:4201)
#pragma comment(lib,"winmm.lib")

//	There is a structure for mpeg, so use lets use it from the platform SDK!
#include <mmreg.h>


// Since it runs on NT, the version is DirectX3!
#define DIRECTINPUT_VERSION 0x0300
#define DIRECTDRAW_VERSION	0x0300
#define DIRECTSOUND_VERSION 0x0300
//	Å™Countermeasure for the increased size of the DSBUFFERDESC structure since DirectX7.
#include <dinput.h>			// DirectInput header
#include <ddraw.h>			// DirectDraw header
#include <dsound.h>			// DirectSound header
#include <dmusicc.h>		// DirectMusic header(Requires DirectX6 or newer SDK)
#include <dmusici.h>

//	For DirectX-related GUID settings
#if	defined(yaneSDK_MSVC_Mode)	//	BCC does not need to link to it because it is included in import32.lib
	#pragma comment(lib,"dxguid.lib")
#endif

#else
// GCC Mode
// TODO heh
#endif // yaneSDK_GCC_Mode_NOWIN

//namespace yaneuraoGameSDK3rd {}
//using namespace yaneuraoGameSDK3rd;	//	You're using yaneSDK3rd, right??
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

//	If you are creating a plug-in dll...
#ifdef COMPILE_YANE_PLUGIN_DLL
	//	Force them to use YTL::string or they will conflict.
	#undef USE_yaneString
	#define USE_yaneString
#endif

#if defined(USE_STL_OnVisualC) || defined(USE_STLPort)
	//	Å´In the case of STLPort, it seems that you have to set it in the include path..
	#include <stdio.h>
	// STL related
	#include <set>
	#include <list>
	#include <vector>
	#include <stack>
	#include <map>
	#include <algorithm>
    #include <memory>

	using std::set;
	//	I'm sorry (;ÅLÑD`)
	using std::list;
	using std::vector;
	using std::stack;
	using std::map;
	using std::pair;

	//	What about algorithm functions?
	//	Do I have to specify each one individually? That sucks...
	using std::find;

#ifndef USE_yaneString
    //#include <cstring> // for memcpy()
    #include <string> // for string class

    using std::string;
#endif

#endif // defined(USE_STL_OnVisualC) || defined(USE_STLPort)


#ifdef USE_yaneString
	//	YTL/yaneString.h load and use
	#include "YTL/string.h"				//	std::string compatible class
	using namespace yaneuraoGameSDK3rd::YTL::YTLstring;
	//	#define string YTL::string
	//	Å™This definition, freezes in VS.NET IntelliSense, perhaps because it looks recursive.
#endif


#include <stdlib.h>		// _MAX_PATH,qsort,rand..

// -------- Ç†Ç¡ÇΩÇÁï÷óòÇ©Ç»Å`ÅOÅO ----------------------------------
//	Would YTL be useful if it were available?

#include "YTL/index.h"				//	Yanurao Template Library
#include "Thread/yaneThreadLocal.h"	//	ThreadLocal template

//	Required for Error output
#include "Auxiliary/yaneStream.h"

// -------- Let's typedef what we need ---------------------------

//	Constant rectangle pointer, etc. (defined in MFC but with consideration of porting to BCB)
typedef const RECT* LPCRECT;
typedef const POINT* LPCPOINT;
typedef const SIZE* LPCSIZE;

#endif
