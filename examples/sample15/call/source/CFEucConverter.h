//  CFEucConverter.h
//		EUCをそのままワイドバイトに変換するクラス（UNICODEへの変換ではない）
//			programmed by coLun	'03/05/12 - '03/05/19

#ifndef __CFEucConverter_h__
#define __CFEucConverter_h__

#include "IRegex.h"
/*
参考ページ（SJIS編からのコピペ）：（サイトトップじゃありません）
	http://www.asahi-net.or.jp/~hc3j-tkg/unicode/
	http://tohoho.wakusei.ne.jp/wwwkanji.htm

EUCと、ワイドバイトえせEUCとの変換（SJIS編からのコピペ）：
	　UNICODEに準じてASCIIの128文字を00XXとするので、
	　マルチバイト時に文字列末等で相棒のいなかった場合をXX00で表すことにする。
	（本プログラムではマルチバイト文字の２バイト目の判断は行っていないので、相棒がいないのは文字列末のみ）
	　よって\xAA\xBB\xCC\xDDというマルチバイトコードは
	　\xAABB\xCCDDと変換される必要があり、リトルエンディアンＯＳなのにビッグエンディアンとなる。
	（まぁ、ＯＳがどうのは関係ないですね・・・wchar_t*をchar*に無理やり変換とかしない限りは）

	参考までにcharの範囲は-128～127まででsignedなのに、wchar_tは0～65536まででunsigned・・・らしいです・・・
	ナンデダロー　ナンデダロー　ナ冫デダナンデダロー
*/

class CFEucConverter : public IJcodeConverter{
public:
	virtual ~CFEucConverter(){};
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
			size_t i;
			char c;
			for(i=0;i<str.size();){
				if(flag){
					m_posmap[wstr.size()]=(int)i;
					m_wposmap[i]=(int)wstr.size();
				}
				c=str[i];
				if ('\xA1'<=c && c<='\xFE' || c=='\x8E') {
					++i;
					if(i<str.size()){
						if(flag){m_wposmap[i]=(int)wstr.size();}
						//\xAA\xBB\xCC\xDDというマルチバイトコードは
						//\xAABB\xCCDDと変換される。（リトルエンディアンパソコンで、ビッグエンディアン！？）
						wstr.append((size_t)1, (((wchar_t)c)<<8)|(((wchar_t)str[i]) & 0xff));
						++i;
					}
					else{
						//マルチバイト時に文字列末だった場合は\xXX00とする。
						wstr.append((size_t)1, ((wchar_t)c)<<8);
					}
				} else if (c=='\x8F') {
				//EUC特殊追加文字・・・３バイト文字ってやつです。
					wstr.append((size_t)1, ((wchar_t)c) & 0xff);
					++i;
					if(i<str.size()){
						if(flag){
							m_posmap[wstr.size()]=(int)i;
							m_wposmap[i]=(int)wstr.size();
						}
						wstr.append((size_t)1, ((wchar_t)str[i]) & 0xff);
						++i;
						if(i<str.size()){
							if(flag){
								m_posmap[wstr.size()]=(int)i;
								m_wposmap[i]=(int)wstr.size();
							}
							m_wstr.append((size_t)1, ((wchar_t)str[i]) & 0xff);
							++i;
						}
					}
				} else {
					//ASCIIの7ビット分のコードは\x00XXとする
					wstr.append((size_t)1, ((wchar_t)c) & 0xff);
					++i;
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
			m_wposmap.resize(wstr.size()*2+1);
		}
		{
			size_t i;
			wchar_t wc;
			char c;
			for(i=0;i<wstr.size();++i){
				if(flag){
					m_posmap[i]=(int)str.size();
					m_wposmap[str.size()]=(int)i;
				}
				wc=wstr[i];
				if(0xff00&wc){
					str.append((size_t)1, (char)(wc>>8));
					c=(char)wc;
					if(c){
						if(flag){m_wposmap[str.size()]=(int)i;}
						str.append((size_t)1, c);
					}
				}
				else{
					str.append((size_t)1, (char)wc);
				}
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
