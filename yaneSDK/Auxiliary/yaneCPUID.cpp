#include "stdafx.h"
#include "yaneCPUID.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

//	static object
singleton<CCPUID> CCPUID::m_obj;

/*
	CPU IDの説明については
		http://www.dinop.com/vc/cpuidentify.html
	が参考になる
*/

/*
int		CCPUID::GetID(){
	if (m_nCPUID) return m_nCPUID;	//	すでに検査済みならば、それを返す

	int nID,nErr = 0;

	//	VC++5ではPentium用の命令は使えない。^^;
	#define cpuid __asm _emit 0x0F __asm _emit 0xA2

	//	CPUを検査する
	_asm {
		//	CPUID命令は使えるのか？
		pushfd
		pop		eax
		mov		ebx, eax
		xor		eax,0x200000	//	toggle bit21
		push	eax
		popfd
		pushfd
		pop		eax
		cmp		eax,ebx
		jz		NO_CPUID

		mov		eax,1			//	get version info
		cpuid
		mov		nID,edx
		jmp		CPUID_EXIT

	NO_CPUID:
		mov		nErr,1		//	486 lower
	CPUID_EXIT:
	}

	if (nErr) {
		//	CPUID命令に失敗しているので486以前
		m_nCPUID = 1;
	} else if ((nID & 0x800000)==0) {		//	bit23
		//	MMXのサポートビットが立っていないので非MMX
		if ((nID & 0x8000)==0) {
			//	CMOV条件分岐をサポートしていないのでPentium Pro未満
			m_nCPUID = 2;
		} else {
			//	CMOV条件分岐をサポートしているのでPentium Pro以降
			m_nCPUID = 3;
		}
	} else {
		//	MMX対応
		m_nCPUID = 4;

		//	SSE対応
		if (nID & 0x2000000) m_nCPUID+=8;	//	bit25

		//	SSE2対応
		if (nID & 0x4000000) m_nCPUID+=16;	//	bit26
	}
	return m_nCPUID;

	#undef cpuid
}
*/

//	BCC5.5でコンパイル通るようにハンドアセンブルに変更しちゃる！
//		by yaneurao('03/07/06)
int		CCPUID::GetID(){
	if (m_nCPUID) return m_nCPUID;	//	すでに検査済みならば、それを返す

	int nID,nErr = 0;

	//	CPUを検査する
	//	注：ハンドアセンブルされた内容はコンパイラによって検証されないので、
	//	　　ebx,ebp,edi,espをpushせずに使用すると何が起こるか分からない。
	//	　　対応がめんどいのでそいつらを使用しない事にする。
	//		…と思ったらcpuidがebxを潰すのでebxをpush/popする(;´Д`)
	//		by ENRA('03/07/10)
/*
00491EEE 9C					pushfd
00491EEF 58					pop         eax
// 00491EF0 8B D8				mov         ebx,eax
00491EF0 8B C8				mov         ecx,eax
00491EF2 35 00 00 20 00		xor         eax,200000h
00491EF7 50					push        eax  
00491EF8 9D					popfd            
00491EF9 9C					pushfd           
00491EFA 58					pop         eax  
// 00491EFB 3B C3				cmp         eax,ebx 
00491EFB 3B C1				cmp         eax,ecx 
00491EFD 74 0B				je          NO_CPUID (491F0Ah) 
00491EFF B8 01 00 00 00		mov         eax,1 
00491F04 53					push		ebx            
00491F05 0F A2				cpuid            
00491F07 5B					pop			ebx            
00491F08 8B C2				mov         eax,edx 
// 00491F08 EB 03				jmp         CPUID_EXIT (491F0Dh) 
00491F0A c3					ret
// 00491F0A C6 01 01			mov         byte ptr [ecx],1 
00491F0B C6 44 24 04 01		mov         byte ptr [esp+4],1 
 */
	static const BYTE code_area[] ={
//		0x8b,0xc8,	// mov ecx,eax
		0x9c,
		0x58,
		0x8b,0xc8,	// 0xd8->0xc8に変更
		0x35,0,0,0x20,0,
		0x50,
		0x9d,
		0x9c,
		0x58,
		0x3b,0xc1,	// 0xc3->0xc1に変更
		0x74,0x0a,	// 0x0b->0x0aに変更
		0xb8,1,0,0,0,
		0x53,		// 追加(push ebx)
		0x0f,0xa2,
		0x5b,		// 追加(pop ebx)
		0x8b,0xc2,
		0xc3,	//	ret
		0xC6,0x44,0x24,0x04,0x01,
//0xc6,1,1,
		0xc3	//	ret
	};

	//nID = reinterpret_cast<int (*)(int&)>(&code_area)(nErr);
	int(*f)(int&) = (int(*)(int&))&code_area;
	nID = f(nErr);
	//	(C)やねうらお
	
/*
	//	CPUを検査する
	_asm {
		//	eax = *nErr
		mov		ecx,eax

		//	CPUID命令は使えるのか？
		pushfd
		pop		eax
		mov		ebx, eax
		xor		eax,0x200000	//	toggle bit21
		push	eax
		popfd
		pushfd
		pop		eax
		cmp		eax,ebx
		jz		NO_CPUID

		mov		eax,1			//	get version info
		cpuid
//		mov		nID,edx
		mov		eax,edx
		jmp		CPUID_EXIT

	NO_CPUID:
//		mov		nErr,1		//	486 lower
		mov		[ecx],1
	CPUID_EXIT:
	}
*/
	if (nErr) {
		//	CPUID命令に失敗しているので486以前
		m_nCPUID = 1;
	} else if ((nID & 0x800000)==0) {		//	bit23
		//	MMXのサポートビットが立っていないので非MMX
		if ((nID & 0x8000)==0) {
			//	CMOV条件分岐をサポートしていないのでPentium Pro未満
			m_nCPUID = 2;
		} else {
			//	CMOV条件分岐をサポートしているのでPentium Pro以降
			m_nCPUID = 3;
		}
	} else {
		//	MMX対応
		m_nCPUID = 4;

		//	SSE対応
		if (nID & 0x2000000) m_nCPUID+=8;	//	bit25

		//	SSE2対応
		if (nID & 0x4000000) m_nCPUID+=16;	//	bit26
	}
	return m_nCPUID;
}



} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
