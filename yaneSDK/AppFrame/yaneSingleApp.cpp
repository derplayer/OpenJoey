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

	if (m_oMutex.Open(sName)) {	// ��������...
		//	���̋N���t�@�C���Ɠ����̎��s�t�@�C����T���D�D�D
		//	�������AWindowClass����������Έ�ӂɓ���ł��Ȃ��悤��...
		CFindWindow fw;
		fw.Find(CShell().getModuleName());
		return false;
	} else {
		return true;
	}
}

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd
