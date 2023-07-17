//	yaneApp.h :
//		application thread class
//			programmed by yaneurao	'00/02/25

#ifndef __yaneAppBase_h__
#define __yaneAppBase_h__

#include "../Window/yaneWinHook.h"
#include "../Window/yaneWindow.h"
#include "../Thread/yaneThread.h"
#include "../Timer/yaneIntervalTimer.h"

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

class IAppBase {
/**
	�A�v���P�[�V�������N���X�̃C���^�[�t�F�[�X
*/
public:
	virtual bool	IsThreadValid()=0;
	virtual void	InvalidateThread()=0;
	//	����CAppBase�ł�CThread::InvalidateThread()�̌Ăяo���ɉ�����

	virtual IWindow* GetMyWindow()=0;
	virtual HWND	GetHWnd() const=0;
	virtual LRESULT	Run()=0;
	virtual void	Close()=0;
	virtual IIntervalTimer* GetIntervalTimer()=0;

////////////////////////////////////////////////////////////////
//	main�Ƃ�sub�Ƃ������T�O���̂��i���Z���X�Ȃ̂�
//	�����̃��\�b�h�͔p�~�̕����ŁD�D

	///		����g�����ȁH
///	static	IAppBase* GetMainApp()	 { return m_lpMainApp; }
	/// ���C���A�v���ւ̃|�C���^��Ԃ�

///	static	HWND	GetMainWnd()	 { return GetMainApp()->GetHWnd(); }
	/// ���C���A�v���ۗ̕L����E�B���h�D�n���h����Ԃ�

	virtual bool	IsMainApp() const =0;
	virtual void	SetMainApp(bool b) =0;

	///	���[�J�[�X���b�h�̎擾
	virtual IThread* GetWorkerThread()=0;

////////////////////////////////////////////////////////////////

	virtual ~IAppBase(){}

protected:
	//	static IAppBase* m_lpMainApp;
	//	���C���E�B���h�D�i����̏I�����v���O�����I���Ƃ݂Ȃ��j
};

class CAppBase : public IWinHook,public IAppBase , public CThread {
/**
	�A�v���P�[�V�������N���X

	�A�v���P�[�V�����́A��������h��������Ɨǂ��B

	���Ȃ킿�ACAppBase �Ƃ� �X���b�h���� + CWindow + ���b�Z�[�W�|���v

	���̃N���X�̃C���X�^���X�𕡐��쐬���邱�Ƃɂ���āA
	�}���`�E�B���h�D�������ł���B

	���@�E�B���h�D��������Ƃ��̏������L�q����ɂ�

	WM_CLOSE���Ăяo�����ƁAOnPreClose���Ăяo����܂��B
	���ꂪ��0��Ԃ��΁AWM_CLOSE�͏�������܂���B
	�i�E�B���h�D�͕����܂���j
	�����āA���̂Ƃ��ɂ��̃E�B���h�D����邩�ǂ��������[�U�[��
	�₢���킹��悤�ȏ��������āA�����邱�Ƃ��m�肵����A
	IAppBase::Close���Ăяo���܂��B��������΁A�E�B���h�D�͕����܂��B

*/
public:

	virtual LRESULT Run();
	/**
�@	���̊֐����Ăяo�����Ƃɂ���đ�����������A
	���[�J�[�X���b�h(virtual void MainThread()�����s����)�𐶐����܂��B
	�������������̂�҂��Ă��琧���Ԃ��܂��B

	���̊֐����Ăяo������CreateThread�Œ��ڃX���b�h���N�����Ă����܂��܂���B
	���̏ꍇ�́A���̐���������҂����ɐ����Ԃ��܂�
	*/

	virtual HWND	GetHWnd() const { return m_oWindow.GetHWnd(); }
	///	�E�B���h�D�n���h���擾

	virtual IWindow* GetMyWindow() { return &m_oWindow; }
	///	�E�B���h�D�N���X�擾

	bool	IsMainApp() const { return m_bMainApp; }
	void	SetMainApp(bool b){ m_bMainApp = b; }
	/**
		����CAppBase�����C���A�v���ł��邩��ݒ肷��
		default��false
		�����true�ɂ��Ă����΁A���̃A�v�����I������Ƃ���
		���̃X���b�h�����ׂ�invalidate����i�悤��WM_QUIT�𔭍s����j
	*/

	bool	IsMessage() const { return m_bMessage; }
	///	���b�Z�[�W���[�v�X���b�h�͐����Ă��邩

	///	�X���b�h�̐������`�F�b�N overriden from CThread
	virtual bool	IsThreadValid();
	///	�ˁ@�����̂Ȃ��Ƀ��b�Z�[�W���[�v�������Ɨǂ�

	///	�X���b�h��Invalidate�B
	virtual void	InvalidateThread();	//	overriden from CThread
	/**
		�X���b�h���̂𖳌��ɂ���BIdle���[�h�œ��삵�Ă���X���b�h�́A
		Window���b�Z�[�W�����Ȃ�����A�������܂܂Ȃ̂ŁA���������ꍇ�ɂ́A
		�����I��WM_CLOSE�𑗂��Ă��K�v������B
		���̊֐��́A������������������B���Ƃ��΁ACDebugWindow�́A
		�ʏ�AIdle���[�h�œ��삵�Ă���B
	*/

	///	�E�B���h�D�ŏ������ɑҋ@���邩
	volatile bool*	GetWaitIfMinimized() {	return &m_bWaitIfMinimized; }
	/**
		IsThreadValid�Ăяo�����ɁA�E�B���h�D���ŏ�������Ă���΂��̂܂�
		�ҋ@���邩�H��bool�t���O���擾����B
			�g�p�ၨ�@ *GetWaitIfMinimized( ) = true;
	*/

	///	Idle���[�h(default:false)�ɂ���̂�
	bool* GetIdle() { return &m_bIdle;}
	/**
		�t���O�̃|�C���^���Ԃ�̂ŁAIdle���[�h�ɂ���ɂ́A
		*GetIdle( ) = true;�̂悤�ɂ���B
		Idle���[�h�ɂ���ƁAIsThreadValid��Window���b�Z�[�W����������Ƃ��ɁA
		Window���b�Z�[�W�������݂��Ȃ��Ƃ��́A���̃X���b�h�͖������܂܂ɂȂ�B
		�i�b�o�t�p���[��H��Ȃ��j
		Idle���[�h�ɂȂ��Ă���X���b�h���E���ɂ́A
		���̃N���X��InvalidateThread���Ăяo�����ƁB
	*/

	///	OnPreClose�����������ɋ����I��WM_CLOSE�����s����
	virtual void	Close();

	///	���[�J�[�X���b�h�̎擾
	virtual IThread* GetWorkerThread()
		{ return this; }
	/**
		���̃t���[�����[�N�ɂ́A���b�Z�[�W�|���v�͑��݂���
		���[�J�[�X���b�h�݂̂ŁA����CAppBase���̂��A
		CThread�̔h���N���X�Ȃ̂ŁA��������̂܂ܕԂ��Ă����Ηǂ��B
	*/

	CAppBase();
	virtual ~CAppBase();

protected:
	//////////////////////////////////////////////////////////////////////////
	///	�p������Ƃ������Ƃ��邩���i�΁j�֐��̈ꗗ

	///	���ꂪ���s�X���b�h�Ȃ̂Ōp�����āA������p�ӂ��ĂˁI
	///	�E�B���h�D�쐬��(OnCreate()�̂���)�ɌĂяo�����́B
	virtual void MainThread() {
		while (IsThreadValid())
			IThread::getThread()->sleep(20);
			//�@�������Ă����΃X���b�h�C���^���v�g�ɑ΂��鉞������������
	}

	//	���̑����K�X�I�[�o�[���C�h���Ă˂�
	virtual LRESULT OnInit()	{ return 0; }		///	��������
	virtual LRESULT OnPreCreate(CWindowOption& opt);///	�E�B���h�D�쐬���O
	virtual LRESULT OnCreate()	{ return 0; }		///	�E�B���h�D�쐬����
	virtual LRESULT OnDestroy() { return 0; }		///	�I�����O

	virtual LRESULT OnPreClose() { return 0; }
	///	WM_CLOSE�����O�ɌĂяo����A������0��Ԃ���WM_CLOSE�͎��s����Ȃ�
	///	���̏󋵉��ɂ����ăE�B���h�D��CLOSE�����邽�߂ɂ́AClose���Ăяo������

	///	brief Window Message to override
	virtual void OnPaint()		{ }

	IIntervalTimer*	GetIntervalTimer();
	/**
		�C���^�[�o���^�C�}�̎擾
		IsThreadValid���Ăяo�����Ƃ��ɁA���̃R�[���o�b�N��������
	*/

	//////////////////////////////////////////////////////////////////////////
protected:
	CWindow m_oWindow;			//	�ۗL���Ă���window
	bool	m_bMessage;			//	���b�Z�[�W���[�v�X���b�h�͐����Ă��邩

	virtual void	ThreadProc();					//	override from CThread
	virtual LRESULT WndProc(HWND,UINT,WPARAM,LPARAM);	//	override in CWinHook
	void	InnerStopThread();

	volatile bool	m_bWaitIfMinimized;		//	�E�B���h�D�ŏ������ɑҋ@���邩

	bool	m_bClose;			//	OnPreClose���Ă΂���WM_CLOSE���������邩�H
	bool	m_bIdle;			//	Idle���[�h
	HACCEL	m_hAccel;			//	�A�N�Z�����[�^

	CIntervalTimer	m_vIntervalTimer;
	volatile bool m_bMainApp;		//	���C���A�v���Ȃ̂�

private:
	static	DWORD WINAPI RunPrepare(LPVOID lpVoid); //	Run����̃W�����v��
	LRESULT RunThread();						//	�E�B���h�D�̍쐬��WorkThread�̍쐬��MessageLoop
	LRESULT MessagePump(bool bPeek=true);			//	MessagePomp
	//	bPeek==true�Ȃ��Peek���āA���b�Z�[�W�������Ȃ�܂ŏ�������
	//	bPeek==false�Ȃ�΁A���b�Z�[�W���ЂƂł���������܂ł��̊֐����甲���Ȃ�

	volatile LONG	m_nThreadStatus;				//	�X���b�h�̃X�e�[�^�X
};

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

#endif
