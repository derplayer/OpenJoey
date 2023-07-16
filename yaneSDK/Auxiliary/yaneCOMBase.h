// yaneCOM.h :
//	initialize/terminate COM
//
//	COM���g�p����ŏ��ƍŌ�ŏ������^�I���������K�v�Ȃ̂ŁA
// ����Ȃ��񂪕K�v�ɂȂ��Ă���...
//

#ifndef __yaneCOMBase_h__
#define __yaneCOMBase_h__


//	#include "../YTL/yaneThreadLocal.h"
//	�������stdafx.h�œǂݍ���ł���
#include "../Thread/yaneCriticalSection.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class CCOMBase {
/**
	COM�́A�X���b�h�x�[�X�ŏ��������Ȃ���΂Ȃ�Ȃ��B
	����āACOM���g���O�ɂ́Ainc_ref�֐����Ăяo���A
	COM���g���I������Ƃ��ɂ́Adec_ref���Ăяo���Ηǂ��N���X
*/
public:
	static LRESULT inc_ref();			// COM�̎Q�ƃJ�E���g�̉��Z
	static LRESULT dec_ref();			// COM�̎Q�ƃJ�E���g�̌��Z

protected:
	static ThreadLocal<int> m_nCount;	//	�Q�ƃJ�E���g��ێ����Ă���
	static CCriticalSection m_cr;
	static CCriticalSection* GetCriticalSection() { return& m_cr; }
};

template <class T>
class CCOMObject {
/**
	CoCreateInstance�`ReleaseInstance��wrapper
	T ��COM�I�u�W�F�N�g
*/
public:
	LRESULT CreateInstance(REFCLSID clsid,REFIID guid){
		CCOMBase::inc_ref();
		Release();	//	�O�̂����ł�����
		if (FAILED(::CoCreateInstance(clsid,NULL,CLSCTX_INPROC_SERVER,
			guid, (VOID**)&pObj))){
			pObj = NULL;
			CCOMBase::dec_ref();
		}
		//	CoCreateInstance�̊֐��̎��s�ƁApObj��NULL�Ȃ�Ύ��s�Ƃ݂Ȃ�
		return pObj==NULL?1:0;
	}
	///	�C���X�^���X�𐶐�����

	LRESULT Release(){
		LRESULT lr = 0;
		if (pObj!=NULL){
			pObj->Release();
			pObj = NULL;
			lr = CCOMBase::dec_ref();
		}
		return lr;
	}
	///	�C���X�^���X���������

	T	get() const { return pObj; }
	///	���������I�u�W�F�N�g���擾����

	operator T* () const { return get(); }
	///	�Öٕϊ��q
	T& operator*() const  {return *get(); }
	T* operator->() const {return get(); }
	///	-> ���Z�q����`���Ă���

	CCOMObject() : pObj(0) {}
	virtual ~CCOMObject() { Release(); }

protected:
	T		pObj;	//	COM�I�u�W�F�N�g
};

class CLoadLibraryWrapper {
/**
	LoadLibrary�`ReleaseLibrary��wrapper
*/
public:
	///	DLL��Load
	LRESULT Load(const string& libname) {
		m_hDLL = LoadLibrary(libname.c_str());
		return m_hDLL==NULL?1:0;
	}

	///	DLL��Release
	void Release(){
		if (m_hDLL!=NULL){
			::FreeLibrary(m_hDLL);
			m_hDLL = NULL;
		}
	}

	///	DLL���̊֐��A�h���X�̎擾
	void* GetProc(const string& funcname){
		return ::GetProcAddress(m_hDLL,funcname.c_str());
	}

	///	�ǂݍ���DLL�̃C���X�^���X��Ԃ�
	HINSTANCE Get() const { return m_hDLL; }

	CLoadLibraryWrapper() { m_hDLL = NULL; }
	virtual ~CLoadLibraryWrapper() { Release(); }
protected:
	HINSTANCE m_hDLL;
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
