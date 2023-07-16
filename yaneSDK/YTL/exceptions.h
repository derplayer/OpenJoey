/*
	���肪���ȗ�O���`
*/

#ifndef __YTLExceptions_h__
#define __YTLExceptions_h__

#ifdef USE_EXCEPTION

namespace yaneuraoGameSDK3rd {
namespace YTL {

class CException {
/**
	���ׂĂ̗�O�́A��������h��������
*/
public:
	CException(){
#ifdef USE_STOP_EXCEPTION
	MessageBox(NULL,"��O�����������̂Œ�~���܂��I","��O����",MB_OK);
	/**	���̃_�C�A���O���o������ɁAVS����
		�v���Z�X�̃A�^�b�`�����āA�r���h�˃u���[�N
		����΃R�[���X�^�b�N�����Ăǂ��Ŕ���������O������ł���
	*/

	///	���łɃ������G���[���킴�Ɣ���������R�[�h���d�|����
	* LPLONG(0xcdcdcdcd) = 0;
	///	WinNT/2000�n�Ȃ�΁A�킴�Ƃ��̃������G���[�𔭐������āA
	///	�f�o�b�O�����ق��������葁��
#endif
	}
	virtual string getError() const { return ""; };
};

class CIndexOutOfBoundsException : public CException {
///		�z��͈͊O�ւ̃������ւ̃A�N�Z�X��O
///		class smart_ptr ���g�p���Ă���Ƃ��Ƀ`�F�b�N�@�\�ɂ�蔭��
public:
	virtual string getError() const {
		return "�z��͈͊O�ւ̃������ւ̃A�N�Z�X";
	}
};

class CNullPointerException : public CException {
///		Null pointer����̃A�N�Z�X��O
///		class smart_ptr ���g�p���Ă���Ƃ��Ƀ`�F�b�N�@�\�ɂ�蔭��
public:
	virtual string getError() const {
		return "Null pointer����̃A�N�Z�X";
	}
};

class CRuntimeException : public CException {
///		�͈͊O�̐������w�肳��Ă���Ƃ��̃A�N�Z�X��O
public:
	CRuntimeException(const string& strErrorName="") : m_str(strErrorName) {
		if (m_str.empty()){
			m_str = "���s����O�i�͈͊O�̐��l���w�肳��Ă���)";
		}
	}
	virtual string getError() const {
		return m_str;
	}
protected:
	string m_str;
};

/////////////////////////////////////////////////////////////////
//	�X���b�h��O

class CInterruptedException : public CException {
///		wait/sleep�őҋ@���̃X���b�h�ɑ΂���
///		interrupt���Ăяo�����Ƃ��̗�O�icf.Java�̗�O�j
public:
	virtual string getError() const {
		return "InterruptedException��O������";
	}
};

class CIllegalMonitorStateException : public CException {
///		���b�N�������Ă��Ȃ��X���b�h��wait,notify,notifyAll
///		���Ăяo�����Ƃ��̗�O�icf.Java�̗�O�j
public:
	virtual string getError() const {
		return "CIllegalMonitorStateException��O������";
	}
};

/////////////////////////////////////////////////////////////////

class CSyntaxException : public CException {
///		��ʓI�ȕ��@�G���[�ɑ΂���A�N�Z�X��O
public:
	CSyntaxException(const string& strErrorName) : m_str(strErrorName) {}	
	virtual string getError() const {
		return m_str;
	}
protected:
	string m_str;
};

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif	//	USE_EXCEPTION

#endif
