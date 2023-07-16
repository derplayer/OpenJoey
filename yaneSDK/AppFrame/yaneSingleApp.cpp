#include "stdafx.h"
#include "yaneSingleApp.h"
#include "yaneFindWindow.h"
#include "../Auxiliary/yaneShell.h"

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

bool	CSingleApp::IsValid(const string& strMutexName){

	string sName = strMutexName;
	if (sName.empty()){
		sName = CShell().getModuleMutexName();
	}

	if (m_oMutex.Open(sName)) {	// あかんやん...
		//	この起動ファイルと同名の実行ファイルを探す．．．
		//	しかし、WindowClass名が無ければ一意に特定できないようだ...
		CFindWindow fw;
		fw.Find(CShell().getModuleName());
		return false;
	} else {
		return true;
	}
}

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd
