//  CFEucConverter.h
//		EUC�����̂܂܃��C�h�o�C�g�ɕϊ�����N���X�iUNICODE�ւ̕ϊ��ł͂Ȃ��j
//			programmed by coLun	'03/05/12 - '03/05/19

#ifndef __CFEucConverter_h__
#define __CFEucConverter_h__

#include "IRegex.h"
/*
�Q�l�y�[�W�iSJIS�҂���̃R�s�y�j�F�i�T�C�g�g�b�v���Ⴀ��܂���j
	http://www.asahi-net.or.jp/~hc3j-tkg/unicode/
	http://tohoho.wakusei.ne.jp/wwwkanji.htm

EUC�ƁA���C�h�o�C�g����EUC�Ƃ̕ϊ��iSJIS�҂���̃R�s�y�j�F
	�@UNICODE�ɏ�����ASCII��128������00XX�Ƃ���̂ŁA
	�@�}���`�o�C�g���ɕ����񖖓��ő��_�̂��Ȃ������ꍇ��XX00�ŕ\�����Ƃɂ���B
	�i�{�v���O�����ł̓}���`�o�C�g�����̂Q�o�C�g�ڂ̔��f�͍s���Ă��Ȃ��̂ŁA���_�����Ȃ��͕̂����񖖂̂݁j
	�@�����\xAA\xBB\xCC\xDD�Ƃ����}���`�o�C�g�R�[�h��
	�@\xAABB\xCCDD�ƕϊ������K�v������A���g���G���f�B�A���n�r�Ȃ̂Ƀr�b�O�G���f�B�A���ƂȂ�B
	�i�܂��A�n�r���ǂ��̂͊֌W�Ȃ��ł��ˁE�E�Ewchar_t*��char*�ɖ������ϊ��Ƃ����Ȃ�����́j

	�Q�l�܂ł�char�͈̔͂�-128�`127�܂ł�signed�Ȃ̂ɁAwchar_t��0�`65536�܂ł�unsigned�E�E�E�炵���ł��E�E�E
	�i���f�_���[�@�i���f�_���[�@�i�q�f�_�i���f�_���[
*/

class CFEucConverter : public IJcodeConverter{
public:
	virtual ~CFEucConverter(){};
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
						//\xAA\xBB\xCC\xDD�Ƃ����}���`�o�C�g�R�[�h��
						//\xAABB\xCCDD�ƕϊ������B�i���g���G���f�B�A���p�\�R���ŁA�r�b�O�G���f�B�A���I�H�j
						wstr.append((size_t)1, (((wchar_t)c)<<8)|(((wchar_t)str[i]) & 0xff));
						++i;
					}
					else{
						//�}���`�o�C�g���ɕ����񖖂������ꍇ��\xXX00�Ƃ���B
						wstr.append((size_t)1, ((wchar_t)c)<<8);
					}
				} else if (c=='\x8F') {
				//EUC����ǉ������E�E�E�R�o�C�g�������Ă�ł��B
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
					//ASCII��7�r�b�g���̃R�[�h��\x00XX�Ƃ���
					wstr.append((size_t)1, ((wchar_t)c) & 0xff);
					++i;
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
