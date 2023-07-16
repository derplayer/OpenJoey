#include "stdafx.h"
#include "yaneDirtyRect.h"
#include <limits.h>

namespace yaneuraoGameSDK3rd {
namespace Draw {

//	�͈͎w��^�C�v
#pragma warning(disable:4701)	//	���������� �l�̊��蓖�Ă��Ă��Ȃ��\���̂��郍�[�J���ϐ��̑��warning

void	CDirtyRect::AddRect(int left,int top,int right,int bottom){
	int nRectLast = m_nRectLast;
	::SetRect(&m_arcRect[nRectLast],left,top,right,bottom);
	m_anRectArea[nRectLast] = (right-left)*(bottom-top);

	if (nRectLast!=nRectSize){
		//	�܂�����܂���̂ŁA���̂܂܉��Z�����̂ŏI���ł���
			//	�ʐς������Ă���
		m_nRectLast++;
		return ;
	}

	//	��������ȏ�A����܂�����B
	//	�K�[�x�W���܂���

	//	�s���j�t
	//		�݂��Ɍ������邱�Ƃɂ���āA��������ʐς��ŏ��ƂȂ���̂�I�����A�폜����
	int min_area = INT_MAX;			//	�����ɂ���đ�������ʐς̍ŏ��l
	int	min_a;						//	���������Ƃ��̖ʐ�
	int min_i,min_j;				// ���̂Ƃ���i,j�̒l
	int	nLeft,nTop,nRight,nBottom;	// ���̂Ƃ��̋�`

	for(int i=0;i<nRectLast;i++){
		for(int j=i+1;j<nRectLast+1;j++){
			//	��`i�Ƌ�`j���������Ă݂�

			//	�����O�̖ʐ�
			int nAI = m_anRectArea[i];
			int nAJ = m_anRectArea[j];
			//	��`�̊O�����擾
			int left	= m_arcRect[i].left	   < m_arcRect[j].left	 ? m_arcRect[i].left   : m_arcRect[j].left;
			int top		= m_arcRect[i].top	   < m_arcRect[j].top	 ? m_arcRect[i].top	   : m_arcRect[j].top;
			int right	= m_arcRect[i].right   > m_arcRect[j].right	 ? m_arcRect[i].right  : m_arcRect[j].right;
			int bottom	= m_arcRect[i].bottom  > m_arcRect[j].bottom ? m_arcRect[i].bottom : m_arcRect[j].bottom;
			//	�������ꂽ�ʐς��擾
			int nAC = (right - left) * (bottom-top);
			//	�������邱�Ƃɂ���ĕω����鍷�����擾
			int nADiff = nAC - (nAI + nAJ);
			if (nADiff < min_area) {
				min_a	 = nAC;
				min_area = nADiff;
				min_i = i; min_j=j;
				nLeft = left; nTop = top; nRight = right; nBottom = bottom;
			}
		}
	}

	//	min_i��min_j����������
		//	min_i�ɑ��
	m_anRectArea[min_i] = min_a;
	::SetRect(&m_arcRect[min_i],nLeft,nTop,nRight,nBottom);
		//	min_j������
	if (min_j!=nRectLast) {
		//	�R�s��K�v������Ƃ������R�s��
		m_anRectArea[min_j] = m_anRectArea[nRectLast];
		m_arcRect[min_j] = m_arcRect[nRectLast];
	}
}
#pragma warning(default:4701)	//	����������

void	CDirtyRect::Refresh(){
	//	�s���j�t
	//		�P�D�݂��Ɍ������邱�Ƃɂ���āA��������ʐς��O�ȉ��Ȃ�΁A
	//			�������č폜����
	//		�Q�D�������������ɓ��荞��ł���̂Ȃ�΁A������ؒf����
	
	int nRectLast = m_nRectLast;

	for(int i=0;i<nRectLast-1;i++){
		for(int j=i+1;j<nRectLast;){
			//	��`i�Ƌ�`j���������Ă݂�

			//	�����O�̖ʐ�
			int nAI = m_anRectArea[i];
			int nAJ = m_anRectArea[j];
			//	��`�̊O�����擾
			int left	= m_arcRect[i].left	   < m_arcRect[j].left	 ? m_arcRect[i].left   : m_arcRect[j].left;
			int top		= m_arcRect[i].top	   < m_arcRect[j].top	 ? m_arcRect[i].top	   : m_arcRect[j].top;
			int right	= m_arcRect[i].right   > m_arcRect[j].right	 ? m_arcRect[i].right  : m_arcRect[j].right;
			int bottom	= m_arcRect[i].bottom  > m_arcRect[j].bottom ? m_arcRect[i].bottom : m_arcRect[j].bottom;
			//	�������ꂽ�ʐς��擾
			int nAC = (right - left) * (bottom-top);
			//	�������邱�Ƃɂ���ĕω����鍷�����擾
			int nADiff = nAC - (nAI + nAJ);
			if (nADiff <= 0) {	//	�������邱�Ƃɂ���ď������Ȃ�
				//	i��j�͌�������
				m_anRectArea[i] = nAC;
				::SetRect(&m_arcRect[i],left,top,right,bottom);
				//	j������
				if (j!=nRectLast-1) {
				//	�R�s��K�v������Ƃ������R�s��
					m_anRectArea[j] = m_anRectArea[nRectLast-1];
					m_arcRect[j] = m_arcRect[nRectLast-1];
				}
				nRectLast--;
			} else {
				//	�폜���������Ȃ������Ƃ��̂�j�����Z
				j++;
			}
		}
	}
	m_nRectLast = nRectLast;
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
