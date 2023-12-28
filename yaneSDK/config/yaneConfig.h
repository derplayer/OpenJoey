/*

	yaneSDK configuration

*/
#ifndef __yaneConfig_h__
#define __yaneConfig_h__

//	Compiler specific settings
#if	defined(_MSC_VER) && (_MSC_VER>=1200)
//	Compile confirmed with VC6.0(sp5)/VC7.0/VC7.1
	#define yaneSDK_MSVC_Mode

	// Newer Platform SDK want those
	#define WINVER 0x0400
    #define _WIN32_WINNT 0x0400
	#define _WIN32_WINNT_NT4 0x0400

	// The debugger can't handle symbols more than 255 characters long.
	// STL often creates symbols longer than that.
	// When symbols are longer than 255 characters, the warning is disabled.
	#pragma warning(disable:4786)
	#pragma warning(disable:4503)

	// --------	 Suppress warnings that should not appear at warning level 4 for VC++7 -------
	#pragma warning(disable:4100) // The argument is never referenced in the function body warning
	#pragma warning(disable:4127) // while(true) or something like that will cause a warning.
	#pragma warning(disable:4244) // warnings from int to truncation of UCHAR, etc.
	#pragma warning(disable:4706) // warning for while (lr=func())
	#pragma warning(disable:4710) // warning when inline function is not expanded inline
	#pragma warning(disable:4702) // Warnings in unreachable code (lots of them in STL)
	// -------- Annoying waring measures that will appear in VC7 --------------------
	//	Disable waring in function throw declarations (VC7)
	#pragma warning(disable:4290)

	// MSVC 2003 Toolkit warnings
	#pragma warning(disable:4820)
	#pragma warning(disable:4668) // GEEEZ
	#pragma warning(disable:4512)
	#pragma warning(disable:4619)
	#pragma warning(disable:4625)
	#pragma warning(disable:4626)
	#pragma warning(disable:4511)
	#pragma warning(disable:4530)
	#pragma warning(disable:4265)
	#pragma warning(disable:4266)
	#pragma warning(disable:4917)
	#pragma warning(disable:4217)
	#pragma warning(disable:4191)
	#pragma warning(disable:4610)
	#pragma warning(disable:4242)
	#pragma warning(disable:4061)

#elif defined(__BORLANDC__) && (__BORLANDC__>=0x551)
//	Compile confirmed with BCB5(up1)/BCC5.5.1
	#define yaneSDK_BCC_Mode

	//	If you don't have TASM loaded, you should define it
	#define yaneSDK_CantUseInlineAssembler

	//	Suppress strange warnings
	#pragma	warn-8022 //Å@Hiding functions with overrides

#elif defined(__MWERKS__) && (__MWERKS__>=0x3003)
//	Compile confirmed with CodeWarrior8.3
	#define yaneSDK_CodeWarrior_Mode

	//	InlineAssembler can be used, but the grammar seems to be different
	#define yaneSDK_CantUseInlineAssembler

#elif defined(__GNUC__)
    /*
    #if defined(__i386__)
        #define yaneSDK_GCC_Mode // experimental
    #else
        #define yaneSDK_GCC_Mode_NOWIN
    #endif
    */
    #define yaneSDK_GCC_Mode_NOWIN
    #define USE_STLPort // force it

	//	Just to be sure, should work?
	#define yaneSDK_CantUseInlineAssembler

#else
//	What a weird compiler ÅR(`ÑDÅL)Ém
	#error Sorry, yaneSDK3rd cannot be compiled by this compiler.

#endif



// -=-=-=-=-=-=-=-=- Elimination of plugin creation function -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//#define COMPILE_YANE_PLUGIN_DLL	//	Create a plug-in DLL?
	//	(Please enable only when creating Å™ DLL)
/**
	However, all functions of yaneSDK3rd can be used on the DLL side.
		There is no guarantee. For example, a non-local static object is
	is initialized before DllMain is called, so
	If you do new in it, you will do new before operator new is replaced.
	new will fail. YaneDllInitializer on the Dll side from the Main side
	It can only be new after it has been called.

	If you know in advance that you will use it from the Dll side, from the Main side
	Exported using CObjectCreater::RegistClass, and from the DLL side
	You should be able to access it through the interface.

	See also USE_DEFAULT_DLLMAIN and EXPORT_YANE_PLUGIN_DLL below.
*/

//#define NOT_USE_DEFAULT_DLLMAIN
// If you have defined this symbol, when creating YANE_PLUGIN_DLL,
// DllMain written by user. (Refer to DllMain in yaneObjectCreater.h
// Please write it down)

//#define EXPORT_YANE_PLUGIN_DLL
/**
	Unimplemented

	Function to export frequently used classes for YanePlugInDll
	If you define this and compile it, the frequently used class will be
	Export (register with CObjectCreater::RegisterClass).
*/

// -=-=-=-=-=-=-=-=- Eliminate unused features -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// --- memory management -----------------------------------------------

//#define USE_MEMORY_CHECK		//	Do you want to leave a log record for each new and delete?
//#define USE_MEMORY_STATE		//	Use the memory debug class (CMemoryState)
								//	(This class also acts as a fast new/delete)
								//	This class is under construction. don't use it! !

// #define USE_EXCEPTION			//	use exception handling

// If the above option uses exceptions,
#ifdef _DEBUG
// #define USE_STOP_EXCEPTION
	// Defined in YTL/exceptions.h
	// show a dialog when an exception occurs
	// Measures to stop on memory errors (only when debugging)
#endif

// --- MIDI Output system  --------------------------------------------------------
//

// --- stream playback  ----------------------------------------------------

#define USE_StreamSound		// use CStreamSound

// --- Drawing --------------------------------------------------------------

//	#define USE_Direct3D		//	Should I use CDirect3DPlane/CDirect3DDraw?

#define USE_FastDraw		//	Should CFastPlane/CFastDraw be used?
#define USE_DIB32			//	This is temporarily used for the implementation of FastPlane
							//	Will be removed in a later version
#define USE_YGA				//	do you use YGA? (image format)

// --- JoyStick connection -------------------------------------------------------

#define USE_JOYSTICK		//	Do you use JoyStick?

// --- Movie playback system -----------------------------------------------------

#define USE_MovieDS			// Use DirectShow (recommended)
							// If the format is compatible with DirectShow, it can be played in most cases.
							// Requires DirectX 6.1 or higher
							// If DirectShow is not included, the USE_MovieAVI below is defined
							// It is designed to try to play there
#define USE_MovieAVI		// Use the AVIStream function. However, it can only play AVI files.

// --- Screen saver system ----------------------------------------------------
//#define USE_SAVER

// --- CErrorLog output system ----------------------------------------------------

#define USE_ErrorLog		// Enable CErrorLog class.
// If you don't define this, the CErrorLog class will be an empty class and will
// Strings, etc. disappear due to VC++ optimization.

// --- STL	--------------------------------------------------------------
//	Choose the type of STL to use

//#define USE_STL_OnVisualC
//	Using Visual C++'s STL

#define USE_STLPort
/**
	If you use STLPort, comment out USE_STL_OnVisualC above!

	It does not matter where the STLPort is placed. I have yaneSDK (yaneSDK.h)
	Create a folder called stlport in one folder above the existing folder,
	In it, stlport (confirmed operation in 4.5.3) decompressed is placed.

	Folder organisation:
		|---yaneSDK
		|	|---config
		|	|---AppFrame
		|	|---Misc
		|---stlport
			|---doc
			|---etc
			|---src
			|---stlportÅ@Å©Specify this folder as the VC++ Åginclude pathÅh
			|---test	(It must be specified first. It is too late after reading the standard header)
						For VC++.NET, right click on the project in Solution Explorer
						Specify in Configuration Properties ÅÀ C/C++ ÅÀ General ÅÀ Additional Include Directories


	libÇÕÅA
		Run command prompt
			in the bin folder of the VC installation directory
			VCVARS32.BAT
		After running
			C:\STLport-4.5.3\src
		andÅA
			nmake -f vc7.mak (VC++6Ç»ÇÁÇŒvc6.mak)
		and then specify
			C:\STLport-4.5.3\lib
		as the lib path
		(Configuration Properties ÅÀ Linker ÅÀ General ÅÀ Additional Library Directories)
*/

// --- YTL	--------------------------------------------------------------
//	The string class uses Yaneurao version
//#define USE_yaneString
/**
	Å™Å™
	When throwing a DLL file and an object with CObjectCreater,
	If you don't define this, different versions of the compiler will change the implementation of string to
	It's bad because it's different.
*/

//	use delegates
#define yaneSDK_USE_delegate

#endif
