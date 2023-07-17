//  CFJisConverter.h
//		JIS��EUC�����ă��C�h�o�C�g�ɕϊ�����N���X�iUNICODE�ւ̕ϊ��ł͂Ȃ��j
//			programmed by coLun	'03/05/12 - '03/05/22

#ifndef __CFJisConverter_h__
#define __CFJisConverter_h__

#include "IRegex.h"
/*
�Q�l�y�[�W�iSJIS�҂���̃R�s�y�j�F�i�T�C�g�g�b�v���Ⴀ��܂���j
	http://www.asahi-net.or.jp/~hc3j-tkg/unicode/
	http://tohoho.wakusei.ne.jp/wwwkanji.htm

JIS�ƁA���C�h�o�C�g����JIS�Ƃ̕ϊ��iSJIS�҂���̃R�s�y�j�F
	�@UNICODE�ɏ�����ASCII��128������00XX�Ƃ���̂ŁA
	�@�}���`�o�C�g���ɕ����񖖓��ő��_�̂��Ȃ������ꍇ��XX00�ŕ\�����Ƃɂ���B
	�i�{�v���O�����ł̓}���`�o�C�g�����̂Q�o�C�g�ڂ̔��f�͍s���Ă��Ȃ��̂ŁA���_�����Ȃ��͕̂����񖖂̂݁j
	�@�����\xAA\xBB\xCC\xDD�Ƃ����}���`�o�C�g�R�[�h��
	�@\xAABB\xCCDD�ƕϊ������K�v������A���g���G���f�B�A���n�r�Ȃ̂Ƀr�b�O�G���f�B�A���ƂȂ�B
	�i�܂��A�n�r���ǂ��̂͊֌W�Ȃ��ł��ˁE�E�Ewchar_t*��char*�ɖ������ϊ��Ƃ����Ȃ�����́j

	�Q�l�܂ł�char�͈̔͂�-128�`127�܂ł�signed�Ȃ̂ɁAwchar_t��0�`65536�܂ł�unsigned�E�E�E�炵���ł��E�E�E
	�i���f�_���[�@�i���f�_���[�@�i�q�f�_�i���f�_���[

SJIS��EUC�Ɍ����Ȃ�JIS���L�̃G�X�P�[�v�V�[�P���X�F�i�Q�l�y�[�W�̂Ƃق�WWW���傩��̓]�p�j
�L���\�L			16�i�\�L			�Ӗ�								�R�����̂�����
ESC ( B				1B 28 42			ASCII								��������1�o�C�g�R�[�h
ESC ( J				1B 28 4A			JIS X 0201-1976 Roman Set			 - jcode.pl�ŏ�Ɠ�������
ESC ( I				1B 28 49			JIS X 0201-1976 �Љ���				 - 1�o�C�g�R�[�h�B�E�E�E���ꂪEUC�ɕϊ�������2�o�C�g�ɁE�E�E�I�I�@�ł�JIS�ɂ͖��֌W�B
ESC $ @				1B 24 40			JIS X 0208-1978(�ʏ́F��JIS)		��������2�o�C�g�R�[�h
ESC $ B				1B 24 42			JIS X 0208-1983(�ʏ́F�VJIS)		 - jcode.pl�ŏ�Ɠ��������B������ʂ���K�v�Ȃ�
ESC & @ ESC $ B		1B 26 40 1B 24 42	JIS X 0208-1990						 - �]�v�Ȃ��̂����Ă邯�Ǎ��{�͏�ɓ����B
ESC $ ( D			1B 24 28 44			JIS X 0212-1990						 - 2�o�C�g�R�[�h�E�E�E���ꂪEUC�ɕϊ�������3�o�C�g�ɁE�E�E�I�I�@�ł�JIS�ɂ͖��֌W�B

	�@JIS�ł͕����R�[�h�̈悪�A���ꂼ��̃G�X�P�[�v�V�[�P���X�ł��Ԃ�̂ŁA
	�@�������̂��Ƃ���EUC�ɕϊ�����̂���Ԏ����葁�����ł��B
	�@�Ƃ����킯�ŁA���̃v���O�����́@�uJIS��������JIS�v�ł͂Ȃ��A�uJIS��������EUC�v�ɂȂ�܂ӁB
*/

class CFJisConverter : public IJcodeConverter{
public:
	virtual ~CFJisConverter(){};
	virtual basic_string<wchar_t> ConvertToWString(const basic_string<char> & str, bool flag=false){
	//String��WString�ɕϊ����܂��B�t���O��true�̂Ƃ��A���̕�������N���X���ɋL�����A���GetPos��GetWPos�Ȃǂ��g���܂�

		if(flag){m_str=str;}

		basic_string<wchar_t> wstr_dmy;
		basic_string<wchar_t> & wstr=flag ? m_wstr : wstr_dmy;
		//������ł��߂ȂƂ��́�����Ȃ̂ǂ��H
		//basic_string<wchar_t> & wstr=*(flag ? (&m_wstr) : (&wstr_dmy));

		wstr.erase();
		wstr.reserve(str.size());
		if(flag){
			m_posmap.resize(str.size()+1);//�I�[�̏ꏊ�f�[�^�܂Ŋ܂܂ȃA�J���̂�+1���Ă܂�
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
						//ASCII��7�r�b�g���̃R�[�h��\x00XX�Ƃ���
						wstr.append((size_t)1, ((wchar_t)c) & 0xff);
						++i;
					} else if(state==1) {//�S�p����
						++i;
						if(i<e && str[i]!='\x1B'){
							if(flag){m_wposmap[i]=(int)wstr.size();}
							wstr.append((size_t)1, (((wchar_t)c) & 0xff) | (((wchar_t)str[i])<<8) | 0x8080);
							++i;
						}
					} else if(state==2) {//���p�J�i
						++i;
						wstr.append((size_t)1, (((wchar_t)c) & 0xff) | 0x8E80);
					} else if(state==3) {//�������
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
		if(flag){m_posmap.resize(wstr.size()+1);}//�I�[�̏ꏊ�f�[�^�܂Ŋ܂܂ȃA�J���̂�+1���Ă܂�
		return wstr;
	}
	virtual basic_string<char> ConvertToString(const basic_string<wchar_t> & wstr, bool flag){
	//�R���o�[�^����WString�����܂��B�ȍ~�AString��WString�Ǝ��o�����R�ŁA
	//���ꂼ��̕���pos�Ԃł̕ϊ����ȒP�ɍs���܂��B
		if(flag){m_wstr=wstr;}

		basic_string<char> str_dmy;
		basic_string<char> & str=flag ? m_str : str_dmy;
		//������ł��߂ȂƂ��́�����Ȃ̂ǂ��H
		//basic_string<char> & str=*(flag ? (&m_str) : (&str_dmy));

		str.erase();
		str.reserve(wstr.size()*2);
		if(flag){
			m_posmap.resize(wstr.size()+1);//�I�[�̏ꏊ�f�[�^�܂Ŋ܂܂ȃA�J���̂�+1���Ă܂�
			m_wposmap.resize(wstr.size()*5+3+1);//�ň��̎��Ԃ�z�肷��ƁA*5+3+1�ɂȂ�̂łӁE�E�E
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
					if((wc & 0xff00)!=0x8E00){//�S�p
						if(state!=1){//�S�p���[�h�ɂ���
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
					}else{//���p�J�i
						if(state!=2){//���p�J�i���[�h�ɂ���
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
				} else if(wc==0x8F) {//�������
					++i;
					if(i<wstr.size()){
						if(flag){m_posmap[i]=(int)str.size();}
						wc=wstr[i];
						++i;
						if(i<wstr.size()){
							if(flag){m_posmap[i]=(int)str.size();}
							if(state!=3){//����������[�h�ɂ���
								str.append("\x1B$(D");
								if(flag){
									m_wposmap[str.size()-3]=(int)i-2;
									m_wposmap[str.size()-2]=(int)i-2;
									m_wposmap[str.size()-1]=(int)i-2;
									m_wposmap[str.size()]=(int)i-2;
									m_posmap[i-2]=(int)str.size();//�G�X�P�[�v�R�[�h�̌��pos������K�v������̂ŁE�E�E
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
					if(state!=0){//ASCII���[�h�ɂ���
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
			if(state!=0){//ASCII���[�h�ɂ���
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
		if(flag){m_wposmap.resize(str.size()+1);}//�I�[�̏ꏊ�f�[�^�܂Ŋ܂܂ȃA�J���̂�+1���Ă܂�
		return str;
	}
	virtual basic_string<char> GetString(){
		return m_str;
	}
	virtual basic_string<wchar_t> GetWString(){
		return m_wstr;
	}
	virtual size_t GetPos(size_t wpos){
	//�u������n�v���O�Ɓu�����񖖁v����͎g��Ȃ��悤�Ɋ肢�܂��B
	//����͖���`�ł��B
	//�i�{�����ł͖\�����܂��j
		return m_posmap[wpos];
	}
	virtual size_t GetWPos(size_t pos){
	//�u������n�v���O�Ɓu�����񖖁v����͎g��Ȃ��悤�Ɋ肢�܂��B
	//����͖���`�ł��B
	//�i�{�����ł͖\�����܂��j
		return m_wposmap[pos];
	}
private:
	basic_string<char> m_str;
	basic_string<wchar_t> m_wstr;
	vector<size_t> m_posmap;
	vector<size_t> m_wposmap;
};

#endif
