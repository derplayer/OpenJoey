#include "stdafx.h"
#include "function_callback.h"
//	YTL�̂Ȃ���.cpp�͂�����̃t�H���_�ɒu���Ă������ق���
//	�v���W�F�N�g�ɑ}������̂��y����ۂ�

namespace yaneuraoGameSDK3rd {
namespace YTL {

//	��e���v���[�g�Ȃ̂�inline�ŏ����Ȃ������Ԃ�
function_callback_v* function_callback_v::Create(void (*f)()){
	return new function_callback_vg0(f);
}

template <class Result>
function_callback_r<Result>* function_callback_r<Result>::Create(Result (*f)()){
	return new function_callback_rg0<Result>(f);
}
	//	���e���v���[�g���O�ɏ����̂͑�ςł��Ȃ�(;�L�D`)

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd
