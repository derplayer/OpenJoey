// yaneIMEWrapper.h : easy ime wrapper
//		programmed by yaneurao(M.Isozaki) '02/03/08
//

#ifndef __yaneIMEWrapper_h__
#define __yaneIMEWrapper_h__


#include <imm.h>	// for ImmAssociateContext

#pragma comment(lib,"imm32.lib")

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class IIMEWrapper {
public:
	virtual void Show()=0;		/// IME�̕\��
	virtual void Hide()=0;		///	IME�̔�\��
	virtual ~IIMEWrapper(){}
};

class CIMEWrapper : public IIMEWrapper {
/**

	IME�̕\���E��\���𐧌䂵�܂��B
	Show��A���ŌĂяo���Ă��A
	��xHide���Ăяo����IME�͏����܂��B
	�i�Q�ƃJ�E���g�͎���Ă��܂���j

		CIMEWrapper::GetObj()->Show();
		CIMEWrapper::GetObj()->Hide();
	�̂悤�ɂ��Ďg���܂��B

	���ӁF
	�E�B���h�D�N���X���ŁA�t���X�N���[�����[�h�ɂȂ����Ƃ��ɂ́A
	�����I�ɃI�t�ɂȂ�悤�ɂ��Ă��܂��B
	����́ADirectDraw���AIME(��GDI)�𖳎����ē]�����Ă��܂����߂ł��B

	�ꉞ�AClipper���d�|���Ă����΂����͉���ł���̂ł����A
	�t���X�N���[������Flip���s�Ȃ��ݒ肾�ƁAIME�͂�����܂����A
	�Q�[������IME���g�����Ƃ͖������낤�Ƃ������ƂŁA���������d�l�ɂ��܂��B
	�����C�ɓ���Ȃ��ꍇ�͂��̂Q�_�ɋC�����Ȃ���ύX����悤�ɂ��Ă��������B

	�܂��AIME�̕\���E��\����Window�Ɗ֘A���čs�Ȃ��邽�߁A
	Window�����O��Hide���Ă��Ӗ����Ȃ��̂Œ��ӂ��Ă��������B
*/
public:
	virtual void Show();	/// IME�̕\��
	virtual void Hide();	///	IME�̔�\��

	CIMEWrapper() { m_nStat=0; }
	virtual ~CIMEWrapper() {
	//	if (m_nStat==2) Show();	//	�����Ă���Ȃ�Ώo��������
	//	��singleton�̃f�X�g���N�^����singleton�̌Ăяo���͋֎~
	}

	static CIMEWrapper* GetObj() { return m_vIMEWrapper.get(); }
	/**
		singleton�ȃ|�C���^�̎擾

		�������Aproxy_ptr�Ȃ̂ŁA�񃍁[�J����static�I�u�W�F�N�g��
		�R���X�g���N�^����Q�Ƃ��Ă͂����Ȃ�
	*/

protected:
	static singleton<CIMEWrapper> m_vIMEWrapper;

	int m_nStat;
	//	���݂̏�� 0:�s�� 1:Show 2:Hide
	//	�˘A���I�ȌĂяo����h�~����
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
