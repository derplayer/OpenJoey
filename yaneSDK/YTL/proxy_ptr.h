/*
	proxy_ptr	: a pointer treating proxy object
		reprogrammed & desinged by yaneurao(M.Isozaki) '02/03/07
*/

#ifndef __YTLProxyPtr_h__
#define __YTLProxyPtr_h__

namespace yaneuraoGameSDK3rd {
namespace YTL {

template<class T> class proxy_ptr {
/**
	���̃|�C���^�́A
	�K�v�ɔ���ꂽ�Ƃ��Ɏn�߂�object��new����悤�ȃ|�C���^

	�v��������܂Ő�����ۗ�����B������㗝�I�u�W�F�N�g(virtual proxy)

	�񃍁[�J����static��proxy_ptr���O������singleton�I�Ɏg���Ƃ��ɂ�
	�������̖�肪����̂ŁA
	class singleton �̂ق����g���Ă��������B
*/
public:
	proxy_ptr() : m_lpObj(NULL) {}

	///	�|�C���^�̂ӂ�����邽�߂̎d�|��
	T& operator*() { return *get(); }
	T* operator->() { return get();	}
	T* get() { CheckInstance(); return m_lpObj; }

	///	�I�u�W�F�N�g��Null���ǂ��������؂���
	bool	isNull() const { return m_lpObj == NULL; }

	///	�I�u�W�F�N�g�������I�ɉ�̂���
	///	�i�f�X�g���N�^�ł�������Ă���j
	void Release() {
		if (m_lpObj!=NULL) {
			delete m_lpObj;
			m_lpObj = NULL;
		}
	}

	void CheckInstance(){
		if (m_lpObj==NULL) m_lpObj = new T;
	}
	/**
		�C���X�^���X�����݂���̂����̃`�F�b�N���s�Ȃ��A
		���݂��Ȃ���ΐ�������B�{���A�����I�ɍs�Ȃ��K�v�͖������A
		���̃I�u�W�F�N�g���̂����ɏ������̎��Ԃ̂�����A
		���A���^�C���ɂ��̏��������s�Ȃ��Ă͍���Ƃ��ɂ́A
		�i�K�v�Ȃ̂��킩���Ă���΁j���O�ɐ������s�Ȃ��Ă���
		���ƂŁA���������ł���B
	*/

	~proxy_ptr() { Release(); }
protected:

	T*	m_lpObj;
};

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
