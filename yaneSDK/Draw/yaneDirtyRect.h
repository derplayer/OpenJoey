//
//	Dirty Rect (�X�V��`)�@�Ǘ��N���X
//
//		programmed and designed by yaneurao '02/01/16

#ifndef __yaneDirtyRect_h__
#define __yaneDirtyRect_h__


namespace yaneuraoGameSDK3rd {
namespace Draw {

class IDirtyRect {
public:
	virtual void	Clear()=0;
	virtual void	Refresh()=0;
	virtual void	AddRect(int left,int top,int right,int bottom)=0;
	virtual void	AddRectSize(int x,int y,int sx,int sy)=0;
	virtual void	AddRect(const RECT& rc)=0;
	virtual int		GetRectNum() const=0;
	virtual LPCRECT	GetRect() const=0;

	virtual ~IDirtyRect(){}
};

class CDirtyRect : public IDirtyRect {
/**
	�g�������B
		Clear�ŃN���A�B
		AddRect�Ȃ�Ƃ��ŁA��`�����Z���Ă���
		�i�`�悲�ƂɊ��x�ƂȂ��J��Ԃ��j
		���̂Ƃ��A���̃N���X�̓����ŁA�ő�X�V��`(dirty rect)��
		�Ȃ�悤�Ɍ�������Ă����B�������A��ʍ���̏�����
		�E���̏������X�V���������Ȃ̂ɉ�ʑS�̂��ő�X�V��`(dirty rect)
		�Ƃ��đI������Ă��܂��̂͂܂����̂ŁA���̂悤�ȃP�[�X�ɂ��Ȃ���
		�ő�5��dirty rect�������Ă��ǂ����Ƃɂ���B

		�Ō�ARefresh���Ăяo�����Ƃɂ��A�d������i���邢�͊��S�Ɋ܂܂��j
		dirty rect����������B

		�����āAGetRectNum,GetRect�ŁAdirty rect�̐��Ƌ�`���擾�ł���B
*/
public:

	///	���ׂĂ��N���A����
	void	Clear(){ m_nRectLast = 0; }

	///	���t���b�V������
	void	Refresh();

	///	��`�̉��Z
	///		�͈͎w��^�C�v
	void	AddRect(int left,int top,int right,int bottom);
	///		���W�{�傫���w��^�C�v
	void	AddRectSize(int x,int y,int sx,int sy)
	{	AddRect(x,y,x+sx,y+sy);}
	///		��`�ŉ��Z
	void	AddRect(const RECT& rc)
	{	AddRect(rc.left,rc.top,rc.right,rc.bottom); }

	//	--- property

	///	���ʂ̎擾
	///		�������ꂽ��`���v��(0:�Ȃ�)
	int		GetRectNum() const { return m_nRectLast; }
	///		�������ꂽ��`�ւ̃|�C���^���擾
	LPCRECT	GetRect() const { return &m_arcRect[0]; }

	//	-----

	CDirtyRect() { Clear(); }

protected:
	enum	{ nRectSize = 5 };
	//	���̐�������`���m��
	//	�܂��A���܂�ׂ����Ƃ�ƁA�K�[�x�W�̌�����������̂�
	//	5���炢�ɂ��Ă����B

	RECT	m_arcRect[nRectSize+1];	   //	��`���
	int		m_anRectArea[nRectSize+1]; //	��`�ʐ�
	int		m_nRectLast;	//	���������܂Ŏg���Ă��邩�̃J�E���^
							//	�������==0 : ���g�p
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
