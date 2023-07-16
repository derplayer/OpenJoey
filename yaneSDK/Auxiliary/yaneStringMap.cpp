#include "stdafx.h"
#include "yaneStringMap.h"
#include "yaneStringScanner.h"
#include "yaneFile.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

void	CStringMap::Serialize(ISerialize&s){
	//	m_vStringMapをまるごとserialize
	int n;
	if (s.IsStoring()){
		n = GetMap()->size();
		s << n; // サイズを突っ込むなりよー＾＾

		mymap::iterator it;
		for(it = GetMap()->begin();it!=GetMap()->end();++it){
			s << *const_cast<string*>(&it->first);
			s << *const_cast<string*>(&it->second);
		}

	} else {
		s << n;		//	長さを復元するなりよ
		GetMap()->clear();	//	いったんクリアするなりよ

		for(int i = 0; i<n ; i++){
			string s1,s2;
			s << s1;
			s << s2;
			GetMap()->insert(pair<string,string>(s1,s2));
		}
	}
}


//	キーに対するデータの書き込み
void	CStringMap::Write(const string& strKey,const string& strData){
	mymap::iterator it;
	it = GetMap()->find(strKey);
	if (it==GetMap()->end()){
		//	そのkeyは存在しない
		//	ほな、挿入すっかー
		GetMap()->insert(pair<string,string>(strKey,strData));
	} else {
		//	上書き上書き上書き～○(≧∇≦)o
		it->second = strData;
	}
}

void	CStringMap::Write(const string& strKey,LONGLONG nData){
	string s;
	CStringScanner::NumToString(nData,s);
	Write(strKey,s);
}

//	キーに対するデータの追加書き込み
void	CStringMap::Append(const string& strKey,const string& strData){
	mymap::iterator it;
	it = GetMap()->find(strKey);
	if (it==GetMap()->end()){
		//	そのkeyは存在しない
		//	ほな、挿入すっかー
		GetMap()->insert(pair<string,string>(strKey,strData));
	} else {
		//	追加追加追加～○(≧∇≦)o　夏はスイカ～○(≧∇≦)o
		it->second += strData;
		//	stringの追加なんで、これでええんでしょ？
	}
}

void	CStringMap::Append(const string& strKey,LONGLONG nData){
	string s;
	CStringScanner::NumToString(nData,s);
	Write(strKey,s);
}

//	キーに対するデータの読み出し
LRESULT	CStringMap::Read(const string& strKey,string& strData){
	mymap::iterator it;
	it = GetMap()->find(strKey);
	if (it==GetMap()->end()){
		//	そのkeyは存在しない
		strData.erase();
		return -1;
	}
	strData = it->second;
	return 0;
}

LRESULT	CStringMap::Read(const string& strKey,LONGLONG& nData){
	string s;
	LRESULT lr = Read(strKey,s);
	if (lr!=0) {
		nData = 0; // fail safe対策
		return lr;
	}
	return CStringScanner::StringToNum(s,nData);
}

//	キーに対するデータの直接取得
	//	(そのキーが存在しないときは、strDefault or lDefaultが返る)
string	CStringMap::GetStr(const string& strKey,const string& strDefault){
	string s;
	Read(strKey,s);
	if (s.empty()) {
		return strDefault; // エラー時はszDefaultを返す
	}
	return s;
}

int CStringMap::GetNum(const string& strKey,int nDefault){
	LONGLONG l;
	if (Read(strKey,l)!=0) return nDefault; // error時はnDefaultを返す
	return (int)l;
}

LONGLONG	CStringMap::GetLongNum(const string& strKey,LONGLONG nDefault){
	LONGLONG l;
	if (Read(strKey,l)!=0) return nDefault; // error時はnDefaultを返す
	return l;
}

//	キーに対するデータの削除
bool	CStringMap::Delete(const string& strKey){
	//	削除したときにはtrue
	return GetMap()->erase(strKey)!=0;
}

//	まるごと削除
void	CStringMap::Clear(){
	//	clear all data..
	GetMap()->clear();
}


//	定義ファイルからデータを読み出して、格納する機能
//	(シリアライズとは違い、特殊なフォーマットのファイルから読み出す)
LRESULT	CStringMap::ReadFromConfigFile(const string& strFileName){
	CFile file;
	LRESULT lr = file.Read(strFileName);
	if (lr!=0) return lr;	//	読み込めて無いじゃん..

	Clear();	//	データのクリア

	string buf;
	string strKey,strData;
	while (true){
		LRESULT lr = file.ReadLine(buf);
		if (lr==1) break;
		if (buf.empty()) continue;
		if (buf.size()>=2 && buf[0] == '/' && buf[1] =='/') continue; // コメント行やん..
		if (buf.size()>=1 && buf[0]=='#') {	//	設定行やん..
			LPCSTR psz = buf.c_str()+1;
			if (CStringScanner::SkipSpace(psz)!=0) continue;
			strKey = CStringScanner::GetStr(psz);
			if (CStringScanner::SkipSpace(psz)!=0) continue;
			strData = CStringScanner::GetStr(psz);
			if (strData.empty()) continue ; // データあらへんで？
			Write(strKey,strData);
		}
	}
	return 0;
}

LRESULT CStringMap::WriteToConfigFile(const string& strFileName){
	CFile file;
	if (file.Open(strFileName.c_str(),"w")!=0) return 1;

	mymap::iterator it;
	for(it = GetMap()->begin();it!=GetMap()->end();++it){
		file.Write(
			string("#") + (it->first) + " " + (it->second) + '\n'
		);
	}
	return 0;
}

//	文字列をこの連想記憶で置換する
void	CStringMap::Replace(string& s,bool bCaseSensitive){
	//	置換しながらの表示
	mymap::iterator it;
	for(it = GetMap()->begin();it!=GetMap()->end();++it){
		CStringScanner::Replace(s,
			static_cast<string>(it->first).c_str(),
			static_cast<string>(it->second).c_str(),
			bCaseSensitive);
	}
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
