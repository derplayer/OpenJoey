#ifndef __YTLMediator_h__
#define __YTLMediator_h__

namespace yaneuraoGameSDK3rd {
namespace YTL {

template <class T>
class mediator {
public:
	mediator(T* pT=NULL) : m_pT(pT) {}
	void SetOutClass(T*pT) { m_pT = pT;}
	T& GetOutClass() const { return *m_pT;}
private:
	T*	m_pT;
};

#define outer GetOutClass()
	//	これを用意しておけば、
	//	outerというキーワードをあたかも外部クラスを指定する
	//	演算子のように使うことが出来る

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
