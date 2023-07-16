#include "stdafx.h"
#include "yaneSerialize.h"
#include "yaneStringScanner.h"
#include "yaneFile.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

void	CSerialize::Clear(){
	m_abyData.clear();	//	�X�g���[���̃N���A
	m_nDataPos = 0;		//	�擪���w������
	m_bStoring = true;	//	�i�[������
}

//	����Ȃ�ł����񂩂��ȁD�D
ISerialize& CSerialize::operator << (IArchive& vData){
	vData.Serialize(*this);
	return *this;
}

//	--- for storing or unstoring primitive data
ISerialize& CSerialize::operator << (BYTE& byData){
	if (IsStoring()){
	//	�ۑ�����񂩂��ȁH
		GetData()->push_back(byData);
	} else {
	//	���o���񂩂��ȁH
		byData = (*GetData())[m_nDataPos];
		m_nDataPos++;
	}
	return *this;
}

ISerialize& CSerialize::operator << (int& nData){
	if (IsStoring()){
	//	�ۑ�����񂩂��ȁH
		//	�T�C�Y�s���Ɖ��肵���R�[�f�B���O�O�O�G
		int nSize = sizeof(int);
		BYTE* pByte = (BYTE*)&nData;
		for(int i=0;i<nSize;i++){
			GetData()->push_back(pByte[i]);
		}
	} else {
	//	�X�g���[��������o���񂩂��ȁH
		//	�T�C�Y�s���Ɖ��肵���R�[�f�B���O�O�O�G
		int nSize = sizeof(int);
		BYTE* pByte = (BYTE*)&nData;
		for(int i=0;i<nSize;i++){
			pByte[i] = (*GetData())[m_nDataPos++];
		}
	}
	return *this;
}

ISerialize& CSerialize::operator << (bool& bData){
	if (IsStoring()){
	//	�ۑ�����񂩂��ȁH
		//	�T�C�Y�s���Ɖ��肵���R�[�f�B���O�O�O�G
		int nSize = sizeof(bool);
		BYTE* pByte = (BYTE*)&bData;
		for(int i=0;i<nSize;i++){
			GetData()->push_back(pByte[i]);
		}
	} else {
	//	�X�g���[��������o���񂩂��ȁH
		//	�T�C�Y�s���Ɖ��肵���R�[�f�B���O�O�O�G
		int nSize = sizeof(bool);
		BYTE* pByte = (BYTE*)&bData;
		for(int i=0;i<nSize;i++){
			pByte[i] = (*GetData())[m_nDataPos++];
		}
	}
	return *this;
}

ISerialize& CSerialize::operator << (string& strData){
	if (IsStoring()){
	//	�ۑ�����񂩂��ȁH
		//	�T�C�Y�s���Ɖ��肵���R�[�f�B���O�O�O�G
		BYTE* pByte = (BYTE*)strData.c_str();
		int nSize = strData.size() + 1;
		//	string�����Ɏ�������Ă��NULL�͊܂܂ւ�˂�..
		//	���ꂵ�����Astring�̃G�������g��char���Ɖ��肵�Ă���̂�
		//	�{���͖�肪����..���̂ւ�unicode�Ή��̂Ƃ��ɏC�����ׂ�

		//	NULL�̃o�C�g���܂߂�K�v������
		for(int i=0;i<nSize;i++){
			GetData()->push_back(pByte[i]);
		}
	} else {
	//	�X�g���[��������o���񂩂��ȁH
		//	�T�C�Y�s���Ɖ��肵���R�[�f�B���O�O�O�G
		int nSize = ::strlen((LPCSTR)&(*GetData())[m_nDataPos]) + 1;
		strData = (LPCSTR)(&(*GetData())[m_nDataPos]);
		m_nDataPos += nSize;
	}
	return *this;
}

ISerialize& CSerialize::operator << (vector<int>& anData){
	int n;
	if (IsStoring()) {
		n = anData.size();
		(*this) << n;	//	�T�C�Y�����łɕۑ����Ȃ��ẮI
	} else {
		(*this) << n;	//	�����𕜌�
		anData.resize(n);
	}
	for(int i=0;i<n;i++){
		(*this) << anData[i];
	}
	return *this;
}

ISerialize& CSerialize::operator << (vector<bool>& abData){
	int n;// = abData.size();
// �ǉ�
	if (IsStoring()) {
		n = abData.size();
		(*this) << n;	//	�T�C�Y�����łɕۑ����Ȃ��ẮI
	} else {
		(*this) << n;	//	�����𕜌�
		abData.resize(n);
	}
// �ǉ��I���
	bool b;
	for(int i=0;i<n;i++){
	//	vector<bool>�̎����́A�o�C�g�f�[�^�ł͂Ȃ��Abit�̃p�b�N�ł���̂ŁA
	//		(*this) << b;�̂悤�Ȃ��Ƃ͏o���Ȃ��B(STLPort)
		if (IsStoring()) {
			b = abData[i];
			(*this) << b;
		} else {
			(*this) << b;
			abData[i] = b;
		}
	}
	return *this;
}

ISerialize& CSerialize::operator << (vector<BYTE>& abyData){
	int n;
	if (IsStoring()) {
		n = abyData.size();
		(*this) << n;	//	�T�C�Y�����łɕۑ����Ȃ��ẮI
	} else {
		(*this) << n;	//	�����𕜌�
		abyData.resize(n);
	}
	for(int i=0;i<n;i++){
		*this << abyData[i];
	}
	return *this;
}

ISerialize& CSerialize::operator << (vector<string>& astrData){
	int n;
	if (IsStoring()) {
		n = astrData.size();
		(*this) << n;	//	�T�C�Y�����łɕۑ����Ȃ��ẮI
	} else {
		(*this) << n;	//	�����𕜌�
		astrData.resize(n);
	}
	for(int i=0;i<n;i++){
		*this << astrData[i];
	}
	return *this;
}

// �ǉ�
ISerialize& CSerialize::operator << (vector<WORD>& awData){
	int n;
	if (IsStoring()) {
		n = awData.size();
		(*this) << n;	//	�T�C�Y�����łɕۑ����Ȃ��ẮI
	} else {
		(*this) << n;	//	�����𕜌�
		awData.resize(n);
	}
	for(int i=0;i<n;i++){
		*this << awData[i];
	}
	return *this;
}

ISerialize& CSerialize::operator << (vector<DWORD>& adwData){
	int n;
	if (IsStoring()) {
		n = adwData.size();
		(*this) << n;	//	�T�C�Y�����łɕۑ����Ȃ��ẮI
	} else {
		(*this) << n;	//	�����𕜌�
		adwData.resize(n);
	}
	for(int i=0;i<n;i++){
		*this << adwData[i];
	}
	return *this;
}
// �ǉ��I���

//	�������g���I�O�O�G
ISerialize& CSerialize::operator << (ISerialize& vData){
	//	vector<BYTE>�Ȃ̂ŁA���Ȃ��V���A���C�Y�ł���
	*this << *vData.GetData();
	*this << *vData.GetStoring(); //m_bStoring;	//	�i�[����
	*this << *vData.GetPos();	//	m_nDataPos;	//	�|�W�V����
	return *this;
}

//	�����āA�V���A���C�Y�Ԃ̃R�s�[�I
ISerialize& CSerialize::operator = (ISerialize& vSeri){
	//	vector�̃R�s�[�Ȃ̂ŁAstd::vector�ɂ��C���O�O�G
	m_abyData  = *vSeri.GetData();	//	m_abyData;
	m_bStoring = *vSeri.GetStoring(); // m_bStoring;
	m_nDataPos = *vSeri.GetPos(); // m_nDataPos;
//	������R�s�[����ق��������悤�ȋC�����邪�A�A�A
//	m_bStoring = true;
//	m_nDataPos = 0;
//	�����Z�b�g���Ă����̂��D�D
	return *this;
}

LRESULT CSerialize::Save(const string& filename){
	//	�X�g���[���̃t�@�C���ւ̕ۑ�
	CFile file;
	return file.Save(filename,GetData());
	//	return CFile().Save(...)�̂悤��
	//	�e���|�����I�u�W�F�N�g�ŏ����Ă��ǂ����A
	//	VC++6.0���āAreturn�̂Ƃ���ł���������A
	//	�o�O���Ă��悤�ȋC������O�O�G > �T�[�r�X�p�b�N���ĂĂȂ���
}

LRESULT CSerialize::Load(const string& filename){	//	�X�g���[���̃t�@�C������̕���
	//	�X�g���[������̃t�@�C���̓ǂݏo��
	CFile file;
	return file.Load(filename,GetData());
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
