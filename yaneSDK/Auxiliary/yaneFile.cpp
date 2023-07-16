#include "stdafx.h"
#include "yaneFile.h"
#include "yaneLZSS.h"
#include "yaneStringScanner.h"
#include "../AppFrame/yaneAppInitializer.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

#ifndef COMPILE_YANE_PLUGIN_DLL

//	CFileの初期化用オブジェクト
CFile::CFileInitializer::CFileInitializer(){
	//	BCCは明示的にキャストしないとオーバーロードされた関数を判別できない
	void(*f)() = &CFile::SetCurrentDir;
	CAppInitializer::RegistInitCallback( smart_ptr<function_callback>(function_callback_v::Create(f)) );
}
CFile::CFileInitializer m_dummy;

#else
//　仕方ないので、CAppInitializer::DllInit内で初期化するか..
#endif

//////////////////////////////////////////////////////////////////////////////

string	CFile::m_strCurrentDirectory;		// 現在対象としているディレクトリ
smart_ptr<vector<string> >	CFile::m_astrPathList;	// パスリスト

//////////////////////////////////////////////////////////////////////////////
//	<yanepack用構造体について>
//		ファイルヘッダー "yanepack" 8バイト + 格納ファイル数 DWORD
//		その後、以下のCFileInfoが格納ファイル数だけ来て、あとはデータ
struct CFileInfo {
	CHAR	filename[32];
	DWORD	startpos;	//	seek pos
	size_t	filesize;	//	file size
};
struct CFileInfoEx {
	CHAR	filename[32];
	DWORD	startpos;	//	seek pos
	size_t	filesize;	//	file size(展開後のファイルサイズ)
	size_t	arcsize;	//	arc size (圧縮サイズ)
};
struct CFileInfoDx {
	CHAR	filename[256];	//	階層化ファイルなので、サイズでかしとこ
	DWORD	startpos;	//	seek pos
	size_t	filesize;	//	file size(展開後のファイルサイズ)
	size_t	arcsize;	//	arc size (圧縮サイズ)
};
//////////////////////////////////////////////////////////////////////////////

CFile::CFile() {
	m_lpFile = NULL;
	m_lpFileAdr		= NULL;
	m_dwFileSize	= 0;
	m_lpFileMemPos	= NULL;
	m_bEncode		= false;
	m_bTempFile		= false;
	m_bCompress		= false;
	m_bMemoryImage	= false;
}

CFile::~CFile() {
	Close();
}

LRESULT CFile::Close(){
	if (m_lpFile!=NULL) {
		LRESULT l;
		l = ::fclose(m_lpFile);
		m_lpFile = NULL;
		return l;
	}
	if (m_bMemoryImage) {
	//	メモリ上の仮想ファイル
		m_lpFileAdr = NULL;
		m_dwFileSize = 0;
		m_lpFileMemPos = NULL;
		m_bMemoryImage = false;
	}
	if (m_lpFileAdr!=NULL) {
		::GlobalFree(m_lpFileAdr);
//		delete [] m_lpFileAdr;
		m_lpFileAdr = NULL;
		m_dwFileSize = 0;
		m_lpFileMemPos = NULL;
	}
	if (m_bTempFile) {
		m_bTempFile = false;
		Delete(m_strFileName);	//	ファイルの削除
	}
	m_bCompress	= false;
	m_strFileName.erase();
	return 0;
}

//	ファイルの削除
LRESULT	CFile::Delete(const string& filename){
	//	こんなん渡すなよ...
	if (filename.empty()) return -1;

	string fn(MakeFullName(filename));
	return !::DeleteFile(fn.c_str());
}

LRESULT	CFile::CreateTemporary(bool bForce/* = false*/){
	if (m_lpFileAdr==NULL && !bForce) return 1;
	if (m_bCompress || m_bMemoryImage || bForce) {
	//	圧縮ファイルか、メモリイメージ（ファイル実体が無い）
	//	のときのみduplicateする

		string strTmpFileName;
		LRESULT lr = GetTemporaryFileName(strTmpFileName);
		if (lr!=0) return 2;

		if (WriteBack(strTmpFileName) != 0) {
//			Err.Out("CFile::CreateTemporary::テンポラリファイルに書き込めない。");
			return 2;
		}
		Close();
		//	write backしたほうを自分のガメっているファイル名とする
		m_bTempFile = true;
		m_strFileName = strTmpFileName;
	} else {
		//	こちらも、Closeしてしもたほうが健全＾＾；
		string s;
		s = m_strFileName;
		Close();
		m_strFileName = s;
	}
	return 0;
}


LRESULT	CFile::GetTemporaryFileName(string& strFileName){
	//	テンポラリファイルを取得する

	CHAR	szTmpPath[MAX_PATH];
	CHAR	szTmpFileName[MAX_PATH];
	if (::GetTempPath(sizeof(szTmpPath), szTmpPath) == 0) {
//		Err.Out("CFile::CreateTemporary::テンポラリパスが取得できない。");
		return 2;
	}
	if (::GetTempFileName(szTmpPath, "YNE", 0, szTmpFileName) == 0) {
//		Err.Out("CFile::CreateTemporary::テンポラリファイルが作成できない。");
		return 2;
	}
	strFileName = szTmpFileName;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
string CFile::GetCurrentDir() {
	if (m_strCurrentDirectory.empty()) {
		//	無いようなので、設定する
		CHAR dir[_MAX_PATH];
		::GetCurrentDirectory(_MAX_PATH,dir);
		SetCurrentDir(dir);
	}
	return m_strCurrentDirectory;
}

void CFile::SetCurrentDir(){
	CHAR dir[_MAX_PATH];
	::GetCurrentDirectory(_MAX_PATH,dir);
	SetCurrentDir(dir);
}

void CFile::SetCurrentDir(const string& dir){

	bool bDir = false;
	m_strCurrentDirectory = dir;
	if (!m_strCurrentDirectory.empty() && m_strCurrentDirectory[0]=='"') {
		bDir = true;
		m_strCurrentDirectory.erase(0,1);	// コマンドラインを渡したなぁ...
	}
	// c:\test\test.exe　のようにコマンドラインを入れてカレントディレクトリが
	// 設定できるように工夫。(bDir==trueのとき)
	// c:\test\　まで。最後の\を含めて保存。

	string::size_type pos;
	if (bDir) {
		pos = m_strCurrentDirectory.find('.');
	} else {
		pos = string::npos;
	}
	string::iterator it;
	if (pos==string::npos) {		// . が無いならば
		it = m_strCurrentDirectory.end() - 1;
		if (*it!='\\' && *it!='/') {
			// \か/で終わっていないならば\を追加
			m_strCurrentDirectory += '\\';
		}
	} else { // c:\test\test or c:\test\test.exe
		GetParentDir(m_strCurrentDirectory);
	}
}

LRESULT	CFile::GetParentDir(string& filename){						//	親フォルダを返す
// e.g. "c:\test1\test2\test3.exe" -> "c:\test1\test2\" -> "c:\test1\" -> "c:\" -> "c:\"
	//	漢字の１バイト目を避ける必要あり...
	//
	//	1. もし、ネットワークドライブで有れば、コンピュータ名をスキップする。
	//	　　そうすることで、ローカルのパスと同じ扱いになる。
	//　　　（\\HOST\PATH までが、ドライブレターであるとする ＞ 仕様
	//	2. \と/を探す。その際に最後に見つかった位置と最後から２番目に見つかった位置を記録しておく
	//	3. 末尾が\でも/でもない場合、ルートならば￥を付加する。ルートでなければ、最後　　　　　に見つかったところまでを親ディレクトリとして返す
	//	4. 末尾が\ か /である場合　、ルートならばそのまま返す。ルートでなければ、最後から２番目に見つかったところまでを親ディレクトリとして返す
	//
	//	2001/07/29 sohei
	//	  ルートに有るファイル名を渡されたら\や/は一つしかないが、ルートを返さないといけない
	//	2001/08/06 sohei
	//	  string が \ を付加することで拡張された場合、バグるのを修正
	//	　ねこ☆あるふぁ～　さん報告有り難うございます。

	if (filename.empty()) return 1;	//	こんなん渡すな^^;

	string::iterator it = filename.begin();
	if (filename.substr(0,2)=="\\\\") {
		//	ネットワークパス
		it += 2;	if (*it == '\0')	return 1;	//	\\ でおわりかいな^^;
		//	このときは、コンピュータ名をスキップする
		bool bKanji=false;
		for(;;it++){
			if (*it == '\0') break;
			if (bKanji) { bKanji=false; continue; }
//			if (((BYTE)*it>=0x80 && (BYTE)*it<=0xa0) || ((BYTE)*it>=0xe0 && (BYTE)*it<=0xff)) {
			if (  CStringScanner::IsLead_ShiftJIS(*it)  ){
				bKanji = true;	//	２バイトコードの１バイト目であった
			} else {
				bKanji = false;	//	２バイトコードの１バイト目ではなかった
				if (*it == '\\' || *it=='/') {	//	区切り文字
					it++;	//	\\HOST\ の次からとすればローカルと同じ扱いになる
					break;
				}
			}
		}
	} else {
	}
	string::iterator pos1,pos2;	
	// pos1 : 最後に見つかった\か/の位置
	// pos2 : 最後から２番目に見つかった\か/の位置
//	pos1 = pos2 = string::npos;	// not found
	pos1 = pos2 = filename.end();	// not found

	bool bKanji=false;
	
	while (true) {
		if (*it == '\0') break;
		if (bKanji) { bKanji=false; goto skip; }
//		if (((BYTE)*it>=0x80 && (BYTE)*it<=0xa0) || ((BYTE)*it>=0xe0 && (BYTE)*it<=0xff)) {
		if ( CStringScanner::IsLead_ShiftJIS(*it) ){
			bKanji = true;	//	２バイトコードの１バイト目であった
		} else {
			bKanji = false;	//	２バイトコードの１バイト目ではなかった
			if (*it == '\\' || *it=='/') {	//	区切り文字
				pos2 = pos1;
				pos1 = it;
			}
		}
skip:;
		it++;
	}
	//	2001/08/06	sohei
	//		string が伸張したときに、イテレータが無効になって、
	//		不正な処理を引き起こしていたのを修正。
	//		ねこ☆あるふぁ～　さん有り難うございます。

	//	末尾が\でも/でもないやん
	if ( !bKanji && (*(it-1)!='\\' && *(it-1)!='/')) {
		//	この場合は最後に見つかった所までが必要。
		if (pos1 == filename.end()) {	//	一つも\が無いならルートなので\を付加する
			filename+='\\';
		} else {
			filename.erase(pos1+1,filename.end());	//	ルートではないので、最後に見つかったところまでを返す
		}
	} else {
		//	この場合は２番目に見つかった所までが必要。
		if (pos2 == filename.end()) return 1;	//	ルートなんで取り除く必要はない
		filename.erase(pos2+1,filename.end());		//	最後から２番目までを返せばよい
	}
	return 0;
}

string	CFile::GetSuffix()const{
	return GetSuffixOf(m_strFileName);
}

string	CFile::GetPureFileName()const{
	return GetPureFileNameOf(m_strFileName);
}

// ------ static function ------
string	CFile::GetSuffixOf(const string& filename){
	string suffix;
	suffix = GetPureFileNameOf(filename);
	// 　　　　↑間にディレクトリ記号があるとまずいのでパス名除去
	string::size_type pos;
	pos = suffix.rfind('.');
	if (pos==string::npos) {
		suffix.erase();
	} else {
		suffix.erase(0,pos+1);
	}
	return suffix;
}	

string	CFile::GetPureFileNameOf(const string& filename){

	if (filename.empty()) return string("");

	string purefilename;
	string::iterator pos1;
	// pos1 : 最後に見つかった\か/の位置
	purefilename = filename;
	string::iterator it = purefilename.begin();
	pos1 = purefilename.end();	// not found
	bool bKanji=false;
	while (true) {
		if (*it == '\0') break;
		if (bKanji) { bKanji=false; goto skip; }
//		if (((BYTE)*it>=0x80 && (BYTE)*it<=0xa0) || ((BYTE)*it>=0xe0 && (BYTE)*it<=0xff)) {
		if ( CStringScanner::IsLead_ShiftJIS(*it) ){
			bKanji = true;	//	２バイトコードの１バイト目であった
		} else {
			bKanji = false;	//	２バイトコードの１バイト目ではなかった
			if (*it == '\\' || *it=='/') {
				pos1 = it;
			}
		}
skip:;
		it++;
	}
	if (pos1!=purefilename.end()){
		purefilename.erase(purefilename.begin(),pos1+1);
	}
	return purefilename;
}

//////////////////////////////////////////////////////////////////////////////
//	static function..
void  CFile::ToLower(string &str){
	//	小文字化する(do_tolower)
	string::iterator it = str.begin();
	bool bSkip = false;
	while (it!=str.end()){
		if (bSkip) {
			bSkip = false;
			goto Repeat;
		}

//		if (((BYTE)*it>=0x80 && (BYTE)*it<=0xa0) || ((BYTE)*it>=0xe0 && (BYTE)*it<=0xff)) {
		if ( CStringScanner::IsLead_ShiftJIS(*it) ){
			//	２バイトコードの１バイト目であった
			bSkip = true;
			goto Repeat;
		} else {
			//	２バイトコードの１バイト目ではなかった
			bSkip = false;
		}
		*it = tolower(*it);
Repeat:;
		it ++;
	}
}
//////////////////////////////////////////////////////////////////////////////

LRESULT CFile::Open(const string& filename,const string& access)
{
	LRESULT lr;
	//	カレントフォルダの読み込み
	lr = InnerOpen(MakeFullName(filename), access);
	if (lr==0) return 0;

	if (GetPathList().isNull()) return lr;	//	パス指定されてないじゃん．．＾＾；

	//	設定されていたパスを順次たどって調べる
	pathlist::iterator it = GetPathList()->begin();
	while (it!=GetPathList()->end()){
		lr = InnerOpen(*it + '/' + filename, access);
		if (lr == 0) return 0; // いけたじょ＾＾；
		it++;
	}
	return lr;	//	だめだったじょ、、（たぶん）
}

LRESULT CFile::InnerOpen(const string& filename,const string& access)
{
	Close();
	m_lpFile = fopen(filename.c_str(), access.c_str());
	if (m_lpFile==NULL) return 1;

	// ファイル名を設定
	m_strFileName = filename;
	return 0;
}

LRESULT CFile::Write(const string& s) {
	if (m_lpFile==NULL) return -1;
	if (fprintf(m_lpFile,"%s",s.c_str())<0) return 1;
	return 0;
}

LRESULT	CFile::WriteBack(const string& filename){
	return Write(filename,GetMemory(),GetSize());
}

//////////////////////////////////////////////////////////////////////////////

DWORD	CFile::GetSize() const {
	if (m_bEncode) return m_dwFileSize - 4;	//	４バイト減る
	return m_dwFileSize;
}

LPVOID	CFile::GetMemory() const {
	if (m_bEncode) return (BYTE*)m_lpFileAdr + 4;	//	識別ヘッダの４バイトずれる
	return m_lpFileAdr;
}

LRESULT	CFile::Encode(){
	if (*(DWORD*)m_lpFileMemPos == 0x314B5059) return 1;	// already encoding!!
	if (m_bEncode) return 1;	//	already encoding!!
	
	DWORD size = m_dwFileSize+4;
	BYTE* m_lpFileAdr2 // = new BYTE[size+1];
		= (BYTE*)GlobalAlloc(GMEM_FIXED | GMEM_NOCOMPACT,size+1);

	//	１バイト多めに確保して、最後は\0を保証する。そうすれば、
	//	文字列比較等がやりやすくなる。
	*((BYTE*)m_lpFileAdr2 + size) = 0;

	for (DWORD i=0;i<size-4;i++){
		//	ニブルで入れ替えるだけ:p
		*(m_lpFileAdr2+i+4) =	(((*((BYTE*)m_lpFileAdr+i) & 0xf) << 4) +
								 ((*((BYTE*)m_lpFileAdr+i) &0xf0) >> 4)) ^ 0xcc;
	}
	*(DWORD*)m_lpFileAdr2 =	 0x314B5059;	// YPK1:ヘッダ付与

	Close();

	m_lpFileAdr		= (LPVOID)m_lpFileAdr2;
	m_dwFileSize	= size;
	m_lpFileMemPos	= (LPCSTR)m_lpFileAdr; // for ReadLine
	m_bEncode		= false;	//	あえてfalseにすることによってヘッダー付与
	return 0;
}

string	CFile::GetWindowsDir(){
	//	システムディレクトリ取得（終端は\\）
	CHAR	buf[MAX_PATH];
	::GetWindowsDirectory(buf,MAX_PATH);
	if (buf[::lstrlen(buf)-1]!='\\'){	//	終端は'\'か？
		::lstrcat(buf,"\\");
	}
	return buf;	//	コピーコンストラクタが起動するので安全
}

//////////////////////////////////////////////////////////////////////////////
string	CFile::MakeFullName(const string& filename){	// 起動ディレクトリを補って完全パスを作る
	string file;
	if (filename.substr(0,2)=="\\\\") {
			//	ネットワークパスなので何もしない
			file = filename;
	} else {
		string::size_type pos = filename.find(':');
		if (pos==string::npos) {
			//	フルパスでなければカレントディレクトリを補う

			// ..\\での駆け上がり相対パスをサポート
			string dir;
//			dir = m_strCurrentDirectory;
			dir = GetCurrentDir();
				//	↑設定されていなければ、この瞬間に設定する
			file = filename;
			while (true) {
				if (file.substr(0,3)=="..\\" || file.substr(0,3)=="../"){
					GetParentDir(dir);
					file.erase(0,3);
				} else if ((file.substr(0,2)==".\\" || file.substr(0,2)=="./")) {
					//	./によりカレントフォルダを示す
					file.erase(0,2);
				} else if (!CStringScanner::IsLead_ShiftJIS(file.at(0))&&(file.substr(0,1)=="\\" || file.substr(0,1)=="/")){
					//	\単独なら無視する  マルチバイト文字の1byte目を考慮する
					file.erase(0,1);
				} else {
					break;
				}
			}
			file = dir + file;
		} else {
			file = filename;
		}
	}
	/*
		'01/10/07	やねうらお
			/	だとうまく読み込めないことがあるようなので、\に置換する＾＾；
		'02/02/11	炎羅
			マルチバイトを考慮していなかったので修正した
			ファイルの存在チェックをして、無かったらGetPathで探すようにした
	*/


//	コメントアウト by やねうらお '02/03/14 01:36
#if 0

	// GetPath()!=NULL時は、指定されたファイルが存在するかチェック
	if(!GetPath().isNull()){
		bool bFound = false;
		if(!PathFileExists(file)){
			// 圧縮されているかも
			string fullname(file);
			string inner_filename(fullname);
			GetParentDir(fullname);
			inner_filename.erase(0, fullname.size());
			fullname[fullname.size() - 1] = '.';
			fullname += "dat";
			if(PathFileExists(fullname)) { bFound=true; }	// 圧縮ファイルが見つかった
		}else{
			bFound=true;
		}

		// 見つからないって事は他の場所にある
		if(!bFound){
			const string file2 = file;
			vector<string>::iterator it = GetPath()->begin();
			while(it!=GetPath()->end()){
				file = *it + '/' + filename;
				if(!PathFileExists(file)){
					// 圧縮されているかも
					string fullname(file);
					string inner_filename(fullname);
					GetParentDir(fullname);
					inner_filename.erase(0, fullname.size());
					fullname[fullname.size() - 1] = '.';
					fullname += "dat";
					if(PathFileExists(fullname)) { bFound=true;	 break; }	// 圧縮ファイルが見つかった
				}else{
					bFound=true;  break;
				}
				it++;
			}
			// 見つからなかったら、カレントのに戻しておこう
			if(!bFound) { file = file2; }
		}
	}
#endif

	{
		bool bKanji=false;
		string::iterator it = file.begin();
		while (it!=file.end()){
			if( CStringScanner::IsLead_ShiftJIS(*it) ){
				//	２バイトコードの１バイト目であった
				bKanji=true;
			}else{
				//	２バイトコードの１バイト目ではなかった
				if(!bKanji && *it == '/') { *it='\\'; }
				bKanji=false;
			}
			it++;
		}
	}

	return file;
}

bool CFile::PathFileExists(const string& fullname)
{
	HANDLE hFile = ::CreateFile(fullname.c_str(),
		GENERIC_READ,		// Read
		FILE_SHARE_READ,	// ReadOpenなら共有を許すのはマナー 
		NULL,				// security
		OPEN_EXISTING,		// 存在してなければエラー
		FILE_ATTRIBUTE_NORMAL,	//	ファイル属性
		NULL				// テンプレートファイル
	);

	if (hFile == INVALID_HANDLE_VALUE) { // あかんやん！
		// closeする必要はない(INVALIDハンドルなんだし)
		return false;
	}
	::CloseHandle(hFile);
	return true;
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CFile::Read(const string& strFileName){
	LRESULT lr;
	//	カレントフォルダの読み込み
	lr = InnerRead(strFileName);
	if (lr==0) return 0;

	if (m_astrPathList.isNull()) return lr;	//	パス指定されてないじゃん．．＾＾；

	//	設定されていたパスを順次たどって調べる
	pathlist::iterator it = m_astrPathList->begin();
	while (it!=m_astrPathList->end()){
		lr = InnerRead(*it + '/' + strFileName);
		if (lr == 0) return 0; // いけたじょ＾＾；
		it++;
	}
	return lr;	//	だめだったじょ、、（たぶん）
}

#pragma warning(disable:4701) // 値が割り当てられてない変数かもwarning
LRESULT CFile::InnerRead(const string& filename){

	if (filename.empty()) return -1; // 空やん？

	DWORD NumberOfBytesRead;
	int nFileType = 0;
	//	0:普通のファイル 1:yanePack
	//	2:yanePackEx 3:yanePackDx

	DWORD dwReadSize;	// 読み込むサイズ

	Close(); // 前に読み込んでたら、それを解放するなり！
	m_strFileName = filename;

	if (filename[0] == '!') {
		//	メモリイメージ
		if(::sscanf(filename.c_str(),"!%x,%x",&m_lpFileAdr,&m_dwFileSize)!=2) {
			return 6; // メモリ上のファイルでないやん？
		}
		m_lpFileMemPos	= (LPCSTR)m_lpFileAdr;
		m_bMemoryImage	= true;
		return 0;
	}
	m_bMemoryImage = false;

	string fullname;
	fullname = MakeFullName(filename);
	HANDLE hFile = ::CreateFile(fullname.c_str(),
		GENERIC_READ,		// Read
		FILE_SHARE_READ,	// ReadOpenなら共有を許すのはマナー 
		NULL,				// security
		OPEN_EXISTING,		// 存在してなければエラー
		FILE_ATTRIBUTE_NORMAL,	//	ファイル属性
		NULL				// テンプレートファイル
	);

	if (hFile == INVALID_HANDLE_VALUE) { // あかんやん！
		// closeする必要はない(INVALIDハンドルなんだし)

		//	ひょっとして圧縮ファイルか？
		//	test/script/game.c
		//	⇒ test/script.datにgame.cを探す
		//	⇒ test.datにscript/game.cを探す
		//	このように順番にかけあがって調べていく

		/*
		string::size_type pos1,pos2;
		pos1 = fullname.rfind('/');
		pos2 = fullname.rfind('\\');
		//	これらの記号が見つからないということは考えられない。
		//	後に存在するほう以降を切り落とす
		if (pos1 == string::npos) pos1=0;
		if (pos2 == string::npos) pos2=0;
		if (pos2 > pos1) pos1 = pos2;
		string inner_filename;
		inner_filename = fullname.substr(pos1+1,string::npos);
		fullname.erase(pos1);
		fullname += ".dat";
		*/

		//	fixed by TearDrop_Stone
		string inner_filename(fullname);
		string dirname(fullname);

	//	---- 圧縮ファイルは駆け上がって調べるので、そのリトライループ
CompressFileRetry:;

		GetParentDir(dirname);
		{
			string temp(dirname);
			GetParentDir(temp);
			if (temp==dirname){
				return 1;	//	これ以上、駆け上がれましぇん
			}
		}

		inner_filename = fullname.substr(dirname.size()
			,fullname.length()-dirname.length());
		dirname[dirname.size() - 1] = '.';
		dirname += "dat";

		hFile = ::CreateFile(dirname.c_str(),
			GENERIC_READ,		// Read
			FILE_SHARE_READ,	// ReadOpenなら共有を許すのはマナー 
			NULL,				// security
			OPEN_EXISTING,		// 存在してなければエラー
			FILE_ATTRIBUTE_NORMAL,	//	ファイル属性
			NULL				// テンプレートファイル
		);

		if (hFile == INVALID_HANDLE_VALUE) {
			goto CompressFileRetry;
		}

		//	圧縮ファイルが見つかった！
		CHAR ident[9];
		ident[8] = '\0';
		if (!::ReadFile(hFile,ident,8,&NumberOfBytesRead,NULL)){
			::CloseHandle(hFile);
			goto CompressFileRetry;
		}
		if (!strcmp(ident,"yanepack")) nFileType = 1;
		ef (!strcmp(ident,"yanepkEx")) nFileType = 2;
		ef (!strcmp(ident,"yanepkDx")) nFileType = 3;
		if (nFileType==0) {
			::CloseHandle(hFile);
			goto CompressFileRetry;
		}

		DWORD filenum;
		if (!::ReadFile(hFile,&filenum,sizeof(filenum),&NumberOfBytesRead,NULL)){
			::CloseHandle(hFile);
			goto CompressFileRetry;
		}
		
		bool bFound = false;
		
		ToLower(inner_filename);
		CFileInfo info;
		CFileInfoEx infoEx;
		CFileInfoDx	infoDx;
		DWORD dwStartPos; // ファイル開始オフセット
		for(int i=0;i<(int)filenum;i++){
			string s;
			switch (nFileType){
			case 1: // yanePack
				if (!::ReadFile(hFile,&info,sizeof(info),&NumberOfBytesRead,NULL)){
					::CloseHandle(hFile);
					goto CompressFileRetry;
				}
				s = info.filename;	//	小文字化してからテストする
				break;
			case 2: // yanePackEx
				if (!::ReadFile(hFile,&infoEx,sizeof(infoEx),&NumberOfBytesRead,NULL)){
					::CloseHandle(hFile);
					goto CompressFileRetry;
				}
				s = infoEx.filename;	//	小文字化してからテストする
				break;
			case 3: // yanePackDx
				if (!::ReadFile(hFile,&infoDx,sizeof(infoDx),&NumberOfBytesRead,NULL)){
					::CloseHandle(hFile);
					goto CompressFileRetry;
				}
				s = infoDx.filename;	//	小文字化してからテストする
				break;
			}
			ToLower(s);
			if (s == inner_filename) {
				//	一致した！
				// info.startposにSeek
				switch (nFileType){
				case 1:	m_dwFileSize = info.filesize;
						dwReadSize	 = info.filesize;
						dwStartPos	 = info.startpos;
					break;
				case 2:	m_dwFileSize = infoEx.filesize;
						dwReadSize	 = infoEx.arcsize;
						dwStartPos	 = infoEx.startpos;
					break;
				case 3:	m_dwFileSize = infoDx.filesize;
						dwReadSize	 = infoDx.arcsize;
						dwStartPos	 = infoDx.startpos;
					break;
				}
				bFound = true;
				break;
			}
		}
		if (!bFound) {
			::CloseHandle(hFile);
			//	さらに駆け上がって調べる
			goto CompressFileRetry;
		}
		m_bCompress	= true;	//	ここで設定しとこか＾＾；
		if (::SetFilePointer(hFile,dwStartPos,NULL,FILE_BEGIN)==0xFFFFFFFF) {
			::CloseHandle(hFile);
			goto CompressFileRetry;
		}
		goto StartOfRead;
	}
	
	// 4GB以上のファイルをメモリに読み込むことなんて考えられない
	m_dwFileSize = ::GetFileSize(hFile,NULL);
	
	if (m_dwFileSize == 0xFFFFFFFF) {
		m_dwFileSize = 0;
		::CloseHandle(hFile);
		// Err.Out("CFile::ReadFileでFileSize取得失敗");
		return 2; // filesizeの取得に失敗。（滅多にないと思うけど）
	}

	//	読み込みサイズ
	dwReadSize = m_dwFileSize;

StartOfRead:;
	//	まず読み込むためのメモリ確保！
	m_lpFileAdr = (LPVOID)::GlobalAlloc(GMEM_FIXED | GMEM_NOCOMPACT,m_dwFileSize+1);

	// m_lpFileAdr = new BYTE[m_dwFileSize+1];

	// いまからすぐ使うのに、このエリア圧縮しちゃやーよ。
	if (m_lpFileAdr==NULL) {
		::CloseHandle(hFile);
		// Err.Out("CFile::ReadFileでメモリ確保失敗");
		return 3; // メモリ確保できんかったわ。
				  // やっぱファイルがでかいといかんなー（笑）
	}
	//	１バイト多めに確保して、最後は\0を保証する。そうすれば、
	//	文字列比較等がやりやすくなる。
	*((BYTE*)m_lpFileAdr + m_dwFileSize) = 0;

	if ((nFileType==2||nFileType==3) && (m_dwFileSize!=dwReadSize)) {
		//	圧縮ファイルなので自前で読み込む
		smart_ptr<BYTE> lpBuf;
		lpBuf.AddArray(dwReadSize);
		if (lpBuf.isNull()) {
		//	でかすぎてメモリ足りん:p
			Close();
			::CloseHandle(hFile); // あれま...
			return 4;
		}
		if (!::ReadFile(hFile,lpBuf.get(),dwReadSize,&NumberOfBytesRead,NULL)){
		//	なぜか読み込み失敗..
			Close();
			::CloseHandle(hFile); // あれま...
			return 4;
		}
		//	圧縮ファイル
		CLZSS lzss;
		lzss.Decode(lpBuf.get(),*(BYTE**)&m_lpFileAdr,m_dwFileSize,false);	//	エラー処理はいらんよね？？
	} else if (!::ReadFile(hFile,m_lpFileAdr,dwReadSize,&NumberOfBytesRead,NULL)){
		Close();
		::CloseHandle(hFile); // あれま...
		// Err.Out("CFileでファイル読み込みに失敗");
		return 4; // ファイルの読み込みに失敗。
	}

	if (::CloseHandle(hFile)==false) {
		Close();
		// Err.Out("CFileでファイルのcloseに失敗");
		return 5; // Closeの失敗。そんなんあるんかいな...
	}

	if (*(DWORD*)m_lpFileAdr == 0x314B5059) {	//	HEADER:YPK1(yanepack1) == Data is Encoding...
		m_bEncode = true;
		for (DWORD i=4;i<m_dwFileSize;i++){
			//	ニブルで入れ替えるだけ:p
			*((BYTE*)m_lpFileAdr+i) =	(((*((BYTE*)m_lpFileAdr+i) & 0xf) << 4) +
										 ((*((BYTE*)m_lpFileAdr+i) &0xf0) >> 4)) ^ 0xcc;
		}
		m_lpFileMemPos = (LPCSTR)m_lpFileAdr + 4; // for ReadLine
	} else {
		m_bEncode = false;
		m_lpFileMemPos = (LPCSTR)m_lpFileAdr; // for ReadLine
	}

	return 0; // やっとこ正常終了:)
}
#pragma warning(default:4701) // 値が割り当てられてない変数かもwarning

LRESULT CFile::Write(const string& fileName,LPVOID lpMem,DWORD dwSize){

	string strTempName;
	strTempName = CFile::MakeFullName(fileName);
	HANDLE hFile = ::CreateFile(strTempName.c_str(),
		GENERIC_WRITE,		// Write
		0,					// 
		NULL,				// security
		TRUNCATE_EXISTING,
		FILE_ATTRIBUTE_NORMAL,	//	ファイル属性
		NULL				// テンプレートファイル
	);
	//	OPEN_ALWAYSでは、前のファイルサイズが残ってしまう...

	if (hFile == INVALID_HANDLE_VALUE) { // あかんやん！
		hFile = ::CreateFile(strTempName.c_str(),
			GENERIC_WRITE,		// Write
			0,					// 
			NULL,				// security
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,	//	ファイル属性
			NULL				// テンプレートファイル
		);
		if (hFile == INVALID_HANDLE_VALUE) return 1; // Open失敗		
	}

	DWORD NumberOfBytesRead = 0;
	if (!::WriteFile(hFile,lpMem,dwSize,&NumberOfBytesRead,NULL)){
		::CloseHandle(hFile); // あれま...
		// Err.Out("CFile::Writeでファイル書き込みに失敗");
		return 2;
	}

	if (!::CloseHandle(hFile)) {
		return 3; // Closeの失敗。そんなんあるんかいな...
	}

	return 0; // やっとこ正常終了:)
}

const string	CFile::GetName() const {
	// 読み込んでいるファイル名を返す
	return m_strFileName;
}

void CFile::Reset(){
	m_lpFileMemPos = (LPCSTR)GetMemory();
}

//	UNICODE非対応。しかもWindowsのテキストファイル専用:p
LRESULT		CFile::ReadLine(LPSTR buf,DWORD dwSize){	 // バッファはdwSizeバイト用意しといてねん
	if (m_lpFileMemPos == NULL) return 3; // どうなっとんのー！！
	if (m_lpFileMemPos >= (LPCSTR)m_lpFileAdr + m_dwFileSize) return 1;

	for(int i=0;i<(int)(dwSize-1);i++){
		if (*m_lpFileMemPos == 0x0D && *(m_lpFileMemPos+1) == 0x0A ||
			(m_lpFileMemPos >= (LPCSTR)m_lpFileAdr + m_dwFileSize)) { // CR+LF
			*buf = '\0'; // 一行終了！
			m_lpFileMemPos+=2;
			return 0;
		}
		*(buf++) = *(m_lpFileMemPos++);
	}
	*buf = '\0';
	return 2;	// buffer over
}

LRESULT		CFile::ReadLine(string&s){
	// こちらは、バッファは自前で用意しなくて良い。
	//	返し値 0: 正常終了 1:EOF   3:ファイル読み込んでない＾＾；
	if (m_lpFileMemPos == NULL) return 3; // どうなっとんのー！！
	if (m_lpFileMemPos >= (LPCSTR)m_lpFileAdr + m_dwFileSize) return 1;
//	if (m_lpFileMemPos+1 >= (LPCSTR)m_lpFileAdr + m_dwFileSize) return 1;
				//	↑ケツに'\0'を追加しているので、+1が必要?

	DWORD dwSize = 0;
	while (true) {
		if (*m_lpFileMemPos == 0x0D && *(m_lpFileMemPos+1) == 0x0A ||
			(m_lpFileMemPos >= (LPCSTR)m_lpFileAdr + m_dwFileSize)) { // CR+LF
//			*buf = '\0'; // 一行終了！
			//LPSTR psz = new CHAR[dwSize+1];
			//::CopyMemory(psz,m_lpFileMemPos-dwSize,dwSize);
			//*(psz+dwSize) = '\0';
			//s = psz;
			//delete [] psz;
			s.assign(m_lpFileMemPos-dwSize,dwSize);
			m_lpFileMemPos+=2;
			return 0;
		}
//		*(buf++) = *(m_lpFileMemPos++);
		++m_lpFileMemPos;
		++dwSize;
	}
	//	ここには来ない..
	return 0;
}

LRESULT		CFile::ReadLine2(LPSTR Senario,DWORD dwSize){
	smart_ptr<CHAR> lpBuf;
	lpBuf.AddArray(dwSize);	//	同サイズ確保

	::lstrcpy(Senario,"");	//	一応、終了したときにゴミを出さないように...
	if (ReadLine(lpBuf.get())!=0) return 1;
	if (!::lstrcmp(lpBuf.get(),"\"END\"")) return 1; // 終わりんこ
	::lstrcpy(Senario,lpBuf.get()+1);	//	先頭の"を削除

	while (true) {
		if (Senario[strlen(Senario)-1] == '\"') break;	//	デリミタ:="
		if (ReadLine(lpBuf.get())!=0) return 1;
		::lstrcat(Senario,"\n");
		::lstrcat(Senario,lpBuf.get());
	}
	Senario[::lstrlen(Senario)-1] = '\0';
	return 0;
}

LRESULT	CFile::ReadData(BYTE*p,DWORD size){
	if (m_lpFileMemPos == NULL) return 2; // どうなっとんのー！！
	if (m_lpFileMemPos >= (LPCSTR)m_lpFileAdr + m_dwFileSize) return 1;

	::CopyMemory(p,m_lpFileMemPos,size);
	m_lpFileMemPos+=size;
	return 0;
}

//	データの保存／読み込み(CSerializeと併用すると良い)
LRESULT		CFile::Load(const string& filename,vector<BYTE>*vData){
	LRESULT lr = Read(filename);
	if (lr!=0) return lr; // 読み込み失敗しとるやん．．．
	//	読み込んだファイルの長さを調べて、それ全部vDataにコピったる！
	const int nSize = GetSize();
	vData->resize(nSize);
	::CopyMemory((LPVOID)&(*vData)[0],GetMemory(),nSize);
	return 0;
}

LRESULT		CFile::Save(const string& filename,vector<BYTE>*vData){
	return Write(filename,(LPVOID)&(*vData)[0],vData->size());
}

LRESULT		CFile::Save(const string& filename,vector<string>*vData){
	//	これ、fopenして書き込んだほうが、てっとり早いんかな？
	const int nSize = vData->size();
	FILE *fp = fopen(filename.c_str(),"w");
	if (fp==NULL) return 1;

	for(int i=0;i<nSize;i++){
		if (fprintf(fp,(*vData)[i].c_str())<0) return 2;
		//	書き込みしてる最中にHDD溢れたんちゃうんけ？
	}
	fclose(fp);
	return 0;
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
