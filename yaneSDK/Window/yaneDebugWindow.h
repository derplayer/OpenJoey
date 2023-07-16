//	yaneDebugWindow.h :
//		programmed by yaneurao	'00/02/25

#ifndef __yaneDebugWindow_h__
#define __yaneDebugWindow_h__


#include "../AppFrame/yaneAppBase.h"
#include "../Auxiliary/yaneStream.h"

namespace yaneuraoGameSDK3rd {
namespace Window {

class CDebugWindow : public CAppBase , public ITextOutputStream {
/**
	�G���[���O�̏o�͂̂��߂̃E�B���h�D�N���X�B
	class CAppBase ����h���������A�v���N���X�Ȃ̂ŁARun()�֐����Ăяo���A
	���𐶐����Ă���g�����ƁB�܂��ACAppBase����h�������Ă���̂ŁA
	class CAppFrame ��MainThread�̂Ȃ��Ŏg�����ƁB

	�f�o�b�O�̂Ƃ��̕ϐ��̃E�I�b�`���Ɏg���Ȃ�΁A
	class CDbg �̂ق����g���ق����f�R�֗��B

	�g�p��F
	CDebugWindow dbg;
	dbg.Run();
�@	dbg.Out("This may be a bug...");
*/
public:
	virtual void	Clear();						//	��������N���A����
	virtual void __cdecl Out(LPSTR fmt, ... );		//	�������\������
	virtual void	Out(int);						//	������\������
	virtual void	Out(const string&);
	virtual void	Out(LONG*);		//	�ψ��������o�[�W����
protected:
	virtual LRESULT OnPreCreate(CWindowOption&);		//	overriden from CAppBase
};

//	�f�o�b�O�E�B���h�D�̃��[�J���C�Y��
class CDbg : public ITextOutputStream {
/**
	CDebugWindow���A���[�J���C�Y�����o�[�W�����B
	CDebugWindow�Ɠ��������o�֐������B
	���̃N���X�̏ꍇ�ARun()�͕s�v�ŁAOut�����Ƃ��ɕK�v�Ƃ����
	�����I�Ƀf�o�b�O�E�B���h�D���J���B�f�o�b�O�p�r�ɂ́A���ꂪ�֗��B

	��
	CDbg().Out("x = %d",x);
*/
public:
	//	�֐��C���^�[�t�F�[�X�͓����B���ׂ�CDebugWindow�ɈϏ����Ă��܂�
	virtual void	Clear()					{
		CCriticalLock guard(GetCriticalSection());
		CheckValid(); GetWindow()->Clear();
	}

	virtual void __cdecl Out(LPSTR fmt, ... );
	virtual void	Out(int i);
	virtual void	Out(const string&s);
	virtual void	Out(LONG*lpl);
	
	static void	SelectWindow(int nDebugWindowNo){
		//if (0<=nDebugWindowNo && nDebugWindowNo<10) m_nTarget = nDebugWindowNo;
		SetTargetWindowNo(nDebugWindowNo);
	}
	/**
		�f�o�b�O�E�B���h�D�̃i���o�[���w�肷��B0�`9�Bdefault�ł�0�B
		������ύX���邱�ƂŁA�o�͐��ύX�ł���B
		�i�����̃f�o�b�O�E�B���h�D���쐬�ł���B�j
	*/
	static int	GetTargetWindowNo() {
		CCriticalLock guard(GetCriticalSection());
		return m_nTarget;
	}
	static void	SetTargetWindowNo(int nDebugWindowNo) {
		CCriticalLock guard(GetCriticalSection());
		if (0<=nDebugWindowNo && nDebugWindowNo<10) m_nTarget = nDebugWindowNo;
	}

protected:
	void	CheckValid();
	CDebugWindow* GetWindow(){ return m_lpDebugWindow[m_nTarget]; }

	static	CDebugWindow* m_lpDebugWindow[10];
		//	�ő�10�i�΁j
	static	int m_nTarget;	//	���ݑΏۂƂ��Ă���f�o�b�O�E�B���h�D

	static	CCriticalSection	m_cs;
	static	CCriticalSection*	GetCriticalSection() { return &m_cs;}
	//	���Ȃ�₩�₷��Ƃ��́A�r������p

	//	����ACAppManager����I�����ɃR�[���o�b�N���Ă��炤
	static void		Release(int nTarget);
};

} // end of namespace Window
} // end of namespace yaneuraoGameSDK3rd

#endif
