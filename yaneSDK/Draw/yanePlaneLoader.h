//
//	yanePlaneLoader.h :
//
//		�v���[���̓����I���[�f�B���O
//

#ifndef __yanePlaneLoader_h__
#define __yanePlaneLoader_h__

#include "../Auxiliary/yaneLoadCache.h"
#include "../Draw/yaneSurface.h"
#include "../Draw/yanePlane.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CPlaneLoader : public CLoadCache {
/**
	class ISurface �h���N���X�̂��߂� LoadCache

	ISurface�̂��߂�factory�́A
		CPlane::SetFactory�Őݒ肳�ꂽ���̂������I�Ɏg�p����
		(����CPlaneLoader�̃R���X�g���N�^��CPlane::GetFactory����)
		CPlane::SetFactory�Őݒ肳�ꂽ���̈ȊO�̂�p�������Ȃ�΁A
		CPlaneLoader::SetFactory�ł��Ƃ���ݒ肷��Ηǂ��B

	�g�p��)
	CPlaneLoader pl;
	pl.SetReadDir("testplane/");	//	�ǂݍ���path�̐ݒ�
	pl.Set("planedefine.txt",true);

*/
public:

//	virtual smart_ptr<ISurface> GetPlane(int nNo);		///	�v���[���̎擾
	virtual CPlane GetPlane(int nNo);		///	�v���[���̎擾
	//	���������̂ق����g���₷���C������̂����B

	///	nNo�̃t�@�C�������擾
	string	GetFileName(int nNo );

	/**
		�����F�̐ݒ�^�擾
		���̐F���f�B�t�H���g�̔����F�Ƃ���
		�i�f�B�t�H���g= RGB(0,255,0)���Ȃ킿��)

		���ӁF�ʗ�A�T�[�t�F�[�X�̍���(0,0)�̓_�𔲂��F�Ƃ��邪�A
			���֐�����CPlaneLoader�ł́A�F�Ŕ����F�����߂�B
	*/
	void	SetColorKey(ISurfaceRGB rgb)
		{ m_rgbColorKey = rgb; m_bRGBColorKey = true; }
	ISurfaceRGB GetColorKey() const
		{ return m_rgbColorKey; }

	///	�����F�́ARGB�Ŏw�肳��Ă��邩�H
	bool	IsRGBColorKey() const { return m_bRGBColorKey; }

	///	�����F���A�ʒu�w��Őݒ肷��
	void	SetColorKeyPos(int x,int y)
	{	m_bRGBColorKey=false; m_nColorKeyPosX=x; m_nColorKeyPosY=y; }

	///	�T�[�t�F�[�X��factory��ݒ�/�擾����(default�ł�factory�ɂ�
	///	CPlane::GetFactory�������̂��������Ă���)
	void SetFactory(const smart_ptr<IPlaneFactory>& pFactory) { m_pPlaneFactory = pFactory; }
	smart_ptr<IPlaneFactory> GetFactory() { return m_pPlaneFactory; }

	CPlaneLoader();
protected:
	virtual	LRESULT InnerLoad(const smart_obj& obj);
	ISurfaceRGB m_rgbColorKey;
	bool	m_bRGBColorKey;		//	�����F�́ARGB�Ŏw��Ȃ̂��H
	int		m_nColorKeyPosX;
	int		m_nColorKeyPosY;
	smart_ptr<IPlaneFactory>  m_pPlaneFactory;
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif