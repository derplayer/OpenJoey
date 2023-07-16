/**
	ISurface��256�{�g���₷������I
*/

#ifndef __yanePlane_h__
#define __yanePlane_h__


namespace yaneuraoGameSDK3rd {
namespace Draw {

class ISurface;
class IPlaneFactory {
public:
	virtual ISurface* CreateInstance() = 0;
};

class CPlane {
/**
	smart_ptr<ISurface> �Ƃ������������͖̂ʓ|�ɂ܂�Ȃ��̂ŁD�D
*/
public:
	///	�|�C���^�̂ӂ�����邽�߂̎d�|��
	ISurface& operator*() const	 {return *get(); }
	ISurface* operator->() const {return get();	 }

	///	ISurface�ւ̈Öق̕ϊ�
	operator ISurface* () const { return get(); }

	ISurface* get() const { return p_.get(); }

	//	default factory�̐ݒ�/�擾
	static void SetFactory(const smart_ptr<IPlaneFactory>& pFactory) { pFactory_ = pFactory; }
	static smart_ptr<IPlaneFactory> GetFactory() { return pFactory_; }

	void	CreateInstance(const smart_ptr<IPlaneFactory>& pFactory);
	///	factory��n���Ζ����I�ɃT�[�t�F�[�X�𐶐�����

	CPlane();
	/**
		������́ASetFactory����Ă���factory���玩���I��surface�𐶐�����
	*/

	CPlane(const smart_ptr<IPlaneFactory>& pFactory);
	///	surface��factory��n���΁A�����p���Đ������ĕێ�����

	CPlane(const smart_ptr<ISurface>& surface);
	///	copy constructor

	CPlane(ISurface* surface);
	///�@������́A���L����CPlane���ێ�����̂ŁA
	///		CPlane plane(new CTextFastPlane);
	///	�̂悤�ɂ��Ďg�����ƁI

protected:
	smart_ptr<ISurface> p_; // p_;�@�����ċ����Ă�́H
	static ThreadLocal<smart_ptr<IPlaneFactory> > pFactory_;	// default factory
};

class CFastDraw;
struct CPlaneFactoryFastPlane : public IPlaneFactory {
/**
	IPlaneFactory�h���N���X�B������CFastPlane�𐶐����邽�߂�factory
*/
	CPlaneFactoryFastPlane(CFastDraw*p) : pDraw_(p) {}
	virtual ISurface* CreateInstance();
protected:
	CFastDraw* pDraw_;
};

class CFastPlaneFactory {
/**
	���̃N���X�́A�R���X�g���N�^�̂Ȃ��ŁA
	IPlaneFactory�h���N���X��CPlane�ɑ΂��Đݒ肷��
	����ɂ��A���̃N���X��CApp�Ɏ�������������΁A
	���̃X���b�h�́ACPlane�́A���ׂ�CFastPlane���Ӗ����邱�ƂɂȂ�
*/
public:
	CFastDraw*	GetDraw() { return m_vFastDraw.get(); }

	CFastPlaneFactory();

protected:
	smart_ptr<CFastDraw>	m_vFastDraw;	//	FastDraw�����Ă܂��˂�
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
