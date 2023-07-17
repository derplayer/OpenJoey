
#ifndef __CApp_h__
#define __CApp_h__

class CApp : public CAppFrame {
public:

protected:
	//	こいつがメインね
	void MainThread(void);		  //  これが実行される

	void	Listing();	//	リストアップ
	void	Compare();	//	比較

	LRESULT	CalcMD5(const string& filename, DWORD& dwSize, string& result);
};

#endif
