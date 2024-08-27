#include "stdafx.h"
#include "../../../yaneSDK/yaneSDK.h"

class CApp : public CAppFrame {

	CDirectDraw m_Draw;
	CKey		m_Key;

	CDirectDraw* GetDraw(void)  { return& m_Draw; }
	CKey*		 GetKey(void)	{ return& m_Key;  }

	//	���������C����
    void MainThread(void) {                 //  ���ꂪ���s�����

		GetDraw()->SetDisplay(false);		//	Window���[�h
		GetDraw()->CreateSecondaryDIB();	//	Secondary�Ƃ���DIB�𐶐�

		CDIB32 dib[4];
		dib[0].CreateSurface(640,480);		//	�]���p�o�b�t�@
		dib[1].Load("ar_010a_s.jpg");
		dib[2].Load("sr_011a_s.jpg");
		dib[3].Load("marupe.jpg");

		CFPSTimer t;		//	FPS�����p
		t.SetFPS(30);

		CFPSLayer fps(&t);	//	FPS�l�̕`��

		CTextLayer tl;
		tl.GetFont()->SetSize(20);
		tl.GetFont()->SetText("A Real Time Morphing is Very Cool!!");
		tl.SetPos(100,440);

		CDIB32Morph morpher[3];
		//	�]�����Ɠ]����̑Ή��_������
		morpher[0].Set(0,0,0,0);		//	�S��
		morpher[0].Set(456,0,542,0);
		morpher[0].Set(0,404,0,438);
		morpher[0].Set(456,404,542,438);
		morpher[0].Set(150,6,262,168);	//	��̗֊s�U�_	
		morpher[0].Set(126,245,312,386);
		morpher[0].Set(178,296,396,396);
		morpher[0].Set(280,326,472,374);
		morpher[0].Set(346,226,488,292);
		morpher[0].Set(346,146,476,218);
		morpher[0].SetDiv(542,438,10,10);//	����\
		morpher[0].Calc();				//	�v�Z���Ȃ��Ⴂ�O�O
		//	�ȉ����l
		morpher[1].Set(0,0,0,0);
		morpher[1].Set(542,0,306,0);
		morpher[1].Set(0,438,0,258);
		morpher[1].Set(542,438,306,258);
		morpher[1].Set(262,168,58,140);
		morpher[1].Set(312,386,60,188);
		morpher[1].Set(396,396,100,240);
		morpher[1].Set(472,374,180,236);
		morpher[1].Set(488,292,222,190);
		morpher[1].Set(476,218,76,60);
		morpher[1].SetDiv(542,438,10,10);
		morpher[1].Calc();

		int	nPhase = 0;
		bool bReverse = false;
		int nSpeed = 2;
		
		while(IsThreadValid()) {				//  ���ꂪValid�̊ԁA�܂�葱����
			if (!bReverse){
				nPhase+=nSpeed;
				if (nPhase>=512) {
					bReverse = true;
					nSpeed<<=1;
					if (nSpeed >= 64) {
						nSpeed = 2;
					}
					nPhase -=nSpeed;
				}
			} else {
				nPhase -= nSpeed;
				if (nPhase<=0) {
					bReverse = false;
					nSpeed<<=1;
					if (nSpeed >= 64) {
						nSpeed = 2;
					}
					nPhase +=nSpeed;
				}
			}
			
			GetKey()->Input();
			if (GetKey()->IsVKeyPress(0)) break;		//	ESC�ŏI��

			if   (nPhase<=256) {
				morpher[0].OnDraw(GetDraw()->GetSecondaryDIB(),
					&dib[1],&dib[2],&dib[0],nPhase);
			} ef (nPhase<=512) {
				morpher[1].OnDraw(GetDraw()->GetSecondaryDIB(),
					&dib[2],&dib[3],&dib[0],nPhase-256);
			} else {
//				morpher[1].OnDraw(GetDraw()->GetSecondaryDIB(),
//					&dib[2],&dib[3],&dib[0],nPhase-256);
			}

			GetDraw()->OnDrawDIB();
			t.WaitFrame();
        }
    }
};

//  ���ꂪmain window�̂��߂̃N���X�B
class CAppMainWindow : public CAppBase {    //  �A�v���P�[�V�����N���X����h��
    virtual void MainThread(void){          //  ���ꂪ���[�J�[�X���b�h
        CApp().Start();                     //  CApp app; app.Start();�̈Ӗ���
    }
};

//  ���킸�ƒm�ꂽWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    CAppInitializer::Init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);  //  �K�������Ă�
	CSingleApp sapp;
	if (sapp.IsValid()) {
	    CAppMainWindow().Run();                 //  ��Œ�`�������C���̃E�B���h�D���쐬
	}
    return 0;
}
