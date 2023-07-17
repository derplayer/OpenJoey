
#include "stdafx.h"
#include "../../yaneSDK/yaneSDK.h"
#include "CScnConvert.h"

///////////////////////////////////////////////////////////
//	コンバータ本体↓
//	いじるときは、これを適当にいじってくらはい。

LRESULT CScnConvert::Convert(string filename,string outputpath){

	//	ソースファイル
	CFile src_file;
	src_file.Read(filename);

	//	書き込むファイル
	CFile dst_file;
	bool bWrite=false; // 書き込みCHU！

	m_nLineNo = 0;	//	解析中の行番号
	bool bRemoveBracket = false;

	bool bOutBR2 = true;

	string title;	//	ページタイトル

	int	anBox[10] = {0};	//	box用の通しナンバー
	string aszBox[10] ={
		"脚注-","list-","枠-","図-"
	};
	int nChap = 0;
	int	nSection = 0;
	int nLineNo = 0;

	CFile listfile;

	while (true){
/////////////////////////////////////////////////////////
//	解析部　～ここから～
/////////////////////////////////////////////////////////
	string linebuf;
	if (src_file.ReadLine(linebuf)!=0) break;
	m_nLineNo++;

	LPSTR psz = (LPSTR)linebuf.c_str();
	string line = linebuf;	//	出力用のラインはコピっとく！
	//	こうしておけば、lineを潰しても、pszのポイント先には影響しない

	//	その行の最後に<BR>を放り込むのか？
	bool bOutBR = true;

	//	☆　もし先頭が // から始まるのであれば、これは特殊効果指定
	if (CStringScanner::IsToken(psz,"//")){

		//	特殊効果指定（タグの省略表記）なので、行末の<HR>は放り込まなくて良い
//		bOutHR = false;
		
		//	－－－－　以下、コピペの嵐＾＾；

		/*
			// log 数字は、出力するシナリオファイルナンバー
			例.
				//	log 100
				とあれば、そこ以降はlog0100.htmlというファイルに出力
		*/
		if (CStringScanner::IsToken(psz,"chap")){
			int nSceneNo;
			if (CStringScanner::GetNumFromCsv(psz,nSceneNo)!=0){
				Err("chap指定ナンバーの指定がおかしい。");
			} else {
				//	前の開いてたら閉じる
				if (dst_file.GetFilePtr()!=NULL){
					dst_file.Write("<HR></BODY></HTML>");
					OutPageTotal(nChap,nLineNo);
					nLineNo = 0;
				}

				//	脚注データ初期化
				ZERO(anBox);
				nSection = 0;
				nChap = nSceneNo;

				string obj_filename;
				obj_filename = outputpath + "/chap" + CStringScanner::NumToStringZ(nSceneNo,4)+".html";
				dst_file.Open(obj_filename.c_str(),"w");
				bWrite = true;

//	―――ヘッダ
dst_file.Write("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">");
dst_file.Write("<HTML><HEAD>");
dst_file.Write("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=Shift_jis\">");
dst_file.Write("<meta http-equiv=\"Content-Language\" content=\"ja\">");
dst_file.Write("<META name=\"GENERATOR\" content=\"auto diary generator by yaneurao\">");
dst_file.Write("<TITLE>"+title+"</TITLE>");
dst_file.Write("</HEAD>");

				dst_file.Write("<P><A name=\"top\"></A><BR>\n<BODY>");
			}
			continue;
		}

		/*
			//	title	ページタイトル
			⇒これは、タイトル文字列として処理する
		*/
		if (CStringScanner::IsToken(psz,"title")){
			title = psz;
			continue;
		}

		/*
			//	sect 小見出し
			⇒　§2.1　のようになる
		*/
		if (CStringScanner::IsToken(psz,"sect")){
			nSection++;
			dst_file.Write("<HR>\n§"+
				CStringScanner::NumToString(nChap)+"."+
				CStringScanner::NumToString(nSection)+"　"+psz+"<BR>"
			);
			continue;
		}

		/*
			//	date 2 1 2
			→　<HR><P><FONT size="+2">2002</FONT> 年 <FONT size="+2">1</FONT> 月 <FONT size="+2">2</FONT>日 <A href="#top"></P>
		*/
		if (CStringScanner::IsToken(psz,"date")){
			int n1,n2,n3;
			if (CStringScanner::GetNumFromCsv(psz,n1)!=0 ||
				CStringScanner::GetNumFromCsv(psz,n2)!=0 ||
				CStringScanner::GetNumFromCsv(psz,n3)!=0){
					Err("date指定ナンバーの指定がおかしい。");
				//	↑エラーは第１パラメータが存在しないとき。
			} else {
				if (n1 <2000) { n1+=2000; }	//	2000年だよーん
				line = "<HR><P><FONT size=\"+2\">"+CStringScanner::NumToString(n1)
						+ "</FONT> 年 <FONT size=\"+2\">"
						+ CStringScanner::NumToString(n2)
						+ "</FONT> 月 <FONT size=\"+2\">"
						+ CStringScanner::NumToString(n3)
						+ "</FONT>日</P>"; // <A href=\"#top\">△</A></P>";
				goto output;
			}
		}
		/*
			//	box ～ eboxはボックス化する
		*/
		if (CStringScanner::IsToken(psz,"box")){
			int n1;
			string line2;
			if (CStringScanner::GetNumFromCsv(psz,n1)==0){
				line2 = aszBox[n1] + CStringScanner::NumToString(anBox[n1]+1) + psz;
				anBox[n1]++;
			}

			if (n1==1){
			//	listなのでファイルに出力
				string obj_filename;
				obj_filename = "list/Chap" + CStringScanner::NumToStringZ(nChap,2) + "_list" + CStringScanner::NumToStringZ(anBox[n1],2)+ ".txt";
				listfile.Open(obj_filename.c_str(),"w");
			}

			string line;
			line = "<P align=\"center\">" + line2;
			line +=
				string("<TABLE border=\"0\" cellpadding=\"2\" cellspacing=\"0\">")
				+"<TBODY><TR><TD bgcolor=\"#9999ff\">"
				+"<TABLE border=\"0\" cellpadding=\"10\" cellspacing=\"0\">"
				+"<TBODY><TR><TD bgcolor=\"#ccccff\"><pre>\n";
			dst_file.Write(line);
			if (n1!=3) {	//	図のときは、これは無視
				bOutBR2 = false;
			}
			continue;
		}
		if (CStringScanner::IsToken(psz,"ebox")){
			string line;
			line = "</pre></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></P>\n";
			dst_file.Write(line);
			bOutBR2 = true;
			listfile.Close();
			continue;
		}

	} //	end of checking "//"

/*
	//	空行は無視
	//	ReadLineなので改行コードでなく'\0'を検出すればｏｋ
	if (*psz == '\0') continue;
*/
	if (CStringScanner::IsToken(psz,"http://")) {
		line = "<A href=\"" + line + "\">"+line + "</A>";
		dst_file.Write(line);
		bOutBR2 = true;
		continue;
	}

	//	それ以外ならばそのまま書き込む

	//	１行書き込み
output:;
	if (bWrite) {
		//	list fileにも出力する
		if (listfile.GetFilePtr()!=NULL){
			listfile.Write(line);
			listfile.Write("\n");
		}

		//	タブコードはpreタグの無いところでは半角スペース４つに置換する
		{
Loop:;
			LPCSTR psz = line.c_str();
			int i=0;
			while (*psz!='\0'){
				if (*psz == '\t'){
					//	４タブの空白でサプレスする
					line = line.substr(0,i) + ("    "+(i&3)) + line.substr(i+1,string::npos);
					goto Loop;
				}
				i++; psz++;
			}
		}
		if (bOutBR && bOutBR2) {
			line += "<BR>\n";
		} else {
			if (!bOutBR2){
				//	preタグの特殊枠なので、< >は置換すべし
				CStringScanner::Replace(line,"&","&amp;");
				CStringScanner::Replace(line,"<","&lt;");
				CStringScanner::Replace(line,">","&gt;");
			}
			line += '\n';
		}
		//	%が、C言語の出力書式に基づいて置換されてしまうといけない
		CStringScanner::Replace(line,"%","%%"); // %を%%に置換

		//	全角アルファベットの半角化
		{
			char ch[] = "A";
			char c;
			for(c = 'A';c<='Z';c++){
				ch[0] = c;
				char buf[] = "Ａ";
				buf[1] += c-'A';
				CStringScanner::Replace(line,buf,ch);

				ch[0] = c-'A'+'a';
				char buf2[] = "ａ";
				buf2[1] += c-'A';
				CStringScanner::Replace(line,buf2,ch);
			}
			for(c = '0';c<='9';c++){
				ch[0] = c;
				char buf[] = "０";
				buf[1] += c-'0';
				CStringScanner::Replace(line,buf,ch);
			}
			CStringScanner::Replace(line,"／","/");
		}
		
		
		dst_file.Write(line);

		/*
		line noの加算
		*/
		int nLineAdd = line.length()/80;
		nLineNo += nLineAdd + 1;

	}
	
/////////////////////////////////////////////////////////
//	解析部　～ここまで～
/////////////////////////////////////////////////////////
	}
	//	CFileは、デストラクタによって自動的に閉じられる

	//	前の開いてたら閉じる
	if (dst_file.GetFilePtr()!=NULL){
		dst_file.Write("<HR></BODY></HTML>");
		OutPageTotal(nChap,nLineNo);
	}

	return 0;
}

void	CScnConvert::Err(string ErrMes){
	CDbg().Out("Error>"+CStringScanner::NumToString(m_nLineNo) + "行目:" + ErrMes);
}

void	CScnConvert::OutPageTotal(int nChapter,int nLineNo){
	CFile file;
	if (file.Open("html/covertlog.txt","aw")!=0) return ;
	fprintf(file.GetFilePtr(),"Chapter %.2d ... %.6d lines .. %.4d pages\n",nChapter,nLineNo,nLineNo/40 + 1);
}
