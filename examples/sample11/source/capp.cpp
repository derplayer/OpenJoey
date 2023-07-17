#include "stdafx.h"
#include "md5.h"

//	言わずと知れたWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	必ず書いてね

	unsigned char* p = (unsigned char*)"message digest";
	MD5 md5(p);
//	md5.finalize();
	char * psz = md5.hex_digest();
	//	↑これで、HEX文字列(32バイト+'\0')が取得できる
	CDbg().Out("\""+string((char*)p)+"\"のMD5は"+psz);
	delete [] psz;	//	これを忘れるとメモリリークする

	return 0;
}
