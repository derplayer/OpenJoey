/*
	ref_callback_deleter	: �Q�ƃJ�E���^�t���R�[���o�b�N��̎q
		programmed & desinged by yaneurao(M.Isozaki) '02/03/10
*/

#ifndef __YTLRefCallbackDeleter_h__
#define __YTLRefCallbackDeleter_h__

#include "function_callback.h"
#include "smart_ptr.h"
//	��̎��i��̌�j�ɃR�[���o�b�N�����Ă����I�u�W�F�N�g

namespace yaneuraoGameSDK3rd {
namespace YTL {

//	��z��I�u�W�F�N�g��delete���邽�߂̃e���v���[�g
template<class T>
class nonarray_callback_ref_object : public ref_object {
/**
		��z��I�u�W�F�N�g��delete���邽�߂̃e���v���[�g
		��̎��i��̌�j�ɃR�[���o�b�N�����Ă����
		class ref_object class function_callback ���Q�Ƃ̂���

		//	�g�p��
		smart_ptr<CSound> s;
		CSound* sound = new CSound;
		smart_ptr<function_callback>
			func(function_callback_v::Create(&test));
		s.Add(sound,new nonarray_callback_ref_object<CSound>(sound,func));
		������smart_ptr<CSound>�́A��̎��i��̌�j��test�֐����Ăяo�����
*/
public:
	nonarray_callback_ref_object(T* p,const smart_ptr<function_callback>& func)
		: m_p(p),m_func(func) {}
	virtual void	delete_instance() {
		delete m_p;
		m_func->run();
	}
private:
	T* m_p;	//	�폜���ׂ��A�h���X��ێ����Ă���
	smart_ptr<function_callback> m_func;
	//	���ėp�R�[���o�b�N�e���v���[�g
};

//	�C�ӂ̔z���delete���邽�߂̃e���v���[�g
template<class T>
class array_callback_ref_object : public ref_object {
/**
		�C�ӂ̔z���delete���邽�߂̃e���v���[�g
		��̎��i��̌�j�ɃR�[���o�b�N�����Ă����
		class ref_object class function_callback ���Q�Ƃ̂���

*/
public:
	array_callback_ref_object(T* pa,const smart_ptr<function_callback>& func)
		: m_pa(pa),m_func(func) {}
	virtual void	delete_instance() {
		delete [] m_pa;
		m_func->run();
	}
private:
	T* m_pa;	//	�폜���ׂ��A�h���X��ێ����Ă���
	smart_ptr<function_callback> m_func;
	//	���ėp�R�[���o�b�N�e���v���[�g
};

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
