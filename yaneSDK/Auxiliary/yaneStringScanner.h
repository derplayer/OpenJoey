//
//	�������̓A�V�X�g�֐�
//

#ifndef __yaneStringScanner_h__
#define __yaneStringScanner_h__


namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class CStringScanner {
/**
	������̉�͂̃T�|�[�g�����܂��B
	class CTextDraw,class CScenarioView	��
	���ۂɎg���Ă���̂ŁA���̎���������ƎQ�l�ɂȂ�ł��傤�B
*/
public:
static	bool	IsToken(LPCSTR &lp,LPCSTR lp2);
	///	�g�[�N���`�F�b�N
	///	��v��́A���̕�����lp�͑O�i����

static	LRESULT	SkipTo(LPCSTR &lp,LPCSTR lp2);
	///	�w��g�[�N���܂œǂݔ�΂�
	///	��v��́A���̕�����lp�͑O�i����B'\0'�ɏo���킵�����0���Ԃ�

static	LRESULT	SkipTo2(LPCSTR &lp,LPCSTR lp2,LPSTR lp3);
	///	�w��g�[�N���܂œǂݔ�΂�
	///	��v��́A���̕�����lp�͑O�i����B'\0'�ɏo���킵�����0���Ԃ�
	///	lp3(buf�͌Ăяo�����ŗp�ӂ��邱��)�ɂ́A�X�L�b�v����܂ł̕����񂪓���

static	LRESULT SkipSpace(LPCSTR &lp);
	///	�X�y�[�X�A�^�u�A���s��؂�l�߂�
	///	'\0'�ɏo���킵�����0���Ԃ�

static	LRESULT SkipSpace2(LPCSTR &lp);
	///	�X�y�[�X�A�^�u�A���s�A�J���}��؂�l�߂�
	///	'\0'�ɏo���킵�����0���Ԃ�

static	LRESULT	GetStrNum(LPCSTR &lp,int& nRate);
	///	"+1"�Ƃ���������Ȃ��nRate�ɂ�1���Ԃ�

static	LRESULT	GetNum(LPCSTR &lp,int& nVal);
	///	+1�Ƃ���������Ȃ��nVal�ɂ�1���Ԃ�

static	LRESULT GetStrColor(LPCSTR &lp,COLORREF& m_nFontColor);
	/// "#ff0000"�Ƃ���������Ȃ��RGB(255,0,0)���Ԃ�

static	string GetStr(LPCSTR &lp,CHAR c);
	///	c�ɑ�������Ƃ���܂ł̕������Ԃ�

static	string GetStr(LPCSTR &lp);
	///	�X�y�[�X�A�^�u�A���s�ɑ�������܂ł̕������Ԃ�

static	string GetStrFileName(LPCSTR &lp);
	///	�^�u�A���s�ɑ�������܂ł̕������Ԃ�

static	string GetNextStr(LPCSTR &lp);
///	�ŏ��ɑ��������X�y�[�X�A�^�u�A���s�𖳎����A���̂��ƃX�y�[�X�A�^�u�A
///	���s�ɑ�������܂ł̕������Ԃ�
///	�������A"�@�Ŏn�܂��Ă���ꍇ�́A�ēx"�ɑ�������܂ł����̕�����Ƃ��ĕԂ��B
///	�i���̂Ƃ�" �͎����I�Ɏ�菜�����j

static LRESULT GetNumFromCsv(LPCSTR &lp,int& nVal,bool bSkip=false);
///	�J���}�A�X�y�[�X�𖳎����Ȃ��琔�l��ǂݍ��ނ̂�
static string GetStrFromCsv(LPCSTR &lp);
///	�J���}�A�X�y�[�X�𖳎����Ȃ��當�����ǂݍ��ނ̂��B
/**
	�J���}�A�X�y�[�X�𖳎����Ȃ��琔�l��ǂݍ��ށB
	CSV�Ƃ����̂́AMicrosoft Excel�Ȃǂŏo�͂ł���J���}��؂�̃e�L�X�g�`���B
	bSkip == true�̏ꍇ�AGetNumFromCsv/GetStrFromCsv���������Ƃ͕K������,��
	�����Ȃ�܂��B(bSkip==false�Ȃ�΁A�����Ȃ�܂���)
	1,2,3�ň��Get�����2,3�ɂȂ�܂��B
	,,2,3�Ȃ�΁A�G���[���Ԃ��l�Ƃ��ĕԂ�A����,�������Ȃ�A,2,3�ƂȂ�܂��B

	GetNumFromCsv��GetStrFromCsv�́A
	/�̓Z�p���[�^�Ƃ݂Ȃ��悤�ɂ��܂����B�i�M�E�ցE�L�j�����
	("//"�ł̃R�����g����������)
*/

static void	Replace(string& buf,LPCSTR pSrc,LPCSTR pDst,bool bCaseSensitive=true);
///	string���Ɋ܂܂��pSrc�ƈ�v���镔����pDst�ɒu������
///	���{��i�}���`�o�C�g�����j�Ή��BbCaseSensitive==true�̂Ƃ��́A
///	�A���t�@�x�b�g�̑啶������������ʂ���B

///		�����ƕ�����̑��ݕϊ��֐�
///		�擪�̃X�y�[�X�͖������Ȃ�
static LRESULT	StringToNum(string s,LONGLONG& l);
static void NumToString(LONGLONG l,string& s);
static string NumToString(LONGLONG l) { string s; NumToString(l,s); return s;}
//	��������́A�[���T�v���X���āAn���ɂ���B
static void NumToStringZ(LONGLONG l,string& s,int n);
static string NumToStringZ(LONGLONG l,int n) { string s; NumToStringZ(l,s,n); return s;}

static	int	CharToHex(CHAR c);
/*
	'0'-'9','A'-'F','a'-'f'���A�����ɕϊ�����
	�͈͊O�̐������w�肳���΁ACRuntimeException��O�𓊂���
*/

static	CHAR HexToChar(int n);
/**
	0-15���A'0'-'9','A'-'F'�ɕϊ�����
	�͈͊O�̐������w�肳���΁ACRuntimeException��O�𓊂���
*/

static LRESULT Find(LPCSTR &lpSrc,LPCSTR lpMoji);
/**
	lpSrc�Ŏ�����镶����o�b�t�@(�I�[��'\0'�Ɖ���)����lpMoji�Ŏ������
	�������T���B���������ꍇ�́A0���Ԃ�BlpSrc�|�C���^�́A���̕�����
	�擪�܂Ői�ށB�݂���Ȃ������ꍇ�́AlpSrc��'\0'���w��
*/

///	�}���`�o�C�g���ʗp�֐�
static bool IsLead_ShiftJIS(CHAR c);
static bool IsTrail_ShiftJIS(CHAR c);
};

class CLineParser {
/**
	��s�H�킹�āA�������͂���
*/
public:

	void	SetLine( LPCSTR lpszStr );				//	1���C���̕�������Z�b�g����
	bool	IsMatch( LPCSTR lpszCmpStr );			//	�w�肵��������Ɠ��������ۂ���Ԃ�
	LRESULT	GetNum( int& nNo );						//	���l�𓾂�
	LRESULT	GetStr( string& str );					//	"..."�ň͂܂ꂽ������𓾂�

	static void	ConvertCR(LPSTR);					//	�u\n�v��'\n'�ɕϊ�

	CLineParser();									//	�R���X�g���N�^
	virtual	~CLineParser();							//	�f�X�g���N�^

protected:
	void	Garbage();								//	�S�~��
	LPSTR	m_lpszReadData;							//	1���C����ǂݍ��񂾃f�[�^
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
