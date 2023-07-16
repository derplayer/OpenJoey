// yaneFile.h
//
//	standard file stream wrapper
//		programmed by yaneurao '00/02/25
//		modified for yanePackDx '02/03/13

#ifndef __yaneFile_h__
#define __yaneFile_h__


namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class IFile {
public:
	virtual LRESULT		Open(const string& filename,const string& access)=0;
	virtual LRESULT			Write(const string&)=0;
	virtual LRESULT			Close()=0;
	virtual FILE*			GetFilePtr()const=0;
	virtual LRESULT			Read(const string& filename)=0;
	virtual const string	GetName()const=0;
	virtual string			GetPureFileName()const=0;
	virtual string			GetSuffix()const=0;
	virtual LPVOID			GetMemory()const=0;
	virtual DWORD			GetSize()const=0;
	virtual LRESULT			ReadLine(LPSTR buf,DWORD dwSize=256)=0;
	virtual LRESULT			ReadLine2(LPSTR buf,DWORD dwSize=512)=0;
	virtual LRESULT			ReadLine(string&s)=0;
	virtual void			Reset()=0;
	virtual LRESULT			ReadData(BYTE*p,DWORD size)=0;
	virtual LRESULT			Write(const string& filename,LPVOID mem,DWORD size)=0;
	virtual LRESULT			WriteBack(const string& filename)=0;
	virtual LRESULT			Encode()=0;
	virtual LRESULT			Delete(const string& filename)=0;
	virtual LRESULT			CreateTemporary(bool bForce=false)=0;
	virtual LRESULT			Load(const string& filename,vector<BYTE>*vData)=0;
	virtual LRESULT			Save(const string& filename,vector<BYTE>*vData)=0;
	virtual LRESULT			Save(const string& filename,vector<string>*vData)=0;
	virtual ~IFile(){}
};

class CFile : public IFile {
/**
ファイルの入出力用のクラスです。

読み込みは、yanePack形式(ファイル結合による合体)、yanePackEx形式
(LZSS法による圧縮),yanePackDx形式(サブフォルダ圧縮対応のyanePackEx)
のファイルも対応しています。

書き込みは、残念ながら、yanePack/yanePackEx形式のファイルには書き込めません。

yanePack形式／yanePackEx形式／yanePackDx形式の仕様については後述します。
yanePack形式／yanePackEx形式／yanePackDxのファイルを作るためには、別途、
yanePack／yanePackEx／yanePackDxをご用意ください。


yanePackの仕様(yaneuraoGameSDK1.00でサポートしていた形式／互換のため読み込めます)

☆　内容

yanePackは、複数ファイルを一つにまとめるためのものです。（圧縮はしません）

CFileの一括読み込み系の関数ならば、yanePack形式であることを意識せずに読み出しが可能です。

 <yanepack用構造体について>
// ファイルヘッダー "yanepack" 8バイト + 格納ファイル数がDWORDできて、
// その後、以下のCFileInfoが格納ファイル数だけ来て、あとはデータです。
struct CFileInfo {
char filename[32];
DWORD startpos; // seek pos(ファイル先頭からのオフセット)
size_t filesize; 　　// file size（そのファイルのサイズ）
};

ファイルの拡張子はdatにしてください。
--------------------------------------------------------------------------------yanePackＥｘの仕様(yaneuraoGameSDK2ndでサポートする形式）
--------------------------------------------------------------------------------☆　内容
yanePackExは、複数ファイルを一つにまとめ、かつ圧縮するためのものです。圧縮にはLZSS法を用います（CLZSSクラスをそのまま利用します）
 <yanepackEx用構造体について>
// ファイルヘッダー "yanepkEx" 8バイト + 格納ファイル数がDWORDできて、
// その後、以下のCFileInfoが格納ファイル数だけ来て、あとはデータです。
struct CFileInfo {
char filename[32];
DWORD startpos; // seek pos(ファイル先頭からのオフセット)
size_t filesize; 　　// file size（そのファイルの圧縮サイズ）
size_t packsize;　 // packsize（そのファイルの展開後のサイズ)
// 非圧縮のファイルは、filesize==packsizeとなっています
};
ファイルの拡張子はdatにしてください。
--------------------------------------------------------------------------------yanePackＤｘの仕様(yaneuraoGameSDK3rdでサポートする形式）
--------------------------------------------------------------------------------☆　内容
yanePackDxは、yanePackExのフォルダ対応版です。
 <yanepackDx用構造体について>
// ファイルヘッダー "yanepkDx" 8バイト + 格納ファイル数がDWORDできて、
// その後、以下のCFileInfoが格納ファイル数だけ来て、あとはデータです。
struct CFileInfo {
char filename[256];
DWORD startpos; // seek pos(ファイル先頭からのオフセット)
size_t filesize; 　　// file size（そのファイルの圧縮サイズ）
size_t packsize;　 // packsize（そのファイルの展開後のサイズ)
// 非圧縮のファイルは、filesize==packsizeとなっています
};
ファイルの拡張子はdatにしてください。
ファイル名は、
	"test\subfolder\sample.txt"
のように、フォルダ付きで格納されていることがあります。
よって、
	CFile::Readは、
		"test\subfolder\sample.txt"
	というファイルが無ければ、次は
		"test\subfolder.dat" のなかの"sample.txt"
	それが無ければ
		"test.dat"のなかの"subfolder\sample.txt"
	というように、ルートディレクトリまで遡及して検索していきます。

*/
public:
	///	ストリーム系ファイル操作
	virtual LRESULT			Open(const string& filename,const string& access);
															///	Stream系Open
	virtual LRESULT			Write(const string&);
				///	Stream系Write
				///	書式なし出力なので % とか混じってても安心
	virtual LRESULT			Close();								///	Close
	virtual FILE*			GetFilePtr()const { return m_lpFile; }
	/**
Openでファイル名を指定し、オープンします。accessはアクセス指定子で、fopenで指定するものと同じです。Writeは、文字列を出力します（それしか有りません） Closeは、ファイルを閉じます。何かのときはGetFilePtrでオープンしているファイルのFILE*が得られるので、それを間接で書き込んで行くことは出来ます。逆にユーザー側でOpenしたFILE*をSetFilePtrを設定し、CFileのメンバ関数(と言ってもWriteとCloseぐらいしかありませんが)を使うことも出来ます。
*/

	///			一括読み込み系
	/**
		ファイル名は"c:\"とかのように:が含まれていなければ、起動ディレクトリ
		(SetCurrentDirで設定されているディレクトリ)相対パスです。
	*/
	virtual LRESULT			Read(const string& filename);
	///		一括読み込み系（yanepack系対応）

	virtual const string	GetName()const;
	///	読み込んでいるファイル名を返す
	virtual string			GetPureFileName()const;
	///		パス無しファイル名を取得
	virtual string			GetSuffix()const;
	///		読み込んでいるファイルの拡張子を返す
	virtual LPVOID			GetMemory()const;
	///		読み込んでいるメモリを返す
	virtual DWORD			GetSize()const;
	///		読み込んでいるファイルサイズを返す
	///		Readで読み込んだものを解放するには、Close()を呼び出すこと

	/**
	Readでオープンします。オープンした瞬間に、メモリに一括で読み込まれます。Readで読み込んだものを解放するには、Close()を呼び出します。Close()を呼び出すまで、GetMemoryで取得されるファイルの内容が格納されているメモリは有効です。
	*/

	///		メモリに読み込んだファイルを一行ずつ読み出すルーチン
	virtual LRESULT			ReadLine(LPSTR buf,DWORD dwSize=256);
	/// バッファはdwSizeバイト用意しといてねん
	///		返し値 0: 正常終了	1: EOF	2:バッファあふれ
	///		3:ファイル読み込んでいない

	virtual LRESULT			ReadLine2(LPSTR buf,DWORD dwSize=512);
	///		バッファはdwSizeバイト用意しといてねん
	///		これは、シナリオファイル("..."が1メッセージ,終了文字"END"であるファイル)の読み込み用

	virtual LRESULT			ReadLine(string&s);
	/// こちらは、バッファは自前で用意しなくて良い。
	///	返し値 0: 正常終了 1:EOF   3:ファイル読み込んでない＾＾；

	virtual void			Reset();
	///	上のReadLineでの読み込みポインタを先頭ポインタに戻す

	virtual LRESULT			ReadData(BYTE*p,DWORD size);
	//	バイト単位の読み出し

	///		一括書き込み系
	///	一気にファイルに書き込みます。圧縮ファイルには書き出せません。
	virtual LRESULT			Write(const string& filename,LPVOID mem,DWORD size);

	///	ReadOpenしたものを書き戻します。圧縮ファイルには書き戻せません。
	virtual LRESULT			WriteBack(const string& filename);

	///	とっさに暗号化（単なるニブル入れ替え）
	virtual LRESULT			Encode();	///	内部バッファをエンコードする

	///	ファイルの削除
	virtual LRESULT			Delete(const string& filename);

	virtual LRESULT			CreateTemporary(bool bForce=false);
	/**
必要ならばテンポラリファイルを作成し書き出す。（一括読み込み系のオープンが成功していることが前提）
この関数が終了後は、CFileはファイルの中身は保有していない。ファイル名だけは持っているので、GetNameでファイル名の取得は可能。
テンポラリファイルを作成する条件は、読み込んでいたファイルが圧縮ファイルであること。
テンポラリファイルを作成した場合は、GetNameで返ってくるファイル名は、そのファイル名となる。テンポラリファイルは、Closeか、デストラクタで削除される。

つまり、メモリ上のファイルでは実行できないAPI(例：MCI関連等)に対し、一時ファイルを用意するのに使う。

bForce == trueの場合は、強制的にテンポラリファイルを作り、そちらに書き出す。
（ディフォルトではfalse）
	*/

	///	データの保存／読み込み(class CSerializeと併用すると良い)
	virtual LRESULT			Load(const string& filename,vector<BYTE>*vData);
	virtual LRESULT			Save(const string& filename,vector<BYTE>*vData);

	//	データの保存(文字列保存)
	virtual LRESULT			Save(const string& filename,vector<string>*vData);

	///	-----------------------------------------------------
	///		☆　ファイルの探索pathを設定する

	///	ReadするときのPath設定
	static void		SetPathList(const smart_ptr<vector<string> >& astrPathList){ m_astrPathList = astrPathList; }
	///	でvListにかかれているパスへもファイルを見に行く。
	///	カレントフォルダからのパスが優先検索経路。
	///	パス名の最後尾には、'\'は不要。

	typedef vector<string> pathlist;
	static smart_ptr<pathlist> GetPathList() { return m_astrPathList; }

	///	-----------------------------------------------------

	///		起動フォルダ検出系
	/**
CFileが対象としているのは、CFileのメンバm_szCurrentDirectoryです。これは、GetCurrentDirかSetCurrentDirが呼び出されるまで初期化されません。起動直後のカレントフォルダをCFileの対象フォルダとしたいのであれば、起動直後にCFile::SetCurrentDir()を呼び出す必要があります。この関数は、現在のカレントフォルダをCFileのカレントフォルダに設定するからです。Windowsは、ダイアログ等を開いてフォルダ移動を行なうと、そこをカレントフォルダに設定してしまいますので、そうなってからGetCurrentDirを呼び出すと、CFileのメンバm_szCurrentDirectoryは、初期化されていないため、その瞬間、現在のカレントフォルダを取得しようとしますが、それは移動した先のフォルダであるため、起動フォルダ相対にはなりません。細かいことですが、気をつけてください。

class CFileInitializer が、class CAppInitializer の初期化コールバックにおいて
SetCurrentDirするので、class CAppInitializer を用いているプログラムならば
以上のことは、気にしなくて良い。
	*/
	static string	GetCurrentDir();
	/// 現在、CFileでカレントフォルダとして設定されているフォルダ名を取得

	static void		SetCurrentDir();
	/// 現在のカレントフォルダを、CFileのカレントフォルダにする

	static void		SetCurrentDir(const string& dir);
	/// 指定したパスをCFileのカレントフォルダにする。

	static	LRESULT	GetParentDir(string& filename);
	//	親フォルダを返す
	/**
	例） "c:\test1\test2\test3.exe" -> "c:\test1\test2\" -> "c:\test1\" -> "c:\" -> "c:\"というように、一度呼び出すごとに一つずつ親フォルダになる。ネットワークドライブにも対応している。（以下の例）

	例
	\\ -> \\
	\\aaa -> \\aaa\
	\\aaa\ -> \\aaa\
	\\aaa\bbb -> \\aaa\
	\\aaa\bbb\ -> \\aaa\
	\\aaa\bbb\ccc -> \\aaa\bbb\
	\\aaa\bbb\ccc\ -> \\aaa\bbb\
	\\aaa\bbb\ccc\ddd -> \\aaa\bbb\ccc\
	\\aaa\bbb\ccc\ddd\ -> \\aaa\bbb\ccc\
	\ -> \
	C: -> C:\
	C:\ -> C:\
	C:\aaa -> C:\
	C:\aaa\ -> C:\
	C:\aaa\bbb -> C:\aaa\
	C:\aaa\bbb\ -> C:\aaa\
	C:\aaa\bbb\ccc -> C:\aaa\bbb\
	C:\aaa\bbb\ccc\ -> C:\aaa\bbb\
	C:\aaa\bbb\ccc\ddd -> C:\aaa\bbb\ccc\
	C:\aaa\bbb\ccc\ddd\ -> C:\aaa\bbb\ccc\
	*/

	static string	MakeFullName(const string& filename);
	/// 起動ディレクトリを補って完全パスを作る
	/**
⇒　このとき、 / （スラッシュ）は、 \ （バックスラッシュ）へ変換するようになっています。（そうしないと、うまく動かないコーデック等が存在するため）　よって、何らかの読み込み関数を自作する場合は、かならず、このMakeFullNameを経由させたほうが無難と言えます。
	*/

	static string	GetWindowsDir();
	///	Windowsディレクトリの取得（終端は'\'）

	///		ファイル関連文字列操作系
	static string	GetSuffixOf(const string&);
	///		ファイル名の拡張子取得

	static string	GetPureFileNameOf(const string&);
	///		パス付きファイル名からファイル名部分のみを取得

	static void		ToLower(string &);
	///		小文字化する

	static LRESULT	GetTemporaryFileName(string& strFileName);
	///	テンポラリファイルを取得する
	///	この関数を実行した瞬間、ファイルが生成され、そのファイル名が返る

	/**
ファイル名は、内部的にMakeFullNameで完全パス付きのファイル名に変換されてから実行されます。MakeFullNameには、CFileのメンバm_szCurrentDirectoryが使われます。（上の対象フォルダ設定系の説明を読むこと）　最初の呼び出し段階で、このメンバ変数が初期化されていない場合は、現在のカレントディレクトリを取得し、それをm_szCurrentDirectoryにします。通常、この仕様で問題ないと思うのですが、ダイアログ等を使う場合はこの限りではありません。注意してください。

MakeFullNameでは、"../../test.wav"のような駆け上がりパスもサポートしています。Open等の関数でも、最初にこの関数を呼び出してパス補正をするので、このような駆け上がりパスが利用できます。

（一応、CAppInitializer::Init関数内でSetCurrentDir(void)を呼び出しているので問題ないと思いますが、CFileだけご自分のプログラムに流用されるときなどには気をつけてください）

また、ファイルを読み込んだときに内部的に確保されるメモリは、テキストファイルを読み込んだときに、最後は'\0'を保証するため、１バイト多めに確保して最後のバイトを'\0'にしています
*/

/**
☆　メモリイメージをCFileに読み込ませる

たとえば、ファイル名をもらい、それをCFileでReadで読み込むようなクラスを作るとき、メモリ上に配置されたイメージを指定したい場合などは、それ用の関数も用意しなくてはなりません。このような部分をCFileが少しでも吸収してくれれば良いと思ったので、メモリ上に配置したデータを、以下のようにして「！」で始まり、１６進数の文字列でメモリの先頭アドレスと、そのブロックのサイズをファイル名として指定してやれば

	LPCSTR lpsz = "これ表示できるかな？<HR>うまくいけば良いのだけど"
	"そう簡単でもないか？<HR>いけてるいけてる．．<HR>";
	::wsprintf(buf,"!%x,%x",lpsz,strlen(lpsz)+1);

CFileは、これをファイルと錯覚し、あたかもファイルであるかのように扱うことが出来ます。
*/

	static bool PathFileExists(const string& fullname);

	CFile();
	virtual ~CFile();

#ifndef COMPILE_YANE_PLUGIN_DLL
	//	CFileのグローバル初期化子
	class CFileInitializer {
		static CFileInitializer m_dummy;
	public:
		CFileInitializer();
	};
#endif

protected:
	LRESULT			InnerRead(const string& filename);				//	Readのpath無効バージョン
	LRESULT			InnerOpen(const string& filename,const string& access);	//	Openのpath無効バージョン

	FILE*			m_lpFile;			// 書き込み時に使用する(cf.yanefileDLL)
	LPVOID			m_lpFileAdr;		// ファイルの読み込んだアドレス
	DWORD			m_dwFileSize;		// ファイルサイズなの！
	LPCSTR			m_lpFileMemPos;		// ReadLineのためのメモリポジション
	string			m_strFileName;		// 読み込んでいるファイル名
	bool			m_bEncode;
	bool			m_bTempFile;		//	テンポラリファイルなのか？(closeの際に削除される)
	bool			m_bCompress;		//	Openで読み込んだファイルは圧縮ファイルだったのか？
	bool			m_bMemoryImage;		//	メモリ上に読み込まれた仮想ファイル

	static string	m_strCurrentDirectory;		// 現在対象としているディレクトリ
	static smart_ptr<pathlist>	m_astrPathList;	//パスリスト
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
