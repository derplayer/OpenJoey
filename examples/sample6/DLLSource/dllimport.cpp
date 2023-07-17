// dllimport.cpp : DLL �A�v���P�[�V�����p�̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

//	�P�DDllMain�͉������Ȃ��ėǂ�
BOOL APIENTRY DllMain( HANDLE hModule, 
					   DWORD  ul_reason_for_call, 
					   LPVOID lpReserved
					 )
{
	return TRUE;
}

//	�Q�D���̃t�@�C����include����
#include "yaneObjectCreater.h"

//	�R�DDLL����export����N���X�̃C���^�[�t�F�[�X��p�ӂ���
class IPlugInTest {
public:
	virtual int Calc(int n)=0;
	virtual ~IPlugInTest(){}
};

//	�S�D���̃C���^�[�t�F�[�X�ɑ΂��Ď������s�Ȃ�
class CPlugInTest : public IPlugInTest {
	virtual int Calc(int n){
		//	1����n�܂ł̍��v���v�Z����
		int nTotal = 0;
		for(int i=1;i<=n;i++){
			nTotal+=i;
		}
		return nTotal;
	}
};

//	�T�DDLL������export����N���X��factory��o�^����
void	YaneRegistPlugIn(IObjectCreater*p){
	p->RegistClass("CPlugInTest",new factory<CPlugInTest>);
}
