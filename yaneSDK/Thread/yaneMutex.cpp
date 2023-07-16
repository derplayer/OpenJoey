//---------------------------------------------------------------------
//		  yaneMutex Class の実装
//---------------------------------------------------------------------
// あまり美しくないし、ひょっとするアプリ二重起動防止以外には使えないかも
#include "stdafx.h"
#include "yaneMutex.h"

namespace yaneuraoGameSDK3rd {
namespace Thread {

CMutex::CMutex(){
	m_hMutex = NULL;
}

CMutex::~CMutex(){
	Release();
}

LRESULT CMutex::Open(const string& strMutexName){
	// 同一プロセスのスレッド間でならCriticalSectionを用いるべき！
	Release();

	// この名前で登録
	m_hMutex = ::CreateMutex(NULL,true,strMutexName.c_str());
	if (GetLastError()==ERROR_ALREADY_EXISTS) return 1;
	//	⇒エラーではなく、既存であるという意味で1を返している
	//	この場合も、MutexのOpenには成功していると考えることが出来る。

	if (GetLastError()==ERROR_INVALID_HANDLE) return 2;
	//	openに失敗＾＾；

	// 既存のため所有権が獲得できていない
	return 0; // それ以外のエラーって想定できないし、いいんでない？
}

void CMutex::Wait(){
	if (m_hMutex!=NULL){
		::WaitForSingleObject(m_hMutex,INFINITE);
		// 所有権が得られるまで待つ
	}
}

void CMutex::Release(){
	if (m_hMutex!=NULL) {
		::ReleaseMutex(m_hMutex);	//	所有権の解放
		::CloseHandle(m_hMutex);	//	生成したMutexの破壊
		//	他のが生きていると、無事破壊できるとも限らないが．．？

/*
	MSDNライブラリ2001年4月リリースより：
ハンドルを閉じるには、CloseHandle 関数を使います。プロセスが終了する際に、
システムはそのプロセスが所有していたハンドルを自動的に閉じます。
ミューテックスオブジェクトに対して 1 つまたは複数のハンドルが開いている場合、
最後のハンドルが閉じた時点で、そのミューテックスオブジェクトは破棄されます。
*/
		//	「1 つまたは複数のハンドルが開いている」の「開いている」は、
		//	CreateMutexのことを意味するのだと思うが、
		//	つまり、解放に関しては、
		//	参照カウントのような処理をしていると考えることが出来るのだろう．．

		m_hMutex = NULL;
	}
}

LRESULT	CMutexSection::Enter(const string& strMutexName){
	//	MutexSectionに入る
	LRESULT lr = m_vMutex.Open(strMutexName);
	if (lr==0) return 0; //	獲得成功！
	if (lr==1) {
		m_vMutex.Wait(); //	所有権を獲得するまで待つ
		return 0;
	}
	if (lr==2) return 1; // 名前エラー
	return -1;
}

void	CMutexSection::Leave(){
	//	MutexSectionから出る
	m_vMutex.Release();
}

} // end of namespace Thread
} // end of namespace yaneuraoGameSDK3rd
