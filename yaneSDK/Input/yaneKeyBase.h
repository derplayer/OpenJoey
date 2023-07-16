// yaneKeyBase.h
//	 This is a based wrapper of input.
//		programmed by yaneurao(M.Isozaki) '99/11/13

#ifndef __yaneKeyBase_h__
#define __yaneKeyBase_h__


namespace yaneuraoGameSDK3rd {
namespace Input {

class IKeyBase {
/**
	キー入力のための基底クラス。
	DirectInput,JoyStick,MIDI Input等の入力系に対して
	共通のインターフェースを与えるために使う。
*/
public:
	virtual LRESULT	Input() = 0;		// must be override...
	/// 状態取得（これを実行した瞬間、キーのリアルタイム情報を取得する）

	/**
		以下の３つの関数で、キー判定を行なう。
		GetKeyStateによって得られた情報を調べて返すだけなので、
		GetKeyStateを忘れないように注意。
	*/
	virtual bool	IsKeyPress(int key) const;
	/// あるキーが現在押されているか

	virtual bool	IsKeyPush(int key) const;
	/// あるキーが前回のGetKeyStateのときから押されたか）

	virtual const BYTE*	GetKeyData() const;
	/// 取得したキー情報(BYTE m_byKeyBuffer[256])へのポインタを得る)
	///	直接ポインタを得てぐりぐりする人のために＾＾；

	IKeyBase();
	virtual ~IKeyBase(){}
protected:
	virtual void	Reset();
	/**
		バッファの中身をクリアする。JoyStickのボタン数を変更したときなど
		押されたままの状態で残るのを防止する
	*/

	// bufferのflip! (C)yaneurao
//	void	FlipKeyBuffer(int& var) { var = 1 - var;}
	void	FlipKeyBuffer(int& var) { var ^= 1;} // 普通、こうか．．(;´Д`)

	int		m_nKeyBufNo;			//	裏と表をflipして、差分をとるのに利用
	BYTE	m_byKeyBuffer[2][256];	//	key buffer
};

} // end of namespace Input
} // end of namespace yaneuraoGameSDK3rd

#endif
