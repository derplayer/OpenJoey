#include "stdafx.h"
#include "yaneMemory.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

//	new,delete���ƂɃ��O�L�^���c�����H
#ifdef	USE_MEMORY_CHECK

/*
	new��delete�ŁA���O��f���o���B���̓f���o���ꂽ���O�̂����A
	new����delete���Ă��Ȃ�������˂��~�߂�ɂ́A���Ƃ���Perl��
	���̂悤�ȃv���O�����������Ηǂ��B

	# memcheck.pl
	while (<>) {
		if		(m/new[\s]*([a-f\d]+)[\s]*:[\s]*([\d]+)/){
			$dat{$1} = $2;
		} elsif (m/del[\s]*([a-f\d]+)/){
			delete $dat{$1};
		}
	}

	foreach (keys %dat) {
		print "$_ : $dat{$_}\n";
	}

		��L�̃v���O�����́A�W�����͂���e�L�X�g�𓾂āA
		�W�����͂ɏo�͂��Ă���̂ŁA���s�́A

	jperl memcheck.pl < debug_memory.txt > result.txt

	�̂悤�ɂ��čs�Ȃ��B

	�����āAnew����delete����Ă��Ȃ��������^�T�C�Y������ł���΁A
	���x�́A�ȉ���operator new�̊֐����ŁA�ǂ̃������ł��邩��
	�`�F�b�N����if���������āA���̏����������̂Ƃ���Ƀu���[�N
	�|�C���g���d�|���đ��点�Ă݂�B��������΁A�ǂ��Ŋm�ۂ��ꂽ
	���������������Ă��Ȃ����������킩��B

*/

void* operator new (size_t t){
	void *p = ::malloc(t);

	FILE*fp=fopen("debug_memory.txt","aw+");
	fprintf(fp,"new %.8x :%d\n",p,t);
	fclose(fp);

	return p;
}

void operator delete(void*p){
	if (p==NULL) return ;

	FILE*fp=fopen("debug_memory.txt","aw+");
	fprintf(fp,"del %.8x\n",p);
	fclose(fp);

	::free(p);
}

#endif

///////////////////////////////////////////////////////////////////////////////

#ifdef USE_MEMORY_STATE	//	�������̃f�o�b�O�N���X�𗘗p���邩
CMemoryState* CMemoryState::g_lpMemoryState = NULL;

void* operator new (size_t t){
	CMemoryState* lpMemoryState = CMemoryState::g_lpMemoryState;
	static bool bRecursive = false;
	// ���̊֐�����set::insert�ōċA�I�ɌĂяo�����̂��K�[�h����

/*
	if (lpMemoryState!=NULL && lpMemoryState->m_lpaby!=NULL){
		// �r���I�A�N�Z�X
		lpMemoryState->m_vCS.Enter();
		//	������new / delete���K�v
		if (lpMemoryState->m_nMax == 0 || lpMemoryState->m_nBlockSize < t) {
			//	�󂫃G���A����ւ� or �K��T�C�Y�ȏ�̃������m�ۗv��
			return (void *)::malloc(t);
		} else {
			// �r���I�A�N�Z�X
			lpMemoryState->m_vCS.Enter();
			//	�󂫃u���b�N��Ԃ�
			const LPVOID pTemp = (void*)(lpMemoryState->m_lpaby +
				lpMemoryState->m_lpanEmptyBlock[--(lpMemoryState->m_nMax)]
					* lpMemoryState->m_nBlockSize);
			// �r���I�A�N�Z�X
			lpMemoryState->m_vCS.Leave();
			return pTemp;
		}
	}
*/
	if (lpMemoryState!=NULL && lpMemoryState->m_lpaby!=NULL){
		//	������new / delete���K�v
		if (lpMemoryState->m_nBlockSize < t) {
			//	�K��T�C�Y�ȏ�̃������m�ۗv��
			return (void *)::malloc(t);
		} else {
			// �r���I�A�N�Z�X
			lpMemoryState->m_vCS.Enter();
			if (lpMemoryState->m_nMax == 0) {
				//	�󂫃G���A����ւ�
				lpMemoryState->m_vCS.Leave();
				return (void *)::malloc(t);
			}
			//	�󂫃u���b�N��Ԃ�
			const LPVOID pTemp = (void*)(lpMemoryState->m_lpaby +
				lpMemoryState->m_lpanEmptyBlock[--(lpMemoryState->m_nMax)]
					* lpMemoryState->m_nBlockSize);
			lpMemoryState->m_vCS.Leave();
			return pTemp;
		}
	}


	LPVOID p = (LPVOID)::malloc(t);
	// �r���I�A�N�Z�X
	if (lpMemoryState!=NULL) lpMemoryState->m_vCS.Enter();
	if (bRecursive){
		if (lpMemoryState!=NULL) lpMemoryState->m_vCS.Leave();
		return p;
	}
	//	�ċA�I�ɌĂяo����Ă���Ȃ�΁A���̂܂܋A��

	if (lpMemoryState!=NULL) {
		bRecursive = true;
		lpMemoryState->m_alpMemory.insert(pair<LPVOID,size_t>(p,t));
		bRecursive = false;
		lpMemoryState->m_vCS.Leave();
	}
	return p;
}

void operator delete(void*p){
	CMemoryState* lpMemoryState = CMemoryState::g_lpMemoryState;

	if (lpMemoryState!=NULL && lpMemoryState->m_lpaby!=NULL){
		//	������new / delete���K�v

		//	new����x���Ăяo����Ă��Ȃ��̂�delete���Ăяo����邱�Ƃ͖���
		//	�i�Ɖ��肵�Ă���j

		int n = (BYTE*)p - (BYTE*)lpMemoryState->m_lpaby;

		//	�����̃q�[�v��̃��������H
		if ((0 <= n) &&
			(n < lpMemoryState->m_nBlockNum*lpMemoryState->m_nBlockSize)) {
			// �r���I�A�N�Z�X
			lpMemoryState->m_vCS.Enter();
			n /= lpMemoryState->m_nBlockSize;
			lpMemoryState->m_lpanEmptyBlock[++lpMemoryState->m_nMax] = n;
			//	�������̉��
			lpMemoryState->m_vCS.Leave();
		} else {
			::free(p);
		}
		return;	// �����Ŕ����Ȃ��ƂQ�d��free���Ă��܂�
	}

	::free(p);
	if (CMemoryState::g_lpMemoryState!=NULL) {
		// �r���I�A�N�Z�X
		lpMemoryState->m_vCS.Enter();
		CMemoryState::g_lpMemoryState->m_alpMemory.erase(p);
		lpMemoryState->m_vCS.Leave();
	}
	//	������Ɏ��s����悤�Ȃ�A�����new���Ă��Ȃ���������delete�Ȃ̂����A
	//	���̂悤�ȃo�O�͋H�Ȃ̂ŋC�ɂ��Ȃ����Ƃɂ���
}

LRESULT	CMemoryState::Dump(ITextOutputStream*pDump) {
	LRESULT lr = 0;
	if (m_lpaby!=NULL){
		//	������new / delete���g�p���Ă���Ƃ��̃��������[�N�I
		for(int i=m_nMax;i<m_nBlockNum;i++){
			pDump->Out("�������Ă��Ȃ��������𔭌� %.8x (%d bytes����)\n",
				(LONG)((BYTE*)m_lpaby + m_lpanEmptyBlock[i]*m_nBlockSize),m_nBlockSize);
		}
	} else {
		map<LPVOID,size_t>::iterator it;
		it = this->m_alpMemory.begin();
		pDump->Clear();
		while (it != this->m_alpMemory.end()) {
			pDump->Out("�������Ă��Ȃ��������𔭌� %.8x (%d bytes)\n",(LONG)(it->first),it->second);
			it++; lr ++;
		}
	}
	return lr;
}

void	CMemoryState::BeginSnap() {
	Release();
	GetCallBack()->clear();
	g_lpMemoryState = this;
}
///	�X�i�b�v���J�n����

void	CMemoryState::BeginSnap(int nBlockSize,int nBlockNum) {
	Release();
	m_nBlockSize = nBlockSize;
	m_nBlockNum	 = nBlockNum;
	m_nMax	= nBlockNum; // �S����
	m_lpaby = new BYTE[nBlockSize*nBlockNum];
	m_lpanEmptyBlock = new int[nBlockNum];
	for(int i=0;i<nBlockNum;i++){ m_lpanEmptyBlock[i] = i; }
	GetCallBack()->clear();
	g_lpMemoryState = this;
}
///	�X�i�b�v���J�n����

void	CMemoryState::EndSnap() {
	smart_vector_ptr<function_callback>::iterator it=GetCallBack()->begin();
	while (it!=GetCallBack()->end()){
		(*it)->run(); it++;
	}
	GetCallBack()->clear();
	g_lpMemoryState = NULL;
}
///	�X�i�b�v���I������

#endif

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

