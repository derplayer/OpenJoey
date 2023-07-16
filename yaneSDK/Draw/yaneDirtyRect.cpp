#include "stdafx.h"
#include "yaneDirtyRect.h"
#include <limits.h>

namespace yaneuraoGameSDK3rd {
namespace Draw {

//	範囲指定タイプ
#pragma warning(disable:4701)	//	↓↓↓↓↓ 値の割り当てられていない可能性のあるローカル変数の代入warning

void	CDirtyRect::AddRect(int left,int top,int right,int bottom){
	int nRectLast = m_nRectLast;
	::SetRect(&m_arcRect[nRectLast],left,top,right,bottom);
	m_anRectArea[nRectLast] = (right-left)*(bottom-top);

	if (nRectLast!=nRectSize){
		//	まだ入りましゅので、そのまま加算したので終わりでしゅ
			//	面積を代入しておく
		m_nRectLast++;
		return ;
	}

	//	もうこれ以上、入りましぇん。
	//	ガーベジしましゅ

	//	《方針》
	//		互いに結合することによって、増加する面積が最小となるものを選択し、削除する
	int min_area = INT_MAX;			//	結合によって増加する面積の最小値
	int	min_a;						//	結合したときの面積
	int min_i,min_j;				// そのときのi,jの値
	int	nLeft,nTop,nRight,nBottom;	// そのときの矩形

	for(int i=0;i<nRectLast;i++){
		for(int j=i+1;j<nRectLast+1;j++){
			//	矩形iと矩形jを結合してみる

			//	結合前の面積
			int nAI = m_anRectArea[i];
			int nAJ = m_anRectArea[j];
			//	矩形の外側を取得
			int left	= m_arcRect[i].left	   < m_arcRect[j].left	 ? m_arcRect[i].left   : m_arcRect[j].left;
			int top		= m_arcRect[i].top	   < m_arcRect[j].top	 ? m_arcRect[i].top	   : m_arcRect[j].top;
			int right	= m_arcRect[i].right   > m_arcRect[j].right	 ? m_arcRect[i].right  : m_arcRect[j].right;
			int bottom	= m_arcRect[i].bottom  > m_arcRect[j].bottom ? m_arcRect[i].bottom : m_arcRect[j].bottom;
			//	結合された面積を取得
			int nAC = (right - left) * (bottom-top);
			//	結合することによって変化する差分を取得
			int nADiff = nAC - (nAI + nAJ);
			if (nADiff < min_area) {
				min_a	 = nAC;
				min_area = nADiff;
				min_i = i; min_j=j;
				nLeft = left; nTop = top; nRight = right; nBottom = bottom;
			}
		}
	}

	//	min_iとmin_jを結合する
		//	min_iに代入
	m_anRectArea[min_i] = min_a;
	::SetRect(&m_arcRect[min_i],nLeft,nTop,nRight,nBottom);
		//	min_jを消去
	if (min_j!=nRectLast) {
		//	コピる必要があるときだけコピる
		m_anRectArea[min_j] = m_anRectArea[nRectLast];
		m_arcRect[min_j] = m_arcRect[nRectLast];
	}
}
#pragma warning(default:4701)	//	↑↑↑↑↑

void	CDirtyRect::Refresh(){
	//	《方針》
	//		１．互いに結合することによって、増加する面積が０以下ならば、
	//			結合して削除する
	//		２．一方がもう一方に入り込んでいるのならば、そこを切断する
	
	int nRectLast = m_nRectLast;

	for(int i=0;i<nRectLast-1;i++){
		for(int j=i+1;j<nRectLast;){
			//	矩形iと矩形jを結合してみる

			//	結合前の面積
			int nAI = m_anRectArea[i];
			int nAJ = m_anRectArea[j];
			//	矩形の外側を取得
			int left	= m_arcRect[i].left	   < m_arcRect[j].left	 ? m_arcRect[i].left   : m_arcRect[j].left;
			int top		= m_arcRect[i].top	   < m_arcRect[j].top	 ? m_arcRect[i].top	   : m_arcRect[j].top;
			int right	= m_arcRect[i].right   > m_arcRect[j].right	 ? m_arcRect[i].right  : m_arcRect[j].right;
			int bottom	= m_arcRect[i].bottom  > m_arcRect[j].bottom ? m_arcRect[i].bottom : m_arcRect[j].bottom;
			//	結合された面積を取得
			int nAC = (right - left) * (bottom-top);
			//	結合することによって変化する差分を取得
			int nADiff = nAC - (nAI + nAJ);
			if (nADiff <= 0) {	//	結合することによって小さくなる
				//	iとjは結合する
				m_anRectArea[i] = nAC;
				::SetRect(&m_arcRect[i],left,top,right,bottom);
				//	jを消去
				if (j!=nRectLast-1) {
				//	コピる必要があるときだけコピる
					m_anRectArea[j] = m_anRectArea[nRectLast-1];
					m_arcRect[j] = m_arcRect[nRectLast-1];
				}
				nRectLast--;
			} else {
				//	削除が発生しなかったときのみjを加算
				j++;
			}
		}
	}
	m_nRectLast = nRectLast;
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
