/*
	ref_deleter	: �Q�ƃJ�E���^�t����̎q
		programmed & desinged by yaneurao(M.Isozaki) '02/03/03
*/

#ifndef __YTLRefDeleter_h__
#define __YTLRefDeleter_h__

namespace yaneuraoGameSDK3rd {
namespace YTL {

class ref_object {
/**
	�Q�ƃJ�E���g�Ǘ������A
	�C�ӂ̗v�f��delete���邽�߂̊��I�u�W�F�N�g

	�Ǝ��̉�̏������������Ȃ�΁A
	���Ƃ���ref_object�h���N���X���ȉ��̂悤�ɗp�ӂ���B

class my_ref_object : public ref_object {
public:
	my_ref_object(CSound* p,const smart_ptr<function_callback>&func)
		: m_p(p),m_func(func) {}
	virtual void	delete_instance() {
		delete m_p;
		m_func->run();
	}
private:
	CSound* m_p;
	smart_ptr<function_callback_v> m_func;
	//	���ėp�R�[���o�b�N�e���v���[�g
};

*/
public:
	virtual void	delete_instance()=0;
	///	�ێ����Ă���I�u�W�F�N�g��delete����B�h���N���X�Œ�`����

	//	�Q�ƃJ�E���^�̑���
	void	inc_ref(){ m_nRef++; }
	bool	dec_ref(){ m_nRef--;
		//	�Q�ƃJ�E���^��0�ɂȂ����Ƃ��ɃI�u�W�F�N�g�̏��L���������
		//	�����I��delete����B���̂Ƃ��ɂ��̊֐��̕Ԃ��l��true�ɂȂ�
		if (m_nRef == 0) {
			if (m_bOwner) delete_instance();
			return true;
		}
		return false;
	}
	///	�Q�ƃJ�E���^�̐ݒ�^�擾
	void	set_ref(int n) { m_nRef = n; }
	int		get_ref() const { return m_nRef; }

	///	�I�[�i�[���̐ݒ�^�擾
	void	set_owner(bool b) { m_bOwner = b; }
	bool	get_owner() const { return m_bOwner; }

	///	�I�u�W�F�N�g�T�C�Y�̐ݒ�^�擾
	void	set_obj_size(int n) { m_nObjSize = n; }
	int		get_obj_size() const { return m_nObjSize; }

	///	�z��I�u�W�F�N�g�̍ő吔�̐ݒ�^�擾
	void	set_size(int n) { m_nMaxSize = n;}
	int		get_size() const { return m_nMaxSize; }

	virtual ~ref_object() {}	//	place holder

protected:
	//	���Ȃ炸�ȉ��̕ϐ����A�N�Z�b�T��ʂ��ď��������Ďg�����ƁI

	int		m_nRef;		//	�Q�ƃJ�E���g
	bool	m_bOwner;	//	�I�[�i�[��
	int		m_nObjSize;	//	�I�u�W�F�N�g�ЂƂ̃T�C�Y
	int		m_nMaxSize;	//	�z��I�u�W�F�N�g�̍ő�
};

//	��z��I�u�W�F�N�g��delete���邽�߂̃e���v���[�g
template<class T>
class nonarray_ref_object : public ref_object {
public:
	nonarray_ref_object(T* p) : m_p(p) {}
	virtual void	delete_instance() {
		delete m_p;
	}
private:
	T* m_p;	//	�폜���ׂ��A�h���X��ێ����Ă���
};

//	�C�ӂ̔z���delete���邽�߂̃e���v���[�g
template<class T>
class array_ref_object : public ref_object {
public:
	array_ref_object(T* pa) : m_pa(pa) {}
	virtual void	delete_instance() {
		delete [] m_pa;
	}
private:
	T* m_pa;	//	�폜���ׂ��A�h���X��ێ����Ă���
};

/*	//	����Ȃ��̂�����Ă��d���Ȃ��̂��H
//	deallocator��functor�Ŏw�肷�邽�߂̃e���v���[�g
template<class T>
class deallocator_ref_object : public ref_object {
public:
	deallocator_ref_object(T t_) : t(t_) {}
	virtual void	delete_instance() {
		t();
	}
private:
	T	t;
};
*/

/*	//	�ǂ���owner����������Ή������Ȃ��̂ł��̃N���X�͕s�v
//	�������Ȃ�ref_object
class null_ref_object : public ref_object{
public:
	virtual void	delete_instance() {}
	//	�������Ȃ�
};
*/

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
