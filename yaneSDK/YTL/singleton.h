/*
	singleton	: singleton template
		programmed & desinged by yaneurao(M.Isozaki) '02/03/13
*/

#ifndef __YTLSingleton_h__
#define __YTLSingleton_h__

#ifdef USE_MEMORY_STATE
	#include "../Auxiliary/yaneMemory.h"
#endif

#include "../Thread/yaneCriticalSection.h"

namespace yaneuraoGameSDK3rd {
namespace YTL {

template <class T>
class singleton {
/**
	���̃|�C���^�́A
	�K�v�ɔ���ꂽ�Ƃ��Ɏn�߂�object��new����悤�ȃ|�C���^

	�񃍁[�J����static�I�u�W�F�N�g�Ƃ��āA

	class CHoge{
	public:
		static singleton<CHoge> m_obj;
		static CHoge* GetObj() { return m_obj->get(); }
	};
	�̂悤�Ɏg���đ��v�I�I�u�W�F�N�g�̉�̂ɂ��Ă��ۏ؂����

	�Ƃ������A���̃N���X�́A�ЂƂ̃N���X�ɂ��A��������݂ł��Ȃ��B
	���܂��ɁA��L�̂悤��static�Ƃ��Ă����g���Ȃ�(;�L�D`)
*/
public:
	singleton() {
	//	�������͉����s�Ȃ�Ȃ�
	//	�Ƃ������񃍁[�J����static�I�u�W�F�N�g��
	//	�������̖�肪����̂ŁA���̃I�u�W�F�N�g�̏������ɂ��Ă�
	//	�����s�Ȃ��Ă͂����Ȃ�
	}

	~singleton() { Release(); }

	///	�|�C���^�̂ӂ�����邽�߂̎d�|��
	T& operator*() { return *get(); }
	T* operator->() { return get();	}
	T* get() { CheckInstance(); return m_lpObj; }

	///	�I�u�W�F�N�g��Null���ǂ��������؂���
	bool	isNull() const { return m_lpObj == NULL; }

	///	�I�u�W�F�N�g�������I�ɉ�̂���
	///	�i�f�X�g���N�^�ł�������Ă���j
	void Release();

	void CheckInstance();
	/**
		�C���X�^���X�����݂���̂����̃`�F�b�N���s�Ȃ��A
		���݂��Ȃ���ΐ�������B�{���A�����I�ɍs�Ȃ��K�v�͖������A
		���̃I�u�W�F�N�g���̂����ɏ������̎��Ԃ̂�����A
		���A���^�C���ɂ��̏��������s�Ȃ��Ă͍���Ƃ��ɂ́A
		�i�K�v�Ȃ̂��킩���Ă���΁j���O�ɐ������s�Ȃ��Ă���
		���ƂŁA���������ł���B
	*/

#if 0
//#ifdef USE_MEMORY_STATE
		if (CMemoryState::IsActive()){
		/**	Log�L�^��(BeginSnap�`EndSnap�̍Œ�)
			���̂Ƃ��ɐ������ꂽ�I�u�W�F�N�g�́AEndSnap�̂��ƁA
			operator new/delete�̃I�[�o�[���[�h�����������܂ł�
			�I�u�W�F�N�g���R�[���o�b�N�ɂ���ĉ�̂��Ă��K�v������
		*/
			//	callback�I�u�W�F�N�g��p��
			smart_ptr<function_callback> fn(
				function_callback_v::Create((void(singleton<T>::*)())Release,this)
			);
			CMemoryState::RegistCallBack(fn);
			//	callback���˗�����
		}
//#endif
#endif

protected:
	static CCriticalSection m_cs;
	//	������́A�V���O���X���b�h���́A�p���Ȃ�

	static T*	m_lpObj;
	//	static�ŗp�ӂ��邱�ƂŁA
	//	�����N��(�񃍁[�J����static�I�u�W�F�N�g�̏������O)��NULL
	//	�ł��邱�Ƃ�ۏ؂���
};

///////////////////////////////////////////////////////////////////////////////////////

//	static �ȃI�u�W�F�N�g
template <class T> T* singleton<T>::m_lpObj = 0;
template <class T> CCriticalSection singleton<T>::m_cs;

}}

#include "../AppFrame/yaneAppInitializer.h"	//	�}���`�X���b�h���삩�ǂ������擾

namespace yaneuraoGameSDK3rd {
namespace YTL {

//	�����̎����ŕK�v�ƂȂ�CAppInitializer�̂Ȃ���singelton��p���Ă���̂Ł���
//		include���Ȃ��ƁA�z�Q�ƂɂȂ��Ă��܂�
template <class T> void	singleton<T>::CheckInstance() {
	if (m_lpObj==NULL) {
		if (CAppInitializer::IsMultiThread()){
		//	�}���`�X���b�h���́Adouble-checked locking
			CCriticalLock guard(&m_cs);
			if (m_lpObj==NULL){
				m_lpObj = new T;
			}
		} else {
			m_lpObj = new T;
		}
	}
}

template <class T> void	singleton<T>::Release() {
	if (m_lpObj!=NULL) {
		if (CAppInitializer::IsMultiThread()){
		//	�}���`�X���b�h���́Adouble-checked locking
			CCriticalLock guard(&m_cs);
			if (m_lpObj!=NULL){
				delete m_lpObj;
				m_lpObj = NULL;
			}
		} else {
			delete m_lpObj;
			m_lpObj = NULL;
		}
	}
}

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
