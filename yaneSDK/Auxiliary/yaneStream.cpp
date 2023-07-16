#include "stdafx.h"
#include "yaneStream.h"
#include "yaneFile.h"

////////////////////////////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

void	CTextOutputStreamFile::Clear() {
	if (!m_strFileName.empty()){
		::DeleteFile(m_strFileName.c_str());
	}
}

void __cdecl CTextOutputStreamFile::Out(LPCSTR fmt, ... ) {
	if (m_strFileName.empty()) return ;
	CHAR	buf[512];
//	::wvsprintf(buf,fmt,(LPCSTR)(&fmt+1) );

//	int n = ::lstrlen(buf) - 1;
	const int n = ::wvsprintf(buf, fmt, (va_list)(&fmt+1)) - 1;
	if (buf[n] == '\n') buf[n] = '\0'; // ���s��ׂ�

	const string filename(CFile::MakeFullName(m_strFileName));
	FILE* file = fopen(filename.c_str(),"aw");
	if (file!=NULL){
		fprintf(file,"%s\n",buf);	//	���s�����ǋL
		fclose(file);
	}
}	///	�������\������

void	ITextOutputStream::Out(const string& str){
	Out((LPCSTR)str.c_str());
}

void	ITextOutputStream::Out(int i){
	CHAR buf[16];
	::wsprintf(buf,"%d",i);
	Out(buf);
};

void __cdecl CTextOutputStreamVersatile::Out(LPCSTR fmt, ... )
//	���ψ����̈Ϗ��͕s���H
{
	CHAR	buf[512];
	::wvsprintf(buf, fmt, (va_list)(&fmt+1));
	m_vDevice->Out(buf);
}

//	�G���[�o�̓f�o�C�X���ЂƂp��
#ifdef USE_ErrorLog		//	CErrorLog�N���X��L���ɂ���B
	//	�G���[�o�̓f�o�C�X���ЂƂp��
	CTextOutputStreamVersatile Err;
#else
	CTextOutputNullDevice Err;
	//	�����Ȃ����Ƃ��΍œK���ŏ�����ł��傤�D�D
#endif

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

////////////////////////////////////////////////////////////////////////////
