#include "stdafx.h"
#include "CApp.h"

class CMyTextDraw {
public:
	CTextDrawBase* GetTextDraw() { return& m_TextDraw; }
	CDIB32* GetDIB() { return m_TextDraw.GetDIB(); }
	int	GetPat() { return m_nPat; } // �`��p�^�[����ێ�����

	CMyTextDraw(void) { Init(); }
	virtual ~CMyTextDraw() {}

	void Init() {
		m_File.Read("data/out.html");
		m_TextContext.SetTextPtr((LPSTR)m_File.GetMemory());
		m_TextContext.m_nWidth = 260;
		m_TextContext.m_nHInterval = 0;
		m_TextContext.SetBaseFontSize(25);
		m_TextContext.m_rgbColorBk = RGB(128,128,128);
		m_TextDraw.SetContext(m_TextContext);
		m_TextDraw.UpdateText();
		m_nPat = -1;
		CheckTag();	//	���m�̃^�O�`�F�b�N
	}

	void Update(){
		m_TextDraw.GoNextContext();
		m_TextDraw.UpdateText();
		CheckTag();	//	���m�̃^�O�`�F�b�N
	}

	void CheckTag(){
		//	CTextDraw�̃T�|�[�g���Ă��Ȃ��^�O���E���A
		//	���̏������s�Ȃ�

		//	�����ł́A���ۂɁA�A�j���p�^�[�����w�肷��<PAT n>�^�O�A
		//	�W�����v����������<Goto label��>�^�O�A
		//�@�W�����v��̃��x�����`����<JumpLabel label��>�^�O��
		//	�ǂݍ��݂��������Ă݂�B

		vector<LPSTR>::iterator it = (*m_TextDraw.GetTagList()).begin();
		while (it!=(*m_TextDraw.GetTagList()).end()){
			LPSTR lpsz = *it;
			//	<PAT n>�^�O���H
			if (CStringScanner::IsToken(lpsz,"PAT")){
				CStringScanner::GetNum(lpsz,m_nPat); // m_nPat�Ɍ㑱�̐�����ǂݍ���
			}ef(CStringScanner::IsToken(lpsz,"GOTO")){
			// <GOTO label>�^�O���T�|�[�g����ɂ́A�������
				{
					string label;
					label = CStringScanner::GetStr(lpsz,'>');	//	���^�O�܂ł̃��x�����擾
					m_TextDraw.SetTextOffset(0);	//	�e�L�X�g�擪����A���̃��x����T��
					while (true) {
						if (m_TextDraw.UpdateText(false)!=0) break;	//	�J���ǂ݂��āAjumplabel�^�O��T��
						vector<LPSTR>::iterator it = (*m_TextDraw.GetTagList()).begin();
						while (it!=(*m_TextDraw.GetTagList()).end()){
							LPSTR lpsz = *it;
							if (CStringScanner::IsToken(lpsz,"JUMPLABEL")){
								string label2;
								label2 = CStringScanner::GetStr(lpsz,'>');	//	���^�O�܂ł̃��x�����擾
								if (label == label2) {
									//	��v�����̂ŁA�����Ń��^�[������
									*m_TextDraw.GetNextContext() = *m_TextDraw.GetContext();
									return ;
								}
							}
							it ++;
						}
						m_TextDraw.GoNextContext();
					}
					return ; // �I��
				}
			} else {
			//	����ȊO�̃^�O�͖���
			}
			it++;
		}
	}

protected:
	CFile m_File;
	CTextDrawContext m_TextContext;
	CTextDrawDIB32A m_TextDraw;	//	�A���`�F���t���o�[�W����
	int	m_nPat;		//	�L�����N�^�[�`��p�^�[����ێ�����
};

void	CApp::MainThread(void) {				 //	 ���ꂪ���s�����
	GetDraw()->SetDisplay(false);

	CDIB32 dib[4];
	dib[0].Load("data/Mie_00.yga"); // 0:�ʏ�
	dib[1].Load("data/Mie_01.yga"); // 1:�����ڂ�
	dib[2].Load("data/Mie_02.yga"); // 2:�Ղ�
	dib[3].Load("data/Mie_03.yga"); // 3:���ւ�

//	CDIB32 bg;
//	bg.Load("data/logo.jpg");

	CMyTextDraw textdraw;

	CFPSTimer fps;
	fps.SetFPS(30);	//	�e�L�g�[��redraw���ĂȂ��Ⴂ�O�O�G
	CKey key;

	CRootCounter rct(0,255,4);
	rct.SetReverse(true);

	while (IsThreadValid()){
		key.Input();
		if (key.IsVKeyPress(0)) break;	//	ESC�L�[�ŏI�������
		GetKey()->Input();
		if (GetKey()->IsVKeyPush(5)) {	//	�X�y�[�X�L�[�œǂݐi�߂�
			textdraw.Update();
		}
		RECT rc;
		::SetRect(&rc,0,0,280,480);
		GetDraw()->Clear(RGB(255,255-rct,rct),&rc);
		rct.Inc();
		::SetRect(&rc,280,0,640,480);
		GetDraw()->Clear(RGB(0,0,0),&rc);
//		GetDraw()->BltFast(&bg,0,0);
		if (textdraw.GetPat()!=-1) {
			GetDraw()->BlendBltFastAlpha(&dib[textdraw.GetPat()],0,0);
		}
		{
			int sx,sy;
			textdraw.GetDIB()->GetSize(sx,sy);
			GetDraw()->BlendBltFastAlpha(textdraw.GetDIB(),340,(480-sy)/2);
		}
		GetDraw()->OnDraw();
		fps.WaitFrame();
	}
}

//	���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {	//	�A�v���P�[�V�����N���X����h��
	virtual void MainThread(void){			//	���ꂪ���[�J�[�X���b�h
		CApp().Start();						//	CApp app; app.Start();�̈Ӗ���
	}
};

//	���킸�ƒm�ꂽWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CAppInitializer::Init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	�K�������Ă�
	CSingleApp sapp;
	if (sapp.IsValid()) {
		CAppMainWindow().Run();					//	��Œ�`�������C���̃E�B���h�D���쐬
	}
	return 0;
}
