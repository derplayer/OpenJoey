//
//		�Q�[���p�A�ėp�J�E���^
//

#ifndef __yaneCounter_h__
#define __yaneCounter_h__


#include "../Auxiliary/yaneSerialize.h"

namespace yaneuraoGameSDK3rd {
namespace Math {

class ICounter;

class ICounterFactory {
/**
	���[�U�[�œƎ��ɃJ�E���^(ICounter�h���N���X)��p�ӂ���ꍇ�A
	GetType�ŁA1000�ȏ��unique�Ȑ�����Ԃ��悤�ɂ��āA
	����Factory�h���N���X�ŁACreateInstance���I�[�o�[���C�h���āA
	���̐������󂯎�����Ƃ��ɁA���̃N���X��new���ĕԂ��悤�ɂ��āA
	����Factory���AICounter::SetFactory����Ηǂ��B

	��������΁ACProxyCounter�ɂ��A���̐V����������J�E���^��
	�V���A���C�Y���\�ƂȂ�B
*/
public:
	virtual ICounter* CreateInstance(int n)=0;

	virtual ~ICounterFactory(){}
};

class ICounter : public IArchive {
/**
		�J�E���^�p�̊��N���X
*/
public:
	///		int�Ƃ̐e�a��
	virtual operator int () const=0;
	virtual const int operator = (int n)=0;

	///		�����Z
	virtual void	Inc()=0;
	virtual void	Dec()=0;

	///		�����ʒu���H
	virtual bool IsBegin() const = 0;
	///		�I�[�ɒB�����̂��H
	virtual bool IsEnd() const = 0;

	virtual int GetType() const = 0;
	static	ICounter* CreateInstance(int nType);
	/**
		�蓮RTTI
			CNullCounter		:	0
			CRootCounter		:	1
			CSaturationCounter	:	2
			CInteriorCounter	:	3
		GetType�́A���I�Ȍ^��ԍ��Ŏ擾����
		CreateInstance�́A�ԍ�����A���̃I�u�W�F�N�g�𐶐����邽�߂�factory
		��L�̔ԍ��ȊO�ł���΁ASetFactory�Őݒ肳�ꂽfactory��p���Đ�������
	*/
	static	void	SetFactory(const smart_ptr<ICounterFactory>& v)
		{	m_vFactory = v; }
	static	smart_ptr<ICounterFactory>	GetFactory()
		{ return m_vFactory; }

	virtual ~ICounter(){}

protected:
	static	smart_ptr<ICounterFactory> m_vFactory;
};

class CNullCounter : public ICounter {
/**
		class ICounter ��Null Device
*/
public:
	virtual operator int () const { return 0; }
	virtual const int operator = (int n) { return n; }
	virtual void	Inc() {}
	virtual void	Dec() {}
	virtual bool IsBegin() const { return false;}
	virtual bool IsEnd() const { return false; }
	virtual int GetType() const { return 0; }
	virtual void Serialize(ISerialize&) {}
};

/////////////////////////////////////////////////////////////////////////

class CProxyCounter : public ICounter {
/**
	ICounter�̂��߂̑㗝��B�����́A�f�V���A���C�Y�̂Ƃ���
	ICounter��factory��p���āA���̃I�u�W�F�N�g���Č�����
*/
public:
	CProxyCounter() : m_vCounter(new CNullCounter) {}
	CProxyCounter(const smart_ptr<ICounter>& p) : m_vCounter(p){}

	///		int�Ƃ̐e�a��
	virtual operator int () const { return *m_vCounter.get(); }
	virtual const int operator = (int n) { return ((*m_vCounter.get()) = n);}

	///		�����Z
	virtual void	Inc() { m_vCounter->Inc(); }
	virtual void	Dec() { m_vCounter->Dec(); }

	///		�����ʒu���H
	virtual bool IsBegin() const { return m_vCounter->IsBegin(); }
	///		�I�[�ɒB�����̂��H
	virtual bool IsEnd() const { return m_vCounter->IsEnd(); }

protected:
	smart_ptr<ICounter>	m_vCounter;
	ICounter* GetCounter() { return m_vCounter.get(); }

	//	overriden from IArchive
	virtual void Serialize(ISerialize&);
};

/////////////////////////////////////////////////////////////////////////

class CRootCounter : public ICounter {
/**
	�����l(nStart)�E�I���l(nEnd)�A�����ĕK�v�Ȃ�Α����l(nStep)��ݒ肵�܂��B
	���̂��ƁAInc�֐������s����ƁA1�������܂��B(nStep���ݒ肳��Ă���΁A
	���̐����������܂�)�@�����āA���Z�������ʁAnEnd�ȏ�ɂȂ�Ǝ����I��
	nStart�ɖ߂�܂��B(nEnd�ɂ͂Ȃ�܂���)

	Reset�֐��́A�J�E���^�l�������lnStart�ɖ߂��܂��B
	���邢�́ASetInit�֐���Reset��̏����l���ݒ肳��Ă���΁A���̒l�ɂ��܂��B

	�܂��Aint�^�Ƃ̑��ݕϊ����\�ł��̂ŁA��������int�^�̕ϐ��ł��邩�̂悤��
	�g���܂��B

	��j
	CRootCounter r;
	r.Set ( 0, 256 , -5 );
	// nStart == 0 , nEnd == 256�Ƃ����悤�ɃJ�E���^��ݒ肷��
	r = 128;

	���̏�ԂŁAr++; ���T����s����ƂT��ڂ� r == 129�ɂȂ�Br--;
	���T����s����ƂT��ڂ� r == 127�ɂȂ�B

	�܂��AnStart��nEnd�łȂ��ėǂ��̂ł��B
	�܂�A���Z( Inc / ++ )�ł́AnEnd�����փC���N�������g���܂��B
	���Z( Dec / -- )�ł́AnStart�����փC���N�������g���܂��B

	�܂��AStep���}�C�i�X�ł���ꍇ�́A
	���̐�Βl�񐔂�Inc�����o�֐��̌Ăяo���ɂ���āA1����nEnd�����ɐi�݂܂��B
*/
public:
	///	nStep�͈��̑����̐�Βl�B�}�C�i�X��1/nStep�̈Ӗ�
	///	nStart��nEnd�łȂ��ėǂ�
	void	Set(int nStart,int nEnd,int nStep=1)
		{ m_nStart=nStart; m_nEnd=nEnd; m_nStep=nStep; Reset(); }
	void	SetStep(int nStep) { m_nStep = nStep; }
	void	SetStart(int nStart) { m_nStart = nStart; }
	void	SetEnd(int nEnd) { m_nEnd = nEnd; }

	///	�擾
	int		GetStep() const { return m_nStep; }
	int		GetStart() const { return m_nStart; }
	int		GetEnd() const { return m_nEnd; }

	///	�J�E���^�̃��Z�b�g
	void	Reset() { m_nRootCount= m_nStart; m_nRate=0; }

	///	property..
	virtual bool	IsBegin() const { return m_nRootCount == m_nStart; }
	virtual bool	IsEnd() const { return m_nRootCount == m_nEnd; }

	CRootCounter();
	CRootCounter(int nEnd);
	CRootCounter(int nStart,int nEnd,int nStep=1);

	//	int�Ƃ̑��ݕϊ�
	operator int () const { return m_nRootCount; }
	const int operator = (int n) { m_nRootCount = n; return n; }
	int		Get () const { return m_nRootCount; }

	//	�J�E���^�̃C���N�������g(�I�[�܂ŒB����ƁA�����Œ�~����)
	void	Inc() { inc(true); }
	void	Dec() { inc(false); }
	//	���Z�i��End�����փC���N�������g�j�^���Z�i��Start�����ւ̃C���N�������g�j
	CRootCounter& operator++()
		{ Inc(); return (*this); }
	CRootCounter operator++(int)
		{ CRootCounter _Tmp = *this; Inc(); return (_Tmp); }
	CRootCounter& operator--()
		{ Dec(); return (*this); }
	CRootCounter operator--(int)
		{ CRootCounter _Tmp = *this; Dec(); return (_Tmp); }

	virtual int GetType() const { return 1; }

protected:
	void	inc(bool bAdd=true);

	int		m_nRootCount;
	int		m_nStart;
	int		m_nEnd;
	int		m_nStep;
	int		m_nRate;	//	nStep<0�̂Ƃ��́A�����Inc()��+1�����

	virtual void Serialize(ISerialize&s) {
		s << m_nRootCount << m_nStart << m_nEnd << m_nStep << m_nRate;
	}
};

class CSaturationCounter : public ICounter {
/**
	�O�a�J�E���^�Bclass CRootCounter �̃o���G�[�V�����B
	�u�J�E���^��++���Ă����A�I���l�ɂȂ����Ƃ��ɁA�����ŃJ�E���^��
	��~����v�Ƃ��������������قȂ�B
*/
public:
	///	nStep�͈��̑����̐�Βl�B�}�C�i�X��1/nStep�̈Ӗ�
	///	nStart��nEnd�łȂ��ėǂ�
	void	Set(int nStart,int nEnd,int nStep=1)
		{ m_nStart=nStart; m_nEnd=nEnd; m_nStep=nStep; Reset(); }
	void	SetStep(int nStep) { m_nStep = nStep; }
	void	SetStart(int nStart) { m_nStart = nStart; }
	void	SetEnd(int nEnd) { m_nEnd = nEnd; }

	///	�擾
	int		GetStep() const { return m_nStep; }
	int		GetStart() const { return m_nStart; }
	int		GetEnd() const { return m_nEnd; }

	///	�J�E���^�̃��Z�b�g
	void	Reset() { m_nRootCount= m_nStart; m_nRate=0; }

	///	property..
	virtual bool	IsBegin() const { return m_nRootCount == m_nStart; }
	virtual bool	IsEnd() const { return m_nRootCount == m_nEnd; }

	CSaturationCounter();
	CSaturationCounter(int nEnd);
	CSaturationCounter(int nStart,int nEnd,int nStep=1);

	//	int�Ƃ̑��ݕϊ�
	operator int () const { return m_nRootCount; }
	const int operator = (int n) { m_nRootCount = n; return n; }
	int		Get () const { return m_nRootCount; }

	//	�J�E���^�̃C���N�������g(�I�[�܂ŒB����ƁA�����Œ�~����)
	void	Inc() { inc(true); }
	void	Dec() { inc(false); }
	//	���Z�i��End�����փC���N�������g�j�^���Z�i��Start�����ւ̃C���N�������g�j
	CSaturationCounter& operator++()
		{ Inc(); return (*this); }
	CSaturationCounter operator++(int)
		{ CSaturationCounter _Tmp = *this; Inc(); return (_Tmp); }
	CSaturationCounter& operator--()
		{ Dec(); return (*this); }
	CSaturationCounter operator--(int)
		{ CSaturationCounter _Tmp = *this; Dec(); return (_Tmp); }

	virtual int GetType() const { return 2; }

protected:
	void	inc(bool bAdd=true);

	int		m_nRootCount;
	int		m_nStart;
	int		m_nEnd;
	int		m_nStep;
	int		m_nRate;	//	nStep<0�̂Ƃ��́A�����Inc()��+1�����

	virtual void Serialize(ISerialize&s) {
		s << m_nRootCount << m_nStart << m_nEnd << m_nStep << m_nRate;
	}
};

class CInteriorCounter : public ICounter {
/**
	�����J�E���^��񋟂��܂��B

	Set�����o�֐��ŁA�����l(nStart)�E�I���l(nEnd)�A
	�����ĕ�����(nFrames)��ݒ肵�܂��B

	��������΁A���̃J�E���^��nStart����n�܂�A
	nFrame���Inc�����o�֐��̌Ăяo���ɂ����nEnd�ɓ��B����悤��
	�����J�E���^�ƂȂ�܂��B

	�܂��AInc�́Aoperator++�Ƃ��Ă���`���Ă���܂��B
	Inc�̋t����ł���Dec������A�����operator--�Ƃ��Ă���`���Ă���܂��B
*/
public:
	CInteriorCounter();

	///	int�Ƃ̑��ݕϊ�
	virtual operator int () const { return m_nNow; }
	virtual const int operator = (int n) { m_nNow = m_nStart = n; m_nFramesNow = 0; return n; }

	virtual void	Inc();		///	���Z
	CInteriorCounter& operator++() { Inc(); return (*this); }
	CInteriorCounter operator++(int) { CInteriorCounter _Tmp = *this; Inc(); return (_Tmp); }

	virtual void	Dec();		///	���Z
	CInteriorCounter& operator--() { Dec(); return (*this); }
	CInteriorCounter operator--(int) { CInteriorCounter _Tmp = *this; Dec(); return (_Tmp); }

	void	Set(int nStart,int nEnd,int nFrames);
	/**
		�����l(nStart)�E�I���l(nEnd)�A�����ĕ�����(nFrames)��ݒ肵�܂��B
	*/

	///	���݂̒l���i�ꎞ�I�Ɂj�ύX����B����Inc/Dec�ŁA����Ȓl�ɖ߂�
	void	Set(int nNow) { *this = nNow; }

	virtual bool	IsBegin() const { return (m_nNow == m_nStart);}
	virtual bool	IsEnd() const { return (m_nNow == m_nEnd);}

	///	property
	int		GetFrame() const { return m_nFrames; }
	int		GetStart() const { return m_nStart;}
	int		GetEnd() const { return m_nEnd;}
	///		���݂̃t���[����(inc��������)�̎擾�B
	int		GetFrameNow() const { return m_nFramesNow; }

	virtual int GetType() const { return 3; }

protected:
	int		m_nNow;			//	���݂̒l
	int		m_nStart;		//	�����l
	int		m_nEnd;			//	�I���l
	int		m_nFrames;		//	�t���[���������i�I���l�ɂȂ�܂ŉ���Inc������΂����̂��j
	int		m_nFramesNow;	//	���݁A���t���[���ڂ��H

	virtual void Serialize(ISerialize&s) {
		s << m_nNow << m_nStart << m_nEnd << m_nFrames << m_nFramesNow;
	}
};

/////////////////////////////////////////////////////////////////////////

} // end of namespace Math
} // end of namespace yaneuraoGameSDK3rd

#endif
