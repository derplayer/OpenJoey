/*
	smart_ptr	: an intelligent pointer
		reprogrammed & desinged by yaneurao(M.Isozaki) '02/02/28
*/

#ifndef __YTLSmartPtr_h__
#define __YTLSmartPtr_h__

#include "../Thread/yaneCriticalSection.h"
#include "ref_deleter.h"
#include "exceptions.h"

namespace yaneuraoGameSDK3rd {
namespace YTL {

//	smart_ptr_base�͍폜
//	smart_ptr<void> => smart_obj
//	shared_ptr<void> => shared_obj
//	��typedef���Ă����̂ŁA�ȍ~�͂��������g���ă`��

namespace smart_ptr_detail
{
	template<class T> struct smart_ptr_traits {
		typedef T& reference_type;
	};
	template<> struct smart_ptr_traits<void> {
		typedef void reference_type;
	};
}

template<
	class T,
	class ThreadingModel = ThreadingModels::SingleThread
>
class smart_ptr : /*public smart_ptr_base,*/ public ThreadingModel
{
/**
	a real smart pointer
	�X�}�[�g�|�C���^�N���X�i��z��^�z�񓝍��Łj

	�i�Q�ƃJ�E���g�������Ă��āA�ǂ�������Q�Ƃ���Ȃ��Ȃ����Ƃ���
	�����I�ɃI�u�W�F�N�g����̂���悤�Ȍ����|�C���^�j

	�g�����F
			smart_ptr<CHoge> s1(new CHoge);

		�Ƃ����悤�ɂ���΁A���Ƃ́A

			smart_ptr<CHoge> s2;
			s2 = s1;

		�Ƃ����悤�ȃR�s�[��������Ă��A�Ō�Ɏc�����X�}�[�g�|�C���^����
		�ӔC�������ăI�u�W�F�N�g����̂��Ă����I

		���@�z��I�u�W�F�N�g���\�F

			smart_ptr<CHoge> s1;
			s1.AddArray(10);
			//	��CHoge* s1 = new CHoge[10]; �̈Ӗ��ɋ߂�

			//	CHogeDerived��CHoge�h���N���X
			s1.AddArray(new CHogeDerived[10],10);
			���B

		���@�z��|�C���^�̐������A�b�v�L���X�g�����\�B

		�܂��A�R���X�g���N�^��explicit�Œ�`����Ă���̂ŁA
			void function(smart_array<CHoge> av);
		�Ƃ����悤�Ȋ֐��̌Ăяo���ŁA���ʂ̃|�C���^��n�������Ƃ��́A
		���L�������̃X�}�[�g�|�C���^�𐶐����ēn�����ƁI

		�܂�A
			function(smart_array<CHoge>(pHoge,false));
		�Ƃ���āA�R���X�g���N�^�̑�Q�p�����[�^��false�ɂ��ČĂяo���B

		���@ThreadingModel::LockPolicy�|���V�[�N���X��ς��邱�Ƃɂ���Ĕr������̗L����I���\

		�r�����䂪�K�v�Ȃ��Ƃ��F
			smart_ptr<CHoge> s1;
		�r�����䂪�K�v�ł���Ƃ��F
			smart_ptr<CHoge, ThreadingModels::MultiThread> s1;


		���l�B���̃N���X�́A
			gcc version 2.95.3-6 (mingw)
			[command line] g++ -pedantic -Wall -ansi
		�ŃR���p�C���ʂ�͂��H�i���͂킩��Ȃ��j
*/
public:
	typedef smart_ptr<T, ThreadingModel> self_type;
	typedef typename smart_ptr_detail::smart_ptr_traits<T>::reference_type reference_type;

	///	���̃|�C���^�i��z��I�u�W�F�N�g�j�����R���X�g���N�^
	template <class S>
	explicit smart_ptr(S* _P, bool bOwner=true) :
	m_lpObj(NULL) , m_nIndex(0) , m_lpRefObj(NULL)
	/**
		_P�́A�I�u�W�F�N�g��n���܂��B
		bOwner�́A���̔z��X�}�[�g�|�C���^�ɔ��߃I�u�W�F�N�g��
			�����폜���ė~�����Ƃ���true�ɂ��܂��Bdefault�ł�true�B
	*/
	{
		if (_P!=NULL) { Init(_P,bOwner,1); }
			//	S�����z�N���X�ł��邱�Ƃ͖����D�D�͂�
	}

	///	�f�B�t�H���g�R���X�g���N�^
	smart_ptr() :	m_lpObj(NULL) , m_nIndex(0) , m_lpRefObj(NULL) {}

	//	�R�s�[�R���X�g���N�^
	smart_ptr(const self_type& s){
		inc_ref(s);
	}

	smart_ptr& operator = (const self_type& s){
		if (this != &s) { dec_ref(); inc_ref(s); }
		return *this;
	}

	~smart_ptr() {
		dec_ref();
	}

	///	�|�C���^�̂ӂ�����邽�߂̎d�|��
	reference_type operator*() const  {return *get(); }
	T* operator->() const {return get(); }

	/// �v�f�ւ̃A�N�Z�X
	reference_type operator [] (const int n) { return *get(n); }

	/**
		�|�����[�t�B�b�N�Ȕz��I�u�W�F�N�g���擾����
		�͈͊O�ւ̗v�f�ւ̃A�N�Z�X���ɂ́A
		CIndexOutOfBoundsException��O�����o�����
		NULL�|�C���^�ɑ΂���A�N�Z�X��CNullPointerException��O
		�X�}�[�g�|�C���^��NULL���ǂ�����isNull�Ń`�F�b�N����̂�
		���������d�l�ŗǂ�
	*/
	T* get(const int n=0) const
#ifdef USE_EXCEPTION
		throw(CIndexOutOfBoundsException,CNullPointerException)
#endif
	{
		ThreadingModel::Lock guard(*this);
#ifdef USE_EXCEPTION
		//	���̃|�C���^��Null�̂Ƃ���Null�|�C���^�̎擾�i�R�s�[���֎~����邱�ƂɂȂ邪�A����ŗǂ��j
		if ( isNull() ) { throw CNullPointerException(); }
		else if ( isEnd() ) { throw CIndexOutOfBoundsException(); }
#endif

		return getPointer(n);
	}

	///	get()�̃A�N�Z�X�ᔽ�ȃ|�C���^�ł���O���������Ȃ��o�[�W����
	T* getPointer(const int n=0) const
	{
		ThreadingModel::Lock guard(*this);
		if ( isNull() ) return NULL;
		return (T*)((BYTE*)m_lpObj + (n+m_nIndex)*get_obj_size());
	}

	/**
		�ȉ��̃��\�b�h�Ŏ擾�ł���smart_ptr�́A
		�Q�ƃJ�E���g�������L���Ă���̂ŁA�������������Ȃ�����
		�z��I�u�W�F�N�g���������Ȃ�
	*/
	///	�z��I�u�W�F�N�g�̂���v�f�ւ̃X�}�[�g�|�C���^���擾����
	self_type get_smart_ptr(const int n=0) const {
		ThreadingModel::Lock guard(*this);
		return self_type(*this)+=n;	//	����index�����Z
	}
	///	�z��I�u�W�F�N�g�̐擪�v�f�ւ̃X�}�[�g�|�C���^���擾����
	self_type begin() const {
		return get_smart_ptr(0-m_nIndex);
	}
	///	�z��̍Ō�̗v�f�̎��̗v�f�ւ̃X�}�[�g�|�C���^���擾����
	self_type end() const {
		return get_smart_ptr(size()-m_nIndex);
	}
	///	���̃|�C���^���z��͈̔͊O���w���Ă��邩���`�F�b�N����
	bool	isEnd() const {
		return (((ULONG)m_nIndex) >= (ULONG)size());
	}

	void	Add(){
	///	���L�����������C���X�^���X�̒ǉ������\��
		dec_ref();				//	�O�̂���
		Init(new T,true,1);		//	�x������
	}
	template <class S>
	void	Add(S*_P){				//	�|�����[�t�B�b�N�Ȍ^�ł���
		dec_ref();				//	�O�̂���
		if (_P!=NULL) {
			Init(_P,true,1);
		}
	}
	//	�����̂Q�̊֐����P�ɂ܂Ƃ߂��
	//	new T�̕����ŁAT �����ۃN���X���ƃR���p�C���G���[�ɂȂ�B

	template <class S>
	void	Add(S*_P,ref_object*ref){
	/**
		��̎�i���K�肵���|�C���^��n��
		�i���̌�A���̃X�}�[�g�|�C���^�͏��L�������j

		��)	smart_ptr<CHoge> pHoge;
			pHoge.Add(new CHoge,new my_ref_object);
	*/
		dec_ref();
		if (_P!=NULL){
			Init(_P,true,1,false,ref);
		}
	}

	///	���L���̖����|�C���^�̐ݒ�
	template <class S>
	void	Set(S*_P){			//	�|�����[�t�B�b�N�Ȍ^�ł���
		dec_ref();				//	�O�̂���
		if (_P!=NULL) {
			Init(_P,false,1);
		}
	}

	///	���L���������Ȃ��z��C���X�^���X�̒ǉ��\��
	template <class S>
	void	SetArray(S* p, const int n) {
		dec_ref();
		Init(p,false,n,true);
	}

	///	���L�����������z��C���X�^���X�̒ǉ������\��
	void	AddArray(const int n){
		dec_ref();
		if (n<=0) return ; // �Ȃ�ł�˂�D�D
		Init(new T[n],true,n,true);
	}

	template <class S>
	void	AddArray(S*_P, const int n, ref_object*ref=NULL){
	/**
		��̎�i���K�肵���z��|�C���^��n��
		�i���̌�A���̃X�}�[�g�|�C���^�͏��L�������j

		��)	smart_ptr<CHoge> pHoge;
			pHoge.Add(new CHoge[10],new my_ref_object);
	*/
		dec_ref();
		if (n<=0) return ; // �Ȃ�ł�˂�D�D
		Init(_P,true,n,true,ref);
	}

	template <class RelateType>
	self_type&	UpCast(const smart_ptr<RelateType,ThreadingModel>& s){
	///	�����I�A�b�v�L���X�g�\���@���@cf.smart_ptr_static_cast
	/**	�������A�������̂ł���smart_ptr�ԂŃA�b�v�L���X�g���Ă͂����Ȃ�
		���Ȃ킿�A
			smart_ptr<CHoge> p;
			p.UpCast(p); // ����͂���Ă͂����Ȃ�
	*/
		ThreadingModel::Lock guard(*this);
		if ((void*)this != (void*)&s) {
		//	���^���Ⴄ�����m��Ȃ��̂ł��̒��x�̃`�F�b�N�����o���Ȃ�
			dec_ref();

			//	�ȉ���inc_ref(s)�Ɠ���
			m_lpObj		= static_cast<T*>(s.getObj());
			//	����̓L���X�g���܂ނ̂ŃA�b�v�L���X�g�ł��Ȃ����
			//	�R���p�C���G���[�ɂȂ�
			m_nIndex	= *(const_cast<smart_ptr<RelateType,ThreadingModel>*>(&s))->getIndex();
			m_lpRefObj	= s.getRefObj();
			if (m_lpRefObj!=NULL) m_lpRefObj->inc_ref();
		}
		return *this;
	}

	template <class RelateType>
	self_type&	DownCast(const smart_ptr<RelateType,ThreadingModel>& s){
	///	�����I�A�b�v�L���X�g�\���@���@cf.smart_ptr_dynamic_cast
	/**	�������A�������̂ł���smart_ptr�ԂŃ_�E���L���X�g���Ă͂����Ȃ�
		���Ȃ킿�A
			smart_ptr<CHoge> p;
			p.DownCast(p); // ����͂���Ă͂����Ȃ�
	*/
		ThreadingModel::Lock guard(*this);
		if ((void*)this != (void*)&s) {
		//	���^���Ⴄ�����m��Ȃ��̂ł��̒��x�̃`�F�b�N�����o���Ȃ�
			dec_ref();

			//	�ȉ���inc_ref(s)�Ɠ���
			m_lpObj		= dynamic_cast<T*>(s.getObj());
			//	����̓L���X�g���܂ނ̂Ń_�E���L���X�g�ł��Ȃ����
			//	�R���p�C���G���[�ɂȂ�
			m_nIndex	=
				*(const_cast<smart_ptr<RelateType,ThreadingModel>*>(&s))->getIndex();
			m_lpRefObj	= s.getRefObj();
			if (m_lpRefObj!=NULL) m_lpRefObj->inc_ref();
		}
		return *this;
	}

	bool	unique() const {
	///	�I�u�W�F�N�g���Q�Ƃ��Ă���̂��B�ꂩ�H
		return count()==1;
	}
	int		count()	const {
	///	�I�u�W�F�N�g�̎Q�Ɛ���Ԃ�
		ThreadingModel::Lock guard(*this);
		return m_lpRefObj!=NULL?m_lpRefObj->get_ref():0;
	}

	bool	isNull() const {
	///	�w���Ă���I�u�W�F�N�g��NULL���H
		return m_lpObj==NULL;
	}

	int		size() const {
	///	�z��I�u�W�F�N�g�Ȃ�΁A�ő吔��Ԃ�
	///	��z��I�u�W�F�N�g�Ȃ�΁A1���Ԃ�
		if (m_lpRefObj==NULL) return 0;
		ThreadingModel::Lock guard(*this);
		return m_lpRefObj->get_size();
	}

	void	Delete() {	dec_ref(); }
	///	���̃I�u�W�F�N�g����̂���i�������A���̃X�}�[�g�|�C���^����Q��
	///	����Ă���΁A���ۂ�delete�܂ł͏o���Ȃ��j

	///	�X�}�[�g�|�C���^�̉����Z
	void dec() { 
		ThreadingModel::Lock guard(*this);
		m_nIndex--; }
	void inc() { 
		ThreadingModel::Lock guard(*this);
		m_nIndex++; }

	///	�����Z�̂��߂̃I�y���[�^
	self_type& operator++() {
		inc(); return (*this); }
	self_type operator++(int){
		ThreadingModel::Lock guard(*this);
		self_type _Tmp = *this; inc(); return (_Tmp); }
	self_type& operator--() {
		dec(); return (*this); }
	self_type operator--(int)
		{	ThreadingModel::Lock guard(*this);
		self_type _Tmp = *this; dec(); return (_Tmp); }
	self_type& operator+=(int _N)
		{	ThreadingModel::Lock guard(*this);
		m_nIndex += _N; return (*this); }
	self_type& operator-=(int _N)
		{	ThreadingModel::Lock guard(*this);
		m_nIndex -= _N; return (*this); }
	self_type operator+(int _N) const
		{ self_type _Tmp = *this; return (_Tmp += _N); }
	self_type operator-(int _N) const
		{ self_type _Tmp = *this; return (_Tmp -= _N); }

	///	��r�̂��߂̃I�y���[�^
	bool operator==(const self_type& _X) const
		{ThreadingModel::Lock guard(*this);
		return (getPointer() == _X.getPointer()); }
	bool operator!=(const self_type& _X) const
		{return (!(*this == _X)); }
	bool operator<(const self_type& _X) const
		{ThreadingModel::Lock guard(*this);
		return (getPointer() < _X.getPointer()); }
	bool operator>(const self_type& _X) const
		{return (_X < *this); }
	bool operator<=(const self_type& _X) const
		{return (!(_X < *this)); }
	bool operator>=(const self_type& _X) const
		{return (!(*this < _X)); }

	///	��̎q�I�u�W�F�N�g�ւ̃|�C���^��Ԃ��i���������p�j
	ref_object* getRefObj() const { return m_lpRefObj; }
	///	�I�u�W�F�N�g�̎����i���������p�j
	T* getObj() const { return m_lpObj; }
	///	index�̎擾(iterator�I�Ɏg�p���Ă���Ƃ�)�^���������p
	int* getIndex() { return &m_nIndex; }

protected:
	int		get_obj_size() const {
	//	�I�u�W�F�N�g�ЂƂ̗v�f�̃T�C�Y��Ԃ��i���������p�j
		ThreadingModel::Lock guard(*this);
		return m_lpRefObj!=NULL?m_lpRefObj->get_obj_size():0;
	}

	void	set_size(int nSize) {
	///	size�֐��Ŏ擾�ł���T�C�Y��ύX����
		ThreadingModel::Lock guard(*this);
		if (m_lpRefObj==NULL) return ;
		m_lpRefObj->set_size(nSize);
	}

	//	�������⏕�֐�
	template <class S>
	void	Init(S *s,bool bOwner,int nSize,bool bArray=false,ref_object*ref=NULL){
		ThreadingModel::Lock guard(*this);
		//	�����́A
		//	�P�D�I�u�W�F�N�g�̃N���X�@�Q�D���̃I�u�W�F�N�g�̏��L��
		//	�R�D���̃I�u�W�F�N�g�̐��@�S�D�z��I�u�W�F�N�g���ǂ���
		if (m_lpRefObj!=NULL) delete m_lpRefObj;
		m_nIndex = 0;
		m_lpObj = s;
		//	�������ŁA�R���p�C���G���[���o��̂́A
		//	smart_ptr�̃R���X�g���N�^�̈����Ɏw�肵�Ă���^��
		//	���̃X�}�[�g�|�C���^�Ŏw�肵�Ă���^�̔h���N���X���Ŗ����̂�����

		if (nSize==0) return ; // �Ȃ�ŁH�H
#ifdef USE_EXCEPTION
		if (!bOwner && ref!=NULL){
			//	Owner�Ŗ����̂ɉ�̃I�u�W�F�N�g��n���Ă���Ƃ͂ǂ�������������H
			throw CSyntaxException("���L���������̂ɉ�̃I�u�W�F�N�g���w�肳��Ă���");
		}
#endif
		/*	//	Owner����������΁A�ǂ����I�u�W�F�N�g�͍폜����Ȃ�
			//	(��ref_deleter::dec_ref)
		if (!bOwner) {
			m_lpRefObj = new null_ref_object;
			//	���L���������̂ŉ�̂��������Ȃ��I�u�W�F�N�g
		} else
		*/	
		if (ref!=NULL) {
			m_lpRefObj = ref;
		} else if (!bArray) {
			m_lpRefObj = new nonarray_ref_object<S>(s);
		} else {
			m_lpRefObj = new array_ref_object<S>(s);
		}
		m_lpRefObj->set_ref(1);
		m_lpRefObj->set_owner(bOwner);
		//	�I�[�i�[�Ŗ����̂ł���Ό����ĉ�̂���Ȃ��悤�ɂ��Ă���
		m_lpRefObj->set_obj_size(sizeof(S));
		m_lpRefObj->set_size(nSize);
	}

	//	�Q�ƃJ�E���g�̑����i�������p�̂݁j
	void	inc_ref(const self_type& s) {
		ThreadingModel::Lock guard(*this);
		m_lpObj		= s.m_lpObj;
		m_nIndex	= s.m_nIndex;
		m_lpRefObj	= s.m_lpRefObj;
		if (m_lpRefObj!=NULL) m_lpRefObj->inc_ref();
	}
	void	dec_ref() {
		ThreadingModel::Lock guard(*this);
		if (m_lpRefObj!=NULL){
			if (m_lpRefObj->dec_ref()){
				//	�I�u�W�F�N�g�͉�̎q�o�R�ō폜����
				delete m_lpRefObj;
			}
			m_lpRefObj = NULL;
			m_lpObj = NULL;
		}
	}

private:
	T*		m_lpObj;			//	���I�u�W�F�N�g
	int		m_nIndex;			//	����smart_ptr���A���̃I�u�W�F�N�g��
								//	���Ԗڂ̗v�f���w���Ă��邩
	ref_object* m_lpRefObj;		//	��̗p�I�u�W�F�N�g
};

template<typename T>
class shared_ptr : public smart_ptr<T, ThreadingModels::MultiThread>
{
public:
	typedef	shared_ptr<T> self_type;
	typedef	smart_ptr<T, ThreadingModels::MultiThread> base_type;

	template <class S>
	explicit shared_ptr(S* _P, bool bOwner=true) : base_type(_P,bOwner) {};
	shared_ptr() : base_type() {}
	shared_ptr(const base_type& s) : base_type(s) {}
};

typedef	smart_ptr<void> smart_obj;
typedef	shared_ptr<void> shared_obj;

//////////////////////////////////////////////////////////////////////////
//	cast�p���Z�q(smart_ptr,shared_ptr�����ɑ΂��Ďg����)

template <typename T, typename U, class ThreadingModel>
smart_ptr<T, ThreadingModel> smart_ptr_static_cast(const smart_ptr<U, ThreadingModel>& r)
/**
	smart_ptr<U>�̎��Q�ƃJ�E���^�ƁA�����ŕԂ����smart_ptr<T>��
	�Q�ƃJ�E���^�Ƃ͋��L�����B(���Ȃ킿smart_ptr<U>�̎����L����
	���D����Ȃ�)

	�g�p��)
		smart_ptr<RealResult> result;
		return smart_ptr_static_cast<IResult>(result);
*/
{	return smart_ptr<T, ThreadingModel>().UpCast(r);	}

template <typename T, typename U, class ThreadingModel>
smart_ptr<T, ThreadingModel> smart_ptr_dynamic_cast(const smart_ptr<U, ThreadingModel>& r)
/**
	smart_ptr<U>�̎��Q�ƃJ�E���^�ƁA�����ŕԂ����smart_ptr<T>��
	�Q�ƃJ�E���^�Ƃ͋��L�����B(���Ȃ킿smart_ptr<U>�̎����L����
	���D����Ȃ�)

	�g�p��)
		smart_ptr<RealResult> result;
		return smart_ptr_static_cast<IResult>(result);
*/
{	return smart_ptr<T, ThreadingModel>().DownCast(r);	}

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
