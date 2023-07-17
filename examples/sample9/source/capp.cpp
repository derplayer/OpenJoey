#include "stdafx.h"

//	１．yaneConfig.hの
//	#define COMPILE_YANE_PLUGIN_DLL	//	plug-in DLLを作成するのか？
//	を有効にする

//	２. yaneConfigの#define NOT_USE_DEFAULT_DLLMAIN
//	を無効にする(ディフォルトで無効)　このシンボルを定義すればユーザー独自の
//	DllMainを書くことが出来る。そのときはyaneObjectCreater.hにあるDllMainを
//	参考に書いてください。

//	３．DLLからexportするクラスのインターフェースを用意する
class IPlugInTest {
public:
	virtual int Calc(int n)=0;
	virtual ~IPlugInTest(){}
};

//	４．そのインターフェースに対して実装を行なう
class CPlugInTest : public IPlugInTest {
	virtual int Calc(int n){
		//	1からnまでの合計を計算する
		int nTotal = 0;
		for(int i=1;i<=n;i++){
			nTotal+=i;
		}
		return nTotal;
	}
};

//	５．DLL側からexportするクラスのfactoryを登録する
//	※　staticな変数等でnewが必要なものは、ここでnewする。
void	YaneRegistPlugIn(IObjectCreater*p){
	p->RegistClass("CPlugInTest",new factory<CPlugInTest>);	//	factoryを登録しまする
	//	↑ここで登録しているクラスにstaticなオブジェクトがあって、その初期化をしたいならば、
	//	ここでnewするか、あるいは、このfactoryを派生させて、
	//	コンストラクタで、そのstaticなオブジェクトポインタがNULLならば
	//	newして、デストラクタで、そのオブジェクトポインタが非NULLならdeleteする
	//	ようにすれば良い
}
