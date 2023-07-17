// dllimport.cpp : DLL アプリケーション用のエントリ ポイントを定義します。
//

#include "stdafx.h"

//	１．DllMainは何もしなくて良い
BOOL APIENTRY DllMain( HANDLE hModule, 
					   DWORD  ul_reason_for_call, 
					   LPVOID lpReserved
					 )
{
	return TRUE;
}

//	２．このファイルをincludeする
#include "yaneObjectCreater.h"

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
void	YaneRegistPlugIn(IObjectCreater*p){
	p->RegistClass("CPlugInTest",new factory<CPlugInTest>);
}
