/*
	factory_permutation	: �N���X�u���p
		programmed & desinged by yaneurao(M.Isozaki) '02/03/07
*/

#ifndef __YTLfactory__
#define __YTLfactory__

namespace yaneuraoGameSDK3rd {
namespace YTL {

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

///	T�����ۃN���X�̂Ƃ���factory
template <class T>
class factory_i : public factory_base {
/**
	factory
*/
public:
	virtual T* CreateInstance()=0;
	virtual void* Create()=0;
};

template <class T,class S>
class factory_i2 : public factory_i<T> {
///	T�͊��N���X(����)�BS�͂��̔h���N���X�B
	virtual T* CreateInstance() { return new S; }
	virtual void* Create() { return new S; }
};

#endif

//////////////////////////////////////////////////////////////////

#ifndef __YTLfactory_permutation_h__
#define __YTLfactory_permutation_h__

#include "smart_ptr.h"

template <class T,class S>
class factory_permutation {
/**
	�N���X�u�����s�Ȃ����߂̃e���v���[�g�B

	�f�B�t�H���g�ŁAS�N���X�𐶐�����B
	����́AT�̔h���N���X�ł��邱�ƁB

	SetFactory�ł́AT�̔h���N���X�̃|�C���^��n���B
	CreateInstance���Ăяo�����ƁA���ݕێ����Ă���factory�ɂ����
	�I�u�W�F�N�g�𐶐����A����smart_ptr��Ԃ��B

	T�����ۃN���X�̂Ƃ���:
		class factory_permutation_i
	��p���邱��

	�g�p��j
	factory_permutation<IHoge,CHogeSakura> f;
	//	CHogeSakura��IHoge�h���N���X
	//	�f�B�t�H���g�ŁA���̃I�u�W�F�N�g�����������
	smart_ptr<IHoge> p = f.CreateInstance();
	//	�������΁ACHogeSakura�̃I�u�W�F�N�g�����������
	f.SetFactory((CHogeTomoyo*)0);
	//	��������I�u�W�F�N�g��IHoge�h���N���X�ł���CHogeTomoyo��
	//�@�ύX���Ă݂�
	smart_ptr<IHoge> p = f.CreateInstance();
	//	�������΁ACHogeTomoyo�̃I�u�W�F�N�g�����������
	smart_ptr<factory<IHoge> > h = f.GetFactory();
	//	�����ł́A����factory��u����������factory<CHogeTomoyo>
	//	�I�u�W�F�N�g�����������

*/
public:
	factory_permutation() :
	  m_vFactory(new factory2<T,S>){}

	///		factory�̐ݒ�^�擾
	template <class U>
	void	SetFactory(const U*) {
	//	�������Ŏw�肵�Ă���class U�́AT�̔h���N���X�Ɖ���B
		m_vFactory.Add(new factory2<T,U>);
	}
	smart_ptr<factory<T> > GetFactory() const
	{ return m_vFactory; }

	///		factory�ɂ���ăC���X�^���X�̐���
	smart_ptr<T> CreateInstance() {
		return smart_ptr<T>(m_vFactory->CreateInstance());
	}

protected:
	smart_ptr<factory<T> >	m_vFactory;
};

template <class T,class S>
class factory_permutation_i {
/**
	�N���X�u�����s�Ȃ����߂̃e���v���[�g�B
	class factory_permutation �̊��N���XT�����ۃN���X�̂Ƃ���
	�o�[�W����
*/
public:
	factory_permutation_i() :
	  m_vFactory(new factory_i2<T,S>){}

	///		factory�̐ݒ�^�擾
	template <class U>
	void	SetFactory(const U*) {
	//	�������Ŏw�肵�Ă���class U�́AT�̔h���N���X�Ɖ���B
		m_vFactory.Add(new factory_i2<T,U>);
	}
	smart_ptr<factory_i<T> > GetFactory() const
	{ return m_vFactory; }

	///		factory�ɂ���ăC���X�^���X�̐���
	smart_ptr<T> CreateInstance() {
		return smart_ptr<T>(m_vFactory->CreateInstance());
	}

protected:
	smart_ptr<factory_i<T> >	m_vFactory;
};

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
