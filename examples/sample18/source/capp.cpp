#include "stdafx.h"

#include "capp.h"

void	CApp::MainThread() {

	GetDraw()->SetDisplay();

	CFPSTimer timer;
	timer.SetFPS(10);

	//	これをメインプリにする（終了するときに、他のウィンドゥをすべて閉じる）
	SetMainApp(true);

	//------- sound再生のサンプル -----------

	//	1.mp3を非ストリーム再生で再生するサンプル
	CSound mysound;					//	CSoundを用意(これの実体はsmart_ptr<ISound>
	mysound.Load("sound.mp3");		//	ファイル名を指定して読み込む
	mysound->Play();				//	再生するなりよ
	//	この場合、mp3があまりに長いとstatic bufferに入らないので、
	//	再生できない可能性がある。(6MぐらいのMP3ファイルはまず再生されない)
	mysound->Stop();
	//	↑再生を止めるにはStopを呼び出す

	//	2.ストリーム再生の場合
	soundFactory_.SetStreamPlay(true);
	mysound.Load("sound.mp3");		//	ファイル名を指定して読み込む
	mysound->Play();				//	再生するなりよ
	//	ストリーム再生なので再生できる(と思われる)
	mysound->Stop();
	//	↑再生を止めるにはStopを呼び出す

	//	3.SoundLoaderを用いて再生させる場合
	CSoundLoader loader;
	loader.SetStreamPlay(true);
	//	↑ストリーム再生かどうかを、CSoundFactoryを介さず強制的に指定することが出来る
	//	(このメンバを呼び出して設定した場合、CSoundFactoryで指定したものより優先される)

	loader.SetSoundFactory(smart_ptr<ISoundFactory>(&soundFactory_,false));
	//	↑SoundFactoryを設定しておく
	loader.Set("sound.cfg.txt",true);
	//	↑ファイルから設定ファイルを読み込ませる
	string s = loader.GetFileName(3);
	//	↑番号を指定してファイル名を取得できる機能
	//	CDbg().Out(s);
	CSound sound = loader.GetSound(7);
	//	↑GetSoundすれば、ISound派生クラスのインスタンスが生成され、
	//	ファイルからの読み込みも行なわれる
	sound->Play();
	//	Play
	sound->Stop();
	//	停止
	sound->Close();
	//	解放
	sound = loader.GetSound(3);
	//	次のSoundを取得する
	//	以下、こうやって、Soundをいろいろ再生でける。


	//	4.SELoaderを用いる場合
	{
	CSELoader loader;

	CDefaultCacheStaleListener* listener = new CDefaultCacheStaleListener;
	listener->SetMax(10); // 10個越えて読み込んだ場合、古いものから解放していく
	loader.SetCacheStaleListener(smart_ptr<ICacheStaleListener>(listener));

	loader.SetSoundFactory(smart_ptr<ISoundFactory>(&soundFactory_,false));
	loader.Set("se.cfg.txt",true);

	CSound sound = loader.GetSound(10);
	sound->Play();
	}

	//	5.PlaneLoaderを用いる場合
	CPlaneLoader planeloader;
	planeloader.Set("plane.cfg.txt",true);

	while (IsThreadValid()){
		ISurface* pSecondary = GetDraw()->GetSecondary();

		//	PlaneLoaderからプレーンをもらってそれを描画する場合
		CPlane plane = planeloader.GetPlane(5);
		pSecondary->BltFast(plane,0,0);

		GetDraw()->OnDraw();

		timer.WaitFrame();
	}
}

//	これがmain windowのためのクラス。
class CAppMainWindow : public CAppBase {	//	アプリケーションクラスから派生
	virtual void MainThread(){			   //  これがワーカースレッド
		CApp().Start();
	}
};

//	言わずと知れたWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	{
		/*
		{	//	エラーログをファイルに出力するのら！
			CTextOutputStreamFile* p = new CTextOutputStreamFile;
			p->SetFileName("Error.txt");
			Err.SelectDevice(smart_ptr<ITextOutputStream>(p));
		}
		*/

		CSingleApp sapp;
		//	↑これを先に書かないと、スレッドを起動して
		//	CAppInitializerのデストラクタで全スレッドの終了を待つような場合
		//	CSingleAppのデストラクタが先に起動してしまうので、まずいのだ．．

		CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
		//	↑必ず書いてね

		if (sapp.IsValid()) {
			CThreadManager::CreateThread(new CAppMainWindow);
			//	上で定義したメインのウィンドゥを作成
//			CThreadManager::CreateThread(new CAppMainWindow);
//			↑複数書くと、複数ウィンドゥが生成されるのだ

		}
		//	ここでCAppInitializerがスコープアウトするのだが、このときに
		//	すべてのスレッドの終了を待つことになる
	}
	return 0;
}
