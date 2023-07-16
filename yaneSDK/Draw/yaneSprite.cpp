
#include "stdafx.h"
#include "yaneSprite.h"
#include "yaneSurface.h"
#include "../Auxiliary/yaneFile.h"
#include "../Auxiliary/yaneStringScanner.h" // CLineParser
#include "yanePlane.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

//////////////////////////////////////////////////////////////////////////////
//	CSprite

void	CSimpleSprite::set(ISurface*lp){
	//	�v���[���S�����̃X�v���C�g�Ƃ���
	WARNING(lp==NULL,"CSimpleSprite::SetSurface��lp==NULL");

	pSurface = lp;
	int	sx,sy;
	pSurface->GetSize(sx,sy);
	::SetRect(&rcRect,0,0,sx,sy);
	nOx = nOy = 0;
	nHeight	= sy;
	bFast	= true;
}

void	CSimpleSprite::set(ISurface*lp,const RECT &rc){
	//	�v���[���S�����̃X�v���C�g�Ƃ���
	WARNING(lp==NULL,"CSimpleSprite::SetSurface��lp==NULL");

	pSurface = lp;
	rcRect = rc;
	nOx = nOy = 0;
	nHeight	= rc.bottom-rc.top;
	bFast	= true;
}

//////////////////////////////////////////////////////////////////////////////
//	CSprite

CSprite::CSprite() {
	m_nAnimation	= 0;
	m_nDirection	= 0;
	m_nOx			= 0;
	m_nOy			= 0;
	m_nPriority		= 0;	//	���ꂪ�f�B�t�H���g�i�[���Ӗ��͖����j
	m_bVisible		= true;
	m_nHeight		= 0;	//	�[���Ӗ��͂Ȃ�
	m_vSprtieVector.Add(new spritevectorofvector);
}

CSprite::~CSprite(){
}

void	CSprite::setOffset(int nOffsetX,int nOffsetY){
	m_nOx = nOffsetX;
	m_nOy = nOffsetY;
}

void	CSprite::getOffset(int& nOffsetX,int& nOffsetY){
	nOffsetX = m_nOx;
	nOffsetY = m_nOy;
}

void	CSprite::incMotion(){
	m_nAnimation++;
	if (m_nAnimation >= (int)getDirectionalSprite()->size()) {
		m_nAnimation = 0;
	}
}

void	CSprite::setDirection(int nDirection){
	//	�����̕ύX�������ꍇ�̓A�j���J�E���^�����Z�b�g���Ȃ��i�d�l�j
	if (m_nDirection != nDirection) {
		m_nAnimation = 0;
	}
	m_nDirection = nDirection;
}

//////////////////////////////////////////////////////////////////////////////

void	CSpriteEx::Blt(ISurface*pSurface,LPRECT lpClip){
	Blt(pSurface,0,0,lpClip);
}

void	CSpriteEx::Blt(ISurface*pSurface,int x,int y,LPRECT lpClip){
	BltFix(pSurface,x,y,lpClip);
	incMotion();
}

void	CSpriteEx::BltFix(ISurface*pSurface,LPRECT lpClip){
	int x,y;
	getPos(x,y);
	BltFix(pSurface,x,y,lpClip);
}

void	CSpriteEx::BltFix(ISurface*pSurface,int x,int y,LPRECT lpClip){
	//	�L�����H
	if (!isEnable()) return ;

	//	CDirectDraw�ŕ`�悷��ꍇLayer����
	CSimpleSprite &s = *getSprite();

	int ox,oy;
	getOffset(ox,oy);
	ox+=x+s.nOx;
	oy+=y+s.nOy;

	//	���̂܂܈Ϗ����Ă܂��Ƃ������`
	if (!s.bFast) {
		pSurface->Blt(s.pSurface,ox,oy,NULL,&s.rcRect,lpClip);
	} else {
		pSurface->BltFast(s.pSurface,ox,oy,NULL,&s.rcRect,lpClip);
	}
}

void	CSpriteEx::BltOnce(ISurface*pSurface,int x,int y,LPRECT lpClip){
	BltFix(pSurface,x,y,lpClip);
	
	//	�P�c�ɂȂ��Ă�����A����ȏ�͉��Z���Ȃ�
	int n=getMotion();
	incMotion();
	if (getMotion()==0) {
		setMotion(n);
	}
}

//////////////////////////////////////////////////////////////////////////////
///	yaneSDK2nd����̂Ԃ��������B���܂肢�������ł͂Ȃ��̂Ń}�l���Ȃ��Ńl

void	CSpriteChara::Release()
{
	m_apSurface.clear();
	m_vSpriteVector.Delete();
}

LRESULT	CSpriteChara::Load(const string& strFileName)
{
	Release();
	m_vSpriteVector.Add(new CSprite::spritevectorofvector);
	CFile file;
	if (file.Read(strFileName)!=0) return 1;

	//	ReadLine()�ň�s����͂��ăX�v���C�g�ݒ���s�Ȃ�

	//	�X�v���C�g�X���C�X�̗p��
	CSimpleSprite ssprite[256];
	ZERO(ssprite);

	int		nLine = 0;
	CHAR	szBuf[256];
	while (true) {
		nLine++;

		CLineParser		lineParser;
		if( file.ReadLine( szBuf ) !=0 ) break;
		lineParser.SetLine( szBuf );					//	���C�� �p�[�T�[�ɕ�������Z�b�g����

		//////////////////////////////////////////////////
		// �e�R�}���h�ʂɎ��s������

		// �X�v���C�g��`���̐ݒ�
		if ( lineParser.IsMatch( "#SpriteMax" ) ){
			int		n;
			if ( lineParser.GetNum( n ) != 0 ) { Err.Out("CSpriteChara::Load��#SpriteMax�Ɏ��s"); return 2; }
//			m_lpSprite.resize(n);
//			m_nSpriteMax = n;
			m_vSpriteVector->resize(n);
			continue;
		}
		////	�ő��v�@ SpriteMax > SpriteF > Sprite
		//	�X�v���C�gx�̋�`�z�̐ݒ� (�����������F�͖���)
		if ( lineParser.IsMatch( "#SpriteF" ) ){
			WARNING(m_vSpriteVector->empty(),"CSpriteChara::Load��SpriteMax�Ȃ���#Sprite���L��");
			int		n,m;
			lineParser.GetNum(n);
			while (true){
				if (lineParser.GetNum(m)) break;
				ssprite[m].bFast=true;

				//	�����́A����
				ssprite[m].nHeight = 32;
					//	����͂���œ��ꂵ���ق����悳��...
					//	�����ύX���K�v�ȏꍇ�́A�e�L�����N���X�ŕύX���邱��

				(*m_vSpriteVector)[n].push_back(ssprite[m]);
			}
			continue;
		}
/*
	�ˁ@�ꊇ��`
	#SpritePlane 0,0,34					//	�X�v���C�g0�`34�́A�v���[��0����35��(�v���[��34)�Ƃ���B
	#SpritePlaneF 0,0,34					//	�X�v���C�g0�`34�́A�v���[��0����35��(�v���[��34)�Ƃ���B�����������F�͖���
*/

		//	��z�X�v���C�gx�̐ݒ�
		if ( lineParser.IsMatch( "#SpritePlaneF" ) ){
			WARNING(m_vSpriteVector->empty(),"CSpriteChara::Load��SpriteMax�Ȃ���#Sprite���L��");
			int		n,m,l;
			lineParser.GetNum(n);
			if (lineParser.GetNum(m)) break;
			if (lineParser.GetNum(l)) break;

			CSimpleSprite s = {0};
			for(int i=n;i<n+l;i++){
				ssprite[i].bFast=true;
				//	�����́A����
				ssprite[i].nHeight = 32;
				//	����͂���œ��ꂵ���ق����悳��...
				//	�����ύX���K�v�ȏꍇ�́A�e�L�����N���X�ŕύX���邱��
				s.pSurface = m_apSurface[m+i].get();
				{
					int sx,sy;
					s.pSurface->GetSize(sx,sy);
					::SetRect(&s.rcRect,0,0,sx,sy);
				}
//				m_lpSprite[i].SetSpriteAdd(&s);
				(*m_vSpriteVector)[n].push_back(s);
			}
			continue;
		}

		//	��z�X�v���C�gx�̐ݒ�
		if ( lineParser.IsMatch( "#SpritePlane" ) ){
			WARNING(m_vSpriteVector->empty(),"CSpriteChara::Load��SpriteMax�Ȃ���#Sprite���L��");
			int		n,m,l;
			lineParser.GetNum(n);
			if (lineParser.GetNum(m)) break;
			if (lineParser.GetNum(l)) break;

			CSimpleSprite s = {0};
			for(int i=n;i<n+l;i++){
				ssprite[i].bFast=false;
				//	�����́A����
				ssprite[i].nHeight = 32;
				//	����͂���œ��ꂵ���ق����悳��...
				//	�����ύX���K�v�ȏꍇ�́A�e�L�����N���X�ŕύX���邱��
				s.pSurface = m_apSurface[m+i].get();
				{
					int sx,sy;
					s.pSurface->GetSize(sx,sy);
					::SetRect(&s.rcRect,0,0,sx,sy);
				}
//				m_lpSprite[i].SetSpriteAdd(&s);
				(*m_vSpriteVector)[n].push_back(s);
			}
			continue;
		}


		//	�X�v���C�gx�̋�`�z�̐ݒ�
		if ( lineParser.IsMatch( "#Sprite" ) ){
			WARNING(m_vSpriteVector->empty(),"CSpriteChara::Load��SpriteMax�Ȃ���#Sprite���L��");
			int		n,m;
			lineParser.GetNum(n);
			while (true){
				if (lineParser.GetNum(m)) break;
				ssprite[m].bFast=false;

				//	�����́A����
				ssprite[m].nHeight = 32;
					//	����͂���œ��ꂵ���ق����悳��...
					//	�����ύX���K�v�ȏꍇ�́A�e�L�����N���X�ŕύX���邱��
				
//				m_lpSprite[n].SetSpriteAdd(&ssprite[m]);
				(*m_vSpriteVector)[n].push_back(ssprite[m]);
			}
			continue;
		}

		// �w�肵���v���[��p�ɃX�v���C�g�f�[�^��ǂݍ���
		if ( lineParser.IsMatch( "#Plane" ) ){
			int		n;
			lineParser.GetNum( n );
			//	�s�����́Anew����
			while ((int)m_apSurface.size()<=n){
				m_apSurface.insert(CPlane::GetFactory()->CreateInstance());
			}
			string szFile;
			lineParser.GetStr(szFile);
			m_apSurface[n]->Load(szFile);
			continue;
		}

		//	�w�肵���v���[��p�̔����F�����W�Ŏw��
		if ( lineParser.IsMatch( "#ColorKeyA" ) ){
			int		n,m;
			lineParser.GetNum( n );
			lineParser.GetNum( m );
			//	�s�����́Anew����
			//	(�������A������new�������̂ɑ΂���SetColorKey�͕s�������D�D)
			while ((int)m_apSurface.size()<=m){
				m_apSurface.insert(CPlane::GetFactory()->CreateInstance());
			}
			int x,y;
			lineParser.GetNum( x );
			lineParser.GetNum( y );
			for(int i=n;i<=m;i++) {
				m_apSurface[i]->SetColorKeyPos(x,y);
			}
			continue;
		}

		//	�w�肵���v���[��p�̔����F��RGB�Ŏw��
		if ( lineParser.IsMatch( "#ColorKeyB" ) ){
			int		n,m;
			lineParser.GetNum( n );
			lineParser.GetNum( m );
			//	�s�����́Anew����
			while ((int)m_apSurface.size()<=m){
				m_apSurface.insert(CPlane::GetFactory()->CreateInstance());
			}
			int r,g,b;
			lineParser.GetNum( r );
			lineParser.GetNum( g );
			lineParser.GetNum( b );
			for(int i=n;i<=m;i++) {
				m_apSurface[i]->SetColorKey(ISurface::makeRGB(r,g,b));
			}
			continue;
		}

		//	�w�肵���v���[��p�̔����F�����W�Ŏw��
		if ( lineParser.IsMatch( "#ColorKey" ) ){
			int		n;
			lineParser.GetNum( n );
			//	�s�����́Anew����
			while ((int)m_apSurface.size()<=n){
				m_apSurface.insert(CPlane::GetFactory()->CreateInstance());
			}
			int x,y;
			lineParser.GetNum( x );
			lineParser.GetNum( y );
			m_apSurface[n]->SetColorKeyPos(x,y);
			continue;
		}

		//	�ő��v�@ (RectOffset > RectA > Rect)
		//	��`a����b�܂łɁA(x,y)�̃I�t�Z�b�g��^����
		if ( lineParser.IsMatch( "#RectOffset" ) ){
			int		a,b;
			lineParser.GetNum( a );
			lineParser.GetNum( b );
			int		x,y;
			lineParser.GetNum(x);
			lineParser.GetNum(y);
			for(int i=a;i<=b;i++){
				ssprite[i].nOx = x;
				ssprite[i].nOy = y;
			}
			continue;
		}

		//	��`a����A�v���[��p�ɑ΂��āA(x,y)����W�~H�ŉ�x��A�cy�s�̌J��ւ�����`���s��
		if ( lineParser.IsMatch( "#RectA" ) ){
			int		n;
			lineParser.GetNum( n );
			int		nPlane;
			lineParser.GetNum( nPlane );

			int		x,y,w,h,xt,yt;
			lineParser.GetNum(x);
			lineParser.GetNum(y);
			lineParser.GetNum(w);
			lineParser.GetNum(h);
			lineParser.GetNum(xt);
			lineParser.GetNum(yt);
			for(int j=0;j<yt;j++){
				for(int i=0;i<xt;i++){
					::SetRect( &ssprite[n+i+j*xt].rcRect,x+w*i,y+h*j,x+w*(i+1),y+h*(j+1));
					ssprite[n+i+j*xt].pSurface = m_apSurface[nPlane].get();
				}
			}
			continue;
		}

		//	��`a�Ƃ��ăv���[��p��(x,y)����W�~H�̋�`�Ƃ���
		if ( lineParser.IsMatch( "#Rect" ) ){
			int		n;
			lineParser.GetNum( n );
			WARNING(n<0||n>255,"CSpriteChara::Load��n<0||n>255");
			int		nPlane;
			lineParser.GetNum( nPlane );
			int		x,y,sx,sy;
			lineParser.GetNum(x);
			lineParser.GetNum(y);
			lineParser.GetNum(sx);
			lineParser.GetNum(sy);
			::SetRect( &ssprite[n].rcRect,x,y,x+sx,y+sy);
			ssprite[n].pSurface = m_apSurface[nPlane].get();
			continue;
		}

		/*
		//	�X�v���C�gs1����s2�̓��C���[L�Ƃ��Ē�`�i�f�B�t�H���g��5�j
		if ( lineParser.IsMatch( "#Layer" ) ){
			int		a,b;
			lineParser.GetNum( a );
			lineParser.GetNum( b );
			int		nLayer;
			lineParser.GetNum(nLayer);
			for(int i=a;i<=b;i++){
				m_lpSprite[i].SetLayer(nLayer);
			}
			continue;
		}
		*/
		if (lineParser.IsMatch("#")){
			Err.Out("CSpriteChara::Load��%s��%d�s�ɕs���Ȗ���",strFileName,nLine);
			return 1;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////

/*
	�X�v���C�g��`�t�@�C���̃t�H�[�}�b�g
	�E�啶���������͋�ʂ��Ȃ�
	�E�P�s�͂Q�T�U�����܂�
	�E��`��`��256�܂�
	�E�X�v���C�g�Ƃ́A�����A�̓����iCSimpleSprite�j���`����
	�E�X�v���C�g��`�́A��`�����ԂɎw�肷�邱�Ƃɂ���čs�Ȃ�
	�E�e�s�A//�ȍ~�̓R�����g�Ƃł���
	�E�v���[���͖����ɗ��p�ł���
	�E��`�ƂƂ��ɁA���̈ʒu�i�x�[�X���C���j���w�肷�邱�Ƃ�����
	�E��`�ƂƂ��ɁA�I�t�Z�b�g���w�肷�邱�Ƃ�����
	�E�X�v���C�g�E��`�ԍ��͂O����n�܂�
	#SpriteMax	4			//	�X�v���C�g��`���͂S�B����͐擪�Ŏw�肷��
	#Plane 0,"test.bmp"		//	�v���[��0�Ƃ���test.bmp��ǂݍ���
	#ColorKey 0,4,10		//	0�Ԃ̃v���[���̔����F���ʒu�Ŏw�肵�A�����(4,10)�Ƃ���(�f�B�t�H���g��(0,0))
	#Plane 1,"test2.bmp"	//	�v���[��1�Ƃ���test2.bmp��ǂݍ���
	#Rect 0,1,48,48,64,128	//	��`0�Ƃ��ăv���[��1��(48,48)����W64�~H128�̋�`�Ƃ���
	#Rect 1,1,112,48,64,128	//	��`1�Ƃ��ăv���[��1��(112,48)����W64�~H128�̋�`�Ƃ���
	#Sprite 1,0,1,1,0		//	�X�v���C�g1�͋�`0����`1����`1����`0�Ƃ����z�Ƃ���
	#SpriteF 0,0,1,1,0		//	�X�v���C�g0�͋�`0����`1����`1����`0�Ƃ����z�Ƃ���B�����������F�͖���


	�ˁ@�ꊇ��`
	#SpritePlane 0,0,34					//	�X�v���C�g0�`34�́A�v���[��0����35��(�v���[��0����v���[��34)�Ƃ���B
	#SpritePlaneF 0,0,34				//	�X�v���C�g0�`34�́A�v���[��0����35��(�v���[��0����v���[��34)�Ƃ���B�����������F�͖���
	#ColorKeyA 0,34,4,10				//	0�`34�̃v���[���̃J���[�L�[�͈ʒu�Ŏw�肵�A�����(4,10)�Ƃ���(�f�B�t�H���g��(0,0))
	#ColorKeyB 0,107,0,255,0			
	//	0-107�Ԃ̃v���[���̔����F��RGB�Ŏw�肵�A�����(0,255,0)�Ƃ���

//	#SpriteA 0,0,34						//	�X�v���C�g0�`34�́A��`0����35�ڂ̋�`�܂łƂ���B�i�������j

	#RectA	12,0,10,15,32,64,4,3	//	��`12�`�́A�v���[��0�ɑ΂���AW32�~H64�̃T�C�Y�ł���A
									//	�����(10,15)�Ɏn�܂�A��4��A�c3�s��12�񕪌J��ւ���
									//	��`�����B�܂��`12�`23�܂ł���C�ɒ�`�����
	#RectOffset 10,16,5,6	//	��`10����16�܂łɁA(5,6)�̃I�t�Z�b�g��^����

//	#Layer 1,3,5			//	�X�v���C�g1����3��Layer5�Ƃ��Ē�`�i�f�B�t�H���g��5�j
//

*/

}} // end of namespace
