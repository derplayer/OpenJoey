#ifndef __yaneStringMap_h__
#define __yaneStringMap_h__


#include "yaneSerialize.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class IStringMap {
public:
	typedef map<string,string> mymap;

	virtual void	Write(const string& strKey,const string& strData)=0;
	virtual void	Write(const string& strKey,LONGLONG nData)=0;
	virtual void	Append(const string& strKey,const string& strData)=0;
	virtual void	Append(const string& strKey,LONGLONG nData)=0;
	virtual LRESULT	Read(const string& strKey,string& strData)=0;
	virtual LRESULT	Read(const string& strKey,LONGLONG& nData)=0;
	virtual string	GetStr(const string& strKey,const string& strDefault="")=0;
	virtual int		GetNum(const string& strKey,int nDefault=0)=0;
	virtual LONGLONG	GetLongNum(const string& strKey,LONGLONG nDefault=0)=0;
	virtual bool	Delete(const string& strKey)=0;
	virtual void	Clear()=0;
	virtual LRESULT ReadFromConfigFile(const string& strFileName)=0;
	virtual LRESULT WriteToConfigFile(const string& strFileName)=0;
	virtual void	Replace(string& s,bool bCaseSensitive=false)=0;
	virtual mymap* GetMap()=0;

	virtual ~IStringMap(){}
};

//	string to string�̘A�z�L���^�W��
class CStringMap : public IStringMap , public IArchive {
/**
�L�[�i������j�ƃf�[�^�Ƃ̘A�z�L����񋟂��܂��B
Perl�Ŏg���悤�ȁA�Ȃ�ł�����ϐ����~���������̂ō��܂����B
CGI�����Ƃ��Ƃ��ɕ֗��ł��B

�����I�ɂ́Amap<string , string >�Ƃ��Ď������Ă���܂��B

class IArchive �h���N���X�Ȃ̂ŁA���̂܂܃V���A���C�Y���邱�Ƃ��o���܂��B
*/
public:

	///	�L�[�ɑ΂���f�[�^�̏�������
	virtual void	Write(const string& strKey,const string& strData);
	virtual void	Write(const string& strKey,LONGLONG nData);
	///		�ˁ@���̃L�[�����ɑ��݂���ꍇ�́A�㏑���B

	///	�L�[�ɑ΂���f�[�^�̒ǉ���������
	virtual void	Append(const string& strKey,const string& strData);
	virtual void	Append(const string& strKey,LONGLONG nData);
/**
�ˁ@string�o�[�W�����i�O�ҁj�̂ق��́A�����񂪉��Z�����BLONGLONG�o�[�W�����i��ҁj�̏ꍇ�́AWrite�Ɠ�������B
*/

	///	�L�[�ɑ΂���f�[�^�̓ǂݏo��
	virtual LRESULT	Read(const string& strKey,string& strData);
	virtual LRESULT	Read(const string& strKey,LONGLONG& nData);

	///	�L�[�ɑ΂���f�[�^�̒��ڎ擾
	///	(���̃L�[�����݂��Ȃ��Ƃ��́AstrDefault or lDefault���Ԃ�)
	virtual string	GetStr(const string& strKey,const string& strDefault="");
	virtual int		GetNum(const string& strKey,int nDefault=0);
	/**	�������LONGLONG�̂������̂����i�ȉ��́j�A����́A�Ƃ��ǂ�
			VC++��release�o�[�W�������Ɠ����R���p�C���G���[��
			�Ȃ�Ƃ�������B�����炭�ALONGLONG�̍œK���̃o�O�B
	*/
	virtual LONGLONG	GetLongNum(const string& strKey,LONGLONG nDefault=0);

	///	�L�[�ɑ΂���f�[�^�̍폜
	virtual bool	Delete(const string& strKey);
	///		�폜�����Ƃ��ɂ�true

	///	�܂邲�ƍ폜
	virtual void	Clear();	//	clear all data..

	///	��`�t�@�C������f�[�^��ǂݏo���āA�i�[����@�\
	///	(�V���A���C�Y�Ƃ͈Ⴂ�A���̂悤�ȃt�H�[�}�b�g�̃t�@�C������ǂݏo��)
	/**
		// �_�u���X���b�V���s�̓R�����g
		#string1 string2 // ���̂悤��#�ŊJ�n���āA������P�C�Q���Ȃ�ׂ�
		//	���̂Ƃ��A�X�y�[�X���Z�p���[�^�B
		��������ƁAstring1���AstrKey,string2��szData�Ƃ��āA
		�ۑ�(Write)�����B
		����āA��`�t�@�C������ǂݍ��݌�AGetStr("string1");�̂悤�ɂ����
		string2���擾�ł���ƌ����킯�I��(���ށ�)o
	*/
	virtual LRESULT ReadFromConfigFile(const string& strFileName);

	///	���̌`���̃t�@�C���Ƃ��ď����o���o�[�W����
	virtual LRESULT WriteToConfigFile(const string& strFileName);


	//	�f�[�^�܂邲�ƕ\��(�f�o�b�O�p)
	void	Out(){
		mymap::iterator it;
		printf ("StringMap::Out() \n");
		for(it = GetMap()->begin();it!=GetMap()->end();it++){
			printf("%s => %s\n",(static_cast<string>(it->first).c_str())
							   ,(static_cast<string>(it->second).c_str()));
		}
	}

	///	����������̘A�z�L���Œu������
	virtual void	Replace(string& s,bool bCaseSensitive=false);

	///	�A�z�L�����̂��擾����B
	virtual mymap* GetMap() { return& m_vStringMap; }

/*	//	����ς��A����Ȃ̗p�ӂ�����A�z��A�N�Z�X����₱�����Ȃ��ł�߁A�A
	//	�A�z�L�[�̒��ڎw��I�y���[�^
	const string operator [] (string strKey);
	string operator [] (string strKey);
*/

protected:
	///	override from IArchive
	virtual void Serialize(ISerialize&s);

	mymap m_vStringMap;
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
