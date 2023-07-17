　boost::regex++を、yaneSDK3rdを使ってDLLごしに呼び出せるようにしたものです。
　ビルド（コンパイル）にはboostをインストールしてある必要があります。

　また、boost::regex++は、staticなクラスがnewを１回だけ使うので、
これはCObjectCreaterのDLL呼び出しの条件をクリアできません。
　そこで、このDLLをコンパイルする際は、[yaneObjectCreater.cpp]内の
newおよびdeleteを以下のように書き換えるなどの工夫が必要です。
（newはstaticなクラスによって１回のみ使われるので、１個だけ覚えておければOK）


static void * dmy_local_new_pointer=NULL;
//	newとdeleteは、Main側のものを呼ぶ
void* operator new (size_t t){
	IObjectCreater* pp = IObjectCreater::GetObj();
	if (pp!=NULL){
		return pp->New(t);
	} else {
		//WARNING(true,"operator newがIObjectCreaterを初期化する前に呼び出されている");
		return (dmy_local_new_pointer=(void*)malloc(t));	//	とってもﾏｽﾞｰ(ﾟДﾟ)
	}
}

void operator delete(void*p){
	IObjectCreater* pp = IObjectCreater::GetObj();
	if (pp!=NULL){
		if(p==dmy_local_new_pointer){
			::free(p);
		}
		else{
			pp->Delete(p);
		}
	} else {
		//WARNING(true,"operator deleteがIObjectCreaterを終了後に呼び出されている");
		::free(p);					//	とってもﾏｽﾞｰ(ﾟДﾟ)
	}
}


　この[yaneObjectCreater.cpp]の書き換えは、DLL側でのみ有効になるので、
呼び出し側ではこれらの変更を行う必要はありません。
　標準の[yaneObjectCreater.cpp]で問題なく呼び出すことが可能です。
