
//
//	Audio Mixer :
//		いちから書き直したった！
//			programmed by yaneurao (M.Isozaki) '02/03/22
//

#include "stdafx.h"
#include "yaneAudioMixer.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

CAudioMixer::CAudioMixer()
{
	m_hMixer = NULL;				//	ミキサデバイスのハンドル
	m_MixerID = NULL;				//	有効なミキサデバイスの識別子
	m_dwVol = 0;					//	元のボリューム設定(Absで)
	m_lPan = 0;						//	元のパン設定

	Open();
}

CAudioMixer::~CAudioMixer(){
	Close();
}

//////////////////////////////////////////////////////////////////////////////

LRESULT		CAudioMixer::Open( )
{
	m_bSuccessInit = false;

	if ( Close()!=0 ) return 1;

	if ( ::mixerGetNumDevs() == 0 ) {
		Err.Out( "CAudioMixer::Open ミキサデバイス数が 0か、エラーが発生しました" );
		return 2;
	}

	// ミキサデバイスをオープンする
	MMRESULT	res;
	res = ::mixerOpen( &m_hMixer, m_MixerID, 0, 0, MIXER_OBJECTF_MIXER );
	if ( res != MMSYSERR_NOERROR ) {
		ErrAudioMixer( res, "ミキサデバイスをオープンできませんでした\n" );		
		return 3;
	}

	//	ミキサデバイスを取得する
	if ( GetMixerDev() != 0 ) return 4;

	//	元のボリュームを得る
	for ( int i = 0 ; i < 4 ; i++ ) {
		// 特定のミキサコントロールを得る
		if ( GetMixerLineControl( i ) != 0 ) continue;
		// 元のボリュ－ムを保存する
		int nSize = m_MCD_UnSigned.size();
		if (nSize!=0){
			GetAudioInfo()[i].m_dwOriginalVol.resize(nSize);
			for (int j=0;j<nSize;j++){
				GetAudioInfo()[i].m_dwOriginalVol[j] = m_MCD_UnSigned[j].dwValue;//	元のボリューム設定(チャンネル1)
			}
		} else {
			GetAudioInfo()[i].m_dwOriginalVol.clear();
		}
	}

	//	この時点で初期化成功
	m_bSuccessInit = true;
	return 0;
}


////////////////////////////////////
//	SetVolumeRel
//	ボリュームの設定(相対)
////////////////////////////////////
LRESULT		CAudioMixer::SetVolumeRel( eAudioMix eDevice, double dRelVolume)
{
	if (!IsSuccessInit()) return -1; // 初期化に失敗している

	//	元のボリュームに対する音量の設定
	vector<DWORD>	nowVol;

	GetVolumeOrg( eDevice, nowVol );	
	//	元のボリュームを取得する

	for(int i=0;i<(int)nowVol.size();i++){
		nowVol[i] = (DWORD)(nowVol[i] * dRelVolume);
	}

	// ボリュームの設定(委譲する)
	return SetVolumeAbs( eDevice, nowVol);
}


///////////////////////////////////////////////////////////////
//	SetVolumeAbs
//	ボリュームの設定(絶対)
///////////////////////////////////////////////////////////////
LRESULT		CAudioMixer::SetVolumeAbs( eAudioMix eDevice, const vector<DWORD>& adwVol)
{
	if (!IsSuccessInit()) return -1; // 初期化に失敗している

	// 現在のコントロールの取得
	if (GetMixerLineControl( eDevice )!=0) return -1;

	for ( DWORD i = 0 ; i < m_MixerLine[eDevice].cChannels ; i++ ){
		DWORD dwVolume;
		if (i < adwVol.size()) {
			dwVolume = adwVol[i];
			if ( dwVolume > 65535 ) dwVolume = 65535;
//			if ( dwVolume < 0 ) dwVolume = 0;	//	無符号なのでありえない
		} else {
			dwVolume = 0;
		}
		m_MCD_UnSigned[i].dwValue = dwVolume;
	}

	// ボリュームの更新
	SetMixerControlDetails( eDevice, false );

	return 0;
}


//////////////////////////////////////////
//	GetVolumeRel
//	ボリュームの取得(相対)
//////////////////////////////////////////
LRESULT		CAudioMixer::GetVolumeRel(eAudioMix eDevice,double &dRelVolume) const {
	if (!IsSuccessInit()) return -1; // 初期化に失敗している

	// 現在のボリューム値の取得
	vector<DWORD>	nowVol;

	if ( GetVolumeAbs( eDevice, nowVol) != 0 ) return 1;

	//	元のボリュームに対する音量の設定
	vector<DWORD>	orgVol;
	if (GetVolumeOrg( eDevice, orgVol )!=0 ) return 1;

	//	元のvol合計と、現在のvol合計の比率で計算する
	DWORD dwNow=0;
	{for(int i=0;i<(int)nowVol.size();i++){ dwNow += nowVol[i]; }}
	DWORD dwOrg=0;
	{for(int i=0;i<(int)orgVol.size();i++){ dwOrg += orgVol[i]; }}
	
	if (dwOrg!=0) {	//	この判定いるやろ..
		dRelVolume = (dwNow)/(double)(dwOrg);
	} else {
		if (dwNow==0){
			dRelVolume = 0;
		} else {
			dRelVolume = 0xffff;	// infinity..
		}
	}
	return 0;
}


//////////////////////////////////////////
//	GetVolumeAbs
//	ボリュームの取得(絶対)
//////////////////////////////////////////
LRESULT		CAudioMixer::GetVolumeAbs(eAudioMix eDevice,vector<DWORD> &dwVolume) const
{
	if (!IsSuccessInit()) return -1; // 初期化に失敗している

	if (const_cast<CAudioMixer*>(this)->GetMixerLineControl( eDevice )!=0) return -1;

	int nSize = m_MCD_UnSigned.size();
	if (nSize!=0) {
		dwVolume.resize(nSize);
		for(int i=0;i<nSize;i++){
			dwVolume[i] = m_MCD_UnSigned.get(i)->dwValue;		// 現在の音量を得る
		}
	} else {
		dwVolume.clear();
	}

	return 0;
}

////////////////////////////////////////
//	GetVolumeOrg
//	元のボリュームを取得する
////////////////////////////////////////
LRESULT	CAudioMixer::GetVolumeOrg( eAudioMix eDevice, vector<DWORD> &vol) const
{
	if (!IsSuccessInit()) return -1; // 初期化に失敗している

	vol = const_cast<CAudioMixer*>(this)->GetAudioInfo()[eDevice].m_dwOriginalVol;
	return 0;
}

////////////////////////////////////////
//	SetVolumeOrg
//	元のボリュームに設定する
////////////////////////////////////////
LRESULT	CAudioMixer::SetVolumeOrg(eAudioMix eDevice)
{
	if (!IsSuccessInit()) return -1; // 初期化に失敗している

	int nLineID = (int)eDevice;
	if ( m_hMixer == NULL ) return 1;
	if (GetMixerLineControl(nLineID)!=0) return 2;

	int nSize = m_MCD_UnSigned.size();
	for(int i=0;i<nSize;i++){
		if ((int)GetAudioInfo()[nLineID].m_dwOriginalVol.size()>i){
			m_MCD_UnSigned[i].dwValue = GetAudioInfo()[nLineID].m_dwOriginalVol[i];
		}
	}
	return SetMixerControlDetails(nLineID,false);
}

////////////////////////////////
//	Close
//	close audio mixer(and retrive volume)
////////////////////////////////
LRESULT		CAudioMixer::Close( ){

	LRESULT lr = 0;

	if ( m_hMixer != NULL ){
		for ( int i = 0 ; i < 4 ; i++ )
			SetVolumeOrg((eAudioMix)i);	//	元のボリューム値に戻す

		MMRESULT	res = ::mixerClose ( m_hMixer );								//	ミキサデバイスを閉じる
		if ( res != MMSYSERR_NOERROR ) {
			ErrAudioMixer( res, "ミキサデバイスを閉じられませんでした\n" );
			lr = 1;
		}
	}

	//	ミキサコントロールのプロパティを取得、設定できる構造体のメモリ解放
	m_MCD_UnSigned.Delete();
	return lr;
}

//////////////////////////////////////////////
//	GetMixerLineControl
//	オーディオラインのコントロール情報を得る
//////////////////////////////////////////////
LRESULT	CAudioMixer::GetMixerLineControl( int iLineID ){

	// ミキサデバイスのハンドルを予め得ているか否かの判定
	if ( m_hMixer == NULL ) {
		Err.Out( "ミキサデバイスのハンドルがありませんでした" );
		return 1;
	}

	// iLineIDをサポートしているか否かの判定
	if (! GetAudioInfo()[iLineID].m_bAudioComponent ) {
		Err.Out("%dのIDのオーディオ コンポーネントは、このサウンドボードにはサポートされていませんでした",iLineID);
		return 1;
	}

	return GetMixerLineControl(m_MixerLine[iLineID]);
}

LRESULT CAudioMixer::GetMixerLineControl(MIXERLINE& ml){

	if (ml.cControls<=0) return 1; // 無理ゆったらだめぽ

	// ミキサコントロールを仮に得る変数にメモリを確保する
	smart_ptr<MIXERCONTROL> pMc;
	pMc.AddArray(ml.cControls);

	// オーディオラインコントロール情報の初期化処理
	MIXERLINECONTROLS&	mlc =  m_MixerLineControl;
	mlc.cbStruct	= sizeof (MIXERLINECONTROLS);
	mlc.dwLineID	= ml.dwLineID;
	mlc.cControls	= ml.cControls;
	mlc.cbmxctrl	= sizeof (MIXERCONTROL);
	mlc.pamxctrl	= pMc.get();

	// オーディオラインコントロール情報を得る
	MMRESULT	res;
	res = ::mixerGetLineControls( (HMIXEROBJ)m_hMixer, &mlc, MIXER_GETLINECONTROLSF_ALL );
	if ( res != MMSYSERR_NOERROR) {
		ErrAudioMixer( res, "オーディオラインのコントロール情報を得られませんでした\n" );
		return 1;
	}

	bool	bResult = false;
	for (DWORD i = 0 ; i < mlc.cControls ; i++ ){
		if ( pMc[i].dwControlType == MIXERCONTROL_CONTROLTYPE_VOLUME ){

			//	ミキサコントロール状態情報構造体を得る
			//	ミキサコントロールのプロパティを取得、設定できる構造体のメモリ確保
			DWORD channels = ml.cChannels;
			if (channels == 0) {
//				Err.Out("%dのIDのオーディオ コンポーネントはチャンネル数が0でした",i);
				continue;
			}

/**
			m_MCD_UnSigned.AddArray(max(channels,2));
			//	channel数が1でも2として扱わないと、音量保存の部分がめんどう＾＾；
*/

			m_MCD_UnSigned.AddArray(channels);

			// ミキサコントロール状態情報構造体の初期化処理
			m_MCD.cbStruct		=	sizeof (MIXERCONTROLDETAILS);
			m_MCD.dwControlID	=	pMc[i].dwControlID;
			m_MCD.cChannels		=	channels;
			m_MCD.cMultipleItems=	pMc[i].cMultipleItems;
			m_MCD.cbDetails		=	sizeof (MIXERCONTROLDETAILS_UNSIGNED);
			m_MCD.paDetails		=	m_MCD_UnSigned.get();

			// ミキサコントロール状態情報構造体の取得
			res = ::mixerGetControlDetails( (HMIXEROBJ)m_hMixer, &m_MCD, MIXER_GETCONTROLDETAILSF_VALUE );
			if ( res != MMSYSERR_NOERROR ) {
//				ErrAudioMixer( res, "ミキサコントロール状態情報構造体の取得ができませんでした\n" );
				continue;
			}

			//	ようやくコピー
			m_MixerControl = pMc[i];
			bResult = true;
			break;
		}
	}

	// 失敗していたら
	if ( !bResult ) {
		Err.Out( "MixerControl構造体に関連付けられるオーディオラインコントロールがありませんでした" );
		return 1;
	}

	return 0;
} // GetMixerControlDetails


///////////////////////////////////////////////
//	SetMixerControlDetails
//	ミキサコントロール状態情報構造体を得る
///////////////////////////////////////////////
LRESULT	CAudioMixer::SetMixerControlDetails( int iLineID, bool bMono )
{
	// ミキサデバイスのハンドルを予め得ているか否か
	if ( m_hMixer == NULL ) {
		Err.Out( "ミキサデバイスのハンドルがありませんでした" );
		return 1;
	}
	// iLineIDをサポートしているか否かの判定
	if ( !GetAudioInfo()[iLineID].m_bAudioComponent) {
		Err.Out( iLineID + "のIDのオーディオ コンポーネントは、このサウンドボードにはサポートされていませんでした" );
		return 1;
	}

	if (m_MCD_UnSigned.size()==0) {
		//	ミキサー無いよ？(;´・ω・`)ｼｮﾎﾞｰﾝ
		return 1;
	}

	// ミキサコントロール状態情報構造体の初期化処理
	m_MCD.cbStruct		= sizeof (MIXERCONTROLDETAILS);
	m_MCD.dwControlID	= m_MixerControl.dwControlID;
	if ( bMono ) {
		m_MCD.cChannels		=	1;
	} else {
		m_MCD.cChannels		=	m_MixerLine[iLineID].cChannels;
	}
	m_MCD.cMultipleItems	=	m_MixerControl.cMultipleItems;
	m_MCD.cbDetails			=	sizeof (MIXERCONTROLDETAILS_UNSIGNED);
	m_MCD.paDetails			=	m_MCD_UnSigned.get();

	// ミキサコントロール状態情報構造体の取得
	MMRESULT	res;
	res = ::mixerSetControlDetails( (HMIXEROBJ)m_hMixer, &m_MCD, MIXER_GETCONTROLDETAILSF_VALUE );
	if ( res != MMSYSERR_NOERROR ){
		ErrAudioMixer( res, "ミキサコントロール状態情報構造体の取得ができませんでした\n" );
		return 1;
	}

	return 0;
} // SetMixerControlDetails


/////////////////////////////////////////// Private /////////////////////////////////////////
//////////////////////////////
//private for Open
//	GetMixerDev
//	ミキサデバイスを取得する
//////////////////////////////
LRESULT	CAudioMixer::GetMixerDev( )
{
	// ミキサデバイスの情報を得る
	MMRESULT	res;
	res = ::mixerGetDevCaps( (UINT)m_MixerID, &m_MixerCaps, sizeof (m_MixerCaps) );
	if ( res != MMSYSERR_NOERROR ) {
		ErrAudioMixer( res, "ミキサデバイス情報を得られませんでした\n" );
		return 1;
	}

	for (DWORD i = 0 ; i < m_MixerCaps.cDestinations ; i++ )
	{
		// ミキサデバイスのディスティネ―ション、ソース情報を得る
		MIXERLINE	ml = { sizeof(ml) };		//	オーディオライン状態とメトリックス情報が入る構造体
		// ミキサデバイスのディスティネ―ション情報を得る
		ml.dwDestination = i;

		res = ::mixerGetLineInfo( (HMIXEROBJ)m_hMixer, &ml, MIXER_GETLINEINFOF_DESTINATION );
		if ( res != MMSYSERR_NOERROR ) {
			ErrAudioMixer( res, "ミキサデバイスのディスティネ―ション情報を得られませんでした\n" );
			return 1;
		}
		//	Getしたディスティネーションが、オーディオ出力に対するコンポーネントか否かを
		//	判定する(ヘッドホンもこれに入る)
		if ( ml.dwComponentType == MIXERLINE_COMPONENTTYPE_DST_SPEAKERS ) {

			//	bug-fixed '00/09/13
			//	こいつは、ライン出力できるのかを調べて、ライン出力できるならば、これを取得する
			if (GetMixerLineControl(ml)!=0) continue;	//	こんな奴は資格なし！＾＾；

			m_MixerLine[0] = ml;
			GetAudioInfo()[0].m_bAudioComponent = true;
			///	これがマスターヴォリュームに違い無い

			DWORD	iDstConnect = ml.cConnections;
			// ミキサデバイスのソース情報を得る
			for (DWORD j = 0 ; j < iDstConnect ; j++ )
			{
				ml.cbStruct = sizeof (ml);
				ml.dwDestination = i;
				ml.dwSource = j;
				res = ::mixerGetLineInfo( (HMIXEROBJ)m_hMixer, &ml, MIXER_GETLINEINFOF_SOURCE );
				if ( res != MMSYSERR_NOERROR ) {
					ErrAudioMixer( res, "ミキサデバイスのソース情報を得られませんでした\n" );
					return 1;
				}
				int nLineNo=0;
				switch (ml.dwComponentType) {
				case MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT:
					nLineNo = 1; break;
				case MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER:
					nLineNo = 2; break;
				case MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC:
					nLineNo = 3; break;
				}

				//	最初に取得したところから上書きは出来ないようにする '00/09/13				//	⇒　本当は、
				//	複数のサウンドカードをサポートしなければならないのでは．．

				if (nLineNo!=0 && !GetAudioInfo()[nLineNo].m_bAudioComponent
					&& GetMixerLineControl(ml)==0) {
					m_MixerLine[nLineNo] = ml;
					GetAudioInfo()[nLineNo].m_bAudioComponent = true;
				}
			}
		}
	}
	return 0;
} // GetMixerDev

LRESULT	CAudioMixer::RestoreVolume(eAudioMix eDevice){
	vector<DWORD> adwVol;
	if (GetVolumeAbs(eDevice,adwVol)!=0){
		return 1;
	}
	GetAudioInfo()[eDevice].m_dwOriginalVol = adwVol;
	//	初期ヴォリューム量としてコピー！
	return 0;
}

/////////////////////////////////////
//	ErrAudioMixer
//	オーディオミキサのエラー処理
/////////////////////////////////////
void	CAudioMixer::ErrAudioMixer( MMRESULT AMErr, LPCSTR strBuf )
{
	switch ( AMErr )
	{
		case MMSYSERR_NOERROR:
			Err.Out( "CAudioMixer::あれ？NOERRORが返ってますよ" );
			break;
		case MMSYSERR_INVALHANDLE:
			Err.Out( "CAudioMixer::指定したデバイスハンドルが不正です" );
			break;
		case MIXERR_INVALCONTROL:
			Err.Out( "CAudioMixer::コントロールの参照が不正です" );
			break;
		case MMSYSERR_BADDEVICEID:
			Err.Out( "CAudioMixer::デバイス識別子が不正または有効範囲外でした" );
			break;
		case MMSYSERR_INVALFLAG:
			Err.Out( "CAudioMixer::フラグの指定が不正です" );
			break;
		case MMSYSERR_INVALPARAM:
			Err.Out( "CAudioMixer::アドレスが不正です" );
			break;
		case MMSYSERR_NODRIVER:
			Err.Out( "CAudioMixer::デバイスドライバが存在しませんでした" );
			break;
		case MMSYSERR_ALLOCATED:
			Err.Out( "CAudioMixer::指定したリソースは、最大許容数のクライアントにすでに割り当てられていました" );
			break;
		case MMSYSERR_NOMEM:
			Err.Out( "CAudioMixer::必要なメモリを割り当てることができませんでした" );
			break;

		default:
			Err.Out( "CAudioMixer::原因不明のエラーが起こりました" );
			break;
	}
	Err.Out( strBuf );
} // ErrAudioMixer

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
