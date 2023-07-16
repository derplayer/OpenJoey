#include "stdafx.h"
#include "yaneSurfaceBltter.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

//---	inlineでほとんど書いちゃったから、こっちはもぬけの殻(;´Д`)

LRESULT CSurfaceBltter::Blt1Line(const void* pSrc,int nSrcSurfaceType,void *pDst,int nDstSurfaceType,int nLength)
	{
		switch(nSrcSurfaceType){
		case 3:{
			switch(nDstSurfaceType){
			case 5:
				Blt1Line((CFastPlaneRGB565*)pSrc,(CFastPlaneRGB888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 12:
				Blt1Line((CFastPlaneRGB565*)pSrc,(CFastPlaneARGB8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			default:
				return 1; // not supported
			}
			break;
				} // end case 3
		case 4:{
			switch(nDstSurfaceType){
			case 3:
				Blt1Line((CFastPlaneRGB555*)pSrc,(CFastPlaneRGB565*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 4:
				Blt1Line((CFastPlaneRGB555*)pSrc,(CFastPlaneRGB555*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 5:
				Blt1Line((CFastPlaneRGB555*)pSrc,(CFastPlaneRGB888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 6:
				Blt1Line((CFastPlaneRGB555*)pSrc,(CFastPlaneBGR888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 7:
				Blt1Line((CFastPlaneRGB555*)pSrc,(CFastPlaneXRGB8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 8:
				Blt1Line((CFastPlaneRGB555*)pSrc,(CFastPlaneXBGR8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 10:
				Blt1Line((CFastPlaneRGB555*)pSrc,(CFastPlaneARGB4565*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 11:
				Blt1Line((CFastPlaneRGB555*)pSrc,(CFastPlaneARGB4555*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 12:
				Blt1Line((CFastPlaneRGB555*)pSrc,(CFastPlaneARGB8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 13:
				Blt1Line((CFastPlaneRGB555*)pSrc,(CFastPlaneABGR8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			default:
				return 1; // not supported
			}
			break;
				} // end case 4
		case 5:{
			switch(nDstSurfaceType){
			case 3:
				Blt1Line((CFastPlaneRGB888*)pSrc,(CFastPlaneRGB565*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 4:
				Blt1Line((CFastPlaneRGB888*)pSrc,(CFastPlaneRGB555*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 5:
				Blt1Line((CFastPlaneRGB888*)pSrc,(CFastPlaneRGB888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 6:
				Blt1Line((CFastPlaneRGB888*)pSrc,(CFastPlaneBGR888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 7:
				Blt1Line((CFastPlaneRGB888*)pSrc,(CFastPlaneXRGB8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 8:
				Blt1Line((CFastPlaneRGB888*)pSrc,(CFastPlaneXBGR8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 10:
				Blt1Line((CFastPlaneRGB888*)pSrc,(CFastPlaneARGB4565*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 11:
				Blt1Line((CFastPlaneRGB888*)pSrc,(CFastPlaneARGB4555*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 12:
				Blt1Line((CFastPlaneRGB888*)pSrc,(CFastPlaneARGB8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 13:
				Blt1Line((CFastPlaneRGB888*)pSrc,(CFastPlaneABGR8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			default:
				return 1; // not supported
			}
			break;
				} // end case 5
		case 6:{
			switch(nDstSurfaceType){
			case 5:
				Blt1Line((CFastPlaneBGR888*)pSrc,(CFastPlaneRGB888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 12:
				Blt1Line((CFastPlaneBGR888*)pSrc,(CFastPlaneARGB8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			default:
				return 1; // not supported
			}
			break;
				} // end case 6
		case 7:{
			switch(nDstSurfaceType){
			case 5:
				Blt1Line((CFastPlaneXRGB8888*)pSrc,(CFastPlaneRGB888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 12:
				Blt1Line((CFastPlaneXRGB8888*)pSrc,(CFastPlaneARGB8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			default:
				return 1; // not supported
			}
			break;
				} // end case 7
		case 8:{
			switch(nDstSurfaceType){
			case 5:
				Blt1Line((CFastPlaneXBGR8888*)pSrc,(CFastPlaneRGB888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 12:
				Blt1Line((CFastPlaneXBGR8888*)pSrc,(CFastPlaneARGB8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			default:
				return 1; // not supported
			}
			break;
				} // end case 8
		case 10:{
			switch(nDstSurfaceType){
			case 5:
				Blt1Line((CFastPlaneARGB4565*)pSrc,(CFastPlaneRGB888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 12:
				Blt1Line((CFastPlaneARGB4565*)pSrc,(CFastPlaneARGB8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			default:
				return 1; // not supported
			}
			break;
				} // end case 10
		case 11:{
			switch(nDstSurfaceType){
			case 5:
				Blt1Line((CFastPlaneARGB4555*)pSrc,(CFastPlaneRGB888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 12:
				Blt1Line((CFastPlaneARGB4555*)pSrc,(CFastPlaneARGB8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			default:
				return 1; // not supported
			}
			break;
				} // end case 11
		case 12:{
			switch(nDstSurfaceType){
			//	αサーフェースから、非αサーフェースへの転送はillegalな気が..
			case 3:
				Blt1Line((CFastPlaneARGB8888*)pSrc,(CFastPlaneRGB565*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 4:
				Blt1Line((CFastPlaneARGB8888*)pSrc,(CFastPlaneRGB555*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 5:
				Blt1Line((CFastPlaneARGB8888*)pSrc,(CFastPlaneRGB888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 6:
				Blt1Line((CFastPlaneARGB8888*)pSrc,(CFastPlaneBGR888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 7:
				Blt1Line((CFastPlaneARGB8888*)pSrc,(CFastPlaneXRGB8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 8:
				Blt1Line((CFastPlaneARGB8888*)pSrc,(CFastPlaneXBGR8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 10:
				Blt1Line((CFastPlaneARGB8888*)pSrc,(CFastPlaneARGB4565*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 11:
				Blt1Line((CFastPlaneARGB8888*)pSrc,(CFastPlaneARGB4555*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 12:
				Blt1Line((CFastPlaneARGB8888*)pSrc,(CFastPlaneARGB8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			case 13:
				Blt1Line((CFastPlaneARGB8888*)pSrc,(CFastPlaneABGR8888*)pDst,
					nLength,CFastPlaneCopySrc()); break;
			default:
				return 1; // not supported
			}
			break;
				} // end case 12
		default:
			return 1; // not supported
		} // end switch

		return 0;
	}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
