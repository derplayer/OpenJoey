
#include "stdafx.h"
#include "CMonologueView.h"
#include "../../../yaneSDK/yaneLineParser.h"
#include "../../../yaneSDK/yaneSinTable.h"

static CSinTable gSinTable;

CMonologueView::CMonologueView(CApp* lpApp){
	m_lpApp = lpApp;
//	GetApp()->GetMouse()->ResetButton();
	m_bFadeIn	=false;			//	フェードイン中？
	m_bFadeOut	=false;			//	フェードアウト中？
	GetApp()->GetDraw()->SetFillColor(RGB(0,0,0));
	m_bBlack	= true;
	for(int i=0;i<64;i++){
		m_FadeBlt.GetFadeTable()[i]=i*4;
		m_FadeBlt.GetFadeTable()[480-i-1]=i*4;
		m_FadeBlt.GetRasterTable()[64-i] = gSinTable.Sin(i << 4)*(i-64)/8 >> 16;
	}
	m_nTextPos = 480;
	m_nTextNext = 0;
	m_nTextLast = 0;
	m_nReadMessageCount = 0;
	m_nPosX = m_nPosY = 0;
	m_nCutCount = 0;
	m_nEnd = 0;
}

CMonologueView::~CMonologueView(){
}

LRESULT	CMonologueView::OnDraw(void){
	GetApp()->GetKey()->Input();
//	bool bLB,bRB;
//	GetApp()->GetMouse()->GetButton(bLB,bRB);
	if (GetApp()->GetKey()->IsVKeyPress(5) /* || bLB || bRB */){
		if (m_nEnd==0) {
			m_nEnd = 1;
		}
	}
	if (m_nEnd) {
		GetApp()->GetDraw()->SetBrightness(256-m_nEnd*256/10);
		m_nEnd++;
		if (m_nEnd == 10) {
			m_bEof = true;	//	もうええやん＾＾
			GetApp()->GetDraw()->SetBrightness(256);
			GetApp()->GetBGMLoader()->Stop();
		}
	}

	if (m_bFadeIn){
		//	フェード処理
		DWORD dw = m_Timer.Get();
		dw = dw * 256 / 3000;	//	3秒
		if (dw>256) {
			m_bFadeIn = false;
			dw = 256;
			GetNextMessage();
			return 0;
		}
		OnDrawBG(dw);
	} else if (m_bFadeOut) {
		//	フェードアウト処理
		DWORD dw = m_Timer.Get();
		dw = dw * 256 / 3000;
		if (dw>256) {
			m_bFadeOut = false;
			m_BGPlane[0].Release();
			m_BGPlane[1].Release();
			m_BGPlane[2].Release();
			dw = 256;
			m_bBlack = true;
			GetNextMessage();
			return 0;
		}
		OnDrawBG(256-dw);
	} else {
		OnDrawBG();
	}

	m_nReadMessageCount ++;
	if (m_nReadMessageCount == 7){
		m_nReadMessageCount = 0;
		GetNextMessage();
	}

	return IsEof()?1:0;
}

void	CMonologueView::OnDrawBG(int nFadeLevel){
	if (m_bBlack) {
		GetApp()->GetDraw()->Clear();
	} else {
		if (nFadeLevel==256){
			int n = m_nPosY/480;
			int r = m_nPosY%480;
			GetApp()->GetDraw()->BltFast(&m_BGPlane[n],0,-r);
			if (r!=0) {
				GetApp()->GetDraw()->BltFast(&m_BGPlane[n+1],0,480-r);
			}
		} else {
			int n = m_nPosY/480;
			int r = m_nPosY%480;
			GetApp()->GetDraw()->BlendBltFast(&m_BGPlane[n],0,-r
				,nFadeLevel,nFadeLevel,nFadeLevel,0,0,0);
			if (r!=0) {
				GetApp()->GetDraw()->BlendBltFast(&m_BGPlane[n+1],0,480-r
				,nFadeLevel,nFadeLevel,nFadeLevel,0,0,0);
			}
		}
	}
	if (m_nCutCount) {
		int nCut = m_nCutCount;
		if (nCut > 256) nCut = 512-nCut;
		if (m_nCutCount!=256){
			m_nCutCount+=16;
		}
		if (m_nCutCount==512) {
			m_nCutCount = 0;
		}
		CPlaneTransBlt::CutInBlt1(GetApp()->GetDraw()->GetSecondary(),&m_CutPlane,m_nCutX,m_nCutY,nCut,true);
	}
	for (int i=0;i<32;i++){
		m_FadeBlt.FadeBlt(GetApp()->GetDraw()->GetSecondary(),&m_TextPlane[(i+m_nTextLast)%32],32,m_nTextPos+i*28);
	}
	m_nTextPos -= 4;
	m_nPosY ++;
	if (m_nPosY > 480*2) {
		m_nPosY = 480*2;
	}
}


LRESULT CMonologueView::Load(LPSTR lpszFile){
	LRESULT lr;
	lr = m_File.Read(lpszFile);
	if (lr) return lr;
	m_bEof = false;
	GetNextMessage();
	return 0;
}

void	CMonologueView::GetNextMessage(void){
	if (m_bEof) return ;
	while (true) {
		CHAR buf[256];
		if (m_File.ReadLine(buf)) {
			m_bEof = true;
			return ;
		}
		CLineParser line;
		line.SetLine(buf);
		if (line.IsMatch("//")) {
			continue;	//	コメント行
		}
		if (line.IsMatch("#black")){
			m_BGPlane[0].Release();
			m_BGPlane[1].Release();
			m_bBlack = true;
			continue;
		}
		if (line.IsMatch("#bgmFadeOut")){
//			int nTime;
//			line.GetNum(nTime);
			GetApp()->GetBGMLoader()->GetFader()->FadeOut(AUDIO_MIX_MASTER,800 /*nTime*/);
			continue;
		}
		if (line.IsMatch("#bgmReset")){
			GetApp()->GetBGMLoader()->Stop();
			GetApp()->GetBGMLoader()->GetFader()->ResetVolume(AUDIO_MIX_MASTER);
			continue;
		}
		if (line.IsMatch("#bgm")){	//	音楽の再生
			int nNo;
			line.GetNum(nNo);
			GetApp()->GetBGMLoader()->Play(nNo);
			continue;
		}
		if (line.IsMatch("#bg3")){	//	bg変更
			string s;
			line.GetStr(s);
			m_BGPlane[0].Load(s);
			line.GetStr(s);
			m_BGPlane[1].Load(s);
			line.GetStr(s);
			m_BGPlane[2].Load(s);
			m_bBlack = false;
			m_nPosY = 0;
			continue;
		}
		if (line.IsMatch("#bg")){	//	bg変更
			string s;
			line.GetStr(s);
			m_BGPlane[0].Load(s);
			m_BGPlane[1].Release();
			m_bBlack = false;
			continue;
		}
		if (line.IsMatch("#music")){	//	音楽のファイル指定再生
			string s;
			line.GetStr(s);
			m_BGMSound.Load(s);
			m_BGMSound.SetLoopMode(true);
			m_BGMSound.Play();
			continue;
		}
		if (line.IsMatch("#SetFillColor")){	//　画面のクリア色の設定
			int	r,g,b;
			line.GetNum(r);
			line.GetNum(g);
			line.GetNum(b);
			GetApp()->GetDraw()->SetFillColor(RGB(r,g,b));
			continue;
		}
		if (line.IsMatch("#se")){	//	効果音の再生
			int nNo;
			line.GetNum(nNo);
			GetApp()->GetSELoader()->Play(nNo);
			GetApp()->GetSELoader()->OnPlayAndReset();
			continue;
		}
		if (line.IsMatch("#FadeIn")){	//　フェードイン
			m_bFadeIn	=true;			//	フェードイン中？
			m_bFadeOut	=false;			//	フェードアウト中？
			m_Timer.Reset();
			continue;
		}
		if (line.IsMatch("#FadeOut")){	//　フェードアウト
			m_bFadeIn	=false;			//	フェードイン中？
			m_bFadeOut	=true;			//	フェードアウト中？
			m_Timer.Reset();
			continue;
		}
		if (line.IsMatch("#CutIn")){	//　CutIn
			string s;
			line.GetStr(s);
			m_CutPlane.Load(s);
			line.GetNum(m_nCutX);
			line.GetNum(m_nCutY);
			m_nCutCount = 16;
			continue;
		}
		if (line.IsMatch("#CutOut")){	//　CutOut
			m_nCutCount = 256+16;
			continue;
		}

		CLineParser::ConvertCR(buf);
		SetText(buf);
		break;
	}
}

void	CMonologueView::SetText(LPSTR lpsz){
	m_TextPlane[m_nTextNext].GetFont()->SetText(lpsz);
	m_TextPlane[m_nTextNext].GetFont()->SetSize(28);
	m_TextPlane[m_nTextNext].GetFont()->SetHeight(25);
	m_TextPlane[m_nTextNext].GetFont()->SetBackColor(RGB(68,79,16));
	m_TextPlane[m_nTextNext].GetFont()->SetColor(RGB(180,180,180));
	m_TextPlane[m_nTextNext].UpdateText();

	m_nTextNext++;
	if (m_nTextNext == 32) {
		m_nTextNext = 0;
	}
	if (m_nTextPos < -32){
		m_nTextPos += 28;
		m_nTextLast++;
		if (m_nTextLast == 32) {
			m_nTextLast = 0;
		}
	}
}
