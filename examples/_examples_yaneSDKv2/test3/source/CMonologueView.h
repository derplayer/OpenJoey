//
//	�wRevolution�x	2000/07	 Waffle 
//		programmed by yaneurao(M.Isozaki)
//

#ifndef __CMonologueView_h__
#define __CMonologueView_h__

#include "CApp.h"			//	���C���A�v��

class CMonologueView {
public:
	//	���m���[�O��ʕ`��
	virtual LRESULT OnDraw(void);

	//	�ǂݍ���
	virtual	LRESULT Load(LPSTR lpszFile);

	bool	IsEof(void) { return m_bEof; }

	CMonologueView(CApp* lpApp);
	virtual ~CMonologueView();

protected:
	CApp*	GetApp(void) { return m_lpApp; }
	CApp*	m_lpApp;
	bool	m_bEof;
	bool	m_bFadeIn;			//	�t�F�[�h�C�����H
	bool	m_bFadeOut;			//	�t�F�[�h�A�E�g���H
	CTimer	m_Timer;
	void	GetNextMessage(void);
	virtual	void OnDrawBG(int nFadeLevel=256);
	bool	m_bBlack;
	CFile	m_File;
	CSound	m_BGMSound;

	CPlane		m_BGPlane[3];		//	BG*3
	int			m_nPosX,m_nPosY;	//	BG�\�����W
	int			m_nScrollSpeed;		//	BG�X�N���[���X�s�[�h
	CTextPlane	m_TextPlane[32];	//	�z��Text
	int			m_nTextPos;			//	�e�L�X�g�\���|�W�V����
	int			m_nTextNext;		//	���Ɏg��m_TextPlane
	int			m_nTextLast;		//	�ŏ��̃e�L�X�g
	int			m_nMsgSpeed;		//	���b�Z�[�W�̃X�s�[�h
	CPlaneFadeBlt m_FadeBlt;		//	�t�F�[�hBlter
	void	SetText(LPSTR);			//	�e�L�X�g�̐ݒ�
	int			m_nReadMessageCount;//	���̃��b�Z�[�W

	CPlane		m_CutPlane;			//	Cut in/out����v���[��
	int			m_nCutCount;		//	���̃J�E���^
	int			m_nCutX,m_nCutY;

	int			m_nEnd;				//	�I���J�E���^
};

#endif
