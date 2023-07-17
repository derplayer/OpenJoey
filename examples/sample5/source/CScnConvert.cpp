
#include "stdafx.h"
#include "../../yaneSDK/yaneSDK.h"
#include "CScnConvert.h"

///////////////////////////////////////////////////////////
//	�R���o�[�^�{�́�
//	������Ƃ��́A�����K���ɂ������Ă���͂��B

LRESULT CScnConvert::Convert(string filename,string outputpath){

	//	�\�[�X�t�@�C��
	CFile src_file;
	src_file.Read(filename);

	//	�������ރt�@�C��
	CFile dst_file;
	bool bWrite=false; // ��������CHU�I

	m_nLineNo = 0;	//	��͒��̍s�ԍ�
	bool bRemoveBracket = false;

	bool bOutBR2 = true;

	string title;	//	�y�[�W�^�C�g��

	int	anBox[10] = {0};	//	box�p�̒ʂ��i���o�[
	string aszBox[10] ={
		"�r��-","list-","�g-","�}-"
	};
	int nChap = 0;
	int	nSection = 0;
	int nLineNo = 0;

	CFile listfile;

	while (true){
/////////////////////////////////////////////////////////
//	��͕��@�`��������`
/////////////////////////////////////////////////////////
	string linebuf;
	if (src_file.ReadLine(linebuf)!=0) break;
	m_nLineNo++;

	LPSTR psz = (LPSTR)linebuf.c_str();
	string line = linebuf;	//	�o�͗p�̃��C���̓R�s���Ƃ��I
	//	�������Ă����΁Aline��ׂ��Ă��Apsz�̃|�C���g��ɂ͉e�����Ȃ�

	//	���̍s�̍Ō��<BR>����荞�ނ̂��H
	bool bOutBR = true;

	//	���@�����擪�� // ����n�܂�̂ł���΁A����͓�����ʎw��
	if (CStringScanner::IsToken(psz,"//")){

		//	������ʎw��i�^�O�̏ȗ��\�L�j�Ȃ̂ŁA�s����<HR>�͕��荞�܂Ȃ��ėǂ�
//		bOutHR = false;
		
		//	�|�|�|�|�@�ȉ��A�R�s�y�̗��O�O�G

		/*
			// log �����́A�o�͂���V�i���I�t�@�C���i���o�[
			��.
				//	log 100
				�Ƃ���΁A�����ȍ~��log0100.html�Ƃ����t�@�C���ɏo��
		*/
		if (CStringScanner::IsToken(psz,"chap")){
			int nSceneNo;
			if (CStringScanner::GetNumFromCsv(psz,nSceneNo)!=0){
				Err("chap�w��i���o�[�̎w�肪���������B");
			} else {
				//	�O�̊J���Ă������
				if (dst_file.GetFilePtr()!=NULL){
					dst_file.Write("<HR></BODY></HTML>");
					OutPageTotal(nChap,nLineNo);
					nLineNo = 0;
				}

				//	�r���f�[�^������
				ZERO(anBox);
				nSection = 0;
				nChap = nSceneNo;

				string obj_filename;
				obj_filename = outputpath + "/chap" + CStringScanner::NumToStringZ(nSceneNo,4)+".html";
				dst_file.Open(obj_filename.c_str(),"w");
				bWrite = true;

//	�\�\�\�w�b�_
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
			//	title	�y�[�W�^�C�g��
			�˂���́A�^�C�g��������Ƃ��ď�������
		*/
		if (CStringScanner::IsToken(psz,"title")){
			title = psz;
			continue;
		}

		/*
			//	sect �����o��
			�ˁ@��2.1�@�̂悤�ɂȂ�
		*/
		if (CStringScanner::IsToken(psz,"sect")){
			nSection++;
			dst_file.Write("<HR>\n��"+
				CStringScanner::NumToString(nChap)+"."+
				CStringScanner::NumToString(nSection)+"�@"+psz+"<BR>"
			);
			continue;
		}

		/*
			//	date 2 1 2
			���@<HR><P><FONT size="+2">2002</FONT> �N <FONT size="+2">1</FONT> �� <FONT size="+2">2</FONT>�� <A href="#top"></P>
		*/
		if (CStringScanner::IsToken(psz,"date")){
			int n1,n2,n3;
			if (CStringScanner::GetNumFromCsv(psz,n1)!=0 ||
				CStringScanner::GetNumFromCsv(psz,n2)!=0 ||
				CStringScanner::GetNumFromCsv(psz,n3)!=0){
					Err("date�w��i���o�[�̎w�肪���������B");
				//	���G���[�͑�P�p�����[�^�����݂��Ȃ��Ƃ��B
			} else {
				if (n1 <2000) { n1+=2000; }	//	2000�N����[��
				line = "<HR><P><FONT size=\"+2\">"+CStringScanner::NumToString(n1)
						+ "</FONT> �N <FONT size=\"+2\">"
						+ CStringScanner::NumToString(n2)
						+ "</FONT> �� <FONT size=\"+2\">"
						+ CStringScanner::NumToString(n3)
						+ "</FONT>��</P>"; // <A href=\"#top\">��</A></P>";
				goto output;
			}
		}
		/*
			//	box �` ebox�̓{�b�N�X������
		*/
		if (CStringScanner::IsToken(psz,"box")){
			int n1;
			string line2;
			if (CStringScanner::GetNumFromCsv(psz,n1)==0){
				line2 = aszBox[n1] + CStringScanner::NumToString(anBox[n1]+1) + psz;
				anBox[n1]++;
			}

			if (n1==1){
			//	list�Ȃ̂Ńt�@�C���ɏo��
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
			if (n1!=3) {	//	�}�̂Ƃ��́A����͖���
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
	//	��s�͖���
	//	ReadLine�Ȃ̂ŉ��s�R�[�h�łȂ�'\0'�����o����΂���
	if (*psz == '\0') continue;
*/
	if (CStringScanner::IsToken(psz,"http://")) {
		line = "<A href=\"" + line + "\">"+line + "</A>";
		dst_file.Write(line);
		bOutBR2 = true;
		continue;
	}

	//	����ȊO�Ȃ�΂��̂܂܏�������

	//	�P�s��������
output:;
	if (bWrite) {
		//	list file�ɂ��o�͂���
		if (listfile.GetFilePtr()!=NULL){
			listfile.Write(line);
			listfile.Write("\n");
		}

		//	�^�u�R�[�h��pre�^�O�̖����Ƃ���ł͔��p�X�y�[�X�S�ɒu������
		{
Loop:;
			LPCSTR psz = line.c_str();
			int i=0;
			while (*psz!='\0'){
				if (*psz == '\t'){
					//	�S�^�u�̋󔒂ŃT�v���X����
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
				//	pre�^�O�̓���g�Ȃ̂ŁA< >�͒u�����ׂ�
				CStringScanner::Replace(line,"&","&amp;");
				CStringScanner::Replace(line,"<","&lt;");
				CStringScanner::Replace(line,">","&gt;");
			}
			line += '\n';
		}
		//	%���AC����̏o�͏����Ɋ�Â��Ēu������Ă��܂��Ƃ����Ȃ�
		CStringScanner::Replace(line,"%","%%"); // %��%%�ɒu��

		//	�S�p�A���t�@�x�b�g�̔��p��
		{
			char ch[] = "A";
			char c;
			for(c = 'A';c<='Z';c++){
				ch[0] = c;
				char buf[] = "�`";
				buf[1] += c-'A';
				CStringScanner::Replace(line,buf,ch);

				ch[0] = c-'A'+'a';
				char buf2[] = "��";
				buf2[1] += c-'A';
				CStringScanner::Replace(line,buf2,ch);
			}
			for(c = '0';c<='9';c++){
				ch[0] = c;
				char buf[] = "�O";
				buf[1] += c-'0';
				CStringScanner::Replace(line,buf,ch);
			}
			CStringScanner::Replace(line,"�^","/");
		}
		
		
		dst_file.Write(line);

		/*
		line no�̉��Z
		*/
		int nLineAdd = line.length()/80;
		nLineNo += nLineAdd + 1;

	}
	
/////////////////////////////////////////////////////////
//	��͕��@�`�����܂Ł`
/////////////////////////////////////////////////////////
	}
	//	CFile�́A�f�X�g���N�^�ɂ���Ď����I�ɕ�����

	//	�O�̊J���Ă������
	if (dst_file.GetFilePtr()!=NULL){
		dst_file.Write("<HR></BODY></HTML>");
		OutPageTotal(nChap,nLineNo);
	}

	return 0;
}

void	CScnConvert::Err(string ErrMes){
	CDbg().Out("Error>"+CStringScanner::NumToString(m_nLineNo) + "�s��:" + ErrMes);
}

void	CScnConvert::OutPageTotal(int nChapter,int nLineNo){
	CFile file;
	if (file.Open("html/covertlog.txt","aw")!=0) return ;
	fprintf(file.GetFilePtr(),"Chapter %.2d ... %.6d lines .. %.4d pages\n",nChapter,nLineNo,nLineNo/40 + 1);
}
