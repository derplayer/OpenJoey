#include "stdafx.h"
#include "yaneFlagDebugWindow.h"

void CFlagDebugApp::MainThread(){
	GetDraw()->SetDisplay();
	GetDraw()->GetSecondary()->SetFillColor(ISurface::makeRGB(255,255,255));

	GetPlane()->Load(GetDebugBmpFile());

	int nStart = 0x0000;
	const int nWidth = 4;
	const int nHeight = 11;
	int nSelected = 0;

	bool bEdit = false; //�@�G�f�B�b�g����\���t���O
	int nEditNum = 0;   //�@�G�f�B�b�g���̐���
	bool bEditSigned = false; // �G�f�b�g���̐����̕���

	int nNot = 10;		//	�f�B�t�H���g10�i���\�L

	CFPSTimer timer;
	timer.SetFPS(10);	//	���ꂭ�炢�̃��X�|���X�ł����񂿂Ⴄ�́H

	while (IsThreadValid()){

		GetDraw()->GetSecondary()->Clear();
		{
			int x,y,mb;
			GetMouse()->GetInfo(x,y,mb);
			HDC hdc = GetDraw()->GetSecondary()->GetDC();
			if (hdc!=NULL){
				CFont font;
				font.SetSize(12);
				font.SetColor(RGB(16,0,0));
				int nLastSelected = nSelected;
				if ((x<100) || (x>=50*nWidth+100) ||
					(y<0)   || (y>=16*nHeight)){
					nSelected = -1;
				} else {
					nSelected = nStart + ((x-100)/50) + (y/16)*nWidth;
				}
				if (nSelected!=nLastSelected){
					bEdit = false;	//	�}�E�X���ړ����Ă���Δj���I
				}
				int* p = m_funcFlag(nSelected);
				if (p!=NULL) {
					string s;
					s = "�t���ONo." + CStringScanner::NumToStringZ(nSelected,4)+
						"\n����:" + m_funcFlagExplanation(nSelected);
					font.SetText(s);
					font.OnDraw(hdc,0,(nHeight-1)*16+20);
					DrawNum(*p,0,(nHeight)*16+30,10,10); // 10�i���\�L
					DrawNum(*p,200,(nHeight)*16+30,8,16); // 16�i���\�L
					DrawNum(*p,150,(nHeight+1)*16+30,16,2); // 2�i���\�L
					DrawNum(*p>>16,0,(nHeight+1)*16+30,16,2); // 2�i���\�L
				}

				font.SetSize(20);
				font.SetColor(RGB(255,0,0));

				font.SetText("��\n��\n��\n��\n16\n10\n8");
				font.OnDraw(hdc,300,0);

				if (x>=300 && x<320 && y>=0 && y<=160 && mb){
					int nCommand = (y-0)/20;
					switch (nCommand) {
					case 0 : nStart -= 40; break;
					case 1 : nStart -= 4; break;
					case 2 : nStart += 4; break;
					case 3 : nStart += 40; break;
					case 4 : nNot = 16; break;
					case 5 : nNot = 10; break;
					case 6 : nNot = 8; break;
				//	case 7 : nNot = 2; break;
					}
					if (nStart<0) nStart = 0;
				}
				
				GetDraw()->GetSecondary()->ReleaseDC();
			}
		}
		{
			for(int j=0;j<nHeight;j++){
				DrawNum(nStart + j*nWidth , 20 ,j*16 + 0 ,4,nNot);
				for(int i=0;i<nWidth;i++){
					int nIndex = nStart + i + j*nWidth;
					int* p = m_funcFlag(nIndex);
					if (p==NULL) goto LoopExit;

					int x = i*50+80+20;
					int y = j*16 + 0;
					/*
					if (nIndex == nSelected){
						//	�I���ʒu���
						ISurfaceRGB rgb = GetDraw()->GetSecondary()->GetFillColor();
						GetDraw()->GetSecondary()->SetFillColor(ISurface::makeRGB(0,0,255));
						RECT rc = { x,y,x+40,y+16 };
						GetDraw()->GetSecondary()->Clear(&rc); // BG��ŏ���
						GetDraw()->GetSecondary()->SetFillColor(rgb);
					}
					*/		//	������͔����F�������Ȃ邩�D�D
					int nViewNum,nUp,nL,nPre;
					if (nNot<=10){
						nPre = 5;
					} else {
						nPre = 4;
					}
					if (nIndex == nSelected && bEdit){
					//	�G�f�B�b�g���Ȃ̂ŃG�f�B�b�g���̐�����\��
						nViewNum = nEditNum*(bEditSigned?-1:1);
						nUp = 2; nL = 9;
					} else {
						nViewNum = *p;
						nUp = 1; nL = 0;
					}
					DrawNum(nViewNum , x-nL*nPre, y ,nPre*nUp,nNot);
					if (nIndex == nSelected){
						//	�I���ʒu�����F����
						RECT rc = { x,y,x+40,y+16 };
						if (nNot==10){
							rc.right += 9;
						}
						if (!bEdit){
							GetDraw()->GetSecondary()->SubColorFast(ISurface::makeRGB(0,0,100),&rc);
						} else {
							rc.left -= nL*nPre;
							GetDraw()->GetSecondary()->AddColorFast(ISurface::makeRGB(255,255,255),&rc); // ���ɂ���
							DrawNum(nViewNum , x-nL*nPre, y ,nPre*nUp,nNot);
							GetDraw()->GetSecondary()->AddColorFast(ISurface::makeRGB(255,0,0),&rc);
						}
					}
				}
			}
LoopExit:;
		}

		//	�L�[���͂��ď�������
		GetInput()->Input();
		{
			int n = -1;
			if (GetInput()->IsKeyPush(DIK_0) || GetInput()->IsKeyPush(DIK_NUMPAD0)) n=0;
			ef (GetInput()->IsKeyPush(DIK_1) || GetInput()->IsKeyPush(DIK_NUMPAD1)) n=1;
			ef (GetInput()->IsKeyPush(DIK_2) || GetInput()->IsKeyPush(DIK_NUMPAD2)) n=2;
			ef (GetInput()->IsKeyPush(DIK_3) || GetInput()->IsKeyPush(DIK_NUMPAD3)) n=3;
			ef (GetInput()->IsKeyPush(DIK_4) || GetInput()->IsKeyPush(DIK_NUMPAD4)) n=4;
			ef (GetInput()->IsKeyPush(DIK_5) || GetInput()->IsKeyPush(DIK_NUMPAD5)) n=5;
			ef (GetInput()->IsKeyPush(DIK_6) || GetInput()->IsKeyPush(DIK_NUMPAD6)) n=6;
			ef (GetInput()->IsKeyPush(DIK_7) || GetInput()->IsKeyPush(DIK_NUMPAD7)) n=7;
			ef (GetInput()->IsKeyPush(DIK_8) || GetInput()->IsKeyPush(DIK_NUMPAD8)) n=8;
			ef (GetInput()->IsKeyPush(DIK_9) || GetInput()->IsKeyPush(DIK_NUMPAD9)) n=9;
			ef (GetInput()->IsKeyPush(DIK_A)) n=10;
			ef (GetInput()->IsKeyPush(DIK_B)) n=11;
			ef (GetInput()->IsKeyPush(DIK_C)) n=12;
			ef (GetInput()->IsKeyPush(DIK_D)) n=13;
			ef (GetInput()->IsKeyPush(DIK_E)) n=14;
			ef (GetInput()->IsKeyPush(DIK_F)) n=15;
			ef (GetInput()->IsKeyPush(DIK_MINUS) || GetInput()->IsKeyPush(DIK_NUMPADMINUS)) bEditSigned = !bEditSigned;

			if (n>=nNot) {
				//	�i���\�L���傫�Ȑ��̓��͖͂���
				n = -1;
			}
			if (bEditSigned && !bEdit){
				bEdit = true;
				nEditNum = 0;
			}
			if (n!=-1) {
				if (bEdit) {
					nEditNum = nEditNum*nNot+n;
				} else {
					bEdit = true;
					bEditSigned = false; // ���Z�b�g
					nEditNum = n;
				}
			}
			if (GetInput()->IsKeyPush(DIK_RETURN)||
				GetInput()->IsKeyPush(DIK_NUMPADENTER))
			{
				if (bEdit) {
					bEdit = false;
					int*p = m_funcFlag(nSelected);
					if (p!=NULL){
						*p = nEditNum * (bEditSigned?-1:1);
					}
					bEditSigned = false;
				}
			}
		}


		GetDraw()->OnDraw();

		timer.WaitFrame();
	}
}

void	CFlagDebugApp::DrawNum(int nNum,int x,int y,int nFigure,int nNot){
	//	������(x,y)�� nFigure���ŕ`�悷��
	bool bSigned = nNum<0; // �}�C�i�X����?
	if (bSigned && nNot==10) { nNum = -nNum; } else { bSigned = false; }
	DWORD nN = nNum; // unsiged�Ōv�Z
	while (nFigure--){
		int n = nN % nNot;
		nN /= nNot;

		RECT rc = { n*9,0,(n+1)*9,15 } ;
		GetDraw()->GetSecondary()->Blt(GetPlane(),x+nFigure*9,y,NULL,&rc);
		if (nN == 0 && bSigned){
			RECT rc = { 16*9,0,(16+1)*9,15 } ;
			GetDraw()->GetSecondary()->Blt(GetPlane(),x+(nFigure-1)*9,y,NULL,&rc);
			break; // �}�C�i�X�����`�悵�ă��^�[��
		}
	}
}
