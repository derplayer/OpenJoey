#include "stdafx.h"

//	�P�DyaneConfig.h��
//	#define COMPILE_YANE_PLUGIN_DLL	//	plug-in DLL���쐬����̂��H
//	��L���ɂ���

//	�Q. yaneConfig��#define NOT_USE_DEFAULT_DLLMAIN
//	�𖳌��ɂ���(�f�B�t�H���g�Ŗ���)�@���̃V���{�����`����΃��[�U�[�Ǝ���
//	DllMain���������Ƃ��o����B���̂Ƃ���yaneObjectCreater.h�ɂ���DllMain��
//	�Q�l�ɏ����Ă��������B

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
//	���@static�ȕϐ�����new���K�v�Ȃ��̂́A������new����B
void	YaneRegistPlugIn(IObjectCreater*p){
	p->RegistClass("CPlugInTest",new factory<CPlugInTest>);	//	factory��o�^���܂���
	//	�������œo�^���Ă���N���X��static�ȃI�u�W�F�N�g�������āA���̏��������������Ȃ�΁A
	//	������new���邩�A���邢�́A����factory��h�������āA
	//	�R���X�g���N�^�ŁA����static�ȃI�u�W�F�N�g�|�C���^��NULL�Ȃ��
	//	new���āA�f�X�g���N�^�ŁA���̃I�u�W�F�N�g�|�C���^����NULL�Ȃ�delete����
	//	�悤�ɂ���Ηǂ�
}
