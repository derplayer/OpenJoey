//	yaneVirtualKey.h :
//		Virtual Key Class for CKeyBase
//				programmed by yaneurao(M.Isozaki) '99/11/13
//

#ifndef __yaneVirtualKey_h__
#define __yaneVirtualKey_h__


#include "yaneKeyBase.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

const int g_VIRTUAL_KEY_MAX=88;	//	���z�L�[�̍ő�i88��:p�j

class CVirtualKey : public IKeyBase {
/**
	�����L�[���̓N���X�BCKeyBase�̔h���N���X�𗘗p���āA
	�������ЂƂ܂Ƃ߂ɂ��ĊǗ��ł���B

	���Ƃ��΁A�L�[�{�[�h�́��L�[�ƁA�e���L�[�̂W�L�[�A
	�W���C�X�e�B�b�N�́����͂��A��̉��z�L�[�Ƃ��ēo�^���邱�Ƃɂ���āA
	�����̂ǂꂩ������͂���Ă��邩���A�֐����ЂƂĂяo��������
	����ł���悤�ɂȂ�B

	���ۂ�class CKey,class CKey2�́A
	���̃N���X�̉��p����Ȃ̂ŁA��������Q�Ƃ��邱�ƁB

	�S�̓I�ȗ���́A�L�[�f�o�C�X�̓o�^�����z�L�[�̐ݒ�Ƃ��Ă����āA
	Input�����̂�IsVKeyPress/IsVKeyPush�Ŕ���ł��B
*/
public:
	CVirtualKey();
	virtual ~CVirtualKey();

	///	�L�[�f�o�C�X�̓o�^
	/**	�L�[�f�o�C�X�Ƃ́ACKeyBase�̔h���N���X�̃C���X�^���X�B
		��̓I�ɂ�CDirectInput,CMIDIInput,CJoyStick�̃C���X�^���X��
		��������B���͂������L�[�f�o�C�X���܂��o�^����B
		������Input���Ăяo�����Ƃɂ���āA������GetState���Ăяo�����B
	*/
	void	ClearKeyDevice();			///	�f�o�C�X�̃N���A
	void	AddDevice(const smart_ptr<IKeyBase>&);		///	�f�o�C�X�̒ǉ�
	void	RemoveDevice(const smart_ptr<IKeyBase>&);	///	�f�o�C�X�̍폜
	smart_ptr<IKeyBase> GetDevice(int n);	///	���Ԗڂɓo�^�����f�o�C�X�̎擾
	/**
		���̊֐����g���΂��Ԗځi�O����n�܂�j��AddDevice�����f�o�C�X��
		�擾�ł���B�j
	*/

	///	���z�L�[�̒ǉ��E�폜
	void	ClearKeyList();							///	���z�L�[�̃��Z�b�g
	void	AddKey(int vkey,int nDeviceNo,int key);		///	���z�L�[�̒ǉ�
	void	RemoveKey(int vkey,int nDeviceNo,int key);	///	���z�L�[�̍폜
	/**
		vkey�́A���z�L�[�ԍ��B�����0�`VIRTUAL_KEY_MAX-1
		(����88��define����Ă���)�Ԃ܂œo�^�\�B
		�L�[�f�o�C�XnDeviceNo�́AGetDevice�Ŏw�肷��i���o�[�Ɠ������́B
		key�́A���̃L�[�f�o�C�X��key�i���o�[�B
	*/

	///	----	overriden from IKeyBase	 ------

	virtual LRESULT	Input();
	///�@�o�^���Ă������f�o�C�X����ǂ݂���

///	virtual bool	IsKeyPress(int vkey);		///	���z�L�[�𒲂ׂ�
///	virtual bool	IsKeyPush(int vkey);		///	���z�L�[�𒲂ׂ�
/**
	���z�L�[��vkey��������Ă��邩�ǂ����𒲂ׂ�B
	IsKeyPress�̂ق��́A���݉�����Ă����true�BIsKeyPush�́A
	�O�񉟂���Ă��Ȃ��āA���񉟂���Ă����true�B
*/

///	virtual const BYTE*	GetKeyData() const;
	/// �擾�����L�[���(BYTE m_byKeyBuffer[256])�ւ̃|�C���^�𓾂�)
	///	���ڃ|�C���^�𓾂Ă��肮�肷��l�̂��߂ɁO�O�G

protected:
	class CKeyInfo {
	public:
		int			m_nDeviceNo;
		int			m_nKey;
	};
	typedef smart_vector_ptr<CKeyInfo> CVKeyList;
	CVKeyList	m_VKey[g_VIRTUAL_KEY_MAX];			//	���z�L�[���X�g

	smart_vector_ptr<IKeyBase>	m_alpDevice;		//	���̓L�[�f�o�C�X���X�g
};

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd

#endif
