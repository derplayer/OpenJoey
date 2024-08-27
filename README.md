# OpenJoey - Open Source implementation of Yu-Gi-Oh! - Power of Chaos games
Nothing to see here (yet?). I only got around to patch up and run a yaneSDKv3 skeleton example.
Pull requests are always welcome.

## What is yaneSDK?
TL;DR: Imagine early 2000s free C++ Unity engine for japanese indie devs.

## What do i need to compile this project?
The "TEST" project file is for Code::Blocks. You will also need:
* Microsoft Visual C++ Toolkit 2003 (the SDKv3 has also support for VC++6/.NET/.NET2003 and BCC5.5/BCB5/6!, look in yaneSDK\ProjectFiles folder)
* Microsoft Platform SDK (older one that plays nice with Visual C++ Toolkit 2003)
* Microsoft® DirectX® 8.0 SDK
The sdk can also be compiled in lib mode.

## Untouched yaneSDK source code
The raw untouched source code is in misc folder. (v1-v5)
The engine is used by way more games (mostly older visual novels).

# Where was the yaneSDK used?
YaneSDKv2 is used in: Yu-Gi-Oh! Power of Chaos games.
YaneSDKv3 is used in: Yu-Gi-Oh! Online Duel Evolution.
It was also used by other japanese games in early 00's (way too many to count, most confirmed ones are visual novels) and is used even to this day for some VNs.

## Documentation
On the official website.
There is also a very cool [gamedev programming book series](https://www.amazon.co.jp/Windowsプロフェッショナルゲームプログラミング-やね-うらお/dp/479800314X) on it how to use the engine (but in japanese only).

## License
The original yaneSDK was released as public domain by Motohiro Isozaki aka [yaneurao](https://github.com/yaneurao).
This repo is more like an mirror with tweaks. You can also download the original source code on his website if you want.
[Official YaneSDK Homepage](https://bm98.yaneu.com/yaneSDK.html)

![Alt text](/misc/scr.jpg?raw=true "Skeleton screenshot")