
#ifndef __yaneRound_h__
#define __yaneRound_h__

namespace yaneuraoGameSDK3rd {
namespace Math {

template <class T>
inline T roundRShift(const T t,int nRShiftCount)
/**
	�ۂ�(�l�̌ܓ�)���̉E�V�t�g
		t��nRShiftCount�����E�V�t�g���ĕԂ�

���ƕ��̑Ώ̐����l����ƁA�����Ɋւ��Ă͎l�̌ܓ��́g���h
�̏ꍇ�� 0���牓�����̐����ɍs���ė~�����C�����܂��B
�����̎l�̌ܓ��ɂ��ċc�_�����̂��قƂ�ǌ������Ƃ�
�����̂ł����A���̂ق������R�ł��傤�B�����Ŏl�̌ܓ�
�ƌ����Ƃ��A���̒�`�Ɋ�Â����̂Ƃ��܂��B�܂��A
���̒�`�Ɋ�Â��ƁA-3.5�͎l�̌ܓ������-4�ɂȂ�ׂ���
���傤���A-3.4�Ȃ�Ύl�̌ܓ��́g�́h�ŁA-3�ɂȂ�ׂ��ł��B

�t�ɂ����Ȃ�Ȃ���΁Asin,cos�ŉ~��`���Ƃ��ɔ�Ώ�(3.5��
4�ɂȂ�̂�-3.5��-4�ɂȂ�)�̂Řc�񂾂��̂ɂȂ�\��������܂��B
*/
{
	T r;
	r = t + (nRShiftCount<=0?0:((T)1) << (nRShiftCount-1))
		- !(t >= 0);
	r >>= nRShiftCount;

/*
*/
	return r;
}

template <class T>
inline T roundDiv(const T x,const T y)
/**
	�ۂ�(�l�̌ܓ�)���̊���Z
		r = x/y��Ԃ�
	x�������̂Ƃ����������������Ԃ�

	roundDiv(4/3) = 1 , roundDiv(-4,3) = -1
	roundDiv(3/2) = 2 , roundDiv(-3,2) = -2(-1�ł͂Ȃ����Ƃɒ���)
*/
{
	T r;
	if (x>=0)
		r = ( x + (y>>1) ) / y;
	else
		r = ( x - (y>>1) ) / y;
	return r;
}

}}	//	end of namespace

#endif
