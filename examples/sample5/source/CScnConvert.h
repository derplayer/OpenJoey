
//
//	yaneSDK2nd�̃V�i���I�N���X�p�̃V�i���I�R���o�[�g�N���X
//
//					programmed by yaneurao('02/01/03)

#pragma once

class CScnConvert {
public:
	LRESULT Convert(string filename,string outputpath);
	//	����t�@�C�����A�ϊ�����B�o�͐�path�́A�����Őݒ肵�Ă���
	void	Err(string ErrMes);
protected:
	int		m_nLineNo;	//	��͒��̃��C���i���o�[

	//	�y�[�W���̃g�[�^�����o��
	void	OutPageTotal(int nChapter,int nLineNo);
};
