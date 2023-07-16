//	yaneKey.h :
//		Integrated Key Input using CVirtualKey(sample)
//
//		programmed by yaneurao '00/02/29

#ifndef __yaneKey_h__
#define __yaneKey_h__


#include "yaneVirtualKey.h"		//	仮想キー管理
#include "yaneKeyInput.h"		//	キー入力
#include "yaneJoyStick.h"		//	JoyStick入力

//////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Input {

class CKey1 : public CVirtualKey {
/**

  class CKey1,class CKey2,class CKey3,class CKey4 は、
  キーデバイス登録済み、仮想キー設定済みのクラスです。

	ＫｅｙＢｏａｒｄ　＋　ＪｏｙＳｔｉｃｋ判定
	こちらは、上下左右＋２ボタンのゲーム用
	ジョイスティックは１が接続されていないときは自動的に２を使うようになります

０：ＥＳＣキー
１：テンキー８，↑キー，ジョイスティック↑
２：テンキー２，↓キー，ジョイスティック↓
３：テンキー４，←キー，ジョイスティック←
４：テンキー２，→キー，ジョイスティック→
５：スペースキー，ジョイスティック　ボタン１
６：テンキーEnter,リターンキー，左シフト，右シフト。ジョイスティック　ボタン２
	
*/
public:
	CKey1(bool bBackGround=false);
	/**
		bBackGround == trueならば、ウィンドゥにフォーカスが
		無くとも入力を行なう
	*/
private:
	CKeyInput		m_Key;
	CJoyStick		m_Joy;
};

class CKey2 : public CVirtualKey {
/**
  class CKey1,class CKey2,class CKey3,class CKey4 は、
  キーデバイス登録済み、仮想キー設定済みのクラスです。

	ＫｅｙＢｏａｒｄ　＋　ＪｏｙＳｔｉｃｋ判定
	こちらは、上下左右＋６ボタンのゲーム用
	ジョイスティックは１が接続されていないときは自動的に２を使うようになります

０：ＥＳＣキー
１：テンキー８，↑キー，ジョイスティック↑
２：テンキー２，↓キー，ジョイスティック↓
３：テンキー４，←キー，ジョイスティック←
４：テンキー２，→キー，ジョイスティック→
５：スペースキー，Ｚキー，ジョイスティック　ボタン１
６：テンキーEnter,リターンキー，Ｘキー，ジョイスティック　ボタン２
７：Ｃキー，ジョイスティック　ボタン３
８：Ａキー，ジョイスティック　ボタン４
９：Ｓキー，ジョイスティック　ボタン５
１０：Ｄキー，ジョイスティック　ボタン６

*/
public:
	CKey2(bool bBackGround=false);
	/**
		bBackGround == trueならば、ウィンドゥにフォーカスが
		無くとも入力を行なう
	*/
private:
	CKeyInput		m_Key;
	CJoyStick		m_Joy;
};

//////////////////////////////////////////////////////

class CKey3 : public CVirtualKey {
/**
  class CKey1,class CKey2,class CKey3,class CKey4 は、
  キーデバイス登録済み、仮想キー設定済みのクラスです。

	ＫｅｙＢｏａｒｄのみ
	こちらは、上下左右＋２ボタンのゲーム用

  このクラスは、class CKey1 からジョイスティックの
  割り当てを除いたものです。
*/
public:
	CKey3(bool bBackGround=false);
	/**
		bBackGround == trueならば、ウィンドゥにフォーカスが
		無くとも入力を行なう
	*/
private:
	CKeyInput	m_Key;
};

class CKey4 : public CVirtualKey {
/**
  class CKey1,class CKey2,class CKey3,class CKey4 は、
  キーデバイス登録済み、仮想キー設定済みのクラスです。

	ＫｅｙＢｏａｒｄのみ
	こちらは、上下左右＋６ボタンのゲーム用

  このクラスは、class CKey2 からジョイスティックの
  割り当てを除いたものです。
*/
public:
	CKey4(bool bBackGround=false);
	/**
		bBackGround == trueならば、ウィンドゥにフォーカスが
		無くとも入力を行なう
	*/
private:
	CKeyInput	m_Key;
};

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd

#endif
