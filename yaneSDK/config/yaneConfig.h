/*

	yaneSDK configuration

*/
#ifndef __yaneConfig_h__
#define __yaneConfig_h__

//	コンパイラ別の設定
#if	defined(_MSC_VER) && (_MSC_VER>=1200)
//	VC6.0(sp5)/VC7.0/VC7.1でコンパイル確認済み
	#define yaneSDK_MSVC_Mode

	// The debugger can't handle symbols more than 255 characters long.
	// STL often creates symbols longer than that.
	// When symbols are longer than 255 characters, the warning is disabled.
	#pragma warning(disable:4786)
	#pragma warning(disable:4503)

	// --------	 VC++7用にwarningレベル4で出ては困るwarningを抑制 -------
	#pragma warning(disable:4100) // 引数は関数本体部で１度も参照されていないwarning
	#pragma warning(disable:4127) // while(true)とかでwarningが出る
	#pragma warning(disable:4244) // intからUCHAR等の切り詰めに対するwarning
	#pragma warning(disable:4706) // while (lr=func())とかに対するwarning
	#pragma warning(disable:4710) // inline関数がinline展開されない時のwarning
	#pragma warning(disable:4702) // 非到達コードのwarning(STLで大量に出る)
	// -------- VC7で出てくる、うっとおしいwaring対策 --------------------
	//	関数のthrow宣言におけるwaringを不可にする(VC7)
	#pragma warning(disable:4290)

#elif defined(__BORLANDC__) && (__BORLANDC__>=0x551)
//	BCB5(up1)/BCC5.5.1でコンパイル確認済み
	#define yaneSDK_BCC_Mode

	//	TASM積んでないならdefineすべし
	#define yaneSDK_CantUseInlineAssembler

	//	へんなwarningの抑制
	#pragma	warn-8022 //　オーバーライドによる関数隠蔽

#elif defined(__MWERKS__) && (__MWERKS__>=0x3003)
//	CodeWarrior8.3でコンパイル確認済み
	#define yaneSDK_CodeWarrior_Mode

	//	InlineAssemblerが使えるけど文法が違うようなので
	#define yaneSDK_CantUseInlineAssembler

#else 
//	なんやこのけったいなコンパイラヽ(`Д´)ノ
	#error Sorry, yaneSDK3rd cannot be compiled by this compiler.

#endif



// -=-=-=-=-=-=-=-=- Plugin作成機能の排除 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//#define COMPILE_YANE_PLUGIN_DLL	//	plug-in DLLを作成するのか？
	//	（↑DLLを作成するときのみ有効にしてください）
/**
	ただし、yaneSDK3rdのすべての機能をDLL側で使用できることを
	保証するものではありません。たとえば、非ローカルなstaticなオブジェクトは、
	DllMainが呼び出される以前に初期化されるので、
	そのなかでnewしていると、operator newが置換されるまえにnewすることに
	なるので、newに失敗します。Dll側のYaneDllInitializerがMain側から
	呼び出された以降にしかnewすることは出来ません。

	Dll側から使うことが事前にわかっているのならば、Main側から
	CObjectCreater::RegistClassを用いて、Exportしてしまい、DLL側からは
	インターフェースを介してアクセスすれば良いです。

	以下のUSE_DEFAULT_DLLMAINとEXPORT_YANE_PLUGIN_DLLも参照してください。
*/

//#define NOT_USE_DEFAULT_DLLMAIN
//	↑このシンボルを定義してあれば、YANE_PLUGIN_DLLを作成するときに、
//		DllMainをユーザーが書く。(yaneObjectCreater.hにある、DllMainを参考に
//		書いてチョーダイ)

//#define EXPORT_YANE_PLUGIN_DLL
/**
	※　未実装　※

	YanePlugInDll用に、よく使うクラスをexportしておく機能
	これをdefineしてコンパイルすれば、よく使うクラスを
	export(CObjectCreater::RegisterClassで登録)します。

	exportされるのは、
	・
	・
	です。
*/

// -=-=-=-=-=-=-=-=- 使わない機能の排除 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// --- メモリマネージメント -----------------------------------------------

//#define USE_MEMORY_CHECK		//	new,deleteごとにログ記録を残すか？
//#define USE_MEMORY_STATE		//	メモリのデバッグクラス(CMemoryState)を利用するか
								//	（このクラスは、高速なnew/deleteの役目も果す）
								//	↑このクラスは作りかけ。使わないでね！！

#define USE_EXCEPTION			//	例外処理を使う

//	↑もし、上記オプションで例外を使用するとして、
#ifdef _DEBUG
//	#define USE_STOP_EXCEPTION
	//	YTL/exceptions.hで定義されている
	//	例外が発生したときにダイアログを表示して
	//	メモリエラーで停止するための措置(デバッグ時のみ)
#endif

// --- MIDI出力系  --------------------------------------------------------
//

// --- ストリーム再生  ----------------------------------------------------

#define USE_StreamSound		//	CStreamSoundを使用する

// --- 描画	 --------------------------------------------------------------

//	#define USE_Direct3D		//	CDirect3DPlane,CDirect3DDrawを使用するのか

#define USE_FastDraw		//	CFastPlane,CFastDrawを使用するのか
#define USE_DIB32			//	←これはFastPlaneの実装のために仮に利用している
							//	のちのバージョンでは消す予定
#define USE_YGA				//	YGAを使用するのか

// --- JoyStick関連 -------------------------------------------------------

#define USE_JOYSTICK		///	JoyStickを使用するのか

// --- ムービー再生系 -----------------------------------------------------

#define USE_MovieDS			// DirectShowを使う（推奨） 
							// DirectShowに対応したフォーマットで有ればだいたい再生出来る
							// ただしDirectX6.1以上が入っている必要がある
							// DirectShowが入っていない場合、下のUSE_MovieAVIが定義されていれば
							// そちらで再生を試みるようになっている
#define USE_MovieAVI		// AVIStream関数を用いる。ただしAVIファイルしか再生できない。

// --- スクリーンセーバー系 ----------------------------------------------------
//#define USE_SAVER

// --- CErrorLog出力系 ----------------------------------------------------

#define USE_ErrorLog		//	CErrorLogクラスを有効にする。
//	これをdefineしなければ、CErrorLogクラスは空のクラスになり、デバッグ用のエラー
//	文字列等はVC++の最適化によって消滅する。

// --- STL	--------------------------------------------------------------
//	使用するSTLの種類を選択する

#define USE_STL_OnVisualC
//	Visual C++のSTLを使用する

//#define USE_STLPort
/**
	STLPortを用いる場合は、上記のUSE_STL_OnVisualCをコメントアウトすること！

	STLPortの配置場所はどこでも構いません。私はyaneSDK(yaneSDK.h)の
	存在するフォルダのひとつ上のフォルダにstlportというフォルダを作成し、
	そのなかにstlport(4.5.3にて動作を確認)解凍したものを配置しています。

	フォルダ編成は
		|---yaneSDK
		|	|---config
		|	|---AppFrame
		|	|---いろいろ
		|---stlport
			|---doc
			|---etc
			|---src
			|---stlport　←このフォルダをVC++の「includeパス」として指定してやる
			|---test		(一番指定しないといけない。標準ヘッダを読んでからでは遅い)
						VC++.NETならばプロジェクトをソリューションエクスプローラーで右クリックして
						構成プロパティ⇒C/C++⇒全般⇒追加のインクルードディレクトリで指定


	libは、
		コマンドプロンプトを実行して、
			VCのインストールディレクトリのbinフォルダにある
			VCVARS32.BAT
		を実行後、
			C:\STLport-4.5.3\src
		で、
			nmake -f vc7.mak (VC++6ならばvc6.mak)
		とすれば、
			C:\STLport-4.5.3\lib
		に生成されるので、そこをlibパスとして指定してやる
		（構成のプロパティ⇒リンカ⇒全般⇒追加のライブラリディレクトリ）
*/

// --- YTL	--------------------------------------------------------------
//	stringクラスは、やねうらお版を用いる
//#define USE_yaneString
/**
	↑↑
	CObjectCreaterで、DLLファイルとオブジェクトを投げ合う場合は、
	これを定義しておかないと、コンパイラのバージョンが異なるとstringの実装が
	異なるのでまずい。
*/

//	delegateを使う
#define yaneSDK_USE_delegate

#endif
