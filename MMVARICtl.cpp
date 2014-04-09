//Copyright+LGPL

//-----------------------------------------------------------------------------------------------------------------------------------------------
// Copyright 1999-2014 Makoto Mori, JE3HHT
//-----------------------------------------------------------------------------------------------------------------------------------------------
// This file is part of MMVARI.ocx.

// MMVARI.ocx is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// MMVARI.ocx is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License along with MMVARI.ocx. If not, see
// <http://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------------------------------------------------------------------------

// MMVARICtl.cpp : CMMVARICtrl ActiveX コントロール クラスのインプリメンテーション

#include "stdafx.h"
#include "MMVARI.h"
#include "MMVARICtl.h"
#include "MMVARIPpg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CMMVARICtrl, COleControl)

/////////////////////////////////////////////////////////////////////////////
#define	MFSKAFC_1stATACK	(8*11025/2048)		// 最初の8秒は範囲広い
#define	MFSKAFC_2ndATACK	(30*11025/2048)		// ３０秒経過後は範囲狭い
#define	MFSKAFC_MAX			(60*11025/2048)

/////////////////////////////////////////////////////////////////////////////
// メッセージ マップ

BEGIN_MESSAGE_MAP(CMMVARICtrl, COleControl)
	//{{AFX_MSG_MAP(CMMVARICtrl)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_MESSAGE(WM_WAVE, OnWaveMessage )
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// ディスパッチ マップ

BEGIN_DISPATCH_MAP(CMMVARICtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CMMVARICtrl)
	DISP_PROPERTY_NOTIFY(CMMVARICtrl, "wCollectType", m_wCollectType, OnWCollectTypeChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CMMVARICtrl, "wTxCarrier", m_wTxCarrier, OnWTxCarrierChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CMMVARICtrl, "dwATC", m_dwATC, OnDwATCChanged, VT_I4)
	DISP_PROPERTY_NOTIFY(CMMVARICtrl, "bActive", m_bActive, OnBActiveChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CMMVARICtrl, "bATC", m_bATC, OnBATCChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CMMVARICtrl, "bAddStartCR", m_bAddStartCR, OnBAddStartCRChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CMMVARICtrl, "bAddStopCR", m_bAddStopCR, OnBAddStopCRChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CMMVARICtrl, "bNET", m_bNET, OnBNETChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CMMVARICtrl, "statLostRX", m_statLostRX, OnStatLostRXChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CMMVARICtrl, "statLostTX", m_statLostTX, OnStatLostTXChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CMMVARICtrl, "bCOMFSK", m_bCOMFSK, OnBCOMFSKChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CMMVARICtrl, "bCOMFSKINV", m_bCOMFSKINV, OnBCOMFSKINVChanged, VT_BOOL)
	DISP_PROPERTY_EX(CMMVARICtrl, "bTX", GetBTX, SetBTX, VT_BOOL)
	DISP_PROPERTY_EX(CMMVARICtrl, "dblSampleFreq", GetDblSampleFreq, SetDblSampleFreq, VT_R8)
	DISP_PROPERTY_EX(CMMVARICtrl, "dwSampleBase", GetDwSampleBase, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CMMVARICtrl, "dblFFTSampleFreq", GetDblFFTSampleFreq, SetNotSupported, VT_R8)
	DISP_PROPERTY_EX(CMMVARICtrl, "dblTxOffset", GetDblTxOffset, SetDblTxOffset, VT_R8)
	DISP_PROPERTY_EX(CMMVARICtrl, "wModeMax", GetWModeMax, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "bReqRX", GetBReqRX, SetBReqRX, VT_BOOL)
	DISP_PROPERTY_EX(CMMVARICtrl, "nBPF", GetNBPF, SetNBPF, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "strSoundID", GetStrSoundID, SetStrSoundID, VT_BSTR)
	DISP_PROPERTY_EX(CMMVARICtrl, "wSoundCH", GetWSoundCH, SetWSoundCH, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "wSoundRxFIFO", GetWSoundRxFIFO, SetWSoundRxFIFO, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "wSoundTxFIFO", GetWSoundTxFIFO, SetWSoundTxFIFO, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "wModGain", GetWModGain, SetWModGain, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "dwHandle", GetDwHandle, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CMMVARICtrl, "strVersion", GetStrVersion, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CMMVARICtrl, "wAFCWidth", GetWAFCWidth, SetWAFCWidth, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "wAFCLevel", GetWAFCLevel, SetWAFCLevel, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "statSoundOpen", GetStatSoundOpen, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "statIsPlaying", GetStatIsPlaying, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "wChannels", GetWChannels, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "bLoopExternal", GetBLoopExternal, SetBLoopExternal, VT_BOOL)
	DISP_PROPERTY_EX(CMMVARICtrl, "wBufferCount", GetWBufferCount, SetWBufferCount, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "strPTTPort", GetStrPTTPort, SetStrPTTPort, VT_BSTR)
	DISP_PROPERTY_EX(CMMVARICtrl, "bPTTLock", GetBPTTLock, SetBPTTLock, VT_BOOL)
	DISP_PROPERTY_EX(CMMVARICtrl, "wFFTType", GetWFFTType, SetWFFTType, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "bNotch", GetBNotch, SetBNotch, VT_BOOL)
	DISP_PROPERTY_EX(CMMVARICtrl, "wNotchFreq", GetWNotchFreq, SetWNotchFreq, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "wCWSpeed", GetWCWSpeed, SetWCWSpeed, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "bTreatCenterMFSK", GetBTreatCenterMFSK, SetBTreatCenterMFSK, VT_BOOL)
	DISP_PROPERTY_EX(CMMVARICtrl, "bMetricSqMFSK", GetBMetricSqMFSK, SetBMetricSqMFSK, VT_BOOL)
	DISP_PROPERTY_EX(CMMVARICtrl, "wBufferMax", GetWBufferMax, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "wDiddleRTTY", GetWDiddleRTTY, SetWDiddleRTTY, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "bPlayBack", GetBPlayBack, SetBPlayBack, VT_BOOL)
	DISP_PROPERTY_EX(CMMVARICtrl, "wLang", GetWLang, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "wDefaultCharset", GetWDefaultCharset, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "dblTxShift", GetDblTxShift, SetDblTxShift, VT_R8)
	DISP_PROPERTY_EX(CMMVARICtrl, "wFFTWidth", GetWFFTWidth, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "wTxState", GetWTxState, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "wWaveMax", GetWWaveMax, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "statComOpen", GetStatComOpen, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "bHPF", GetBHPF, SetBHPF, VT_BOOL)
	DISP_PROPERTY_EX(CMMVARICtrl, "dblCustomSampleFreq", GetDblCustomSampleFreq, SetNotSupported, VT_R8)
	DISP_PROPERTY_EX(CMMVARICtrl, "wNumNotches", GetWNumNotches, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CMMVARICtrl, "bCustomFilter", GetBCustomFilter, SetNotSupported, VT_BOOL)
	DISP_PROPERTY_EX(CMMVARICtrl, "wNotchTaps", GetWNotchTaps, SetWNotchTaps, VT_I2)
	DISP_FUNCTION(CMMVARICtrl, "SendTone", SendTone, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMMVARICtrl, "PlayBack", PlayBack, VT_BOOL, VTS_I2)
	DISP_FUNCTION(CMMVARICtrl, "SetTX", SetTX, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CMMVARICtrl, "SendText", SendText, VT_I2, VTS_BSTR)
	DISP_FUNCTION(CMMVARICtrl, "SendCWID", SendCWID, VT_I2, VTS_BSTR)
	DISP_FUNCTION(CMMVARICtrl, "GetSendText", GetSendText, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CMMVARICtrl, "AdjustClock", AdjustClock, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CMMVARICtrl, "SetClockAdjust", SetClockAdjust, VT_EMPTY, VTS_I2 VTS_R8)
	DISP_FUNCTION(CMMVARICtrl, "GetVariType", GetVariType, VT_BSTR, VTS_I2)
	DISP_FUNCTION(CMMVARICtrl, "AttachLongArray", AttachLongArray, VT_EMPTY, VTS_PI4 VTS_PI4 VTS_I2)
	DISP_FUNCTION(CMMVARICtrl, "SendChar", SendChar, VT_I2, VTS_I2)
	DISP_FUNCTION(CMMVARICtrl, "CreateVaricodeList", CreateVaricodeList, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION(CMMVARICtrl, "CreateCustomFilter", CreateCustomFilter, VT_BOOL, VTS_I2 VTS_PR8)
	DISP_FUNCTION(CMMVARICtrl, "DeleteCustomFilter", DeleteCustomFilter, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMMVARICtrl, "FindNotchFreq", FindNotchFreq, VT_I2, VTS_I2)
	DISP_FUNCTION(CMMVARICtrl, "AddNotchFreq", AddNotchFreq, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CMMVARICtrl, "DeleteNotch", DeleteNotch, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CMMVARICtrl, "CalcCustomCharacteristic", CalcCustomCharacteristic, VT_BOOL, VTS_PR8 VTS_PR8 VTS_I2 VTS_I2)
	DISP_FUNCTION(CMMVARICtrl, "CreateCustomFilterByKW", CreateCustomFilterByKW, VT_BOOL, VTS_I2 VTS_I2 VTS_R8 VTS_R8 VTS_I2)
	DISP_FUNCTION(CMMVARICtrl, "CreateCustomFilterByIDFT", CreateCustomFilterByIDFT, VT_BOOL, VTS_I2 VTS_PR8 VTS_I2)
	DISP_FUNCTION(CMMVARICtrl, "ReadFFT", ReadFFT, VT_I2, VTS_PI4 VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "strModeList", GetStrModeList, SetNotSupported, VT_BSTR, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "strMode", GetStrMode, SetStrMode, VT_BSTR, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "dblSpeed", GetDblSpeed, SetDblSpeed, VT_R8, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "wRxCarrier", GetWRxCarrier, SetWRxCarrier, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "bRxEnabled", GetBRxEnabled, SetBRxEnabled, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "wSQLevel", GetWSQLevel, SetWSQLevel, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "bSQ", GetBSQ, SetNotSupported, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "wSN", GetWSN, SetNotSupported, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "wCharset", GetWCharset, SetWCharset, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "bAFC", GetBAFC, SetBAFC, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "bSync", GetBSync, SetNotSupported, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "dblFreqError", GetDblFreqError, SetNotSupported, VT_R8, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "dblBandWidth", GetDblBandWidth, SetNotSupported, VT_R8, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "wMetricMFSK", GetWMetricMFSK, SetNotSupported, VT_I2, VTS_I2 VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "bUOS", GetBUOS, SetBUOS, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "dblRxShift", GetDblRxShift, SetDblRxShift, VT_R8, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "wMode", GetWMode, SetWMode, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "bCodeMM", GetBCodeMM, SetNotSupported, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "bPTTLines", GetBPTTLines, SetBPTTLines, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "bOverLevel", GetBOverLevel, SetNotSupported, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "wNotches", GetWNotches, SetWNotches, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "wRxCenter", GetWRxCenter, SetNotSupported, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "bCodeBAUDOT", GetBCodeBAUDOT, SetNotSupported, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CMMVARICtrl, "bRTTYFFT", GetBRTTYFFT, SetBRTTYFFT, VT_I2, VTS_I2)
	DISP_STOCKPROP_HWND()
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CMMVARICtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// イベント マップ

BEGIN_EVENT_MAP(CMMVARICtrl, COleControl)
	//{{AFX_EVENT_MAP(CMMVARICtrl)
	EVENT_CUSTOM("OnDrawWave", FireOnDrawWave, VTS_I2  VTS_PI4  VTS_PI4)
	EVENT_CUSTOM("OnDrawFFT", FireOnDrawFFT, VTS_I2  VTS_PI4)
	EVENT_CUSTOM("OnGetTxChar", FireOnGetTxChar, VTS_PI2)
	EVENT_CUSTOM("OnPTT", FireOnPTT, VTS_I2)
	EVENT_CUSTOM("OnTxState", FireOnTxState, VTS_I2)
	EVENT_CUSTOM("OnPlayBack", FireOnPlayBack, VTS_I2)
	EVENT_CUSTOM("OnSpeed", FireOnSpeed, VTS_I2  VTS_R8)
	EVENT_CUSTOM("OnClockAdjust", FireOnClockAdjust, VTS_PI4)
	EVENT_CUSTOM("OnTiming", FireOnTiming, VTS_I2  VTS_I4  VTS_I2)
	EVENT_CUSTOM("OnTxCarrier", FireOnTxCarrier, VTS_I2)
	EVENT_CUSTOM("OnRxCarrier", FireOnRxCarrier, VTS_I2  VTS_I2)
	EVENT_CUSTOM("OnNET", FireOnNET, VTS_I2)
	EVENT_CUSTOM("OnMode", FireOnMode, VTS_I2  VTS_I2  VTS_BSTR)
	EVENT_CUSTOM("OnRxChar", FireOnRxChar, VTS_I2  VTS_BSTR  VTS_I2)
	EVENT_CUSTOM("OnError", FireOnError, VTS_I2)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// プロパティ ページ

// TODO: プロパティ ページを追加して、BEGIN 行の最後にあるカウントを増やしてください。
BEGIN_PROPPAGEIDS(CMMVARICtrl, 1)
	PROPPAGEID(CMMVARIPropPage::guid)
END_PROPPAGEIDS(CMMVARICtrl)


/////////////////////////////////////////////////////////////////////////////
// クラス ファクトリと guid を初期化します

IMPLEMENT_OLECREATE_EX(CMMVARICtrl, "MMVARI.MMVARICtrl.1",
	0x9c0d49dd, 0x5c05, 0x456d, 0x91, 0x6b, 0x98, 0xc4, 0xcf, 0x63, 0x17, 0x2f)


/////////////////////////////////////////////////////////////////////////////
// タイプ ライブラリ ID とバージョン

IMPLEMENT_OLETYPELIB(CMMVARICtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// インターフェイス ID

const IID BASED_CODE IID_DMMVARI =
		{ 0x652f4647, 0xf36a, 0x443c, { 0x8b, 0xa1, 0x86, 0xd0, 0x5e, 0x82, 0x25, 0x7 } };
const IID BASED_CODE IID_DMMVARIEvents =
		{ 0x77710981, 0x37e2, 0x42a5, { 0xb8, 0x9e, 0x37, 0x63, 0x8e, 0xe1, 0x7, 0x79 } };


/////////////////////////////////////////////////////////////////////////////
// コントロールの型情報の制御

static const DWORD BASED_CODE _dwMMVARIOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CMMVARICtrl, IDS_MMVARI, _dwMMVARIOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CMMVARICtrl::CMMVARICtrlFactory::UpdateRegistry -
// CMMVARICtrl のシステム レジストリのエントリを追加または削除します 

BOOL CMMVARICtrl::CMMVARICtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: コントロールがアパートメント モデルのスレッド処理の規則に従っていることを
	// 確認してください。詳細については MFC のテクニカル ノート 64 を参照してください。
	// アパートメント モデルのスレッド処理の規則に従わないコントロールの場合は、6 番目
	// のパラメータを以下のように変更してください。
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_MMVARI,
			IDB_MMVARI,
			afxRegApartmentThreading,
			_dwMMVARIOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CMMVARICtrl::CMMVARICtrl - コンストラクタ

CMMVARICtrl::CMMVARICtrl()
{
	InitializeIIDs(&IID_DMMVARI, &IID_DMMVARIEvents);

	m_SampleFreq = SAMPFREQ;
	m_TxOffset = 0;

	m_nErrorCode = 0;
	m_Dec2.m_pMain = this;
    m_PTTCOM = "NONE";
    m_PTTLock = TRUE;
	m_bPTTLines[pttlineRTS] = m_bPTTLines[pttlineDTR] = 1;
	m_bPTTLines[pttlineTXD] = 0;
    m_fPlayBack = FALSE;
	m_PlayBackSpeed = 5;
	m_MFSK_Center = FALSE;
	m_MFSK_SQ_Metric = FALSE;
	m_MaxCarrier = 2700;
    m_DecCutOff = 2700;
	m_TX = txRX;

	m_bCOMFSK = FALSE;
	m_bCOMFSKINV = FALSE;

	m_pCom = NULL;
    m_fReqRX = FALSE;
	m_nSoundTimerID = 0;
	m_LoopBack = loopINTERNAL;
	m_BufferSize = 2048;
	m_fHPF = FALSE;
    m_NotchFreq = 1950;
//    m_Notches.m_nBaseTaps = 128;
//    m_Notches.m_NotchWidth = 1;
	m_nSubWindow = 0;
	m_fSuspend = FALSE;
	m_fTone = FALSE;
	m_TxState = txstateRX;

	m_ModFSK.m_pMainCtrl = this;
	m_ModGain = int(MODGAIN);
	m_FFTVType = 2;

	m_bActive = FALSE;
	m_wCollectType = 0;
	m_wTxCarrier = 1750;
	m_bNET = TRUE;
	m_bAddStartCR = FALSE;
	m_bAddStopCR = FALSE;
	m_dwATC = 0;
	m_bATC = TRUE;
	m_statLostRX = m_statLostTX = 0;

	m_wRxCarrier = 1750;
	m_WaterSemaphore = 0;

    m_AFCWidth = 50;
    m_AFCLevel = 12;	// SN=12dB
	m_ATCSpeed = 0;
    m_ATCLimit = 25000;
	m_AFCKeyTimer = 0;

	m_FFTSmooth = 2;

	for( int i = 0; i < RXMAX; i++ ){
		m_RxSet[i].m_nIndex = i;
		m_RxSet[i].m_pFFT = &m_FFT;
	}
    m_RxSet[0].m_CarrierFreq = 1750;
    m_RxSet[1].m_CarrierFreq = 1600;
    m_RxSet[2].m_CarrierFreq = 1900;
    m_RxSet[3].m_CarrierFreq = 1450;
    m_RxSet[4].m_CarrierFreq = 2050;
    m_RxSet[5].m_CarrierFreq = 750;
    m_RxSet[6].m_CarrierFreq = 1000;
    m_RxSet[7].m_CarrierFreq = 1250;
    m_RxSet[8].m_CarrierFreq = 1500;

    m_nBPFType = 0;

	memset(m_fftbuf, 0, sizeof(m_fftbuf));
	memset(m_fftout, 0, sizeof(m_fftout));
	SampleFreq(m_SampleFreq);
}
/////////////////////////////////////////////////////////////////////////////
// CMMVARICtrl::~CMMVARICtrl - デストラクタ

CMMVARICtrl::~CMMVARICtrl()
{
	CloseAll();
}


/////////////////////////////////////////////////////////////////////////////
// CMMVARICtrl::OnDraw - 描画関数

void CMMVARICtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	pdc->FillSolidRect(&rcBounds, RGB(192,192,192));

	int x = rcBounds.left + (rcBounds.Width() - 16)/2;
	int y = rcBounds.top + (rcBounds.Height() - 15)/2;
	CBitmap bmp;
	bmp.LoadBitmap(IDB_MMVARI);
	CDC dc;
	dc.CreateCompatibleDC(pdc);
	CBitmap *pOldBmp = dc.SelectObject(&bmp);
	pdc->BitBlt(x, y, 16, 15, &dc, 0, 0, SRCCOPY);
	dc.SelectObject(pOldBmp);
	dc.DeleteDC();
	bmp.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// CMMVARICtrl::DoPropExchange - 永続性のサポート

void CMMVARICtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	PX_Double(pPX, _T("dblSampleFreq"), m_SampleFreq, 11025);
	PX_Double(pPX, _T("dblTxOffset"), m_TxOffset, 0);
	if( pPX->IsLoading() ){
		SampleFreq(m_SampleFreq);
	}
	PX_Short(pPX, _T("wCollectType"), m_wCollectType, 0);
	PX_Short(pPX, _T("wTxCarrier"), m_wTxCarrier, 1750);
	PX_Long(pPX, _T("dwATC"), m_dwATC, 0);
	PX_Bool(pPX, _T("bATC"), m_bATC, TRUE);
	PX_Bool(pPX, _T("bAddStartCR"), m_bAddStartCR, FALSE);
	PX_Bool(pPX, _T("bAddStopCR"), m_bAddStopCR, FALSE);
	PX_Bool(pPX, _T("bNET"), m_bNET, TRUE);
	PX_Bool(pPX, _T("bPlayBack"), m_fPlayBack, FALSE);
	PX_Bool(pPX, _T("bCOMFSK"), m_bCOMFSK, FALSE);
	PX_Bool(pPX, _T("bCOMFSKINV"), m_bCOMFSKINV, FALSE);

	CString cs;
	short	w;
	BOOL	f;
	//	DISP_PROPERTY_EX(CMMVARICtrl, "strSoundID", GetStrSoundID, SetStrSoundID, VT_BSTR)
	StrSoundID(cs);
	PX_String(pPX, _T("strSoundID"), cs, "-1");
	SetSoundCard(m_Wave.m_SoundStereo, cs);
	//DISP_PROPERTY_EX(CMMVARICtrl, "wNotchFreq", GetWNotchFreq, SetWNotchFreq, VT_I2)
	w = m_NotchFreq;
	PX_Short(pPX, _T("wNotchFreq"), w, 1950);
	m_NotchFreq = w;

	//DISP_PROPERTY_EX(CMMVARICtrl, "nBPF", GetNBPF, SetNBPF, VT_I2)
	w = m_nBPFType;
	PX_Short(pPX, _T("nBPF"), w, 0);
	m_nBPFType = w;
	//DISP_PROPERTY_EX(CMMVARICtrl, "wSoundCH", GetWSoundCH, SetWSoundCH, VT_I2)
	w = m_Wave.m_SoundStereo;
	PX_Short(pPX, _T("wSoundCH"), w, 0);
	m_Wave.m_SoundStereo = w;
	//DISP_PROPERTY_EX(CMMVARICtrl, "wSoundRxFIFO", GetWSoundRxFIFO, SetWSoundRxFIFO, VT_I2)
	w = m_Wave.m_InFifoSize;
	PX_Short(pPX, _T("wSoundRxFIFO"), w, 12);
	m_Wave.m_InFifoSize = w;
	//DISP_PROPERTY_EX(CMMVARICtrl, "wSoundTxFIFO", GetWSoundTxFIFO, SetWSoundTxFIFO, VT_I2)
	w = m_Wave.m_OutFifoSize;
	PX_Short(pPX, _T("wSoundTxFIFO"), w, 6);
	m_Wave.m_OutFifoSize = w;
	//DISP_PROPERTY_EX(CMMVARICtrl, "wModGain", GetWModGain, SetWModGain, VT_I2)
	w = m_ModGain;
	PX_Short(pPX, _T("wModGain"), w, 16384);
	m_ModGain = w;
	//DISP_PROPERTY_EX(CMMVARICtrl, "wAFCWidth", GetWAFCWidth, SetWAFCWidth, VT_I2)
	w = m_AFCWidth;
	PX_Short(pPX, _T("wAFCWidth"), w, 50);
	m_AFCWidth = w;
	//DISP_PROPERTY_EX(CMMVARICtrl, "wAFCLevel", GetWAFCLevel, SetWAFCLevel, VT_I2)
	w = m_AFCLevel;
	PX_Short(pPX, _T("wAFCLevel"), w, 12);
	m_AFCLevel = w;
	//DISP_PROPERTY_EX(CMMVARICtrl, "bLoopExternal", GetBLoopExternal, SetBLoopExternal, VT_BOOL)
	PX_Bool(pPX, _T("bLoopExternal"), m_LoopBack, FALSE);
	//DISP_PROPERTY_EX(CMMVARICtrl, "strPTTPort", GetStrPTTPort, SetStrPTTPort, VT_BSTR)
	PX_String(pPX, _T("strPTTPort"), m_PTTCOM, "NONE");
	//DISP_PROPERTY_EX(CMMVARICtrl, "bPTTLock", GetBPTTLock, SetBPTTLock, VT_BOOL)
	PX_Bool(pPX, _T("bPTTLock"), m_PTTLock, TRUE);
	//DISP_PROPERTY_EX(CMMVARICtrl, "wFFTType", GetWFFTType, SetWFFTType, VT_I2)
	w = m_FFT.m_FFTGain;
	PX_Short(pPX, _T("wFFTType"), w, 1);
	m_FFT.m_FFTGain = w;
	//DISP_PROPERTY_EX(CMMVARICtrl, "bNotch", GetBNotch, SetBNotch, VT_BOOL)
	f = m_Notches.m_Count;
	PX_Bool(pPX, _T("bNotch"), f, FALSE);
	if( f ){
		if( !m_Notches.m_Count ) m_Notches.Add(m_NotchFreq, TRUE);
	}
	else {
		if( m_Notches.m_Count ) m_Notches.Delete();
	}
	//DISP_PROPERTY_EX(CMMVARICtrl, "wNotchFreq", GetWNotchFreq, SetWNotchFreq, VT_I2)
	w = m_NotchFreq;
	PX_Short(pPX, _T("wNotchFreq"), w, 1950);
	m_NotchFreq = w;	
	//DISP_PROPERTY_EX(CMMVARICtrl, "wCWSpeed", GetWCWSpeed, SetWCWSpeed, VT_I2)
	w = m_ModFSK.GetCWSpeed();
	PX_Short(pPX, _T("wCWSpeed"), w, 20);
	m_ModFSK.SetCWSpeed(w);
	//DISP_PROPERTY_EX(CMMVARICtrl, "bTreatCenterMFSK", GetBTreatCenterMFSK, SetBTreatCenterMFSK, VT_BOOL)
	PX_Bool(pPX, _T("bTreatCenterMFSK"), m_MFSK_Center, FALSE);
	//DISP_PROPERTY_EX(CMMVARICtrl, "bMetricSqMFSK", GetBMetricSqMFSK, SetBMetricSqMFSK, VT_BOOL)
	PX_Bool(pPX, _T("bMetricSqMFSK"), m_MFSK_SQ_Metric, FALSE);
	//DISP_PROPERTY_EX(CMMVARICtrl, "wDiddleRTTY", GetWDiddleRTTY, SetWDiddleRTTY, VT_I2)
	w = m_ModFSK.m_Encode.GetDiddle();
	PX_Short(pPX, _T("wDiddleRTTY"), w, 0);
	m_ModFSK.m_Encode.SetDiddle(w);
	//DISP_PROPERTY_EX(CMMVARICtrl, "dblTxShift", GetDblTxShift, SetDblTxShift, VT_R8)
	double d = m_ModFSK.m_RTTYShift;
	PX_Double(pPX, _T("dblTxShift"), d, 170.0);
	m_ModFSK.SetRTTYShift(d);
	//DISP_PROPERTY_EX(CMMVARICtrl, "bHPF", GetBHPF, SetBHPF, VT_BOOL)
	PX_Bool(pPX, _T("bHPF"), m_fHPF, FALSE);
	//DISP_PROPERTY_EX(CMMVARICtrl, "wNotchTaps", GetWNotchTaps, SetWNotchTaps, VT_I2)
	w = m_Notches.m_nBaseTaps;
	PX_Short(pPX, _T("wNotchTaps"), w, 128);
	m_Notches.m_nBaseTaps = w;
}


/////////////////////////////////////////////////////////////////////////////
// CMMVARICtrl::OnResetState - コントロールのプロパティ値をリセット

void CMMVARICtrl::OnResetState()
{
	COleControl::OnResetState();  // DoPropExchange を呼び出してデフォルト値にリセット

	// この位置にコントロールの状態をリセットする処理を追加してください
}


/////////////////////////////////////////////////////////////////////////////
// CMMVARICtrl::AboutBox - "バージョン情報" のダイアログ ボックスを表示

void CMMVARICtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_MMVARI);
	dlgAbout.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
void CMMVARICtrl::SetTxState(int state)
{
	if( m_TxState != state ){
		m_TxState = state;
		if( m_bActive ) FireOnTxState(state);
	}
}
//---------------------------------------------------------------------------
// サウンドカード初期化パラメータの生成
void __fastcall CMMVARICtrl::InitWFX(void)
{
	m_WFX.wFormatTag = WAVE_FORMAT_PCM;
	m_WFX.nChannels = WORD(m_Wave.m_SoundStereo ? 2 : 1);
	m_WFX.wBitsPerSample = 16;
	m_WFX.nSamplesPerSec = m_SAMPBASE;
	m_WFX.nBlockAlign = WORD(m_WFX.nChannels *(m_WFX.wBitsPerSample/8));
	m_WFX.nAvgBytesPerSec = m_WFX.nBlockAlign * m_WFX.nSamplesPerSec;
	m_WFX.cbSize = 0;
}

//---------------------------------------------------------------------------
/*
	Clock	復調器	FFT
	6000Hz	6000Hz	6000Hz	1024
	8000Hz	8000Hz	8000Hz	2048
	11025Hz	5513Hz	11025Hz	2048
	12000Hz	6000Hz	6000Hz	1024
	16000Hz	8000Hz	8000Hz	2048
	18000Hz	6000Hz	6000Hz	1024
	22050Hz	7350Hz	7350Hz	2048
	24000Hz	6000Hz	6000Hz	1024
	44100Hz	6300Hz	6300Hz	1024
	48000Hz	6000Hz	6000Hz	1024
	50000Hz	6250Hz	6250Hz	1024
*/
BOOL __fastcall CMMVARICtrl::SampleFreq(double f)
{
	int fftsize = m_FFT_SIZE;
	if( f >= 49000.0 ){							// 50000Hz
	    SAMPFREQ = f;
		m_SAMPBASE = 50000;
        m_fDec = 7;
        m_DecFactor = 0.125;
        m_SAMPTYPE = 10;
        m_BufferSize = 8192;
		m_FFT_SIZE = 1024;
        m_FFTFactor = 0.125;
		m_DecCutOff = 2900;
		m_MaxCarrier = 3000;
	}
	else if( f >= 46000.0 ){					// 48000Hz
	    SAMPFREQ = f;
		m_SAMPBASE = 48000;
        m_fDec = 7;
        m_DecFactor = 0.125;
        m_SAMPTYPE = 9;
        m_BufferSize = 8192;
		m_FFT_SIZE = 1024;
        m_FFTFactor = 0.125;
		m_DecCutOff = 2800;
		m_MaxCarrier = 2950;
    }
	else if( f >= 43000.0 ){					// 44100Hz
	    SAMPFREQ = f;
		m_SAMPBASE = 44100;
        m_fDec = 6;
        m_DecFactor = 1.0/7.0;
        m_SAMPTYPE = 8;
        m_BufferSize = 8192;
		m_FFT_SIZE = 1024;
        m_FFTFactor = 1.0/7.0;
		m_DecCutOff = 2900;
		m_MaxCarrier = 3000;
    }
	else if( f >= 23000.0 ){					// 24000Hz
	    SAMPFREQ = f;
		m_SAMPBASE = 24000;
        m_fDec = 3;
        m_DecFactor = 0.25;
        m_SAMPTYPE = 7;
        m_BufferSize = 4096;
		m_FFT_SIZE = 1024;
        m_FFTFactor = 0.25;
		m_DecCutOff = 2800;
		m_MaxCarrier = 2950;
    }
	else if( f >= 20000.0 ){					// 22050Hz
	    SAMPFREQ = f;
		m_SAMPBASE = 22050;
        m_fDec = 2;
        m_DecFactor = 1.0/3.0;
        m_SAMPTYPE = 6;
        m_BufferSize = 6144;
		m_FFT_SIZE = 2048;
        m_FFTFactor = 1.0/3.0;
		m_DecCutOff = 2700;
		m_MaxCarrier = 2700;
    }
	else if( f >= 17000.0 ){					// 18000Hz
	    SAMPFREQ = f;
		m_SAMPBASE = 18000;
        m_fDec = 2;
        m_DecFactor = 1.0/3.0;
        m_SAMPTYPE = 5;
        m_BufferSize = 3072;
		m_FFT_SIZE = 1024;
        m_FFTFactor = 1.0/3.0;
		m_DecCutOff = 2800;
		m_MaxCarrier = 2950;
    }
	else if( f >= 15000.0 ){					// 16000Hz
	    SAMPFREQ = f;
		m_SAMPBASE = 16000;
        m_fDec = 1;
        m_DecFactor = 0.5;
        m_SAMPTYPE = 4;
        m_BufferSize = 4096;
		m_FFT_SIZE = 2048;
        m_FFTFactor = 0.5;
		m_DecCutOff = 2600;
		m_MaxCarrier = 2600;
    }
	else if( f >= 11600.0 ){					// 12000Hz系
	    SAMPFREQ = f;
		m_SAMPBASE = 12000;
        m_fDec = 1;
        m_DecFactor = 0.5;
        m_SAMPTYPE = 3;
        m_BufferSize = 2048;
		m_FFT_SIZE = 1024;
        m_FFTFactor = 0.5;
		m_DecCutOff = 2900;
		m_MaxCarrier = 2950;
	}
    else if( f > 10000.0 ){						// 11025Hz系
	    SAMPFREQ = f;
		m_SAMPBASE = 11025;
        m_fDec = 1;
        m_DecFactor = 0.5;
        m_SAMPTYPE = 0;
        m_BufferSize = 2048;
		m_FFT_SIZE = 2048;
        m_FFTFactor = 1.0;
		m_DecCutOff = 2700;
		m_MaxCarrier = 2700;
    }
    else if( f > 7000.0 ){						// 8000Hz系
	    SAMPFREQ = f;
		m_SAMPBASE = 8000;
        m_fDec = 0;
        m_DecFactor = 1.0;
        m_SAMPTYPE = 1;
        m_BufferSize = 2048;
		m_FFT_SIZE = 2048;
        m_FFTFactor = 1.0;
		m_DecCutOff = 3000;
		m_MaxCarrier = 3000;
    }
	else if( f > 5000 ){						// 6000Hz系
	    SAMPFREQ = f;
		m_SAMPBASE = 6000;
        m_fDec = 0;
        m_DecFactor = 1.0;
        m_SAMPTYPE = 2;
        m_BufferSize = 1024;
		m_FFT_SIZE = 1024;
        m_FFTFactor = 1.0;
		m_DecCutOff = 2950;
		m_MaxCarrier = 2950;
    }
    else {
		return FALSE;
    }
	m_SampleFreq = SAMPFREQ;
	m_DemSampleFreq = m_SampleFreq * m_DecFactor;
	m_FFTSampFreq = m_SampleFreq * m_FFTFactor;
	m_CalcFFTWidth = int(4001 * m_FFT_SIZE / m_FFTSampFreq);
	if( m_CalcFFTWidth > (m_FFT_SIZE*2+1) ) m_CalcFFTWidth = (m_FFT_SIZE*2+1);
	m_Notches.m_SampleFreq = m_FFTSampFreq;
	if( m_bActive ){
	    if( fftsize != m_FFT_SIZE ) m_FFT.InitFFT(m_FFT_SIZE);
		if( m_Notches.m_Count ) m_Notches.Create();
	}
    return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::UpdateModGain(void)
{
	m_ModGainR = m_ModGain;
    if( m_ModGainR < 1024 ) m_ModGainR = 1024;
    if( m_ModGainR > 31774 ) m_ModGainR = 31774;
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::KillSoundTimer(void)
{
	if( m_nSoundTimerID ){
		KillTimer(m_nSoundTimerID);
		m_nSoundTimerID = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::SetRXFifo(int d)
{
	if( d == m_Wave.m_InFifoSize ) return;

    BOOL f = m_Wave.IsInOpen();
    m_Wave.InClose();
    m_Wave.m_InFifoSize = d;
    if( f ) OpenSound(FALSE);
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::SetTXFifo(int d)
{
	if( d == m_Wave.m_OutFifoSize ) return;

    BOOL f = m_Wave.IsOutOpen();
    m_Wave.OutAbort();
    m_Wave.m_OutFifoSize = d;
    if( f ) ReOutOpen();
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::StrSoundID(CString &cs)
{
	if( m_Wave.m_SoundID == -2 ){
		cs = m_Wave.m_SoundMMW;
    }
    else {
		char bf[128];
		LPSTR t = bf;
		sprintf(t, "%d", m_Wave.m_SoundID);
		if( m_Wave.m_SoundID != m_Wave.m_SoundTxID ){
            t += strlen(t);
            sprintf(t, ",%d", m_Wave.m_SoundTxID);
        }
		cs = t;
    }
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::SetSoundCard(int ch, LPCSTR pID)
{
	BOOL f = FALSE;
	int id = m_Wave.m_SoundID;
    int idtx = m_Wave.m_SoundTxID;
	if( pID ){
		LPSTR pBF = StrDupe(pID);
		LPSTR pp, tt;
        pp = SkipSpace(StrDlm(tt, pBF));
		if( sscanf(tt, "%d", &id) != 1 ){
			id = -2;
            f = strcmp(m_Wave.m_SoundMMW, tt);
            m_Wave.m_SoundMMW = tt;
        }
        else if( *pp ){
			sscanf(pp, "%d", &idtx);
        }
        else {
			idtx = id;
        }
        delete pBF;
    }
	if( (m_Wave.m_SoundStereo != ch) || (id != m_Wave.m_SoundID) || (idtx != m_Wave.m_SoundTxID) || f ){
		BOOL f = FALSE;
		if( m_bActive ){
			if( m_TX ) ToRX();
			f = m_Wave.IsInOpen();
			m_Wave.InClose();
		}
        m_Wave.m_SoundStereo = ch;
        m_Wave.m_SoundID = id;
        m_Wave.m_SoundTxID = idtx;
        InitWFX();
		if( f ) OpenSound(FALSE);
    }
}

//---------------------------------------------------------------------------
BOOL __fastcall CMMVARICtrl::IsSoundOpen(void)
{
	return m_Wave.IsInOpen() || m_Wave.IsOutOpen();
}

//---------------------------------------------------------------------------
BOOL __fastcall CMMVARICtrl::OpenSound(BOOL fTX)
{
	m_Wave.m_hWnd = m_hWnd;
	m_Wave.UpdateDevice(m_Wave.m_SoundID);
	BOOL r;
	if( fTX ){
		r = m_Wave.OutOpen(&m_WFX, m_Wave.m_SoundTxID, m_BufferSize);
    }
    else {
		r = m_Wave.InOpen(&m_WFX, m_Wave.m_SoundID, m_BufferSize);
    }
	if( !r && (m_nErrorCode == errorSoundOpen) ) m_nErrorCode = 0;
    if( r || m_nSoundTimerID || m_fSuspend ) return r;

	// サウンドカードリカバリタイマーを起動
	if( m_bActive ) FireOnDrawFFT(0, (long *)m_fftout);
	m_SoundMsgTimer = m_Wave.GetTimeout();
	m_RecoverSoundMode = fTX;
	m_nSoundTimerID = SetTimer(timerSOUND, intvalSOUND, NULL);
    return r;
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::CloseSound(void)
{
	KillSoundTimer();
	m_Wave.InClose();
	m_Wave.OutAbort();
}
//---------------------------------------------------------------------------
BOOL __fastcall CMMVARICtrl::ReOutOpen(void)
{
	if( !OpenSound(TRUE) ) return FALSE;
    memset(m_wBuffer, 0, sizeof(m_wBuffer));
	while(!m_Wave.IsOutBufFull()) m_Wave.OutWrite(m_wBuffer);
    return TRUE;
}

//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::OnWaveIn(void)
{
	while(m_Wave.IsInOpen() && !m_Wave.IsInBufNull()){
		if( !m_Wave.InRead(m_wBuffer) ){
			m_Wave.InClose();
        	OpenSound(FALSE);
            m_statLostRX = LOSTMSGTIME * int(m_SampleFreq) / m_BufferSize;
			m_nErrorCode = errorLostRX;
			FireOnError(m_nErrorCode);
        }
		else if( m_statLostRX ){
			m_statLostRX--;
			if( !m_statLostRX ) m_nErrorCode = 0;
		}
		if( m_PlayBack.IsActive() && !m_TX ){
			if( m_PlayBack.IsPlaying() ){
				for( int i = 0; i < m_PlayBackSpeed; i++ ){
		        	if( m_PlayBack.Read(m_wBuffer) ){
						DoDem();
                    }
                    else {
						break;
                    }
                }
                if( !m_PlayBack.IsPlaying() ){
					StopPlayBack();
                }
            }
            else {
	        	m_PlayBack.Write(m_wBuffer);
		        DoDem();
            }
        }
        else {
	        DoDem();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::OnWaveOut(void)
{
	while(m_Wave.IsOutOpen() && !m_Wave.IsOutBufFull()){
		DoMod();
		if( (m_LoopBack == loopINTERNAL) ) DoDem();

		if( !m_Wave.OutWrite(m_wBuffer) ){
			m_Wave.OutAbort();
			OpenSound(TRUE);
	        m_statLostTX = LOSTMSGTIME * int(m_SampleFreq) / m_BufferSize;
			m_nErrorCode = errorLostRX;
			FireOnError(m_nErrorCode);
		}
		else if( m_statLostTX ){
			m_statLostTX--;
			if( !m_statLostTX ) m_nErrorCode = 0;
		}
	}

	if( m_fReqRX ){
		if( m_fTone ){
			if( m_fReqRX == 1 ){
                m_ModFSK.m_OutVol = 0;
				m_Wave.SetOutBCC(m_Wave.GetOutBC());
				m_fReqRX++;
				SetTxState(txstateWAIT);
            }
			else if( m_Wave.GetOutBCC() < 0 ){
				ToRX();
			}
        }
   		else if( m_ModFSK.m_Encode.m_Idle ){
			if( m_fReqRX == 1 ){
				m_fReqRX++;
				if( m_bAddStopCR && m_fSendChar ){
    				m_ModFSK.m_Encode.PutChar('\r');
    				m_ModFSK.m_Encode.PutChar('\n');
				}
				else {
					m_ModFSK.m_Encode.SetMark();
					m_Wave.SetOutBCC(m_Wave.GetOutBC());
					m_fReqRX++;
				}
				SetTxState(txstateWAIT);
			}
			else if( m_fReqRX == 2 ){
				m_ModFSK.m_Encode.SetMark();
				m_Wave.SetOutBCC(m_Wave.GetOutBC());
				m_fReqRX++;
			}
			else if( m_Wave.GetOutBCC() < 0 ){
				ToRX();
			}
            else {
                m_ModFSK.m_OutVol = 0;
            }
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::DoDem(double d)
{
	if( m_SkipTmg > 0 ){
		m_SkipTmg--;
        return;
    }
	CRxSet *pRxSet = &m_RxSet[0];
	CDEMFSK *pDem = pRxSet->m_pDem;
    int c;
	if( pDem->Do(d, pRxSet->m_SQ, pRxSet->m_fATC ) ){
		while((c = pDem->GetData())!=-1){
			PutDumpChar(c, pRxSet);
        }
	}
	if( m_wCollectType ){
		if( m_wCollectType == collectWAVE ){
			m_Collect1.Do(pDem->GetS());
			m_Collect2.Do(long(pDem->m_d));
			if( m_Collect1.IsFull() ){
				FireOnDrawWave(m_Collect1.GetCount(), m_Collect1.GetZP(), m_Collect2.GetZP());
		    	m_Collect1.Clear(); m_Collect2.Clear();
                m_Lock = FALSE;
            }
        }
        else {
			if( pDem->GetTmgLock() && pDem->m_Lock ){
				m_Collect1.Do(pDem->GetTmg()*16384);
   				m_Collect2.Do(pDem->GetS());
    	        m_Lock = TRUE;
				if( (m_RxSet[0].IsMFSK() ) && m_Collect1.IsFull() ){
					FireOnDrawWave(m_Collect1.GetCount(), m_Collect1.GetZP(), m_Collect2.GetZP());
			    	m_Collect1.Clear(); m_Collect2.Clear();
            	    m_Lock = FALSE;
            	}
	        }
    	    else if( m_Lock ){
				FireOnDrawWave(m_Collect1.GetCount(), m_Collect1.GetZP(), m_Collect2.GetZP());
		    	m_Collect1.Clear(); m_Collect2.Clear();
				m_Lock = FALSE;
        	}
		}
    }
    if( m_nSubWindow ){
		pRxSet = &m_RxSet[1];
	    for( int i = 1; i <= m_nSubWindow; i++, pRxSet++ ){
	        pDem = pRxSet->m_pDem;
			if( pRxSet->IsActive() ){
				if( pDem->Do(d, pRxSet->m_SQ, pRxSet->m_fATC ) ){
					while((c = pDem->GetData())!=-1){
						PutDumpChar(c, pRxSet);
                    }
	            }
	        }
	    }
    }
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::DoDem(void)
{
	long timing;
	int i;
    SHORT *wp = m_wBuffer;

    double *dp = m_fftbuf;
    BOOL fNOTCH = (m_TX != txINTERNAL) && m_Notches.m_FIR.IsActive();
	switch(m_SAMPTYPE){
        case 0:		// 11025Hz
		    for( i = 0; i < m_BufferSize; i++, wp++ ){
				if( m_fHPF ) *wp = short(m_HPF.Do(*wp));
				if( fNOTCH ) *wp = short(m_Notches.m_FIR.Do(*wp));
				if( m_Dec2.Do(*wp) ){
					DoDem(m_Dec2.GetOut());
        		}
				*dp++ = *wp;
        	}
        	break;
        case 1:		// 8000Hz
		case 2:		// 6000Hz
		    for( i = 0; i < m_BufferSize; i++, wp++ ){
				if( m_fHPF ) *wp = short(m_HPF.Do(*wp));
				if( fNOTCH ) *wp = short(m_Notches.m_FIR.Do(*wp));
				DoDem(*wp);
				*dp++ = *wp;
        	}
        	break;
        default:
//					12000Hz
//					18000Hz
//					24000Hz
//					44100Hz
//					48000Hz
//					50000Hz
//					// 16000Hz
//					// 22050Hz
		    for( i = 0; i < m_BufferSize; i++, wp++ ){
				if( m_fHPF ) *wp = short(m_HPF.Do(*wp));
				if( m_Dec2.Do(*wp) ){
					if( fNOTCH ) m_Dec2.SetOut(m_Notches.m_FIR.Do(m_Dec2.GetOut()));
					DoDem(m_Dec2.GetOut());
					*dp++ = m_Dec2.GetOut();
        		}
        	}
        	break;
    }
	try {
	    m_FFT.Calc(m_fftbuf, m_CalcFFTWidth, FFTSC*10.0, m_FFTSmooth, m_fftout);
    }
    catch(...){
		m_FFT.InitFFT(m_FFT_SIZE);
        m_FFT.m_FFTDIS = 0;
    }
	for( i = 0; i < RXMAX; i++ ){
		CalcStgFFT(&m_RxSet[i]);
    }
	FireOnDrawFFT(m_CalcFFTWidth, (long *)m_fftout);
	CRxSet *pRxSet = m_RxSet;
	CDEMFSK *pDem = pRxSet->m_pDem;
	pRxSet->m_fOver = (m_TX!=txINTERNAL) && pDem->m_AGC.GetOver();
#if 0
	if( pDem->m_bCollect && pDem->m_Collect[0].IsFull() ){
		FireOnDrawScope(0, pDem->m_Collect[0].GetCount(), pDem->m_Collect[0].GetZP(), pDem->m_Collect[1].GetZP());
		pDem->m_Collect[0].Clear();
		pDem->m_Collect[1].Clear();
	}
#endif

	if( m_TX != txINTERNAL ){
		if( m_AFCKeyTimer ){
        	m_AFCKeyTimer--;
        }
		else if( pRxSet->m_fAFC ){
			pDem->UpdateBPF();
			m_wRxCarrier = int(pDem->m_CarrierFreq+0.5);
			if( pRxSet->m_CarrierFreq != m_wRxCarrier ){
				pRxSet->m_CarrierFreq = m_wRxCarrier;
				FireOnRxCarrier(0, m_wRxCarrier);
			}
        }
		if( pRxSet->IsRTTY() ){
			if( pRxSet->m_pDem->m_Decode.IsRTTYTmg() ){
				timing = long(pDem->m_Decode.GetRTTYTmg() * 100.0);
            }
            else {
				timing = 0;
            }
			if( pRxSet->m_Timing != timing ){
				pRxSet->m_Timing = timing;
				FireOnTiming(0, timing, 1);
			}
        }
		else if( pRxSet->IsMFSK() ){
			timing = pRxSet->m_pDem->GetClockError();
			FireOnTiming(0, timing, 0);
			if( pRxSet->m_Timing != timing ){
				pRxSet->m_Timing = timing;
				FireOnTiming(0, timing, 0);
			}
        }
		else if( m_bATC ){
			timing = pDem->GetClockError();
			m_dwATC = timing;
			if( pRxSet->m_Timing != timing ){
				pRxSet->m_Timing = timing;
				FireOnTiming(0, timing, 0);
			}
        }
    }
    if( m_nSubWindow ){
		pRxSet = &m_RxSet[1];
		int freq;
   		for( i = 1; i <= m_nSubWindow; i++, pRxSet++ ){
			if( pRxSet->IsActive() ){
				pDem = pRxSet->m_pDem;
				pRxSet->m_fOver = (m_TX!=txINTERNAL) && pDem->m_AGC.GetOver();
				if( pRxSet->m_fAFC ){
					pDem->UpdateBPF();
					freq = int(pDem->m_CarrierFreq+0.5);
					if( pRxSet->m_CarrierFreq != freq ){
						pRxSet->m_CarrierFreq = freq;
						FireOnRxCarrier(i, freq);
					}
				}
				if( pRxSet->IsRTTY() ){
					if( pDem->m_Decode.IsRTTYTmg() ){
						timing = long(pDem->m_Decode.GetRTTYTmg() * 100.0);
					}
					else {
						timing = 0;
					}
					if( pRxSet->m_Timing != timing ){
						pRxSet->m_Timing = timing;
						FireOnTiming(i, timing, 1);
					}
				}
				else {
					timing = pDem->GetClockError();
					if( pRxSet->m_Timing != timing ){
						pRxSet->m_Timing = timing;
						FireOnTiming(i, timing, 0);
					}
				}
#if 0
				if( pDem->m_bCollect && pDem->m_Collect[0].IsFull() ){
					FireOnDrawScope(pRxSet->m_nIndex, pDem->m_Collect[0].GetCount(), pDem->m_Collect[0].GetZP(), pDem->m_Collect[1].GetZP());
					pDem->m_Collect[0].Clear();
					pDem->m_Collect[1].Clear();
				}
#endif
			}
        }
    }
	if( m_Clock.IsActive() ){
		wp = m_wBuffer;
		for( i = 0; i < m_BufferSize; i++ ){
			if( m_Clock.Do(*wp++) ){
				FireOnClockAdjust((long *)m_Clock.GetData());
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::DoMod(void)
{
	int i;
    SHORT *wp = m_wBuffer;
	if( m_fTone ){
	    for( i = 0; i < m_BufferSize; i++, wp++ ){
			*wp = SHORT(m_ModFSK.DoCarrier()*m_ModGainR);
	  	}
    }
    else {
	    for( i = 0; i < m_BufferSize; i++, wp++ ){
			*wp = SHORT(m_ModFSK.Do()*m_ModGainR);
	  	}
    }
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::RxChar(int nIndex, int m)
{
	char bf[8];
	LPSTR t = bf;
	if( m & 0x0000ff00 ){
		*t++ = BYTE(m >> 8);
	}
	*t++ = BYTE(m);
	*t = 0;
	FireOnRxChar(nIndex, bf, m);
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::PutDumpChar(int d, CRxSet *pRxSet)
{
	if( pRxSet->IsRTTY() ){
		if( d ) RxChar(pRxSet->m_nIndex, d);
    }
    else {
		int m;
		if( pRxSet->IsMFSK() ){
        	m = d;
        }
        else {
	        m = g_VariCode.Index2Mbcs(d, (pRxSet->m_fJA && !pRxSet->m_fTWO));
        }
		if( pRxSet->m_fTWO ){
			if( (m >= 0) && (m < 256) ){
				if( pRxSet->m_fMBCS ){
					m |= pRxSet->m_fMBCS;
           	        pRxSet->m_fMBCS = 0;
                }
				else if( pRxSet->m_MBCS.IsLead(BYTE(m)) ){
					pRxSet->m_fMBCS = (m << 8);
           	    }
				if( !pRxSet->m_fMBCS ) RxChar(pRxSet->m_nIndex, m);
   	        }
       	}
        else {
			RxChar(pRxSet->m_nIndex, m);
       	}
   }
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::SetTxCarrier(short wFreq)
{
	if( m_wTxCarrier != wFreq ){
		m_wTxCarrier = wFreq;
		if( m_bActive ){
			m_ModFSK.SetCarrierFreq(wFreq);
			if( m_TX == txINTERNAL ){
        		m_RxSet[0].SetCarrierFreq(wFreq);
			}
			FireOnTxCarrier(wFreq);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::SetRxCarrier(short nIndex, short wFreq)
{
	CRxSet *pRxSet = &m_RxSet[nIndex];
	if( nIndex == 0 ){
		if( m_TX != txINTERNAL ){
			if( m_wRxCarrier != wFreq ){
				m_wRxCarrier = wFreq;
				pRxSet->SetCarrierFreq(wFreq);
				if( m_bActive ){
					FireOnRxCarrier(0, wFreq);
					if( m_bNET ){
						SetTxCarrier(wFreq);
					}
				}
			}
		}
	}
	else if( (nIndex >= 1) && (nIndex < RXMAX) ){
		if( pRxSet->m_CarrierFreq != wFreq ){
			pRxSet->SetCarrierFreq(wFreq);
			if( m_bActive ) FireOnRxCarrier(nIndex, wFreq);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::SetTXInternal(void)
{
	if( m_LoopBack == loopINTERNAL ){
		m_RxSet[0].SetCarrierFreq(m_wTxCarrier * m_SampleFreq/(m_SampleFreq+m_TxOffset));
   		CRxSet *pRxSet = m_RxSet;
		for( int i = 0; i < RXMAX; i++, pRxSet++ ){
			if( pRxSet->IsActive() ){
				if( i ){
       	        	pRxSet->SetSampleFreq((m_SampleFreq+m_TxOffset)*m_DecFactor);
					if( pRxSet->m_fAFC ){
						int f = int(pRxSet->m_pDem->m_CarrierFreq - m_wTxCarrier);
    	                f = ABS(f);
						if( f < (pRxSet->m_Speed * 0.5) ){
							pRxSet->SetCarrierFreq(m_wTxCarrier);
							FireOnRxCarrier(i, m_wTxCarrier);
						}
                    }
				}
				pRxSet->m_fOver = FALSE;
		        pRxSet->m_pDem->MakeBPF(8);		// CPU負荷低減のため
				pRxSet->m_pDem->m_Decode.m_fATC = FALSE;
				pRxSet->m_pDem->m_fAFC = FALSE;
				pRxSet->m_pDem->m_Decode.Create();
				pRxSet->m_pDem->SetTmg(i ? 0 : int(1e6*((m_SampleFreq+m_TxOffset)/m_SampleFreq - 1.0)));
		    	pRxSet->m_pDem->m_AGC.Reset();
                pRxSet->m_PeakSig = 0;
                pRxSet->m_AvgSig.Create(32);
				for( int j = 0; j < m_BufferSize; j++ ){
       				pRxSet->m_pDem->Do(0);
		        }
				pRxSet->InitStgFFT();
            }
		}
		FFTSampleFreq(m_SampleFreq + m_TxOffset);
    }
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::ToTone(void)
{
	if( !m_bActive ) return;
	if( m_fSuspend ) return;
	m_fTone = TRUE;
	m_TX = (m_LoopBack == loopINTERNAL) ? txINTERNAL : txEXTERNAL;
	if( m_LoopBack == loopINTERNAL ){
    	m_Wave.InClose();
		KillSoundTimer();
    }
   	if( m_bNET ){
		SetTxCarrier(m_wRxCarrier);
    }
	m_ModFSK.Reset();	// For Signal/CW gain
	m_ModFSK.SetCarrierFreq(m_wTxCarrier);
	SetTXInternal();
	m_fSendChar = FALSE;
	m_Wave.m_hWnd = m_hWnd;
	m_WaterSemaphore++;
	if( !m_Wave.IsOutOpen() ) OpenSound(TRUE);
	memset(m_wBuffer, 0, sizeof(m_wBuffer));
	SetPTT(TRUE);
	if( m_Wave.IsOutFirst() ){
		while(!m_Wave.IsOutBufFull()){
			DoMod();
			if( m_LoopBack == loopINTERNAL ) DoDem();
			m_Wave.OutWrite(m_wBuffer);
   		}
    }
	SetTxState(txstateTONE);
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::ToTX(BOOL fCW)
{
	if( !m_bActive ) return;
	if( m_fSuspend ) return;
	StopPlayBack();
	BOOL fATone = m_fTone;
	m_fTone = FALSE;
	m_TX = (m_LoopBack == loopINTERNAL) ? txINTERNAL : txEXTERNAL;
	if( m_LoopBack == loopINTERNAL ){
    	m_Wave.InClose();
		KillSoundTimer();
    }
   	if( m_bNET ){
		SetTxCarrier(m_wRxCarrier);
    }
	if( !fATone ) m_ModFSK.Reset();	// For Signal/CW gain
	m_ModFSK.m_Encode.Reset(fCW);
	if( m_bAddStartCR && !fCW ){
	   	m_ModFSK.m_Encode.PutChar('\r');
	   	m_ModFSK.m_Encode.PutChar('\n');
    }
	if( !fATone ) m_ModFSK.SetCarrierFreq(m_bNET ? m_RxSet[0].m_pDem->m_CarrierFreq : m_wTxCarrier);
	m_RxSet[0].m_pDem->ResetMFSK();
	SetTXInternal();
	m_fSendChar = FALSE;
	m_Wave.m_hWnd = m_hWnd;
	m_WaterSemaphore++;
	if( !m_Wave.IsOutOpen() ) OpenSound(TRUE);
	if( m_pCom ){
		m_pCom->SetFSK(m_bCOMFSK, m_bCOMFSKINV);
		m_pCom->SetDiddle(m_ModFSK.m_Encode.GetDiddle());
    	m_pCom->SetSendChar(m_RxSet[0].IsRTTY());
    }
	if( !fATone ){
		SetPTT(TRUE);
		memset(m_wBuffer, 0, sizeof(m_wBuffer));
    }
	if( m_LoopBack == loopINTERNAL ){
		DoDem();
		if( !m_RxSet[0].IsMFSK() ){
			for( int i = 0; i < 8192; i++ ){
				m_RxSet[0].m_pDem->m_Decode.Do(1, TRUE, FALSE);
        	}
        }
    }
	if( m_Wave.IsOutFirst() ){
		while(!m_Wave.IsOutBufFull()){
			DoMod();
			if( m_LoopBack == loopINTERNAL ) DoDem();
			m_Wave.OutWrite(m_wBuffer);
   		}
    }
	SetTxState(txstateTX);
    if( m_fReqRX ){
		m_fReqRX = 1;
		m_ModFSK.m_Encode.m_fReqRX = TRUE;
		SetTxState(txstateREQRX);
    }
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::ToRX(void)
{
	m_fTone = FALSE;

	m_TX = txRX;
	m_fReqRX = FALSE;
	m_ModFSK.m_Encode.m_fReqRX = FALSE;
   	m_Wave.OutAbort();
	SetPTT(FALSE);
	m_WaterSemaphore++;
	if( !m_Wave.IsInOpen() ){		// Loopback == loopINTERNAL
		DoSetBPF(m_nBPFType);
	    m_RxSet[0].SetCarrierFreq( (m_wRxCarrier != int(m_ModFSK.m_CarrierFreq+0.5)) ? m_wRxCarrier : m_ModFSK.m_CarrierFreq);
        CRxSet *pRxSet = m_RxSet;
        int atctmg = m_dwATC;
		for(int i = 0; i < RXMAX; i++, pRxSet++ ){
			if( pRxSet->IsActive() ){
				if( i ){
					pRxSet->SetSampleFreq(m_DemSampleFreq);
                	pRxSet->m_pDem->MakeBPF(m_RxSet[0].m_pDem->m_PreBPFTaps);
                }
				pRxSet->m_pDem->m_AFCCount = 0;
				pRxSet->m_pDem->m_fAFC = pRxSet->m_fAFC;
				pRxSet->m_pDem->m_Decode.m_fATC = i ? TRUE : m_bATC;
//				if( !pRxSet->IsRTTY() ) pRxSet->m_pDem->m_Decode.SetTmg(i ? 0 : atctmg);
				pRxSet->m_pDem->SetTmg((i || pRxSet->IsMFSK()) ? 0 : atctmg);
                pRxSet->m_pDem->m_Decode.ResetMeasRTTY();
                pRxSet->m_pDem->ResetMFSK();
				for( int j = 0; j < m_BufferSize; j++ ){
		        	pRxSet->m_pDem->Do(0);
		            pRxSet->m_pDem->m_Decode.Do(0, 0, FALSE);
        		}
                pRxSet->m_pDem->m_AGC.Reset();
				pRxSet->m_fOver = FALSE;
				pRxSet->InitStgFFT();
		        pRxSet->m_SQ = FALSE;
        		pRxSet->m_AFCTimerW = int(5 * m_SampleFreq / m_BufferSize);
		        pRxSet->m_AFCTimerN = int(m_SampleFreq / m_BufferSize);
                pRxSet->m_SQTimer = 0;
				pRxSet->m_AFCTimerPSK = 0;
				pRxSet->m_AFCTimerMFSK = MFSKAFC_MAX;
                pRxSet->m_PeakSig = 0;
                pRxSet->m_AvgSig.Create(32);
            }
        }
		OpenSound(FALSE);
		FFTSampleFreq(m_SampleFreq);
    	m_FFT.ClearStg();
    }
    else {								// loopEXTERNAL
        m_RxSet[0].m_AFCTimerN = int(3 * m_SampleFreq / m_BufferSize);
    }
	m_RxSet[0].m_PeakSig = 0;
	m_RxSet[0].m_AFCTimerPSK = 0;
	m_RxSet[0].m_AFCTimerMFSK = MFSKAFC_MAX;
	SetTxState(txstateRX);
}

//---------------------------------------------------------------------------
double __fastcall SqrtToDB(double d)
{
	d /= (0.00345 * 10.0 * FFTSC);
    d = d * d;
	d = d * d;
    return FFTSC * 10.0 * (log10(d+2.81458e4) - 4.4494132);
}
//---------------------------------------------------------------------------
double __fastcall DBToSqrt(double d)
{
	d /= FFTSC * 10.0;
    d += 4.4494132;
    d = pow(10, d) - 2.81458e4;
	d = pow(d, 0.25);
    return d * (0.00345 * 10.0 * FFTSC);
}
//---------------------------------------------------------------------------
double __fastcall CMMVARICtrl::AdjDB(double d)
{
    if( m_FFT.m_FFTGain ){
		return SqrtToDB(d);
    }
    else {
		return d * 100.0 / FFTSC;
    }
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::CalcStgFFT(CRxSet *pRxSet)
{
	if( !pRxSet->IsActive() ) return;

	CDEMFSK *pDem = pRxSet->m_pDem;
	int fo = int(pDem->m_CarrierFreq);

	int fm, xl, xh, xnl, xnh, xol, xoh;
    switch(pRxSet->m_Mode){
		case MODE_GMSK:
		    fm = int(pRxSet->m_Speed * 2 + 100);
			xl = int((fo - fm) * m_FFT_SIZE / m_FFTSampFreq);
		    xh = int((fo + fm) * m_FFT_SIZE / m_FFTSampFreq);
			xnl = int((fo - pRxSet->m_Speed*1.6) * m_FFT_SIZE / m_FFTSampFreq);
		    xnh = int((fo + pRxSet->m_Speed*1.6) * m_FFT_SIZE / m_FFTSampFreq);
		    xol = int((fo - pRxSet->m_Speed/4) * m_FFT_SIZE / m_FFTSampFreq);
		    xoh = int((fo + pRxSet->m_Speed/4) * m_FFT_SIZE / m_FFTSampFreq);
			break;
        case MODE_FSK:
		    fm = int(pRxSet->m_Speed * 4 + 100);
			xl = int((fo - fm) * m_FFT_SIZE / m_FFTSampFreq);
		    xh = int((fo + fm) * m_FFT_SIZE / m_FFTSampFreq);
			xnl = int((fo - pRxSet->m_Speed*3.2) * m_FFT_SIZE / m_FFTSampFreq);
		    xnh = int((fo + pRxSet->m_Speed*3.2) * m_FFT_SIZE / m_FFTSampFreq);
		    xol = int((fo - pRxSet->m_Speed/2) * m_FFT_SIZE / m_FFTSampFreq);
		    xoh = int((fo + pRxSet->m_Speed/2) * m_FFT_SIZE / m_FFTSampFreq);
        	break;
		case MODE_N_BPSK:
		case MODE_BPSK:
		    fm = int(pRxSet->m_Speed * 2 + 100);
			xl = int((fo - fm) * m_FFT_SIZE / m_FFTSampFreq);
		    xh = int((fo + fm) * m_FFT_SIZE / m_FFTSampFreq);
			xnl = int((fo - pRxSet->m_Speed*1.6) * m_FFT_SIZE / m_FFTSampFreq);
		    xnh = int((fo + pRxSet->m_Speed*1.6) * m_FFT_SIZE / m_FFTSampFreq);
		    xol = int((fo - pRxSet->m_Speed*0.6) * m_FFT_SIZE / m_FFTSampFreq);
		    xoh = int((fo + pRxSet->m_Speed*0.6) * m_FFT_SIZE / m_FFTSampFreq);
			break;
		case MODE_FSKW:
		case MODE_RTTY:
        case MODE_U_RTTY:
			fm = int((pRxSet->m_pDem->m_RTTYShift * 0.5) + 150);
            if( fm > 1500 ) fm = 1500;
			xl = int((fo - fm) * m_FFT_SIZE / m_FFTSampFreq);
		    xh = int((fo + fm) * m_FFT_SIZE / m_FFTSampFreq);
			xnl = xl;
		    xnh = xh;
			fm = int((pRxSet->m_pDem->m_RTTYShift * 0.5) + 15);
		    xol = int((fo - fm) * m_FFT_SIZE / m_FFTSampFreq);
		    xoh = int((fo + fm) * m_FFT_SIZE / m_FFTSampFreq);
        	break;
		case MODE_mfsk_L:
			fm = int(pRxSet->m_pDem->m_MFSK_BW);
			if( m_MFSK_Center ){
				xl = int((fo - fm/2 - 150) * m_FFT_SIZE / m_FFTSampFreq);
			    xh = int((fo + fm/2 + 150) * m_FFT_SIZE / m_FFTSampFreq);
				xnl = xl;
		    	xnh = xh;
			    xol = int((fo - fm/2 - 15) * m_FFT_SIZE / m_FFTSampFreq);
			    xoh = int((fo + fm/2 + 15) * m_FFT_SIZE / m_FFTSampFreq);
            }
            else {
				xl = int((fo - fm - 150) * m_FFT_SIZE / m_FFTSampFreq);
			    xh = int((fo + 150) * m_FFT_SIZE / m_FFTSampFreq);
				xnl = xl;
		    	xnh = xh;
			    xol = int((fo - fm - 15) * m_FFT_SIZE / m_FFTSampFreq);
			    xoh = int((fo + 15) * m_FFT_SIZE / m_FFTSampFreq);
            }
            break;
		case MODE_mfsk_U:
			fm = int(pRxSet->m_pDem->m_MFSK_BW);
			if( m_MFSK_Center ){
				xl = int((fo - fm/2 - 150) * m_FFT_SIZE / m_FFTSampFreq);
			    xh = int((fo + fm/2 + 150) * m_FFT_SIZE / m_FFTSampFreq);
				xnl = xl;
		    	xnh = xh;
			    xol = int((fo - fm/2 - 15) * m_FFT_SIZE / m_FFTSampFreq);
			    xoh = int((fo + fm/2 + 15) * m_FFT_SIZE / m_FFTSampFreq);
            }
			else {
				xl = int((fo - 150) * m_FFT_SIZE / m_FFTSampFreq);
			    xh = int((fo + fm + 150) * m_FFT_SIZE / m_FFTSampFreq);
				xnl = xl;
			    xnh = xh;
			    xol = int((fo - 15) * m_FFT_SIZE / m_FFTSampFreq);
			    xoh = int((fo + fm + 15) * m_FFT_SIZE / m_FFTSampFreq);
            }
            break;
    }
	fm = int(200 * m_FFT_SIZE / m_FFTSampFreq);
	if( xl < fm ) xl = fm;
	if( xol == xoh ){xol--; xoh++;}
    if( xol < fm ) xol = fm;

	int i, y;
	int sum = 0;
    int sumn = 0;
    int max = 0;
    int wmax = 0;
	int m = 0;
    int n = 0;
	for( i = xl; i < xh; i++ ){
		y = m_fftout[i];
        if( max < y ) max = y;
		if( (i >= xol) && (i <= xoh) ){
			if( wmax < y ) wmax = y;
        }
		else if( (i >= xnl) && (i <= xnh) ){
			sumn += y;
            n++;
        }
        else {
	        sum += y;
            m++;
        }
    }
	if( m ) sum /= m;
    if( n ) sumn /= n;
	if( !m || (sumn < sum) ) sum = sumn;
	pRxSet->m_StgFFT.Sum = (pRxSet->m_StgFFT.Sum + sum) / 2;
    pRxSet->m_StgFFT.Max = (pRxSet->m_StgFFT.Max + max) / 2;
	pRxSet->m_StgFFT.WMax = (pRxSet->m_StgFFT.WMax + wmax) / 2;
    if( m_FFT.m_FFTGain ){
		pRxSet->m_StgFFT.dBSum = int(SqrtToDB(pRxSet->m_StgFFT.Sum));
		pRxSet->m_StgFFT.dBMax = int(SqrtToDB(pRxSet->m_StgFFT.Max));
		pRxSet->m_StgFFT.dBWMax = int(SqrtToDB(pRxSet->m_StgFFT.WMax));
		if( m_TX == txINTERNAL ) pRxSet->m_StgFFT.dBSum = 0;
    }
    else {
		double k = 100.0 / FFTSC;
		pRxSet->m_StgFFT.dBSum = int(pRxSet->m_StgFFT.Sum * k);
		pRxSet->m_StgFFT.dBMax = int(pRxSet->m_StgFFT.Max * k);
		pRxSet->m_StgFFT.dBWMax = int(pRxSet->m_StgFFT.WMax * k);
		if( m_TX == txINTERNAL ) pRxSet->m_StgFFT.dBSum = -200;
    }

	BOOL fMFSKMet = FALSE;
	int d = pRxSet->m_StgFFT.Sig - 500;
    if( d > LEVELMAX ) d = LEVELMAX;
	int sq = pRxSet->m_SQ;
	if( (m_TX != txINTERNAL) && pRxSet->IsMFSK() && m_MFSK_SQ_Metric ){
		pRxSet->m_SQ = (pRxSet->m_pDem->GetMFSKMetric(0)) >= pRxSet->m_SQLevel;
    }
    else {
		if( pRxSet->m_pDem->IsMFSKSQ() && (d < pRxSet->m_SQLevel) ){
			fMFSKMet = TRUE;
        	pRxSet->m_SQ = TRUE;
	    }
    	else {
	    	pRxSet->m_SQ = (d >= pRxSet->m_SQLevel);
	    }
    	if( pRxSet->m_SQLevel < 50 ) pRxSet->m_SQ = TRUE;
	}
    if( pRxSet->m_SQ ){
		pRxSet->m_SQTimer++;
		if( !sq ){
			pRxSet->m_pDem->m_AFCCount = 0;
			pRxSet->m_pDem->m_Decode.m_BAUDOT.ClearRX();
            pRxSet->m_fMBCS = 0;
			if( !m_TX ){
	           	pRxSet->m_PeakSig = 0;
   		        pRxSet->m_AvgSig.Create(32);
            }
        }
		if( m_TX ){
			pRxSet->m_cAutoTS1 = int(-5 * m_SampleFreq/m_BufferSize);
        }
		else if( pRxSet->m_cAutoTS1 >= 0 ){
			pRxSet->m_cAutoTS1 = int(10 * m_SampleFreq/m_BufferSize);
        }
    }
    else {
		pRxSet->m_SQTimer = 0;
		if( pRxSet->m_cAutoTS1 ){
			if( pRxSet->m_cAutoTS1 > 0 ){
				pRxSet->m_cAutoTS1--;
            }
            else {
				pRxSet->m_cAutoTS1++;
            }
        }
    }
    if( pRxSet->m_cAutoTS2 ) pRxSet->m_cAutoTS2--;

	if( pRxSet->m_AFCTimerN ){
		pRxSet->m_AFCTimerN--;
        pRxSet->m_pDem->m_fEnableAFC = FALSE;
    }
	else if( pRxSet->Is170() ){
		pRxSet->m_pDem->m_fEnableAFC = pRxSet->m_SQ && (pRxSet->m_SQTimer > 4) && (pRxSet->m_StgFFT.Sig >= (600+500));
    }
    else {
		pRxSet->m_pDem->m_fEnableAFC = pRxSet->m_SQ;
    }

	if( pRxSet->m_StgFFT.Timer ){
		pRxSet->m_StgFFT.Timer--;
        return;
	}
    else {
	    pRxSet->m_StgFFT.Sig = (pRxSet->m_StgFFT.dBWMax - pRxSet->m_StgFFT.dBSum);
		pRxSet->m_fATC = (pRxSet->m_StgFFT.Sig >= ((m_ATCLevel*100)+300));
		m = pRxSet->Is170() ? (1700+300) : (2000+300);
        pRxSet->m_pDem->m_Decode.SetMeasClock((pRxSet->m_StgFFT.Sig >= m));
    }

	// 広域AFCの動作
	BOOL fAFC = pRxSet->m_pDem->m_fAFC && m_AFCWidth && (m_TX != txINTERNAL);
	if( fAFC ){
		switch(pRxSet->m_Mode){
			case MODE_N_BPSK:
			case MODE_BPSK:
			case MODE_GMSK:
        		sq = (pRxSet->m_SQTimer >= 8) ? pRxSet->m_AFCSQ : (d >= pRxSet->m_StgFFT.dBMax/3);
        		break;
			case MODE_FSKW:
			case MODE_FSK:
        		sq = (pRxSet->m_SQTimer >= 32) ? pRxSet->m_AFCSQ : (d >= pRxSet->m_StgFFT.dBMax/2);
        		break;
			case MODE_RTTY:
			case MODE_U_RTTY:
        		sq = (pRxSet->m_SQTimer >= 32) ? pRxSet->m_AFCSQ : (d >= pRxSet->m_StgFFT.dBMax/2);
				break;
			case MODE_mfsk_L:
			case MODE_mfsk_U:
        		sq = (pRxSet->m_SQTimer >= 8) ? pRxSet->m_AFCSQ : (d >= pRxSet->m_StgFFT.dBMax/2);
				break;
		}
		if( !sq ){
			if( (m_AFCWidth > 50) && !pRxSet->IsMFSK() ){
				xl = int((fo - m_AFCWidth) * m_FFT_SIZE / m_FFTSampFreq);
    			xh = int((fo + m_AFCWidth) * m_FFT_SIZE / m_FFTSampFreq);
				int o = (xh + xl)/2;
				int m = (xh - xl)/2;
				int rmax = 0;
				int lmax = 0;
				int rc = 0;
				int lc = 0;
				int lm = int(200 * m_FFT_SIZE / m_FFTSampFreq);
				for( i = 1; i < m; i++ ){
					if( (o - i) > lm ){
						y = m_fftout[o-i];
    					if( y > lmax ){
							lc = i;
            				lmax = y;
            			}
					}
					y = m_fftout[o+i];
					if( y > rmax ){
						rc = i;
						rmax = y;
					}
				}
				int th = ((m_AFCLevel*100) + 450);
                if( pRxSet->IsMFSK() && (th < 1650) ) th = 1650;
				lmax = int(AdjDB(lmax) - pRxSet->m_StgFFT.dBSum);
				rmax = int(AdjDB(rmax) - pRxSet->m_StgFFT.dBSum);
				if( pRxSet->IsRTTY() ){
					if( (lmax > th) && (rmax > th) ){
						o += (rc - lc);
					}
					else if( !sq ){
						if( lmax > th ){
							o -= lc;
						}
						else if( rmax > th ){
							o += rc;
						}
						else {
							o = 0;
						}
					}
					else {
						o = 0;
					}
				}
				else if( (lmax > th) && (rmax > th) ){
					if( (rc + lc) < 10 ){
						if( rmax > lmax ){
							o += rc;
       					}
       					else {
							o -= lc;
       					}
					}
					else {
						if( rc < lc ){
							o += rc;
       					}
       					else {
							o -= lc;
       					}
					}
				}
				else if( lmax > th ){
					o -= lc;
				}
				else if( rmax > th ){
					o += rc;
				}
				else {
					o = 0;
				}
				if( o ){
					int fq = int(o * m_FFTSampFreq / m_FFT_SIZE);
					o = fq - fo;
					m = ABS(o);
					if( m < m_AFCWidth ){
						int diff;
						BOOL bPSK = FALSE;
						switch(pRxSet->m_Mode){
							case MODE_N_BPSK:
							case MODE_BPSK:
                    			bPSK = TRUE;
        	        			diff = int(pRxSet->m_Speed*1.2);
								break;
							case MODE_GMSK:
        	        			diff = int(pRxSet->m_Speed);
								break;
                			case MODE_FSK:
        	        			diff = int(pRxSet->m_Speed*2);
                				break;
							case MODE_FSKW:
                			case MODE_RTTY:
                			case MODE_U_RTTY:
        	        			diff = int(pRxSet->m_pDem->m_RTTYShift);
                				break;
                            case MODE_mfsk_L:
								if( !m_MFSK_Center ) fq += int(pRxSet->m_pDem->m_MFSK_BW/2);
                            	diff = int(pRxSet->m_pDem->m_MFSK_BW*1.5);
                                break;
                            case MODE_mfsk_U:
								if( !m_MFSK_Center ) fq -= int(pRxSet->m_pDem->m_MFSK_BW/2);
                            	diff = int(pRxSet->m_pDem->m_MFSK_BW*1.5);
                            	break;
            			}
						if( !pRxSet->m_AFCTimerW && (pRxSet->m_AFCTimerW2 > 8) &&
                        	(ABS(pRxSet->m_AFCFQ - fq) < diff)
                        ){
							if( (m < 30) || (bPSK && (m < 50)) || (pRxSet->IsRTTY() && (m < 220)) ){
								fo = GetSignalFreq(fo+o, 50, pRxSet);
   								o = 0;
       						}
           	    			else if( m < 80 ){
								o /= 3;
							}
							else if( m < 160 ){
								o /= 8;
           					}
               				else {
								o /= 10;
							}
							pDem->SetCarrierFreq(fo + o);
       					}
						pRxSet->m_AFCTimerW2++;
						pRxSet->m_AFCFQ = int(pRxSet->m_AvgAFC.DoZ(fq));
					}
				}
				else {
					pRxSet->m_AFCTimerW2 -= pRxSet->IsRTTY() ? 3 : 2;
					if( pRxSet->m_AFCTimerW2 < 0 ) pRxSet->m_AFCTimerW2 = 0;
				}
			}
            else {
				pRxSet->m_AFCFQ = fo;
            }
		}
		else if( pRxSet->IsMFSK() ){
	        if( !fMFSKMet ){
				DoAFCMFSK(pRxSet, fo, TRUE);
        	}
        }
		else {
			double bw = pRxSet->GetBandWidth();
			fm = GetSignalFreq(fo, int(bw*0.75), pRxSet, pRxSet->m_SQ ? 600 : (m_AFCLevel*100)+450);
			pRxSet->m_AFCFQ = int(pRxSet->m_AvgAFC.DoZ(fm));
			if( pRxSet->m_SQ ){
				m = ABS(fo - pRxSet->m_AFCFQ);
		        fm = int(bw * 0.5);
   			    if( fm < m_AFCWidth ) fm = m_AFCWidth;
				if( (m >= int(bw*0.2)) && (m < fm) ){
					pRxSet->m_AFCTimerPSK++;
					if( pRxSet->m_AFCTimerPSK >= UINT(pRxSet->IsBPSK() ? 6 : 8) ){
						fm = int((pRxSet->m_AFCFQ - fo)*0.75);
						pDem->SetCarrierFreq(fo+fm);
					}
				}
				else {
					pRxSet->m_AFCTimerPSK = 0;
				}
            }
		}
	}
	else if( pRxSet->IsMFSK() ){
		DoAFCMFSK(pRxSet, fo, FALSE);
    }
    else {
		double bw = pRxSet->GetBandWidth();
		fm = GetSignalFreq(fo, int(bw*0.75), pRxSet, pRxSet->m_SQ ? 600 : (m_AFCLevel*100)+450);
		pRxSet->m_AFCFQ = int(pRxSet->m_AvgAFC.DoZ(fm));
    }
	pRxSet->m_AFCSQ = pRxSet->m_SQ;
    if( pRxSet->m_SQ ){
        pRxSet->m_AFCTimerW = int(3 * m_SampleFreq / m_BufferSize);
    }
	else if( pRxSet->m_AFCTimerW ){
    	pRxSet->m_AFCTimerW--;
        pRxSet->m_AFCTimerW2 = 0;
    }
    sum = int((pRxSet->m_StgFFT.Sig-500)*0.01);
	if( m_TX != txINTERNAL ){
		if( m_FFT.m_FFTGain ){
			if( sum >= 38 ){
				sum += int((sum - 38) * 1.75);
            }
        }
        else {
			if( sum >= 32 ){
				sum += int((sum - 32) * 0.357);
            }
        }
    }
    if( sum < 0 ) sum = 0;
    if( sum > 96 ) sum = 96;
    pRxSet->m_StgFFT.DispSig = sum;
	if( !m_TX ){
       	if( pRxSet->m_PeakSig < sum ) pRxSet->m_PeakSig = sum;
		if( pRxSet->m_SQ ) pRxSet->m_AvgSig.Do(sum);
    }
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::DoAFCMFSK(CRxSet *pRxSet, int fo, BOOL fUpdate)
{
	if( !m_AFCWidth ) return;

	int bw;
	int xl, xh, x, xx;
    CDEMFSK *pDem = pRxSet->m_pDem;

	if( m_MFSK_Center ){
		bw = int((pDem->m_MFSK_BW*0.5) + 0.5);
    	xl = fo - bw;
    	xh = fo + bw;
    }
    else {
		bw = int(pDem->m_MFSK_BW + 0.5);
		if( pRxSet->m_Mode == MODE_mfsk_U ){
			xl = fo; xh = fo + bw;
        }
        else {
			xl = fo - bw; xh = fo;
        }
    }
	xl = int((xl * m_FFT_SIZE / m_FFTSampFreq));
	xh = int((xh * m_FFT_SIZE / m_FFTSampFreq) + 0.5);
	int f;
    if( pRxSet->m_AFCTimerMFSK < MFSKAFC_2ndATACK ){
		if( pRxSet->m_AFCTimerMFSK < MFSKAFC_1stATACK ){
			f = m_AFCWidth;
   			if( f >= 32 ) f = 32;
	    }
    	else {
			f = int(pDem->m_MFSK_SPEED);
	    }
	    f = int(f * m_FFT_SIZE / m_FFTSampFreq);
		xl -= f; xh += f;
    }
    else {
		if( pDem->m_MFSK_TONES == 8 ){
			xl-=2; xh+=2;
	    }
    	else {
			xl--; xh++;
	    }
    }

	if( pRxSet->m_AFCTimerMFSK < MFSKAFC_MAX ){
		pRxSet->m_AFCTimerMFSK++;
        if( pDem->GetMFSKMetric(0) >= 800 ){
			pRxSet->m_AFCTimerMFSK+=8;
        }
    }

	int d;
    int avg = 0;
    int max = 0;
    xx = xl;
	for( x = xl; x <= xh; x++ ){
    	d = m_fftout[x];
        avg += d;
        if( max < d ){
        	max = d;
            xx = x;
        }
    }
    avg /= (xh - xl);
	int maxdb, avgdb;
    if( m_FFT.m_FFTGain ){
		avgdb = int(SqrtToDB(avg));
		maxdb = int(SqrtToDB(max));
    }
    else {
		double k = 100.0 / FFTSC;
        avgdb = int(avg * k);
        maxdb = int(max * k);
	}
	if( (maxdb - avgdb) < 750 ) return;

   	xl = m_fftout[xx] - m_fftout[xx-1];
   	xh = m_fftout[xx] - m_fftout[xx+1];
	if( (xl > 0) && (xh > 0) ){	// FFT間隔補正
        double off = double(xl - xh) * 0.5 / double(xl + xh);
		x = int(((xx + off) * m_FFTSampFreq / m_FFT_SIZE) + 0.5);
   	}
    else {
		x = int((xx * m_FFTSampFreq / m_FFT_SIZE) + 0.5);
    }
	f = 0;
	if( m_MFSK_Center ){
		if( x < (fo - bw) ){
			f = x + bw;
        }
        else if( x > (fo + bw) ){
			f = x - bw;
        }
    }
    else if( pRxSet->m_Mode == MODE_mfsk_U ){
		if( x < fo ){
			f = x;
        }
        else if( x > (fo + bw) ){
			f = x - bw;
        }
    }
    else {
		if( x < (fo - bw) ){
			f = x + bw;
        }
        else if( x > fo ){
			f = x;
        }
    }
	double fq;
    if( pDem->m_pMFSK->GetAFCShift(fq) ){
#if 0
		static int Cnt = 0;
		char bf[256];
		sprintf(bf, "%.1lf (%d)", fq, Cnt++);
		Application->MainForm->Caption = bf;
#endif
		fq += pDem->m_CarrierFreq;
		pRxSet->m_AvgAFC.Do(fq);
		pRxSet->m_AFCFQ = int(pRxSet->m_AvgAFC.Do(fq) + 0.5);
		pDem->m_RxFreq = pRxSet->m_AFCFQ;
		if( fUpdate ){
           	if( pRxSet->m_AFCFQ != int(pDem->m_CarrierFreq) ){
       			pDem->SetCarrierFreq(pRxSet->m_AFCFQ);
            }
        }
    }
    else if( f ){
		pRxSet->m_AFCFQ = int(pRxSet->m_AvgAFC.Do(f)+0.5);
		pDem->m_RxFreq = pRxSet->m_AFCFQ;
		if( fUpdate ){
           	if( pRxSet->m_AFCFQ != int(pDem->m_CarrierFreq) ){
	        	pDem->SetCarrierFreq(pRxSet->m_AFCFQ);
            }
        }
        else {
			pRxSet->m_AFCTimerMFSK = 0;
        }
    }
    else if( !fUpdate ){
		DoAvg(pDem->m_RxFreq, fo, (pRxSet->m_AFCTimerMFSK >= (15*4)) ? 0.1 : 0.005);
		pRxSet->m_AvgAFC.Do(pDem->m_RxFreq);
    }
}

//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::FFTSampleFreq(double fq)
{
    m_FFTSampFreq = fq * m_FFTFactor;
}

//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::SetPTT(BOOL fTX)
{
	if( m_bActive ) FireOnPTT(fTX);
	if( !m_pCom && strcmpi(m_PTTCOM, "NONE") && m_PTTLock ) OpenCom();
	if( m_pCom ) m_pCom->SetPTT(fTX);
    m_Wave.SetPTT(fTX);
}

//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::DoSetBPF(int n)
{
	m_nBPFType = n;
	if( !m_bActive ) return;
	int taps = g_tBpfTaps[n];
	CDEMFSK *pDem = m_RxSet[0].m_pDem;
    int delay = (pDem->m_inBPF.GetTap() - taps)/2;
    pDem->MakeBPF(taps);
    CRxSet *pRxSet = &m_RxSet[1];
	for( int i = 1; i < RXMAX; i++, pRxSet++ ){
		if( pRxSet->IsActive() ){
			pRxSet->m_pDem->MakeBPF(pDem->m_PreBPFTaps);
    	    pRxSet->m_pDem->m_pBPF = &pDem->m_inBPF;
        }
    }
	if( !m_Wave.IsInOpen() ) return;

    if( delay < 0 ){
		while( delay < 0 ){
			DoDem(0);
       	    delay++;
       	}
    }
   	else {
		m_SkipTmg = delay;
    }
}

//---------------------------------------------------------------------------
int __fastcall CMMVARICtrl::GetSignalFreq(int fo, int fm, CRxSet *pRxSet)
{
	return GetSignalFreq(fo, fm, pRxSet, 600);
}
//---------------------------------------------------------------------------
int __fastcall CMMVARICtrl::GetSignalFreq(int fo, int fm, CRxSet *pRxSet, int th)
{
	if( pRxSet->IsMFSK() ){
		return fo;
    }
	else if( pRxSet->Is170() ){
    	if( (pRxSet != &m_RxSet[0]) || pRxSet->m_fAFC ){
			fm = int(pRxSet->m_pDem->m_RTTYShift + 30);
        }
        else {
        	return fo;
        }
    }
	int xl = int((fo - fm) * m_FFT_SIZE / m_FFTSampFreq);
	int xh = int((fo + fm) * m_FFT_SIZE / m_FFTSampFreq);
	int x;

    if( xl < 0 ){
		x = xl;
        xl = 0;
        xh -= x;
    }

    if( xl >= xh ) return fo;

	int d;
    int avg = 0;
    int max = 0;
    for(x = xl; x < xh; x++ ){
    	d = m_fftout[x];
		avg += d;
        if( max < d ) max = d;
    }
    avg /= (xh - xl);
	int maxdb, avgdb;
    if( m_FFT.m_FFTGain ){
		avgdb = int(SqrtToDB(avg));
		maxdb = int(SqrtToDB(max));
    }
    else {
		double k = 100.0 / FFTSC;
        avgdb = int(avg * k);
        maxdb = int(max * k);
	}
	if( (maxdb - avgdb) >= th ){
		max = (max + (avg * 2)) / 3;
	    int fl = 0;
    	int fh = 0;
        for(x = xl; x <= xh; x++ ){
			if( m_fftout[x] >= max ){
				if( !fl ) fl = x;
                fh = x;
            }
        }
		if( pRxSet->Is170() ){
			xl = int((pRxSet->m_pDem->m_RTTYShift - 20) * m_FFT_SIZE/m_FFTSampFreq);
    	    xh = int((pRxSet->m_pDem->m_RTTYShift + 30) * m_FFT_SIZE/m_FFTSampFreq);
        }
        else {
			xl = 0;
    	    xh = int((pRxSet->m_Speed + 10) * m_FFT_SIZE/m_FFTSampFreq);
        }
       	max = ABS(fh-fl);
		if( ((max >= xl) && (max <= xh)) ){
	    	x = (fl + fh) / 2;
			fo = int(x * m_FFTSampFreq / m_FFT_SIZE);
        }
	}
    return fo;
}

//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::InitCollect(CRxSet *pRxSet, int n)
{
	if( pRxSet != &m_RxSet[0] ) return;

	if( n < 4 ) n = 4;
	if( n > 24 ) n = 24;
	m_WaveBitMax = n;
	InitCollect();
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::InitCollect(void)
{
	if( !m_bActive ) return;

	double speed = m_RxSet[0].m_Speed;
    if( m_RxSet[0].IsMFSK() ) speed = m_RxSet[0].m_pDem->m_MFSK_SPEED;
    m_Collect1.Create(int(m_DemSampleFreq*m_WaveBitMax/speed));
    m_Collect2.Create(int(m_DemSampleFreq*m_WaveBitMax/speed));
	if( m_RxSet[0].IsActive() ){
	    int n = m_WaveBitMax;
    	if( !(n & 1) ) n++;
	    m_RxSet[0].m_pDem->m_Decode.m_cBWave = n;
    }
}

//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::SetATCSpeed(int f)
{
	m_ATCSpeed = f;
    CRxSet *pRxSet = m_RxSet;
	for( int i = 0; i < RXMAX; i++, pRxSet++ ){
    	if( pRxSet->IsActive() ) pRxSet->m_pDem->m_Decode.SetATCSpeed(f);
    }
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::SetATCLimit(int f)
{
	m_ATCLimit = f;
    CRxSet *pRxSet = m_RxSet;
	for( int i = 0; i < RXMAX; i++, pRxSet++ ){
    	if( pRxSet->IsActive() ) pRxSet->m_pDem->m_Decode.SetATCLimit(f);
    }
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::UpdateCharset(int nIndex, short charset)
{
	CRxSet *pRxSet = &m_RxSet[nIndex];
	pRxSet->m_Charset = charset;
	pRxSet->m_MBCS.Create(BYTE(charset));
    pRxSet->m_fJA = (charset == SHIFTJIS_CHARSET);
	if( !nIndex ){
	    m_ModFSK.m_Encode.m_fJA = pRxSet->m_fJA;
	}
	else {
		return;
	}
#if 0
	LPCSTR pKey;
    switch(charset){
		case SHIFTJIS_CHARSET:
			pKey = "JA";
            m_fMBCS = TRUE;
			break;
        case JOHAB_CHARSET:
		case HANGEUL_CHARSET:
			pKey = "HL";
            m_fMBCS = TRUE;
			break;
		case CHINESEBIG5_CHARSET:   //
			pKey = "BV";
            m_fMBCS = TRUE;
			break;
		case 134:       // 簡略
			pKey = "BY";
            m_fMBCS = TRUE;
			break;
		default:
			pKey = "ANSI";
            m_fMBCS = FALSE;
			break;
	}
#endif
}

//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::OpenCom(void)
{
	if( m_nErrorCode == errorComOpen ) m_nErrorCode = 0;
	if( m_pCom ){
    	delete m_pCom;
        m_pCom = NULL;
    }
	if( strcmpi(m_PTTCOM, "NONE") ){
	    m_pCom = new CComm();
    	if( m_pCom->Open(m_PTTCOM, this) ){
			m_pCom->SetFSK(m_bCOMFSK, m_bCOMFSKINV);
			m_pCom->SetPTT(m_TX);
    	}
    	else {
			delete m_pCom;
    	    m_pCom = NULL;
			m_nErrorCode = errorComOpen;
			if( m_bActive ) FireOnError(m_nErrorCode);
    	}
    }
}

//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::CloseCom(void)
{
	if( m_pCom ){
    	delete m_pCom;
        m_pCom = NULL;
    }
}

//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::OpenAll(void)
{
	if( !m_RxSet[0].IsActive() ) m_RxSet[0].Create(FALSE, this);
	SampleFreq(m_SampleFreq);
	UpdateModGain();
	m_FFT.InitFFT(m_FFT_SIZE);
	memset(m_fftout, 0, sizeof(m_fftout));
	m_WaterSemaphore++;

	InitWFX();
	m_Dec2.SetSampleFreq(m_fDec, m_SampleFreq);
	m_ModFSK.m_Encode.m_pMainCtrl = this;
	m_ModFSK.m_Encode.m_BAUDOT.m_pMainCtrl = this;
	m_ModFSK.SetMFSKType(m_RxSet[0].m_MFSK_TYPE);
	m_ModFSK.SetSampleFreq(m_SampleFreq+m_TxOffset);
	m_RxSet[0].SetSampleFreq(m_DemSampleFreq);
	InitCollect(m_RxSet, m_RxSet[0].IsRTTY() ? 7 : 14);
	m_HPF.Create(ffHPF, 300, m_SampleFreq, 2, 1, 0.3);
	m_ModFSK.SetSpeed(m_RxSet[0].m_Speed);
	m_ModFSK.SetType(m_RxSet[0].m_Mode);
	m_RxSet[0].SetMFSKType(m_RxSet[0].m_MFSK_TYPE);
	m_RxSet[0].SetMode(m_RxSet[0].m_Mode);
	m_RxSet[0].m_pDem->SetSpeed(m_RxSet[0].m_Speed);
	m_RxSet[0].SetCarrierFreq(m_wRxCarrier);
	m_RxSet[0].m_pDem->m_Decode.m_fATC = m_bATC;
	m_ModFSK.SetCarrierFreq(m_wTxCarrier);
	SetATCSpeed(m_ATCSpeed);
	SetATCLimit(m_ATCLimit);
	m_Notches.Create();
	if( m_fPlayBack ) m_PlayBack.Init(m_BufferSize, m_SAMPBASE);

	for( int i = 0; i < RXMAX; i++ ){
		m_RxSet[i].m_AFCTimerW = int(3 * m_SampleFreq / m_BufferSize);
		m_RxSet[i].m_AFCTimerN = int(3 * m_SampleFreq / m_BufferSize);
	}

	UpdateCharset(0, m_RxSet[0].m_Charset);
	OpenCom();
	OpenSound(FALSE);
	DoSetBPF(m_nBPFType);
	FireOnTxCarrier(m_wTxCarrier);
	FireOnRxCarrier(0, m_wRxCarrier);
	FireOnMode(0, m_RxSet[0].m_Mode, g_tDispModeTable[m_RxSet[0].m_Mode]);
	FireOnSpeed(0, GetDispSpeed(0));
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::CloseAll(void)
{
	if( m_TX ) ToRX();
	m_Clock.Delete();
	m_Collect1.Delete(); m_Collect2.Delete();
	int i;
	for( i = 0; i < RXMAX; i++ ){
		m_RxSet[i].Delete();
	}
	CloseSound();
	CloseCom();
	m_PlayBack.Delete();		
}

//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::SetSampleFreq(double f, BOOL fForce)
{
	if( !fForce && (m_SampleFreq == f) ) return;

	if( !SampleFreq(f) ) return;
	if( !m_bActive ) return;
	BOOL fi = m_Wave.IsInOpen();
    BOOL fo = m_Wave.IsOutOpen();
    m_Wave.InClose(); m_Wave.OutAbort();
	InitWFX();
    m_Dec2.SetSampleFreq(m_fDec, m_SampleFreq);
	CRxSet *pRxSet = m_RxSet;
	for( int i = 0; i < RXMAX; i++, pRxSet++ ){
	    pRxSet->SetSampleFreq(m_DemSampleFreq);
		pRxSet->SetCollect(pRxSet->m_fCollectScope);
    }
	InitCollect();
	m_HPF.Create(ffHPF, 300, m_SampleFreq, 2, 1, 0.3);
    m_ModFSK.SetSampleFreq(m_SampleFreq+m_TxOffset);
	m_Notches.Create();
	if( m_fPlayBack ) m_PlayBack.Init(m_BufferSize, m_SAMPBASE);

    if( fi ) OpenSound(FALSE);
	if( fo ) ReOutOpen();
}

//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::SetTxOffset(double f)
{
	if( m_TxOffset == f ) return;

	m_TxOffset = f;
	if( !m_bActive ) return;
    m_ModFSK.SetSampleFreq(m_SampleFreq+m_TxOffset);
}

//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::SetMode(int nIndex, int mode)
{
	if( m_RxSet[nIndex].m_Mode != mode ){
		m_RxSet[nIndex].SetMode(mode);
		if( m_bActive ){
			if( !nIndex ) m_ModFSK.SetType(mode);
			FireOnMode(nIndex, mode, g_tDispModeTable[mode]);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::SetSpeed(int nIndex, double b)
{
	if( m_RxSet[nIndex].IsMFSK() ){
		m_RxSet[nIndex].SetMFSKType(MFSK_Speed2Type(b));
		if( m_bActive ){
			if( !nIndex ){
				m_ModFSK.SetMFSKType(MFSK_Speed2Type(b));
				InitCollect();
			}
			FireOnSpeed(nIndex, GetDispSpeed(nIndex));
		}
	}
	else if( (b >= MIN_SPEED) && (b <= MAX_SPEED) ){
		m_RxSet[nIndex].SetSpeed(b);
		if( m_bActive ){
			if( !nIndex ){
				m_ModFSK.SetSpeed(b);
				InitCollect();
			}
			FireOnSpeed(nIndex, b);
		}
	}
	else {
		if( m_bActive ) FireOnSpeed(nIndex, GetDispSpeed(nIndex));
	}
}
//---------------------------------------------------------------------------
BOOL __fastcall CMMVARICtrl::DoPlayBack(int s)
{
	if( !m_bActive || !m_PlayBack.IsActive() || m_TX ) return FALSE;

    if( s ){
		if( !m_PlayBack.StartPlaying(s) ){
			StopPlayBack();
			s = 0;
        }
		else {
			FireOnPlayBack(TRUE);
		}
    }
    else {
		StopPlayBack();
    }
	return s ? TRUE : FALSE;
}
//---------------------------------------------------------------------------
void __fastcall CMMVARICtrl::StopPlayBack(void)
{
	if( m_PlayBack.IsActive() ){
		m_PlayBack.StopPlaying();
		FireOnPlayBack(FALSE);
    }
}

void CMMVARICtrl::SetSQLevel(short nIndex, short nNewValue) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return;
	m_RxSet[nIndex].m_SQLevel = nNewValue;
}

//---------------------------------------------------------------------------
double __fastcall CMMVARICtrl::GetDispSpeed(int nIndex)
{
	CRxSet *pRxSet = &m_RxSet[nIndex];
	if( pRxSet->IsMFSK() ){
		double b;
		MFSK_SetPara(pRxSet->m_MFSK_TYPE, NULL, &b, NULL);
		return b;
	}
	else {
		return pRxSet->m_Speed;		
	}
}

double CMMVARICtrl::GetDblFreqErrorInternal(short nIndex)
{
	if( !m_bActive ) return 0.0;
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return 0.0;
	BOOL fTX = (m_TX == txINTERNAL);
	CRxSet *pRxSet = &m_RxSet[nIndex];
	if( !pRxSet->IsActive() ) return 0.0;
	if( !pRxSet->m_PFTimer ){
		CDEMFSK *pDem = pRxSet->m_pDem;
		double d = pDem->GetFreqErr();
		if( pRxSet->IsBPSK() ){
			if( m_AFCWidth && !fTX ){
				double f = double(pRxSet->m_AFCFQ - pRxSet->m_CarrierFreq)/pRxSet->GetBandWidth();
				if( fabs(f) >= 0.2 ) d = f;
			}
			d *= 2.0;
		}
		if( fTX ) d *= 0.05;
		return d;
	}
	else {
		pRxSet->m_PFTimer--;
		return 0.0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMMVARICtrl メッセージ ハンドラ
afx_msg LRESULT CMMVARICtrl::OnWaveMessage(WPARAM wParam, LPARAM lParam)
{
	switch(wParam){
		case waveIN:			// 入力イベント
			OnWaveIn();
        	break;
        case waveOUT:			// 出力イベント
			OnWaveOut();
        	break;
	}
	return 0;
}

//---------------------------------------------------------------------------
int __fastcall CMMVARICtrl::OnGetChar(void)
{
	short wChar = 0;
	FireOnGetTxChar(&wChar);
	if( wChar ){
		m_fSendChar = TRUE;
		return wChar & 0x0000ffff;
	}
	else {
		return -1;
	}
}

void CMMVARICtrl::OnTimer(UINT nIDEvent) 
{
	if( nIDEvent == m_nSoundTimerID ){	// サウンドタイマー
		if( m_fSuspend ) return;

		BOOL r;
		if( m_RecoverSoundMode ){
			r = ReOutOpen();
		}
		else {
			r = OpenSound(FALSE);
	    }
		if( r ){
			KillTimer(m_nSoundTimerID);
			m_nSoundTimerID = 0;
		}
		else {
			m_SoundMsgTimer -= intvalSOUND;
			if( m_SoundMsgTimer < 0 ){
				m_nErrorCode = errorSoundOpen;
				if( m_bActive ) FireOnError(m_nErrorCode);
	        }
			if( m_bActive ) FireOnDrawFFT(0, (long *)m_fftout);
		}
	}
	COleControl::OnTimer(nIDEvent);
}

BOOL CMMVARICtrl::DestroyWindow() 
{
	m_bActive = FALSE;
	CloseAll();
	return COleControl::DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
// CMMVARICtrl プロパティ・メソッド
void CMMVARICtrl::OnBActiveChanged() 
{
	if( m_bActive ){
		OpenAll();
	}
	else {
		CloseAll();
	}
	SetModifiedFlag();
}

void CMMVARICtrl::OnWCollectTypeChanged() 
{
	// TODO: ﾊﾝﾄﾞﾗ ｺｰﾄﾞを追加してください

	SetModifiedFlag();
}

void CMMVARICtrl::OnWTxCarrierChanged() 
{
	if( m_wTxCarrier < MIN_CARRIER ) m_wTxCarrier = MIN_CARRIER;
	if( m_wRxCarrier > m_MaxCarrier ) m_wTxCarrier = m_MaxCarrier;
	if( m_bActive && m_TX ){
		m_ModFSK.SetCarrierFreq(m_wTxCarrier);
		if( m_TX == txINTERNAL ) m_RxSet[0].SetCarrierFreq(m_wTxCarrier); 
	}
	if( m_bNET && (m_wTxCarrier != m_wRxCarrier) ){
		m_bNET = FALSE;
		if( m_bActive ) FireOnNET(m_bNET);
	}
	SetModifiedFlag();
	if( m_bActive ) FireOnTxCarrier(m_wTxCarrier);
}


void CMMVARICtrl::OnBNETChanged() 
{
    if( m_bNET && !m_TX ){
		SetTxCarrier(m_wRxCarrier);
    }
	if( m_bActive ) FireOnNET(m_bNET);
	SetModifiedFlag();
}

void CMMVARICtrl::OnBAddStartCRChanged() 
{
	SetModifiedFlag();
}

void CMMVARICtrl::OnBAddStopCRChanged() 
{
	SetModifiedFlag();
}

void CMMVARICtrl::OnDwATCChanged() 
{
	if( m_bActive ){
		if( m_TX == txINTERNAL ) return;
		if( m_RxSet[0].IsRTTY() ) return;
		if(	m_RxSet[0].IsMFSK() ) return;
		m_RxSet[0].m_pDem->m_Decode.SetTmg(m_dwATC);
	}
	SetModifiedFlag();
}

void CMMVARICtrl::OnBATCChanged() 
{
	if( m_bActive ){
		if( m_TX == txINTERNAL ) return;
		if( m_RxSet[0].IsRTTY() ) return;
		if( m_RxSet[0].IsMFSK() ) return;
		m_RxSet[0].m_pDem->m_Decode.m_fATC = m_bATC;
	}
	SetModifiedFlag();
}


BOOL CMMVARICtrl::GetBTX() 
{
	return m_TX;
}

void CMMVARICtrl::SetBTX(BOOL bNewValue) 
{
	if( !m_bActive ) return;
	if( bNewValue ){
		if( m_TX ) return;
		ToTX(FALSE);
	}
	else {
		if( !m_TX ) return;
		ToRX();
	}
	SetModifiedFlag();
}

double CMMVARICtrl::GetDblSampleFreq() 
{
	return m_SampleFreq;
}

void CMMVARICtrl::SetDblSampleFreq(double newValue) 
{
	SetSampleFreq(newValue, FALSE);
	SetModifiedFlag();
}

long CMMVARICtrl::GetDwSampleBase() 
{
	return m_SAMPBASE;
}

double CMMVARICtrl::GetDblFFTSampleFreq() 
{
	return m_FFTSampFreq;
}

double CMMVARICtrl::GetDblTxOffset() 
{
	return m_TxOffset;
}

void CMMVARICtrl::SetDblTxOffset(double newValue) 
{
	SetTxOffset(newValue);
	SetModifiedFlag();
}

short CMMVARICtrl::GetWModeMax() 
{
	return MODE_END;
}

BSTR CMMVARICtrl::GetStrModeList(short nIndex) 
{
	CString strResult;

	if( (nIndex >= 0) && (nIndex < MODE_END) ){
		strResult = g_tDispModeTable[nIndex];
	}
	return strResult.AllocSysString();
}


BSTR CMMVARICtrl::GetStrMode(short nIndex) 
{
	CString strResult;
	if( (nIndex >= 0) && (nIndex < RXMAX) ){
		strResult = g_tDispModeTable[m_RxSet[nIndex].m_Mode];
	}
	return strResult.AllocSysString();
}

void CMMVARICtrl::SetStrMode(short nIndex, LPCTSTR lpszNewValue) 
{
	if( (nIndex >= 0) && (nIndex < RXMAX) ){
		int mode = FindStringTableStrictly(g_tDispModeTable, lpszNewValue, MODE_END);
		if( mode >= 0 ){
			SetMode(nIndex, mode);
			SetModifiedFlag();
		}
	}
}

BOOL CMMVARICtrl::GetBReqRX() 
{
	return m_fReqRX;
}

void CMMVARICtrl::SetBReqRX(BOOL bNewValue) 
{
	if( m_TX ){
		if( bNewValue ){
			if( !m_fReqRX ){
				m_fReqRX = 1;
				m_ModFSK.m_Encode.m_fReqRX = TRUE;
				SetTxState(txstateREQRX);
			}
		}
		else {
			if( m_fReqRX == 1 ){
				m_fReqRX = 0;
				SetTxState(txstateREQRX);
			}
		}
	}
	else {
		m_fReqRX = bNewValue ? 1 : 0;
		m_ModFSK.m_Encode.m_fReqRX = TRUE;
	}
	SetModifiedFlag();
}

double CMMVARICtrl::GetDblSpeed(short nIndex) 
{
	if( (nIndex >= 0) && (nIndex < RXMAX) ){
		return GetDispSpeed(nIndex);
	}
	else {
		return 0.0;
	}
}

void CMMVARICtrl::SetDblSpeed(short nIndex, double newValue) 
{
	SetSpeed(nIndex, newValue);
	SetModifiedFlag();
}

short CMMVARICtrl::GetNBPF() 
{
	return m_nBPFType;
}

void CMMVARICtrl::SetNBPF(short nNewValue) 
{
	if( (nNewValue >= 0) && (nNewValue <= 3) ){
		DoSetBPF(nNewValue);
		SetModifiedFlag();
	}
}

short CMMVARICtrl::GetWRxCarrier(short nIndex) 
{
	if( nIndex == 0 ){
		return m_wRxCarrier;
	}
	else if( (nIndex >= 1) && (nIndex < RXMAX) ){
		return m_RxSet[nIndex].m_CarrierFreq;
	}
	else {
		return 0;
	}
}

void CMMVARICtrl::SetWRxCarrier(short nIndex, short nNewValue) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return;

	if( (nNewValue >= MIN_CARRIER) && (nNewValue <= m_MaxCarrier) ){
		m_RxSet[nIndex].SetCarrierFreq(nNewValue);
		if( !nIndex ){
			m_wRxCarrier = nNewValue;
			if( m_bActive && !m_TX && m_bNET ){
				SetTxCarrier(m_wRxCarrier);
			}
	        m_RxSet[0].m_AFCTimerW = 1;
		    m_RxSet[0].m_SQTimer = 0;
		}
		if( m_bActive ) FireOnRxCarrier(nIndex, nNewValue);
		SetModifiedFlag();
	}
}


short CMMVARICtrl::GetBRxEnabled(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return 0;

	return m_RxSet[nIndex].IsActive();
}

void CMMVARICtrl::SetBRxEnabled(short nIndex, short nNewValue) 
{
	if( nIndex == 0 ){
		if( m_bActive != nNewValue ){
			m_bActive = nNewValue;
			OpenAll();
			SetModifiedFlag();
			return;
		}
	}
	else if( (nIndex < 1) || (nIndex >= RXMAX) ) return;

	CRxSet *pRxSet = &m_RxSet[nIndex];
	if( nNewValue ){
		if( !m_bActive ){
			m_bActive = nNewValue;
			OpenAll();
		}
		if( !pRxSet->IsActive() ){
			pRxSet->Create(TRUE, this);
			UpdateCharset(nIndex, pRxSet->m_Charset);
			FireOnRxCarrier(nIndex, pRxSet->m_CarrierFreq);
			FireOnMode(nIndex, pRxSet->m_Mode, g_tDispModeTable[pRxSet->m_Mode]);
			FireOnSpeed(nIndex, GetDispSpeed(nIndex));
		}
	}
	else {
		if( pRxSet->IsActive() ){
			pRxSet->Delete();
		}
	}
	m_nSubWindow = 0;
	for( int i = RXMAX - 1; i >= 1; i-- ){
		if( m_RxSet[i].IsActive() ){
			m_nSubWindow = i;
			break;
		}
	}
	SetModifiedFlag();
}

short CMMVARICtrl::GetWSQLevel(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return 0;
	return m_RxSet[nIndex].m_SQLevel;
}

void CMMVARICtrl::SetWSQLevel(short nIndex, short nNewValue) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return;
	m_RxSet[nIndex].m_SQLevel = nNewValue;
	SetModifiedFlag();
}

BSTR CMMVARICtrl::GetStrSoundID() 
{
	CString strResult;
	StrSoundID(strResult);
	return strResult.AllocSysString();
}

void CMMVARICtrl::SetStrSoundID(LPCTSTR lpszNewValue) 
{
	SetSoundCard(m_Wave.m_SoundStereo, lpszNewValue);
	SetModifiedFlag();
}

short CMMVARICtrl::GetWSoundCH() 
{
	return m_Wave.m_SoundStereo;
}

void CMMVARICtrl::SetWSoundCH(short nNewValue) 
{
	if( (nNewValue >= 0) && (nNewValue <= 2) ){
		SetSoundCard(nNewValue, NULL);
		SetModifiedFlag();
	}
}

short CMMVARICtrl::GetWSoundRxFIFO() 
{
	return m_Wave.m_InFifoSize;
}

void CMMVARICtrl::SetWSoundRxFIFO(short nNewValue) 
{
	if( (nNewValue >= 4) && (nNewValue <= WAVE_FIFO_MAX) ){
		SetRXFifo(nNewValue);
		SetModifiedFlag();
	}
}

short CMMVARICtrl::GetWSoundTxFIFO() 
{
	return m_Wave.m_OutFifoSize;
}

void CMMVARICtrl::SetWSoundTxFIFO(short nNewValue) 
{
	if( (nNewValue >= 4) && (nNewValue <= WAVE_FIFO_MAX) ){
		SetTXFifo(nNewValue);
		SetModifiedFlag();
	}
}

void CMMVARICtrl::SendTone() 
{
	if( m_bActive && (!m_TX || !m_fTone) ) ToTone();
}

short CMMVARICtrl::GetWModGain() 
{
	return m_ModGain;
}

void CMMVARICtrl::SetWModGain(short nNewValue) 
{
	if( nNewValue >= 0 ){
		m_ModGain = nNewValue;
		UpdateModGain();
		SetModifiedFlag();
	}
}

short CMMVARICtrl::GetBSQ(short nIndex) 
{
	if( (nIndex < 1) || (nIndex >= RXMAX) ) return 0;
	return m_RxSet[nIndex].m_SQ;
}

short CMMVARICtrl::GetWSN(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return 0;
	return m_RxSet[nIndex].m_StgFFT.DispSig;
}

long CMMVARICtrl::GetDwHandle() 
{
	return long(this);
}

BSTR CMMVARICtrl::GetStrVersion() 
{
	CString strResult;
	char bf[128];
	sprintf(bf, "%u.%02u", _wVerMajor, _wVerMinor);
	strResult = bf;
	return strResult.AllocSysString();
}

short CMMVARICtrl::GetWAFCWidth() 
{
	return m_AFCWidth;
}

void CMMVARICtrl::SetWAFCWidth(short nNewValue) 
{
	if( (nNewValue >= 0) && (nNewValue <= 3000) ){
		m_AFCWidth = nNewValue;
		SetModifiedFlag();
	}
}

short CMMVARICtrl::GetWAFCLevel() 
{
	return m_AFCLevel;
}

void CMMVARICtrl::SetWAFCLevel(short nNewValue) 
{
	if( (nNewValue >= 3) && (nNewValue <= 60) ){
		m_AFCLevel = nNewValue;
		SetModifiedFlag();
	}
}
void CMMVARICtrl::OnStatLostRXChanged() 
{
	SetModifiedFlag();
}

void CMMVARICtrl::OnStatLostTXChanged() 
{
	SetModifiedFlag();
}

short CMMVARICtrl::GetStatSoundOpen() 
{
	short r = 0;
	if( m_Wave.IsInOpen() ) r |= 1;
	if( m_Wave.IsOutOpen() ) r |= 2;
	return r;
}

short CMMVARICtrl::GetStatIsPlaying() 
{
	return m_PlayBack.IsActive() && m_PlayBack.IsPlaying();
}

BOOL CMMVARICtrl::PlayBack(short wSec) 
{
	return DoPlayBack(wSec);
}

void CMMVARICtrl::SetTX(short wCmd) 
{
	if( !m_bActive ) return;

	switch(wCmd){
	case cmdtxTX:
		if( !m_TX || m_fTone ) ToTX(FALSE);
		break;
	case cmdtxREQRX:
		m_fReqRX = TRUE;
		break;
	case cmdtxCW:
		if( !m_TX || m_fTone ) ToTX(TRUE);
		break;
	case cmdtxTONE:
		if( !m_TX || !m_fTone ) ToTone();
		break;
	default:
		if( m_TX ) ToRX();
		break;
	}
}

short CMMVARICtrl::SendText(LPCTSTR strText) 
{
	if( !m_bActive ) return 0;

	LPCSTR p;
	int k = 0;
	for( p = strText; *p; p++ ){
		if( k ){
			k |= BYTE(*p);
			m_ModFSK.m_Encode.PutChar(k);
			k = 0;
		}
		else if( m_RxSet[0].m_MBCS.IsLead(BYTE(*p)) ){
			k = BYTE(*p);
			k = k << 8;
		}
		else {
			m_ModFSK.m_Encode.PutChar(BYTE(*p));
		}
	}
	return m_ModFSK.m_Encode.GetBuffCount();
}

short CMMVARICtrl::SendCWID(LPCTSTR strText) 
{
	if( !m_bActive ) return 0;

	LPCSTR p;
	int k = 0;
	for( p = strText; *p; p++ ){
		if( k ){
			k = 0;
		}
		else if( m_RxSet[0].m_MBCS.IsLead(BYTE(*p)) ){
			k = BYTE(*p);
			k = k << 8;
		}
		else {
			m_ModFSK.m_Encode.PutChar(BYTE(*p) | 0x0100);
		}
	}
	return m_ModFSK.m_Encode.GetBuffCount();
}

BSTR CMMVARICtrl::GetSendText() 
{
	CString strResult;
	m_ModFSK.m_Encode.GetSendText(strResult);
	return strResult.AllocSysString();
}


void CMMVARICtrl::AdjustClock(short wWidth) 
{
	if( m_bActive ){
		if( wWidth ){
			m_Clock.Create(wWidth);
		}
		else {
			m_Clock.Delete();
		}
	}
}

void CMMVARICtrl::SetClockAdjust(short wTone, double dblSampleFreq) 
{
	m_Clock.SetToneFreq(wTone);
	m_Clock.SetSampleFreq(dblSampleFreq);
}

short CMMVARICtrl::GetWCharset(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return 0;
	return m_RxSet[nIndex].m_Charset;
}

void CMMVARICtrl::SetWCharset(short nIndex, short nNewValue) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return;

	UpdateCharset(nIndex,  nNewValue);
	SetModifiedFlag();
}

BSTR CMMVARICtrl::GetVariType(short nIndex) 
{
	CString strResult;
	if( (nIndex >= 0) && (nIndex < RXMAX) ){
		strResult = ::GetVariType(BYTE(m_RxSet[nIndex].m_Charset), m_RxSet[nIndex].m_Mode);
	}
	return strResult.AllocSysString();
}

short CMMVARICtrl::GetWChannels() 
{
	return RXMAX;
}

BOOL CMMVARICtrl::GetBLoopExternal() 
{
	return (m_LoopBack == loopEXTERNAL);
}

void CMMVARICtrl::SetBLoopExternal(BOOL bNewValue) 
{
	int loopback = bNewValue ? loopEXTERNAL : loopINTERNAL;
	if( loopback != m_LoopBack ){
		m_LoopBack = loopback;
		if( m_bActive && m_TX ){
			m_Wave.OutAbort();
			ReOutOpen();
		}
		SetModifiedFlag();
	}
}

short CMMVARICtrl::GetWBufferCount() 
{
	return m_ModFSK.m_Encode.GetBuffCount();
}

void CMMVARICtrl::SetWBufferCount(short nNewValue) 
{
	if( nNewValue < m_ModFSK.m_Encode.GetBuffCount() ){
		m_ModFSK.m_Encode.SetBufferCount(nNewValue);
		SetModifiedFlag();
	}
}

BSTR CMMVARICtrl::GetStrPTTPort() 
{
	CString strResult;
	strResult = m_PTTCOM;
	return strResult.AllocSysString();
}

void CMMVARICtrl::SetStrPTTPort(LPCTSTR lpszNewValue) 
{
	if( strcmpi(m_PTTCOM, lpszNewValue) ){
		CloseCom();
		m_PTTCOM = lpszNewValue;
		if( m_bActive ) OpenCom();
		SetModifiedFlag();
	}
}

BOOL CMMVARICtrl::GetBPTTLock() 
{
	return m_PTTLock;
}

void CMMVARICtrl::SetBPTTLock(BOOL bNewValue) 
{
	if( m_PTTLock != bNewValue ){
		CloseCom();
		m_PTTLock = bNewValue;
		if( m_bActive ) OpenCom();
		SetModifiedFlag();
	}
}

void CMMVARICtrl::AttachLongArray(long FAR* pDist, long FAR* pSrc, short wSize) 
{
	memcpy(pDist, pSrc, sizeof(long)*wSize);
}

short CMMVARICtrl::GetBAFC(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return 0;
	return m_RxSet[nIndex].m_fAFC;
}

void CMMVARICtrl::SetBAFC(short nIndex, short nNewValue) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return;
	CRxSet *pRxSet = &m_RxSet[nIndex];
	pRxSet->m_fAFC = nNewValue ? 1 : 0;
	if( pRxSet->IsActive() ){
		CDEMFSK *pDem = pRxSet->m_pDem;
		pDem->m_fAFC = pRxSet->m_fAFC;
		if( pDem->m_fAFC && pRxSet->IsMFSK() ){
			pRxSet->m_AFCFQ = int(pDem->m_CarrierFreq);
			pRxSet->m_AvgAFC.Reset(pDem->m_CarrierFreq);
			pDem->m_RxFreq = pDem->m_CarrierFreq;
			if( pRxSet->m_AFCTimerMFSK < MFSKAFC_1stATACK ){
				pRxSet->m_AFCTimerMFSK = MFSKAFC_1stATACK;
			}
        }
	}
	SetModifiedFlag();
}

short CMMVARICtrl::GetWFFTType() 
{
	return m_FFT.m_FFTGain;
}

void CMMVARICtrl::SetWFFTType(short nNewValue) 
{
	if( m_FFT.m_FFTGain != nNewValue ){
		m_FFT.m_FFTGain = nNewValue;
		for( int i = 0; i < RXMAX; i++ ){
			m_RxSet[i].m_StgFFT.Timer = int(m_SampleFreq/m_BufferSize);
			m_RxSet[i].m_AFCTimerW = int(m_SampleFreq / m_BufferSize);
		}
		SetModifiedFlag();
	}
}

BOOL CMMVARICtrl::GetBNotch() 
{
	return m_Notches.m_Count;
}

void CMMVARICtrl::SetBNotch(BOOL bNewValue) 
{
	if( bNewValue ){
		if( !m_Notches.m_Count ) m_Notches.Add(m_NotchFreq, TRUE);
	}
	else {
		if( m_Notches.m_Count ) m_Notches.Delete();
	}
	SetModifiedFlag();
}

short CMMVARICtrl::GetWNotchFreq() 
{
	return m_NotchFreq;
}

void CMMVARICtrl::SetWNotchFreq(short nNewValue) 
{
	if( (nNewValue >= 100) && (nNewValue < m_MaxCarrier) ){
		m_NotchFreq = nNewValue;
		if( m_bActive && m_Notches.m_Count ) m_Notches.SetFreq(0, m_NotchFreq);
		SetModifiedFlag();
	}
}

short CMMVARICtrl::GetBSync(short nIndex) 
{
	if( !m_bActive ) return 0;
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return 0;
	CRxSet *pRxSet = &m_RxSet[nIndex];
	return (pRxSet->m_pDem->GetSyncState() && pRxSet->m_SQ);
}

double CMMVARICtrl::GetDblFreqError(short nIndex)
{
	return GetDblFreqErrorInternal(nIndex);
}

double CMMVARICtrl::GetDblBandWidth(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return 0.0;
	
	CRxSet *pRxSet = &m_RxSet[nIndex];
	return pRxSet->GetBandWidth();
}

short CMMVARICtrl::GetWCWSpeed() 
{
	return m_ModFSK.GetCWSpeed();
}

void CMMVARICtrl::SetWCWSpeed(short nNewValue) 
{
	if( (nNewValue >= 10) && (nNewValue <= 60) ){
		m_ModFSK.SetCWSpeed(nNewValue);
	}
	SetModifiedFlag();
}

BOOL CMMVARICtrl::GetBTreatCenterMFSK() 
{
	return m_MFSK_Center;
}

void CMMVARICtrl::SetBTreatCenterMFSK(BOOL bNewValue) 
{
	if( bNewValue != m_MFSK_Center ){
		m_MFSK_Center = bNewValue;
		SetSampleFreq(m_SampleFreq, FALSE);
		SetModifiedFlag();
	}
}

BOOL CMMVARICtrl::GetBMetricSqMFSK() 
{
	return m_MFSK_SQ_Metric;
}

void CMMVARICtrl::SetBMetricSqMFSK(BOOL bNewValue) 
{
	if( m_MFSK_SQ_Metric != bNewValue ){
		m_MFSK_SQ_Metric = bNewValue;
		SetModifiedFlag();
	}
}

short CMMVARICtrl::GetWMetricMFSK(short nIndex, short nPhase) 
{
	if( !m_bActive ) return 0;
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return 0;

	CRxSet *pRxSet = &m_RxSet[nIndex];
	return pRxSet->m_pDem->GetMFSKMetric(nPhase);
}

short CMMVARICtrl::GetWBufferMax() 
{
	return AN(m_ModFSK.m_Encode.m_Buff);
}

short CMMVARICtrl::GetWDiddleRTTY() 
{
	return m_ModFSK.m_Encode.GetDiddle();
}

void CMMVARICtrl::SetWDiddleRTTY(short nNewValue) 
{
	if( (nNewValue >= 0) && (nNewValue <= 1) ){
		m_ModFSK.m_Encode.SetDiddle(nNewValue);
		SetModifiedFlag();
	}
}

short CMMVARICtrl::GetBUOS(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return 0;
	
	CRxSet *pRxSet = &m_RxSet[nIndex];
	return pRxSet->IsActive() ? pRxSet->m_pDem->m_Decode.GetUOS() : TRUE;
}

void CMMVARICtrl::SetBUOS(short nIndex, short nNewValue) 
{
	if( !m_bActive ) return;
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return;
	
	CRxSet *pRxSet = &m_RxSet[nIndex];
	if( pRxSet->IsActive() ) pRxSet->m_pDem->m_Decode.SetUOS(nNewValue);
	SetModifiedFlag();
}

BOOL CMMVARICtrl::GetBPlayBack() 
{
	return m_fPlayBack;
}

void CMMVARICtrl::SetBPlayBack(BOOL bNewValue) 
{
	m_fPlayBack = bNewValue;
	if( m_bActive && m_fPlayBack && !m_PlayBack.IsActive() ){
		m_PlayBack.Init(m_BufferSize, m_SAMPBASE);
	}
	else {
		m_PlayBack.Delete();		
	}
	SetModifiedFlag();
}


short CMMVARICtrl::GetWLang() 
{
	return sys.m_wLang;
}

short CMMVARICtrl::GetWDefaultCharset() 
{
	return sys.m_BaseCharset;
}

double CMMVARICtrl::GetDblRxShift(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return 0.0;
	CRxSet *pRxSet = &m_RxSet[nIndex];
	return pRxSet->m_RxShift;	
}

void CMMVARICtrl::SetDblRxShift(short nIndex, double newValue) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return;
	if( (newValue < 10.0) || (newValue > 450.0) ) return;
	CRxSet *pRxSet = &m_RxSet[nIndex];
	pRxSet->SetRxShift(newValue);
	SetModifiedFlag();
}

double CMMVARICtrl::GetDblTxShift() 
{
	return m_ModFSK.m_RTTYShift;
}

void CMMVARICtrl::SetDblTxShift(double newValue) 
{
	if( (newValue >= 10.0) && (newValue <= 450.0) ){
		m_ModFSK.SetRTTYShift(newValue);
		SetModifiedFlag();
	}
}

short CMMVARICtrl::GetWMode(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return sys.m_DefaultMode;
	return m_RxSet[nIndex].m_Mode;
}

void CMMVARICtrl::SetWMode(short nIndex, short nNewValue) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return;
	if( (nNewValue < 0) || (nNewValue >= MODE_END) ) return;

	CRxSet *pRxSet = &m_RxSet[nIndex];
	if( nNewValue != pRxSet->m_Mode ){
		SetMode(nIndex, nNewValue);
		SetModifiedFlag();
	}
}

short CMMVARICtrl::GetWFFTWidth() 
{
	return m_FFT_SIZE;
}


short CMMVARICtrl::GetWTxState() 
{
	return m_TxState;
}

short CMMVARICtrl::SendChar(short wChar) 
{
	if( !m_bActive ) return 0;

	m_ModFSK.m_Encode.PutChar(wChar);
	return m_ModFSK.m_Encode.GetBuffCount();
}

short CMMVARICtrl::GetWWaveMax() 
{
	return m_Collect1.GetMax();
}

BOOL CMMVARICtrl::CreateVaricodeList(LPCTSTR strName) 
{
	return g_VariCode.SaveTable(strName);
}

short CMMVARICtrl::GetBCodeMM(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return 0;
	CRxSet *pRxSet = &m_RxSet[nIndex];

	short r = FALSE;
	switch(pRxSet->m_Mode){
		case MODE_GMSK:
        case MODE_FSK:
        case MODE_FSKW:
        case MODE_BPSK:
			switch(pRxSet->m_Charset){
				case SHIFTJIS_CHARSET:
				case HANGEUL_CHARSET:
				case CHINESEBIG5_CHARSET:   //
				case 134:       // 簡略
                	r = TRUE;
                    break;
				default:
   			        break;
		    }
        	break;
        default:
			break;
    }
    return r;
}

short CMMVARICtrl::GetStatComOpen() 
{
	if( m_pCom != NULL ){
		return TRUE;
	}
	else if( m_bActive && strcmpi(m_PTTCOM, "NONE") ){
		return -1;
	}
	else {
		return 0;
	}
}

BOOL CMMVARICtrl::GetBHPF() 
{
	return m_fHPF;
}

void CMMVARICtrl::SetBHPF(BOOL bNewValue) 
{
	m_fHPF = bNewValue;
	SetModifiedFlag();
}

#if 0
short CMMVARICtrl::GetBCollectScope(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return 0;
	CRxSet *pRxSet = &m_RxSet[nIndex];
	return pRxSet->m_fCollectScope;
}

void CMMVARICtrl::SetBCollectScope(short nIndex, short nNewValue) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return;
	CRxSet *pRxSet = &m_RxSet[nIndex];
	pRxSet->SetCollect(nNewValue);
	SetModifiedFlag();
}
#endif

short CMMVARICtrl::GetBPTTLines(short nLine) 
{
	if( (nLine < 0) || (nLine >= pttlineEND) ) return 0;
	return m_bPTTLines[nLine];
}

void CMMVARICtrl::SetBPTTLines(short nLine, short nNewValue) 
{
	if( (nLine < 0) || (nLine >= pttlineEND) ) return;
	m_bPTTLines[nLine] = nNewValue;
	if( m_pCom ) m_pCom->SetPTT(m_pCom->m_ptt);
	SetModifiedFlag();
}

short CMMVARICtrl::GetBOverLevel(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return 0;
	CRxSet *pRxSet = &m_RxSet[nIndex];
	return pRxSet->m_fOver;
}

short CMMVARICtrl::GetWNotches(short nIndex) 
{
	if( (nIndex < 0) && (nIndex >= m_Notches.m_Count) ) return 0;
	return m_Notches.m_pBase[nIndex].Freq;
}

void CMMVARICtrl::SetWNotches(short nIndex, short nNewValue) 
{
	if( nIndex < 0 ) return;
	if( nNewValue ){
		if( nIndex >= m_Notches.m_Count ){
			m_Notches.Add(nNewValue, TRUE);
		}
		else {
			m_Notches.SetFreq(nIndex, nNewValue);
		}
	}
	else if( nIndex < m_Notches.m_Count ){
		m_Notches.Delete(nIndex);		
	}
	SetModifiedFlag();
}

BOOL CMMVARICtrl::CreateCustomFilter(short nTaps, double FAR* pCoeff) 
{
	if( nTaps < 2 ) return FALSE;
	m_Notches.Delete();
	m_Notches.m_FIR.Create(nTaps, m_Notches.m_SampleFreq);
	memcpy(m_Notches.m_FIR.GetHP(), pCoeff, (nTaps+1) * sizeof(double));
	return TRUE;
}


double CMMVARICtrl::GetDblCustomSampleFreq() 
{
	return m_Notches.m_SampleFreq;
}

void CMMVARICtrl::DeleteCustomFilter() 
{
	if( !m_Notches.m_Count ) m_Notches.m_FIR.Delete();
}

short CMMVARICtrl::GetWNumNotches() 
{
	return m_Notches.m_Count;
}

short CMMVARICtrl::FindNotchFreq(short wFreq) 
{
	return m_Notches.Find(wFreq);
}

void CMMVARICtrl::AddNotchFreq(short wFreq) 
{
	m_Notches.Add(wFreq, FALSE);
}

void CMMVARICtrl::DeleteNotch(short nIndex) 
{
	if( nIndex >= 0 ){
		m_Notches.Delete(nIndex);
	}
	else {
		m_Notches.Delete();
	}
}

BOOL CMMVARICtrl::GetBCustomFilter() 
{
	return !m_Notches.m_Count && m_Notches.m_FIR.IsActive();
}

short CMMVARICtrl::GetWNotchTaps() 
{
	return m_Notches.m_nBaseTaps;
}

void CMMVARICtrl::SetWNotchTaps(short nNewValue) 
{
	if( (nNewValue < 2) || (nNewValue > 2048) ) return;
	m_Notches.m_nBaseTaps = (nNewValue + 1) & 2046;
	if( m_Notches.m_Count ) m_Notches.Create();
	SetModifiedFlag();
}

BOOL CMMVARICtrl::CalcCustomCharacteristic(double FAR* pAmp, double FAR* pPhase, short wWidth, short wMaxFreq) 
{
	CFIR2 *pFIR = &m_Notches.m_FIR;
	if( m_Notches.m_Count || !pFIR->IsActive() ) return FALSE;
	int taps = pFIR->GetTap();
	if( !taps ) return FALSE;

	double fs = pFIR->GetSampleFreq();
	double fk = wMaxFreq / double(wWidth);
	double f = 0.0;
	double ra, im, g;
	double pi2t = 2.0 * PI / fs;
	int i, k;
	double *pH = pFIR->GetHP();
	for( i = 0; i < wWidth; i++, f += fk ){
		ra = im = 0.0;
		for( k = 0; k <= taps; k++ ){
			ra += pH[k] * cos(pi2t*f*k);
			if( k ) im -= pH[k] * sin(pi2t*f*k);
		}
		if( ra * im ){
			g = sqrt(ra * ra + im * im);
		}
		else {
			g = 0.0;
		}
		*pAmp++ = g;
		double ph = 0;
        if( ra ) ph = atan2(im, ra);
		*pPhase++ = ph;
	}
	return TRUE;
}

short CMMVARICtrl::GetWRxCenter(short nIndex) 
{
	CRxSet *pRxSet = &m_RxSet[nIndex];
	int RxCarrier = 0;
	if( nIndex == 0 ){
		RxCarrier = m_wRxCarrier;
	}
	else if( (nIndex >= 1) && (nIndex < RXMAX) ){
		RxCarrier = pRxSet->m_CarrierFreq;
	}
	if( RxCarrier && !m_MFSK_Center && IsMFSK(pRxSet->m_Mode) ){
		int bw = int(pRxSet->GetBandWidth() * 0.5);
		switch(pRxSet->m_Mode){
		case MODE_mfsk_L:
			RxCarrier -= bw;
			break;
		case MODE_mfsk_U:
			RxCarrier += bw;
			break;
		default:
			break;
		}
	}
	return RxCarrier;
}

BOOL CMMVARICtrl::CreateCustomFilterByKW(short nType, short nTaps, double dblFC1, double dblFC2, short wDB) 
{
	if( (nTaps < 2) || (nTaps & 1) ) return FALSE;
	if( (nType < 0) || (nType >= 4) ) return FALSE;

	m_Notches.Delete();
	m_Notches.m_FIR.Create(nTaps, nType, m_Notches.m_SampleFreq, dblFC1, dblFC2, wDB, 1.0);
	return TRUE;
}

BOOL CMMVARICtrl::CreateCustomFilterByIDFT(short nTaps, double FAR* pFreqSamp, short wDB) 
{
	if( (nTaps < 2) || (nTaps & 1) ) return FALSE;
	m_Notches.Delete();
	m_Notches.m_FIR.CreateSamp(nTaps, m_Notches.m_SampleFreq, pFreqSamp, wDB);
	return TRUE;
}

short CMMVARICtrl::ReadFFT(long FAR* pArray, short wCount) 
{
	if( wCount > m_CalcFFTWidth ) wCount = m_CalcFFTWidth;
	memcpy(pArray, m_fftout, sizeof(long)*wCount);
	return wCount;
}

short CMMVARICtrl::GetBCodeBAUDOT(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return 0;
	CRxSet *pRxSet = &m_RxSet[nIndex];
	return IsRTTY(pRxSet->m_Mode);
}

//---------------------------------------------------------------------------
int MungeBits( int r )
{
  int oc = 0;
  if( r & 0x00000010 )
    oc += 0x01;
  if( r & 0x00000008 )
    oc += 0x02;
  if( r & 0x00000004 )
    oc += 0x04;
  if( r & 0x00000002 )
    oc += 0x08;
  if( r & 0x00000001 )
    oc += 0x10;
  return (int)oc;
}
void __fastcall CMMVARICtrl::ExtFskIt( int r )
{
	if( m_pCom && m_pCom->m_bFSKOUT && m_RxSet[0].IsRTTY() ) m_pCom->m_QueueExtfsk.Push( (BYTE)MungeBits(r) );
}


short CMMVARICtrl::GetBRTTYFFT(short nIndex) 
{
	if( (nIndex >= 0) && (nIndex < RXMAX) ){
		return m_RxSet[nIndex].m_fRTTYFFT;
	}
	else {
		return FALSE;
	}
}

void CMMVARICtrl::SetBRTTYFFT(short nIndex, short nNewValue) 
{
	if( (nIndex >= 0) && (nIndex < RXMAX) ){
		m_RxSet[nIndex].SetRTTYFFT(nNewValue);
	}
	SetModifiedFlag();
}


void CMMVARICtrl::OnBCOMFSKChanged() 
{
	if( m_pCom ) m_pCom->SetFSK(m_bCOMFSK, m_bCOMFSKINV);
	SetModifiedFlag();
}

void CMMVARICtrl::OnBCOMFSKINVChanged() 
{
	if( m_pCom ) m_pCom->SetFSK(m_bCOMFSK, m_bCOMFSKINV);
	SetModifiedFlag();
}
