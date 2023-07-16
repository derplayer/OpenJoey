//	yaneChara.h :
//
//		Simple Sprite Manager
//		programmed by yaneurao '00/07/20
//		rewritten by yaneurao '03/06/18
//

#ifndef __yaneSprite_h__
#define __yaneSprite_h__

namespace yaneuraoGameSDK3rd {
namespace Draw {

class ISurface;

struct CSimpleSprite {
/**
	�P���̃X�v���C�g��\������
	�]�����T�[�t�F�[�X�ƁA�]������`�̏�񂵂��Ȃ��B
*/
public:
	ISurface*	pSurface;	///	�]�����v���[��
	RECT		rcRect;		///	�]������`
	// ---------- �g��������Έȉ��̂��g���Ă����� ----------
	int			nOx,nOy;	///	�]���I�t�Z�b�g
	int			nHeight;	///	�x�[�X���C���Z�o�p
	bool		bFast;		///	�����F�����]�����H

	//	---�⏕
	void		set(ISurface*p);
	///	�T�[�t�F�[�X�S�����̂��̃X�v���C�g�Ƃ���
	///	nOx=nOy=0,bFast=true,nHeight=sy

	void		set(ISurface*p,const RECT&rc);
	///	�T�[�t�F�[�X�̈ꕔ�����̃X�v���C�g�Ƃ���
	///	nOx=nOy=0,bFast=true,nHeight=sy

};

class CSprite {
/**
	CSimpleSprite��vector��vector�������o�Ɏ��A�X�v���C�g�N���X

//		�X�v���C�g�Ƃ́A�u�v���[���ɂ������`�v�̏W���Ƃ���
//		��`�ł���_���\���ł����āA�`��Ƃ͐؂藣���čl����ׂ�
//
//		����āA���̃N���X�͕`��Ɋւ��邱�Ƃ��s�Ȃ�Ȃ�

*/
public:
	///	�X�v���C�g�̎���
	typedef vector<CSimpleSprite> spritevector;
	typedef vector<spritevector> spritevectorofvector;

	///	�X�v���C�g���̂̎擾
	spritevectorofvector* getAllSprite() { return m_vSprtieVector.get(); }
	spritevector* getDirectionalSprite()
		{ return &((*getAllSprite())[getDirection()]); }
	CSimpleSprite* getSprite()
		{ return &((*getDirectionalSprite())[getMotion()]); }

	///	�X�}�[�g�|�C���^�ł̎擾/�ݒ�(CSpriteChara����擾���Đݒ肷��ׂ�)
	smart_ptr<spritevectorofvector> getMySprite() { return m_vSprtieVector; }
	void	setMySprite(const smart_ptr<spritevectorofvector>& v)
		{ m_vSprtieVector = v; 	m_nDirection = m_nAnimation	= 0;}
	/**
		(*getAllSprite())[getDirection()][getMotion()]
		����ʂɕ`�悷��̂ł���B

		m_nAnimation (getMotion�Ŏ擾�ł���)�̓A�j���[�V�����J�E���^�Ȃ̂�
		���t���[�������I�ɉ��Z�����
		m_nDirection (getDirection�Ŏ擾�ł���)�́A�L�����N�^�̌�����ύX����B
		�i�������ƂɈ�A�̃A�j���[�V��������`����Ă���ƍl����)

		��L�̕\���́A
		(*getDirectionalSprite())[m_nAnimation]
		��A�P��
		*getSprite()
		�Ə�����B����́A���ݕ\�����Ă���i�\������j�X�v���C�g����
		�擾������̂ł���B

		setMySprite�����Ƃ��AsetDirection(0); setMotion(0); �������ƂɂȂ�
	*/


	///	�X�v���C�g�����̐ݒ�/�擾
	/**
		��:setDirection�ł́A�����̕ύX�������ꍇ�̓A�j���[�V�����J�E���^��
		���Z�b�g���Ȃ��B�����̕ύX������ꍇ�́A�A�j���[�V�����J�E���^��
		0�ɂ���B
	*/
	void	setDirection(int nDirection);
	int		getDirection() const { return m_nDirection; }

	///�@�ǉ��I�t�Z�b�g�ʂ�ݒ�/�擾����i�����l(0,0)�j
	void	setOffset(int nOffsetX,int nOffsetY);
	void	getOffset(int& nOffsetX,int& nOffsetY);

	///	���݂̃A�j���[�V�����ԍ���ݒ�/�擾/�C���N�������g����
	void	setMotion(int n) { m_nAnimation=n; }
	int		getMotion() const { return m_nAnimation; }
	void	incMotion();
	///	�A�j���[�V�������I�[�ɂ܂ŒB������?
	bool	isEnd() const
	{ return (int)const_cast<CSprite*>(this)->getDirectionalSprite()->size()
		== m_nAnimation; }

	///	�X�v���C�g�̕\�����W��ݒ肷��
	void	setPos(int x,int y) { m_nX = x; m_nY = y; }
	void	getPos(int&x,int&y) const { x=m_nX; y=m_nY; }

	///	�����̐ݒ�(���̏����g�������g���܂����\��Ȃ�)
	void	setHeight(int nHeight) { m_nHeight=nHeight; }
	int		getHeight() const { return m_nHeight; }

	///	�X�v���C�g��L���ɂ���/�L�����ǂ����̏�Ԃ��擾����
	///	(���̏����g�������g���܂����\��Ȃ�)
	void	enable(bool bEnable) { m_bVisible = bEnable; }
	bool	isEnable() const { return m_bVisible; }

	///	�X�v���C�g�̃v���C�I���e�B��ݒ肷��
	///	(���̏����g�������g���܂����\��Ȃ�)
	void	setPriority(int nPriority) { m_nPriority = nPriority; }
	int		getPriority() const { return m_nPriority;}

	CSprite();
	virtual ~CSprite();

protected:
	smart_ptr<spritevectorofvector>		m_vSprtieVector;
	int		m_nDirection;		//	�����Ă������
	int		m_nAnimation;		//	�A�j���[�V�����J�E���^
	/**
		m_vSprtieVector[m_nDirection][m_nAnimation]
		����ʂɕ`�悷��ƍl����B

		m_nAnimation�̓A�j���[�V�����J�E���^�Ȃ̂�
		���t���[�������I�ɉ��Z�����

		m_nDirection�́A�L�����N�^�̌�����ύX����B
		�i�������ƂɈ�A�̃A�j���[�V��������`����Ă���ƍl����)
	*/

	//	�ȉ��̃����o�̓f�B�t�H���g��0
	int		m_nX,m_nY;			//	�\���ʒu
	//	�ȉ��͎g�������Ȃ���Ύg��Ȃ��ėǂ�
	int		m_nOx,m_nOy;		//	�I�t�Z�b�g���W
	int		m_nHeight;			//	����(�L�����̃x�[�X���C���Z�o�̂���)
	bool	m_bVisible;			//	�\���E��\��
	int		m_nPriority;		//	�`��v���C�I���e�B
};

struct CSpriteEx : public CSprite {
/**
	�`��@�\���̃X�v���C�g�N���X
	�K�v�ɉ����āA���̃N���X��h�������ă����o�𑫂����A
	CSpirte����ʂɔh�������邩���Ďg���Ă����
*/
public:

	void	Blt(ISurface*pSurface,LPRECT lpClip=NULL);
	//	�ʏ�`��(�A�j���[�V�����J�E���g���Z�����)

	void	Blt(ISurface*pSurface,int x,int y,LPRECT lpClip=NULL);
	//	���W�w����`��(�A�j���[�V�����J�E���g���Z�����)

	void	BltFix(ISurface*pSurface,LPRECT lpClip=NULL);
	//	�A�j���[�V�����J�E���g�͉��Z����Ȃ��`��

	void	BltFix(ISurface*pSurface,int x,int y,LPRECT lpClip=NULL);
	//	���W�w����`��(�A�j���[�V�����J�E���g�͉��Z����Ȃ�)

	void	BltOnce(ISurface*pSurface,int x,int y,LPRECT lpClip=NULL);
	//	�A�j���[�V�����J�E���g���Ō�ɂȂ��Ă����炻��ȏ�͉��Z���Ȃ��`��
	//	(����ȊO�̏ꍇ�ɂ̓A�j���[�V�����J�E���g�����Z�����)
};

class CSpriteChara {
///	�t�@�C������X�v���C�g��`��ǂݍ��ނ��߂̃��[�_�[
///	(yaneSDK2nd��CSpriteChara�݊�)
public:

	///	�L�����p�^�[����`�t�@�C�����e�L�X�g�t�@�C������ǂݍ���/�������
	LRESULT	Load(const string& strFileName);
	void	Release();

	/**
		Load����Ƃ��ɐ�������T�[�t�F�[�X��
		CPlane�ɐݒ肵�Ă���factory��p����
	*/

	///	Load�œǂݍ��񂾃X�v���C�g��`���擾����
	///	(�����œ������̂�CSprite::setMySprite�ɓn���Ɨǂ�)
	smart_ptr<CSprite::spritevectorofvector> getMySprite()
		{ return m_vSpriteVector; }
	void	setMySprite(const smart_ptr<CSprite::spritevectorofvector>& v)
		{ m_vSpriteVector = v; }

protected:

	///	Load�œǂݍ��񂾂Ƃ��̃T�[�t�F�[�X
	smart_vector_ptr<ISurface>	m_apSurface;

	///	�W�J�����X�v���C�g����
	smart_ptr<CSprite::spritevectorofvector> m_vSpriteVector;
};

}} // end of namespace

#endif
