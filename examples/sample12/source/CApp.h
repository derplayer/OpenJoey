
#ifndef __CApp_h__
#define __CApp_h__

class CApp : public CAppFrame {
public:

protected:
	//	���������C����
	void MainThread(void);		  //  ���ꂪ���s�����

	void	Listing();	//	���X�g�A�b�v
	void	Compare();	//	��r

	LRESULT	CalcMD5(const string& filename, DWORD& dwSize, string& result);
};

#endif
