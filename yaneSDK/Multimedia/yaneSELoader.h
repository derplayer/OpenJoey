//
//	yaneSELoader.h :
//
//		�r�d�̓����I���[�f�B���O
//

#ifndef __yaneSELoader_h__
#define __yaneSELoader_h__


#include "../Multimedia/yaneSoundLoader.h"
#include "../Math/yaneCounter.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

class CSELoader : public CSoundLoader {
public:
	enum { nSELoaderCount = 5 };
	//	�f�B�t�H���g�ł͂T�t���[���ȓ��Ȃ�΍Đ��ĊJ���Ȃ�

	//	�t���[�����o�߂����ꍇ
	void		OnPlay();				///	���t���[���Ăяo���Ă�
	void		PlayN(int nNo);			///	���܂����Đ�����(�񃋁[�v�Đ�)
	void		Play(int nNo);			///	���Ă��Ȃ���΍Đ�����(�񃋁[�v�Đ�)
	void		PlayLN(int nNo);		///	���܂����Đ�����(���[�v�Đ�)
	void		PlayL(int nNo);			///	���Ă��Ȃ���΍Đ�����(���[�v�Đ�)
	void		PlayT(int nNo,int nTimes,int nInterval=0);
										///	�Đ��񐔎w��Đ�
	void		Stop(int nNo);			///	�T�E���h���~�߂�
	void		Reset();				///	�T�E���h�̔r���J�E���^�̃��Z�b�g
	bool		IsPlay(int nNo);		///	�T�E���h�͍Đ����Ȃ̂��H

	virtual smart_ptr<ISound>	GetSound(int nNo);	///	�T�E���h�̎擾
	///	������� class CSoundLoader �̃����o���g���Ηǂ�
	//	���_���B�L���b�V�����N���X���̂��̂��Ⴄ�̂ŃI�[�o�[���C�h����K�v������

	///	�Đ��L�����Z���t���O
	///	(���̃t���O��true�Ȃ�΁A���ׂĂ̍Đ��̓L�����Z�������)
	bool*		GetCancelFlag()		{ return &m_bCancel; }

	void		OnPlayAndReset();
	///	���݁A�Đ��˗��̂�����̂������I�ɍĐ�����

	///	�Đ��C���^�[�o���̐ݒ�(�f�B�t�H���g�ł�nSELoaderCount)
	void	SetLockInterval(int nLockInterval) { m_nLockInterval = nLockInterval; }
	int		GetLockInterval() { return m_nLockInterval; }

	///	�C���̂��߂�CLoadCache����I�[�o�[���C�h
//	virtual LRESULT	Set(const string& filename,bool bUseID=false)
//		{ return SetHelper(filename,bUseID,CSELoaderInfo()); }
	//	��CLoadCacheInfo::pObj��smart_ptr<CSELoaderInfo>�𕕓�����̂ŕs�v�ɂȂ���

	CSELoader();
	virtual ~CSELoader();

protected:

	struct CSELoaderInfo
	{
		string strFileName;		//	�ǂݍ��ނׂ��t�@�C����
		smart_ptr<ISound> obj;	//	�ǂݍ��ނׂ��T�E���h
		int		nPlay;
		//	�v���X�̂Ƃ��́Aguard time���Ӗ�����
		//		nSELoaderCount�ōĐ�����āA0�ɂȂ�܂Ńf�N�������g�����
		//	����}�C�i�X�Ȃ�΍Đ��񐔂Ȃ̂�O�O�G
		CRootCounter	nInterval;
		//	�A���Đ��̂Ƃ��̍Đ��C���^�[�o��
		int		nID;
		//	ID�͂����ɂ����߂Ă����Ȃ��ƃI�u�W�F�N�g��iterator�ŉ񂵂�
		//	���񂷂�Ƃ��ɁA���̃I�u�W�F�N�g�ɑ΂��郁���o���Ăяo���Ȃ�
		CSELoaderInfo() : nPlay(0), nID(-1) {}
	};

	virtual void OnSet(const smart_obj& obj,int nID);
	virtual LRESULT	InnerLoad(const smart_obj& obj);
	//	���L���b�V�����N���X���̂��̂��Ⴄ�̂ŃI�[�o�[���C�h����K�v������

	bool	m_bCancel;			//	BGM�L�����Z���t���O
	int		m_nLockInterval;	//	���̃t���[���ȓ��Ȃ�΍Đ��ĊJ���Ȃ�
};

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd

#endif
