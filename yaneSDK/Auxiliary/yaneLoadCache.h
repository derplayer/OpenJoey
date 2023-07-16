// yaneLoadCache.h

#ifndef __yaneLoadCache_h__
#define __yaneLoadCache_h__

#include <LIMITS.h>	//	INT_MAX

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class CIDObjectManager {
/**
	IDとそれに対応する実体を返す
	実体はsmart_obj(どんなオブジェクトでも入るスマートポインタ)で管理
	されているので、非常に便利。
*/
public:
	typedef map<int,smart_obj> CIDObject;

	void	clear();
	///	コンテナをすべて削除する

	LRESULT	erase(int nID);
		///	指定したIDのものを削除する
		///	コンテナが存在し、無事削除できたときは、0が返る

	void	setObject(int nID,const smart_obj& object);
		///	IDに対応するオブジェクトを設定する

	LRESULT getObject(int nID,smart_obj& object);
		///	指定したIDに対応するオブジェクトを取得する

	CIDObject* GetObject() { return& id_object_; }

protected:
	CIDObject	id_object_;
};

///////////////////////////////////////////////////////////////

struct CLoadCacheInfo
/**
	CLoadCacheで用いる構造体
*/
{
	smart_obj	pObj;		//	オブジェクト(CFastPlaneとかCSoundとか..)
	string		strFileName;// 読み込むべきファイル名
};

class ICacheStaleListener
/**
	cacheしているもののなかから
	古くなったデータを捨てるための仕組み
*/
{
public:
	virtual LRESULT OnLoad(const smart_obj& pObj) = 0;
	/**
		pObjを読み込んだときに呼び出される
	*/

	virtual LRESULT OnRelease(const smart_obj& pObj) = 0;
	/**
		pObjを解放したときに呼び出される

		解放作業は、このリスナのなかで行なわなくても良い
	*/

	virtual bool isFull() const = 0;
	/**
		キャッシュ上限になっているなら true
		⇒　このとき古いものから解放するべき
	*/

	virtual ~ICacheStaleListener() {}
};

class CDefaultCacheStaleListener : public ICacheStaleListener
/**
	ディフォルトのStaler
*/
{
public:
	virtual LRESULT OnLoad(const smart_obj& pObj) { m_nRead++; return 0; }
	virtual LRESULT OnRelease(const smart_obj& pObj) { m_nRead--; return 0; }
	virtual bool isFull() const { return m_nRead > m_nMax; }

	void	SetMax(int nMax) { m_nMax = nMax; }
	/**
		この数だけキャッシュする。それをオーバーした場合は
		古いものから解放する
	*/

	CDefaultCacheStaleListener() { m_nMax = INT_MAX; m_nRead = 0; }

protected:
	int m_nRead; // 読み込んでいるデータの数
	int m_nMax;	 // キャッシュすべきデータの上限
};


class ILoadCacheListener {
/**
	class CLoadCache の読み込み時のコールバック。
	必要ならばnNoを変更できる
	読み込むはずのファイルを他のファイルへ置換するのに使う。
*/
public:
	virtual void OnLoad(int &nNo) = 0;
	virtual ~ILoadCacheListener() {}
};

class CDefaultLoadCacheListener : public ILoadCacheListener {
/**
	ILoadCacheListenerの何もしない版
*/
public:
	virtual void OnLoad(int &nNo) {}
};

class ILoadCache {
public:
	virtual LRESULT	Set(const string& filename,bool bUseID=false)=0;

	virtual LRESULT	Load(int nNo)=0;
	virtual LRESULT	Release(int nNo)=0;
	virtual LRESULT	ReleaseAll()=0;

	virtual LRESULT GetObj(int nNo,smart_obj& obj)=0;

	virtual void	SetLoadCacheListener(const smart_ptr<ILoadCacheListener>& v)=0;
	virtual smart_ptr<ILoadCacheListener> GetLoadCacheListener() const =0;

	virtual void SetCacheStaleListener(const smart_ptr<ICacheStaleListener>& p)=0;
	virtual smart_ptr<ICacheStaleListener> GetCacheStaleListener() const=0;

	virtual void	SetCanReloadAgain(bool b)=0;

	virtual void	SetReadDir(const string& path)=0;
	virtual string	GetReadDir() const = 0;

	virtual ~ILoadCache(){}
};

}}

#include "yaneFile.h"
#include "yaneStringScanner.h"

//	templateで書いている部分のために↑のincludeが必要になる(;´Д`)
//	inclusion modelのtemplateは勘弁してくれー

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class CLoadCache : public ILoadCache {
/**
	擬似キャッシュのような働きをします。
	使用方法等については、このクラスの派生も見てください。

	派生クラス：
		class CSoundLoader
		class CPlaneLoader
*/
public:
	virtual LRESULT	Set(const string& filename,bool bUseID=false)
		{ return SetHelper(filename,bUseID,CLoadCacheInfo()); }
	/**
		定義ファイルの設定（読み込み）
		bUseID == trueならば、定義ファイルは、ファイル名の羅列ではなく、
			ファイル名 , ID番号
		の羅列である。このとき指定されたID番号として参照することが出来る
	*/

	template <class T>
	LRESULT SetHelper(const string& filename,bool bUseID,T t){

		//	読み込み済み＆２度読み禁止
		if (!m_bCanReloadAgain) return 0;

		//	ファイルから設定ファイルを読み込むときのために．．．
		CFile file;
		if (file.Read(filename)!=0) return 1;

		int nNums = 0;
		//	↑設定されるファイル数
		while (true){
			CHAR buf[256];
			LRESULT lr = file.ReadLine(buf);
			if (lr==1) break ; // EOF
			if (buf[0]=='\0') continue;		//	空行の読み飛ばし
			if (buf[0]=='/' && buf[1]=='/') continue;	//	コメント行の読み飛ばし
			LPCSTR lp = buf;
			T* pInfo = new T;
			pInfo->strFileName = m_strReadDir + CStringScanner::GetStrFromCsv(lp);

			smart_obj obj(pInfo);

			int nID;
			if (bUseID){
				if (CStringScanner::GetNumFromCsv(lp,nID)!=0){
					return 2;	//	これ、読み込みエラーでそ？
				}
			} else {
				nID = nNums;
			}
			GetIDObjectManager()->setObject(nID,obj);
			OnSet(obj,nID);		//	必要ならば情報を追記	
			nNums ++;
		}
		return 0;
	}
	/**
		Setから呼び出して使うべきhelper関数
		参考⇒　class CSELoader
	*/

	virtual LRESULT	Load(int nNo);
	/**
		そのナンバーのデータの読み込み
		ファイル名にしたがって、オブジェクトは実体化される
	*/

	/**
		SetとLoadは、ペアで使う。
		このとき、リスナICacheStaleListenerのOnLoad,OnReleaseの引数で受け取る
		smart_objの正体はCLoadCacheInfoなので
		smart_ptr<CLoadCacheInfo>* pInfo = obj.get()
		のようにキャストして使う。
	*/

	virtual LRESULT GetObj(int nNo,smart_obj& obj);
	/**
		nNoに対応するオブジェクトを取得する
	*/

	virtual LRESULT	Release(int nNo);
	///	そのナンバーのデータの解放

	virtual LRESULT	ReleaseAll();
	/**
		読み込んでいるデータの全解放

		ただし、このCLoadCacheのインスタンスが読み込んだもののみ。
		キャッシュを共有している他のCLoadCacheが読み込んだものは含まれない。
	*/

	virtual void SetCacheStaleListener(
		const smart_ptr<ICacheStaleListener>& p)
	/**
		キャッシュ上限を設定する仕組み。
		このキャッシュシステムで、キャッシュする上限を与えるリスナ。
		ディフォルトでは、CDefaultCacheStaler。
		これは、読み込んでいる数がCDefaultCacheStaler::SetMaxを超えた場合
		最近使っていないものから解放する仕組み
		defaultではINT_MAX（なのでいつまでも解放されない）
	*/
	{	m_pCacheStaleListener = p;	}

	smart_ptr<ICacheStaleListener>	GetCacheStaleListener() const
		{ return m_pCacheStaleListener; }

	struct CIDObjectAndNumber{
	/**
		CacheManagerは、一番古いオブジェクトを破棄する機能が
		必要なので、CIDObjectManagerのポインタと、ナンバーを持っている
	*/
		CIDObjectManager * pIDObject;
		int			nNum;
		CIDObjectAndNumber():pIDObject(NULL),nNum(0) {}
	};

	typedef fast_list<CIDObjectAndNumber> CCacheManager;
	///	fast_listはYTLで定義されている。listの速いやつだと思いねぇ。

	/**
		CacheManagerに関する、設定～取得関数
		複数のプレーンローダーで、一元的なキャッシュを行ないたいときは、
			CPlaneLoader p1,p2;
			smart_ptr<CLoadCache::CCacheManager> pCache;
			pCache = p2.GetCacheManager();
			p1.SetCacheManager(pCache);
		というように、キャッシュマネージャを“共有”することによって
		ふたつのCPlaneLoaderで、トータル１０枚のグラフィックを
		キャッシュするということも可能になります。（一例です）
	*/
	smart_ptr<CCacheManager> GetCacheManager() const
		{ return m_vCacheManager; }

	void	SetCacheManager(const smart_ptr<CCacheManager>& pCacheManager)
		{ m_vCacheManager = pCacheManager; }

	CIDObjectManager* GetIDObjectManager() { return& m_vIDObject; }

	///	---- おまけ
	///	Setで設定した、ファイル名リストの要素数を返します。
	virtual int		GetSize() const;

	///	property..
	///	リスナの設定～取得
	virtual void	SetLoadCacheListener(const smart_ptr<ILoadCacheListener>& v)
		{ m_pLoadCacheListener = v;}
	virtual smart_ptr<ILoadCacheListener> GetLoadCacheListener() const
		{ return m_pLoadCacheListener; }


	/**
		Setで指定するファイルで書かれているファイルを読み込むとき、
		ここで設定されたDirをくっつけてから処理する。

		例)
			SetReadDir("../Read/");
			//	⇒　"test.bmp"を読み込むとき、
			//		../Read/test.bmpをかわりに読み込む

			注意：
			１．設定するとき、この最後のスラッシュを忘れないこと！
			２．Setを呼び出す前にこの関数で設定すること！
	*/

	virtual void	SetReadDir(const string& strDir)
		{ m_strReadDir = strDir; }

	virtual string	GetReadDir() const 
		{ return m_strReadDir; }

	//	設定ファイルの２度読みの禁止(default:true == ２度読み可能)
	virtual void	SetCanReloadAgain(bool b) { m_bCanReloadAgain = b; }

	CLoadCache();
	virtual ~CLoadCache();

protected:

	/**
		このクラスの使いかた：

		まず、このクラスから派生させ、
		このCCacheManager(GetCacheManagerで取得できる)の
		delegate設定関数で関数をセットします。
	*/

	/////////////////////////////////////////////////////////

	smart_ptr<ILoadCacheListener>	m_pLoadCacheListener;
	smart_ptr<ICacheStaleListener>	m_pCacheStaleListener;

	//	キャッシュシステム
	smart_ptr<CCacheManager>		m_vCacheManager;

	//	実オブジェクト
	CIDObjectManager m_vIDObject;

	/*
		CIDObjectManagerには、すべてのオブジェクトが登録されている
		ただし、そのなかに囲まれているオブジェクトが実体化がされて
		いるとは限らない

		CCacheManagerには、MRU(最近使用したユニット)のID番号のみが
		管理されている。
	*/

	CIDObjectAndNumber GetIDObjectAndNumber(int n)
	{
		CIDObjectAndNumber o;
		o.pIDObject = &m_vIDObject;
		o.nNum = n;
		return o;
	}

	//	設定ファイルの２度読み禁止フラグ
	bool	m_bCanReloadAgain;

	//	ファイルが存在するパス(相対指定)
	string	m_strReadDir;

	virtual LRESULT InnerLoad(const smart_obj& obj) = 0;
	/**
		読み込み作業
		読み込みの結果エラーならば非0を返すようにコーディングする
	*/

	virtual void OnSet(const smart_obj& obj,int nID){}
	/**
		Setでファイルから読み込むときに追加処理を記述したければ、
		この関数をオーバーライドすること
	*/

};

//	順序付けのために必要
inline bool operator < (const CLoadCache::CIDObjectAndNumber& c1, const CLoadCache::CIDObjectAndNumber& c2)
{
	return (c1.pIDObject<c2.pIDObject) || (c1.pIDObject==c2.pIDObject && c1.nNum<c2.nNum);
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

////////////////////////////////////////////////////////////////

#endif
