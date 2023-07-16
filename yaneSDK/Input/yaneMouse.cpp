#include "stdafx.h"
#include "yaneMouse.h"
#include "../AppFrame/yaneAppManager.h"

namespace yaneuraoGameSDK3rd {
namespace Input {

CMouse::CMouse(){
	m_bOutScreenInput = false;
	m_bRB = false;
	m_bLB = false;
	m_bHistRB = false;
	m_bHistLB = false;
	CAppManager::Hook(this);		//	WindowMessageのHook
}

CMouse::~CMouse(){
	CAppManager::Unhook(this);		//	WindowMessageのUnhook
}

//////////////////////////////////////////////////////////////////////////////

LRESULT	CMouse::GetXY(int &x,int &y) const {
	POINT point,point2 = { 0,0 };
	::GetCursorPos(&point);

	if (CAppManager::IsFullScreen()) {
	//	フルスクリーンモードでは見えていないウィンドゥキャプション等の影響で
	//	嘘の位置になってしまうので、そのまま返しておけば良い。
		x = point.x;
		y = point.y;
	} else {
		HWND hWnd = CAppManager::GetHWnd();
		::ClientToScreen(hWnd,&point2);
		x = point.x - point2.x;
		y = point.y - point2.y;

		if (!m_bOutScreenInput) {
			//	ウィンドゥ範囲外であれば、ボタンはリセット
			::ScreenToClient(hWnd,&point);// クライアント座標に変換
			RECT rt;
			::GetClientRect(hWnd,&rt);// クライアント領域取得
			// 範囲外ならマウス状態初期化
			if(point.x<0 || rt.right<=point.x || point.y<0 || rt.bottom<=point.y) {
				//	const_cast fake
				*const_cast<bool*>(&m_bLB) = false;
				*const_cast<bool*>(&m_bRB) = false;
			}
		}
		
	}
	return 0;
}

LRESULT CMouse::SetXY(int x,int y) {
	POINT point,point2 = { 0,0 };

	if (CAppManager::IsFullScreen()) {
		point.x=x;
		point.y=y;
		::SetCursorPos(point.x,point.y);
	} else {
		::ClientToScreen(CAppManager::GetHWnd(),&point2);
		point.x= x + point2.x;
		point.y= y + point2.y;
		::SetCursorPos(point.x,point.y);
	}
	return 0;
}

LRESULT CMouse::GetInfo(int &x,int &y,int &b) const {	// マウスポジションとボタン状態を返す
	CMouse::GetXY(x,y);	//	virtual関数が呼び出されけては何もならない

	b = 0;
	if (m_bRB) b++;		//	ボタン情報。右クリックならば1
	if (m_bLB) b+=2;	//	ボタン情報。左クリックならば2

	return 0;
}

bool CMouse::RButton() const {
	return m_bRB;
}

bool CMouse::LButton() const {
	return m_bLB;
}

void CMouse::SetOutScreenInput(bool bEnable) { m_bOutScreenInput=bEnable;}

//////////////////////////////////////////////////////////////////////////////

void	CMouse::GetButton(bool&bL,bool&bR){
	bL = !m_bHistLB && m_bLB;
	bR = !m_bHistRB && m_bRB;
	m_bHistLB = m_bLB;
	m_bHistRB = m_bRB;
}

void	CMouse::ResetButton(){
	m_bHistRB = false;
	m_bHistLB = false;
}

//////////////////////////////////////////////////////////////////////////////
// メッセージをフックしないとマウスのボタン状態もわからんとは...
LRESULT CMouse::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){ // メッセージのコールバック
	switch(uMsg){
	/*
		押したまま画面外に出たときに、どう処理するのか？
		(画面外ではWM_MOUSEMOVEメッセージはやって来ないので..)
		⇒WM_NCBUTTONUP等はやってくる？　⇒やって来ないみたい(ρ_；)ﾉ
	*/

/*
	//	⇒このメッセージも必ず飛んで来るとは限らないようだ．．
	case WM_NCHITTEST:
	{
		POINT pos;
		RECT rt;
		pos.x = LOWORD(lParam);
		pos.y = HIWORD(lParam);
		::ScreenToClient(hWnd,&pos);// クライアント座標に変換
		GetClientRect(hWnd,&rt);// クライアント領域取得
		// 範囲外ならマウス状態初期化
		if(pos.x<0 || rt.right<=pos.x || pos.y<0 || rt.bottom<=pos.y) {
			m_bLB = false;
			m_bRB = false;
		}
		break;
	}
*/

	case WM_LBUTTONDOWN:
//	case WM_NCLBUTTONDOWN:
		m_bLB = true; break;
	case WM_LBUTTONUP:
//	case WM_NCLBUTTONUP:
		m_bLB = /* m_bLDC = */ false; break;
	case WM_RBUTTONDOWN:
//	case WM_NCRBUTTONDOWN:
		m_bRB = true; break;
	case WM_RBUTTONUP:
//	case WM_NCRBUTTONUP:
		m_bRB = /* m_bRDC = */ false; break;
/*	//	このメッセージは飛んでこない
	case WM_LBUTTONDBLCLK:
//	case WM_NCLBUTTONDBLCLK:
		m_bLDC= true; break;
	case WM_RBUTTONDBLCLK:
//	case WM_NCRBUTTONDBLCLK:
		m_bRDC= true; break;
*/
	case WM_MOUSEMOVE:
//	case WM_NCMOUSEMOVE:	//	画面外にマウスが出たときのために
		m_bLB = (wParam & MK_LBUTTON)!=0;
		m_bRB = (wParam & MK_RBUTTON)!=0;
		break;

	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

CFixMouse::CFixMouse(){
	m_bRBN = false;
	m_bLBN = false;
	m_bHistRB = false;
	m_bHistLB = false;
	m_nRLBN = 0;
	m_nX = m_nY = 0;
	m_nGuardTime = 2;
}

CFixMouse::~CFixMouse(){
}

LRESULT	CFixMouse::Flush(){
	if (m_nGuardTime!=0) m_nGuardTime--;

	m_bHistLB = m_bLBN;
	m_bHistRB = m_bRBN;

	if (GetMouse()->GetInfo(m_nX,m_nY,m_nRLBN)!=0) {
		m_bRBN = m_bLBN = false;
		return 1;
	}
	m_bRBN = (m_nRLBN&1)!=0;
	m_bLBN = (m_nRLBN&2)!=0;

	return 0;
}

LRESULT CFixMouse::GetXY(int &x,int &y)const{
	x = m_nX;
	y = m_nY;
	return 0;
}

LRESULT CFixMouse::SetXY(int x,int y){
					//	マウスを指定のポジションに移動（クライアント座標系にて）
	m_nX = x; m_nY = y;	//	いますぐ情報更新＾＾；
	return GetMouse()->SetXY(x,y);
}

bool	CFixMouse::RButton()const{
	return m_bRBN;
}

bool	CFixMouse::LButton()const{
	return m_bLBN;
}

LRESULT CFixMouse::GetInfo(int &x,int &y,int &b)const{
	x = m_nX;
	y = m_nY;
	b = m_nRLBN;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//	前回のFlushから押されたか？

void	CFixMouse::GetButton(bool&bL,bool&bR){
	bR = !m_bHistRB && m_bRBN;
	bL = !m_bHistLB && m_bLBN;
}
bool	CFixMouse::IsPushRButton() const{
	return !m_bHistRB && m_bRBN && (m_nGuardTime==0);
}
bool	CFixMouse::IsPushLButton() const{
	return !m_bHistLB && m_bLBN && (m_nGuardTime==0);
}

/////////////////////////////////////////////////////////////////////////////
//	前回のFlushから押し上げられたか？

void	CFixMouse::GetUpButton(bool&bL,bool&bR){
	bR = m_bHistRB && !m_bRBN;
	bL = m_bHistLB && !m_bLBN;

}
bool	CFixMouse::IsPushUpRButton()const{
	return m_bHistRB && !m_bRBN && (m_nGuardTime==0);
}
bool	CFixMouse::IsPushUpLButton()const{
	return m_bHistLB && !m_bLBN && (m_nGuardTime==0);
}

/////////////////////////////////////////////////////////////////////////////

void	CFixMouse::ResetButton(){
	m_bHistRB = m_bRBN = false;
	m_bHistLB = m_bLBN = false;
}

void	CFixMouse::SetGuardTime(int nTime){
	m_nGuardTime = nTime;
}

bool	CFixMouse::IsGuardTime() const {
	return m_nGuardTime!=0;
}

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd
