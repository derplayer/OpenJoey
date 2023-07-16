#include "stdafx.h"
#include "yaneCPUID.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

//	static object
singleton<CCPUID> CCPUID::m_obj;

/*
	CPU ID�̐����ɂ��Ă�
		http://www.dinop.com/vc/cpuidentify.html
	���Q�l�ɂȂ�
*/

/*
int		CCPUID::GetID(){
	if (m_nCPUID) return m_nCPUID;	//	���łɌ����ς݂Ȃ�΁A�����Ԃ�

	int nID,nErr = 0;

	//	VC++5�ł�Pentium�p�̖��߂͎g���Ȃ��B^^;
	#define cpuid __asm _emit 0x0F __asm _emit 0xA2

	//	CPU����������
	_asm {
		//	CPUID���߂͎g����̂��H
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
		//	CPUID���߂Ɏ��s���Ă���̂�486�ȑO
		m_nCPUID = 1;
	} else if ((nID & 0x800000)==0) {		//	bit23
		//	MMX�̃T�|�[�g�r�b�g�������Ă��Ȃ��̂Ŕ�MMX
		if ((nID & 0x8000)==0) {
			//	CMOV����������T�|�[�g���Ă��Ȃ��̂�Pentium Pro����
			m_nCPUID = 2;
		} else {
			//	CMOV����������T�|�[�g���Ă���̂�Pentium Pro�ȍ~
			m_nCPUID = 3;
		}
	} else {
		//	MMX�Ή�
		m_nCPUID = 4;

		//	SSE�Ή�
		if (nID & 0x2000000) m_nCPUID+=8;	//	bit25

		//	SSE2�Ή�
		if (nID & 0x4000000) m_nCPUID+=16;	//	bit26
	}
	return m_nCPUID;

	#undef cpuid
}
*/

//	BCC5.5�ŃR���p�C���ʂ�悤�Ƀn���h�A�Z���u���ɕύX�������I
//		by yaneurao('03/07/06)
int		CCPUID::GetID(){
	if (m_nCPUID) return m_nCPUID;	//	���łɌ����ς݂Ȃ�΁A�����Ԃ�

	int nID,nErr = 0;

	//	CPU����������
	//	���F�n���h�A�Z���u�����ꂽ���e�̓R���p�C���ɂ���Č��؂���Ȃ��̂ŁA
	//	�@�@ebx,ebp,edi,esp��push�����Ɏg�p����Ɖ����N���邩������Ȃ��B
	//	�@�@�Ή����߂�ǂ��̂ł�������g�p���Ȃ����ɂ���B
	//		�c�Ǝv������cpuid��ebx��ׂ��̂�ebx��push/pop����(;�L�D`)
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
		0x8b,0xc8,	// 0xd8->0xc8�ɕύX
		0x35,0,0,0x20,0,
		0x50,
		0x9d,
		0x9c,
		0x58,
		0x3b,0xc1,	// 0xc3->0xc1�ɕύX
		0x74,0x0a,	// 0x0b->0x0a�ɕύX
		0xb8,1,0,0,0,
		0x53,		// �ǉ�(push ebx)
		0x0f,0xa2,
		0x5b,		// �ǉ�(pop ebx)
		0x8b,0xc2,
		0xc3,	//	ret
		0xC6,0x44,0x24,0x04,0x01,
//0xc6,1,1,
		0xc3	//	ret
	};

	//nID = reinterpret_cast<int (*)(int&)>(&code_area)(nErr);
	int(*f)(int&) = (int(*)(int&))&code_area;
	nID = f(nErr);
	//	(C)��˂��炨
	
/*
	//	CPU����������
	_asm {
		//	eax = *nErr
		mov		ecx,eax

		//	CPUID���߂͎g����̂��H
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
		//	CPUID���߂Ɏ��s���Ă���̂�486�ȑO
		m_nCPUID = 1;
	} else if ((nID & 0x800000)==0) {		//	bit23
		//	MMX�̃T�|�[�g�r�b�g�������Ă��Ȃ��̂Ŕ�MMX
		if ((nID & 0x8000)==0) {
			//	CMOV����������T�|�[�g���Ă��Ȃ��̂�Pentium Pro����
			m_nCPUID = 2;
		} else {
			//	CMOV����������T�|�[�g���Ă���̂�Pentium Pro�ȍ~
			m_nCPUID = 3;
		}
	} else {
		//	MMX�Ή�
		m_nCPUID = 4;

		//	SSE�Ή�
		if (nID & 0x2000000) m_nCPUID+=8;	//	bit25

		//	SSE2�Ή�
		if (nID & 0x4000000) m_nCPUID+=16;	//	bit26
	}
	return m_nCPUID;
}



} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
