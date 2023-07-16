#include "stdafx.h"
#include "yaneDir.h"

//////////////////////////////////////////////////////////////////////////////
//	<yanepack用構造体について>
//		ファイルヘッダー "yanepack" 8バイト + 格納ファイル数 DWORD
//		その後、以下のCFileInfoが格納ファイル数だけ来て、あとはデータ

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class CFileInfo {
public:
	char	filename[32];
	DWORD	startpos;	//	seek pos
	size_t	filesize;	//	file size
};
struct CFileInfoEx {
	CHAR	filename[32];
	DWORD	startpos;	//	seek pos
	size_t	filesize;	//	file size(圧縮サイズ)
	size_t	arcsize;	//	arc size (展開後のファイルサイズ)
};
struct CFileInfoDx {
	CHAR	filename[256];	//	階層化ファイルなので、サイズでかしとこ
	DWORD	startpos;	//	seek pos
	size_t	filesize;	//	file size(圧縮サイズ)
	size_t	arcsize;	//	arc size (展開後のファイルサイズ)
};
//////////////////////////////////////////////////////////////////////////////

CDir::CDir() {
	m_handle	=	INVALID_HANDLE_VALUE;
	m_hPackFile =	INVALID_HANDLE_VALUE;
	m_lpDir		=	NULL;
	m_file		=	"*.*";

	m_bSubDir	= true;
	m_bPackFile	= true;
	m_bAllFile	= false;
}

CDir::~CDir()	{ CloseFindFile(); }

//////////////////////////////////////////////////////////////////////////////

void CDir::SetFindFile(const string& file){
	CloseFindFile();	//	初期化を兼ねる
	m_file = file;
}

void CDir::SetPath(const string& file){
	m_path = file;
	if (m_path.empty()) return ;
	string::iterator p = m_path.end()-1;
	if (*p=='/' || *p=='\\') m_path.erase(p);
}

void CDir::CloseFindFile() {
	if(m_handle!=INVALID_HANDLE_VALUE) {
		::FindClose(m_handle);
		m_handle=INVALID_HANDLE_VALUE;
	}
	if(m_hPackFile!=INVALID_HANDLE_VALUE) {
		::CloseHandle(m_hPackFile);
		m_hPackFile=INVALID_HANDLE_VALUE;
	}
	DELETE_SAFE(m_lpDir);
}

//////////////////////////////////////////////////////////////////////////////

bool	CDir::IsFileExist(const string& file){		//	このファイルは存在するのか？
	HANDLE h;
	h  = ::CreateFile(file.c_str(),
					GENERIC_READ,		// Read
					FILE_SHARE_READ,	// ReadOpenなら共有を許すのはマナー 
					NULL,				// security
					OPEN_EXISTING,		// 存在してなければエラー
					FILE_ATTRIBUTE_NORMAL,	//	ファイル属性
					NULL				// テンプレートファイル
					);
	if (h==INVALID_HANDLE_VALUE) return false;
	::CloseHandle(h);
	return true;
}

//////////////////////////////////////////////////////////////////////////////
LRESULT CDir::FindFile(string &file){
	int result;
	WIN32_FIND_DATA FindFileData;
	DWORD NumberOfBytesRead;		// dummy

retry:;
	//	sub folderが有効ならば再帰的に検索する
	if (m_lpDir!=NULL) {
		if (m_lpDir->FindFile(file)==0) {
			return 0;
		}
		DELETE_SAFE(m_lpDir);
	}
	if (m_hPackFile!=INVALID_HANDLE_VALUE) {
		while (m_dwFileNum--!=0){
			switch(m_nPackFile) {
			case 0: {
				CFileInfo info;
				if (!::ReadFile(m_hPackFile,&info,sizeof(info),&NumberOfBytesRead,NULL)){
					::CloseHandle(m_hPackFile);
					m_hPackFile=INVALID_HANDLE_VALUE;
					goto retry;
				}
				file = info.filename;	//	小文字化してからテストする
					} break;
			case 1: {
				CFileInfoEx info;
				if (!::ReadFile(m_hPackFile,&info,sizeof(info),&NumberOfBytesRead,NULL)){
					::CloseHandle(m_hPackFile);
					m_hPackFile=INVALID_HANDLE_VALUE;
					goto retry;
				}
				file = info.filename;	//	小文字化してからテストする
					} break;
			case 2: {
				CFileInfoDx info;
				if (!::ReadFile(m_hPackFile,&info,sizeof(info),&NumberOfBytesRead,NULL)){
					::CloseHandle(m_hPackFile);
					m_hPackFile=INVALID_HANDLE_VALUE;
					goto retry;
				}
				file = info.filename;	//	小文字化してからテストする
					} break;
			}
			if (IsMatch(file.c_str(),m_file.c_str())) {
				//	一致した！
				file = m_PackName + file;
				//	圧縮ファイル以外のところに存在すれば、それは却下:p
				//	（重複してリストアップしてしまうため）
				if (!IsFileExist(file)) return 0;
			}
		}
		::CloseHandle(m_hPackFile);
		m_hPackFile=INVALID_HANDLE_VALUE;
	}

	//	新規検索なのか？
	if (m_handle==INVALID_HANDLE_VALUE) {
		string fullname;
		fullname = m_path+"\\*.*";
		m_handle = ::FindFirstFile(fullname.c_str(),&FindFileData);
		if (m_handle == INVALID_HANDLE_VALUE) result=1; else result=0;
	} else {
		if (::FindNextFile(m_handle,&FindFileData)!=0) result=0; else result=1;
	}

	if (result==0) {
		file = FindFileData.cFileName;
		if (!m_bAllFile && (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			if (file == "." || file == "..") goto retry; // こんなん拾うなよ...
			if (m_bSubDir) {	// subフォルダは検索対象か？
				m_lpDir = new CDir;
				m_lpDir->EnableSubdir(m_bSubDir);		// must be true
				m_lpDir->EnablePackFile(m_bPackFile);
				m_lpDir->SetFindFile(m_file);
				m_lpDir->SetPath(m_path+"\\"+file);
			}
			goto retry;
		} else {
			//	圧縮ファイル検索用（めんどくさいなぁ…）
				if (m_nPackFile>=1 && m_bSubDir && m_bPackFile && IsMatch(file.c_str(),"*.dat")){
					//	圧縮ファイルも検索対象なんか？
				string fullname;
				fullname = m_path+"\\"+file;
				m_hPackFile = ::CreateFile(fullname.c_str(),
				GENERIC_READ,		// Read
				FILE_SHARE_READ,	// ReadOpenなら共有を許すのはマナー 
				NULL,				// security
				OPEN_EXISTING,		// 存在してなければエラー
				FILE_ATTRIBUTE_NORMAL,	//	ファイル属性
				NULL				// テンプレートファイル
				);
			}
			if (m_hPackFile == INVALID_HANDLE_VALUE) goto check;
			char ident[9];
			ident[8] = '\0';
			if (!::ReadFile(m_hPackFile,ident,8,&NumberOfBytesRead,NULL)){
				::CloseHandle(m_hPackFile);
				m_hPackFile = INVALID_HANDLE_VALUE;
				goto check;
			}
			if (!strcmp(ident,"yanepack")){
				m_nPackFile = 0;
			} else if (!strcmp(ident,"yanepkEx")){
				m_nPackFile = 1;
			} else if (!strcmp(ident,"yanepkDx")){
				m_nPackFile = 2;
			} else {
				::CloseHandle(m_hPackFile);
				m_hPackFile = INVALID_HANDLE_VALUE;
				goto check;
			}
			if (!::ReadFile(m_hPackFile,&m_dwFileNum,sizeof(m_dwFileNum),&NumberOfBytesRead,NULL)){
				::CloseHandle(m_hPackFile);
				m_hPackFile = INVALID_HANDLE_VALUE;
				goto check;
			}
			//	.datを切り落としてフォルダ名に変換
			string::size_type pos1;
			pos1 = file.rfind('.');
			file.erase(pos1);
			m_PackName = m_path + "\\" + file + "\\";
			goto retry;	//	圧縮ファイル内を探す

check:;
			//	一致した！？
			if (IsMatch(file.c_str(),m_file.c_str())) {
				//	pack fileならば外す必要あり...!?
				file = m_path + "\\" + file;	//	フルパスで返す
				m_bDir = (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) !=0;
				result = 0;
			} else {
				goto retry;
			}

		}
	} else {
		CloseFindFile();
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////////

//	Check two strings match,using wildcard('*'or'?'). 
//	This algorithm is examining recursively.(C) yaneurao 1999
bool CDir::IsMatch(LPCSTR it1,LPCSTR it2){
//	it1がit2とマッチするか
//	it1には、'*'や'.'が使用できる

	//	漢字まじりは勘弁してちょ
	while (true){
		if (*it1 == '*') {
			it1++;
			do {
				if (IsMatch(it1,it2)) return true;	//	一回でも一致すればtrue
			} while (*(it2++)!='\0'); 
			return false;	//	すべて一致しないならばfalse
		}
		if (*it2 == '*') {
			it2++;
			do {
				if (IsMatch(it1,it2)) return true;	//	一回でも一致すればtrue
			} while (*(it1++)!='\0'); 
			return false;	//	すべて一致しないならばfalse
		}
		if ((toupper(*it1) != toupper(*it2)) && (*it1 != '?') && (*it2 != '?')) return false;		// ダメ
		if (*it1=='\0' && *it2=='\0') return true;	//	一致した
		if (*it1=='\0' || *it2=='\0') return false;
		it1 ++;
		it2 ++;
	}
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

//////////////////////////////////////////////////////////////////////////////
