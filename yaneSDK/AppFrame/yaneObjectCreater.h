//
//	yaneObjectCreater.h :
//		ObjcetManager(CoCreateInstance�I�Ȃ���)
//

#ifndef __ObjectCreater_h__
#define __ObjectCreater_h__

//////////////////////////////////////////////////////////////////////////////

#ifdef COMPILE_YANE_PLUGIN_DLL
//	���̊֐����ADLL PlugIn�Ƃ��āA�R�[���o�b�N���Ă��炦��
extern "C"	__declspec( dllexport ) void _cdecl YaneDllInitializer(void*);
#endif

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

/**
	MAIN����DLL����string�̎����ƈقȂ�Ƃ��قȂ�Ƃ܂����B
	yaneSDK�̏ꍇstring.h�͎��O�Ŏ����Ă���̂Ŗ��Ȃ��͂��Ȃ̂����A
	������g�������͂Ȃ��Ƃ������Ƃ����邾�낤�B
	�����ŁACObjectCreater::Create,RegistClass�̈�����
	string���g��Ȃ������o���p�ӂ��āAstring���g���o�[�W�����͂��ׂ�
	������ɈϏ����邱�Ƃɂ����B�����(CObjectCreater�Ɋւ��Ă�)
	string�̃o�[�W�����̈Ⴂ�ɔY�܂���邱�Ƃ��Ȃ��Ȃ�B
	�iexport���Ă���N���X�̃����o�̈�������string���g��Ȃ��Ȃ�΁j
*/

class CDllPlugInLoader {
/**
	class CObjectCreater �Ŏg�p�����
	PlugIn DLL��ǂݍ��ށBPlug In Dll�ɂ��ẮA
	Plug In Dll�쐬�K�C�h�ɂ��Ă������������B
*/
public:
	virtual LRESULT Load(const string& strDllName);
	///	Plug In DLL ��ǂݍ���

	virtual LRESULT Release();
	///	Plug In DLL ���������

	virtual string	GetFileName() const { return m_strFileName; }
	///	�ǂݍ���ł���Plug In DLL�̖��O��Ԃ��i�t���p�X�Łj

	virtual HINSTANCE GetInstance() const { return m_hDll; }

	CDllPlugInLoader();
	virtual ~CDllPlugInLoader();

private:
	HINSTANCE m_hDll;
	string m_strFileName;
};

//////////////////////////////////////////////////////////////////////////////

class IObjectCreater {
/**
	�s�@Plug In DLL �쐬���@�@�t
	yaneSDK3rd�̊e�N���X��p����DLL���쐬���A�����Main������
	�Ăяo�����߂ɂ�

	�P�Dconfig/yaneConfig.h��
	//#define COMPILE_YANE_PLUGIN_DLL	//	plug-in DLL���쐬����̂��H
	��L���ɂ���

	�Q�Dmain���Ɍ��J�������I�u�W�F�N�g�i��factory�j��
		�ȉ��̂悤��YaneRegistPlugIn�֐�������ēo�^����

		void	YaneRegistPlugIn(IObjectCreater*p){
			p->RegistClass("CHoge",new factory<CHoge>);
			p->RegistClass("CHoee",new factory<CHoee>);
						//	�����̃N���X���́A�����܂ŗ�
		}

	�R�D�I�u�W�F�N�g�𖼑O�Ő�������ɂ�IObjectCreater���g��
		�i�g�������́AyaneSDK3rd/AppFrame/yaneObjectCreater�Ɠ����j

		��F
			//	�g���O��Load
			IObjectCreater::GetObj()->LoadPlugIn("enraSound.dll");
			IVorbis* p = (IVorbis*)CObjectCreater::GetObj()->Create("CVorbis");
			if (p!=NULL){
				p->Play();
				delete p;
			}
			//	�g���I�������A���
			IObjectCreater::GetObj()->ReleasePlugIn("enraSound.dll");
*/
public:
	virtual void* Create(LPCSTR strClassName)=0;
	virtual void* Create(const string& strClassName)=0;

	///	�I�u�W�F�N�g��o�^����̂Ɏg��(main,dll����)
	virtual void RegistClass(LPCSTR strClassName,factory_base*)=0;
	virtual void RegistClass(const string& strClassName,factory_base*p)=0;

	virtual LRESULT LoadPlugIn(LPCSTR strFileName)=0;
	virtual LRESULT LoadPlugIn(const string& strFileName)=0;

	virtual LRESULT ReleasePlugIn(LPCSTR strFileName)=0;
	virtual LRESULT ReleasePlugIn(const string& strFileName)=0;

	//	main����new,delete��DLL������Ăяo��
	virtual void* New (size_t t)=0;
	virtual void  Delete(void* p)=0;
	virtual ~IObjectCreater(){}

//	DLL���쐬����Ƃ��̂�
#ifdef COMPILE_YANE_PLUGIN_DLL
	static IObjectCreater* GetObj() { return m_p; }
	static void SetObj(IObjectCreater*p) { m_p = p; }
protected:
	static IObjectCreater* m_p;

#endif
};

class CObjectCreater : public IObjectCreater {
/**
	DLL���̃I�u�W�F�N�g�̐������s�Ȃ��悤�ȊǗ��N���X
	DLL���ŁA����DLL�ɂ���A�I�u�W�F�N�g�𐶐���������o����

	�g�������F
			//	�g���O��Load
			CObjectCreater::GetObj()->LoadPlugIn("enraSound.dll");
			IVorbis* p = (IVorbis*)CObjectCreater::GetObj()->Create("CVorbis");
			if (p!=NULL){
				p->Play();
				delete p;
			}
			//	�g���I�������A���
			CObjectCreater::GetObj()->ReleasePlugIn("enraSound.dll");

		main���ł��낤��dll���ł��낤���A��̕��@�œo�^����Ă���
		�̂ł���΁A�ǂ��ɂ���I�u�W�F�N�g�ł����Ă������ł���I
		���Ȃ킿�Adll������A����dll�œo�^����Ă���I�u�W�F�N�g�ł�
		�Ăяo�����Ƃ��o����I

*/
public:
	static CObjectCreater* GetObj() { return m_vObj.get(); }

	virtual void* Create(LPCSTR strClassName);
	virtual void* Create(const string& strClassName)
		{ return Create(strClassName.c_str()); }
	/**
		����CObjectCreater�ɓo�^����Ă���I�u�W�F�N�g��
		���̖��O�Ő�������

		���̊֐��Ő������ꂽ�I�u�W�F�N�g��delete��delete���ėǂ��B
		�i�����œǂݍ��܂�Ă���Dll�́Aclass CDllPlugInLoader
		�ɏ�����Ă���v���𖞂�������j

		�����o�^����Ă��Ȃ����NULL���Ԃ�

		���@���̃N���X���g���ꍇ�Astring��YTL::string��p���āA
			string�̎����͕s�ςł�����̂Ƃ���B
	*/

	///	PlugInDll��ǂݍ��ށi�Q�ƃJ�E���g�������Ă���j
	virtual LRESULT LoadPlugIn(LPCSTR strFileName);
	virtual LRESULT LoadPlugIn(const string& strFileName)
		{ return LoadPlugIn(strFileName.c_str()); }
	/**
		���̊֐��ŁAPlug In Dll��ǂݍ��ނƁA�����I��
		�N���X�i��factory�j���A�o�^�����B
	*/

	///	PlugInDll���������i�Q�ƃJ�E���g�������Ă���j
	virtual LRESULT ReleasePlugIn(LPCSTR strFileName);
	virtual LRESULT ReleasePlugIn(const string& strFileName)
		{ return ReleasePlugIn(strFileName.c_str()); }
	/**
		���̊֐��ŁAPlug In Dll���������ƁA
		�N���X�i��factory�j�̓o�^�����������
	*/

	virtual void RegistClass(LPCSTR strClassName,factory_base*);
	virtual void RegistClass(const string& strClassName,factory_base*p)
		{ RegistClass(strClassName.c_str(),p); }
	/**
		DLL�����A�I�u�W�F�N�g��o�^����̂Ɏg���B
		main���ł́A�g���K�v�͖����B�i�g���Ă��ǂ��j
	*/

	///	Main����new��delete(DLL������́A������Ăяo���Ă��炤)
	virtual void* New (size_t t);
	virtual void  Delete(void* p);

	CObjectCreater();
protected:
	class CPlugInInfo {
	///	�ǂݍ���ł���Plug In���Ǘ����邽�߂̍\����
	public:
		CDllPlugInLoader	vPlugIn;	//	�v���O�C��
		int					nRef;		//	�Q�ƃJ�E���g
		CPlugInInfo(){ nRef = 1; }
	};
	smart_vector_ptr<CPlugInInfo> m_apPlugIn;

	class CObjectInfo {
	///	�o�^����Ă���Plug-In�ƃI�u�W�F�N�g���Ǘ����邽�߂̍\����
	public:
		CPlugInInfo*	pPlugIn;		//	�v���O�C��
		string			strClassName;	//	�N���X��
		factory_base*	factory;		//	factory
		CObjectInfo():pPlugIn(0),factory(0) {}
		~CObjectInfo() { DELETE_SAFE(factory); }
	};
	smart_vector_ptr<CObjectInfo> m_apObjectInfo;

	smart_vector_ptr<CPlugInInfo>* GetPlugInInfo()
		{ return& m_apPlugIn; }
	smart_vector_ptr<CObjectInfo>* GetObjectInfo()
		{ return& m_apObjectInfo; }

	//	���̃|�C���^�������邽�߂̃N���e�B�J���Z�N�V����
	CCriticalSection m_cr;
	CCriticalSection* GetCriticalSection() { return& m_cr; }

	smart_vector_ptr<CPlugInInfo>::iterator
		FindPlugIn(const string& strPlugInName);
	//	�����̃v���O�C�����ǂݍ��܂�Ă���Ȃ�΁A�����ւ̃|�C���^��Ԃ�

	//	DLL��������������Ƃ��Ɉꎞ�I�ɓn��
	CPlugInInfo* m_pPlugInInfo;

	static	singleton<CObjectCreater>	m_vObj;
};

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

//////////////////////////////////////////////////////////////////////////////

#endif
