#include "stdafx.h"
#include "yaneCounter.h"
#include <limits.h>

namespace yaneuraoGameSDK3rd {
namespace Math {

//////////////////////////////////////////////////////////////////////////////
//	static members..

smart_ptr<ICounterFactory> ICounter::m_vFactory;

//////////////////////////////////////////////////////////////////////////////

ICounter* ICounter::CreateInstance(int nType){
	switch(nType){
	case 0 : new CNullCounter;
	case 1 : new CRootCounter;
	case 2 : new CSaturationCounter;
	case 3 : new CInteriorCounter;
	default :
		if (!m_vFactory.isNull()){
			ICounter* p = m_vFactory->CreateInstance(nType);
			if (p!=NULL) return p;
		}
#ifdef USE_EXCEPTION
		throw CRuntimeException();
		//	�����^�C����O�𔭐�
#else
		return NULL;
#endif
	}
}

void CProxyCounter::Serialize(ISerialize&s){
	if (s.IsStoring()){
		int nType = GetCounter()->GetType();
		s << nType;	//	RTTI
		s << *GetCounter();
	} else {
		int nType;
		s << nType;			//	�^����factory�ɂ���ĕ���
		m_vCounter.Add(ICounter::CreateInstance(nType));
		s << *GetCounter();
	}
}

//////////////////////////////////////////////////////////////////////////////

CRootCounter::CRootCounter() {
	Set(0,INT_MAX,1);
	Reset();
}

CRootCounter::CRootCounter(int nEnd){
	Set(0,nEnd,1);
	Reset();
}

CRootCounter::CRootCounter(int nStart,int nEnd,int nStep){
	Set(nStart,nEnd,nStep);
	Reset();
}

void CRootCounter::inc(bool bAdd){
	bool bInc = (m_nStart > m_nEnd) ^ bAdd; // �t�����J�E���^�H
	if (bInc) {
	//	�C���N�������g
		if (m_nStep>0) {
		//	�����C���N�������g
			m_nRootCount += m_nStep;
		} else {
		//	�����C���N�������g
			m_nRate++; if (m_nRate>=(-m_nStep)) { m_nRate = 0; m_nRootCount++; }
		}
		//	�T�`�����[�g�����̂��H
		int nMax = m_nStart < m_nEnd ? m_nEnd : m_nStart;
		if (m_nRootCount > nMax) {
			int nMin = m_nStart < m_nEnd ? m_nStart : m_nEnd;
			m_nRootCount = nMin;
			//		CRootCounter�Ƃ͂��������Ⴄ����
		}
	} else {
	//	�f�N�������g
		if (m_nStep>0) {
		//	�����f�N�������g
			m_nRootCount -= m_nStep;
		} else {
		//	�����f�N�������g
			m_nRate--; if (m_nRate<=(m_nStep)) { m_nRate = 0; m_nRootCount--; }
			//	�ˁ@m_nRate++�łȂ����Ƃɒ��ӁB
			//	++�̂���--���āA���̐��������Ƃ�Ȃ��Ă͂Ȃ�Ȃ�
			//	���AnStep<0�̂Ƃ��ŏ��̂P��ڂ�--��RootCounter��
			//	1�����Ă͂����Ȃ��B����Ă������������ɂȂ�
		}
		//	�T�`�����[�g�����̂��H
		int nMin = m_nStart < m_nEnd ? m_nStart : m_nEnd;
		if (m_nRootCount < nMin) {
			int nMax = m_nStart < m_nEnd ? m_nEnd : m_nStart;
			m_nRootCount = nMax;
			//		CRootCounter�Ƃ͂��������Ⴄ����
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

CSaturationCounter::CSaturationCounter() {
	Set(0,INT_MAX,1);
	Reset();
}

CSaturationCounter::CSaturationCounter(int nEnd){
	Set(0,nEnd,1);
	Reset();
}

CSaturationCounter::CSaturationCounter(int nStart,int nEnd,int nStep){
	Set(nStart,nEnd,nStep);
	Reset();
}

void CSaturationCounter::inc(bool bAdd){
	bool bInc = (m_nStart > m_nEnd) ^ bAdd; // �t�����J�E���^�H
	if (bInc) {
	//	�C���N�������g
		if (m_nStep>0) {
		//	�����C���N�������g
			m_nRootCount += m_nStep;
		} else {
		//	�����C���N�������g
			m_nRate++; if (m_nRate>=(-m_nStep)) { m_nRate = 0; m_nRootCount++; }
		}
		//	�T�`�����[�g�����̂��H
		int nMax = m_nStart < m_nEnd ? m_nEnd : m_nStart;
		if (m_nRootCount > nMax) m_nRootCount = nMax;
	} else {
	//	�f�N�������g
		if (m_nStep>0) {
		//	�����f�N�������g
			m_nRootCount -= m_nStep;
		} else {
		//	�����f�N�������g
			m_nRate--; if (m_nRate<=(m_nStep)) { m_nRate = 0; m_nRootCount--; }
			//	�ˁ@m_nRate++�łȂ����Ƃɒ��ӁB
			//	++�̂���--���āA���̐��������Ƃ�Ȃ��Ă͂Ȃ�Ȃ�
			//	���AnStep<0�̂Ƃ��ŏ��̂P��ڂ�--��RootCounter��
			//	1�����Ă͂����Ȃ��B����Ă������������ɂȂ�
		}
		//	�T�`�����[�g�����̂��H
		int nMin = m_nStart < m_nEnd ? m_nStart : m_nEnd;
		if (m_nRootCount < nMin) m_nRootCount = nMin;
	}
}
//////////////////////////////////////////////////////////////////////////////

CInteriorCounter::CInteriorCounter(){
	m_nStart	= 0;
	m_nEnd		= 0;
	m_nNow		= 0;
	m_nFrames	= 0;
	m_nFramesNow = 0;
}

void	CInteriorCounter::Set(int nStart,int nEnd,int nFrames){
	WARNING(nFrames == 0,"CInteriorCounter::Set��nFrames == 0");
	m_nStart	= nStart;
	m_nEnd		= nEnd;
	m_nNow		= nStart;
	m_nFrames	= nFrames;
	m_nFramesNow = 0;
}

void	CInteriorCounter::Inc(){
	//	�J�E���^�͏I���l���H
	if (m_nFramesNow >= m_nFrames) {
		m_nNow = m_nEnd;
		return ;
	}
	m_nFramesNow++;
	//	��������
	m_nNow =  m_nStart + m_nFramesNow * (m_nEnd-m_nStart) / m_nFrames;
}

void	CInteriorCounter::Dec(){
	//	�J�E���^�͏����l���H
	if (m_nFramesNow == 0) return ;
	m_nFramesNow--;
	//	��������
	m_nNow =  m_nStart + m_nFramesNow * (m_nEnd-m_nStart) / m_nFrames;
}
//////////////////////////////////////////////////////////////////////////////

} // end of namespace Math
} // end of namespace yaneuraoGameSDK3rd
