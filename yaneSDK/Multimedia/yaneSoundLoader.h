//
//	yaneSoundLoader.h :
//
//		�T�E���h�̓����I���[�f�B���O
//

#ifndef __yaneSoundLoader_h__
#define __yaneSoundLoader_h__


#include "../Auxiliary/yaneLoadCache.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class ISound;
class ISoundFactory;

class CSoundLoader : public CLoadCache {
/**
	class ISound �h���N���X�̂��߂� LoadCache
*/
public:
	virtual smart_ptr<ISound> GetSound(int nNo);		///	�T�E���h�̎擾

	///	�����ΐݒ肵�Ă���[�����I
	///	StreamSound�ōĐ����邩�ǂ����̃|���V�[�����ꂪ�֗^���Ƃ���
	void	SetSoundFactory(const smart_ptr<ISoundFactory>& sf) { m_sf = sf; }
	smart_ptr<ISoundFactory> GetSoundFactory() const { return m_sf; }

	///	StreamPlay�������I�ɗL���^�����ɂ��邽�߂̃I�v�V����
	///	(CSoundFactory�ł̐ݒ���D�悳���)
	void	SetStreamPlay(bool bStreamPlay)
	{
		m_bEnforceStreamPlay = true;
		m_bStreamPlay = bStreamPlay;
	}

	///	nNo�̃t�@�C�������擾
	string	GetFileName(int nNo);

	CSoundLoader():m_bEnforceStreamPlay(false){}

protected:
	smart_ptr<ISoundFactory> m_sf;
	virtual	LRESULT InnerLoad(const smart_obj& obj);
	bool m_bEnforceStreamPlay;
	bool m_bStreamPlay;
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif