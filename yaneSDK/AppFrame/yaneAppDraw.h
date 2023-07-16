//	Application Draw Frame

#ifndef __yaneAppDraw_h__
#define __yaneAppDraw_h__

namespace yaneuraoGameSDK3rd {
namespace AppFrame {

class IAppDraw {
/**
	�A�v���P�[�V�����ɂ�����`��p���N���X
	class CAppManager ����āA�����̃X���b�h��
	�g���Ă���`��N���X���擾�ł���
*/
public:
	virtual int GetType() const = 0;
	/**
		�[��RTTI(�ȉ��̒l���Ԃ�)

		CFastDraw		: 1
		CDirectDraw3D	: 2
	*/


	virtual ~IAppDraw(){}
};

} // end of namespace AppFrame
} // end of namespace yaneuraoGameSDK3rd

#endif
