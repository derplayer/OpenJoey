//////////////////////////////////////////////////////////////////////////////
//
//	�t���O�f�o�b�O�pGUI
//
//		programmed by yaneurao('02/11/06)

#ifndef __yaneFlagDebugWindow_h__
#define __yaneFlagDebugWindow_h__

class CFlagDebugApp : public CAppFrame {
public:

	///	�^����ꂽ�t���O�ԍ��ɑ΂��āA���̃t���O�ւ̃|�C���^��Ԃ��֐�(delegate)
	void SetFlagDelegate(const delegate<int*,int>& func)
		{ m_funcFlag = func;}

	///	�^����ꂽ�t���O�ԍ��ɑ΂��āA���̃t���O�̐����Ԃ��֐�(delegate)
	void SetFlagExplanationDelegate(const delegate<string,int>& func)
		{ m_funcFlagExplanation = func;}

	/**
		����L�̂ӂ��́A�������͈͊O�̐����ł���΁A
			NULL��Ԃ��悤�ɃR�[�f�B���O���Ă�������
	*/

	string	GetDebugBmpFile() const { return m_strDebugBmp; }
	void	SetDebugBmpFile(const string& str) { m_strDebugBmp=str; }

protected:
	virtual void MainThread();

	CFastDraw* GetDraw() { return m_vDraw.GetDraw(); }
	CPlane GetPlane() const { return m_plane;}
	CMouse* GetMouse() { return& m_mouse;}
	CKeyInput* GetInput() { return& m_input; }

	void	DrawNum(int nNum,int x,int y,int nFigure,int bNot=16);
	//	������(x,y)�� nFigure���ŕ`�悷�� nNot�i���ŕ\�L
	
private:
	delegate<int*,int> m_funcFlag;
	//	�^����ꂽ�t���O�ԍ��ɑ΂��āA���̃t���O�ւ̃|�C���^��Ԃ��֐�(delegate)

	delegate<string,int> m_funcFlagExplanation;
	//	�^����ꂽ�t���O�ԍ��ɑ΂��āA���̃t���O�̐����Ԃ��֐�(delegate)

	CFastPlaneFactory m_vDraw;
	string m_strDebugBmp;
	CPlane m_plane;
	CMouse m_mouse;
	CKeyInput m_input;
};

//	���ꂪmain window�̂��߂̃N���X�B
class CFlagDebugWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
	string m_strDebugBmp;
	virtual void MainThread(){			   //  ���ꂪ���[�J�[�X���b�h
		flagdebugger.Add();
		flagdebugger->SetDebugBmpFile(m_strDebugBmp);
		/*
				������new���Ȃ��Ƃ����Ȃ��B
				������new�����A���̃N���X��CFlagDebugWindow��ۗL����ƁA
				���̃N���X�𐶐�����X���b�h��CFlagDebugWindow�𐶐����邱�ƂɂȂ�A
				CFlagDebugWindow�ŗ��p���Ă���CFastPlaneFactory�̃R���X�g���N�^��
				��������X���b�h���ACFlagDebugWindow::MainThread�����s���Ă���X���b�h��
				�قȂ邱�ƂɂȂ�̂ł܂���
			*/
		flagdebugger->SetFlagDelegate(m_funcFlag);
		flagdebugger->SetFlagExplanationDelegate(m_funcFlagExplanation);
		flagdebugger->Start();
		flagdebugger.Delete();
			/*
				�����āAMainThread���甲����܂łɉ�̂��Ă����Ȃ���΁A
				CFastDraw�����b�Z�[�W���t�b�N����֌W��A�܂���
			*/
	}
	virtual LRESULT OnPreCreate(CWindowOption& opt){
		opt.caption = "FlagDebugWindow";
		opt.style = WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
		opt.size_x = 320; opt.size_y = 240;
		opt.bCentering = false;
		return 0;
	}
	smart_ptr<CFlagDebugApp> flagdebugger;
	delegate<int*,int> m_funcFlag;
	delegate<string,int> m_funcFlagExplanation;
public:
	CFlagDebugApp* GetFlagDebugger() { return flagdebugger.get(); }
	void	SetDebugBmpFile(const string& str) { m_strDebugBmp = str; }
		///	����������bmp��p�Ӂ�����̂�

	CFlagDebugWindow() { SetDebugBmpFile("debug_nums.bmp"); }
		//	�f�B�t�H���g�̃t�@�C����ݒ肵�Ƃ���...

	///	�^����ꂽ�t���O�ԍ��ɑ΂��āA���̃t���O�ւ̃|�C���^��Ԃ��֐�(delegate)
	void SetFlagDelegate(const delegate<int*,int>& func)
		{ m_funcFlag = func;}

	///	�^����ꂽ�t���O�ԍ��ɑ΂��āA���̃t���O�̐����Ԃ��֐�(delegate)
	void SetFlagExplanationDelegate(const delegate<string,int>& func)
		{ m_funcFlagExplanation = func;}

	/**
		����L�̂ӂ��́A�������͈͊O�̐����ł���΁A
			NULL��Ԃ��悤�ɃR�[�f�B���O���Ă�������
	*/
};

#endif
