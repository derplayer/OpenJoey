#ifndef __STDAFX_H__
#define __STDAFX_H__

//	�ŏ��ɐݒ��ǂݍ���ǂ����O�O�G
#include "config/yaneConfig.h"


#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
//	windows.h��min/max�́Astd::min , std::max�ƏՓ˂���
//	http://www.devx.com/free/tips/tipview.asp?content_id=3334
#include <windows.h>


// DirectSound���g�����B�����̎g���w�b�_�[���炢������include���[��...
#pragma warning(disable:4201)	//	mmsystem.h�̂Ȃ��Ŏg�p���Ă����W���̊g���@�\�΍�
#include <mmsystem.h>
#pragma warning(default:4201)
#pragma comment(lib,"winmm.lib")

//	mpeg�p�̍\���̂�����̂Ńv���b�g�t�H�[��SDK����ŐV�̂ƍ����ւ��ĂˁI
#include <mmreg.h>


// �m�s�œ��삳����̂ŁA�o�[�W������DirectX3���I�I
#define DIRECTINPUT_VERSION 0x0300
#define DIRECTDRAW_VERSION	0x0300
#define DIRECTSOUND_VERSION 0x0300
//	��DirectX7����DSBUFFERDESC�\���̂̃T�C�Y���傫���Ȃ��Ă���̂ł��̑΍�
#include <dinput.h>			// DirectInput header
#include <ddraw.h>			// DirectDraw header
#include <dsound.h>			// DirectSound header
#include <dmusicc.h>		// DirectMusic header(directX6�ȍ~��SDK���K�v�ɂ�)
#include <dmusici.h>

//	DirectX�֌W��GUID�̐ݒ�̂���
#if	defined(yaneSDK_MSVC_Mode)	//	BCC�ł�import32.lib�Ɋ܂܂�Ă��邽�߁A�����N���Ȃ��Ă�����
	#pragma comment(lib,"dxguid.lib")
#endif


//namespace yaneuraoGameSDK3rd {}
//using namespace yaneuraoGameSDK3rd;	//	yaneSDK3rd�g����ł��H�H
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

//	plug-in dll���쐬����̂Ȃ��...
#ifdef COMPILE_YANE_PLUGIN_DLL
	//	YTL::string���g���悤�ɋ������Ȃ��Ƃ�������
	#undef USE_yaneString
	#define USE_yaneString
#endif

#if defined(USE_STL_OnVisualC) || defined(USE_STLPort)
	//	��STLPort�̏ꍇ��include�p�X�ɐݒ肵�Ă����Ȃ��Ƃ�����炵��..
	#include <stdio.h>
	// STL�֘A
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
	//	using������z�͎��Y�H�H
	//	���A�A���߂�(;�L�D`)

	//	algorithm�̊֐��ǂ�����˂�..
	//	���w�肹��Ƃ�����̂��H�H�ň���..
	using std::find;

#ifndef USE_yaneString
	#include <string>
	using std::string;
#endif

#endif // defined(USE_STL_OnVisualC) || defined(USE_STLPort)


#ifdef USE_yaneString
	//	YTL/yaneString.h��ǂݍ���Ŏg��
	#include "YTL/string.h"				//	std::string�݊��N���X
	using namespace yaneuraoGameSDK3rd::YTL::YTLstring;
	//	#define string YTL::string
	//	�����̒�`���ƁA�ċN�I�Ɍ����邹�����AVS.NET��IntelliSense�ł̓t���[�Y����
#endif


#include <stdlib.h>		// _MAX_PATH,qsort,rand..

// -------- ��������֗����ȁ`�O�O ----------------------------------
//	YTL�́A��������֗��H

#include "YTL/index.h"				//	Yanurao Template Library
#include "Thread/yaneThreadLocal.h"	//	ThreadLocal template

//	Err�o�͂ɕK�v
#include "Auxiliary/yaneStream.h"

// -------- �K�v�Ȃ��̂�typedef���Ƃ����[ ---------------------------

//	�萔��`�|�C���^�Ȃ�(MFC�Œ�`����Ă��邪BCB�ւ̈ڐA���l�����Ē�`)
typedef const RECT* LPCRECT;
typedef const POINT* LPCPOINT;
typedef const SIZE* LPCSIZE;

#endif
