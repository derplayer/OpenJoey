
//	�`��

#include "yaneDirtyRect.h"			//	for Dirty Rect Management
#include "yaneDIBitmap.h"			//	simple DIBSection wrapper
#include "yaneSurface.h"			//	for Surface Base
//	GTL�͂ł����̂�include���܂�����

#include "yaneDirectDraw.h"			//	for DirectDraw(COM wrapper)
#include "yaneFastDraw.h"			//	for FastDraw(FastPlaneControler)
#include "yaneFastPlane.h"			//	for FastPlane(ISurface derived class)

#include "yanePlane.h"				//	for Surface
#include "yanePlaneLoader.h"		//	for SurfaceLoading

//	�X�v���C�g�֘A
#include "yaneSprite.h"				//	for Sprite
#include "yaneSurfaceCache.h"		//	for Sprite(Drawing Priority)

//	�e�L�X�g�����֘A

#include "yaneFont.h"				//	for TextFont
//#include "yaneFontRepositry.h"		//	for Font fly-weight(font cache)
#include "yaneTextDIBitmap.h"		//	for TextDrawing to DIBitmap
#include "yaneTextFastPlane.h"		//	for TextDrawing to FastPlane

//	�V�[���֘A

#include "yaneScene.h"				//	for	Scene Control
//	������^�X�N�V�X�e���g���Ď�������ق��������悤�ȋC������Ȃ��D�D

