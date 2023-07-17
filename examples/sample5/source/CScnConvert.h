
//
//	yaneSDK2ndのシナリオクラス用のシナリオコンバートクラス
//
//					programmed by yaneurao('02/01/03)

#pragma once

class CScnConvert {
public:
	LRESULT Convert(string filename,string outputpath);
	//	あるファイルを、変換する。出力先pathは、こいつで設定しておく
	void	Err(string ErrMes);
protected:
	int		m_nLineNo;	//	解析中のラインナンバー

	//	ページ数のトータルを出力
	void	OutPageTotal(int nChapter,int nLineNo);
};
