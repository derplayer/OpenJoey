
//	テキスト入出力用ストリーム
//		(CDebugWindow等も、こいつから派生)
//	バイナリ入出力用ストリーム
//		(CFile等も、こいつから派生)

#ifndef __yaneStream_h__
#define __yaneStream_h__


//////////////////////////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class IOutputStream {
/**
	出力ストリーム
	//	つくりかけ．．
*/
public:
	virtual ~IOutputStream(){}
};

class ITextOutputStream {
/**
	テキスト出力用のストリーム

	制限：Outは、一度に512文字(終了文字列\0を含む)までしか
			出力をサポートしない。
*/
public:
	virtual void	Clear() {}						///	出力先をクリアする
	virtual void __cdecl Out(LPCSTR fmt, ... ) {}	///	文字列を表示する
	virtual void	Out(int);						///	数字を表示する
	virtual void	Out(const string&);				///	文字列を出力する

	virtual ~ITextOutputStream(){}	///	interfaceなので仮想デストラクタが必要
};

class CTextOutputStreamNullDevice : public ITextOutputStream {
/**
	class ITextOutputStream のNull Device(何もしないデバイス)
*/
public:
	virtual void	Clear() {}
	virtual void __cdecl Out(LPCSTR fmt, ... ) {}
	virtual void	Out(int){}
	virtual void	Out(const string&){}
};

//	上のやつのファイル出力タイプ
class CTextOutputStreamFile : public ITextOutputStream {
/**
	class ITextOutputStream の具象クラス
	（ファイルに逐一出力するタイプ）

	Outのごとに、ファイルに出力を行なう
*/
public:
	virtual void	Clear();
	///	ファイルを削除

	virtual void __cdecl Out(LPCSTR fmt, ... );
	///	文字列を表示する

	virtual void	SetFileName(const string& str) { m_strFileName = str; Clear(); }
	///	ファイル名を設定する
	///	(このとき、そのファイル名のファイルが存在するのならばクリアする)

	virtual string	GetFileName() const { return m_strFileName; }
	///	ファイル名を取得する
	///	以降、こいつで設定したファイル名のファイルに対して出力するようになる

protected:
	string m_strFileName;
};

//	上のやつのVisual Studioのデバッグウィンドゥに出力するタイプ
class CTextOutputStreamVSDebug : public ITextOutputStream {
/**
	class ITextOutputStream の具象クラス
	（Visual Studioのデバッグウィンドゥに出力するタイプ）
*/
public:
	virtual void __cdecl Out(LPCSTR fmt, ... ) {
		CHAR	buf[512];
		::wvsprintf(buf, fmt, (va_list)(&fmt+1));
		OutputDebugString(buf);
	}	///	文字列を表示する
};

class CTextOutputStreamVersatile : public ITextOutputStream {
/**
		すべてのデバイスの出力できるデバイス

		エラー出力用デバイスErrは、
		このclass CTextOutputStreamVersatile のインスタンス
*/
public:
	virtual void	SelectDevice(const smart_ptr<ITextOutputStream>& device)
	{ m_vDevice = device; }
	/**
		出力するデバイスのスマートポインタを渡す
		以降、そいつに出力するようになる
	*/

	void	Debug(){
		/**
			デバッグ用に、Error.txtというファイルに出力する
		*/
		CTextOutputStreamFile* p = new CTextOutputStreamFile;
		p->SetFileName("Error.txt");
		SelectDevice(smart_ptr<ITextOutputStream>(p));
	}

	virtual void	Clear() { m_vDevice->Clear();	}
	virtual void __cdecl Out(LPCSTR fmt, ... );	// {	m_vDevice->Out(fmt);}
	//	↑可変引数の委譲は不可か？
	virtual void	Out(int n){	m_vDevice->Out(n);	}
	virtual void	Out(const string&s){ m_vDevice->Out(s); }

	CTextOutputStreamVersatile()
#ifndef COMPILE_YANE_PLUGIN_DLL	//	plug-in DLLを作成しない場合のみ
		: m_vDevice(new CTextOutputStreamNullDevice)
	//	staticオブジェがnewしちゃいかんヽ(`Д´)ノ>DLL
#endif
	{}
		//	最初、NullDeviceを結合しておく

protected:
	smart_ptr<ITextOutputStream> m_vDevice;
};

#ifdef USE_ErrorLog		//	CErrorLogクラスを有効にする。
	//	エラー出力デバイスをひとつ用意
	extern CTextOutputStreamVersatile Err;
#else
	extern CTextOutputNullDevice Err;
	//	↑こないしとけば最適化で消えるでしょう．．
#endif
/**
	エラーログをファイルに出力する例：
	{
		CTextOutputStreamFile* p = new CTextOutputStreamFile;
		p->SetFileName("Error.txt");
		Err.SelectDevice(smart_ptr<ITextOutputStream>(p));
	}
*/

//////////////////////////////////////////////////////////////////////////

class IInputStream {
/**
	入力用ストリーム基底クラス
	class IOutputStream等も参考のこと。
	//	つくりかけ
*/
public:


	virtual ~IInputStream(){}
};

//////////////////////////////////////////////////////////////////////////

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
