//
//	yaneTextDIBitmap.h :
//

#ifndef __yaneTextDIBitmap_h__
#define __yaneTextDIBitmap_h__


#include "yaneFont.h"
#include "yaneDIBitmap.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CTextDIBitmap : public CDIBitmap {
public:

	CFont*	GetFont() { return &m_Font; }
	void	SetTextPos(int x,int y){ m_nTextX=x; m_nTextY=y; }
	void	GetTextPos(int&x,int&y){ x=m_nTextX; y=m_nTextY; }

	/// �c�����̐ݒ�^�擾
	void	SetVertical(bool bVertical = true);
	bool	IsVertical() const { return m_bVertical; }

	LRESULT	UpdateText();

	LRESULT	UpdateTextA();	///	�A���`�F��������o�[�W����
	///	�����l�����悤�ɂȂ�̂ŕ`���ISurface::BlendBltFastAlpha�ōs�Ȃ�
	/**
		����:CDIBitmap��ISurface�h���N���X�ł͂Ȃ��̂�
		���ۂ�CSurfaceInfo��GeneralBlt�œ]�����Ȃ��Ă͂Ȃ�Ȃ�
	*/

	LRESULT	UpdateTextAA();	//	�A���`�F��������o�[�W����
	///	�����l�����悤�ɂȂ�̂ŕ`���ISurface::BlendBltFastAlpha�ōs�Ȃ�

	CTextDIBitmap();
	virtual ~CTextDIBitmap() {};	//	plane holder

protected:
	LRESULT UpdateTextHorizontal();		//�@������ UpdateText
	LRESULT UpdateTextHorizontalA();	//�@������ UpdateTextA
	LRESULT UpdateTextHorizontalAA();	//�@������ UpdateTextA
	LRESULT UpdateTextVertical();		//�@�c���� UpdateText
	LRESULT UpdateTextVerticalA();		//�@�c���� UpdateTextA
	LRESULT UpdateTextVerticalAA();		//�@�c���� UpdateTextA

	CFont		m_Font;				//	�����ŕ`��
	int			m_nTextX,m_nTextY;	//	�e�L�X�g�`��ʒu
	bool		m_bVertical;		//�@�c�������ǂ���
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
