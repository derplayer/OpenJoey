/**
	�s�@Plug In DLL �쐬���@�@�t

	���̃t�@�C���́AyaneuraoGameSDK3rd�pPlug In DLL����邽�߂�
	�w�b�_�t�@�C��

	DLL���̃v���W�F�N�g�ŁA���̃t�@�C����include���Ďg�����ƁI

	�P�D���̃w�b�_��include����
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

#ifndef __ObjectCreater_h__
#define __ObjectCreater_h__

/////////////////////////////////////////////////////////////////////////////
#ifndef __YTLfactory__
#define __YTLfactory__

class factory_base {
/**
	factory �̊��N���X
*/
public:
	virtual void* Create()=0;
	virtual ~factory_base(){}
};

template <class T>
class factory : public factory_base {
/**
	factory
*/
public:
	virtual T* CreateInstance() { return new T; }
	virtual void* Create() { return new T; }
};

template <class T,class S>
class factory2 : public factory<T> {
///	T�͊��N���X�BS�͂��̔h���N���X�B
	virtual T* CreateInstance() { return new S; }
	virtual void* Create() { return new S; }
};

#endif

/////////////////////////////////////////////////////////////////////////////

#include "string.h"	//	YTL/string
using YTL::string;

class IObjectCreater {
public:
	virtual void* Create(const string& strClassName)=0;
	virtual LRESULT LoadPlugIn(const string& strFileName)=0;
	virtual LRESULT ReleasePlugIn(const string& strFileName)=0;

	//	DLL�����I�u�W�F�N�g��o�^����̂Ɏg��
	virtual void RegistClass(const string& strClassName,factory_base*)=0;

	//	Main����new��delete
	virtual void* New (size_t t)=0;
	virtual void  Delete(void* p)=0;

	static IObjectCreater* GetObj() { return m_p; }
	static void SetObj(IObjectCreater*p) { m_p = p; }

	virtual ~IObjectCreater()=0;
protected:
	static IObjectCreater* m_p;
};

//	���̊֐����ADLL PlugIn�Ƃ��āA�R�[���o�b�N���Ă��炦��
extern "C"	__declspec( dllexport ) void _cdecl YaneDllInitializer(void*);

#endif
