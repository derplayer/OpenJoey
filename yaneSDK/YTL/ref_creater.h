///
///		�Q�ƃJ�E���g�t��creater
///
#ifndef __YTLRefCreater_h__
#define __YTLRefCreater_h__

namespace yaneuraoGameSDK3rd {
namespace YTL {

template <class T>
class ref_creater {
	/**
		�Q�ƃJ�E���g�t���I�u�W�F�N�g

		class CSoundManager {
		ref_creater<CDirectSound> m_vCreater;
		public:
		ref_creater<CDirectSound>* GetCreater() { return &m_vCreater;}
		//	�ȉ���
		};

		�P�D
		GetCreater()->inc_ref();
		//	�g���O�ɎQ�ƃJ�E���g���Z

		�Q�D
		CDirectSound* p = GetCreater()->get();
		//	�C���X�^���X�̎擾

		�R�D
		GetCreater()->dec_ref();
		//	�g���I�������Q�ƃJ�E���g���Z
		�i���̂Ƃ������I�ɃI�u�W�F�N�g�͉�������j

	*/
public:

	bool	inc_ref() {
		if (nRef++==0) {
			p = CreateInstance(); return true;
		}
		return false;
	}
	///	�Q�ƃJ�E���g�̉��Z
	///	�ŏ��ɉ��Z�����Ƃ��ɁA�I�u�W�F�N�g�����������
	///	�I�u�W�F�N�g���������ꂽ�̂Ȃ��true

	bool	dec_ref() {
		if (--nRef==0) {
			Release();
			return true;
		}
		return false;
	}
	///	�Q�ƃJ�E���g�̌��Z
	///	�O�ɂȂ����Ƃ��ɁA�I�u�W�F�N�g����̂����
	///	�I�u�W�F�N�g����̂��ꂽ�̂Ȃ��true

	T*	get() const { return p; }
	///	�I�u�W�F�N�gT�̎擾

	void	Release() { if (p) { delete p; p=0; } }
	///	�I�u�W�F�N�g�̋������

	virtual T*	CreateInstance() { return new T; }
	///	AddRef�̂Ƃ��ɃI�u�W�F�N�g�������镔����
	///	�I�[�o�[���C�h�ł���

	ref_creater() : nRef(0),p(0) {}
	virtual ~ref_creater() { Release(); }

protected:
	T*		p;
	int		nRef;
};

template <class T,class S>
class ref_creater_and_mediator :
	public ref_creater<T> , public mediator<S> {
/**
	class ref_creater �̃��[�J���C�Y��

	T �̃R���X�g���N�^��S*��n���āA�����mediator�Ƃ���
	�g�������N���X�̂��߂Ɏg��

	��:
	ref_creater_and_mediator<CDirectSound,ISoundManager>	m_vDirectSound;
	//	DirectSound�͕K�v�ɂȂ����Ƃ��ɂ̂ݐ��������
	�Ƃ���Ă����΁A
		m_vDirectSound.inc_ref()
	�ƊO�����琶����v�����ꂽ�Ƃ��ɁA
		new CDirectSound(pSoundManager);
	�ƁA���̃I�u�W�F�N�g�̐������ɓn���Ă�����S*��
	�R���X�g���N�^�Ɏw�肵�Đ�������B

	inc_ref����܂ł�
		mediator::SetOutClass����΁A
	m_vDirectSound.inc_ref()
	�ŃI�u�W�F�N�g�����������Ƃ��A����SetOutClass�Őݒ肵��
	�|�C���^���R���X�g���N�^�ɓn��܂��B

*/
public:
	ref_creater_and_mediator(S*p=NULL) : mediator<S>(p) {}
protected:
	virtual T* CreateInstance() { return new T(&GetOutClass()); }
};

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
