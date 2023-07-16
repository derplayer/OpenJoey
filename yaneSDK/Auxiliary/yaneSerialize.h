//
//	シリアライズ用クラス
//			programmed by yaneurao	'01/07/16
//
//		ひとつのシリアライズ関数で、ストリームへの保存／復元を
//		兼用するあたりが、新たな発明のような気もするのだが、
//		そうでも無いのだろうか．．＾＾；
//

#ifndef __yaneSerialize_h__
#define __yaneSerialize_h__


namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class ISerialize;
class IArchive {
/**
	シリアライズ用アーカイブクラス

// シリアライズ用アーカイブクラス

virtual void Serialize(ISerialize&)=0;
// こいつをオーバーライドして使うといいのだ

例）
　　void Serialize(ISerialize&s){
　　　s << nData << strData;
　　}

// こんな感じでシリアライズしたい、メンバをすべて書き出すコードを書くべし

そうすれば、
　　　CSerialize s;
　　　CArchive派生クラス a;
　　　s << a;
とシリアライズできるようになる。

*/
public:
	virtual void Serialize(ISerialize&)=0;
	//	こいつをオーバーライドして使うといいのだ

	virtual ~IArchive() {} // place holder

	/**
		例）
			void Serialize(CSerialize&s){
				s << nData << strData;
			}
			//	こんな感じで必要なメンバをすべて書き出すコードを書くべし
	*/
};

/**
		⇒　CSerializeのシリアライズは現在のストア方向(IsStoring)
			参照位置(GetPos)も、一緒にシリアライズするので、
			復元時には、そこを注意する必要がある。
*/

class ISerialize {
public:
	virtual	void	Clear()=0;

	virtual bool	IsStoring() const =0;
	virtual bool*	GetStoring()=0;
	virtual void	SetStoring(bool b)=0;
	virtual void	InnerSetStoring(bool b)=0;
	virtual vector<BYTE>* GetData()=0;
	virtual ISerialize& operator << (IArchive& vData)=0;
	virtual ISerialize& operator << (int& nData)=0;
	virtual ISerialize& operator << (bool& bData)=0;
	virtual ISerialize& operator << (BYTE& byData)=0;
	virtual ISerialize& operator << (string& strData)=0;
	virtual ISerialize& operator << (WORD& wData)=0;
	virtual ISerialize& operator << (DWORD& dwData)=0;
	virtual ISerialize& operator << (vector<int>& anData)=0;
	virtual ISerialize& operator << (vector<bool>& abData)=0;
	virtual ISerialize& operator << (vector<BYTE>& abyData)=0;
	virtual ISerialize& operator << (vector<string>& astrData)=0;
	virtual ISerialize& operator << (vector<WORD>& awData)=0;
	virtual ISerialize& operator << (vector<DWORD>& adwData)=0;
	virtual ISerialize& operator << (ISerialize& vData)=0;
	virtual ISerialize& operator = (ISerialize& vSeri)=0;
	virtual LRESULT Save(const string& filename)=0;
	virtual LRESULT Load(const string& filename)=0;
	virtual int*	GetPos()=0;

	///	任意のデータ型を書き出すためのもの
	///	メンバ関数テンプレートで実装
	template <class T>
	ISerialize& SerializeT (T& vData){
		if (IsStoring()){
		//	保存するんかいな？
			//	サイズ不明と仮定したコーディング＾＾；
			int nSize = sizeof(T);
			BYTE* pByte = (BYTE*)&vData;
			for(int i=0;i<nSize;i++){
				GetData()->push_back(pByte[i]);
			}
		} else {
		//	ストリームから取り出すんかいな？
			//	サイズ不明と仮定したコーディング＾＾；
			int nSize = sizeof(T);
			BYTE* pByte = (BYTE*)&vData;
			for(int i=0;i<nSize;i++){
				pByte[i] = (*GetData())[(*GetPos())++];
			}
		}
		return *this;
	}

	///	あるいは、Arrayも、、
	///	メンバ関数テンプレートでの実装
	template <class T>
	ISerialize& Store(T* pavData,int nSize){
		///	pavData[nSize]をシリアライズ
		if (IsStoring()){
			//	保存するんかいな？
			//	this << nSize;		//	サイズを格納する必要は無い
			for(int i=0;i<nSize;i++){
				*this << *pavData;
				pavData++;
			}
		} else {
			//	取り出すんかいな？
			//	*this << nSize;		//　サイズは明確にわかっている
			for(int i=0;i<nSize;i++){
				*this << *pavData;
				pavData++;
			}
		}
		return *this;
	}

	virtual ~ISerialize(){}
};

class CSerialize : public ISerialize {
/**
	シリアライズ用クラス

	シリアライズ（データ永続化）のため手段。
	メンバ変数・配列などを、ストリームにつっこみ、あるいは取り出して、
	それをファイルに保存／復元することで、アプリケーションが終了したあとも、
	前回の状態を保つことが出来る。

	概略については class IArchive を参照すること。

１．保存するとき
　CSerialize s;
　IArchive派生クラス v;
　s << v; // これで格納
　vector<BYTE>& vData = *(s.GetData());
　↑あとは、こいつを、&vData[0]から、vData.size()バイトだけ
　ファイルとして保存すれば、シリアライズ完了。

２．シリアライズしたデータを読み出すとき
　CSerialize s;
　IArchive派生クラス v;
　vector<BYTE>& vData = *(s.GetData());
　vDataに１．でシリアライズしたファイルから読み込む。
　s.SetStoring(false); // いまから読み出すねん！
　// ↑このフラグさえ、このようにストリームからの読み出しであると
　// 宣言さえすれば、
　s << v; // これ、１．と同じ表記でＯＫ。
　これで、ストリームからの読み出しが完了。

３．上の手順でIArchive派生クラスを使わないとき保存）

　CSerialize s;
　int x,y,z;
　s << x << y << z;
　CSerialize s;
　s.SetStoring(false); // このフラグを倒しておけば取り出し
　int x,y,z;
　s << x << y << z;

　例１．
　int x = 1,y = 2,z = 3;
　CSerialize s;
　string str;
　str = "あいうえお";
　s << str;
　s << x << y << z; // ストリームに保存

　CFile file;
　file.Save("test.dat",s.GetData()); // 保存してみよう＾＾；
　file.Load("test.dat",s.GetData()); // 読み込んでみよう＾＾；

// ↑ s.Save("test.dat");
// s.Load("test.dat"); でも同じ意味

　s.SetStoring(false); // ストリームから獲得
　string sy;
　s << sy; // syには、あいうえおが返ってくる
　s << z << y << x; // z,y,zには、それぞれ1,2,3が返ってくる

注意点：

CSerialize x,y,z;
　x << y << z;を
　x.SetStoring(false); // このフラグを倒しておけば取り出し
　x << y << z;と復元できる。これは、CSerializeの保持している
　アーカイブは、vector<BYTE>であり、vectorの書き出し時には、
　そのサイズvector::size() を先頭にintデータとして書き出してあるためである。よって、

　CSerialize x,y;
　int a,b;
　y << a << b; // yにa,bをシリアライズ
　x << y; // xにyをシリアライズ
とした場合、
　x.SetStoring(false); // このフラグを倒しておけば取り出し
　x << a << b;
では正しく取り出せない。なぜならば、先頭にyのアーカイブサイズが混入しているからである。これを取り除くために、ダミーでintの値を取り出してやると良い。

　int dummy;
　x << dummy << a << b;

配列に対しても正しくシリアライズできる
　例
　int n[5];
　CSerialize x;
　x.Store(&n[0],NELEMS(n));
この部分を、さらにマクロを利用して、

x.Store(ArraySerialize(n));と書ける
（ただし、以下のマクロによる実装なので配列サイズがわかっているときのみ）

#define ArraySerialize(n) &n[0],NELEMS(n)

たとえば、stringに対しても同じように
　string s[5];
　CSerialize x;
　x.Store(ArraySerialize(s));と書ける。

また、IArchive派生クラスに対しても同様に
　CArchiveDerived ar[5];
　CSerialize x;
　x.Store(ArraySerialize(ar));と書けます

☆　CSerializeのシリアライズに関する注意事項　☆

CSerializeは、現在のポジション・格納方向も同時にシリアライズします。

CSerialize s1,s2,s3;
s1 << s2;
のようにした場合、このときs2は、普通、CSerializeの記録時は、格納方向になっているでしょうし、push_backしていくわけで、その格納位置(GetPosで返る)は、末尾になっています。

よって、s1から、
s1.SetStoring(false);
s1 << s3;
と、デシリアライズ（復元）してすぐ、s3からすぐにデータを取り出すことは出来ません。s3からデータをデシリアライズするには、s3.SetStoring(false)として、取り出し方向にしてポジションを先頭にするという作業を行なう必要があります。

よくハマルので、注意しましょう。

*/
public:

	///	データの初期化。
	///	内部のストリームをクリアして、IsStoring()はtrueを返すようになる
	virtual void	Clear();

	virtual bool	IsStoring() const { return m_bStoring; }
	///	格納方向を調べる
	virtual bool*	GetStoring() { return &m_bStoring; }
	///	格納方向へのポインタを取得する
	virtual void	SetStoring(bool b)
	{ m_bStoring = b; m_nDataPos = 0; if (b) GetData()->clear(); }
	/**	↑これをfalseを設定して、取得（復元）方向にしたとき、
		データのどこから読み出しているかを示す
		データポジションポインタもクリアする
		データを保存方向にしたときは、データもクリアする
		（そうしないと、前回のデータに上書きしてしまう）
	*/
	virtual void	InnerSetStoring(bool b) { m_bStoring = b; }
	///	データポジションのリセット・データクリア等を行なわずに格納方向だけ
	///	変更するならば、こっちを使うべし

	///	ストリームの取得
	virtual vector<BYTE>* GetData() { return& m_abyData; }

	//	------ 各種データの格納用オペレータ
	///	アーカイブ！
	virtual ISerialize& operator << (IArchive& vData);

	///	primitive dataに対するシリアライズ
	virtual ISerialize& operator << (int& nData);
	virtual ISerialize& operator << (bool& bData);
	virtual ISerialize& operator << (BYTE& byData);
	virtual ISerialize& operator << (string& strData);
	virtual ISerialize& operator << (WORD& wData){// 追加
		return SerializeT(wData);
	}
	virtual ISerialize& operator << (DWORD& dwData){
		return SerializeT(dwData);
	}

	///	vectorも！
	virtual ISerialize& operator << (vector<int>& anData);
	virtual ISerialize& operator << (vector<bool>& abData);
	virtual ISerialize& operator << (vector<BYTE>& abyData);
	virtual ISerialize& operator << (vector<string>& astrData);
	virtual ISerialize& operator << (vector<WORD>& awData);// 追加
	virtual ISerialize& operator << (vector<DWORD>& adwData);// 追加


	///	自分自身も！＾＾；
	virtual ISerialize& operator << (ISerialize& vData);


	//	-----------------------------------------------------------

	///	そして、シリアライズ間のコピー！
	virtual ISerialize& operator = (ISerialize& vSeri);

	///	あまり、こういうの用意したくないが．．
	virtual LRESULT Save(const string& filename);
	///	ストリームのファイルへの保存
	virtual LRESULT Load(const string& filename);
	///	ストリームのファイルからの復元

	///	現在の格納位置へのポインタを取得
	///	（使わんほうがええけどデバッグ用に。）
	virtual int*		GetPos() { return& m_nDataPos; }

	//	コンストラクタ
	CSerialize() { Clear(); }
	virtual ~CSerialize() {}

protected:
	vector<BYTE> m_abyData;	//	ここにデータは保存される(これをストリームと呼ぶ)
	bool		m_bStoring; //　ストリームに保存中なのか、取り出し中なのか？
	int			m_nDataPos;	//　ストリームからデータを取り出すとき、
							//	現在何バイト目を指しているかを示す
};

///	配列シリアライズのヘルパ
#define ArraySerialize(n) &n[0],NELEMS(n)

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
