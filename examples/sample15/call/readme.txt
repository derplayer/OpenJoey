　[boost_regex.dll]を呼び出す側のプログラムのサンプルです。

　[IRegex.h]と[CFSjisConverter.h]をお好きなプロジェクトのフォルダまで
持って行って使ってください。

　[CFEucConverter.h]は、EUC版のおまけです。
　[CFJisConverter.h]は、JIS版のおまけです。


注意！このDLLを使う側をコンパイルするときは、
yaneSDK3rdのyaneConfig.hにて
	#define USE_yaneString
を必ず有効にすること。

同じフォルダにあるcall.exeは、呼び出す側のサンプルをbuildしたもの。
boost_regex/Release/boost_regex.dll
を、call.exeと同じフォルダに移動させて実行すると良い。

