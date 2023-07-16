//	LZSSによる圧縮ルーチン
//
//	☆　参考文献
//	  奥村晴彦著『C言語による最新アルゴリズム事典』／技術評論社
//

#ifndef __yaneLZSS_h__
#define __yaneLZSS_h__


namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

const int g_LZSS_RING_BUFFER	= 4096; // 環状バッファの大きさ
const int g_LZSS_LONGEST_MATCH	= 18;	// 最長一致長

class ICompress {
/**
	圧縮用インターフェース
		LZSS法で圧縮する実装 class CLZSS も参照のこと。
*/
public:
	///	Decodeする。Dst先のバッファはbDstがtrueならば確保される
	virtual LRESULT Decode(BYTE* lpSrc,BYTE*& lpDst,DWORD  dwSize,bool bDst=true)=0;

	///	Encodeする。Dst先のバッファはbDstがtrueならば確保される
	virtual LRESULT Encode(BYTE* lpSrc,BYTE*& lpDst,DWORD  dwSize,DWORD& dwDstSize,bool bDst=true)=0;

	/**	↑上のの、smart_ptrをとるバージョン
		lpDst.AddArray(size) してメモリを確保しておけば、そこに展開／圧縮される
		lpDst.isNull() == trueならばメモリを確保して返す
	*/
	virtual LRESULT Decode(BYTE* lpSrc,smart_ptr<BYTE>& lpDst,DWORD	 dwSize)=0;
	virtual LRESULT Encode(BYTE* lpSrc,smart_ptr<BYTE>& lpDst,DWORD	 dwSize,DWORD& dwDstSize)=0;

	virtual ~ICompress(){}
};

class CLZSS : public ICompress {
/**
	LZSS法による圧縮ルーチン。
	圧縮はやや遅いですが、展開はメチャ速いのが特徴です。

*/
public:
	//	Decodeする。Dst先のバッファはbDstがtrueならば確保される
	virtual LRESULT Decode(BYTE* lpSrc,BYTE*& lpDst,DWORD  dwSize,bool bDst=true);
	/**
		lpSrcは、展開元のバッファ（圧縮データ）。
		lpDstは、展開先のバッファ（非圧縮データ）。
		（bDst==trueならばこれはDecodeルーチン内でnewで確保されるので、
		展開されたバッファを使用終了後にdelete [] lpDstすれば良い）
		dwSizeは、展開後のバッファ（非圧縮データ）のサイズ。
	*/

	//	Encodeする。Dst先のバッファはbDstがtrueならば確保される
	virtual LRESULT Encode(BYTE* lpSrc,BYTE*& lpDst,DWORD  dwSize,DWORD& dwDstSize,bool bDst=true);
	/**
		lpSrcは、圧縮元のバッファ（非圧縮データ）。
		lpDstは、圧縮先のバッファ（圧縮データ）。
		（bDst==trueならば、これはEncodeルーチン内でnewで確保されるので、
		圧縮されたバッファを使用終了後にはdelete [] lpDstすれば良い）
		dwSizeは、圧縮元のバッファサイズ。dwDstSizeは、圧縮先のバッファサイズ。

		bDst==falseで呼び出した場合、実際の圧縮は行なわない。
		圧縮後のサイズを仮で取得するときなどに使う。
		（このときは、lpDstにはdummy変数か、*(BYTE**)NULLなどでＯＫです。
		その他、LPVOID型の変数を渡したいのであれば参照渡しなので、
		*(BYTE**)&lpBufというようにキャストしてください）

		圧縮率が１００％を超えるような場合には、この関数は非０を返します。
		そのときは、そのファイルの圧縮は諦めてください＾＾
	*/

	/**	↑上のの、smart_ptrをとるバージョン
		lpDst.SetArray(size) してメモリを確保しておけば、そこに展開／圧縮される
		lpDst.isNull() == trueならばメモリを確保して返す
	*/
	virtual LRESULT Decode(BYTE* lpSrc,smart_ptr<BYTE>& lpDst,DWORD	 dwSize);
	virtual LRESULT Encode(BYTE* lpSrc,smart_ptr<BYTE>& lpDst,DWORD	 dwSize,DWORD& dwDstSize);

protected:
	//	出力バイト数カウンタ
	DWORD	m_dwOutCount;

	// テキスト用バッファ
	UCHAR	m_szText[g_LZSS_RING_BUFFER+g_LZSS_LONGEST_MATCH-1];

	//	木
	int		m_dad[g_LZSS_RING_BUFFER+1];
	int		m_lson[g_LZSS_RING_BUFFER+1];
	int		m_rson[g_LZSS_RING_BUFFER+257];

	void	init_tree(void);	//	木の初期化
	void	insert_node(int r);	//	節 r を木に挿入
	void	delete_node(int p);	//	節 p を木から消す

	int		m_matchpos, m_matchlen;	 /* 最長一致位置, 一致長 */

};

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
