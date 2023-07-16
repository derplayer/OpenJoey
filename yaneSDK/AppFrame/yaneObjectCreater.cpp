#include "stdafx.h"
#include "yaneObjectCreater.h"
#include "../Auxiliary/yaneFile.h"

//////////////////////////////////////////////////////////////////////////////

#if !defined(NOT_USE_DEFAULT_DLLMAIN) && defined(COMPILE_YANE_PLUGIN_DLL)
namespace {
CAppInitializer* g_pAppInit;
}

BOOL APIENTRY DllMain( HANDLE hModule,  // HINSTANCE�ł���
					   DWORD  ul_reason_for_call, 
					   LPVOID lpReserved
					 )
{
	//	�Q�DDllMain�ł́ACAppInitializer�ŏ��������郈���V�I

	switch (ul_reason_for_call) {
		case DLL_THREAD_ATTACH :
			   {
				g_pAppInit = new CAppInitializer(hModule,ul_reason_for_call,lpReserved);
				//	�������������K�v�Ȃ炱���ɏ����Ȃ胈
				//	���ӎ���
				//	���D���̎��_�ł͂܂�new/delete�͎g���Ȃ�(�ȉ���5.�̊֐��ōs�Ȃ�)
				//	���DErr�f�o�C�X�͖�������(�K�v�Ȃ�΁A�ȉ���5.�̊֐��ōs�Ȃ�)
			   }
			break;
		case DLL_THREAD_DETACH :
			   {
				if (g_pAppInit!=NULL) { delete g_pAppInit; g_pAppInit=NULL; }
			    //	�I�������͂����ɏ����ă`��
				//	���Ƃ��΁A5.�̊֐�����new����static�ȃ|�C���^
				//	��delete���鏈���Ƃ��B
			   }
		   break;
	};
	return TRUE;
}
#endif

#ifdef COMPILE_YANE_PLUGIN_DLL

#include "../AppFrame/yaneAppInitializer.h"
#include "../Auxiliary/yaneStream.h"
//	�����́A�����̏�����������yaneSDK3rd���C�u�����S�̂ɑ΂��鏉�������K�v�Ȃ̂�
//	include���Ă����ł��BCObjectCreater�������Ԃ����ʂ��Ƃ��́A�����Ă��������B

//	�v���g�^�C�v�錾�O�O�G
void	YaneRegistPlugIn(IObjectCreater*);
//	���̊֐����ADLL PlugIn�Ƃ��āA�R�[���o�b�N���Ă��炤
extern "C"	__declspec( dllexport ) void _cdecl YaneDllInitializer(void*p){
	IObjectCreater* pObjectCreater = static_cast<IObjectCreater*>(p);
	IObjectCreater::SetObj(pObjectCreater);	//	������o�^

	//	yaneSDK3rd�̂Ȃ��ŏ�����(static�ȕϐ�)���K�v�Ȃ��̂͂����ŏ���������
	//	DLL���ł́A����SetObj�����܂�new/delete���g���Ȃ��̂Ŏd���Ȃ��̂ł�
	//	CObjectCreater�̃\�[�X�������Ԃ����ʂ��Ƃ��͍폜���Ă���͂�
	///////////////////////////////////////////////////////////////////
	CFile::SetCurrentDir();	//	CFile��path������
#ifdef USE_ErrorLog		//	CErrorLog�N���X��L���ɂ���B
	//	�G���[�o�̓f�o�C�X���ЂƂp��
	Err.SelectDevice(smart_ptr<ITextOutputStream>(new CTextOutputStreamNullDevice));
#endif
	///////////////////////////////////////////////////////////////////

	//	user��`��
	YaneRegistPlugIn(pObjectCreater);
}

//	new��delete�́AMain���̂��̂��Ă�
void* operator new (size_t t){
	IObjectCreater* pp = IObjectCreater::GetObj();
	if (pp!=NULL){
		return pp->New(t);
	} else {
		WARNING(true,"operator new��IObjectCreater������������O�ɌĂяo����Ă���");
		return (void*)malloc(t);	//	�Ƃ��Ă�Ͻް(߄D�)
	}
}

void operator delete(void*p){
	IObjectCreater* pp = IObjectCreater::GetObj();
	if (pp!=NULL){
		pp->Delete(p);
	} else {
		WARNING(true,"operator delete��IObjectCreater���I����ɌĂяo����Ă���");
		::free(p);					//	�Ƃ��Ă�Ͻް(߄D�)
	}
}
#endif

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

//	DLL��?
#ifdef COMPILE_YANE_PLUGIN_DLL
//	static member..
IObjectCreater* IObjectCreater::m_p;
#endif

CDllPlugInLoader::CDllPlugInLoader(){
	m_hDll = NULL;
}
CDllPlugInLoader::~CDllPlugInLoader(){
	Release();
}

LRESULT CDllPlugInLoader::Load(const string& szDllName){
	m_strFileName = szDllName;
	// DLL��ǂݍ���
	m_hDll = ::LoadLibrary(m_strFileName.c_str());
	if(m_hDll==NULL){
		Err.Out("CDllPlugInLoader::Load \"%s\"�̃��[�h�Ɏ��s", m_strFileName.c_str());
		return 1;
	}
	return 0;
}

LRESULT CDllPlugInLoader::Release(){
	LRESULT lr=0;
	if(m_hDll!=NULL){
		lr = !::FreeLibrary(m_hDll);
		m_hDll = NULL;
	}
	return lr;
}

//////////////////////////////////////////////////////////////////////////////

//	static member..
singleton<CObjectCreater> CObjectCreater::m_vObj;

CObjectCreater::CObjectCreater(): m_pPlugInInfo(0){}

void* CObjectCreater::Create(LPCSTR strClassName){
	void* pObj=NULL;
	CCriticalLock cl(GetCriticalSection());
	{
		smart_vector_ptr<CObjectInfo>& info = *GetObjectInfo();
		smart_vector_ptr<CObjectInfo>::iterator it = info.begin();
		while (it!=info.end()){
			if ((*it)->strClassName==strClassName){
			//	������[�I�I
			//	������factory�Ăяo���ăI�u�W�F�N�g����ĕԂ�����ŁI�I
				//if (it->factory_base!=NULL){
				//	������NULL�ł��邱�Ƃ͂��肦�Ȃ�
					pObj = (*it)->factory->Create();
					break;
				//}
			}
			++it;
		}
	}
	return pObj;
}

LRESULT CObjectCreater::LoadPlugIn(LPCSTR strFileName){
	LRESULT lr=0;
	const string str(CFile::MakeFullName(strFileName));
	//	����AMakeFullName���AFindFile�̂ق����ǂ��̂���...

	CCriticalLock cl(GetCriticalSection());
		//	������plug in��T��
		smart_vector_ptr<CPlugInInfo>::iterator	it = FindPlugIn(str);
		if (it == GetPlugInInfo()->end()) {
			//	�ǂݍ���ŁA���̃N���X�i��factory�j��o�^����
			m_pPlugInInfo = new CPlugInInfo;
			//	���̕ϐ��́ADLL������Q�Ƃ��ꂤ��
			if (m_pPlugInInfo->vPlugIn.Load(str)==0){	
			//	plug-in�ǂݍ��݂ɐ���
				HINSTANCE hDll = m_pPlugInInfo->vPlugIn.GetInstance();
				typedef void (_cdecl *Func)(void*);
				Func YaneDllInitializer = (Func)::GetProcAddress(hDll, "YaneDllInitializer");
				if(YaneDllInitializer==NULL){
					//	���������s�S��
					DELETE_SAFE(m_pPlugInInfo);
				} else {
					//	���L���̈ړ�
					YaneDllInitializer(static_cast<void*>(this));
					GetPlugInInfo()->insert(m_pPlugInInfo);
					m_pPlugInInfo = NULL;	//	delete���Ă͂�����
				}
			} else {
				DELETE_SAFE(m_pPlugInInfo);
			}
		} else {
			//	���ɓǂݍ���ł���̂ŁA���̎Q�ƃJ�E���g�݂̂����Z����
			(*it)->nRef++;
		}
	return lr;
}

LRESULT CObjectCreater::ReleasePlugIn(LPCSTR strFileName){
	LRESULT lr=0;

	const string str(CFile::MakeFullName(strFileName));
	//	����AMakeFullName���AFindFile�̂ق����ǂ��̂���...

	CCriticalLock cl(GetCriticalSection());
		//	������plug in��T��
		smart_vector_ptr<CPlugInInfo>::iterator	it = FindPlugIn(str);
		if (it == GetPlugInInfo()->end()) {
			//	�����Ƃ́A�ǂ�������������H
			Err.Out("CObjectCreater::ReleasePlugIn �o�^����Ă��Ȃ�PlugIn�̍폜");
		} else {
			//	�Q�ƃJ�E���g�����Z����
			if (--(*it)->nRef == 0){
				//	0�ɂȂ����̂ŁA���̃I�u�W�F�N�g��delete����
				CPlugInInfo* pInfo = (*it).get();
				//	����pInfo��p���Ă���I�u�W�F�N�g��
				//	factory��o�^�������Ȃ���΂Ȃ�Ȃ�
				smart_vector_ptr<CObjectInfo>::iterator it2	= GetObjectInfo()->begin();
				while (it2!=GetObjectInfo()->end()){
					if ((*it2)->pPlugIn == pInfo){
						it2 = GetObjectInfo()->erase(it2);
					} else {
						++it2;
					}
				}
				GetPlugInInfo()->erase(it);	//	smart_ptr�Ȃ̂ŁA����ŏ�����
			}
		}
	return lr;
}

smart_vector_ptr<CObjectCreater::CPlugInInfo>::iterator
	CObjectCreater::FindPlugIn(const string& strPlugInName){
/*
	���̊֐��́A
		LRESULT CObjectCreater::LoadPlugIn �� ReleasePlugIn
	����Ăяo�����̂ŁACriticalSection���ł���Ɖ��肵�ėǂ�
*/
	smart_vector_ptr<CPlugInInfo>::iterator	it = GetPlugInInfo()->begin();
	smart_vector_ptr<CPlugInInfo>::iterator it_end	= GetPlugInInfo()->end();
	while (it!=it_end){
		if (strPlugInName == (*it)->vPlugIn.GetFileName())
			break;
		++it;
	}
	return it;
}

void CObjectCreater::RegistClass(LPCSTR strClassName,factory_base*f){
	CObjectInfo* pInfo = new CObjectInfo;
	pInfo->pPlugIn		= m_pPlugInInfo;
	pInfo->strClassName = strClassName;
	pInfo->factory		= f;
	GetObjectInfo()->insert(pInfo);
}


void* CObjectCreater::New(size_t t){
	return new BYTE[t];
}

void  CObjectCreater::Delete(void* p){
	delete [] p;
}

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd
