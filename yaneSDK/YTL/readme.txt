
このフォルダにあるのは、YTL(Yaneurao Template Library)

	exception			:	例外の集合

	ref_deleter			:	参照カウントを持った解体子(smart_ptrの実装に必要)
	ref_callback_deleter:	参照カウントを持ったコールバック機能付き解体子

	smart_ptr			:	real smartポインタ。
							配列も非配列も完全に統合して扱える
							Javaのスマートポインタ以上のスマートポインタ
	smart_vector_ptr	:	smartポインタのstd::vectorバージョン
		→　やねうらおＨＰ「天才ゲームプログラマ養成ギプス　第１１章」参照のこと	smart_list_ptr		:	smartポインタのstd::listバージョン

	list_chain			:	std::list のカスタムバージョン

	proxy_ptr			:	必要に迫られたときに始めてnewするようなポインタ
	//	これだとstaticなproxy_ptrをsingleton的に使用できない...

	singleton			:	必要に迫られたときに始めてnewするようなポインタ
	//	proxy_ptrの代わりに使う

	mediator			:	（Java的な）内部クラスを仮想的に実現する
		→　やねうらおＨＰ「天才ゲームプログラマ養成ギプス　第１１章」参照のこと
	factory_permutation	:	クラス置換のためのテンプレート
		→　やねうらおＨＰ「天才ゲームプログラマ養成ギプス　第３章」参照のこと

	function_callback	:	関数，メンバ関数なんでもコールバック用オブジェクト

	ref_creater			:	参照カウント付き生成子

	string				:	std::string互換クラス

	yaneMacro			:	簡単なマクロ集


