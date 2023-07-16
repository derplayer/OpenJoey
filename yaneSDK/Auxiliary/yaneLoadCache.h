// yaneLoadCache.h

#ifndef __yaneLoadCache_h__
#define __yaneLoadCache_h__

#include <LIMITS.h>	//	INT_MAX

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class CIDObjectManager {
/**
	ID�Ƃ���ɑΉ�������̂�Ԃ�
	���̂�smart_obj(�ǂ�ȃI�u�W�F�N�g�ł�����X�}�[�g�|�C���^)�ŊǗ�
	����Ă���̂ŁA���ɕ֗��B
*/
public:
	typedef map<int,smart_obj> CIDObject;

	void	clear();
	///	�R���e�i�����ׂč폜����

	LRESULT	erase(int nID);
		///	�w�肵��ID�̂��̂��폜����
		///	�R���e�i�����݂��A�����폜�ł����Ƃ��́A0���Ԃ�

	void	setObject(int nID,const smart_obj& object);
		///	ID�ɑΉ�����I�u�W�F�N�g��ݒ肷��

	LRESULT getObject(int nID,smart_obj& object);
		///	�w�肵��ID�ɑΉ�����I�u�W�F�N�g���擾����

	CIDObject* GetObject() { return& id_object_; }

protected:
	CIDObject	id_object_;
};

///////////////////////////////////////////////////////////////

struct CLoadCacheInfo
/**
	CLoadCache�ŗp����\����
*/
{
	smart_obj	pObj;		//	�I�u�W�F�N�g(CFastPlane�Ƃ�CSound�Ƃ�..)
	string		strFileName;// �ǂݍ��ނׂ��t�@�C����
};

class ICacheStaleListener
/**
	cache���Ă�����̂̂Ȃ�����
	�Â��Ȃ����f�[�^���̂Ă邽�߂̎d�g��
*/
{
public:
	virtual LRESULT OnLoad(const smart_obj& pObj) = 0;
	/**
		pObj��ǂݍ��񂾂Ƃ��ɌĂяo�����
	*/

	virtual LRESULT OnRelease(const smart_obj& pObj) = 0;
	/**
		pObj����������Ƃ��ɌĂяo�����

		�����Ƃ́A���̃��X�i�̂Ȃ��ōs�Ȃ�Ȃ��Ă��ǂ�
	*/

	virtual bool isFull() const = 0;
	/**
		�L���b�V������ɂȂ��Ă���Ȃ� true
		�ˁ@���̂Ƃ��Â����̂���������ׂ�
	*/

	virtual ~ICacheStaleListener() {}
};

class CDefaultCacheStaleListener : public ICacheStaleListener
/**
	�f�B�t�H���g��Staler
*/
{
public:
	virtual LRESULT OnLoad(const smart_obj& pObj) { m_nRead++; return 0; }
	virtual LRESULT OnRelease(const smart_obj& pObj) { m_nRead--; return 0; }
	virtual bool isFull() const { return m_nRead > m_nMax; }

	void	SetMax(int nMax) { m_nMax = nMax; }
	/**
		���̐������L���b�V������B������I�[�o�[�����ꍇ��
		�Â����̂���������
	*/

	CDefaultCacheStaleListener() { m_nMax = INT_MAX; m_nRead = 0; }

protected:
	int m_nRead; // �ǂݍ���ł���f�[�^�̐�
	int m_nMax;	 // �L���b�V�����ׂ��f�[�^�̏��
};


class ILoadCacheListener {
/**
	class CLoadCache �̓ǂݍ��ݎ��̃R�[���o�b�N�B
	�K�v�Ȃ��nNo��ύX�ł���
	�ǂݍ��ނ͂��̃t�@�C���𑼂̃t�@�C���֒u������̂Ɏg���B
*/
public:
	virtual void OnLoad(int &nNo) = 0;
	virtual ~ILoadCacheListener() {}
};

class CDefaultLoadCacheListener : public ILoadCacheListener {
/**
	ILoadCacheListener�̉������Ȃ���
*/
public:
	virtual void OnLoad(int &nNo) {}
};

class ILoadCache {
public:
	virtual LRESULT	Set(const string& filename,bool bUseID=false)=0;

	virtual LRESULT	Load(int nNo)=0;
	virtual LRESULT	Release(int nNo)=0;
	virtual LRESULT	ReleaseAll()=0;

	virtual LRESULT GetObj(int nNo,smart_obj& obj)=0;

	virtual void	SetLoadCacheListener(const smart_ptr<ILoadCacheListener>& v)=0;
	virtual smart_ptr<ILoadCacheListener> GetLoadCacheListener() const =0;

	virtual void SetCacheStaleListener(const smart_ptr<ICacheStaleListener>& p)=0;
	virtual smart_ptr<ICacheStaleListener> GetCacheStaleListener() const=0;

	virtual void	SetCanReloadAgain(bool b)=0;

	virtual void	SetReadDir(const string& path)=0;
	virtual string	GetReadDir() const = 0;

	virtual ~ILoadCache(){}
};

}}

#include "yaneFile.h"
#include "yaneStringScanner.h"

//	template�ŏ����Ă��镔���̂��߂Ɂ���include���K�v�ɂȂ�(;�L�D`)
//	inclusion model��template�͊��ق��Ă���[

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class CLoadCache : public ILoadCache {
/**
	�[���L���b�V���̂悤�ȓ��������܂��B
	�g�p���@���ɂ��ẮA���̃N���X�̔h�������Ă��������B

	�h���N���X�F
		class CSoundLoader
		class CPlaneLoader
*/
public:
	virtual LRESULT	Set(const string& filename,bool bUseID=false)
		{ return SetHelper(filename,bUseID,CLoadCacheInfo()); }
	/**
		��`�t�@�C���̐ݒ�i�ǂݍ��݁j
		bUseID == true�Ȃ�΁A��`�t�@�C���́A�t�@�C�����̗���ł͂Ȃ��A
			�t�@�C���� , ID�ԍ�
		�̗���ł���B���̂Ƃ��w�肳�ꂽID�ԍ��Ƃ��ĎQ�Ƃ��邱�Ƃ��o����
	*/

	template <class T>
	LRESULT SetHelper(const string& filename,bool bUseID,T t){

		//	�ǂݍ��ݍς݁��Q�x�ǂ݋֎~
		if (!m_bCanReloadAgain) return 0;

		//	�t�@�C������ݒ�t�@�C����ǂݍ��ނƂ��̂��߂ɁD�D�D
		CFile file;
		if (file.Read(filename)!=0) return 1;

		int nNums = 0;
		//	���ݒ肳���t�@�C����
		while (true){
			CHAR buf[256];
			LRESULT lr = file.ReadLine(buf);
			if (lr==1) break ; // EOF
			if (buf[0]=='\0') continue;		//	��s�̓ǂݔ�΂�
			if (buf[0]=='/' && buf[1]=='/') continue;	//	�R�����g�s�̓ǂݔ�΂�
			LPCSTR lp = buf;
			T* pInfo = new T;
			pInfo->strFileName = m_strReadDir + CStringScanner::GetStrFromCsv(lp);

			smart_obj obj(pInfo);

			int nID;
			if (bUseID){
				if (CStringScanner::GetNumFromCsv(lp,nID)!=0){
					return 2;	//	����A�ǂݍ��݃G���[�ł��H
				}
			} else {
				nID = nNums;
			}
			GetIDObjectManager()->setObject(nID,obj);
			OnSet(obj,nID);		//	�K�v�Ȃ�Ώ���ǋL	
			nNums ++;
		}
		return 0;
	}
	/**
		Set����Ăяo���Ďg���ׂ�helper�֐�
		�Q�l�ˁ@class CSELoader
	*/

	virtual LRESULT	Load(int nNo);
	/**
		���̃i���o�[�̃f�[�^�̓ǂݍ���
		�t�@�C�����ɂ��������āA�I�u�W�F�N�g�͎��̉������
	*/

	/**
		Set��Load�́A�y�A�Ŏg���B
		���̂Ƃ��A���X�iICacheStaleListener��OnLoad,OnRelease�̈����Ŏ󂯎��
		smart_obj�̐��̂�CLoadCacheInfo�Ȃ̂�
		smart_ptr<CLoadCacheInfo>* pInfo = obj.get()
		�̂悤�ɃL���X�g���Ďg���B
	*/

	virtual LRESULT GetObj(int nNo,smart_obj& obj);
	/**
		nNo�ɑΉ�����I�u�W�F�N�g���擾����
	*/

	virtual LRESULT	Release(int nNo);
	///	���̃i���o�[�̃f�[�^�̉��

	virtual LRESULT	ReleaseAll();
	/**
		�ǂݍ���ł���f�[�^�̑S���

		�������A����CLoadCache�̃C���X�^���X���ǂݍ��񂾂��̂̂݁B
		�L���b�V�������L���Ă��鑼��CLoadCache���ǂݍ��񂾂��̂͊܂܂�Ȃ��B
	*/

	virtual void SetCacheStaleListener(
		const smart_ptr<ICacheStaleListener>& p)
	/**
		�L���b�V�������ݒ肷��d�g�݁B
		���̃L���b�V���V�X�e���ŁA�L���b�V����������^���郊�X�i�B
		�f�B�t�H���g�ł́ACDefaultCacheStaler�B
		����́A�ǂݍ���ł��鐔��CDefaultCacheStaler::SetMax�𒴂����ꍇ
		�ŋߎg���Ă��Ȃ����̂���������d�g��
		default�ł�INT_MAX�i�Ȃ̂ł��܂ł��������Ȃ��j
	*/
	{	m_pCacheStaleListener = p;	}

	smart_ptr<ICacheStaleListener>	GetCacheStaleListener() const
		{ return m_pCacheStaleListener; }

	struct CIDObjectAndNumber{
	/**
		CacheManager�́A��ԌÂ��I�u�W�F�N�g��j������@�\��
		�K�v�Ȃ̂ŁACIDObjectManager�̃|�C���^�ƁA�i���o�[�������Ă���
	*/
		CIDObjectManager * pIDObject;
		int			nNum;
		CIDObjectAndNumber():pIDObject(NULL),nNum(0) {}
	};

	typedef fast_list<CIDObjectAndNumber> CCacheManager;
	///	fast_list��YTL�Œ�`����Ă���Blist�̑�������Ǝv���˂��B

	/**
		CacheManager�Ɋւ���A�ݒ�`�擾�֐�
		�����̃v���[�����[�_�[�ŁA�ꌳ�I�ȃL���b�V�����s�Ȃ������Ƃ��́A
			CPlaneLoader p1,p2;
			smart_ptr<CLoadCache::CCacheManager> pCache;
			pCache = p2.GetCacheManager();
			p1.SetCacheManager(pCache);
		�Ƃ����悤�ɁA�L���b�V���}�l�[�W�����g���L�h���邱�Ƃɂ����
		�ӂ���CPlaneLoader�ŁA�g�[�^���P�O���̃O���t�B�b�N��
		�L���b�V������Ƃ������Ƃ��\�ɂȂ�܂��B�i���ł��j
	*/
	smart_ptr<CCacheManager> GetCacheManager() const
		{ return m_vCacheManager; }

	void	SetCacheManager(const smart_ptr<CCacheManager>& pCacheManager)
		{ m_vCacheManager = pCacheManager; }

	CIDObjectManager* GetIDObjectManager() { return& m_vIDObject; }

	///	---- ���܂�
	///	Set�Őݒ肵���A�t�@�C�������X�g�̗v�f����Ԃ��܂��B
	virtual int		GetSize() const;

	///	property..
	///	���X�i�̐ݒ�`�擾
	virtual void	SetLoadCacheListener(const smart_ptr<ILoadCacheListener>& v)
		{ m_pLoadCacheListener = v;}
	virtual smart_ptr<ILoadCacheListener> GetLoadCacheListener() const
		{ return m_pLoadCacheListener; }


	/**
		Set�Ŏw�肷��t�@�C���ŏ�����Ă���t�@�C����ǂݍ��ނƂ��A
		�����Őݒ肳�ꂽDir���������Ă��珈������B

		��)
			SetReadDir("../Read/");
			//	�ˁ@"test.bmp"��ǂݍ��ނƂ��A
			//		../Read/test.bmp�������ɓǂݍ���

			���ӁF
			�P�D�ݒ肷��Ƃ��A���̍Ō�̃X���b�V����Y��Ȃ����ƁI
			�Q�DSet���Ăяo���O�ɂ��̊֐��Őݒ肷�邱�ƁI
	*/

	virtual void	SetReadDir(const string& strDir)
		{ m_strReadDir = strDir; }

	virtual string	GetReadDir() const 
		{ return m_strReadDir; }

	//	�ݒ�t�@�C���̂Q�x�ǂ݂̋֎~(default:true == �Q�x�ǂ݉\)
	virtual void	SetCanReloadAgain(bool b) { m_bCanReloadAgain = b; }

	CLoadCache();
	virtual ~CLoadCache();

protected:

	/**
		���̃N���X�̎g�������F

		�܂��A���̃N���X����h�������A
		����CCacheManager(GetCacheManager�Ŏ擾�ł���)��
		delegate�ݒ�֐��Ŋ֐����Z�b�g���܂��B
	*/

	/////////////////////////////////////////////////////////

	smart_ptr<ILoadCacheListener>	m_pLoadCacheListener;
	smart_ptr<ICacheStaleListener>	m_pCacheStaleListener;

	//	�L���b�V���V�X�e��
	smart_ptr<CCacheManager>		m_vCacheManager;

	//	���I�u�W�F�N�g
	CIDObjectManager m_vIDObject;

	/*
		CIDObjectManager�ɂ́A���ׂẴI�u�W�F�N�g���o�^����Ă���
		�������A���̂Ȃ��Ɉ͂܂�Ă���I�u�W�F�N�g�����̉��������
		����Ƃ͌���Ȃ�

		CCacheManager�ɂ́AMRU(�ŋߎg�p�������j�b�g)��ID�ԍ��݂̂�
		�Ǘ�����Ă���B
	*/

	CIDObjectAndNumber GetIDObjectAndNumber(int n)
	{
		CIDObjectAndNumber o;
		o.pIDObject = &m_vIDObject;
		o.nNum = n;
		return o;
	}

	//	�ݒ�t�@�C���̂Q�x�ǂ݋֎~�t���O
	bool	m_bCanReloadAgain;

	//	�t�@�C�������݂���p�X(���Ύw��)
	string	m_strReadDir;

	virtual LRESULT InnerLoad(const smart_obj& obj) = 0;
	/**
		�ǂݍ��ݍ��
		�ǂݍ��݂̌��ʃG���[�Ȃ�Δ�0��Ԃ��悤�ɃR�[�f�B���O����
	*/

	virtual void OnSet(const smart_obj& obj,int nID){}
	/**
		Set�Ńt�@�C������ǂݍ��ނƂ��ɒǉ��������L�q��������΁A
		���̊֐����I�[�o�[���C�h���邱��
	*/

};

//	�����t���̂��߂ɕK�v
inline bool operator < (const CLoadCache::CIDObjectAndNumber& c1, const CLoadCache::CIDObjectAndNumber& c2)
{
	return (c1.pIDObject<c2.pIDObject) || (c1.pIDObject==c2.pIDObject && c1.nNum<c2.nNum);
}

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

////////////////////////////////////////////////////////////////

#endif
