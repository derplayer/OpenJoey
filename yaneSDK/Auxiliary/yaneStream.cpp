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
	if (buf[n] == '\n') buf[n] = '\0'; // 改行を潰す

	const string filename(CFile::MakeFullName(m_strFileName));
	FILE* file = fopen(filename.c_str(),"aw");
	if (file!=NULL){
		fprintf(file,"%s\n",buf);	//	改行自動追記
		fclose(file);
	}
}	///	文字列を表示する

void	ITextOutputStream::Out(const string& str){
	Out((LPCSTR)str.c_str());
}

void	ITextOutputStream::Out(int i){
	CHAR buf[16];
	::wsprintf(buf,"%d",i);
	Out(buf);
};

void __cdecl CTextOutputStreamVersatile::Out(LPCSTR fmt, ... )
//	↑可変引数の委譲は不可か？
{
	CHAR	buf[512];
	::wvsprintf(buf, fmt, (va_list)(&fmt+1));
	m_vDevice->Out(buf);
}

//	エラー出力デバイスをひとつ用意
#ifdef USE_ErrorLog		//	CErrorLogクラスを有効にする。
	//	エラー出力デバイスをひとつ用意
	CTextOutputStreamVersatile Err;
#else
	CTextOutputNullDevice Err;
	//	↑こないしとけば最適化で消えるでしょう．．
#endif

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

////////////////////////////////////////////////////////////////////////////
