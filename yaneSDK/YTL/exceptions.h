/*
	ありがちな例外を定義
*/

#ifndef __YTLExceptions_h__
#define __YTLExceptions_h__

#ifdef USE_EXCEPTION

namespace yaneuraoGameSDK3rd {
namespace YTL {

class CException {
/**
	すべての例外は、こいつから派生させる
*/
public:
	CException(){
#ifdef USE_STOP_EXCEPTION
	MessageBox(NULL,"例外が発生したので停止します！","例外発生",MB_OK);
	/**	このダイアログが出た直後に、VSから
		プロセスのアタッチをして、ビルド⇒ブレーク
		すればコールスタックを見てどこで発生した例外か特定できる
	*/

	///	ついでにメモリエラーをわざと発生させるコードを仕掛ける
	* LPLONG(0xcdcdcdcd) = 0;
	///	WinNT/2000系ならば、わざとこのメモリエラーを発生させて、
	///	デバッグしたほうが手っ取り早い
#endif
	}
	virtual string getError() const { return ""; };
};

class CIndexOutOfBoundsException : public CException {
///		配列範囲外へのメモリへのアクセス例外
///		class smart_ptr を使用しているときにチェック機構により発生
public:
	virtual string getError() const {
		return "配列範囲外へのメモリへのアクセス";
	}
};

class CNullPointerException : public CException {
///		Null pointerからのアクセス例外
///		class smart_ptr を使用しているときにチェック機構により発生
public:
	virtual string getError() const {
		return "Null pointerからのアクセス";
	}
};

class CRuntimeException : public CException {
///		範囲外の数字が指定されているときのアクセス例外
public:
	CRuntimeException(const string& strErrorName="") : m_str(strErrorName) {
		if (m_str.empty()){
			m_str = "実行時例外（範囲外の数値が指定されている)";
		}
	}
	virtual string getError() const {
		return m_str;
	}
protected:
	string m_str;
};

/////////////////////////////////////////////////////////////////
//	スレッド例外

class CInterruptedException : public CException {
///		wait/sleepで待機中のスレッドに対して
///		interruptを呼び出したときの例外（cf.Javaの例外）
public:
	virtual string getError() const {
		return "InterruptedException例外が発生";
	}
};

class CIllegalMonitorStateException : public CException {
///		ロックを持っていないスレッドがwait,notify,notifyAll
///		を呼び出したときの例外（cf.Javaの例外）
public:
	virtual string getError() const {
		return "CIllegalMonitorStateException例外が発生";
	}
};

/////////////////////////////////////////////////////////////////

class CSyntaxException : public CException {
///		一般的な文法エラーに対するアクセス例外
public:
	CSyntaxException(const string& strErrorName) : m_str(strErrorName) {}	
	virtual string getError() const {
		return m_str;
	}
protected:
	string m_str;
};

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif	//	USE_EXCEPTION

#endif
