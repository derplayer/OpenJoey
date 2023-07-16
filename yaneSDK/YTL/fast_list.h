/*
	fast_list	: O(log N)で検索できるlist
		programmed & desinged by yaneurao(M.Isozaki) '02/11/18
*/

#ifndef __YTLFastList_h__
#define __YTLFastList_h__

namespace yaneuraoGameSDK3rd {
namespace YTL {

template <class T>
class fast_list {
/**
	キャッシュシステム
		・オブジェクトは番号(class T)で管理
		・参照することによってチェインの先頭にくる
		・チェインの最後尾（一番参照していないもの）を
			取り出す機能あり
		・オブジェクトの生成／解放にはこのクラスは関与しない
*/
public:
	typedef list<T> listchain;
	typedef map<T, typename listchain::iterator> listchainmap;

	LRESULT ReferToObject(const T& t)
		/**
			IDに対するオブジェクトを参照したものとして、
			このオブジェクトに対する使用カウントを加算する

			返値
			0: そのオブジェクトIDはすでにリストに存在した
				参照したということで、リスト先頭に移動させた
	
			1: そのオブジェクトIDは存在しなかった
				ので先頭に追加した。
		*/
	{
		listchainmap::iterator it;
		it = GetMap()->find(t);
		if (it==GetMap()->end()){
			//	そのkeyは存在しない
			GetList()->push_front(t);
			GetMap()->insert(pair<T,listchain::iterator>(t,GetList()->begin()));
//			m_funcCreateDelegate(n);
			return 1;
		}
		GetList()->erase(it->second);	//	この要素を先頭に移動
		GetList()->push_front(t);		//	先頭に追加
		it->second = GetList()->begin();//	mapのiteratorがそこを指すように変更
		return 0;
	}


	LRESULT	releaseLast(T& t)
	/**
		最後のひとつ(一番使っていないオブジェクト)を解放する

		返値
			0 : 正常解放
				（このとき、tに解放したIDが入る）
			1 : 解放すべきオブジェクトが無い 
	*/
	{
		listchain::reverse_iterator it = GetList()->rbegin();
		if (it==GetList()->rend()) {
			return 1; // 解放すべきオブジェが無い
		}
//		m_funcReleaseDelegate(*it);
		t = *it; // こいつだ！
		GetMap()->erase(*it);   // それをmap上から参照しているiteratorを削除
		GetList()->erase(it.base());
		return 0;
	}

	LRESULT release(const T& id)
	/**
		このIDで指定したオブジェクトを解放する

		返値
			0 : 正常解放
			1 : 指定されたオブジェクトは存在しなかった
	*/
	{
		listchainmap::iterator it;
		it = GetMap()->find(id);
		if (it==GetMap()->end()){
			//	そのkeyは存在しない
			return 1;
		}
		GetList()->erase(it->second);	//	この要素を削除
		GetMap()->erase(it);
		return 0;
	}

	LRESULT	releaseAll()
		/**
			すべてのオブジェクト(LookObjectで参照したもの)を
			解放する。

			解放は、setReleaseDelegateで設定した関数を
			呼び出すことによって行なうなら以下のように書くべきか．．
		listchain::iterator it = GetList()->begin();
		while (it!=GetList()->end()){
			m_funcReleaseDelegate(*it);
			it++;
		}
		GetList()->clear();
		GetMap()->clear();
		*/
	{
		GetList()->clear(); GetMap()->clear();
		//　オブジェクトの生成／解放にはこのクラスは関与しない

		return 0;
	}

	bool	isExist(const T& id)
	///	このIDに対応するオブジェクトが存在するか調べる
	///	存在するならばtrueが返る

	/**
		以下のGetListとGetMapは、互いに連動しているので
		いっぽうだけの削除等は不可
	*/
	{
		listchainmap::iterator it;
		it = GetMap()->find(id);
		return it!=GetMap()->end();
	}

	listchain* GetList() { return& m_list; }
	///	参照した番号順のリスト。この後方にあるものが古いものだから
	///	そいつらから破棄するヨロシ

	listchainmap* GetMap() { return &m_map; }
	///	listだと線形探索になって大規模だと遅いので、iteratorを別途保持

protected:
	listchain m_list;
	//	参照した番号順のリスト。この後方にあるものが古いものだから
	//	そいつらから破棄するヨロシ

	listchainmap m_map;
	//	listだと線形探索になって大規模だと遅いので、iteratorを別途保持
};

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
