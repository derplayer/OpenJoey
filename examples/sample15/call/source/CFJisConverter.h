//  CFJisConverter.h
//		JISをEUC化してワイドバイトに変換するクラス（UNICODEへの変換ではない）
//			programmed by coLun	'03/05/12 - '03/05/22

#ifndef __CFJisConverter_h__
#define __CFJisConverter_h__

#include "IRegex.h"
/*
参考ページ（SJIS編からのコピペ）：（サイトトップじゃありません）
	http://www.asahi-net.or.jp/~hc3j-tkg/unicode/
	http://tohoho.wakusei.ne.jp/wwwkanji.htm

JISと、ワイドバイトえせJISとの変換（SJIS編からのコピペ）：
	　UNICODEに準じてASCIIの128文字を00XXとするので、
	　マルチバイト時に文字列末等で相棒のいなかった場合をXX00で表すことにする。
	（本プログラムではマルチバイト文字の２バイト目の判断は行っていないので、相棒がいないのは文字列末のみ）
	　よって\xAA\xBB\xCC\xDDというマルチバイトコードは
	　\xAABB\xCCDDと変換される必要があり、リトルエンディアンＯＳなのにビッグエンディアンとなる。
	（まぁ、ＯＳがどうのは関係ないですね・・・wchar_t*をchar*に無理やり変換とかしない限りは）

	参考までにcharの範囲は-128～127まででsignedなのに、wchar_tは0～65536まででunsigned・・・らしいです・・・
	ナンデダロー　ナンデダロー　ナ冫デダナンデダロー

SJISやEUCに見られないJIS特有のエスケープシーケンス：（参考ページのとほほWWW入門からの転用）
記号表記			16進表記			意味								コルンのつけたし
ESC ( B				1B 28 42			ASCII								ここから1バイトコード
ESC ( J				1B 28 4A			JIS X 0201-1976 Roman Set			 - jcode.plで上と同じ扱い
ESC ( I				1B 28 49			JIS X 0201-1976 片仮名				 - 1バイトコード。・・・これがEUCに変換されると2バイトに・・・！！　でもJISには無関係。
ESC $ @				1B 24 40			JIS X 0208-1978(通称：旧JIS)		ここから2バイトコード
ESC $ B				1B 24 42			JIS X 0208-1983(通称：新JIS)		 - jcode.plで上と同じ扱い。実質区別する必要なし
ESC & @ ESC $ B		1B 26 40 1B 24 42	JIS X 0208-1990						 - 余計なものがついてるけど根本は上に同じ。
ESC $ ( D			1B 24 28 44			JIS X 0212-1990						 - 2バイトコード・・・これがEUCに変換されると3バイトに・・・！！　でもJISには無関係。

	　JISでは文字コード領域が、それぞれのエスケープシーケンスでかぶるので、
	　いっそのことえせEUCに変換するのが一番手っ取り早そうです。
	　というわけで、このプログラムは　「JIS←→えせJIS」ではなく、「JIS←→えせEUC」になりまふ。
*/

class CFJisConverter : public IJcodeConverter{
public:
	virtual ~CFJisConverter(){};
	virtual basic_string<wchar_t> ConvertToWString(const basic_string<char> & str, bool flag=false){
	//StringをWStringに変換します。フラグがtrueのとき、この文字列をクラス内に記憶し、後にGetPosやGetWPosなどが使えます

		if(flag){m_str=str;}

		basic_string<wchar_t> wstr_dmy;
		basic_string<wchar_t> & wstr=flag ? m_wstr : wstr_dmy;
		//↑これでだめなときは↓こんなのどう？
		//basic_string<wchar_t> & wstr=*(flag ? (&m_wstr) : (&wstr_dmy));

		wstr.erase();
		wstr.reserve(str.size());
		if(flag){
			m_posmap.resize(str.size()+1);//終端の場所データまで含まなアカンので+1してます
			m_wposmap.resize(str.size()+1);
		}
		{
			size_t i, e=str.size();
			char c;
			int state=0;
			for(i=0;i<e;){
				if(flag){m_wposmap[i]=(int)wstr.size();}
				c=str[i];
				if (c=='\x1B') {
					++i;
					if(i<e){
						if(flag){m_wposmap[i]=(int)wstr.size();}
						c=str[i];
						if(c=='$'){
							++i;
							if(i<e){
								if(flag){m_wposmap[i]=(int)wstr.size();}
								c=str[i];
								if(c=='@' || c=='B'){
									++i;
									state=1;
								} else if(c=='('){
									++i;
									if(i<e){
										if(flag){m_wposmap[i]=(int)wstr.size();}
										c=str[i];
										if(c=='D'){
                                            ++i;
											state=3;
										}
									}
								}
							}
						} else if (c=='(') {
							++i;
							if(i<e){
								if(flag){m_wposmap[i]=(int)wstr.size();}
								c=str[i];
								if(c=='B' || c=='J'){
									++i;
									state=0;
								} else if(c=='I'){
									++i;
									state=2;
								}
							}
						} else if (c=='&') {
							++i;
							if(i<e){
								if(flag){m_wposmap[i]=(int)wstr.size();}
								c=str[i];
								if(c=='@'){
									++i;
								}
							}
						}
					}
				} else {
					if(flag){m_posmap[wstr.size()]=(int)i;}
					if(state==0) {
						//ASCIIの7ビット分のコードは\x00XXとする
						wstr.append((size_t)1, ((wchar_t)c) & 0xff);
						++i;
					} else if(state==1) {//全角文字
						++i;
						if(i<e && str[i]!='\x1B'){
							if(flag){m_wposmap[i]=(int)wstr.size();}
							wstr.append((size_t)1, (((wchar_t)c) & 0xff) | (((wchar_t)str[i])<<8) | 0x8080);
							++i;
						}
					} else if(state==2) {//半角カナ
						++i;
						wstr.append((size_t)1, (((wchar_t)c) & 0xff) | 0x8E80);
					} else if(state==3) {//難しい漢字
						++i;
						if(i<e && str[i]!='\x1B'){
							if(flag){m_wposmap[i]=(int)wstr.size();}
							wstr.append((size_t)1, 0x8F);
							if(flag){m_posmap[wstr.size()]=(int)i-1;}
							wstr.append((size_t)1, (((wchar_t)c) & 0xff) | 0x80);
							if(flag){m_posmap[wstr.size()]=(int)i-1;}
							wstr.append((size_t)1, (((wchar_t)str[i]) & 0xff) | 0x80);
							++i;
						}
					}
				}
			}
			if(flag){
				m_posmap[wstr.size()]=(int)i;
				m_wposmap[i]=(int)wstr.size();
			}
		}
		if(flag){m_posmap.resize(wstr.size()+1);}//終端の場所データまで含まなアカンので+1してます
		return wstr;
	}
	virtual basic_string<char> ConvertToString(const basic_string<wchar_t> & wstr, bool flag){
	//コンバータ内にWStringを入れます。以降、StringとWStringと取り出し自由で、
	//それぞれの文字pos間での変換が簡単に行えます。
		if(flag){m_wstr=wstr;}

		basic_string<char> str_dmy;
		basic_string<char> & str=flag ? m_str : str_dmy;
		//↑これでだめなときは↓こんなのどう？
		//basic_string<char> & str=*(flag ? (&m_str) : (&str_dmy));

		str.erase();
		str.reserve(wstr.size()*2);
		if(flag){
			m_posmap.resize(wstr.size()+1);//終端の場所データまで含まなアカンので+1してます
			m_wposmap.resize(wstr.size()*5+3+1);//最悪の事態を想定すると、*5+3+1になるのでふ・・・
		}
		{
			size_t i;
			wchar_t wc;
			char c;
			int state=0;
			for(i=0;i<wstr.size();++i){
				if(flag){
					m_posmap[i]=(int)str.size();
					m_wposmap[str.size()]=(int)i;
				}
				wc=wstr[i];
				if(0xff00&wc){
					if((wc & 0xff00)!=0x8E00){//全角
						if(state!=1){//全角モードにする
							str.append("\x1B$B");
							if(flag){
								m_wposmap[str.size()-2]=(int)i;
								m_wposmap[str.size()-1]=(int)i;
								m_wposmap[str.size()]=(int)i;
							}
							state=1;
						}
						str.append((size_t)1, (char)((wc>>8)&0x7f));
						c=(char)wc;
						if(c){
							if(flag){m_wposmap[str.size()]=(int)i;}
							str.append((size_t)1, c&0x7f);
						}
					}else{//半角カナ
						if(state!=2){//半角カナモードにする
							str.append("\x1B(I");
							if(flag){
								m_wposmap[str.size()-2]=(int)i;
								m_wposmap[str.size()-1]=(int)i;
								m_wposmap[str.size()]=(int)i;
							}
							state=2;
						}
						str.append((size_t)1, wc & 0x007f);
					}
				} else if(wc==0x8F) {//難しい漢字
					++i;
					if(i<wstr.size()){
						if(flag){m_posmap[i]=(int)str.size();}
						wc=wstr[i];
						++i;
						if(i<wstr.size()){
							if(flag){m_posmap[i]=(int)str.size();}
							if(state!=3){//難しい漢字モードにする
								str.append("\x1B$(D");
								if(flag){
									m_wposmap[str.size()-3]=(int)i-2;
									m_wposmap[str.size()-2]=(int)i-2;
									m_wposmap[str.size()-1]=(int)i-2;
									m_wposmap[str.size()]=(int)i-2;
									m_posmap[i-2]=(int)str.size();//エスケープコードの後にposが来る必要があるので・・・
									m_posmap[i-1]=(int)str.size();
									m_posmap[i]=(int)str.size();
								}
								state=3;
							}
							str.append((size_t)1, (char)wc);
							if(flag){m_wposmap[str.size()]=(int)i-2;}
							str.append((size_t)1, (char)wstr[i]);
						}
					}
				} else {//ASCII
					if(state!=0){//ASCIIモードにする
						str.append("\x1B(B");
						if(flag){
							m_wposmap[str.size()-2]=(int)i;
							m_wposmap[str.size()-1]=(int)i;
							m_wposmap[str.size()]=(int)i;
						}
						state=0;
					}
					str.append((size_t)1, (char)wc);
				}
			}
			if(state!=0){//ASCIIモードにする
				str.append("\x1B(B");
				if(flag){
					m_wposmap[str.size()-2]=(int)i;
					m_wposmap[str.size()-1]=(int)i;
				}
				state=0;
			}
			if(flag){
				m_posmap[i]=(int)str.size();
				m_wposmap[str.size()]=(int)i;
			}
		}
		if(flag){m_wposmap.resize(str.size()+1);}//終端の場所データまで含まなアカンので+1してます
		return str;
	}
	virtual basic_string<char> GetString(){
		return m_str;
	}
	virtual basic_string<wchar_t> GetWString(){
		return m_wstr;
	}
	virtual size_t GetPos(size_t wpos){
	//「文字列始」より前と「文字列末」より後は使わないように願います。
	//動作は未定義です。
	//（本実装では暴走します）
		return m_posmap[wpos];
	}
	virtual size_t GetWPos(size_t pos){
	//「文字列始」より前と「文字列末」より後は使わないように願います。
	//動作は未定義です。
	//（本実装では暴走します）
		return m_wposmap[pos];
	}
private:
	basic_string<char> m_str;
	basic_string<wchar_t> m_wstr;
	vector<size_t> m_posmap;
	vector<size_t> m_wposmap;
};

#endif
