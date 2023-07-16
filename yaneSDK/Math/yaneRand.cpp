#include "stdafx.h"
#include "yaneRand.h"

namespace yaneuraoGameSDK3rd {
namespace Math {

void	CRand::SetSeed(DWORD dwSeed){
	//	乱数の種の設定。必ず一度呼び出す必要がある
	const int N = 624;
	for (int i=0;i<N;i++) {
		m_dwMt[i] = dwSeed & 0xffff0000;
		dwSeed = 69069 * dwSeed + 1;
		m_dwMt[i] |= (dwSeed & 0xffff0000) >> 16;
		dwSeed = 69069 * dwSeed + 1;
	}
	m_nMti = N;
}

/* Period parameters */
#define N 624
//	m_nMtの配列サイズも、このサイズなのだが、VC++6ではclass内const定数が
//	使えないのでこっちに持ってきている
#define M 397
#define MATRIX_A 0x9908b0df	  /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */	 
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

DWORD CRand::Get() {

	DWORD y;
	static DWORD mag01[2]={0x0, MATRIX_A};
	/* mag01[x] = x * MATRIX_A	for x=0,1 */

	if (m_nMti >= N) { /* generate N words at one time */
		int kk;

		if (m_nMti == N+1)	 /* if sgenrand() has not been called, */
			SetSeed(4357);	 /* a default initial seed is used	 */

		for (kk=0;kk<N-M;kk++) {
			y = (m_dwMt[kk]&UPPER_MASK)|(m_dwMt[kk+1]&LOWER_MASK);
			m_dwMt[kk] = m_dwMt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		for (;kk<N-1;kk++) {
			y = (m_dwMt[kk]&UPPER_MASK)|(m_dwMt[kk+1]&LOWER_MASK);
			m_dwMt[kk] = m_dwMt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		y = (m_dwMt[N-1]&UPPER_MASK)|(m_dwMt[0]&LOWER_MASK);
		m_dwMt[N-1] = m_dwMt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];

		m_nMti = 0;
	}
	
	y = m_dwMt[m_nMti++];
	y ^= TEMPERING_SHIFT_U(y);
	y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
	y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
	y ^= TEMPERING_SHIFT_L(y);

	return y; 
}

} // end of namespace Math
} // end of namespace yaneuraoGameSDK3rd
