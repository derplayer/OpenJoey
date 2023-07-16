#include "stdafx.h"
#include "yaneStringMap.h"
#include "yaneStringScanner.h"
#include "yaneFile.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

void	CStringMap::Serialize(ISerialize&s){
	//	m_vStringMap���܂邲��serialize
	int n;
	if (s.IsStoring()){
		n = GetMap()->size();
		s << n; // �T�C�Y��˂����ނȂ��[�O�O

		mymap::iterator it;
		for(it = GetMap()->begin();it!=GetMap()->end();++it){
			s << *const_cast<string*>(&it->first);
			s << *const_cast<string*>(&it->second);
		}

	} else {
		s << n;		//	�����𕜌�����Ȃ��
		GetMap()->clear();	//	��������N���A����Ȃ��

		for(int i = 0; i<n ; i++){
			string s1,s2;
			s << s1;
			s << s2;
			GetMap()->insert(pair<string,string>(s1,s2));
		}
	}
}


//	�L�[�ɑ΂���f�[�^�̏�������
void	CStringMap::Write(const string& strKey,const string& strData){
	mymap::iterator it;
	it = GetMap()->find(strKey);
	if (it==GetMap()->end()){
		//	����key�͑��݂��Ȃ�
		//	�قȁA�}���������[
		GetMap()->insert(pair<string,string>(strKey,strData));
	} else {
		//	�㏑���㏑���㏑���`��(���ށ�)o
		it->second = strData;
	}
}

void	CStringMap::Write(const string& strKey,LONGLONG nData){
	string s;
	CStringScanner::NumToString(nData,s);
	Write(strKey,s);
}

//	�L�[�ɑ΂���f�[�^�̒ǉ���������
void	CStringMap::Append(const string& strKey,const string& strData){
	mymap::iterator it;
	it = GetMap()->find(strKey);
	if (it==GetMap()->end()){
		//	����key�͑��݂��Ȃ�
		//	�قȁA�}���������[
		GetMap()->insert(pair<string,string>(strKey,strData));
	} else {
		//	�ǉ��ǉ��ǉ��`��(���ށ�)o�@�Ă̓X�C�J�`��(���ށ�)o
		it->second += strData;
		//	string�̒ǉ��Ȃ�ŁA����ł�����ł���H
	}
}

void	CStringMap::Append(const string& strKey,LONGLONG nData){
	string s;
	CStringScanner::NumToString(nData,s);
	Write(strKey,s);
}

//	�L�[�ɑ΂���f�[�^�̓ǂݏo��
LRESULT	CStringMap::Read(const string& strKey,string& strData){
	mymap::iterator it;
	it = GetMap()->find(strKey);
	if (it==GetMap()->end()){
		//	����key�͑��݂��Ȃ�
		strData.erase();
		return -1;
	}
	strData = it->second;
	return 0;
}

LRESULT	CStringMap::Read(const string& strKey,LONGLONG& nData){
	string s;
	LRESULT lr = Read(strKey,s);
	if (lr!=0) {
		nData = 0; // fail safe�΍�
		return lr;
	}
	return CStringScanner::StringToNum(s,nData);
}

//	�L�[�ɑ΂���f�[�^�̒��ڎ擾
	//	(���̃L�[�����݂��Ȃ��Ƃ��́AstrDefault or lDefault���Ԃ�)
string	CStringMap::GetStr(const string& strKey,const string& strDefault){
	string s;
	Read(strKey,s);
	if (s.empty()) {
		return strDefault; // �G���[����szDefault��Ԃ�
	}
	return s;
}

int CStringMap::GetNum(const string& strKey,int nDefault){
	LONGLONG l;
	if (Read(strKey,l)!=0) return nDefault; // error����nDefault��Ԃ�
	return (int)l;
}

LONGLONG	CStringMap::GetLongNum(const string& strKey,LONGLONG nDefault){
	LONGLONG l;
	if (Read(strKey,l)!=0) return nDefault; // error����nDefault��Ԃ�
	return l;
}

//	�L�[�ɑ΂���f�[�^�̍폜
bool	CStringMap::Delete(const string& strKey){
	//	�폜�����Ƃ��ɂ�true
	return GetMap()->erase(strKey)!=0;
}

//	�܂邲�ƍ폜
void	CStringMap::Clear(){
	//	clear all data..
	GetMap()->clear();
}


//	��`�t�@�C������f�[�^��ǂݏo���āA�i�[����@�\
//	(�V���A���C�Y�Ƃ͈Ⴂ�A����ȃt�H�[�}�b�g�̃t�@�C������ǂݏo��)
LRESULT	CStringMap::ReadFromConfigFile(const string& strFileName){
	CFile file;
	LRESULT lr = file.Read(strFileName);
	if (lr!=0) return lr;	//	�ǂݍ��߂Ė��������..

	Clear();	//	�f�[�^�̃N���A

	string buf;
	string strKey,strData;
	while (true){
		LRESULT lr = file.ReadLine(buf);
		if (lr==1) break;
		if (buf.empty()) continue;
		if (buf.size()>=2 && buf[0] == '/' && buf[1] =='/') continue; // �R�����g�s���..
		if (buf.size()>=1 && buf[0]=='#') {	//	�ݒ�s���..
			LPCSTR psz = buf.c_str()+1;
			if (CStringScanner::SkipSpace(psz)!=0) continue;
			strKey = CStringScanner::GetStr(psz);
			if (CStringScanner::SkipSpace(psz)!=0) continue;
			strData = CStringScanner::GetStr(psz);
			if (strData.empty()) continue ; // �f�[�^����ւ�ŁH
			Write(strKey,strData);
		}
	}
	return 0;
}

LRESULT CStringMap::WriteToConfigFile(const string& strFileName){
	CFile file;
	if (file.Open(strFileName.c_str(),"w")!=0) return 1;

	mymap::iterator it;
	for(it = GetMap()->begin();it!=GetMap()->end();++it){
		file.Write(
			string("#") + (it->first) + " " + (it->second) + '\n'
		);
	}
	return 0;
}

//	����������̘A�z�L���Œu������
void	CStringMap::Replace(string& s,bool bCaseSensitive){
	//	�u�����Ȃ���̕\��
	mymap::iterator it;
	for(it = GetMap()->begin();it!=GetMap()->end();++it){
		CStringScanner::Replace(s,
			static_cast<string>(it->first).c_str(),
			static_cast<string>(it->second).c_str(),
			bCaseSensitive);
	}
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
