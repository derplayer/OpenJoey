//
//	yaneSurfaceCache
//
//	�v���C�I���e�B��������ISurface(�`��v���C�I���e�B����������)
//

#ifndef __yaneSurfaceCache_h__
#define __yaneSurfaceCache_h__

#include "yaneSurface.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CSurfaceCache : public ISurface {
/**
	�v���C�I���e�B��������ISurface(�`��v���C�I���e�B����������)
*/
public:
	/**
		���݂̕`��v���C�I���e�B��ݒ肷��
		���̌�̕`��v��(ISurface�̃����o�Ăяo��)�́A
		���ׂĂ���priority���ݒ肳���B
		�K���ݒ肵�Ă���`��v���𑗂邱�ƁI
	*/
	void	setPriority(int nPriority){ m_nPriority = nPriority; }
	int		getPriority() const { return m_nPriority; }

	/**
		���܎����Ă���`��v����flush����(�f���o��)
		���̂Ƃ��Apriority�̒Ⴂ���Ԃɓf���o��
		���ʂƂ��āApriority�̍������̂��O�ʂɕ`�悳��邱�ƂɂȂ�
		(�f���o������A�`��v���̓N���A����遨�����I��reset()���Ăяo�����)
	*/
	void	flush(ISurface* pDstSurface){
		std::sort(m_draw_list.begin(),m_draw_list.end(),
			sort_functor());
		//	priority�ł̃\�[�g
		draw_list::iterator it = m_draw_list.begin();
		while (it!=m_draw_list.end()){
			smart_ptr<function_callback>& fn = it->second.first;
			fn->bind(pDstSurface);	fn->run(); //	�Ϗ���̕ύX�����Ă̎��s
			++it;
		}
		reset();
	}

	/**
		�`��v������������B
		���̊֐��̂Ȃ���setPriority(0) ���s�Ȃ�
	*/
	void	reset(){
		m_draw_list.clear();
		m_nPriority = 0;
	}

	CSurfaceCache():m_nPriority(0){}

private:
	typedef smart_ptr<function_callback> func;
	typedef smart_ptr<void> Object;

	typedef pair<int,pair<func,Object> > draw_request;
	//	priority�ƃR�[���o�b�N��B

	typedef vector<draw_request> draw_list;

	//	sort policy�́A�D�揇�ʂŏ���(���l�̑傫�����̂قǌ��ɒu��)
	struct sort_functor {
		bool operator ()
			(const draw_request& rhs,const draw_request &lhs) const {
			return rhs.first > lhs.first;
		}
	};

	draw_list	m_draw_list;
	int			m_nPriority;

	//	helpper function
	void	push(const func& fn,const Object& obj=Object())
	{
		m_draw_list.push_back(
			draw_request(getPriority(),pair<func,Object>(fn,obj)));
	}

public:
	//	�ȉ��AISurface�̃����o�Ƃ܂����������B
	//	overriden from ISurface

	virtual LRESULT GeneralBlt(CSurfaceInfo::EBltType type,const CSurfaceInfo* pSrc,
		const CSurfaceInfo::CBltInfo* pBltinfo,DWORD*pAdditionalParameter=NULL) {
			CSurfaceInfo::CBltInfo* pInfo
				= const_cast<CSurfaceInfo::CBltInfo*>(pBltinfo);
			Object obj(CSurfaceInfo::getWrappedPtr(
				type,pInfo,pAdditionalParameter));
			//	�����ɗa���Ă��܂�

			push(func(function_callback_r<LRESULT>::Create(
				&ISurface::GeneralBlt,(ISurface*)0,
				type,pSrc,pInfo,pAdditionalParameter))
				,
				obj
				);
			return 0;
	}
	virtual LRESULT GeneralMorph(CSurfaceInfo::EBltType type,const CSurfaceInfo*pSrc,
		const CSurfaceInfo::CMorphInfo* pMorphInfo,DWORD*pAdditionalParameter=NULL)
	{
			CSurfaceInfo::CMorphInfo* pInfo
				= const_cast<CSurfaceInfo::CMorphInfo*>(pMorphInfo);
			Object obj(CSurfaceInfo::getWrappedPtr(
				type,pInfo,pAdditionalParameter));
			//	�����ɗa���Ă��܂�

			push(func(function_callback_r<LRESULT>::Create(
				&ISurface::GeneralMorph,(ISurface*)0,
				type,pSrc,pInfo,pAdditionalParameter))
				,
				obj
				);
			return 0;
	}

	virtual LRESULT GeneralEffect(CSurfaceInfo::EEffectType type,LPCRECT prc=NULL,
		DWORD*pAdditionalParameter=NULL)
	{
			Object obj(CSurfaceInfo::getWrappedPtr(
				type,prc,pAdditionalParameter));

			push(func(function_callback_r<LRESULT>::Create(
				&ISurface::GeneralEffect,(ISurface*)0,
				type,prc,pAdditionalParameter))
				,
				obj
				);
			return 0;
	}
	//	blt�n�ȊO�͎������ꂸ
	virtual int GetType() const { return 0; }
	virtual LRESULT GetSize(int& x,int& y) const { return 0; }
	virtual LRESULT SetSize(int x,int y) { return 0; }
	virtual ISurfaceRGB GetColorKey() const { return 0; }
	virtual smart_ptr<ISurface> clone() { return smart_ptr<ISurface>(new CSurfaceNullDevice);}
	virtual ISurfaceRGB	GetFillColor() const { return 0;}
	virtual LRESULT CreateSurfaceByType(int sx,int sy,int nType) { return 0; }

	virtual	LRESULT SetColorKey(ISurfaceRGB rgb) {
		push(func(function_callback_r<LRESULT>::Create(
			&ISurface::SetColorKey,(ISurface*)0,
			rgb)));
		return 0;
	}
	virtual LRESULT SetColorKeyPos(int x,int y){
		push(func(function_callback_r<LRESULT>::Create(
			&ISurface::SetColorKeyPos,(ISurface*)0,
			x,y)));
		return 0;
	}
	virtual LRESULT		Clear(LPCRECT lpRect=NULL){
		push(func(function_callback_r<LRESULT>::Create(
			&ISurface::Clear,(ISurface*)0,
			lpRect)));
		return 0;
	}
	virtual void		SetFillColor(ISurfaceRGB c) {
		push(func(function_callback_v::Create(
			&ISurface::SetFillColor,(ISurface*)0,
			c)));
	}
};

}} // end of namespace

#endif
