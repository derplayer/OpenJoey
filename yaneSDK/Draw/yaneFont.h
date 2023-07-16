//	yaneFont.h :
//
//		Font Wrapper
//

#ifndef __yaneFont_h__
#define __yaneFont_h__


namespace yaneuraoGameSDK3rd {
namespace Draw {

class CFont {
/**
	�����t�H���g�̂��߂̃N���X
		class CTextDIBitmap
		class CTextFastPlane
	���Q�l�ɂ��邱��
*/
public:

	///	---- �e�L�X�g�̐ݒ�ƕ`��

	void	SetText(const string& s);
	/// �\��������̎w��i\n�ŉ��s���g�p�ł���j

	void	__cdecl SetText(LPSTR fmt, ... );
	///	���Ɠ����Bprintf�o�͏������g����B
	///	������buf��512�����܂�

	///	������\��������Ƃ��Ďw��
	void	SetText(int i);

	///	�\��������̎擾
	string	GetText() { return m_String; }

	/**
		�`��T�C�Y�𓾂�
		���̃��\�b�h�́A�v���p�e�B�ݒ��ɌĂяo���Ɨǂ�
	*/
	LRESULT	GetSize(int& sx,int& sy);

	///	HDC��n���āA�����ɕ`�悵�Ă��炤
	void	OnDraw(HDC hdc,int x=0,int y=0);

	///	------	property	�i�����̉E�ɂ��銇�ʐ����́A�f�B�t�H���g�l�j

	///	�t�H���g�N�I���e�B				(0�j
	void	SetQuality(int nQuality);
	int		GetQuality() const { return m_nQuality; }

	/// Font�T�C�Y��pt��(�|�C���g��)�Őݒ�/�擾			(16)
	void	SetSize(int nSize);
	int		GetSize() const { return m_nSize; }

	/// �e�L�X�g�J���[�ݒ�^�擾			(255,255,255)
	void	SetColor(COLORREF rgb);
	COLORREF GetColor() const { return m_nRgb; }

	/// �����̉e��						(64,64,64)
	void	SetBackColor(COLORREF rgb);
	COLORREF GetBackColor() const { return m_nBkRgb; }

	/// �w�i�F							(CLR_INVALID)
	void	SetBGColor(COLORREF rgb);
	COLORREF GetBGColor() const { return m_nBGRgb; }

	/// ���s�܂ł̍s��					(20)
	void	SetHeight(int nHeight);
	int		GetHeight() const { return m_nHeight; }

	/// �t�H���g�Z���N�g�i�ԍ��Łj		(0) : MS�S�V�b�N
	void	SetFont(int nFontNo);

	/// �t�H���g�Z���N�g�i�t�H���g���Łj
	void	SetFont(const string& fontname);
	/**
		�������A�c�����t�H���g(@)�́A����Őݒ肷��̂ł͂Ȃ��A
		SetVerticalFont���\�b�h�ŏc�����ݒ肷��
	*/

	/**
		�ݒ肳��Ă���t�H���g���̎擾
		�������ASetVerticalFont(true)�ŁA�c�����t�H���g��
		�I������Ă���ꍇ�́A�t�H���g���̐擪�� '@' ��
		�t�^�������̂�Ԃ�
	*/
	string	GetFont() const;

	/// �����̑���						(300:FW_LIGHT) , FW_BOLD��700
	void	SetWeight(int nWeight);

	/// �Α�								(false)
	void	SetItalic(bool b);

	/// �����̗L��						(false)
	void	SetUnderLine(bool b);

	/// �ł��������̗L��					(false)
	void	SetStrikeOut(bool b);

	/// �e�̃I�t�Z�b�g				(2,2)
	void	SetShadowOffset(int nOx,int nOy);
	void	GetShadowOffset(int &nOx,int &nOy) const { nOx=m_nShadowOffsetX; nOy=m_nShadowOffsetY; }

	/**
		�c�����t�H���g��ݒ�(default:false)
		@ ��擪�ɒǉ�����
	*/
	void	SetVerticalFont(bool b) { m_bVerticalFont = b; }
	bool	IsVerticalFont() const { return m_bVerticalFont; }

	CFont();
	virtual ~CFont();

protected:
	string	m_String;			//	�\�����镶����
	int		m_nQuality;			//	�t�H���g�N�I���e�B
	COLORREF m_nRgb;			//	�����F
	COLORREF m_nBkRgb;			//	�����̉e
	COLORREF m_nBGRgb;			//	�w�i�F
	int		m_nSize;			//	�I������Ă���t�H���g�T�C�Y
	string	m_FontName;			//	�I������Ă���t�H���g��
	int		m_nHeight;			//	���̍s�܂ł̋���
	int		m_nWeight;			//	�����̑���
	bool	m_bItalic;			//	�Α�
	bool	m_bUnderLine;		//	����
	bool	m_bStrikeOut;		//	�ł�������
	bool	m_bVerticalFont;	//	�c�����t�H���g�̑I��

	int		m_nShadowOffsetX;	//	�e�̃I�t�Z�b�gX���W
	int		m_nShadowOffsetY;	//	�e�̃I�t�Z�b�gY���W

	//	Auxiliary
	void	TextOut(HDC hdc,int x,int y,const string& s);
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
