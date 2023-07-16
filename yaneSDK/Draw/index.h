
//	描画

#include "yaneDirtyRect.h"			//	for Dirty Rect Management
#include "yaneDIBitmap.h"			//	simple DIBSection wrapper
#include "yaneSurface.h"			//	for Surface Base
//	GTLはでかいのでincludeちましぇん

#include "yaneDirectDraw.h"			//	for DirectDraw(COM wrapper)
#include "yaneFastDraw.h"			//	for FastDraw(FastPlaneControler)
#include "yaneFastPlane.h"			//	for FastPlane(ISurface derived class)

#include "yanePlane.h"				//	for Surface
#include "yanePlaneLoader.h"		//	for SurfaceLoading

//	スプライト関連
#include "yaneSprite.h"				//	for Sprite
#include "yaneSurfaceCache.h"		//	for Sprite(Drawing Priority)

//	テキスト文字関連

#include "yaneFont.h"				//	for TextFont
//#include "yaneFontRepositry.h"		//	for Font fly-weight(font cache)
#include "yaneTextDIBitmap.h"		//	for TextDrawing to DIBitmap
#include "yaneTextFastPlane.h"		//	for TextDrawing to FastPlane

//	シーン関連

#include "yaneScene.h"				//	for	Scene Control
//	→これタスクシステム使って実装するほうがいいような気もするなぁ．．

