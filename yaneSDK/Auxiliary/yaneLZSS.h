//	LZSS�ɂ�鈳�k���[�`��
//
//	���@�Q�l����
//	  �������F���wC����ɂ��ŐV�A���S���Y�����T�x�^�Z�p�]�_��
//

#ifndef __yaneLZSS_h__
#define __yaneLZSS_h__


namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

const int g_LZSS_RING_BUFFER	= 4096; // ��o�b�t�@�̑傫��
const int g_LZSS_LONGEST_MATCH	= 18;	// �Œ���v��

class ICompress {
/**
	���k�p�C���^�[�t�F�[�X
		LZSS�@�ň��k������� class CLZSS ���Q�Ƃ̂��ƁB
*/
public:
	///	Decode����BDst��̃o�b�t�@��bDst��true�Ȃ�Ίm�ۂ����
	virtual LRESULT Decode(BYTE* lpSrc,BYTE*& lpDst,DWORD  dwSize,bool bDst=true)=0;

	///	Encode����BDst��̃o�b�t�@��bDst��true�Ȃ�Ίm�ۂ����
	virtual LRESULT Encode(BYTE* lpSrc,BYTE*& lpDst,DWORD  dwSize,DWORD& dwDstSize,bool bDst=true)=0;

	/**	����̂́Asmart_ptr���Ƃ�o�[�W����
		lpDst.AddArray(size) ���ă��������m�ۂ��Ă����΁A�����ɓW�J�^���k�����
		lpDst.isNull() == true�Ȃ�΃��������m�ۂ��ĕԂ�
	*/
	virtual LRESULT Decode(BYTE* lpSrc,smart_ptr<BYTE>& lpDst,DWORD	 dwSize)=0;
	virtual LRESULT Encode(BYTE* lpSrc,smart_ptr<BYTE>& lpDst,DWORD	 dwSize,DWORD& dwDstSize)=0;

	virtual ~ICompress(){}
};

class CLZSS : public ICompress {
/**
	LZSS�@�ɂ�鈳�k���[�`���B
	���k�͂��x���ł����A�W�J�̓��`�������̂������ł��B

*/
public:
	//	Decode����BDst��̃o�b�t�@��bDst��true�Ȃ�Ίm�ۂ����
	virtual LRESULT Decode(BYTE* lpSrc,BYTE*& lpDst,DWORD  dwSize,bool bDst=true);
	/**
		lpSrc�́A�W�J���̃o�b�t�@�i���k�f�[�^�j�B
		lpDst�́A�W�J��̃o�b�t�@�i�񈳏k�f�[�^�j�B
		�ibDst==true�Ȃ�΂����Decode���[�`������new�Ŋm�ۂ����̂ŁA
		�W�J���ꂽ�o�b�t�@���g�p�I�����delete [] lpDst����Ηǂ��j
		dwSize�́A�W�J��̃o�b�t�@�i�񈳏k�f�[�^�j�̃T�C�Y�B
	*/

	//	Encode����BDst��̃o�b�t�@��bDst��true�Ȃ�Ίm�ۂ����
	virtual LRESULT Encode(BYTE* lpSrc,BYTE*& lpDst,DWORD  dwSize,DWORD& dwDstSize,bool bDst=true);
	/**
		lpSrc�́A���k���̃o�b�t�@�i�񈳏k�f�[�^�j�B
		lpDst�́A���k��̃o�b�t�@�i���k�f�[�^�j�B
		�ibDst==true�Ȃ�΁A�����Encode���[�`������new�Ŋm�ۂ����̂ŁA
		���k���ꂽ�o�b�t�@���g�p�I����ɂ�delete [] lpDst����Ηǂ��j
		dwSize�́A���k���̃o�b�t�@�T�C�Y�BdwDstSize�́A���k��̃o�b�t�@�T�C�Y�B

		bDst==false�ŌĂяo�����ꍇ�A���ۂ̈��k�͍s�Ȃ�Ȃ��B
		���k��̃T�C�Y�����Ŏ擾����Ƃ��ȂǂɎg���B
		�i���̂Ƃ��́AlpDst�ɂ�dummy�ϐ����A*(BYTE**)NULL�Ȃǂłn�j�ł��B
		���̑��ALPVOID�^�̕ϐ���n�������̂ł���ΎQ�Ɠn���Ȃ̂ŁA
		*(BYTE**)&lpBuf�Ƃ����悤�ɃL���X�g���Ă��������j

		���k�����P�O�O���𒴂���悤�ȏꍇ�ɂ́A���̊֐��͔�O��Ԃ��܂��B
		���̂Ƃ��́A���̃t�@�C���̈��k�͒��߂Ă��������O�O
	*/

	/**	����̂́Asmart_ptr���Ƃ�o�[�W����
		lpDst.SetArray(size) ���ă��������m�ۂ��Ă����΁A�����ɓW�J�^���k�����
		lpDst.isNull() == true�Ȃ�΃��������m�ۂ��ĕԂ�
	*/
	virtual LRESULT Decode(BYTE* lpSrc,smart_ptr<BYTE>& lpDst,DWORD	 dwSize);
	virtual LRESULT Encode(BYTE* lpSrc,smart_ptr<BYTE>& lpDst,DWORD	 dwSize,DWORD& dwDstSize);

protected:
	//	�o�̓o�C�g���J�E���^
	DWORD	m_dwOutCount;

	// �e�L�X�g�p�o�b�t�@
	UCHAR	m_szText[g_LZSS_RING_BUFFER+g_LZSS_LONGEST_MATCH-1];

	//	��
	int		m_dad[g_LZSS_RING_BUFFER+1];
	int		m_lson[g_LZSS_RING_BUFFER+1];
	int		m_rson[g_LZSS_RING_BUFFER+257];

	void	init_tree(void);	//	�؂̏�����
	void	insert_node(int r);	//	�� r ��؂ɑ}��
	void	delete_node(int p);	//	�� p ��؂������

	int		m_matchpos, m_matchlen;	 /* �Œ���v�ʒu, ��v�� */

};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
