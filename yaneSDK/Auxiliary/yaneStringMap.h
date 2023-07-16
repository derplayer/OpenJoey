#ifndef __yaneStringMap_h__
#define __yaneStringMap_h__


#include "yaneSerialize.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class IStringMap {
public:
	typedef map<string,string> mymap;

	virtual void	Write(const string& strKey,const string& strData)=0;
	virtual void	Write(const string& strKey,LONGLONG nData)=0;
	virtual void	Append(const string& strKey,const string& strData)=0;
	virtual void	Append(const string& strKey,LONGLONG nData)=0;
	virtual LRESULT	Read(const string& strKey,string& strData)=0;
	virtual LRESULT	Read(const string& strKey,LONGLONG& nData)=0;
	virtual string	GetStr(const string& strKey,const string& strDefault="")=0;
	virtual int		GetNum(const string& strKey,int nDefault=0)=0;
	virtual LONGLONG	GetLongNum(const string& strKey,LONGLONG nDefault=0)=0;
	virtual bool	Delete(const string& strKey)=0;
	virtual void	Clear()=0;
	virtual LRESULT ReadFromConfigFile(const string& strFileName)=0;
	virtual LRESULT WriteToConfigFile(const string& strFileName)=0;
	virtual void	Replace(string& s,bool bCaseSensitive=false)=0;
	virtual mymap* GetMap()=0;

	virtual ~IStringMap(){}
};

//	string to stringの連想記憶型集合
class CStringMap : public IStringMap , public IArchive {
/**
キー（文字列）とデータとの連想記憶を提供します。
Perlで使うような、なんでも入る変数が欲しかったので作りました。
CGIを作るときとかに便利です。

内部的には、map<string , string >として実装してあります。

class IArchive 派生クラスなので、そのままシリアライズすることも出来ます。
*/
public:

	///	キーに対するデータの書き込み
	virtual void	Write(const string& strKey,const string& strData);
	virtual void	Write(const string& strKey,LONGLONG nData);
	///		⇒　そのキーが既に存在する場合は、上書き。

	///	キーに対するデータの追加書き込み
	virtual void	Append(const string& strKey,const string& strData);
	virtual void	Append(const string& strKey,LONGLONG nData);
/**
⇒　stringバージョン（前者）のほうは、文字列が加算される。LONGLONGバージョン（後者）の場合は、Writeと同じ動作。
*/

	///	キーに対するデータの読み出し
	virtual LRESULT	Read(const string& strKey,string& strData);
	virtual LRESULT	Read(const string& strKey,LONGLONG& nData);

	///	キーに対するデータの直接取得
	///	(そのキーが存在しないときは、strDefault or lDefaultが返る)
	virtual string	GetStr(const string& strKey,const string& strDefault="");
	virtual int		GetNum(const string& strKey,int nDefault=0);
	/**	↑これのLONGLONGのやつ作ったのだが（以下の）、それは、ときどき
			VC++のreleaseバージョンだと内部コンパイルエラーに
			なるときがある。おそらく、LONGLONGの最適化のバグ。
	*/
	virtual LONGLONG	GetLongNum(const string& strKey,LONGLONG nDefault=0);

	///	キーに対するデータの削除
	virtual bool	Delete(const string& strKey);
	///		削除したときにはtrue

	///	まるごと削除
	virtual void	Clear();	//	clear all data..

	///	定義ファイルからデータを読み出して、格納する機能
	///	(シリアライズとは違い、次のようなフォーマットのファイルから読み出す)
	/**
		// ダブルスラッシュ行はコメント
		#string1 string2 // このように#で開始して、文字列１，２をならべる
		//	このとき、スペースがセパレータ。
		そうすると、string1が、strKey,string2がszDataとして、
		保存(Write)される。
		よって、定義ファイルから読み込み後、GetStr("string1");のようにすれば
		string2が取得できると言うわけ！○(≧∇≦)o
	*/
	virtual LRESULT ReadFromConfigFile(const string& strFileName);

	///	↑の形式のファイルとして書き出すバージョン
	virtual LRESULT WriteToConfigFile(const string& strFileName);


	//	データまるごと表示(デバッグ用)
	void	Out(){
		mymap::iterator it;
		printf ("StringMap::Out() \n");
		for(it = GetMap()->begin();it!=GetMap()->end();it++){
			printf("%s => %s\n",(static_cast<string>(it->first).c_str())
							   ,(static_cast<string>(it->second).c_str()));
		}
	}

	///	文字列をこの連想記憶で置換する
	virtual void	Replace(string& s,bool bCaseSensitive=false);

	///	連想記憶自体を取得する。
	virtual mymap* GetMap() { return& m_vStringMap; }

/*	//	やっぱし、こんなの用意したら、配列アクセスがややこしくなるんでやめ、、
	//	連想キーの直接指定オペレータ
	const string operator [] (string strKey);
	string operator [] (string strKey);
*/

protected:
	///	override from IArchive
	virtual void Serialize(ISerialize&s);

	mymap m_vStringMap;
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
