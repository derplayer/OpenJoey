//
//	CPU�̎�������
//

#ifndef __yaneCPUID_h__
#define __yaneCPUID_h__


namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class CCPUID {
/**
	CPU���ʗp
	��)
		if (CCPUID::GetObj()->isMMX()) {
			//	MMX�p�̏���
		}
	�Ƃ����悤�Ɏg��
*/
public:
	int	GetID();
	/**
		CPUID��Ԃ�
		0:������(���ʃG���[)
		1:486�ȑO
		2:MMX,PentiumPro����
		3:PentiumPro(MMX�͔�Ή�)
		-------------------------�ȉ����͕��������B
			�������ꂽ�ꍇ�A���̍��v���A���̊֐��̖߂�l�ƂȂ�
		4:MMX�@�\���p��
		8:SSE�@�\���p�\
		16:SSE2�@�\���p�\
	*/

	bool	isMMX() { return (GetID()&4) != 0; }
	///	MMX�Ή��̃}�V����?

	bool	isSSE() { return (GetID()&8) != 0; }
	///	SSE�Ή��̃}�V����?

	bool	isSSE2() { return (GetID()&16) != 0; }
	///	SSE2�Ή��̃}�V����?

	static CCPUID* GetObj() { return m_obj.get(); }
	///	���̃N���X��singleton�I�u�W�F�N�g�𓾂�

	CCPUID():m_nCPUID(0) {}
protected:
	int	m_nCPUID;
	static singleton<CCPUID> m_obj;
};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
