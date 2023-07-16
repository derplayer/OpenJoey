
#ifndef __yaneRound_h__
#define __yaneRound_h__

namespace yaneuraoGameSDK3rd {
namespace Math {

template <class T>
inline T roundRShift(const T t,int nRShiftCount)
/**
	丸め(四捨五入)つきの右シフト
		tをnRShiftCountだけ右シフトして返す

正と負の対称性を考えると、負数に関しては四捨五入の“入”
の場合は 0から遠い方の整数に行って欲しい気がします。
負数の四捨五入について議論されるのをほとんど見たことが
無いのですが、そのほうが自然でしょう。ここで四捨五入
と言うとき、この定義に基づくものとします。また、
この定義に基づくと、-3.5は四捨五入されて-4になるべきで
しょうし、-3.4ならば四捨五入の“捨”で、-3になるべきです。

逆にこうならなければ、sin,cosで円を描くときに非対称(3.5は
4になるのに-3.5は-4になる)ので歪んだものになる可能性があります。
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
	丸め(四捨五入)つきの割り算
		r = x/yを返す
	xが負数のときも正しく答えが返る

	roundDiv(4/3) = 1 , roundDiv(-4,3) = -1
	roundDiv(3/2) = 2 , roundDiv(-3,2) = -2(-1ではないことに注意)
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
