#include "stdafx.h"
#include "md5.h"

//	���킸�ƒm�ꂽWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	�K�������Ă�

	unsigned char* p = (unsigned char*)"message digest";
	MD5 md5(p);
//	md5.finalize();
	char * psz = md5.hex_digest();
	//	������ŁAHEX������(32�o�C�g+'\0')���擾�ł���
	CDbg().Out("\""+string((char*)p)+"\"��MD5��"+psz);
	delete [] psz;	//	�����Y���ƃ��������[�N����

	return 0;
}
