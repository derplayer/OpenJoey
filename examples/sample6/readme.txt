�EyaneSDK3rd���痘�p�ł���DLL(yaneSDK3rd�nDLL�ƌĂ�)�쐬�L�b�g
		version 1.00

�i���_�j
	����DLL���瑼��DLL�ɑ��݂���N���X��new���邱�Ƃ��o����B
	MAIN��(EXE)�̂ق�����ADLL�ɑ��݂���N���X��new���邱�Ƃ��o����B
	DLL������MAIN���ɑ��݂���N���X��new���邱�Ƃ��o����B

�i�g�p���@�j
	dllimport.cpp�ɐ���������܂��B
	�d�g�݂ɂ��ẮA��˖{�P���Q�Ƃ̂��ƁB

�i�g�p��̒��Ӂj
	new����N���X���͖��O(string)�Ŏw��B
	���̖��O�A�ق��̂Əd�����Ȃ��悤�ɁA
		oggvorbis100::COggReader	���Ƃ�
		yaneSDK3rd::Draw::CFastDraw
	�Ƃ����悤�ɁA�X�R�[�v(::)�ŋ�؂������O���w�肷�邱�Ƃ𐄏����܂��B

	string�N���X���R���p�C���̃o�[�W�������ɍ��E����Ȃ��悤�ɁA
	yaneSDK3rd�ŗp�ӂ��Ă���string�N���X�𗘗p���Ă��������B
	�ˁ@yaneSDK3rd/config/yaneConfig.h�ŁA

		#define USE_yaneString
		//	string�N���X�́A��˂��炨�ł�p����

	���̕�����L���ɂ��Ă����܂��B�i�f�B�t�H���g�ŗL���ɂȂ��Ă���͂��ł��j

	DLL�̃N���X�ɑ΂���vector��n�������ꍇ�́Avector����˂��炨�ł�
	�p���邱�ƁB(��yaneSDK3rd�Ɋ܂܂�Ă��܂�)

	����ȊO�͍��̂߂�ǂ������̂ŃM�u�A�b�v(^^;
	STLPort��p���Ă����΁A�����炭�����͕ύX����Ȃ��ł��傤����A
	������x���S�ɂ��Ƃ�ł��邩�Ǝv���܂��B������ȊO�̃N���X

�i�\�[�X�̐����j
	�EDLLSource	�� DLL���쐬���邽�߂̃L�b�g
	�EcallFromYaneSDK3rd�� yaneSDK3rd�𗘗p���Ă�����s�t�@�C������
			���̌`����DLL�t�@�C�����Ăяo���T���v���\�[�X
	���@yaneSDK3rd�̋@�\���g�킸�AyaneSDK3rd�p��DLL���Ăяo������������
		�ꍇ�ł�������g���Ηǂ��B
		�i�ǂ����g�p���Ă��Ȃ��N���X�̃R�[�h�͍œK���ō폜�����̂�)

�i�����j
	string.h��yaneSDK3rd/YTL/string.h	�̂��̂��̂܂�
	yaneObjectCreater.h��.cpp�́A
		yaneSDK3rd/AppFrame/yaneObjectCreater.h��.cpp���̂܂�

