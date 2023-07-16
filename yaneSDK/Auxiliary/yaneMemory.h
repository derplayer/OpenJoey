
#ifndef __yaneMemory_h__
#define __yaneMemory_h__


#ifdef USE_MEMORY_STATE	//	�������̃f�o�b�O�N���X�𗘗p���邩

#include "../Auxiliary/yaneStream.h"
#include "../Thread/yaneCriticalSection.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class CMemoryState {
/**

	///	�I�I�I�c�`�m�f�d�q�I�I�I
	///	�|�|�|�|���̃N���X�͍�肩���|�|�|�|
	///	�I�I�I�I�g��Ȃ��łˁI�I�I


	�P�D����new���Ă��郁�����̃X�i�b�v�V���b�g���擾���āA
	���������[�N�����o���邽�߂̃N���X�B

	�Q�D���邢�́A���O�̍�����new /delete���`����B

	�P�D�Q�D�̗����Ɏg����B

	yaneConfig.h��USE_MEMORY_STATE��define���Ă���Ƃ��ɂ̂ݗL��

	�g�����P�D

	���̃N���X�𗘗p���āA

	CTextOutputStreamFile dump;
	dump.SetFileName("leaklog.txt");
	CMemoryState s;

	s.BeginSnap();

	{
		BYTE* lpabyLeak1 = new BYTE[100];
		DWORD* lpabyLeak2 = new DWORD[100];

	}
	s.EndSnap();

	s.Dump(&dump);

	�̂悤�ɂ���΁A���̂Q�ӏ��̃��������[�N�����݂��邱�Ƃ����o�ł��܂��B

	�g�����Q�D

	CMemoryState s;

	s.BeginSnap(16,4096);	//	16�o�C�g�̃u���b�N��4096�p�ӂ���
	{
		//	�������������ɏ���

		//	16�o�C�g�ȉ���new�����������ꍇ�A���̃N���X�̊m�ۂ��Ă���
		//	�G���A����u���b�N�P�ʂŗD�悵�Ďg�p���Ă���

	}
	s.EndSnap();
	s.Dump(&dump);
	//	���蓖�Ă�ꂽ�u���b�N�̂���������̃G���A���������Ȃ��
	//	�f�f�̂��߂ɕ\�������邱�Ƃ��o����

	���@���̏ꍇ�ABeginSnap�`EndSnap�̊Ԃ�new����delete����Ă��Ȃ��悤��
		�C���X�^���X���L���Ă͂����܂���B

	��	s.BeginSnap(16,4096);	//	16�o�C�g�̃u���b�N��4096�p�ӂ���
	�̕�����
		s.BeginSnap(); // 16,4096);	//	16�o�C�g�̃u���b�N��4096�p�ӂ���
	�Ƃ����悤�ɃR�����g�A�E�g���āA���������[�N�����o���Ă����A�Ō��
	���̍����ł�new/delete�ɒu��������Ƃ����g�����������\���������D�D

*/
public:
	void	BeginSnap();
	///	�X�i�b�v���J�n����

	void	BeginSnap(int nBlockSize,int nBlockNum);
	///	�X�i�b�v���J�n����

	void	EndSnap();
	///	�X�i�b�v���I������

	LRESULT	Dump(ITextOutputStream*pDump);
	///	���������[�N�̏󋵂�IDumpContext�ɏo�́B
	///	���[�N���Ȃ���΁A�����o�͂�����0���Ԃ�

	CMemoryState() { m_lpaby = NULL; }
	virtual ~CMemoryState() { Release(); }

	static bool	IsActive() { return g_lpMemoryState!=NULL; }
	///	���̃N���X����������new/delete���I�[�o�[���[�h���Ă���Ƃ���
	///	���̃t���O��true�ɂȂ�̂ŁA���̂Ƃ��́A�R�[���o�b�N�I�u�W�F�N�g��
	///	�d�|����Ȃ�A���Ȃ肷��B
	static void RegistCallBack(const smart_ptr<function_callback>& fn){
		if (g_lpMemoryState!=NULL){
			g_lpMemoryState->GetCallBack()->insert(fn);
		}
	}

public:	//	�ʓ|������public�ł�����O�O�G
	map<LPVOID,size_t>	m_alpMemory;
	//	new ���Ă��郁������

	BYTE*	m_lpaby;			//	��������������ێ�
	int*	m_lpanEmptyBlock;	//	�󂫃u���b�N�Ǘ��p
	int		m_nBlockSize;		//	1�u���b�N�̃T�C�Y(byte)
	int		m_nBlockNum;		//	���̐�������C�Ƀu���b�N�m��
	int		m_nMax;				//	�������u���b�N�̎��ɋ󂢂Ă���ԍ�

	static CMemoryState* g_lpMemoryState;
	//	�����ɃX�i�b�v���ł���

	//	�I������CallBack���郊�X�g
	smart_vector_ptr<function_callback> m_afnCallBack;
	smart_vector_ptr<function_callback>* GetCallBack() { return & m_afnCallBack; }

	CCriticalSection m_vCS;

protected:
	void Release(){
		g_lpMemoryState = NULL; // ���܃J�X�^���o�[�W������new/delte���g���Ă͍���
		if (m_lpaby!=NULL) {
			delete [] m_lpaby;
			delete [] m_lpanEmptyBlock;
			m_lpaby = NULL;
		}
	}
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif

#endif
