#include "stdafx.h"
#include "yaneStringScanner.h"

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

bool	CStringScanner::IsToken(LPCSTR &lp,LPCSTR lp2){
	if (lp2==NULL) return false;

	LPCSTR lpx = lp;
	//	ここで、スペース、タブを飛ばして良いと思われる
	SkipSpace(lpx);
	while (true){
		if (*lp2=='\0') {
			//	すべて一致した！
			lp = lpx;	// ポインタを進める！
			return true;
		}
		if (toupper(*(lpx++))!=toupper(*(lp2++))) return false;	//	ダメじゃん...
	}
}

LRESULT	CStringScanner::SkipTo(LPCSTR &lp,LPCSTR lp2){
	while (true){
		if (*lp=='\0') { //	EOFに出くわした！
			return 1;
		}
		//	if (IsToken(lp,lp2)) break;	//	これ、インライン展開したほうがいいなぁ...
		//	遅そうなので↑を手動でインライン展開する＾＾；
		{
			LPCSTR lpx = lp;
			LPCSTR lpx2 = lp2;
			while (true){
			if (*lpx2=='\0') {
				lp = lpx;	// ポインタを進める！
				return 0;	//	一致したんで終了
			}
				if (*(lpx++)!=*(lpx2++)) break;	//	ダメじゃん...
			}
		}
		lp++;
	}
	return 0;
}

//	↑の、一致時に一致するまでにスキップした文字列を文字バッファにコピーする版
LRESULT	CStringScanner::SkipTo2(LPCSTR &lp,LPCSTR lp2,LPSTR lp3){
	LPCSTR lp_org = lp;
	while (true){
		if (*lp=='\0') { //	EOFに出くわした！
			return 1;
		}
		//	if (IsToken(lp,lp2)) break;	//	これ、インライン展開したほうがいいなぁ...
		//	遅そうなので↑を手動でインライン展開する＾＾；
		{
			LPCSTR lpx = lp;
			LPCSTR lpx2 = lp2;
			while (true){
			if (*lpx2=='\0') {
				//	文字バッファにコピーしておく。
				int nSize = lp - lp_org;
				::CopyMemory(lp3,lp_org,nSize);
				*(lp3+nSize) = '\0';

				lp = lpx;	// ポインタを進める！
				return 0;	//	一致したんで終了
			}
				if (*(lpx++)!=*(lpx2++)) break;	//	ダメじゃん...
			}
		}
		lp++;
	}
	return 0;
}

LRESULT CStringScanner::SkipSpace(LPCSTR &lp){
	//	スペース、タブ、改行を切り詰める
	while (*lp==0x0a || *lp==0x0d || *lp==' ' || *lp=='\t'){
		lp++;
	}
	if (*lp=='\0') return 1;
	return 0;
}

LRESULT CStringScanner::SkipSpace2(LPCSTR &lp){
	//	スペース、タブ、カンマ、改行を切り詰める
	//	csv読み込みに使うと便利
	while (*lp==0x0a || *lp==0x0d || *lp==' ' || *lp=='\t' || *lp==','){
		lp++;
	}
	if (*lp=='\0') return 1;
	return 0;
}

LRESULT	CStringScanner::GetStrNum(LPCSTR &lp,int& nRate){
//	"+1"という文字列ならばnRateには1が返る
	if (SkipSpace(lp)!=0) return -1;	//	ダメやん

	if (*lp!='"') return 3;	//	おかしーやん！
	lp++;
	nRate = 0;
	int nSign = 1;
	if (*lp=='+') { lp++; }
	ef (*lp=='-') { nSign = -1; lp++; }
	LRESULT lr = 1;
	while (*lp>='0' && *lp<='9') {
		lr = 0;
		nRate = nRate*10+(*lp-'0');
		lp++;
	}
	nRate *= nSign;
	if (*lp!='"') return 3; // 閉じてないやん
	lp++;
	return lr;
}

LRESULT	CStringScanner::GetNum(LPCSTR &lp,int& nVal){
//	+1という文字列ならばnValには1が返る

	if (SkipSpace(lp)!=0) return -1;	//	ダメやん

	nVal = 0;
	int nSign = 1;
	if (*lp=='+') { lp++; }
	ef (*lp=='-') { nSign = -1; lp++; }
	LRESULT lr = 1;
	while (*lp>='0' && *lp<='9') {
		lr = 0;
		nVal = nVal*10+(*lp-'0');
		lp++;
	}
	nVal *= nSign;
	return lr;
}

LRESULT CStringScanner::GetStrColor(LPCSTR &lp,COLORREF& nFontColor){
// "#ff0000"という文字列ならばRGB(255,0,0)が返る
	if (SkipSpace(lp)!=0) return -1;

	if (*lp!='"') return 3;	//	おかしーやん！
	lp++;
	if (*lp!='#') return 3; // #しかサポートしてないのさっ＾＾；
	lp++;
	LRESULT lr = 1;
	DWORD dw = 0;
	while ((*lp>='0' && *lp<='9')||(*lp>='a' && *lp<='f')||(*lp>='A' && *lp<='F')){
		lr = 0;
		int c;
		if (*lp>='0' && *lp<='9') {
			c = *lp - '0';
		} else {
			c = toupper(*lp) - 'A'+10;
		}
		dw = (dw << 4) + c;
		lp++;
	}
	if (*lp!='"') return 3; // 閉じてないやん
	lp++;
	//	RGBは転置＝天地する
	nFontColor = ((dw & 0xff)<<16) + (dw & 0xff00) + ((dw & 0xff0000)>>16);
	return lr;
}

string CStringScanner::GetStr(LPCSTR &lp,CHAR c){
	//	cに遭遇するところまでの文字列を返す
	string str;
	while ( *lp!=c && *lp != NULL ) {
		str += *(lp++);
	}
	return str;
}

string CStringScanner::GetStr(LPCSTR &lp){
	//	スペース、タブ、改行に遭遇するところまでの文字列を返す
	//	全角未対応＾＾；
	string str;
	while (true) {
		CHAR c = *lp;
		if (c==' ' || c=='\t' || c=='\0' || c=='\n') break;
		str += c;
		lp++;
	}
	return str;
}

string CStringScanner::GetStrFileName(LPCSTR &lp){
	//	タブ、改行に遭遇するところまでの文字列を返す
	//	全角未対応＾＾；
	string str;
	while (true) {
		CHAR c = *lp;
		if (c=='\t' || c=='\0' || c=='\n') break;
		str += c;
		lp++;
	}
	return str;
}

string CStringScanner::GetNextStr(LPCSTR &lp){
	//	スペース、タブ、改行に遭遇するところまでの文字列を返す
	//	全角未対応＾＾；
	string str;
	if (SkipSpace(lp)!=0) return str;	//	ダメね～＾＾；

	bool bDC = (*lp == '"');	//	ダブルコーテで始まってるか？
	if (bDC) lp++;

	while (true) {
		CHAR c = *lp;
		if (bDC) {	//	ダブルコーテスタートならばスペース，タブは無視
			if (c=='"' ) { lp++;break;};
			if (c=='\0') break;
		} else {
			if (c==' ' || c=='\t' || c=='\0' || c=='\n') break;
		}
		str += c;
		lp++;
	}
	return str;
}

LRESULT CStringScanner::GetNumFromCsv(LPCSTR &lp,int& nVal,bool b){
	//	カンマ、スペースを無視しながら数値を読み込むのだ
	if ( !b ){
		if (SkipSpace2(lp)!=0) return 1;
		if (GetNum(lp,nVal)!=0) return 2;
	}else{
		if ( *lp==',' || *lp== '\t' || *lp=='/' ) {
			lp++;
			nVal = 0;
		}else{
			LRESULT lr = GetNum(lp,nVal);
			if ( *lp==',' || *lp== '\t' || *lp=='/' ) {
				lp++;
			}
			if ( lr != 0 ) return 2;
		}
	}
	
	return 0;
}


string CStringScanner::GetStrFromCsv(LPCSTR &lp){
	string str;
	while ( *lp!=',' && *lp!='/' && *lp != NULL ) {
		str += *(lp++);
	}
	return str;
}

void	CStringScanner::Replace(string& buf,LPCSTR pSrc,LPCSTR pDst,bool bCaseSensitive){
	//	string中に含まれるpSrcと一致する部分をpDstに置換する
	//	日本語対応。bCaseSensitive==trueのときは、
	//	アルファベットの大文字小文字を区別する。

	//	置換文字列が空？
	if (buf.empty()) return ;

	//	置換ソースが空白？
	if (*pSrc == '\0') return ; // これはあかんやろ．．
	
	LPCSTR pSz = buf.c_str();
	LPCSTR pSzOrg = pSz; // オリジナルポジション
	while (*pSz!='\0'){
		//	大文字と小文字を区別するとき
		if ((bCaseSensitive && *pSz == *pSrc) ||
			(!bCaseSensitive && toupper(*pSz)==toupper(*pSrc))){
			//	先頭が一致したんで、そこから先も一致するか調べるにゅ
			LPCSTR p1 = pSz+1;
			LPCSTR p2 = pSrc+1;
			while(true) {
				if (*p2=='\0'){
				//	一致しちゃった＾＾；
					goto Found;
				}
				//	大文字と小文字を区別するとき
				if (bCaseSensitive){
					if (*p1!=*p2) break;
					p1++; p2++;
				} else {
				//	大文字と小文字を区別しないとき
					int c = *p1;
					if (toupper(c)!=toupper(*p2)) break;
					if (IsLead_ShiftJIS(c)){
						p1++; p2++;
						if (*p1!=*p2) break;
						//	漢字コードの２バイト目にはCaseInsensitivityを適用しない
						p1++; p2++;
					} else {
						p1++; p2++;
					}
				}
			}
		}
		//	マルチバイト対策
		if (IsLead_ShiftJIS(*pSz)){
			pSz+=2;
		} else {
			pSz++;
		}
	}
	return ;

Found:;
	vector <int> an;
	an.push_back(pSz-pSzOrg); //	現在位置(0オリジン)
	int nL1 = ::strlen(pSrc);
	int nL2 = ::strlen(pDst);
	pSz+= nL1; // 一致した文字列分だけ加算

	//	さらに検索を続ける
	while (*pSz!=0){
		//	大文字と小文字を区別するとき
		if ((bCaseSensitive && *pSz == *pSrc) ||
			(!bCaseSensitive && toupper(*pSz)==toupper(*pSrc))){
			//	先頭が一致したんで、そこから先も一致するか調べるにゅ
			LPCSTR p1 = pSz+1;
			LPCSTR p2 = pSrc+1;
			while(true) {
				if (*p2=='\0'){
				//	一致しちゃった＾＾；
					an.push_back(pSz-pSzOrg); //	現在位置(0オリジン)
					pSz+= nL1; // 一致した文字列分だけ加算
					goto SearchNext;
				}
				//	大文字と小文字を区別するとき
				if (bCaseSensitive){
					if (*p1!=*p2) break;
					p1++; p2++;
				} else {
				//	大文字と小文字を区別しないとき
					int c = *p1;
					if (toupper(c)!=toupper(*p2)) break;
					if (IsLead_ShiftJIS(c)){
						p1++; p2++;
						if (*p1!=*p2) break;
						//	漢字コードの２バイト目にはCaseInsensitiveを適用しない
						p1++; p2++;
					} else {
						p1++; p2++;
					}
				}
			}
		}
		//	マルチバイト対策
		if (IsLead_ShiftJIS(*pSz)){
			pSz+=2;
		} else {
			pSz++;
		}
SearchNext:;
	}

	//	実際の縮小拡大作業
	int n = pSz-pSzOrg; // 元の文字列長
	int m = an.size();
	int nL = n + m*(nL2-nL1) + 1; // 置換する数×１つあたりの変位 + 1(終端のNULL)
	LPSTR pTmp = new CHAR[nL];
	//	nL1==nL2のときは、このバッファ作成作業ははしょれるのだが、
	//	string::c_strは、constなバッファなため、変更は許されていない
	LPSTR pTmpt = pTmp;
	pSz = pSzOrg;
	int k=0;
	for(int i=0;i<n;i++){
		if ((k<m) && (i==an[k])){ // if の評価順は左からと仮定している
			LPCSTR pDstt = pDst;
			while (*pDstt!='\0'){
				*(pTmpt++) = *(pDstt++);
			}
			i += nL1-1;
			pSz += nL1; // ソース分加算
			k++;
		} else {
			*(pTmpt++) = *(pSz++);
		}
	}
	*pTmpt = '\0'; // 終端文字
	buf = pTmp; // 文字列の代入作業
	delete [] pTmp;
}

void	CStringScanner::NumToString(LONGLONG l,string& s){
	if (l==0) {
		s = "0";
		return ;
	}
	bool bMinus;
	if (l<0) {
		bMinus = true;
		l = -l;
	} else {
		bMinus = false;
	}
	s.erase();
	while (l!=0){
		s = (CHAR)((l%10) + '0') + s;
		//	もう少し洗練されたコードも書けるのだが..
		l/=10;
	}
	if (bMinus) {
		s = '-' + s;
	}
}

LRESULT	 CStringScanner::StringToNum(string s,LONGLONG& l){
	LPCSTR psz = s.c_str();
	bool bMinus;
	if (*psz == '-') {
		psz++;
		bMinus = true;
	} else {
		bMinus = false;
	}
	l=0;
	bool bFirst = true;
	while (true){
		CHAR c = *psz;
		if (c>='0' && c<='9'){
			l = l*10 + c-'0';
			bFirst = false;
		} else {
			if (bFirst){
			//	数値代入を１度も行なっていない
				return 1; // error
			}
			break;
		}
		psz++;
	}
	if (bMinus) l=-l;
	return 0;
}

void CStringScanner::NumToStringZ(LONGLONG l,string& s,int n){
	NumToString(l,s);
	int nLength = s.length();
	if (nLength < n){
		//	長さが足りないので、左の桁にゼロを追加。
		string t;
		for(int i=nLength;i<n;i++){ t+='0'; }
		s = t+s;
	} ef (nLength > n){
		//	長すぎるので、左の桁をカットする（そんな必要無いんか？）
		s = s.substr(nLength-n,n);
	}
}

int	CStringScanner::CharToHex(CHAR n){
	if ('A'<=n && n<='F'){ n = n - 'A' + 10; }
	else if ('a'<=n && n<='f') { n = n - 'a' + 10; }
	else if ('0'<=n && n<='9') { n = n - '0'; }
	else {
#ifdef USE_EXCEPTION
		throw CRuntimeException();
#endif
	}
	return n;
}

CHAR CStringScanner::HexToChar(int n){
	if (n<0 || 15<n) {
#ifdef USE_EXCEPTION
		throw CRuntimeException();
#endif
	}
	if (n>=10){ n+='A'-10;} else { n+='0'; }
	return n;
}

LRESULT CStringScanner::Find(LPCSTR &lpSrc,LPCSTR lpMoji){
	if ( lpSrc == NULL || lpMoji==NULL || *lpMoji=='\0')
			return -1; // こんなん渡すなヽ(`Д´)ノ
	while (*lpSrc != '\0') {
		if (*lpSrc == *lpMoji){
			int i=1;
			while (true){
				CHAR c = lpMoji[i];
				if (c=='\0') return 0; // matched
				if (c!=lpSrc[i]) break;	//	to next
				++i;
			}
		}
		++lpSrc;
	}
	return 1; //	not matched
}

bool CStringScanner::IsLead_ShiftJIS(CHAR c) {
	//	Optimize Technique No.004
	//	http://homepage1.nifty.com/herumi/adv/adv40.html
	return ((unsigned char)((c^0x20)-0xA1)<=0x3B)!=0;
}

bool CStringScanner::IsTrail_ShiftJIS(CHAR c){
	return ((0x40<=c&&c<=0x7e) || (0x80<=c&&c<=0xfc))!=0;
}

/////////////////////////////////////////////////////////////////////////////
CLineParser::CLineParser() {
	m_lpszReadData = NULL;										// 1ラインを読み込んだデータ
}

CLineParser::~CLineParser() {
}

void	CLineParser::SetLine( LPCSTR lpszStr ){
	m_lpszReadData = (LPSTR)lpszStr;
}

bool	CLineParser::IsMatch( LPCSTR lpszCmpStr ) {

	if ( m_lpszReadData == NULL ) return false;

	Garbage();				//	ゴミを削除する

	LPSTR lpOrg = m_lpszReadData;
	while ( true ) {
		if ( *lpszCmpStr == '\0' ) return true;
		if ( ::toupper( *(lpszCmpStr++) ) != ::toupper( *(m_lpszReadData++) ) ) {	//	指定した文字列と等しいか否かの判定
			m_lpszReadData = lpOrg;
			return false;
		}
	}
}


LRESULT		CLineParser::GetNum( int& nNo )
{
	if ( m_lpszReadData == NULL ) return 1;

	int		c;
	int		nRetNum = 0;
	bool	bSigned = false;

	Garbage();

	// 『0～9』か『-』を探す
	// 最初が『-』か『0～9』以外の時はGetNum失敗
	c = *m_lpszReadData;
	if ( (c < '0' || c > '9') && (c != '-') ) return 1;

	if (c=='-') {
		bSigned = true;
		m_lpszReadData++;
	}

	while ( true ) {
		c = *m_lpszReadData;
		if ( (c >= '0') && (c <= '9') ) {
			nRetNum = nRetNum*10 + (c -'0');	//	文字列を数値にして、桁を上げる
		} else {
			break;
		}
		m_lpszReadData++;
	}

	if ( bSigned )												//	符号がマイナスか否かの判定
		nRetNum = -nRetNum;

	nNo = nRetNum;												//	結果を渡す
	return 0;
} // GetNum


LRESULT		CLineParser::GetStr( string& str )
{
	if ( m_lpszReadData == NULL ) return 1;

	Garbage();
	
	// 『"』を探す
	// 最初が『"』以外の時はGetStr失敗
	if ( *m_lpszReadData!= '\"' ) return 2;

	m_lpszReadData++;

	// "から"までの文字列を渡す
	// 終了の『"』を探す
	CHAR	szStr[256];
	int		k=0;
	while (true) {
		int c = *(m_lpszReadData++);
		if (c=='\0') return 3;	//	unexpectedly terminate
		
		if (c=='\"') {
			szStr[k] = '\0';
			break;
		} else {
			szStr[k] = c;
		}
		k++;
	}

	str = (LPSTR) szStr;
	return 0;
} // GetStr


/////////////////////////////////////////

void	CLineParser::Garbage() {
	// ゴミ以外が見つかるまでループする
	// ゴミ指定文字 『 (Space)』『	(Tab)』『,』
	while ( true )
	{
		int c = *m_lpszReadData;
		if ( c != ' ' && c != '\t' && c != ',' ) break;
		m_lpszReadData++;
	}
}

void	CLineParser::ConvertCR(LPSTR lpsz){
	LPSTR lp = lpsz;
	for(;;) {
		*lpsz = *lp;
		if (*lpsz == '\0') break;
		if((*lp == '\\') && (*(lp+1)=='n')) {
			*lpsz = '\n';
			lpsz++;
			lp+=2;
			continue;
		}
		lpsz++;
		lp++;
	}
}


} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd
