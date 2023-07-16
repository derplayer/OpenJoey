// yaneDir.h :
//
//		directory file検索 wrapper
//
//			programmed by yaneurao(M.Isozaki) '99/10/19 - '99/11/28
//

#ifndef __yaneDir_h__
#define __yaneDir_h__


namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class CDir {
public:
	//	Directory
	void	SetFindFile(const string& file);					//	検索ファイル名設定
	void	SetPath(const string& file);						//	検索パス設定
	void	EnableSubdir(bool b) { m_bSubDir = b; }				//	検索のときにサブフォルダを有効にするのか
	void	EnablePackFile(bool b) { m_bPackFile = b; }			//	yanePackファイルも検索対象にするのか？

	LRESULT	FindFile(string& file);								//	ファイルを取得(返し値非０ならばNot Found)
	static bool IsMatch(LPCSTR it1,LPCSTR it2);					//	二つの文字列が一致するか？(wildcard使用可能)
	void	Reset() { CloseFindFile(); }

	void	EnableDirListup(bool b) { m_bAllFile = b; }			//	フォルダもマッチング対象にする
	bool	IsDir() const { return m_bDir; }				//	FindFileで取得したのはフォルダか？

	bool	IsFileExist(const string& file);					//	このファイルは存在するのか？

	//	Drives
	DWORD	CheckDrive() { return m_dwDrive=::GetLogicalDrives(); }
	bool	IsExistDrive(int n) const { return (m_dwDrive & (1L << n))!=0; }

	CDir();
	virtual ~CDir();

protected:
	void	CloseFindFile();								//	終了処理
	string	m_file;			//	検索ファイル
	string	m_path;			//	path
	HANDLE	m_handle;		//	検索ハンドル
	bool	m_bSubDir;		//	subdir有効か？	
	bool	m_bPackFile;	//	packfile有効か？	
	CDir*	m_lpDir;		//	subdir検索用(再帰的に利用)

	//	フォルダのリストアップ用
	bool	m_bAllFile;		//	フォルダも検索対象にするのか？
	bool	m_bDir;			//	いま取得したのはフォルダか？

	//	圧縮ファイル用
	HANDLE	m_hPackFile;	//	圧縮ファイル検索用ハンドル
	DWORD	m_dwFileNum;	//	圧縮ファイル内ファイル数
	int		m_nPackFile;	//	圧縮ファイルは 0:yanePack 1:yanePackEx 2:yanePackDx
	string	m_PackName;		//	検索中の圧縮ファイル名
protected:
	DWORD	m_dwDrive;
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
