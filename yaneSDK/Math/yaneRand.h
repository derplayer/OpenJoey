//
//	Mersenne Twister�@�ɂ�鍂���x��������
//	�@2^19937-1�Ƃ������ɒ����������������������������ɐ�������I
//

#ifndef __yaneRand_h__
#define __yaneRand_h__


#include "../Auxiliary/yaneSerialize.h"

namespace yaneuraoGameSDK3rd {
namespace Math {

class CRand : public IArchive { // ���̃N���X�́A�V���A���C�Y�Ή��I
/**
	Mersenne Twister�@�ɂ�鍂���x�̗����������[�`���ł��B
	2^19937-1�Ƃ����V���w�I�ȑ傫���̎������������������������ɐ������܂��B

	class IArchive �h���N���X�Ȃ̂ŁA�C�ӂ̃^�C�~���O�ŗ����̎��
	���̂܂ܕۑ����Ă����A�����n��̗������ēx���������邱�Ƃ��o���܂��B
*/
public:
	///	�����̎擾�B�O�҂�DWORD�S��̗������擾�B
	///	��҂́A0�`n-1�܂ł̗������擾�B
	///	n==0�̏ꍇ��0���Ԃ�(���̂ق����g���₷���̂�)
	DWORD	Get();					//	�����̎擾
	DWORD	Get(DWORD n) {			//	0�`n-1�̗����̎擾
		if (n==0) { 
/**
#ifdef USE_EXCEPTION
			throw	CRuntimeException();
#else
			return n;
#endif
*/
			return 0;
		}
		return Get() % n;
	}

	void	SetSeed(DWORD dwSeed);
	/**
		�����̎��ݒ肵�܂��B�ݒ肳�ꂽ��ɏ]���āA�����͐�������Ă����܂��B
		�K����x�͌Ăяo���K�v������܂��B
		�Ăяo���Ȃ��Ƃ��́A
			SetSeed(4357);
		���A��ԍŏ��̗����擾�̂Ƃ��Ɏ��s����܂��B
	*/

	void	Randomize()
	/**
		�����̎�Ƃ��āA���ݎ�����^���܂��B
		�v����ɁA�Č����̖����������o���܂��B
		SetSeed(GetTickCount())�Ƃ���Ă���̂ŁA
		SetSeed���Ăяo���K�v�͂���܂���B
	*/
		{	SetSeed(::GetTickCount());	}

	CRand() { m_nMti = 624+1; } /* means m_dwMt is not initialized */
	CRand(DWORD dwSeed) { SetSeed(dwSeed); }
	/**
		�R���X�g���N�^�́A�Q��ނ���A�p�����[�^�����̂ق��́A
		�����̏��������s�Ȃ��܂���B�p�����[�^�L��̂ق��́A
		�����̎�������Ƃ��Ď��A����Ɋ�Â��������𐶐����܂��B
		�i�����I��SetSeed���s�Ȃ��Ƃ������Ƃł��B
		����āASetSeed�����̂��ƍs�Ȃ��K�v�͂���܂���j
		�O��Ɠ��������n����Č��������ꍇ�Ȃǂɂ�����g���܂��B
	*/

	virtual ~CRand() {}

protected:
	DWORD	m_dwMt[624];	// the array for the state vector
	int		m_nMti;			// initialization counter

protected:
	//	override from IArchive
	virtual void Serialize(ISerialize& s){
		s.Store(ArraySerialize(m_dwMt));	//	�����o�ϐ��܂邲�ƁA
		s << m_nMti;						//	�V���A���C�Y����̂��I
	}
};

} // end of namespace Math
} // end of namespace yaneuraoGameSDK3rd

#endif
