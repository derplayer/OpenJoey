/*

	yaneSDK configuration

*/
#ifndef __yaneConfig_h__
#define __yaneConfig_h__

//	�R���p�C���ʂ̐ݒ�
#if	defined(_MSC_VER) && (_MSC_VER>=1200)
//	VC6.0(sp5)/VC7.0/VC7.1�ŃR���p�C���m�F�ς�
	#define yaneSDK_MSVC_Mode

	// The debugger can't handle symbols more than 255 characters long.
	// STL often creates symbols longer than that.
	// When symbols are longer than 255 characters, the warning is disabled.
	#pragma warning(disable:4786)
	#pragma warning(disable:4503)

	// --------	 VC++7�p��warning���x��4�ŏo�Ă͍���warning��}�� -------
	#pragma warning(disable:4100) // �����͊֐��{�̕��łP�x���Q�Ƃ���Ă��Ȃ�warning
	#pragma warning(disable:4127) // while(true)�Ƃ���warning���o��
	#pragma warning(disable:4244) // int����UCHAR���̐؂�l�߂ɑ΂���warning
	#pragma warning(disable:4706) // while (lr=func())�Ƃ��ɑ΂���warning
	#pragma warning(disable:4710) // inline�֐���inline�W�J����Ȃ�����warning
	#pragma warning(disable:4702) // �񓞒B�R�[�h��warning(STL�ő�ʂɏo��)
	// -------- VC7�ŏo�Ă���A�����Ƃ�����waring�΍� --------------------
	//	�֐���throw�錾�ɂ�����waring��s�ɂ���(VC7)
	#pragma warning(disable:4290)

#elif defined(__BORLANDC__) && (__BORLANDC__>=0x551)
//	BCB5(up1)/BCC5.5.1�ŃR���p�C���m�F�ς�
	#define yaneSDK_BCC_Mode

	//	TASM�ς�łȂ��Ȃ�define���ׂ�
	#define yaneSDK_CantUseInlineAssembler

	//	�ւ��warning�̗}��
	#pragma	warn-8022 //�@�I�[�o�[���C�h�ɂ��֐��B��

#elif defined(__MWERKS__) && (__MWERKS__>=0x3003)
//	CodeWarrior8.3�ŃR���p�C���m�F�ς�
	#define yaneSDK_CodeWarrior_Mode

	//	InlineAssembler���g���邯�Ǖ��@���Ⴄ�悤�Ȃ̂�
	#define yaneSDK_CantUseInlineAssembler

#else 
//	�Ȃ�₱�̂��������ȃR���p�C���R(`�D�L)�m
	#error Sorry, yaneSDK3rd cannot be compiled by this compiler.

#endif



// -=-=-=-=-=-=-=-=- Plugin�쐬�@�\�̔r�� -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//#define COMPILE_YANE_PLUGIN_DLL	//	plug-in DLL���쐬����̂��H
	//	�i��DLL���쐬����Ƃ��̂ݗL���ɂ��Ă��������j
/**
	�������AyaneSDK3rd�̂��ׂĂ̋@�\��DLL���Ŏg�p�ł��邱�Ƃ�
	�ۏ؂�����̂ł͂���܂���B���Ƃ��΁A�񃍁[�J����static�ȃI�u�W�F�N�g�́A
	DllMain���Ăяo�����ȑO�ɏ����������̂ŁA
	���̂Ȃ���new���Ă���ƁAoperator new���u�������܂���new���邱�Ƃ�
	�Ȃ�̂ŁAnew�Ɏ��s���܂��BDll����YaneDllInitializer��Main������
	�Ăяo���ꂽ�ȍ~�ɂ���new���邱�Ƃ͏o���܂���B

	Dll������g�����Ƃ����O�ɂ킩���Ă���̂Ȃ�΁AMain������
	CObjectCreater::RegistClass��p���āAExport���Ă��܂��ADLL�������
	�C���^�[�t�F�[�X����ăA�N�Z�X����Ηǂ��ł��B

	�ȉ���USE_DEFAULT_DLLMAIN��EXPORT_YANE_PLUGIN_DLL���Q�Ƃ��Ă��������B
*/

//#define NOT_USE_DEFAULT_DLLMAIN
//	�����̃V���{�����`���Ă���΁AYANE_PLUGIN_DLL���쐬����Ƃ��ɁA
//		DllMain�����[�U�[�������B(yaneObjectCreater.h�ɂ���ADllMain���Q�l��
//		�����ă`���[�_�C)

//#define EXPORT_YANE_PLUGIN_DLL
/**
	���@�������@��

	YanePlugInDll�p�ɁA�悭�g���N���X��export���Ă����@�\
	�����define���ăR���p�C������΁A�悭�g���N���X��
	export(CObjectCreater::RegisterClass�œo�^)���܂��B

	export�����̂́A
	�E
	�E
	�ł��B
*/

// -=-=-=-=-=-=-=-=- �g��Ȃ��@�\�̔r�� -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// --- �������}�l�[�W�����g -----------------------------------------------

//#define USE_MEMORY_CHECK		//	new,delete���ƂɃ��O�L�^���c�����H
//#define USE_MEMORY_STATE		//	�������̃f�o�b�O�N���X(CMemoryState)�𗘗p���邩
								//	�i���̃N���X�́A������new/delete�̖�ڂ��ʂ��j
								//	�����̃N���X�͍�肩���B�g��Ȃ��łˁI�I

#define USE_EXCEPTION			//	��O�������g��

//	�������A��L�I�v�V�����ŗ�O���g�p����Ƃ��āA
#ifdef _DEBUG
//	#define USE_STOP_EXCEPTION
	//	YTL/exceptions.h�Œ�`����Ă���
	//	��O�����������Ƃ��Ƀ_�C�A���O��\������
	//	�������G���[�Œ�~���邽�߂̑[�u(�f�o�b�O���̂�)
#endif

// --- MIDI�o�͌n  --------------------------------------------------------
//

// --- �X�g���[���Đ�  ----------------------------------------------------

#define USE_StreamSound		//	CStreamSound���g�p����

// --- �`��	 --------------------------------------------------------------

//	#define USE_Direct3D		//	CDirect3DPlane,CDirect3DDraw���g�p����̂�

#define USE_FastDraw		//	CFastPlane,CFastDraw���g�p����̂�
#define USE_DIB32			//	�������FastPlane�̎����̂��߂ɉ��ɗ��p���Ă���
							//	�̂��̃o�[�W�����ł͏����\��
#define USE_YGA				//	YGA���g�p����̂�

// --- JoyStick�֘A -------------------------------------------------------

#define USE_JOYSTICK		///	JoyStick���g�p����̂�

// --- ���[�r�[�Đ��n -----------------------------------------------------

#define USE_MovieDS			// DirectShow���g���i�����j 
							// DirectShow�ɑΉ������t�H�[�}�b�g�ŗL��΂��������Đ��o����
							// ������DirectX6.1�ȏオ�����Ă���K�v������
							// DirectShow�������Ă��Ȃ��ꍇ�A����USE_MovieAVI����`����Ă����
							// ������ōĐ������݂�悤�ɂȂ��Ă���
#define USE_MovieAVI		// AVIStream�֐���p����B������AVI�t�@�C�������Đ��ł��Ȃ��B

// --- �X�N���[���Z�[�o�[�n ----------------------------------------------------
//#define USE_SAVER

// --- CErrorLog�o�͌n ----------------------------------------------------

#define USE_ErrorLog		//	CErrorLog�N���X��L���ɂ���B
//	�����define���Ȃ���΁ACErrorLog�N���X�͋�̃N���X�ɂȂ�A�f�o�b�O�p�̃G���[
//	�����񓙂�VC++�̍œK���ɂ���ď��ł���B

// --- STL	--------------------------------------------------------------
//	�g�p����STL�̎�ނ�I������

#define USE_STL_OnVisualC
//	Visual C++��STL���g�p����

//#define USE_STLPort
/**
	STLPort��p����ꍇ�́A��L��USE_STL_OnVisualC���R�����g�A�E�g���邱�ƁI

	STLPort�̔z�u�ꏊ�͂ǂ��ł��\���܂���B����yaneSDK(yaneSDK.h)��
	���݂���t�H���_�̂ЂƂ�̃t�H���_��stlport�Ƃ����t�H���_���쐬���A
	���̂Ȃ���stlport(4.5.3�ɂē�����m�F)�𓀂������̂�z�u���Ă��܂��B

	�t�H���_�Ґ���
		|---yaneSDK
		|	|---config
		|	|---AppFrame
		|	|---���낢��
		|---stlport
			|---doc
			|---etc
			|---src
			|---stlport�@�����̃t�H���_��VC++�́uinclude�p�X�v�Ƃ��Ďw�肵�Ă��
			|---test		(��Ԏw�肵�Ȃ��Ƃ����Ȃ��B�W���w�b�_��ǂ�ł���ł͒x��)
						VC++.NET�Ȃ�΃v���W�F�N�g���\�����[�V�����G�N�X�v���[���[�ŉE�N���b�N����
						�\���v���p�e�B��C/C++�ˑS�ʁ˒ǉ��̃C���N���[�h�f�B���N�g���Ŏw��


	lib�́A
		�R�}���h�v�����v�g�����s���āA
			VC�̃C���X�g�[���f�B���N�g����bin�t�H���_�ɂ���
			VCVARS32.BAT
		�����s��A
			C:\STLport-4.5.3\src
		�ŁA
			nmake -f vc7.mak (VC++6�Ȃ��vc6.mak)
		�Ƃ���΁A
			C:\STLport-4.5.3\lib
		�ɐ��������̂ŁA������lib�p�X�Ƃ��Ďw�肵�Ă��
		�i�\���̃v���p�e�B�˃����J�ˑS�ʁ˒ǉ��̃��C�u�����f�B���N�g���j
*/

// --- YTL	--------------------------------------------------------------
//	string�N���X�́A��˂��炨�ł�p����
//#define USE_yaneString
/**
	����
	CObjectCreater�ŁADLL�t�@�C���ƃI�u�W�F�N�g�𓊂������ꍇ�́A
	������`���Ă����Ȃ��ƁA�R���p�C���̃o�[�W�������قȂ��string�̎�����
	�قȂ�̂ł܂����B
*/

//	delegate���g��
#define yaneSDK_USE_delegate

#endif
