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

//---------------------------------------------------------------------------
#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include "ComLib.h"
#include "wave.h"
#include "fft.h"
#include "dsp.h"
#include "comm.h"
//---------------------------------------------------------------------------
#define	MODGAIN		16384.0
#define	LEVELMAX	2048
//---------------------------------------------------------------------------
#define	LOSTMSGTIME	3
//---------------------------------------------------------------------------
enum {
    loopINTERNAL,
    loopEXTERNAL,
};
enum {
	txRX,
    txINTERNAL,
    txEXTERNAL,
};

#define	iniwMETRIC	1
#define	iniwLIMIT	2
#define	iniwBOTH	3
//---------------------------------------------------------------------------
#define	RXMAX	5
//---------------------------------------------------------------------------
class CRxSet {
public:
	int				m_Mode;
	BOOL			m_fJA;
	BOOL			m_fTWO;
    int				m_fMBCS;
	int				m_CarrierFreq;
	CMBCS			m_MBCS;

	BOOL			m_fAFC;
    BOOL			m_SQ;			// スケルチの状態
    int				m_SQLevel;
    int				m_SQTimer;
	double			m_Speed;

    int				m_AFCTimerW;	// 広帯域AFCのガード時間
    int				m_AFCTimerN;	// 高精度AFCのガード時間
	int				m_AFCTimerW2;
    UINT			m_AFCTimerPSK;
	int				m_AFCFQ;
    BOOL			m_AFCSQ;

	CFAVG			m_AvgAFC;

    BOOL			m_fATC;

    int				m_PeakSig;
    CFAVG			m_AvgSig;
	FFTSTG			m_StgFFT;
    CDEMFSK			*m_pDem;
    CFFT			*m_pFFT;

    int				m_X;
    int				m_Y;
    int				m_WaterW;		// for the sub channel

    int				m_cAutoTS1;		// 自動タイムスタンプカウンタ
    int				m_cAutoTS2;		// 自動タイムスタンプカウンタ

    int				m_MFSK_TYPE;
    UINT			m_AFCTimerMFSK;

public:
	CRxSet();
    ~CRxSet();
	void __fastcall SetMFSKType(int type);
	void __fastcall SetSpeed(double b);
	void __fastcall SetMode(int mode);
	void __fastcall SetCarrierFreq(double f);
	void __fastcall Create(BOOL fView);
	void __fastcall Delete(void);
	void __fastcall InitStgFFT(void);
	void __fastcall SetSampleFreq(double f);
	void __fastcall ClearWindow(void);
	double __fastcall GetBandWidth(void);
    double __fastcall GetSpeed(void);
    inline BOOL __fastcall IsActive(void){return m_pDem != NULL;};
    inline BOOL __fastcall IsRTTY(void){::IsRTTY(m_Mode);};
    inline BOOL __fastcall Is170(void){::Is170(m_Mode);};
    inline BOOL __fastcall IsBPSK(void){::IsBPSK(m_Mode);};
    inline BOOL __fastcall IsFSK(void){::IsFSK(m_Mode);};
    inline BOOL __fastcall IsMFSK(void){::IsMFSK(m_Mode);};
};

//---------------------------------------------------------------------------
class CVARI
{
private:	// ユーザー宣言
	BOOL			m_fInitFirst;
    BOOL			m_fDisEvent;
    int				m_AppErr;
public:
	TTimer			*m_pSoundTimer;
	TTimer			*m_pLogLinkTimer;
	TTimer			*m_pRadioTimer;
	TTimer			*m_pMacroTimer;
	TTimer			*m_pPlayBackTimer;
	TTimer			*m_pMacroOnTimer;
	int				m_BufferSize;
	SHORT			m_wBuffer[8192];
	WAVEFORMATEX	m_WFX;
	CWave			m_Wave;
    CWaveFile		m_WaveFile;

	int				m_SoundMsgTimer;
    int				m_RecoverSoundMode;
	int				m_LostSoundRX;
    int				m_LostSoundTX;
	int				m_fShowMsg;
	int				m_cInfoMsg[5];
    int				m_InfoMsgFlag;
	AnsiString		m_InfoMsg[5];
	int				m_cErrorMsg;
	AnsiString		m_ErrorMsg;

	int				m_TX;
	BOOL			m_fHPF;
	CIIR			m_HPF;
	BOOL			m_fNOTCH;
    int				m_NotchFreq;
    int				m_NotchTaps;
    int				m_NotchWidth;
    CFIR2			m_NOTCH;
	int				m_MouseNotch;
	int				m_NotchX, m_NotchY;

	CRxSet			m_RxSet[RXMAX];		// 受信管理データ
    CMODFSK			m_ModFSK;
	int				m_ModGain;
    double			m_ModGainR;
//	double			m_Speed;

#if DEBUG
    CMODFSK			m_ModTest;	// For test
	CVCO			m_VCOTest;	// For test
    CIIR			m_TestHPF;
    CFIR2			m_BPF500;
#endif

	BOOL			m_StatusUTC;
	int				m_SendingEdit;
	int				m_CurrentEdit;
	int				m_SaveEditPage;
	BOOL			m_fSendChar;

	int				m_WaveBitMax;
	int				m_WaveType;
    CCOLLECT		m_Collect1;
    CCOLLECT		m_Collect2;
//	CFAVG			m_AVGWave;

#if DEBUG
    CQSB			m_QSB;
    CNoise			m_Noise;
#endif

	double			m_DecFactor;
	int				m_fDec;
    CDECM2			m_Dec2;

	int				m_RadioScaleCounter;
	int				m_ScaleAsRigFreq;
    int				m_ScaleDetails;
	int				m_FFTVType;
	int				m_FFTSmooth;
	int				m_FFTW;
    int				m_FFTB;
    int				m_FFTU;
	int				m_FFTWindow;
    double			m_FFTFactor;
    double			m_FFTSampFreq;
	double			m_fftbuf[FFT_BUFSIZE*2];
    int				m_fftout[FFT_BUFSIZE];
    CFFT			m_FFT;

	int				m_WaterNoiseL;
    int				m_WaterNoiseH;

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

	DWORD			m_QSOStart;
    SYSTEMTIME		m_LocalTime;

	BOOL			m_fSubWindow;

    int				m_fftSC;
	int				m_fftMX;
	int				m_fftXW, m_fftYW;
    Graphics::TBitmap *m_pBitmapFFT;
	int				m_levelXW, m_levelYW;
    Graphics::TBitmap *m_pBitmapLevel;

	int				m_PFTimer;
    int				m_pfXW, m_pfYW, m_pfXC;
    Graphics::TBitmap *m_pBitmapPF;

	int				m_tWaterLevel[6];
    UCOL			m_tWaterColset[12];
	TColor			m_tWaterColors[256];

    UCOL			m_tFFTColset[6];

    FFTSTG			m_StgWater;

	int				m_TestTimer;
    FILE			*m_fpText;
    FILE			*m_fpTest;

    int				m_MacButtonVW;
	MACBUTTON		m_tMacButton[MACBUTTONALL];

    CComm			*m_pCom;
    CCradio			*m_pRadio;

    char			m_HintText[256];	// ヒント用のバッファ
    char			m_TextBuff[256];	// 汎用のバッファ

	LPCSTR			m_pCheckKey;
	AnsiString		*m_pCheckBuff;

    BOOL			m_fHintUpdate;
	AnsiString		m_HintKey;

	BOOL			m_MouseDown;
    int				m_MouseSubChannel;

	int				m_Priority;
    int				m_Dupe;

    AnsiString		m_StrSel;
    AnsiString		m_UStrSel;

	int				m_MacroMenuNo;
	TPopupMenu		*m_pMacroPopup;
    TMenuItem		*m_pCurrentMacroMenu;
    int				m_CurrentMacro;
	BOOL			m_fMacroRepeat;
    int				m_ReqMacroTimer;
	int				m_ReqAutoClear;
    int				m_ReqAutoReturn;
	int				m_ReqAutoNET;
	TMenuItem		*m_ParentMenu;

    TClockAdjDlg	*m_pClockView;

    BOOL			m_fKeyShift;
    BOOL			m_fSuspend;
    BOOL			m_fTone;

	int				m_RightX;
    int				m_RightFreq;

    AnsiString		m_MacroInput;
    CVal			m_MacroVal;

    BOOL			m_fMBCS;
    BOOL			m_fConvAlpha;
	BOOL			m_fRttyWordOut;

	AnsiString		m_strStatus[statusEND];
    TRect			m_rcStatus[statusEND];


    TTimer			*m_pWheelTimer;

	int				m_CPUBENCHType;
    CFAVG			m_CPUBENCH;

	BYTE			m_PlayBackTime[3];
    CPlayBack		m_PlayBack;
    CWebRef			m_WebRef;
	CLIBL			m_LibDLL;

    int				m_LogBand;
    TMenuItem		*m_pMMHamsoft;

    AnsiString		m_ListBAUD;
#if DEBUG
	TSpeedButton	*m_pDebugButton;
#endif

private:
	void __fastcall OnAppMessage(tagMSG &Msg, bool &Handled);
	void __fastcall OnActiveFormChange(TObject *Sender);
	void __fastcall SetSystemFont(void);
	void __fastcall SoundTimer(TObject *Sender);
	void __fastcall LogLinkTimer(TObject *Sender);
	void __fastcall RadioTimer(TObject *Sender);
	void __fastcall MacroTimer(TObject *Sender);
	void __fastcall WheelTimer(TObject *Sender);
	void __fastcall MacroOnTimer(TObject *Sender);

	LPCSTR __fastcall GetHintStatus(LPCSTR pHint);
	void __fastcall DrawStatusBar(const Windows::TRect &Rect, LPCSTR pText, TColor col);
	void __fastcall DrawHint(void);
	void __fastcall DrawStatus(int n, LPCSTR pText);
	void __fastcall DrawStatus(int n);
	int __fastcall GetStatusIndex(int x);
	int __fastcall GetPopupIndex(TComponent *pComponent);
	void __fastcall UpdateCharset(void);
	void __fastcall UpdateTitle(void);
	void __fastcall ReadRegister(void);
	void __fastcall WriteRegister(void);
	void __fastcall SampleStatus(void);
	void __fastcall PageStatus(void);
	BOOL __fastcall SampleFreq(double f);
	void __fastcall FFTSampleFreq(double f);
	BOOL __fastcall OpenSound(BOOL fTX);
	BOOL __fastcall ReOutOpen(void);
	void __fastcall OpenCom(void);
	void __fastcall OpenRadio(void);
	void __fastcall UpdateUI(void);
	void __fastcall UpdateLogPanel(void);
	void __fastcall OnWaveIn(void);
	void __fastcall OnWaveOut(void);
	void __fastcall RemoveUselessMessage(UINT wParam);
	void __fastcall InitWFX(void);
	void __fastcall Draw(BOOL fPaint);
	void __fastcall DrawFFT(BOOL fPaint);
	void __fastcall DrawWater(BOOL fPaint, BOOL fClear);
	void __fastcall DrawWave(BOOL fPaint);
	void __fastcall DrawPF(BOOL fPaint);
	void __fastcall CalcFFTCenter(int fo);
	void __fastcall CalcFFTFreq(void);
	void __fastcall SetFFTWidth(int fw);
	void __fastcall InitStgFFT(FFTSTG *pStg);
	void __fastcall InitWater(int sw);
	double __fastcall SqrtToDB(double d);
	double __fastcall DBToSqrt(double d);
	double __fastcall AdjDB(double d);
	void __fastcall CalcStgFFT(CRxSet *pRxSet);
	void __fastcall DoAFCMFSK(CRxSet *pRxSet, int fo, BOOL fUpdate);
	void __fastcall CreateWaterColors(void);

	void __fastcall DrawLevel(BOOL fPaint);
	void __fastcall DoMod(void);
	void __fastcall PutDumpChar(int d, CRxSet *pRxSet);
	void __fastcall DoDem(double d);
	void __fastcall DoDem(void);
	void __fastcall UpdateWaveCaption(void);
	void __fastcall SetTXCaption(void);
	void __fastcall SetPTT(BOOL fTX);
	void __fastcall RxStatus(CRxSet *pRxSet, LPCSTR p);
	void __fastcall SetTXInternal(void);
	void __fastcall ToTone(void);
	void __fastcall ToTX(void);
	void __fastcall ToRX(void);
	void __fastcall DeleteSoundTimer(void);
	void __fastcall SetRxFreq(int fq);
	void __fastcall SetTxFreq(int fq);
	void __fastcall SetMacButtonMax(void);
	void __fastcall CreateMacButton(void);
	int __fastcall GetMacButtonNo(TSpeedButton *pButton);
	void __fastcall OnMacButtonClick(TObject *Sender);
	void __fastcall OnMacButtonDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall CreateMacExButton(void);
	void __fastcall DeleteMacExButton(void);
	void __fastcall AttachFocus(void);
	void __fastcall DettachFocus(void);
	TSpeedButton *__fastcall GetDraw(int n);
	int __fastcall GetDrawType(void);
	void __fastcall SetDrawType(int n);

	void __fastcall InitCheckValKey(LPCSTR pKey, AnsiString *pAS);
	BOOL __fastcall CheckKey(LPCSTR pTemplate);
	LPCSTR __fastcall CheckValKey(LPCSTR pTemplate);
	LPCSTR __fastcall CheckValKey(LPCSTR pKey, LPCSTR pTemplate);
	void __fastcall MacroDate(LPSTR t, SYSTEMTIME &now);
	void __fastcall MacroHisName(LPSTR t);
//	BOOL __fastcall CheckCond(LPCSTR p, LPCSTR v, int type, int &op, double &d, AnsiString &as);
	LPCSTR __fastcall Cond(LPCSTR p, TSpeedButton *pButton);
	BOOL __fastcall IsMBCSStr(LPCSTR p);
    BOOL __fastcall GetDataCond(LPCSTR p, int err, TSpeedButton *pButton);
	int __fastcall ConvMacro(LPSTR t, LPCSTR p, TSpeedButton *pButton);
	void __fastcall DoMacroReturn(int f);
	void __fastcall DoMacro(LPCSTR pMacro, TSpeedButton *pButton);
	void __fastcall SendButton(int n);

	void __fastcall UpdateCallsign(void);
	void __fastcall UpdateLogMode(void);
	void __fastcall UpdateLogData(void);
	void __fastcall UpdateTextData(void);
	void __fastcall FindCall(void);
	void __fastcall UpdateLogLink(void);
	void __fastcall AutoLogSave(void);
	void __fastcall WndCopyData(TMessage &Message);
	void __fastcall CheckLogLink(void);
	void __fastcall InitDefKey(void);
	void __fastcall UpdateSubWindow(void);
	void __fastcall DrawSubChannel(TPaintBox *pBox);
	void __fastcall AppException(TObject *Sender, Exception *E);
	void __fastcall ShowSubChannel(int n, int sw);
	void __fastcall MacroGreeting(LPSTR t, LPCSTR pCall, int type);

	void __fastcall KCallClick(TObject *Sender);
	void __fastcall AddCall(LPCSTR p);
	void __fastcall SetTXFocus(void);

	BOOL __fastcall DeleteMacroTimerS(void);
	void __fastcall DeleteMacroTimer(void);
	void __fastcall CreateMacroTimer(int n);

    void __fastcall UpdateLogHeight(void);
	void __fastcall UpdateMacButtonVW(int n);
	void __fastcall UpdateMFSKSpeed(int mode);
	void __fastcall SetMode(int mode);
	void __fastcall DoSuspend(BOOL fMinimize);
	void __fastcall DoResume(void);
	void __fastcall SetEditPage(int cno);
	void __fastcall InitCollect(void);
	void __fastcall LoadMacro(LPCSTR pName);
	void __fastcall SaveMacro(LPCSTR pName);
	void __fastcall LoadMacro(TMemIniFile *pIniFile);
	void __fastcall SaveMacro(TMemIniFile *pIniFile);
	void __fastcall SetSpeedInfo(double b);
	void __fastcall SetNotchFreq(void);
	BOOL __fastcall IsFreqErr(double d);
	int __fastcall GetMsgCount(void);

	inline BOOL __fastcall IsRTTY(void){return m_RxSet[0].IsRTTY();};
	inline BOOL __fastcall Is170(void){return m_RxSet[0].Is170();};
	inline BOOL __fastcall IsBPSK(void){return m_RxSet[0].IsBPSK();};

	void __fastcall OutYaesuVU(int hz);
	void __fastcall OutYaesuHF(int hz);
	void __fastcall OutCIV(int hz);
	void __fastcall OutCIV4(int hz);
	void __fastcall OutKENWOOD(int hz);
	void __fastcall OutJST245(int hz);

	void __fastcall OutModeYaesuHF(LPCSTR pMode);
	void __fastcall OutModeYaesuVU(LPCSTR pMode);
	void __fastcall OutModeCIV(LPCSTR pMode);
	void __fastcall OutModeKENWOOD(LPCSTR pMode);
	void __fastcall OutModeJST245(LPCSTR pMode);

	void __fastcall OnMacroMenuClick(TObject *Sender);
	void __fastcall WaitICOM(void);
	void __fastcall DoMacroMenu(LPCSTR pVal, TSpeedButton *pButton, BOOL fRadio);

	void __fastcall UpdatePlayBack(void);
	void __fastcall DoPlayBack(int s);
	void __fastcall StopPlayBack(void);

	void __fastcall UpdateShowCtrl(void);
	void __fastcall DoEvent(int n);
	void __fastcall UpdateMacroOnTimer(void);
	void __fastcall OnLogFreq(BOOL fLink);
	void __fastcall ShutDown(void);
	void __fastcall DoBench(LPSTR t, int type);
	void __fastcall OnXmClick(TObject *Sender);
	void __fastcall OnXClick(TObject *Sender);
	TMenuItem* __fastcall FindMenu(TMenuItem *pMainMenu, LPCSTR pTitle);
	TMenuItem* __fastcall GetMenuItem(TMenuItem *pMenu, LPCSTR pTitle);
	TMenuItem* __fastcall GetMainMenu(LPCSTR pTitle, BOOL fNew);
	void __fastcall AddExtensionMenu(LPCSTR pMain, LPCSTR pCaption, LPCSTR pHint);
	void __fastcall InsExtensionMenu(LPCSTR pTitle, LPCSTR pPos, LPCSTR pCaption, LPCSTR pHint);
	void __fastcall ShortCutExtensionMenu(TMenuItem *pMenu, LPCSTR pKey);
	BOOL __fastcall IsXMenu(TMenuItem *pItem);
	BOOL __fastcall OnMenuProc(TMenuItem *pMenu, LPCSTR pProc, LPCSTR pPara, BOOL fStop);
	void __fastcall OnMenuProc(TMenuItem *pMenu, LPCSTR pCaption);
	void __fastcall DoParentClick(TMenuItem *pMenu);
    TMenuItem* __fastcall GetMenuArg(AnsiString &arg, LPCSTR pVal, BOOL fArg);
	void __fastcall SetRadioMenu(TMenuItem *pMenu);

	TSpeedButton* __fastcall FindButton(TComponent *pMainControl, LPCSTR pTitle, TSpeedButton *pButton, BOOL fErrMsg);
	void __fastcall ClickButton(TSpeedButton *pButton);

#if DEBUG
	void __fastcall TestSignal(void);
#endif

public:		// ユーザー宣言
	__fastcall TMainVARI(TComponent* Owner);

	void __fastcall DisplayHint(TObject *Sender);
  	void __fastcall OnWave(void);
	void __fastcall SetSoundCard(int ch, LPCSTR pID);
	void __fastcall SetSampleFreq(double f, BOOL fForce);
	void __fastcall SetTxOffset(double f);
	void __fastcall SetRXFifo(int d);
	void __fastcall SetTXFifo(int d);
	void __fastcall UpdatePriority(int Priority);
	void __fastcall DrawFreqScale(TCanvas *pCanvas, int XW, int YW,  int fftb, int fftw, int fh, BOOL fRadio);
	void __fastcall DrawErrorMsg(TCanvas *pCanvas, int XW, int YW, BOOL fReset);
	void __fastcall SetInfoMsg(LPCSTR pStr);
	void __fastcall SetInfoMsg(LPCSTR pStr, int pos);
	void __fastcall SetErrorMsg(LPCSTR pStr);
	int __fastcall GetOverlayTop(void);
	void __fastcall SetConvAlpha(BOOL f);

	void __fastcall SpeedChange(double b);
	void __fastcall UpdateSpeed(CRxSet *pRxSet, double b);
	void __fastcall UpdateMode(CRxSet *pRxSet, int offset);
	void __fastcall SetATCSpeed(int f);
	void __fastcall SetATCLimit(int f);
	BOOL __fastcall GetDataConds(LPCSTR p, int err, TSpeedButton *pButton);

	LPCSTR __fastcall GetMacroValue(LPCSTR pVal);
	LPCSTR __fastcall GetMacroStr(AnsiString &as, LPCSTR pVal);
	int __fastcall GetMacroInt(LPCSTR pVal);
	double __fastcall GetMacroDouble(LPCSTR pVal);
	int __fastcall ConvMacro(AnsiString &as, LPCSTR p, TSpeedButton *pButton);
	int __fastcall ConvMacro_(AnsiString &as, LPCSTR p, TSpeedButton *pButton);
	int __fastcall ConvMacro_(LPSTR t, LPCSTR p, TSpeedButton *pButton);

	int __fastcall GetSignalFreq(int freq, int fm, CRxSet *pRxSet);
	int __fastcall TMainVARI::GetSignalFreq(int fo, int fm, CRxSet *pRxSet, int th);
	void __fastcall OnChar(int Key);
	void __fastcall UpdateModGain(void);

	void __fastcall InitCollect(CRxSet *pRxSet, int n);
// DLL実装で追加した関数
	void __fastcall DoTXOFF(void);
	int				m_nBPFType;
	void __fastcall DoSetBPF(int n);
	void __fastcall DoSetFFTType(int type);
	int				m_fWaterAGC;

	friend int __fastcall OnGetChar(void);

protected:
	void __fastcall OnWaveEvent(TMessage Message);
	void __fastcall CMMML(TMessage Message);
	void __fastcall CMMMR(TMessage Message);
    void __fastcall OnActiveApp(TMessage Message);
BEGIN_MESSAGE_MAP
	MESSAGE_HANDLER(WM_WAVE, TMessage, OnWaveEvent)
	MESSAGE_HANDLER(CM_CMML, TMessage, CMMML)
	MESSAGE_HANDLER(CM_CMMR, TMessage, CMMMR)
	MESSAGE_HANDLER(WM_ACTIVATEAPP, TMessage, OnActiveApp)
END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TMainVARI *MainVARI;
//---------------------------------------------------------------------------
#endif
