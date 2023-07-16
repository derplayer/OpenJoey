//
//	yaneTextFastPlane.h :
//
#ifdef USE_FastDraw

#ifndef __yaneTextFastPlane_h__
#define __yaneTextFastPlane_h__


#include "yaneFont.h"
#include "yaneFastPlane.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CTextFastPlane : public CFastPlane {
public:

	CFont*	GetFont() { return &m_Font; }
	void	SetTextPos(int x,int y){ m_nTextX=x; m_nTextY=y; }
	LRESULT	UpdateText();
	LRESULT	UpdateTextA();	//	�A���`�F��������o�[�W����
										//	�����l�����悤�ɂȂ�̂ŕ`���BlendBltFastAlpha�ōs�Ȃ�
	LRESULT	UpdateTextAA(); //	���ׂȃA���`�F����������o�[�W����
										//	UpdateTextAA��肸���Ԃ�x���O�O�G�G
	void	SetVertical(bool bVertical = true) { m_bVertical = bVertical; }
	bool	IsVertical() { return m_bVertical; }
	CTextFastPlane(CFastDraw* pFastDraw=NULL);
	virtual ~CTextFastPlane() {};	//	plane holder

protected:
	CFont		m_Font;				//	�����ŕ`��
	int			m_nTextX,m_nTextY;	//	�e�L�X�g�`��ʒu
	bool		m_bVertical;		//�@�c�������ǂ���

	virtual		LRESULT OnDraw();	//	���X�g�A�p�֐�
	int			m_nUpdateType;		//	���X�g�A����Ƃ��̎��
									//	0:���X�g�A���Ȃ� 1:UpdateText 2:UpdateTextA 3:UpdateTextAA
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif	//	__yaneTextFastPlane_h__ 

#endif // USE_FastDraw
