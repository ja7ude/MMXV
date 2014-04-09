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

#if !defined(AFX_MMVARICTL_H__118A39FA_4E49_4965_A7D5_A2E6DE554D8D__INCLUDED_)
#define AFX_MMVARICTL_H__118A39FA_4E49_4965_A7D5_A2E6DE554D8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// MMVARICtl.h : CMMVARICtrl ActiveX コントロール クラスの宣言

#include "dsp.h"
#include "Wave.h"
#include "Comm.h"
/////////////////////////////////////////////////////////////////////////////
typedef enum {
	timerNULL,
	timerSOUND,
}TIMERCODE;
#define	intvalSOUND	1000

typedef enum {
	collectNONE,
	collectSYNC,
	collectWAVE,
}MMVX_COLLECTTYPE;

typedef enum {		// 送信ステータス
	txstateRX,
	txstateTX,
	txstateREQRX,
	txstateWAIT,
	txstateTONE,
}MMVX_TXSTATE;

typedef enum {
	cmdtxRX,
	cmdtxTX,
	cmdtxREQRX,
	cmdtxCW,
	cmdtxTONE,
}MMVX_TXCMD;

typedef enum {
	pttlineRTS,
	pttlineDTR,
	pttlineTXD,
	pttlineEND,
}MMVX_PTTLINES;
/////////////////////////////////////////////////////////////////////////////
#define	MFSKAFC_1stATACK	(8*11025/2048)		// 最初の8秒は範囲広い
#define	MFSKAFC_2ndATACK	(30*11025/2048)		// ３０秒経過後は範囲狭い
#define	MFSKAFC_MAX			(60*11025/2048)
/////////////////////////////////////////////////////////////////////////////
// CMMVARICtrl : このクラスの動作の定義に関しては MMVARICtl.cpp を参照してください

class CMMVARICtrl : public COleControl
{
	DECLARE_DYNCREATE(CMMVARICtrl)

public:
	int				m_nBPFType;
	short			m_wRxCarrier;
	UINT			m_WaterSemaphore;
	UINT			m_nErrorCode;
	int				m_TxState;
	double			m_SampleFreq;
	double			m_TxOffset;
	double			m_DemSampleFreq;
	int				m_SAMPBASE;
	int				m_SAMPTYPE;
	int				m_FFT_SIZE;

    BOOL			m_MFSK_Center;
	BOOL			m_MFSK_SQ_Metric;
private:
	UINT_PTR		m_nSoundTimerID;
	int				m_CalcFFTWidth;
public:
	int				m_BufferSize;
	SHORT			m_wBuffer[8192];
	WAVEFORMATEX	m_WFX;
	CWave			m_Wave;

	int				m_SoundMsgTimer;
    int				m_RecoverSoundMode;

	int				m_TX;
	BOOL			m_fHPF;
	CIIR			m_HPF;
//	BOOL			m_fNOTCH;
    int				m_NotchFreq;
	CNotches		m_Notches;

	CRxSet			m_RxSet[RXMAX];		// 受信管理データ
    CMODFSK			m_ModFSK;
	int				m_ModGain;
    double			m_ModGainR;

	int				m_WaveBitMax;
	int				m_WaveType;
    CCOLLECT		m_Collect1;
    CCOLLECT		m_Collect2;

	double			m_DecFactor;
	int				m_fDec;
    CDECM2			m_Dec2;

	int				m_FFTVType;
	int				m_FFTSmooth;

    double			m_FFTFactor;
    double			m_FFTSampFreq;
	double			m_fftbuf[FFT_BUFSIZE*2];
    int				m_fftout[FFT_BUFSIZE];
    CFFT			m_FFT;

    int				m_AFCWidth;
    int				m_AFCLevel;
    int				m_ATCLevel;
    int				m_ATCSpeed;
	int				m_ATCLimit;
	int				m_AFCKeyTimer;

	int				m_SkipTmg;
    BOOL			m_Lock;
	BOOL			m_fReqRX;
	BOOL			m_fDrop;

    CComm			*m_pCom;

    BOOL			m_fSuspend;
    BOOL			m_fTone;
	BOOL			m_fSendChar;
	int				m_nSubWindow;

	BYTE			m_PlayBackTime[3];
    CPlayBack		m_PlayBack;

	CClock			m_Clock;

	BOOL		m_PTTLock;
    CString		m_PTTCOM;
	BOOL		m_bPTTLines[3];

    BOOL		m_fPlayBack;
	int			m_PlayBackSpeed;

	int			m_MaxCarrier;
    int			m_DecCutOff;

    int			m_LoopBack;
private:
	afx_msg LRESULT OnWaveMessage(WPARAM wParam, LPARAM lParam);

	void SetTxState(int state);
	void __fastcall InitWFX(void);
	BOOL __fastcall SampleFreq(double f);
	void __fastcall UpdateModGain(void);
	void __fastcall KillSoundTimer(void);
	void __fastcall SetRXFifo(int d);
	void __fastcall SetTXFifo(int d);
	void __fastcall StrSoundID(CString &cs);
	void __fastcall SetSoundCard(int ch, LPCSTR pID);
	BOOL __fastcall OpenSound(BOOL fTX);
	void __fastcall CloseSound(void);
	BOOL __fastcall ReOutOpen(void);

	void __fastcall OnWaveIn(void);
	void __fastcall OnWaveOut(void);

	void __fastcall DoDem(double d);
	void __fastcall DoDem(void);
	void __fastcall DoMod(void);
	void __fastcall RxChar(int nIndex, int m);
	void __fastcall PutDumpChar(int d, CRxSet *pRxSet);

	void __fastcall SetTXInternal(void);
	void __fastcall ToTone(void);
	void __fastcall ToTX(BOOL fCW);
	void __fastcall ToRX(void);

	double __fastcall AdjDB(double d);
	void __fastcall CalcStgFFT(CRxSet *pRxSet);
	void __fastcall DoAFCMFSK(CRxSet *pRxSet, int fo, BOOL fUpdate);

	void __fastcall FFTSampleFreq(double fq);
	void __fastcall SetPTT(BOOL fTX);
	void __fastcall DoSetBPF(int n);

	int __fastcall GetSignalFreq(int fo, int fm, CRxSet *pRxSet, int th);

	void __fastcall InitCollect(void);
	void __fastcall SetATCSpeed(int f);
	void __fastcall SetATCLimit(int f);
	void __fastcall UpdateCharset(int nIndex, short charset);
	void __fastcall OpenCom(void);
	void __fastcall CloseCom(void);
	void __fastcall OpenAll(void);
	void __fastcall CloseAll(void);
	void __fastcall SetSampleFreq(double f, BOOL fForce);
	void __fastcall SetTxOffset(double f);
	void __fastcall SetSpeed(int nIndex, double b);
	void __fastcall SetMode(int nIndex, int mode);
	BOOL __fastcall DoPlayBack(int s);
	void __fastcall StopPlayBack(void);
	double __fastcall GetDispSpeed(int nIndex);

public:
	BOOL __fastcall IsSoundOpen(void);
	int __fastcall OnGetChar(void);
	inline BOOL IsActive(void){return m_bActive;};

	inline void SetSpeed(CRxSet *pRxSet, double b){SetSpeed(pRxSet - m_RxSet, b);};
	void __fastcall InitCollect(CRxSet *pRxSet, int n);

	int __fastcall GetSignalFreq(int fo, int fm, CRxSet *pRxSet);
	void SetSQLevel(short nIndex, short nNewValue);
	void __fastcall SetTxCarrier(short wFreq);
	void __fastcall SetRxCarrier(short nIndex, short wFreq);
	double GetDblFreqErrorInternal(short nIndex);
	void __fastcall ExtFskIt(int r);

// コンストラクタ
public:
	CMMVARICtrl();

// オーバーライド
	// ClassWizard は仮想関数のオーバライドを生成します
	//{{AFX_VIRTUAL(CMMVARICtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	~CMMVARICtrl();

	DECLARE_OLECREATE_EX(CMMVARICtrl)   // クラス ファクトリ と guid
	DECLARE_OLETYPELIB(CMMVARICtrl)     // GetTypeInfo
	DECLARE_PROPPAGEIDS(CMMVARICtrl)    // プロパティ ページ ID
	DECLARE_OLECTLTYPE(CMMVARICtrl)     // タイプ名とその他のステータス

// メッセージ マップ
	//{{AFX_MSG(CMMVARICtrl)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
// ディスパッチ マップ
	//{{AFX_DISPATCH(CMMVARICtrl)
	short m_wCollectType;
	afx_msg void OnWCollectTypeChanged();
	short m_wTxCarrier;
	afx_msg void OnWTxCarrierChanged();
	long m_dwATC;
	afx_msg void OnDwATCChanged();
	BOOL m_bActive;
	afx_msg void OnBActiveChanged();
	BOOL m_bATC;
	afx_msg void OnBATCChanged();
	BOOL m_bAddStartCR;
	afx_msg void OnBAddStartCRChanged();
	BOOL m_bAddStopCR;
	afx_msg void OnBAddStopCRChanged();
	BOOL m_bNET;
	afx_msg void OnBNETChanged();
	short m_statLostRX;
	afx_msg void OnStatLostRXChanged();
	short m_statLostTX;
	afx_msg void OnStatLostTXChanged();
	BOOL m_bCOMFSK;
	afx_msg void OnBCOMFSKChanged();
	BOOL m_bCOMFSKINV;
	afx_msg void OnBCOMFSKINVChanged();
	afx_msg BOOL GetBTX();
	afx_msg void SetBTX(BOOL bNewValue);
	afx_msg double GetDblSampleFreq();
	afx_msg void SetDblSampleFreq(double newValue);
	afx_msg long GetDwSampleBase();
	afx_msg double GetDblFFTSampleFreq();
	afx_msg double GetDblTxOffset();
	afx_msg void SetDblTxOffset(double newValue);
	afx_msg short GetWModeMax();
	afx_msg BOOL GetBReqRX();
	afx_msg void SetBReqRX(BOOL bNewValue);
	afx_msg short GetNBPF();
	afx_msg void SetNBPF(short nNewValue);
	afx_msg BSTR GetStrSoundID();
	afx_msg void SetStrSoundID(LPCTSTR lpszNewValue);
	afx_msg short GetWSoundCH();
	afx_msg void SetWSoundCH(short nNewValue);
	afx_msg short GetWSoundRxFIFO();
	afx_msg void SetWSoundRxFIFO(short nNewValue);
	afx_msg short GetWSoundTxFIFO();
	afx_msg void SetWSoundTxFIFO(short nNewValue);
	afx_msg short GetWModGain();
	afx_msg void SetWModGain(short nNewValue);
	afx_msg long GetDwHandle();
	afx_msg BSTR GetStrVersion();
	afx_msg short GetWAFCWidth();
	afx_msg void SetWAFCWidth(short nNewValue);
	afx_msg short GetWAFCLevel();
	afx_msg void SetWAFCLevel(short nNewValue);
	afx_msg short GetStatSoundOpen();
	afx_msg short GetStatIsPlaying();
	afx_msg short GetWChannels();
	afx_msg BOOL GetBLoopExternal();
	afx_msg void SetBLoopExternal(BOOL bNewValue);
	afx_msg short GetWBufferCount();
	afx_msg void SetWBufferCount(short nNewValue);
	afx_msg BSTR GetStrPTTPort();
	afx_msg void SetStrPTTPort(LPCTSTR lpszNewValue);
	afx_msg BOOL GetBPTTLock();
	afx_msg void SetBPTTLock(BOOL bNewValue);
	afx_msg short GetWFFTType();
	afx_msg void SetWFFTType(short nNewValue);
	afx_msg BOOL GetBNotch();
	afx_msg void SetBNotch(BOOL bNewValue);
	afx_msg short GetWNotchFreq();
	afx_msg void SetWNotchFreq(short nNewValue);
	afx_msg short GetWCWSpeed();
	afx_msg void SetWCWSpeed(short nNewValue);
	afx_msg BOOL GetBTreatCenterMFSK();
	afx_msg void SetBTreatCenterMFSK(BOOL bNewValue);
	afx_msg BOOL GetBMetricSqMFSK();
	afx_msg void SetBMetricSqMFSK(BOOL bNewValue);
	afx_msg short GetWBufferMax();
	afx_msg short GetWDiddleRTTY();
	afx_msg void SetWDiddleRTTY(short nNewValue);
	afx_msg BOOL GetBPlayBack();
	afx_msg void SetBPlayBack(BOOL bNewValue);
	afx_msg short GetWLang();
	afx_msg short GetWDefaultCharset();
	afx_msg double GetDblTxShift();
	afx_msg void SetDblTxShift(double newValue);
	afx_msg short GetWFFTWidth();
	afx_msg short GetWTxState();
	afx_msg short GetWWaveMax();
	afx_msg short GetStatComOpen();
	afx_msg BOOL GetBHPF();
	afx_msg void SetBHPF(BOOL bNewValue);
	afx_msg double GetDblCustomSampleFreq();
	afx_msg short GetWNumNotches();
	afx_msg BOOL GetBCustomFilter();
	afx_msg short GetWNotchTaps();
	afx_msg void SetWNotchTaps(short nNewValue);
	afx_msg void SendTone();
	afx_msg BOOL PlayBack(short wSec);
	afx_msg void SetTX(short wCmd);
	afx_msg short SendText(LPCTSTR strText);
	afx_msg short SendCWID(LPCTSTR strText);
	afx_msg BSTR GetSendText();
	afx_msg void AdjustClock(short wWidth);
	afx_msg void SetClockAdjust(short wTone, double dblSampleFreq);
	afx_msg BSTR GetVariType(short nIndex);
	afx_msg void AttachLongArray(long FAR* pDist, long FAR* pSrc, short wSize);
	afx_msg short SendChar(short wChar);
	afx_msg BOOL CreateVaricodeList(LPCTSTR strName);
	afx_msg BOOL CreateCustomFilter(short nTaps, double FAR* pCoeff);
	afx_msg void DeleteCustomFilter();
	afx_msg short FindNotchFreq(short wFreq);
	afx_msg void AddNotchFreq(short wFreq);
	afx_msg void DeleteNotch(short nIndex);
	afx_msg BOOL CalcCustomCharacteristic(double FAR* pAmp, double FAR* pPhase, short wWidth, short wMaxFreq);
	afx_msg BOOL CreateCustomFilterByKW(short nType, short nTaps, double dblFC1, double dblFC2, short wDB);
	afx_msg BOOL CreateCustomFilterByIDFT(short nTaps, double FAR* pFreqSamp, short wDB);
	afx_msg short ReadFFT(long FAR* pArray, short wCount);
	afx_msg BSTR GetStrModeList(short nIndex);
	afx_msg BSTR GetStrMode(short nIndex);
	afx_msg void SetStrMode(short nIndex, LPCTSTR lpszNewValue);
	afx_msg double GetDblSpeed(short nIndex);
	afx_msg void SetDblSpeed(short nIndex, double newValue);
	afx_msg short GetWRxCarrier(short nIndex);
	afx_msg void SetWRxCarrier(short nIndex, short nNewValue);
	afx_msg short GetBRxEnabled(short nIndex);
	afx_msg void SetBRxEnabled(short nIndex, short nNewValue);
	afx_msg short GetWSQLevel(short nIndex);
	afx_msg void SetWSQLevel(short nIndex, short nNewValue);
	afx_msg short GetBSQ(short nIndex);
	afx_msg short GetWSN(short nIndex);
	afx_msg short GetWCharset(short nIndex);
	afx_msg void SetWCharset(short nIndex, short nNewValue);
	afx_msg short GetBAFC(short nIndex);
	afx_msg void SetBAFC(short nIndex, short nNewValue);
	afx_msg short GetBSync(short nIndex);
	afx_msg double GetDblFreqError(short nIndex);
	afx_msg double GetDblBandWidth(short nIndex);
	afx_msg short GetWMetricMFSK(short nIndex, short nPhase);
	afx_msg short GetBUOS(short nIndex);
	afx_msg void SetBUOS(short nIndex, short nNewValue);
	afx_msg double GetDblRxShift(short nIndex);
	afx_msg void SetDblRxShift(short nIndex, double newValue);
	afx_msg short GetWMode(short nIndex);
	afx_msg void SetWMode(short nIndex, short nNewValue);
	afx_msg short GetBCodeMM(short nIndex);
	afx_msg short GetBPTTLines(short nLine);
	afx_msg void SetBPTTLines(short nLine, short nNewValue);
	afx_msg short GetBOverLevel(short nIndex);
	afx_msg short GetWNotches(short nIndex);
	afx_msg void SetWNotches(short nIndex, short nNewValue);
	afx_msg short GetWRxCenter(short nIndex);
	afx_msg short GetBCodeBAUDOT(short nIndex);
	afx_msg short GetBRTTYFFT(short nIndex);
	afx_msg void SetBRTTYFFT(short nIndex, short nNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
protected:
	afx_msg void AboutBox();

// イベント マップ
	//{{AFX_EVENT(CMMVARICtrl)
	void FireOnDrawWave(short wCount, long FAR* pArray1, long FAR* pArray2)
		{FireEvent(eventidOnDrawWave,EVENT_PARAM(VTS_I2  VTS_PI4  VTS_PI4), wCount, pArray1, pArray2);}
	void FireOnDrawFFT(short wCount, long FAR* pArray)
		{FireEvent(eventidOnDrawFFT,EVENT_PARAM(VTS_I2  VTS_PI4), wCount, pArray);}
	void FireOnGetTxChar(short FAR* wChar)
		{FireEvent(eventidOnGetTxChar,EVENT_PARAM(VTS_PI2), wChar);}
	void FireOnPTT(short wTX)
		{FireEvent(eventidOnPTT,EVENT_PARAM(VTS_I2), wTX);}
	void FireOnTxState(short wState)
		{FireEvent(eventidOnTxState,EVENT_PARAM(VTS_I2), wState);}
	void FireOnPlayBack(short wStat)
		{FireEvent(eventidOnPlayBack,EVENT_PARAM(VTS_I2), wStat);}
	void FireOnSpeed(short nIndex, double dblSpeed)
		{FireEvent(eventidOnSpeed,EVENT_PARAM(VTS_I2  VTS_R8), nIndex, dblSpeed);}
	void FireOnClockAdjust(long FAR* pArray)
		{FireEvent(eventidOnClockAdjust,EVENT_PARAM(VTS_PI4), pArray);}
	void FireOnTiming(short nIndex, long dwTiming, short wUnit)
		{FireEvent(eventidOnTiming,EVENT_PARAM(VTS_I2  VTS_I4  VTS_I2), nIndex, dwTiming, wUnit);}
	void FireOnTxCarrier(short wFreq)
		{FireEvent(eventidOnTxCarrier,EVENT_PARAM(VTS_I2), wFreq);}
	void FireOnRxCarrier(short nIndex, short wFreq)
		{FireEvent(eventidOnRxCarrier,EVENT_PARAM(VTS_I2  VTS_I2), nIndex, wFreq);}
	void FireOnNET(short bNET)
		{FireEvent(eventidOnNET,EVENT_PARAM(VTS_I2), bNET);}
	void FireOnMode(short nIndex, short mIndex, LPCTSTR strMode)
		{FireEvent(eventidOnMode,EVENT_PARAM(VTS_I2  VTS_I2  VTS_BSTR), nIndex, mIndex, strMode);}
	void FireOnRxChar(short nIndex, LPCTSTR strChar, short wChar)
		{FireEvent(eventidOnRxChar,EVENT_PARAM(VTS_I2  VTS_BSTR  VTS_I2), nIndex, strChar, wChar);}
	void FireOnError(short nErrorCode)
		{FireEvent(eventidOnError,EVENT_PARAM(VTS_I2), nErrorCode);}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// ディスパッチ と イベント ID
public:
	enum {
	//{{AFX_DISP_ID(CMMVARICtrl)
	dispidWCollectType = 1L,
	dispidWTxCarrier = 2L,
	dispidDwATC = 3L,
	dispidBActive = 4L,
	dispidBATC = 5L,
	dispidBAddStartCR = 6L,
	dispidBAddStopCR = 7L,
	dispidBNET = 8L,
	dispidBTX = 13L,
	dispidDblSampleFreq = 14L,
	dispidDwSampleBase = 15L,
	dispidDblFFTSampleFreq = 16L,
	dispidDblTxOffset = 17L,
	dispidWModeMax = 18L,
	dispidBReqRX = 19L,
	dispidNBPF = 20L,
	dispidStrSoundID = 21L,
	dispidWSoundCH = 22L,
	dispidWSoundRxFIFO = 23L,
	dispidWSoundTxFIFO = 24L,
	dispidWModGain = 25L,
	dispidDwHandle = 26L,
	dispidStrVersion = 27L,
	dispidWAFCWidth = 28L,
	dispidWAFCLevel = 29L,
	dispidStatLostRX = 9L,
	dispidStatLostTX = 10L,
	dispidStatSoundOpen = 30L,
	dispidStatIsPlaying = 31L,
	dispidWChannels = 32L,
	dispidBLoopExternal = 33L,
	dispidWBufferCount = 34L,
	dispidStrPTTPort = 35L,
	dispidBPTTLock = 36L,
	dispidWFFTType = 37L,
	dispidBNotch = 38L,
	dispidWNotchFreq = 39L,
	dispidWCWSpeed = 40L,
	dispidBTreatCenterMFSK = 41L,
	dispidBMetricSqMFSK = 42L,
	dispidWBufferMax = 43L,
	dispidWDiddleRTTY = 44L,
	dispidBPlayBack = 45L,
	dispidWLang = 46L,
	dispidWDefaultCharset = 47L,
	dispidDblTxShift = 48L,
	dispidWFFTWidth = 49L,
	dispidWTxState = 50L,
	dispidWWaveMax = 51L,
	dispidStatComOpen = 52L,
	dispidBHPF = 53L,
	dispidDblCustomSampleFreq = 54L,
	dispidWNumNotches = 55L,
	dispidBCustomFilter = 56L,
	dispidWNotchTaps = 57L,
	dispidBCOMFSK = 11L,
	dispidBCOMFSKINV = 12L,
	dispidStrModeList = 79L,
	dispidStrMode = 80L,
	dispidDblSpeed = 81L,
	dispidWRxCarrier = 82L,
	dispidBRxEnabled = 83L,
	dispidWSQLevel = 84L,
	dispidSendTone = 58L,
	dispidBSQ = 85L,
	dispidWSN = 86L,
	dispidPlayBack = 59L,
	dispidSetTX = 60L,
	dispidSendText = 61L,
	dispidSendCWID = 62L,
	dispidGetSendText = 63L,
	dispidAdjustClock = 64L,
	dispidSetClockAdjust = 65L,
	dispidWCharset = 87L,
	dispidGetVariType = 66L,
	dispidAttachLongArray = 67L,
	dispidBAFC = 88L,
	dispidBSync = 89L,
	dispidDblFreqError = 90L,
	dispidDblBandWidth = 91L,
	dispidWMetricMFSK = 92L,
	dispidBUOS = 93L,
	dispidDblRxShift = 94L,
	dispidWMode = 95L,
	dispidSendChar = 68L,
	dispidCreateVaricodeList = 69L,
	dispidBCodeMM = 96L,
	dispidBPTTLines = 97L,
	dispidBOverLevel = 98L,
	dispidWNotches = 99L,
	dispidCreateCustomFilter = 70L,
	dispidDeleteCustomFilter = 71L,
	dispidFindNotchFreq = 72L,
	dispidAddNotchFreq = 73L,
	dispidDeleteNotch = 74L,
	dispidCalcCustomCharacteristic = 75L,
	dispidWRxCenter = 100L,
	dispidCreateCustomFilterByKW = 76L,
	dispidCreateCustomFilterByIDFT = 77L,
	dispidReadFFT = 78L,
	dispidBCodeBAUDOT = 101L,
	dispidBRTTYFFT = 102L,
	eventidOnDrawWave = 1L,
	eventidOnDrawFFT = 2L,
	eventidOnGetTxChar = 3L,
	eventidOnPTT = 4L,
	eventidOnTxState = 5L,
	eventidOnPlayBack = 6L,
	eventidOnSpeed = 7L,
	eventidOnClockAdjust = 8L,
	eventidOnTiming = 9L,
	eventidOnTxCarrier = 10L,
	eventidOnRxCarrier = 11L,
	eventidOnNET = 12L,
	eventidOnMode = 13L,
	eventidOnRxChar = 14L,
	eventidOnError = 15L,
	//}}AFX_DISP_ID
	};
};

double __fastcall DBToSqrt(double d);
double __fastcall SqrtToDB(double d);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MMVARICTL_H__118A39FA_4E49_4965_A7D5_A2E6DE554D8D__INCLUDED)
