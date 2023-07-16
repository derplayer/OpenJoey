//
//	yaneObjectCreater.h :
//		ObjcetManager(CoCreateInstance的なもの)
//

#ifndef __ObjectCreater_h__
#define __ObjectCreater_h__

//////////////////////////////////////////////////////////////////////////////

#ifdef COMPILE_YANE_PLUGIN_DLL
//	この関数を、DLL PlugInとして、コールバックしてもらえる
extern "C"	__declspec( dllexport ) void _cdecl YaneDllInitializer(void*);
#endif

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

/**
	MAIN側とDLL側のstringの実装と異なるとが異なるとまずい。
	yaneSDKの場合string.hは自前で持っているので問題ないはずなのだが、
	これを使いたくはないということもあるだろう。
	そこで、CObjectCreater::Create,RegistClassの引数に
	stringを使わないメンバも用意して、stringを使うバージョンはすべて
	そちらに委譲することにした。これで(CObjectCreaterに関しては)
	stringのバージョンの違いに悩まされることがなくなる。
	（exportしているクラスのメンバの引数等でstringを使わないならば）
*/

class CDllPlugInLoader {
/**
	class CObjectCreater で使用される
	PlugIn DLLを読み込む。Plug In Dllについては、
	Plug In Dll作成ガイドについてご覧ください。
*/
public:
	virtual LRESULT Load(const string& strDllName);
	///	Plug In DLL を読み込む

	virtual LRESULT Release();
	///	Plug In DLL を解放する

	virtual string	GetFileName() const { return m_strFileName; }
	///	読み込んでいるPlug In DLLの名前を返す（フルパスで）

	virtual HINSTANCE GetInstance() const { return m_hDll; }

	CDllPlugInLoader();
	virtual ~CDllPlugInLoader();

private:
	HINSTANCE m_hDll;
	string m_strFileName;
};

//////////////////////////////////////////////////////////////////////////////

class IObjectCreater {
/**
	《　Plug In DLL 作成方法　》
	yaneSDK3rdの各クラスを用いたDLLを作成し、それをMain側から
	呼び出すためには

	１．config/yaneConfig.hで
	//#define COMPILE_YANE_PLUGIN_DLL	//	plug-in DLLを作成するのか？
	を有効にする

	２．main側に公開したいオブジェクト（のfactory）を
		以下のようなYaneRegistPlugIn関数を作って登録する

		void	YaneRegistPlugIn(IObjectCreater*p){
			p->RegistClass("CHoge",new factory<CHoge>);
			p->RegistClass("CHoee",new factory<CHoee>);
						//	↑このクラス名は、あくまで例
		}

	３．オブジェクトを名前で生成するにはIObjectCreaterを使う
		（使いかたは、yaneSDK3rd/AppFrame/yaneObjectCreaterと同じ）

		例：
			//	使う前にLoad
			IObjectCreater::GetObj()->LoadPlugIn("enraSound.dll");
			IVorbis* p = (IVorbis*)CObjectCreater::GetObj()->Create("CVorbis");
			if (p!=NULL){
				p->Play();
				delete p;
			}
			//	使い終わったら、解放
			IObjectCreater::GetObj()->ReleasePlugIn("enraSound.dll");
*/
public:
	virtual void* Create(LPCSTR strClassName)=0;
	virtual void* Create(const string& strClassName)=0;

	///	オブジェクトを登録するのに使う(main,dll共通)
	virtual void RegistClass(LPCSTR strClassName,factory_base*)=0;
	virtual void RegistClass(const string& strClassName,factory_base*p)=0;

	virtual LRESULT LoadPlugIn(LPCSTR strFileName)=0;
	virtual LRESULT LoadPlugIn(const string& strFileName)=0;

	virtual LRESULT ReleasePlugIn(LPCSTR strFileName)=0;
	virtual LRESULT ReleasePlugIn(const string& strFileName)=0;

	//	main側のnew,deleteをDLL側から呼び出す
	virtual void* New (size_t t)=0;
	virtual void  Delete(void* p)=0;
	virtual ~IObjectCreater(){}

//	DLLを作成するときのみ
#ifdef COMPILE_YANE_PLUGIN_DLL
	static IObjectCreater* GetObj() { return m_p; }
	static void SetObj(IObjectCreater*p) { m_p = p; }
protected:
	static IObjectCreater* m_p;

#endif
};

class CObjectCreater : public IObjectCreater {
/**
	DLL側のオブジェクトの生成を行なうような管理クラス
	DLL側で、他のDLLにある、オブジェクトを生成したりも出来る

	使いかた：
			//	使う前にLoad
			CObjectCreater::GetObj()->LoadPlugIn("enraSound.dll");
			IVorbis* p = (IVorbis*)CObjectCreater::GetObj()->Create("CVorbis");
			if (p!=NULL){
				p->Play();
				delete p;
			}
			//	使い終わったら、解放
			CObjectCreater::GetObj()->ReleasePlugIn("enraSound.dll");

		main側であろうがdll側であろうが、上の方法で登録されている
		のであれば、どこにあるオブジェクトであっても生成できる！
		すなわち、dll側から、他のdllで登録されているオブジェクトでも
		呼び出すことが出来る！

*/
public:
	static CObjectCreater* GetObj() { return m_vObj.get(); }

	virtual void* Create(LPCSTR strClassName);
	virtual void* Create(const string& strClassName)
		{ return Create(strClassName.c_str()); }
	/**
		このCObjectCreaterに登録されているオブジェクトを
		その名前で生成する

		この関数で生成されたオブジェクトはdeleteでdeleteして良い。
		（ここで読み込まれているDllは、class CDllPlugInLoader
		に書かれている要件を満たすから）

		もし登録されていなければNULLが返る

		※　このクラスを使う場合、stringはYTL::stringを用いて、
			stringの実装は不変であるものとする。
	*/

	///	PlugInDllを読み込む（参照カウントを持っている）
	virtual LRESULT LoadPlugIn(LPCSTR strFileName);
	virtual LRESULT LoadPlugIn(const string& strFileName)
		{ return LoadPlugIn(strFileName.c_str()); }
	/**
		この関数で、Plug In Dllを読み込むと、自動的に
		クラス（のfactory）が、登録される。
	*/

	///	PlugInDllを解放する（参照カウントを持っている）
	virtual LRESULT ReleasePlugIn(LPCSTR strFileName);
	virtual LRESULT ReleasePlugIn(const string& strFileName)
		{ return ReleasePlugIn(strFileName.c_str()); }
	/**
		この関数で、Plug In Dllを解放すると、
		クラス（のfactory）の登録が解除される
	*/

	virtual void RegistClass(LPCSTR strClassName,factory_base*);
	virtual void RegistClass(const string& strClassName,factory_base*p)
		{ RegistClass(strClassName.c_str(),p); }
	/**
		DLL側が、オブジェクトを登録するのに使う。
		main側では、使う必要は無い。（使っても良い）
	*/

	///	Main側のnewとdelete(DLL側からは、これを呼び出してもらう)
	virtual void* New (size_t t);
	virtual void  Delete(void* p);

	CObjectCreater();
protected:
	class CPlugInInfo {
	///	読み込んでいるPlug Inを管理するための構造体
	public:
		CDllPlugInLoader	vPlugIn;	//	プラグイン
		int					nRef;		//	参照カウント
		CPlugInInfo(){ nRef = 1; }
	};
	smart_vector_ptr<CPlugInInfo> m_apPlugIn;

	class CObjectInfo {
	///	登録されているPlug-Inとオブジェクトを管理するための構造体
	public:
		CPlugInInfo*	pPlugIn;		//	プラグイン
		string			strClassName;	//	クラス名
		factory_base*	factory;		//	factory
		CObjectInfo():pPlugIn(0),factory(0) {}
		~CObjectInfo() { DELETE_SAFE(factory); }
	};
	smart_vector_ptr<CObjectInfo> m_apObjectInfo;

	smart_vector_ptr<CPlugInInfo>* GetPlugInInfo()
		{ return& m_apPlugIn; }
	smart_vector_ptr<CObjectInfo>* GetObjectInfo()
		{ return& m_apObjectInfo; }

	//	↑のポインタをいじるためのクリティカルセクション
	CCriticalSection m_cr;
	CCriticalSection* GetCriticalSection() { return& m_cr; }

	smart_vector_ptr<CPlugInInfo>::iterator
		FindPlugIn(const string& strPlugInName);
	//	同名のプラグインが読み込まれているならば、そいつへのポインタを返す

	//	DLL側を初期化するときに一時的に渡す
	CPlugInInfo* m_pPlugInInfo;

	static	singleton<CObjectCreater>	m_vObj;
};

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

//////////////////////////////////////////////////////////////////////////////

#endif
