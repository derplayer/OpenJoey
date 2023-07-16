// yaneLayer.h

#ifndef __yaneLayer_h__
#define __yaneLayer_h__


#if defined(yaneSDK_CodeWarrior_Mode)
#include "yaneSurface.h" //	�N���X�̒�`�t�@�C����include���Ă��Ȃ��Ƃ���
#endif

namespace yaneuraoGameSDK3rd {
namespace Draw {

class ISurface;
class ILayerList;

class ILayerBase {
/**
	���C���̊��N���X�B
	������A�`�惆�j�b�g(CFastDraw��)�̃��C���R�[���o�b�N�ɓo�^���Ďg�p����B

*/
public:
	///		�`�悷��R�[���o�b�N(�ȉ��̂ǂ��炩���I�[�o�[���C�h����)
	virtual	void	OnDraw(ISurface*lpDraw) {}
	virtual	void	OnDraw(HDC) {}

	///		�|�W�V�����w��/�擾
	void	SetPos(int x,int y)	 { m_nX = x; m_nY = y; }
	void	GetPos(int& x,int& y) const { x = m_nX; y = m_nY; }

	///		�`���L���^����������(�f�B�t�H���g�ŗL��)
	void	Enable(bool bEnable) { m_bEnable = bEnable; }
	bool	IsEnable() const { return m_bEnable; }

	///		LayerList���R���X�g���N�^�œn���΁A�����I�Ƀt�b�N����
	ILayerBase(ILayerList*pLayerList=NULL);

	void	SetLayerList(ILayerList*pLayerList);
	/**
		LayerList��ݒ肷��B(���̏u�ԂɃt�b�N����)
	*/

	///		�f�X�g���N�^�ŁALayerList����t�b�N���Ă����̂��O��
	virtual ~ILayerBase();

protected:
	int		m_nX,m_nY;	//	�I�t�Z�b�g���W
	bool	m_bEnable;

	ILayerList* m_pLayerList;
	/*
		�������t�b�N���Ă��郌�C�����X�g��ێ����Ă����A�f�X�g���N�^��
		�t�b�N����������
	*/
};

class ILayerList {
public:
	virtual void	Add(ILayerBase*)=0;
	virtual void	Del(ILayerBase*)=0;
	virtual void	Clear()=0;
	virtual bool	IsEmpty() const=0;
	
	virtual void	OnDraw(ISurface*)=0;
	virtual void	OnDraw(HDC)=0;

	virtual ~ILayerList(){}
};

class CLayerList : public ILayerList {
public:
	virtual void	Add(ILayerBase*);			///	�������g���t�b�N�ɉ�����
	virtual void	Del(ILayerBase*);			///	�������g���t�b�N����O��
	virtual void	Clear();					///	���ׂĂ��N���A����
	virtual bool	IsEmpty() const;			///	��ł��邩�H

	///	�����̕ێ����Ă���t�b�N���X�g�ɑ΂���OnDraw���Ăяo��
	virtual void	OnDraw(ISurface*);
	virtual void	OnDraw(HDC);

protected:
	list_chain<ILayerBase>	m_listLayer;	//	�t�b�N���X�g
	list_chain<ILayerBase>* GetList() { return& m_listLayer; }
};

////////////////////////////////////////////////////////////////////////////

class CSurfaceLayer : public ILayerBase {
/**
	ISurface*��ێ����Ă��āAOnDraw(ISurface)�ɂ����āA
	�����BltNatural�ŕ`�悷�邾���̃��C��
*/
public:

	CSurfaceLayer(ISurface* pSurface,ILayerList*pLayerList=NULL)
		: m_pSurface(pSurface) , ILayerBase(pLayerList) {}
	CSurfaceLayer(const smart_ptr<ISurface>& pSurface,ILayerList*pLayerList=NULL)
		: m_pSurface(pSurface),ILayerBase(pLayerList) {}

protected:
	virtual void	OnDraw(ISurface*);
	smart_ptr<ISurface>	m_pSurface;
};

}}

#include "yaneFont.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

class CTextLayer : public ILayerBase {
/**
	CFont�������āAOnDraw�ɂ����āACFont::OnDraw���Ăяo���ĕ`�悷��
*/
public:
	///	����Ńt�H���g���擾���Đݒ���s�Ȃ�����
	CFont* GetFont() { return& m_vFont; }

	CTextLayer(ILayerList*pLayerList=NULL):ILayerBase(pLayerList){}
	///	HDC�o�R�ŕ`�悵�Ă���̂ŁAHDCLayer�Ƀt�b�N�����邱�ƁI

protected:
	virtual void	OnDraw(HDC);
	CFont	m_vFont;
};

} namespace Timer { class CFPSTimer; } namespace Draw {

class CFPSLayer : public CTextLayer {
public:

	CFPSLayer(CFPSTimer*pFPSTimer,ILayerList*pLayerList=NULL);

	CFPSTimer* GetFPSTimer() { return m_pFPSTimer; }

protected:
	virtual void	OnDraw(HDC);
	CFPSTimer* m_pFPSTimer;
};

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd

#endif
