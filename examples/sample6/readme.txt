・yaneSDK3rdから利用できるDLL(yaneSDK3rd系DLLと呼ぶ)作成キット
		version 1.00

（利点）
	あるDLLから他のDLLに存在するクラスをnewすることが出来る。
	MAIN側(EXE)のほうから、DLLに存在するクラスをnewすることが出来る。
	DLL側からMAIN側に存在するクラスをnewすることが出来る。

（使用方法）
	dllimport.cppに説明があります。
	仕組みについては、やね本１を参照のこと。

（使用上の注意）
	newするクラス名は名前(string)で指定。
	この名前、ほかのと重複しないように、
		oggvorbis100::COggReader	だとか
		yaneSDK3rd::Draw::CFastDraw
	というように、スコープ(::)で区切った名前を指定することを推奨します。

	stringクラスがコンパイラのバージョン等に左右されないように、
	yaneSDK3rdで用意しているstringクラスを利用してください。
	⇒　yaneSDK3rd/config/yaneConfig.hで、

		#define USE_yaneString
		//	stringクラスは、やねうらお版を用いる

	この部分を有効にしておきます。（ディフォルトで有効になっているはずです）

	DLLのクラスに対してvectorを渡したい場合は、vectorもやねうらお版を
	用いること。(⇒yaneSDK3rdに含まれています)

	それ以外は作るのめんどくさいのでギブアップ(^^;
	STLPortを用いておけば、おそらく実装は変更されないでしょうから、
	ある程度安全にやりとりできるかと思います。＞それ以外のクラス

（ソースの説明）
	・DLLSource	⇒ DLLを作成するためのキット
	・callFromYaneSDK3rd⇒ yaneSDK3rdを利用している実行ファイルから
			この形式のDLLファイルを呼び出すサンプルソース
	☆　yaneSDK3rdの機能を使わず、yaneSDK3rd用のDLLを呼び出したいだけの
		場合でもこれを使えば良い。
		（どうせ使用していないクラスのコードは最適化で削除されるので)

（メモ）
	string.hはyaneSDK3rd/YTL/string.h	のものそのまま
	yaneObjectCreater.hと.cppは、
		yaneSDK3rd/AppFrame/yaneObjectCreater.hと.cppそのまま

