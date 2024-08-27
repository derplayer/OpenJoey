
#include "stdafx.h"
#include "CScnEffect.h"
#include "CEffect.h"	//	Kaine

//	明度を下げるためのEffector
class CEffect1a : public CScenarioEffect {
public:
	virtual void	OnDraw(CPlaneBase* lp, int nPhase){
		//	fadeするかー
		CDIB32* pvDib = (CDIB32*)lp;
		int r = nPhase; if (r<0) r=0; if (r>255) r=255;
		pvDib->SubColorFast(DIB32RGB(r,r,r));
	}
};

//	明度を下げるためのEffector
class CEffect1b : public CScenarioEffect {
public:
	virtual void	OnDraw(CPlaneBase* lp, int nPhase){
		//	fadeするかー
		CDIB32* pvDib = (CDIB32*)lp;
		int r = nPhase; if (r<0) r=0; if (r>255) r=255;
		pvDib->AddColorFast(DIB32RGB(r,r,r));
	}
};

//	雪を降らせるEffector《Kaine》
class CEffect2 : public CScenarioEffect {
public:
	CEffect2() {
		dibsnow.Load("interface/effect/snow.bmp");
		//	初期値はわからん＾＾；
		nObject = 1;
		CSnowObject::AjustNumObject(m_SnowObjectList,&dibsnow,nObject);
		// 空回しすることで雪をちらばせる
		for ( int i = 0 ; i < 30 * 5 ; i++ ){
			for(set<CSnowObject*>::iterator it = m_SnowObjectList.begin();
							it!=m_SnowObjectList.end();it++){
				if ((*it)->IsValid()) (*it)->OnMove();
			}
		}
	}
private:
	virtual void OnDraw(CPlaneBase*lp,int nPhase){
//		lp->ClearRect();
//		lp->BltFast(&dib,0,0);

		// 動的に変更する例
		int nObject2 = nPhase;
		if ( nObject != nObject2 ){
			CSnowObject::AjustNumObject(m_SnowObjectList,&dibsnow,nObject2);
			nObject = nObject2;
		}

		// Draw
		for(set<CSnowObject*>::iterator it = m_SnowObjectList.begin();
						it!=m_SnowObjectList.end();it++){
			if ((*it)->IsValid()) (*it)->OnDraw(lp);
		}

	}
	int		nObject;

	class CSnowObject {
	public:
		CSnowObject(CPlaneBase*dib){
			SetObjectPlane(dib);
			m_bValid = true;
			Init();
			//m_objectinfo.y = rand() % 480; // 最初だけ例外 height
		}
		~CSnowObject(){
		}
		static void AjustNumObject(set<CSnowObject*>&list,CPlaneBase*dib,int num){
			int nObject = list.size();
			if ( num < 0 ) return;
			if ( nObject == num ) return;
			else if ( nObject - num > 0 ){ // delete
				int i = nObject - num;
				for(  set<CSnowObject*>::iterator it = list.begin();
							it!=list.end() , i > 0 ; i--){
					CSnowObject* lp = *it;
					it++;
					list.erase(lp);
					DELETE_SAFE(lp);
				}
				return;
			}
			else if ( nObject - num < 0 ){ // add
				for ( int i = num - nObject ; i > 0 ; i-- ){
					list.insert(new CSnowObject(dib));
				}
				return;
			}
		}
		LRESULT Init(void){
			m_objectinfo.xoffset = rand() % 640; // width
			m_objectinfo.y = - 20 + rand() % 10 ;
			m_objectinfo.dy = 4 + rand() % 4;
			m_objectinfo.count = rand() % 512;
//			m_objectinfo.amp = 15 + rand() % 2;
			m_objectinfo.amp = 14;
			// 遅いものほど小さくする
			switch ( m_objectinfo.dy ){
			case 7:
			case 6:
				m_objectinfo.z = 1;
				break;
			case 5:
			case 4:
				m_objectinfo.z = 2;
				break;
			case 3:
				m_objectinfo.z = 3;
				break;
			case 2:
			case 1:
				m_objectinfo.z = 4;
				break;
			default:
				m_objectinfo.z = 1;
			}
//			m_objectinfo.z = 1 + rand() % 4;
//			timer.Reset();
			return 0;
		};
		void SetObjectPlane(CPlaneBase*dib){m_dib = dib;}
		void OnDraw(CPlaneBase*lp){
			int sx,sy;
			m_dib->GetSize(sx,sy);
			SIZE s = { sx * ( 1 - m_objectinfo.z * 0.2 ) , sy * ( 1 - m_objectinfo.z * 0.2) };
			lp->Blt(m_dib,m_objectinfo.x,m_objectinfo.y,NULL,&s);
			OnMove();
		};
		void OnMove(void){
			if ( m_objectinfo.y > 500 ) Init();
			// 1FPS分の時間をかけてやってフレームスキップを検出したいが
			// １７フレーム＊約30で画面外にでて新しく計算し直してしまうため
			// 画面の上に雨がたまるのでやめ
			//int y = ( timer.Get() + 31 )  >> 5 ; // 32 本当は33で割る
			int y = 1;
			m_objectinfo.y += m_objectinfo.dy * y;
			m_objectinfo.x = m_objectinfo.xoffset + (m_sin.Sin(m_objectinfo.count)>>m_objectinfo.amp);
			m_objectinfo.count += 16;
//			timer.Reset();
		};
		bool IsValid(void){ return m_bValid;}
//		LRESULT ObjectLoad(void){};
	private:
		typedef struct {
			LONG	x,y,z;
			LONG	xoffset;
			int		dx,dy;
			long	count;
			int		amp;
		} OBJECTINFO;
		OBJECTINFO	m_objectinfo;
//		CTimer		timer;
		bool		m_bValid;
		CPlaneBase*		m_dib;
		CSinTable	m_sin;
	};
	set<CSnowObject*> m_SnowObjectList;
	CDIB32 dibsnow;
};

//	雨を降らせるためのEffector《Kaine》
class CEffect3 : public CScenarioEffect {
public:
	CEffect3() {
		dibrain.Load("interface/Effect/rain.bmp");
		nObject = 1;
		CRainObject::AjustNumObject(m_RainObjectList,&dibrain,nObject);
	}
private:
	virtual void OnDraw(CPlaneBase*lp,int nPhase){
//		lp->ClearRect();
//		lp->BltFast(&dib,0,0);

		// 動的に変更する例
		int nObject2 = nPhase;
		if ( nObject != nObject2 ){
			CRainObject::AjustNumObject(m_RainObjectList,&dibrain,nObject2);
			nObject = nObject2;
		}
//		nRain2--;

		// Draw
		for(set<CRainObject*>::iterator it = m_RainObjectList.begin();
						it!=m_RainObjectList.end();it++){
			if ((*it)->IsValid()) (*it)->OnDraw(lp);
		}

	}

	int		nObject;
	class CRainObject;
	set<CRainObject*> m_RainObjectList;
	CDIB32 dibrain;

	class CRainObject {
	public:
		CRainObject(CPlaneBase*dib){
			m_nSpeed = 15;
			m_nDSpeed = 30;
			SetObjectPlane(dib);
			m_bValid = true;
			Init();
			m_objectinfo.y = rand() % 480; // 最初だけ例外 height
		}
		~CRainObject(){
		}
		void SetSpeed(int nMinSpeed,int nDx){ m_nSpeed = nMinSpeed; m_nDSpeed=nDx;}
		static void AjustNumObject(set<CRainObject*>&list,CPlaneBase*dib,int num){
			int nObject = list.size();
			if ( num < 0 ) return;
			if ( nObject == num ) return;
			else if ( nObject - num > 0 ){ // delete
				int i = nObject - num;
				for(  set<CRainObject*>::iterator it = list.begin();
							it!=list.end() , i > 0 ; i--){
					CRainObject* lp = *it;
					it++;
					list.erase(lp);
					DELETE_SAFE(lp);
				}
				return;
			}
			else if ( nObject - num < 0 ){
				for ( int i = num - nObject ; i > 0 ; i-- ){
					list.insert(new CRainObject(dib));
				}
				return;
			}
		}
		LRESULT Init(void){
			m_objectinfo.dy = m_nSpeed + rand() % m_nDSpeed;
			m_objectinfo.x = rand() % 640; // width
			m_objectinfo.y = - 100 + rand() % 20 ;
//			m_objectinfo.dy = 30 + rand() % 10;
//			timer.Reset();
			return 0;
		};
		void SetObjectPlane(CPlaneBase*dib){m_dib = dib;}
		void OnDraw(CPlaneBase*lp){
		//	lp->Blt(m_dib,m_objectinfo.x,m_objectinfo.y);
			DWORD src,dst;
			int d = m_nSpeed+m_nDSpeed;
			src = ((255-d*4)+m_objectinfo.dy*2 );
			dst = 255 - src;
			lp->BlendBlt(m_dib,m_objectinfo.x,m_objectinfo.y,
				DIB32RGB(dst,dst,dst),
				DIB32RGB(src,src,src));
			OnMove();
		};
		void OnMove(void){
			if ( m_objectinfo.y > 500 ) Init();
			// 1FPS分の時間をかけてやってフレームスキップを検出したいが
			// １７フレーム＊約30で画面外にでて新しく計算し直してしまうため
			// 画面の上に雨がたまるのでやめ
			//int y = ( timer.Get() + 31 )  >> 5 ; // 32 本当は33で割る
			int y = 1;
			m_objectinfo.y += m_objectinfo.dy * y;
//			timer.Reset();
		}
		bool IsValid(void){ return m_bValid;}
		LRESULT ObjectLoad(void){};
/*		static DrawAll(CPlaneBase*lp)
		{
			for(set<CRainObject*>::iterator it = m_RainObjectList.begin();
							it!=m_RainObjectList.end();it++){
				if ((*it)->m_bValid) (*it)->OnDraw(lp);
			}
		}
		*/
	protected:
//		void insert(CRainObject*lp){ m_RainObjectList.insert(lp);}
//		void erase(CRainObject*lp){ m_RainObjectList.erase(lp);}
	private:
		typedef struct {
			LONG	x,y;
			int		dy;
		} OBJECTINFO;
		OBJECTINFO	m_objectinfo;
//		CTimer		timer;
		bool		m_bValid;
		CPlaneBase*		m_dib;

	private:
		int		m_nSpeed;
		int		m_nDSpeed;
	};
};

//	異次元のエフェクト
class CEffect4 : public CScenarioEffect {
public:
	CEffect4() {
		diblight.Load("interface/effect/light.bmp");
		dib.CreateSurface(640,480);
//		r.Set(45,210,2);
//		r.Reset();
//		r.SetReverse(false);
	}
private:
	virtual void OnDraw(CPlaneBase*lp,int nPhase){
		int n = 255 - nPhase;
		dib.BltFast((CDIB32*)lp,0,0);
		lp->ClearRect();
		dib.BltToAlpha(&diblight,n,n+128,255,0);	//	light効果
		lp->BlendBltFastAlpha(&dib,0,0);
	}
//	CTextLayer text;
	CDIB32 dib;
	CDIB32 diblight;
//	CRootCounter r;
};

class CEffect5 : public CScenarioEffect {
public:
	CEffect5() {
//		dib.Load("data/29.jpg");
		CDIB32 tmp;
		tmp.Load("interface/effect/light2.bmp");
		diblight.Load("interface/effect/light2.bmp");
		diblight.AddColorBlt(&tmp,0,0);
//		diblight.AddColorBlt(&tmp,0,0);
//		diblight.AddColorBlt(&tmp,0,0);
		nObject = 1;
		CLightObject::AjustNumObject(m_LightObjectList,&diblight,nObject);
		// 空回しすることで雪をちらばせる
		for ( int i = 0 ; i < 30 * 5 ; i++ ){
			for(set<CLightObject*>::iterator it = m_LightObjectList.begin();
							it!=m_LightObjectList.end();it++){
				if ((*it)->IsValid()) (*it)->OnMove();
			}
		}
	}
private:
	virtual void OnDraw(CPlaneBase*lp,int nPhase){
//		lp->ClearRect();
//		lp->BltFast(&dib,0,0);

		// 動的に変更する例
		int nObject2 = nPhase;
		if ( nObject != nObject2 ){
			CLightObject::AjustNumObject(m_LightObjectList,&diblight,nObject2);
			nObject = nObject2;
		}

		// Draw
		for(set<CLightObject*>::iterator it = m_LightObjectList.begin();
						it!=m_LightObjectList.end();it++){
			if ((*it)->IsValid()) (*it)->OnDraw(lp);
		}

	}
//	CDIB32 dib;
//	CTextLayer text;
	int		nObject;

	class CLightObject {
	public:
		CLightObject(CDIB32*dib){
			SetObjectPlane(dib);
			m_bValid = true;
			m_rcount.Set(128,255,4);
			m_rcount.SetInit(rand() % 128 + 128);
			m_rcount.SetReverse(true);
			Init();
		}
		~CLightObject(){
		}
		static void AjustNumObject(set<CLightObject*>&list,CDIB32*dib,int num){
			int nObject = list.size();
			if ( num < 0 ) return;
			if ( nObject == num ) return;
			else if ( nObject - num > 0 ){ // delete
				int i = nObject - num;
				for(  set<CLightObject*>::iterator it = list.begin();
							it!=list.end() , i > 0 ; i--){
					CLightObject* lp = *it;
					it++;
					list.erase(lp);
					DELETE_SAFE(lp);
				}
				return;
			}
			else if ( nObject - num < 0 ){ // add
				for ( int i = num - nObject ; i > 0 ; i-- ){
					list.insert(new CLightObject(dib));
				}
				return;
			}
		}
		LRESULT Init(void){
			m_objectinfo.xoffset = rand() % 640; // width
			m_objectinfo.y = 500 - rand() % 10 ;
			m_objectinfo.dy = 2 + rand() % 4;
			m_objectinfo.count = rand() % 512;
//			m_objectinfo.amp = 15 + rand() % 2;
			m_objectinfo.amp = 14;
			// 遅いものほど小さくする
			switch ( m_objectinfo.dy ){
			case 7:	case 6:	m_objectinfo.z = 1;	break;
			case 5:	case 4:	m_objectinfo.z = 2;	break;
			case 3:			m_objectinfo.z = 3;	break;
			case 2:	case 1:	m_objectinfo.z = 4;	break;
			default:		m_objectinfo.z = 1;
			}
			m_rcount.Reset();
			return 0;
		};
		void SetObjectPlane(CDIB32*dib){m_dib = dib;}
		void OnDraw(CPlaneBase*lp){
			int sx,sy;
			m_dib->GetSize(sx,sy);
			SIZE s = { sx * ( 1 - m_objectinfo.z * 0.2 ) , sy * ( 1 - m_objectinfo.z * 0.2) };
			lp->BlendBlt(m_dib,m_objectinfo.x,m_objectinfo.y,
//				PlaneRGB(255,255,255),PlaneRGB(m_rcount,m_rcount,m_rcount), // 点灯
				PlaneRGB(255,255,255),PlaneRGB(255,255,255),				// ふつうに
				NULL,&s);
//			lp->Blt(m_dib,m_objectinfo.x,m_objectinfo.y,
//				NULL,&s);
			OnMove();
		};
		void OnMove(void){
			if ( m_objectinfo.y < -20 ) Init();
			// 1FPS分の時間をかけてやってフレームスキップを検出したいが
			// １７フレーム＊約30で画面外にでて新しく計算し直してしまうため
			// 画面の上に雨がたまるのでやめ
			//int y = ( timer.Get() + 31 )  >> 5 ; // 32 本当は33で割る
			int y = 1;
			m_objectinfo.y -= m_objectinfo.dy * y;
			m_objectinfo.x = m_objectinfo.xoffset + (m_sin.Sin(m_objectinfo.count)>>m_objectinfo.amp);
			m_objectinfo.count += 16;
			m_rcount.Inc();
		};
		bool IsValid(void){ return m_bValid;}
//		LRESULT ObjectLoad(void){};
	private:
		typedef struct {
			LONG	x,y,z;
			LONG	xoffset;
			int		dx,dy;
			long	count;
			int		amp;
		} OBJECTINFO;
		OBJECTINFO	m_objectinfo;
		bool		m_bValid;
		CDIB32*		m_dib;
		CSinTable	m_sin;
		CRootCounter	m_rcount;

	};
	set<CLightObject*> m_LightObjectList;
	CDIB32 diblight;
};

//	明度を下げるためのEffector
class CEffect6 : public CScenarioEffect {
public:
	virtual void	OnDraw(CPlaneBase* lp, int nPhase){
		CDIB32* pvDib = (CDIB32*)lp;
//		RECT rc;
//		::SetRect(&rc,0,0,639,479);
		eff.ShadeOff(pvDib,nPhase);
	}
protected:
	CEffect eff;
};

//	明度を下げるためのEffector
class CEffect7 : public CScenarioEffect {
public:
	CEffect7(){
		m_dib.CreateSurface(640,480);
	}

	virtual void	OnDraw(CPlaneBase* lp, int nPhase){
		CDIB32* pvDib = (CDIB32*)lp;
		int r = nPhase;
		if (r>255) r=255; if (r<0) r=0;
		pvDib->BlendBlt(&m_dib,0,0,PlaneRGB(r,r,r)^0xffffff,PlaneRGB(r,r,r));
	}

protected:
	virtual void	OnDrawBGSurface(CPlaneBase* lp){
		CDIB32* pvDib = (CDIB32*)lp;
		m_dib.BltFast(pvDib,0,0);
	}
	CDIB32	m_dib;
};

//////////////////////////////////////////////////////////////////////////////
//	Factory..

CScenarioEffect* CMyScenarioEffectFactory::CreateInstance(int nNo){
	switch (nNo){
	case 0: return new CEffect1a;	//	sub color
	case 1: return new CEffect1b;	//	add color
	case 2: return new CEffect2;	//	雪
	case 3: return new CEffect3;	//	雨
	case 4: return new CEffect4;	//	異次元
	case 5: return new CEffect5;	//	不思議な光
	case 6: return new CEffect6;	//	ぼかし
	case 7: return new CEffect7;	//	CrossFader
	}
	return NULL; // error..
}
