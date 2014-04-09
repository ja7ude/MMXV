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

#include "stdafx.h"
#include "MMVARI.h"
///----------------------------------------------------------------

#include <SHELLAPI.H>
#include <math.h>
#include "Main.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainVARI *MainVARI;
//---------------------------------------------------------------------------
int __fastcall OnGetChar(void)
{
	// To do
	return 0;
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TMainVARI::GetHintStatus(LPCSTR pHint)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DisplayHint(TObject *Sender)
{
}
//---------------------------------------------------------------------------
// サウンドカードのオープンのトライ（オープンできない時のリカバリ処理）
void __fastcall TMainVARI::SoundTimer(TObject *Sender)
{
	if( m_fSuspend ) return;

	BOOL r;
	if( m_RecoverSoundMode ){
		r = ReOutOpen();
    }
    else {
		r = OpenSound(FALSE);
    }
	if( r ){
		m_pSoundTimer->Enabled = FALSE;
		delete m_pSoundTimer;
        m_pSoundTimer = NULL;
    }
    else {
		m_SoundMsgTimer -= m_pSoundTimer->Interval;
        if( m_SoundMsgTimer < 0 ){
			m_fShowMsg = TRUE;
        }
		PBoxFFTPaint(NULL);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::LogLinkTimer(TObject *Sender)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::RadioTimer(TObject *Sender)
{
	if( m_pRadio != NULL ){
		m_pRadio->Timer(m_TX, m_pRadioTimer->Interval);
//		if( m_pRadio->IsFreqChange(LogFreq->Text.c_str()) ){
//			LogFreq->Text = m_pRadio->GetFreq();
//		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::MacroTimer(TObject *Sender)
{
	if( m_pMacroTimer ){
		m_pMacroTimer->Enabled = FALSE;
		if( m_CurrentMacro >= 0 ){
			m_fMacroRepeat = TRUE;
    	   	SendButton(m_CurrentMacro);
	    }
        else if( (m_CurrentMacro == -1) && IsXMenu(m_pCurrentMacroMenu) ){
			m_fMacroRepeat = TRUE;
			OnXClick(m_pCurrentMacroMenu);
        }
        else {
			delete m_pMacroTimer;
            m_pMacroTimer = NULL;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::WheelTimer(TObject *Sender)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnActiveApp(TMessage Message)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnActiveFormChange(TObject *Sender)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FormCreate(TObject *Sender)
{
	Application->OnHint = DisplayHint;
    Screen->OnActiveFormChange = OnActiveFormChange;
#if !DEBUG || SHOWERRCOUNT
	Application->OnException = AppException;
#endif
}
//---------------------------------------------------------------------------
// アプリケーション例外
void __fastcall TMainVARI::AppException(TObject *Sender, Exception *E)
{
	m_AppErr++;
#if SHOWERRCOUNT
	sprintf(m_TextBuff, "%u (%u): %s", m_AppErr, sys.m_ErrPhase, E->Message.c_str());
    Caption = m_TextBuff;
#endif
	if( m_AppErr < 128 ){
		if( m_AppErr == 3 ){
			ErrorMB(sys.m_MsgEng?
				"The indistinct error was detected.":
				"予想外のエラーが発生しました. 回復を試みます."
			);
		}
        if( m_FFT.m_FFTDIS ){
			m_FFT.InitFFT();
			m_FFT.m_FFTDIS = FALSE;
        }
#if 0
		if( !m_TX ){
			m_Wave.InClose();
        	OpenSound(FALSE);
	    }
#endif
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TMainVARI::TMainVARI(TComponent* Owner)
	: TForm(Owner)
{
	m_fInitFirst = TRUE;
	m_fDisEvent = TRUE;

	// Windows version
	sys.m_dwVersion = ::GetVersion();
	if( sys.m_dwVersion < 0x80000000 ){                // Windows NT
		sys.m_WinNT = TRUE;
	}
	else {
		sys.m_WinNT = FALSE;
	}
	// 言語
	sys.m_LCID = GetThreadLocale();
#if FORCELANG
	sys.m_wLang = FORCELANG;
#else
    sys.m_wLang = LANGIDFROMLCID(sys.m_LCID);
#endif
    if( sys.m_wLang == 0x0411 ){	// 日本語
		sys.m_FontName = "ＭＳ Ｐゴシック";
		sys.m_FontCharset = SHIFTJIS_CHARSET;
        sys.m_MsgEng = FALSE;
    }
    else {
		sys.m_FontName = "Arial";
		sys.m_FontCharset = ANSI_CHARSET;
        sys.m_MsgEng = TRUE;
    }
	sys.m_fBaseMBCS = SetLangFont(NULL, sys.m_wLang);
    sys.m_DefaultMode = sys.m_fBaseMBCS ? MODE_GMSK : MODE_N_BPSK;

	sys.m_fFixWindow = FALSE;
	sys.m_rcWindow.left = Left;
    sys.m_rcWindow.top = Top;
    sys.m_rcWindow.right = Width;
    sys.m_rcWindow.bottom = Height;

	sys.m_AutoTimeOffset = 0;
	sys.m_TimeOffset;
	sys.m_TimeOffsetMin;
	sys.m_LogName = "NOCALL.MDT";
	InitRADIOPara();
    InitDefKey();

	if( ParamCount() >= 0 ){
		SetDirName(sys.m_BgnDir, ParamStr(0).c_str());
	}
	else {
		SetCurDir(sys.m_BgnDir, sizeof(sys.m_BgnDir));
	}
	strcpy(sys.m_SoundDir, sys.m_BgnDir);
	strcpy(sys.m_LogDir, sys.m_BgnDir);
	strcpy(sys.m_ExtLogDir, sys.m_BgnDir);
	strcpy(sys.m_TextDir, sys.m_BgnDir);
	sprintf(sys.m_MacroDir, "%sMacroTxt\\", sys.m_BgnDir);

    g_VariCode.Init();		// バリコードテーブルを初期化

	sys.m_CallSign = "NOCALL";
    sys.m_PTTCOM = "NONE";
    sys.m_PTTLock = TRUE;
    sys.m_fPlayBack = FALSE;
	sys.m_PlayBackSpeed = 5;
	sys.m_fShowCtrlCode = FALSE;
	sys.m_MFSK_Center = FALSE;
	sys.m_MFSK_SQ_Metric = FALSE;
    sys.m_MacroError = FALSE;
	sys.m_MaxCarrier = 2700;
    sys.m_DecCutOff = 2700;
    memset(&sys.m_PosMacEdit, 0, sizeof(sys.m_PosMacEdit));
	sys.m_MacBuffSize = 4096;
	sys.m_OnTimerInterval = 1000;

	m_QSOStart = 0;
	m_WaterNoiseL = 35;
	m_WaterNoiseH = 65;
	m_TX = txRX;
	m_AppErr = 0;
	m_pCom = NULL;
	m_pRadio = NULL;
    m_fReqRX = FALSE;
    m_pSoundTimer = NULL;
    m_pLogLinkTimer = NULL;
    m_pRadioTimer = NULL;
    m_pMacroTimer = NULL;
    m_pWheelTimer = NULL;
    m_pMacroOnTimer = NULL;
	m_pClockView = NULL;
	sys.m_LoopBack = loopINTERNAL;
	m_fpText = NULL;
    m_fpTest = NULL;
	m_BufferSize = 2048;
	m_fHPF = FALSE;
	m_fNOTCH = FALSE;
    m_NotchFreq = 1750;
    m_NotchTaps = 128;
    m_NotchWidth = 1;
	m_MouseNotch = FALSE;
    m_MouseSubChannel = FALSE;
	m_fSubWindow = FALSE;
	m_MouseDown = FALSE;
	m_CurrentMacro = 0;
    m_pCurrentMacroMenu = NULL;
	m_ReqMacroTimer = 0;
	m_fHintUpdate = FALSE;
	m_LostSoundRX = 0;
    m_LostSoundTX = 0;
	m_fShowMsg = FALSE;
	m_InfoMsgFlag = 0;
    m_cErrorMsg = FALSE;
	m_fKeyShift = FALSE;
	m_ReqAutoClear = FALSE;
	m_fSuspend = FALSE;
	m_fTone = FALSE;
	m_ReqAutoReturn = FALSE;
	m_ReqAutoNET = FALSE;
	m_pMacroPopup = NULL;
	m_MacroMenuNo = 0;
	m_fMacroRepeat = FALSE;

	m_ModGain = MODGAIN;
	m_FFTVType = 2;
	m_ScaleAsRigFreq = 0;
    m_ScaleDetails = TRUE;
	m_CurrentEdit = 0;
    m_SendingEdit = 0;

    m_WaveType = 0;
	m_fRttyWordOut = TRUE;
    m_ListBAUD = "20.0,31.25,45.45,62.5,93.75,125.0,250.0";

    m_AFCWidth = 50;
    m_AFCLevel = 12;	// SN=12dB
	m_ATCLevel = 15;	// SN=15dB
	m_ATCSpeed = 0;
    m_ATCLimit = 25000;
	m_AFCKeyTimer = 0;

	m_FFTSmooth = 2;
	m_FFTW = 1000;
	m_fMBCS = TRUE;
	m_fConvAlpha = TRUE;

    m_PlayBackTime[0]=60;
    m_PlayBackTime[1]=30;
    m_PlayBackTime[2]=15;

	m_CPUBENCHType = -1;
	m_ParentMenu = NULL;

	m_StatusUTC = FALSE;
    m_Priority = 1;
    m_RxSet[0].m_CarrierFreq = 1750;
	m_RxSet[0].Create(FALSE);

    m_RxSet[1].m_CarrierFreq = 1600;
    m_RxSet[2].m_CarrierFreq = 1900;
    m_RxSet[3].m_CarrierFreq = 1450;
    m_RxSet[4].m_CarrierFreq = 2050;
	for( int i = 1; i < RXMAX; i++ ){
		m_RxSet[i].m_rcView.left = 10 + (i * 16);
		m_RxSet[i].m_rcView.top = 10 + (i * 16);
	    m_RxSet[i].m_rcView.right = 500;
        m_RxSet[i].m_rcView.bottom = 124;
    }

	m_fWaterAGC = 0;
    m_nBPFType = 1;
#if DEBUG
	sys.m_test = FALSE;
	sys.m_testSN = 20;
	sys.m_testName = "EProject.txt";
	sys.m_testGain = 0;
    sys.m_testCarrier1 = 1750;
    sys.m_testCarrier2 = 0;
    sys.m_testDB2 = 0;
    sys.m_testQSBTime = 8000;
    sys.m_testQSBDB = 0;
    sys.m_test500 = FALSE;
    sys.m_testPhase = FALSE;
	m_pDebugButton = NULL;
#endif

	sys.m_fAutoTS = FALSE;
	sys.m_EnableMouseWheel = TRUE;

	sys.m_OptionPage = 1;
	sys.m_EventIndex = 0;
	sys.m_fFontFam = FALSE;
	memset(sys.m_tFontFam, 0, sizeof(sys.m_tFontFam));
	memset(m_fftbuf, 0, sizeof(m_fftbuf));

	m_tWaterLevel[0] = 28;
	m_tWaterLevel[1] = 100;
	m_tWaterLevel[2] = 168;
	m_tWaterLevel[3] = 192;
	m_tWaterLevel[4] = 220;
	m_tWaterLevel[5] = 240;

    m_tWaterColset[0].c = clBlack;		// back
    m_tWaterColset[1].d = RGB(0,255,255);	// low
    m_tWaterColset[2].c = clWhite;		// char
    m_tWaterColset[3].c = clRed;		// WAKU
	m_tWaterColset[4].d = RGB(0,255,255);		// RX
    m_tWaterColset[5].c = clBlue;		// TX
    m_tWaterColset[6].c = clYellow;		// mid-low
    m_tWaterColset[7].d = RGB(255,128,0);	// mid
    m_tWaterColset[8].d = RGB(255,0,128);	// mid-high
    m_tWaterColset[9].c = clRed;		// Peak
    m_tWaterColset[10].c = clRed;		// Peak
    m_tWaterColset[11].c = clRed;		// Peak

    m_tFFTColset[0].c = clBlack;		// back
    m_tFFTColset[1].d = RGB(0,255,0);	// high
    m_tFFTColset[2].c = clWhite;		// char
    m_tFFTColset[3].c = clGray;			// Gage
    m_tFFTColset[4].d = RGB(0,255,255);	// RX
    m_tFFTColset[5].c = clBlue;			// TX

	m_MacButtonVW = 3;
    int i;
    char bf[256];
	MACBUTTON *pList = m_tMacButton;
    for( i = 0; i < MACBUTTONALL; i++, pList++ ){
		pList->pButton = NULL;
		sprintf(bf, "M%u", i + 1);
        pList->Name = bf;
        pList->Color = clBlack;
        pList->Text = "";
        pList->Style = 0;
    }
	pList = m_tMacButton;
    pList->Name = "Clear";
    pList->Text = "<%ClearTXW>";
    pList++;
    pList->Name = "CQ";
//    pList->Text = "<%ClearTXW><%TX>CQ CQ CQ de <%MyCall> <%MyCall> <%MyCall> pse (<%VARITYPE>) K\r\n<%RX>";
    pList->Text = "<%ClearTXW><%TX>CQ CQ CQ de <%MyCall> <%MyCall> <%MyCall> pse ";
	if( sys.m_fBaseMBCS ) pList->Text += "(<%VARITYPE>) ";
    pList->Text += "K\r\n<%RX>";
	pList++;
    pList->Name = "CQ2";
    pList->Text = "<%ClearTXW><%TX>CQ CQ CQ de <%MyCall> <%MyCall> <%MyCall>\r\nCQ CQ CQ de <%MyCall> <%MyCall> <%MyCall> pse ";
	if( sys.m_fBaseMBCS ) pList->Text += "(<%VARITYPE>) ";
    pList->Text += "K\r\n<%RX>";
	pList++;
    pList->Name = "1x1";
    pList->Text = "<%NETON><%TX><%HisCall> de <%MyCall> pse K\r\n<%RX>";
	pList++;
    pList->Name = "2x2";
    pList->Text = "<%NETON><%TX><%HisCall> <%HisCall> de <%MyCall> <%MyCall> pse K\r\n<%RX>";
	pList++;
    pList->Name = "RRR";
    pList->Text = "\r\nRRR <%HisCall> de <%MyCall>\r\n";
	if( sys.m_fBaseMBCS ) pList->Text += "--- <%VARITYPE> ---\r\n";
	pList++;
    pList->Name = "BTU";
    pList->Text = "BTU <%HisCall> de <%MyCall> KN\r\n<%RX>";
	pList = &m_tMacButton[12];
    pList->Name = "CWID";
    pList->Text = "<%TX><%CWID=DE <%MyCall>><%RX><%EOF>";
    pList->Style = FSUNDERLINE;
    pList++;
    pList->Name = "TU SK";
    pList->Text = "<%TX><%CWID=TU:><%RX><%EOF>";
    pList->Style = FSUNDERLINE;
	pList++;
    pList++;
    pList->Name = "0x2 - 2x4";
	pList->Text = "<%DisableCR>\r\n#macro <%Menu= 0x2, 0x3, 0x4, -, 1x2, 1x3, 1x4, -, 2x3, 2x4>\r\n#if ValMenu\r\n#define\t_His\t<%Format=%c,<%Input$>>\r\n#define\t_My\t<%Format=%d,<%Find$=x,<%Input$>>>\r\n<%TX><%RX>\r\n#if !Is1stCR\r\n<%CR>\r\n#endif\r\n#if IsCall\r\n<%RepeatText=_His,<%HisCall> >\r\n#endif\r\nde\r\n<%RepeatText=_My, <%MyCall>>\r\n pse K<%CR>\r\n#endif\r\n<%EOF>";

    sys.m_AS = "<%DisableCR>\r\n#if !IsTX\r\n<%AutoNET><%AutoReturn>\r\n#if ValPage!=4\r\n<%Page=4>\r\n#else\r\n<%Page=3>\r\n#endif\r\n<%ClearTXW>\r\n#endif\r\n<%TX><%CWID=@><%RX><%EOF>\r\n";

//	InitStgFFT(&m_StgFFT);
//    m_StgFFT.Timer = 0;

	m_fftMX = 0;
	m_fftSC = 109;

	ReadRegister();
	SampleFreq(SAMPFREQ);
	UpdateModGain();
	m_FFT.InitFFT();
	InitWater(iniwBOTH);

	SetSystemFont();
	UpdateLogHeight();
	InitWFX();
    m_Dec2.SetSampleFreq(m_fDec, SAMPFREQ);
 	m_ModFSK.m_Encode.m_pFunc = OnGetChar;
    m_ModFSK.SetMFSKType(m_RxSet[0].m_MFSK_TYPE);
    m_ModFSK.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
    m_RxSet[0].SetSampleFreq(DEMSAMPFREQ);
	InitCollect(m_RxSet, IsRTTY() ? 7 : 14);
	m_HPF.Create(ffHPF, 300, SAMPFREQ, 2, 1, 0.3);
	m_ModFSK.SetSpeed(m_RxSet[0].m_Speed);
    m_ModFSK.SetType(m_RxSet[0].m_Mode);
    m_RxSet[0].SetMFSKType(m_RxSet[0].m_MFSK_TYPE);
	m_RxSet[0].SetMode(m_RxSet[0].m_Mode);
    m_RxSet[0].m_pDem->SetSpeed(m_RxSet[0].m_Speed);
    m_RxSet[0].SetCarrierFreq(UdRxCarrier->Position);
    m_ModFSK.SetCarrierFreq(UdTxCarrier->Position);
	SetATCSpeed(m_ATCSpeed);
	SetATCLimit(m_ATCLimit);
	SetNotchFreq();
	if( sys.m_fPlayBack ) m_PlayBack.Init(m_BufferSize, SAMPBASE);

#if DEBUG
    m_ModTest.SetMFSKType(m_RxSet[0].m_MFSK_TYPE);
	m_ModTest.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
    m_ModTest.SetSpeed(m_RxSet[0].m_Speed);
    m_ModTest.SetType(m_RxSet[0].m_Mode);
	if( sys.m_testCarrier1 ) m_ModTest.SetCarrierFreq(sys.m_testCarrier1);
	if( sys.m_testCarrier1 ) m_BPF500.Create(96, ffBPF, SAMPFREQ+SAMPTXOFFSET, sys.m_testCarrier1-250, sys.m_testCarrier1+250, 60.0, 1.0);

    m_VCOTest.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
	if( sys.m_testCarrier2 ) m_VCOTest.SetFreeFreq(sys.m_testCarrier2);
#endif

	if( m_FFTW < 750 ){
		m_FFTW = 500;
		SBFFT500->Down = TRUE;
    }
    else if( m_FFTW < 1500 ){
		SBFFT1K->Down = TRUE;
    }
    else if( m_FFTW < 2500 ){
		SBFFT2K->Down = TRUE;
    }
    else {
		m_FFTW = 3000;
		SBFFT3K->Down = TRUE;
    }
    m_FFTWindow = m_FFTW * FFT_SIZE / m_FFTSampFreq;
    CalcFFTCenter(UdRxCarrier->Position);
	for( i = 0; i < RXMAX; i++ ){
		m_RxSet[i].m_AFCTimerW = 3 * SAMPFREQ / m_BufferSize;
		m_RxSet[i].m_AFCTimerN = 3 * SAMPFREQ / m_BufferSize;
    }
	m_pBitmapFFT = new Graphics::TBitmap;
    m_pBitmapFFT->Width = PBoxFFT->Width;
    m_pBitmapFFT->Height = PBoxFFT->Height;
    m_fftYW = m_pBitmapFFT->Height;
    m_fftXW = m_pBitmapFFT->Width;

	m_pBitmapLevel = new Graphics::TBitmap;
    m_pBitmapLevel->Width = PBoxLevel->Width;
    m_pBitmapLevel->Height = PBoxLevel->Height;
    m_levelYW = m_pBitmapLevel->Height;
    m_levelXW = m_pBitmapLevel->Width;

    m_pBitmapPF = new Graphics::TBitmap;
    m_pBitmapPF->Width = PBoxPF->Width;
    m_pBitmapPF->Height = PBoxPF->Height;
	m_pfXW = m_pBitmapPF->Width;
    m_pfYW = m_pBitmapPF->Height;
    m_pfXC = m_pfXW / 2;
	m_PFTimer = 0;

	CreateWaterColors();
    Draw(FALSE);
    DrawLevel(FALSE);
    DrawPF(FALSE);

	UpdateCharset();

	SetTXCaption();
	UpdateTitle();
	if( m_MacButtonVW != 3 ){
		UpdateMacButtonVW(m_MacButtonVW);
    }
    else {
		SetMacButtonMax();
    	CreateMacButton();
    }
	if( m_Priority ) UpdatePriority(-m_Priority);
    OpenCom();
    OpenRadio();
	UpdateMFSKSpeed(m_RxSet[0].m_Mode);
	UpdateUI();
    UpdateWaveCaption();
    SampleStatus();
    PageStatus();
    UpdateMode(m_RxSet, 0);
	if( sys.m_fFixWindow ){
    	SetBounds(sys.m_rcWindow.left, sys.m_rcWindow.top, sys.m_rcWindow.right, sys.m_rcWindow.bottom);
		if( sys.m_WindowState == wsMaximized ) WindowState = wsMaximized;
    }
    m_fDisEvent = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetSystemFont(void)
{
	sys.m_MsgEng = (sys.m_FontCharset != SHIFTJIS_CHARSET);

}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateCharset(void)
{
	int charset = SHIFTJIS_CHARSET;
	SetMBCP(charset);
	m_RxSet[0].m_MBCS.Create();
    m_RxSet[0].m_MBCS.SetCharset(charset);
    m_RxSet[0].m_fJA = (charset == SHIFTJIS_CHARSET);
    m_ModFSK.m_Encode.m_fJA = m_RxSet[0].m_fJA;

	LPCSTR pKey;
    switch(charset){
		case SHIFTJIS_CHARSET:
			pKey = "JA";
            m_fMBCS = TRUE;
//            KJA->Checked = TRUE;
			break;
        case JOHAB_CHARSET:
		case HANGEUL_CHARSET:
			pKey = "HL";
            m_fMBCS = TRUE;
//            KHL->Checked = TRUE;
			break;
		case CHINESEBIG5_CHARSET:   //
			pKey = "BV";
            m_fMBCS = TRUE;
//            KBV->Checked = TRUE;
			break;
		case 134:       // 簡略
			pKey = "BY";
            m_fMBCS = TRUE;
//            KBY->Checked = TRUE;
			break;
        case ANSI_CHARSET:
			pKey = "ANSI";
            m_fMBCS = FALSE;
//            KANSI->Checked = TRUE;
			break;
        case SYMBOL_CHARSET:
			pKey = "SYM";
            m_fMBCS = FALSE;
//            KMISC->Checked = TRUE;
            break;
		default:
			pKey = "Other";
            m_fMBCS = FALSE;
//            KMISC->Checked = TRUE;
			break;
	}
	DrawStatus(statusVARI, pKey);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FormDestroy(TObject *Sender)
{
    if( m_pClockView ) delete m_pClockView;
    delete m_pBitmapFFT;
    delete m_pBitmapLevel;
    m_pBitmapFFT = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FormCloseQuery(TObject *Sender, bool &CanClose)
{
#if !DEBUG || SHOWERRCOUNT
	Application->OnException = NULL;
#endif
    Screen->OnActiveFormChange = NULL;
	DoEvent(macOnExit);
	sys.m_rcWindow.left = Left;
    sys.m_rcWindow.top = Top;
    sys.m_rcWindow.right = Width;
    sys.m_rcWindow.bottom = Height;

	for( int i = 0; i < RXMAX; i++ ){
		m_RxSet[i].Delete();
    }
    WriteRegister();

	sys.m_EnableMouseWheel = FALSE;
    if( m_pSoundTimer ) delete m_pSoundTimer;
    if( m_pLogLinkTimer ) delete m_pLogLinkTimer;
    if( m_pRadioTimer ) delete m_pRadioTimer;
    if( m_pMacroTimer ) delete m_pMacroTimer;
    if( m_pWheelTimer ) delete m_pWheelTimer;
    if( m_pMacroOnTimer ) delete m_pMacroOnTimer;
	m_Wave.OutAbort();
    m_Wave.InClose();
	if( m_pCom ) delete m_pCom;
	if( m_pRadio ) delete m_pRadio;
	UpdatePriority(0);
	CanClose = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateTitle(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdatePriority(int Priority)
{
	if( Priority == m_Priority ) return;

	m_Priority = ABS(Priority);
    if( m_TX ) ToRX();
	BOOL f = m_Wave.IsInOpen();
    if( f ) m_Wave.InClose();
	HANDLE hThread = GetCurrentThread();
	switch(m_Priority){
		case 1:
			::SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
			break;
		case 2:
			::SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
			break;
		default:
			::SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
			break;
	}
	if( f ) OpenSound(FALSE);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::ReadRegister(void)
{
	char bf[256];
	sprintf(bf, "%sMmvari.ini", sys.m_BgnDir);
	TMemIniFile	*pIniFile = new TMemIniFile(bf);

	int i;

    // サウンドカード
	AnsiString as;
    sprintf(bf, "%.2lf", SAMPFREQ);
    as = pIniFile->ReadString("SoundCard", "Clock", bf);
	double fq;
    sscanf(as.c_str(), "%lf", &fq);
	LimitDbl(&fq, MIN_SAMP, MAX_SAMP);
    SampleFreq(fq);
	sprintf(bf, "%.2lf", SAMPTXOFFSET);
    as = pIniFile->ReadString("SoundCard", "TxOffset", bf);
    sscanf(as.c_str(), "%lf", &fq);
	if( fabs(fq) <= 500.0 ){
	    SAMPTXOFFSET = fq;
    }
	m_Wave.m_InFifoSize = pIniFile->ReadInteger("SoundCard", "RxFIFO", m_Wave.m_InFifoSize);
	m_Wave.m_OutFifoSize = pIniFile->ReadInteger("SoundCard", "TxFIFO", m_Wave.m_OutFifoSize);
	m_Wave.m_SoundStereo = pIniFile->ReadInteger("SoundCard", "Source", m_Wave.m_SoundStereo);
	m_Wave.m_SoundID = pIniFile->ReadInteger("SoundCard", "ID", m_Wave.m_SoundID);
	m_Wave.m_SoundTxID = pIniFile->ReadInteger("SoundCard", "IDTX", m_Wave.m_SoundID);
	sys.m_SoundMMW = pIniFile->ReadString("SoundCard", "MMW", sys.m_SoundMMW);

    LimitInt(&m_Wave.m_InFifoSize, 4, 32);
    LimitInt(&m_Wave.m_OutFifoSize, 4, 32);

    /* SYSTEM */
	int IniSchema = pIniFile->ReadInteger("System", "Schema", 0);
    m_Priority = pIniFile->ReadInteger("System", "Priority", m_Priority);
	sys.m_FontName = pIniFile->ReadString("System", "FontName", sys.m_FontName);
	sys.m_FontCharset = (BYTE)pIniFile->ReadInteger("System", "FontCharset", sys.m_FontCharset);

    /* Window */
	sys.m_fFixWindow = pIniFile->ReadInteger("Window", "Fixed", sys.m_fFixWindow);
    sys.m_rcWindow.left = pIniFile->ReadInteger("Window", "Left", sys.m_rcWindow.left);
    sys.m_rcWindow.top = pIniFile->ReadInteger("Window", "Top", sys.m_rcWindow.top);
    sys.m_rcWindow.right = pIniFile->ReadInteger("Window", "Width", sys.m_rcWindow.right);
    sys.m_rcWindow.bottom = pIniFile->ReadInteger("Window", "Height", sys.m_rcWindow.bottom);
	sys.m_WindowState = pIniFile->ReadInteger("Window", "WindowState", wsNormal);

	/* Folder */
    as = pIniFile->ReadString("Folder", "Sound", sys.m_SoundDir);
	strcpy(sys.m_SoundDir, as.c_str());
    as = pIniFile->ReadString("Folder", "Log", sys.m_LogDir);
	strcpy(sys.m_LogDir, as.c_str());
    as = pIniFile->ReadString("Folder", "ExtLog", sys.m_ExtLogDir);
	strcpy(sys.m_ExtLogDir, as.c_str());
    as = pIniFile->ReadString("Folder", "Text", sys.m_TextDir);
	strcpy(sys.m_TextDir, as.c_str());

	/* PTT */
	sys.m_PTTCOM = pIniFile->ReadString("PTT", "Name", sys.m_PTTCOM);
    sys.m_PTTLock = pIniFile->ReadInteger("PTT", "Lock", sys.m_PTTLock);

    /* TX */
    sys.m_LoopBack = pIniFile->ReadInteger("TX", "LoopBack", sys.m_LoopBack);
	int f = pIniFile->ReadInteger("TX", "AutoCR", KOAI->Checked | (KOAO->Checked << 1));
	KOAI->Checked = (f & 1);
    KOAO->Checked = (f & 2);
	UdTxCarrier->Position = (short)pIniFile->ReadInteger("TX", "Carrier", UdTxCarrier->Position);
	SBNET->Down = pIniFile->ReadInteger("TX", "NET", SBNET->Down);
	m_ModGain = pIniFile->ReadInteger("TX", "ModGain", m_ModGain);
	m_fRttyWordOut = pIniFile->ReadInteger("TX", "RttyWordOut", m_fRttyWordOut);

    as = pIniFile->ReadString("TX", "Mode", g_tDispModeTable[sys.m_DefaultMode]);
	m_RxSet[0].m_Mode = GetModeIndex(as.c_str());
	as = pIniFile->ReadString("TX", "Speed", "31.25");
    sscanf(as.c_str(), "%lf", &m_RxSet[0].m_Speed);
	LimitDbl(&m_RxSet[0].m_Speed, MIN_SPEED, MAX_SPEED);
    CBSpeed->Text = StrDbl(bf, m_RxSet[0].m_Speed);
	m_fConvAlpha = pIniFile->ReadInteger("TX", "ConvAlpha", m_fConvAlpha);
	m_RxSet[0].m_MFSK_TYPE = pIniFile->ReadInteger("TX", "MFSK_TYPE", m_RxSet[0].m_MFSK_TYPE);
#if !ENABLE_MFSK8
	if( m_RxSet[0].m_MFSK_TYPE == typMFSK8 ) m_RxSet[0].m_MFSK_TYPE = typMFSK16;
#endif
    sys.m_MFSK_Center = pIniFile->ReadInteger("TX", "MFSK_CENTER", sys.m_MFSK_Center);

    /* RX */
	UdRxCarrier->Position = (short)pIniFile->ReadInteger("RX", "Carrier", UdRxCarrier->Position);
	DoSetBPF(pIniFile->ReadInteger("RX", "BPF", m_nBPFType));
	SBAFC->Down = pIniFile->ReadInteger("RX", "AFC", SBAFC->Down);
	m_RxSet[0].m_pDem->m_fAFC = m_RxSet[0].m_fAFC = SBAFC->Down;
	SBATC->Down = pIniFile->ReadInteger("RX", "ATC", SBATC->Down);
	m_RxSet[0].m_pDem->m_Decode.m_fATC = SBATC->Down;
	m_RxSet[0].m_SQLevel = pIniFile->ReadInteger("RX", "SQLevel", m_RxSet[0].m_SQLevel);
	m_AFCWidth = pIniFile->ReadInteger("RX", "AFCWidth", m_AFCWidth);
	m_AFCLevel = pIniFile->ReadInteger("RX", "AFCLevel", m_AFCLevel);
	if( IniSchema < 3 ){	// For old version
		if( m_AFCWidth > 50 ) m_AFCWidth = 50;
        if( m_AFCLevel < 12 ) m_AFCLevel = 12;
    }
	m_fHPF = pIniFile->ReadInteger("RX", "HPF", m_fHPF);
	m_ATCLevel = pIniFile->ReadInteger("RX", "ATCLevel", m_ATCLevel);
	m_ATCSpeed = pIniFile->ReadInteger("RX", "ATCSpeed", m_ATCSpeed);
	m_ATCLimit = pIniFile->ReadInteger("RX", "ATCLimit", m_ATCLimit);
	sys.m_fAutoTS = pIniFile->ReadInteger("RX", "SWL_TS", sys.m_fAutoTS);
	m_fNOTCH = pIniFile->ReadInteger("RX", "Notch", m_fNOTCH);
	m_NotchFreq = pIniFile->ReadInteger("RX", "NotchFreq", m_NotchFreq);
	sys.m_fPlayBack = pIniFile->ReadInteger("RX", "PlayBack", sys.m_fPlayBack);
	sys.m_PlayBackSpeed = pIniFile->ReadInteger("RX", "PlayBackSpeed", sys.m_PlayBackSpeed);
    sys.m_MFSK_SQ_Metric = pIniFile->ReadInteger("RX", "MFSK_SQ_METRIC", sys.m_MFSK_SQ_Metric);

    /* View */
	m_FFTW = pIniFile->ReadInteger("FFT", "Width", m_FFTW);
	m_FFTSmooth = pIniFile->ReadInteger("FFT", "Smooth", m_FFTSmooth);
	m_FFTVType = pIniFile->ReadInteger("FFT", "Type", m_FFTVType);
	if( m_FFTSmooth < 2 ) m_FFTSmooth = 2;
	m_FFT.m_FFTGain = (m_FFTVType < 2) ? 0 : 1;
	for( i = 0; i < 12; i++ ){
		sprintf(bf, "Col%u", i + 1);
		if( i < 6 ){
	        m_tFFTColset[i].d = pIniFile->ReadInteger("Spec", bf, m_tFFTColset[i].d);
		}
        m_tWaterColset[i].d = pIniFile->ReadInteger("WF", bf, m_tWaterColset[i].d);
		if( i < 6 ){
			sprintf(bf, "Lvl%u", i + 1);
			m_tWaterLevel[i] = pIniFile->ReadInteger("WF", bf, m_tWaterLevel[i]);
        }
    }
	SetDrawType(pIniFile->ReadInteger("View", "DrawType", GetDrawType()));
	m_WaveType = pIniFile->ReadInteger("View", "WaveForm", m_WaveType);
    m_ScaleAsRigFreq = pIniFile->ReadInteger("View", "RigFreqScale", m_ScaleAsRigFreq);
    m_ScaleDetails = pIniFile->ReadInteger("View", "ScaleDetails", m_ScaleDetails);

	LoadRADIOSetup(pIniFile);

    /* Sub channel */
	CRxSet *pRxSet = &m_RxSet[1];
    for( i = 1; i < RXMAX; i++, pRxSet++ ){
		sprintf(bf, "Channel%u", i);
	    as = pIniFile->ReadString(bf, "Mode", g_tDispModeTable[sys.m_DefaultMode]);
		pRxSet->m_Mode = GetModeIndex(as.c_str());
		pRxSet->m_CarrierFreq = pIniFile->ReadInteger(bf, "Carrier", pRxSet->m_CarrierFreq);
		LimitInt(&pRxSet->m_CarrierFreq, MIN_CARRIER, sys.m_MaxCarrier);
        pRxSet->m_WaterW = pIniFile->ReadInteger(bf, "WaterWidth", pRxSet->m_WaterW);
		LimitInt(&pRxSet->m_WaterW, 200, 800);
        pRxSet->m_rcView.left = pIniFile->ReadInteger(bf, "Left", pRxSet->m_rcView.left);
        pRxSet->m_rcView.top = pIniFile->ReadInteger(bf, "Top", pRxSet->m_rcView.top);
        pRxSet->m_rcView.right = pIniFile->ReadInteger(bf, "Width", pRxSet->m_rcView.right);
        pRxSet->m_rcView.bottom = pIniFile->ReadInteger(bf, "Height", pRxSet->m_rcView.bottom);
		LoadFontFromInifile(&pRxSet->m_FontData, bf, pIniFile);
    }
    delete pIniFile;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::WriteRegister(void)
{
	int i;
	char bf[256];
	sprintf(bf, "%sMmvari.ini", sys.m_BgnDir);
	TMemIniFile	*pIniFile = new TMemIniFile(bf);

    // サウンドカード
    sprintf(bf, "%.2lf", SAMPFREQ);
    pIniFile->WriteString("SoundCard", "Clock", bf);
	sprintf(bf, "%.2lf", SAMPTXOFFSET);
    pIniFile->WriteString("SoundCard", "TxOffset", bf);
	pIniFile->WriteInteger("SoundCard", "RxFIFO", m_Wave.m_InFifoSize);
	pIniFile->WriteInteger("SoundCard", "TxFIFO", m_Wave.m_OutFifoSize);
	pIniFile->WriteInteger("SoundCard", "Source", m_Wave.m_SoundStereo);
	pIniFile->WriteInteger("SoundCard", "ID", m_Wave.m_SoundID);
	pIniFile->WriteInteger("SoundCard", "IDTX", m_Wave.m_SoundTxID);
	pIniFile->WriteString("SoundCard", "MMW", sys.m_SoundMMW);

    /* SYSTEM */
    pIniFile->WriteInteger("System", "Schema", INI_SCHEMA);
    pIniFile->WriteInteger("System", "Priority", m_Priority);
	pIniFile->WriteString("System", "FontName", sys.m_FontName);
	pIniFile->WriteInteger("System", "FontCharset", sys.m_FontCharset);

    /* Window */
	if( WindowState == wsNormal ){
		pIniFile->WriteInteger("Window", "Fixed", sys.m_fFixWindow);
    	pIniFile->WriteInteger("Window", "Left", sys.m_rcWindow.left);
    	pIniFile->WriteInteger("Window", "Top", sys.m_rcWindow.top);
    	pIniFile->WriteInteger("Window", "Width", sys.m_rcWindow.right);
    	pIniFile->WriteInteger("Window", "Height", sys.m_rcWindow.bottom);
    }
	pIniFile->WriteInteger("Window", "WindowState", WindowState);

	/* Folder */
    pIniFile->WriteString("Folder", "Sound", sys.m_SoundDir);
	pIniFile->WriteString("Folder", "Log", sys.m_LogDir);
    pIniFile->WriteString("Folder", "ExtLog", sys.m_ExtLogDir);
    pIniFile->WriteString("Folder", "Text", sys.m_TextDir);

	/* PTT */
	pIniFile->WriteString("PTT", "Name", sys.m_PTTCOM);
    pIniFile->WriteInteger("PTT", "Lock", sys.m_PTTLock);

    /* TX */
    pIniFile->WriteInteger("TX", "LoopBack", sys.m_LoopBack);
	pIniFile->WriteInteger("TX", "AutoCR", KOAI->Checked | (KOAO->Checked << 1));
	pIniFile->WriteInteger("TX", "Carrier", UdTxCarrier->Position);
	pIniFile->WriteInteger("TX", "NET", SBNET->Down);
	pIniFile->WriteString("TX", "Mode", g_tDispModeTable[m_RxSet[0].m_Mode]);
	pIniFile->WriteString("TX", "Speed", StrDbl(bf, m_RxSet[0].m_Speed));
	pIniFile->WriteInteger("TX", "ModGain", m_ModGain);
	pIniFile->WriteInteger("TX", "ConvAlpha", m_fConvAlpha);
	pIniFile->WriteInteger("TX", "RttyWordOut", m_fRttyWordOut);
	pIniFile->WriteInteger("TX", "MFSK_TYPE", m_RxSet[0].m_MFSK_TYPE);
    pIniFile->WriteInteger("TX", "MFSK_CENTER", sys.m_MFSK_Center);

    /* View */
	pIniFile->WriteInteger("FFT", "Width", m_FFTW);
	pIniFile->WriteInteger("FFT", "Smooth", m_FFTSmooth);
	pIniFile->WriteInteger("FFT", "Type", m_FFTVType);
	for( i = 0; i < 12; i++ ){
		sprintf(bf, "Col%u", i + 1);
		if( i < 6 ){
	        pIniFile->WriteInteger("Spec", bf, m_tFFTColset[i].d);
        }
        pIniFile->WriteInteger("WF", bf, m_tWaterColset[i].d);
		if( i < 6 ){
			sprintf(bf, "Lvl%u", i + 1);
			pIniFile->WriteInteger("WF", bf, m_tWaterLevel[i]);
        }                      
    }
	pIniFile->WriteInteger("View", "DrawType", GetDrawType());
	pIniFile->WriteInteger("View", "WaveForm", m_WaveType);
    pIniFile->WriteInteger("View", "RigFreqScale", m_ScaleAsRigFreq);
	pIniFile->WriteInteger("View", "ScaleDetails", m_ScaleDetails);

    /* RX */
	pIniFile->WriteInteger("RX", "Carrier", UdRxCarrier->Position);
	pIniFile->WriteInteger("RX", "BPF", m_nBPFType);
	pIniFile->WriteInteger("RX", "AFC", SBAFC->Down);
	pIniFile->WriteInteger("RX", "ATC", SBATC->Down);
	pIniFile->WriteInteger("RX", "SQLevel", m_RxSet[0].m_SQLevel);
	pIniFile->WriteInteger("RX", "AFCWidth", m_AFCWidth);
	pIniFile->WriteInteger("RX", "AFCLevel", m_AFCLevel);
	pIniFile->WriteInteger("RX", "HPF", m_fHPF);
	pIniFile->WriteInteger("RX", "ATCLevel", m_ATCLevel);
	pIniFile->WriteInteger("RX", "ATCSpeed", m_ATCSpeed);
	pIniFile->WriteInteger("RX", "ATCLimit", m_ATCLimit);
	pIniFile->WriteInteger("RX", "SWL_TS", sys.m_fAutoTS);
	pIniFile->WriteInteger("RX", "Notch", m_fNOTCH);
	pIniFile->WriteInteger("RX", "NotchFreq", m_NotchFreq);
	pIniFile->WriteInteger("RX", "PlayBack", sys.m_fPlayBack);
	pIniFile->WriteInteger("RX", "PlayBackSpeed", sys.m_PlayBackSpeed);
    pIniFile->WriteInteger("RX", "MFSK_SQ_METRIC", sys.m_MFSK_SQ_Metric);

	/* Radio */
	SaveRADIOSetup(pIniFile);

    /* Sub channel */
	CRxSet *pRxSet = &m_RxSet[1];
    for( i = 1; i < RXMAX; i++, pRxSet++ ){
		sprintf(bf, "Channel%u", i);
		pIniFile->WriteString(bf, "Mode", g_tDispModeTable[pRxSet->m_Mode]);
        pIniFile->WriteInteger(bf, "Carrier", pRxSet->m_CarrierFreq);
        pIniFile->WriteInteger(bf, "WaterWidth", pRxSet->m_WaterW);
        pIniFile->WriteInteger(bf, "Left", pRxSet->m_rcView.left);
        pIniFile->WriteInteger(bf, "Top", pRxSet->m_rcView.top);
        pIniFile->WriteInteger(bf, "Width", pRxSet->m_rcView.right);
        pIniFile->WriteInteger(bf, "Height", pRxSet->m_rcView.bottom);
		SaveFontToInifile(&m_RxSet[i].m_FontData, bf, pIniFile);
    }

	m_MacroVal.WriteInifile(pIniFile, "MacroVal");

	pIniFile->UpdateFile();
    delete pIniFile;
}
//---------------------------------------------------------------------------
/*
	Clock	復調器	FFT
	6000Hz	6000Hz	6000Hz	1024
	8000Hz	8000Hz	8000Hz	2048
	11025Hz	5513Hz	11025Hz	2048
	12000Hz	6000Hz	6000Hz	1024
	16000Hz	5333Hz	10667Hz	2048
	18000Hz	6000Hz	6000Hz	1024
	22050Hz	5513Hz	11025Hz	2048
	24000Hz	6000Hz	6000Hz	1024
	44100Hz	6300Hz	6300Hz	1024
	48000Hz	6000Hz	6000Hz	1024
	50000Hz	6250Hz	6250Hz	1024
*/
BOOL __fastcall TMainVARI::SampleFreq(double f)
{
	int fftsize = FFT_SIZE;
	if( f >= 49000.0 ){							// 50000Hz
	    SAMPFREQ = f;
		SAMPBASE = 50000;
        m_fDec = 7;
        m_DecFactor = 0.125;
        SAMPTYPE = 10;
        m_BufferSize = 8192;
		FFT_SIZE = 1024;
        m_FFTFactor = 0.125;
		sys.m_DecCutOff = 2900;
		sys.m_MaxCarrier = 3000;
	}
	else if( f >= 46000.0 ){					// 48000Hz
	    SAMPFREQ = f;
		SAMPBASE = 48000;
        m_fDec = 7;
        m_DecFactor = 0.125;
        SAMPTYPE = 9;
        m_BufferSize = 8192;
		FFT_SIZE = 1024;
        m_FFTFactor = 0.125;
		sys.m_DecCutOff = 2800;
		sys.m_MaxCarrier = 2950;
    }
	else if( f >= 43000.0 ){					// 44100Hz
	    SAMPFREQ = f;
		SAMPBASE = 44100;
        m_fDec = 6;
        m_DecFactor = 1.0/7.0;
        SAMPTYPE = 8;
        m_BufferSize = 8192;
		FFT_SIZE = 1024;
        m_FFTFactor = 1.0/7.0;
		sys.m_DecCutOff = 2900;
		sys.m_MaxCarrier = 3000;
    }
	else if( f >= 23000.0 ){					// 24000Hz
	    SAMPFREQ = f;
		SAMPBASE = 24000;
        m_fDec = 3;
        m_DecFactor = 0.25;
        SAMPTYPE = 7;
        m_BufferSize = 4096;
		FFT_SIZE = 1024;
        m_FFTFactor = 0.25;
		sys.m_DecCutOff = 2800;
		sys.m_MaxCarrier = 2950;
    }
	else if( f >= 20000.0 ){					// 22050Hz
	    SAMPFREQ = f;
		SAMPBASE = 22050;
        m_fDec = 3;
        m_DecFactor = 0.25;
        SAMPTYPE = 6;
        m_BufferSize = 4096;
		FFT_SIZE = 2048;
        m_FFTFactor = 0.5;
		sys.m_DecCutOff = 2700;
		sys.m_MaxCarrier = 2700;
    }
	else if( f >= 17000.0 ){					// 18000Hz
	    SAMPFREQ = f;
		SAMPBASE = 18000;
        m_fDec = 2;
        m_DecFactor = 1.0/3.0;
        SAMPTYPE = 5;
        m_BufferSize = 3072;
		FFT_SIZE = 1024;
        m_FFTFactor = 1.0/3.0;
		sys.m_DecCutOff = 2800;
		sys.m_MaxCarrier = 2950;
    }
	else if( f >= 15000.0 ){					// 16000Hz
	    SAMPFREQ = f;
		SAMPBASE = 16000;
        m_fDec = 2;
        m_DecFactor = 1.0/3.0;
        SAMPTYPE = 4;
        m_BufferSize = 3072;
		FFT_SIZE = 2048;
        m_FFTFactor = 2.0/3.0;
		sys.m_DecCutOff = 2600;
		sys.m_MaxCarrier = 2600;
    }
	else if( f >= 11600.0 ){					// 12000Hz系
	    SAMPFREQ = f;
		SAMPBASE = 12000;
        m_fDec = 1;
        m_DecFactor = 0.5;
        SAMPTYPE = 3;
        m_BufferSize = 2048;
		FFT_SIZE = 1024;
        m_FFTFactor = 0.5;
		sys.m_DecCutOff = 2900;
		sys.m_MaxCarrier = 2950;
	}
    else if( f > 10000.0 ){						// 11025Hz系
	    SAMPFREQ = f;
		SAMPBASE = 11025;
        m_fDec = 1;
        m_DecFactor = 0.5;
        SAMPTYPE = 0;
        m_BufferSize = 2048;
		FFT_SIZE = 2048;
        m_FFTFactor = 1.0;
		sys.m_DecCutOff = 2700;
		sys.m_MaxCarrier = 2700;
    }
    else if( f > 7000.0 ){						// 8000Hz系
	    SAMPFREQ = f;
		SAMPBASE = 8000;
        m_fDec = 0;
        m_DecFactor = 1.0;
        SAMPTYPE = 1;
        m_BufferSize = 2048;
		FFT_SIZE = 2048;
        m_FFTFactor = 1.0;
		sys.m_DecCutOff = 3000;
		sys.m_MaxCarrier = 3000;
    }
	else if( f > 5000 ){						// 6000Hz系
	    SAMPFREQ = f;
		SAMPBASE = 6000;
        m_fDec = 0;
        m_DecFactor = 1.0;
        SAMPTYPE = 2;
        m_BufferSize = 1024;
		FFT_SIZE = 1024;
        m_FFTFactor = 1.0;
		sys.m_DecCutOff = 2950;
		sys.m_MaxCarrier = 2950;
    }
    else {
		return FALSE;
    }
    DEMSAMPFREQ = SAMPFREQ * m_DecFactor;
	m_FFTSampFreq = SAMPFREQ * m_FFTFactor;
    if( fftsize != FFT_SIZE ) m_FFT.InitFFT();
	UdRxCarrier->Max = short(sys.m_MaxCarrier);
	UdTxCarrier->Max = short(sys.m_MaxCarrier);
    return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FFTSampleFreq(double fq)
{
    m_FFTSampFreq = fq * m_FFTFactor;
    m_FFTWindow = m_FFTW * FFT_SIZE / m_FFTSampFreq;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SampleStatus(void)
{
	if( SAMPTXOFFSET ){
		sprintf(m_TextBuff, "%.2lf/%.2lf", SAMPFREQ, SAMPTXOFFSET);
    }
    else {
		sprintf(m_TextBuff, "%.2lfHz", SAMPFREQ);
    }
	DrawStatus(statusSAMP, m_TextBuff);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PageStatus(void)
{
	sprintf(m_TextBuff, "Page%d", m_CurrentEdit + 1);
    DrawStatus(statusPAGE, m_TextBuff);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetSampleFreq(double f, BOOL fForce)
{
	if( !fForce && (SAMPFREQ == f) ) return;

	if( !SampleFreq(f) ) return;
	BOOL fi = m_Wave.IsInOpen();
    BOOL fo = m_Wave.IsOutOpen();
    m_Wave.InClose(); m_Wave.OutAbort();
	InitWFX();
    m_Dec2.SetSampleFreq(m_fDec, SAMPFREQ);
	CRxSet *pRxSet = m_RxSet;
	for( int i = 0; i < RXMAX; i++, pRxSet++ ){
	    pRxSet->SetSampleFreq(DEMSAMPFREQ);
    }
    m_FFTWindow = m_FFTW * FFT_SIZE / m_FFTSampFreq;
	InitCollect();
	m_HPF.Create(ffHPF, 300, SAMPFREQ, 2, 1, 0.3);
    m_ModFSK.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
#if DEBUG
    m_ModTest.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
    m_VCOTest.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
	m_TestHPF.Create(ffHPF, 700, SAMPFREQ+SAMPTXOFFSET, 3, 0, 0);
	if( sys.m_testCarrier1 ) m_BPF500.Create(96, ffBPF, SAMPFREQ+SAMPTXOFFSET, sys.m_testCarrier1-250, sys.m_testCarrier1+250, 60.0, 1.0);
#endif
	SetNotchFreq();
	if( sys.m_fPlayBack ) m_PlayBack.Init(m_BufferSize, SAMPBASE);

    SampleStatus();
    if( fi ) OpenSound(FALSE);
	if( fo ) ReOutOpen();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetTxOffset(double f)
{
	if( SAMPTXOFFSET == f ) return;

	SAMPTXOFFSET = f;
    m_ModFSK.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
#if DEBUG
    m_ModTest.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
    m_VCOTest.SetSampleFreq(SAMPFREQ+SAMPTXOFFSET);
	if( sys.m_testCarrier1 ) m_BPF500.Create(96, ffBPF, SAMPFREQ+SAMPTXOFFSET, sys.m_testCarrier1-250, sys.m_testCarrier1+250, 60.0, 1.0);
#endif
    SampleStatus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetRXFifo(int d)
{
	if( d == m_Wave.m_InFifoSize ) return;

    BOOL f = m_Wave.IsInOpen();
    m_Wave.InClose();
    m_Wave.m_InFifoSize = d;
    if( f ) OpenSound(FALSE);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetTXFifo(int d)
{
	if( d == m_Wave.m_OutFifoSize ) return;

    BOOL f = m_Wave.IsOutOpen();
    m_Wave.OutAbort();
    m_Wave.m_OutFifoSize = d;
    if( f ) ReOutOpen();
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::ReOutOpen(void)
{
	if( !OpenSound(TRUE) ) return FALSE;
    memset(m_wBuffer, 0, sizeof(m_wBuffer));
#if 1
	while(!m_Wave.IsOutBufFull()) m_Wave.OutWrite(m_wBuffer);
#else
    for( int i = 0; i < m_Wave.m_OutFifoSize; i++ ) m_Wave.OutWrite(m_wBuffer);
#endif
    return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetSoundCard(int ch, LPCSTR pID)
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
            f = strcmp(sys.m_SoundMMW.c_str(), tt);
            sys.m_SoundMMW = tt;
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
		if( m_TX ) ToRX();
		BOOL f = m_Wave.IsInOpen();
		m_Wave.InClose();
        m_Wave.m_SoundStereo = ch;
        m_Wave.m_SoundID = id;
        m_Wave.m_SoundTxID = idtx;
        InitWFX();
	    if( f ) OpenSound(FALSE);
    }
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::OpenSound(BOOL fTX)
{
	m_Wave.m_hWnd = Handle;
	m_Wave.UpdateDevice(m_Wave.m_SoundID);
	BOOL r;
	if( fTX ){
		r = m_Wave.OutOpen(&m_WFX, m_Wave.m_SoundTxID, m_BufferSize);
    }
    else {
		r = m_Wave.InOpen(&m_WFX, m_Wave.m_SoundID, m_BufferSize);
    }
    if( r || m_pSoundTimer || m_fSuspend ) return r;

	// サウンドカードリカバリタイマーを起動
//    m_fShowMsg = TRUE;
	m_SoundMsgTimer = m_Wave.GetTimeout();
	m_RecoverSoundMode = fTX;
	m_pSoundTimer = new TTimer(this);
    m_pSoundTimer->OnTimer = SoundTimer;
    m_pSoundTimer->Interval = 1000;
    return r;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateUI(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateLogPanel(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OpenCom(void)
{
	if( m_pCom ){
    	delete m_pCom;
        m_pCom = NULL;
    }
	if( strcmpi(sys.m_PTTCOM.c_str(), "NONE") ){
	    m_pCom = new CComm();
    	if( m_pCom->Open(sys.m_PTTCOM.c_str()) ){
			m_pCom->SetPTT(m_TX);
    	}
    	else {
			delete m_pCom;
    	    m_pCom = NULL;
    	}
    }
	DrawStatus(statusCOM);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OpenRadio(void)
{
	if( m_pRadio ){
    	delete m_pRadio;
        m_pRadio = NULL;
    }
    if( m_pRadioTimer ){
		delete m_pRadioTimer;
        m_pRadioTimer = NULL;
    }
	if( strcmp(RADIO.StrPort, "NONE" ) ){
		m_pRadio = new CCradio(TRUE);
		m_pRadio->Open(&RADIO, Handle, CM_CMMR, CM_CRADIO);
		if( m_pRadio->m_CreateON != TRUE ){
			delete m_pRadio;
			m_pRadio = NULL;
			ErrorMB( sys.m_MsgEng ? "Could not open '%s' for Radio commands":"Radioコマンドポート'%s'がオープンできません.", RADIO.StrPort);
		}
		else {
			m_pRadio->SendCommand(RADIO.CmdInit.c_str());
			if( RADIO.PollScan ){
				m_fDisEvent++;
//				LogFreq->Text = "???";
				m_fDisEvent--;
				RADIO.Cmdxx = 0x00;
				m_pRadio->m_ScanAddr = 1;
			}
			if( RADIO.PollType ){
    	        m_pRadioTimer = new TTimer(this);
	            m_pRadioTimer->OnTimer = RadioTimer;
				int intval = ((RADIO.PollInterval + 2) * 100)/2;
        	    m_pRadioTimer->Interval = intval;
            }
		}
	}
	RADIO.change = 0;
}
//---------------------------------------------------------------------------
// サウンドカード初期化パラメータの生成
void __fastcall TMainVARI::InitWFX(void)
{
	m_WFX.wFormatTag = WAVE_FORMAT_PCM;
	m_WFX.nChannels = WORD(m_Wave.m_SoundStereo ? 2 : 1);
	m_WFX.wBitsPerSample = 16;
	m_WFX.nSamplesPerSec = SAMPBASE;
	m_WFX.nBlockAlign = WORD(m_WFX.nChannels *(m_WFX.wBitsPerSample/8));
	m_WFX.nAvgBytesPerSec = m_WFX.nBlockAlign * m_WFX.nSamplesPerSec;
	m_WFX.cbSize = 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxFFTPaint(TObject *Sender)
{
	PBoxFFT->Canvas->Draw(0, 0, m_pBitmapFFT);
    if( !SBWave->Down && (m_fSubWindow || m_fNOTCH) ){
		DrawSubChannel(PBoxFFT);
    }
	DrawErrorMsg(PBoxFFT->Canvas, m_fftXW, m_fftYW, TRUE);
//	if( SBWave->Down && (m_fftMX > 0) ){
	if( m_fftMX > 0 ){
		TCanvas *pCanvas = PBoxFFT->Canvas;
		pCanvas->Pen->Style = psDot;
		pCanvas->Pen->Color = clYellow;
        pCanvas->Brush->Color = clBlack;
		pCanvas->MoveTo(m_fftMX, 0);
		pCanvas->LineTo(m_fftMX, m_fftYW);
		pCanvas->Pen->Style = psSolid;
        if( SBWave->Down ){
			sprintf(m_TextBuff, "%.1lfms", 1000.0 * m_fftMX * m_Collect1.GetMax() / (m_fftXW * DEMSAMPFREQ) );
        }
        else {
			sprintf(m_TextBuff, "%uHz", m_RightFreq);
        }
		DrawMessage(pCanvas, PBoxFFT->Width, PBoxFFT->Height, m_TextBuff, m_fftMX);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxLevelPaint(TObject *Sender)
{
	PBoxLevel->Canvas->Draw(0, 0, m_pBitmapLevel);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::Draw(BOOL fPaint)
{
	if( SBWave->Down ){
		DrawWave(fPaint);
    }
	else if( SBFFT->Down ){
		DrawFFT(fPaint);
    }
    else {
		DrawWater(fPaint, TRUE);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CalcFFTCenter(int fo)
{
	if( m_FFTW == 3000 ){
		m_FFTB = 0;
        m_FFTU = m_FFTB + m_FFTW;
    }
    int fb = fo - m_FFTW/2;
    if( fb < 0 ) fb = 0;
	fb /= 100;
    fb *= 100;
	if( m_FFTW == 3000 ){
		fb = 0;
    }
    else if( (fb + m_FFTW) >= 3000 ){
		fb = 3000 - m_FFTW;
    }
    m_FFTB = fb;
    m_FFTU = m_FFTB + m_FFTW;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CalcFFTFreq(void)
{
	int fo = m_RxSet[0].m_pDem->m_CarrierFreq;
	int edge = 100;
    if( m_RxSet[0].IsMFSK() ){
        edge = 200;
		if( !sys.m_MFSK_Center ){
			int off = m_RxSet[0].m_pDem->m_MFSK_BW/2;
			if( m_RxSet[0].m_Mode == MODE_mfsk_L ){
        	   	fo -= off;
	        }
    	    else {
				fo += off;
	        }
        }
    }
    else if( m_RxSet[0].IsRTTY() ){
		edge = 200;
    }
	if( m_FFTW == 3000 ){
		m_FFTB = 0;
        m_FFTU = m_FFTB + m_FFTW;
    }
	if( (m_FFTB >= 0) && ((m_FFTB+m_FFTW) <= 3000 ) ){
	    if( (fo > m_FFTB+edge) && (fo < (m_FFTB+m_FFTW)-edge) ){
			return;
    	}
    }
	CalcFFTCenter(fo);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawSubChannel(TPaintBox *pBox)
{
	TCanvas *pCanvas = pBox->Canvas;
    int fw = pCanvas->TextWidth("2");
	int fh = pCanvas->TextHeight("2");
	int ffw = fw - 2;
    int ffh = fh - 3;
	POINT pt[6];
	char bf[256];
	CRxSet *pRxSet = &m_RxSet[1];
	for( int i = 1; i < RXMAX; i++, pRxSet++ ){
		if( pRxSet->IsActive() ){
			int fo = pRxSet->m_pDem->m_CarrierFreq + 0.5;
			int xx = 0.5 + (fo - m_FFTB) * m_fftXW / m_FFTW;
			pt[0].x = xx - ffw; pt[0].y = 0;
            pt[1].x = xx + ffw; pt[1].y = 0;
            pt[2].x = xx + ffw; pt[2].y = ffh;
            pt[3].x = xx; pt[3].y = ffh + 3;
            pt[4].x = xx - ffw; pt[4].y = ffh;
            pt[5].x = xx - ffw; pt[5].y = 0;
            pCanvas->Brush->Color = clWhite;
            pCanvas->Pen->Color = clRed;
           	pCanvas->Polygon(pt, 5);
            pRxSet->m_X = xx;
            pRxSet->m_Y = fh/2;
			sprintf(bf, "%u", i);
            fw = pCanvas->TextWidth(bf);
			xx -= fw/2;
			::SetBkMode(pCanvas->Handle, TRANSPARENT);
			pCanvas->TextOut(xx, 0, bf);
        }
    }
    if( m_fNOTCH ){
		int fo = m_NotchFreq;
		int xx = 0.5 + (fo - m_FFTB) * m_fftXW / m_FFTW;
		pt[0].x = xx - ffw; pt[0].y = 0;
        pt[1].x = xx + ffw; pt[1].y = 0;
        pt[2].x = xx + ffw; pt[2].y = ffh;
        pt[3].x = xx; pt[3].y = ffh + 3;
        pt[4].x = xx - ffw; pt[4].y = ffh;
        pt[5].x = xx - ffw; pt[5].y = 0;
        pCanvas->Brush->Color = clYellow;
        pCanvas->Pen->Color = clRed;
      	pCanvas->Polygon(pt, 5);
        m_NotchX = xx;
        m_NotchY = fh/2;
        strcpy(bf, "N");
        fw = pCanvas->TextWidth(bf);
		xx -= fw/2;
		::SetBkMode(pCanvas->Handle, TRANSPARENT);
		pCanvas->TextOut(xx, 0, bf);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawFreqScale(TCanvas *pCanvas, int XW, int YW,  int fftb, int fftw, int fh, BOOL fRadio)
{
    int fw;
	char bf[128];
	int A, B, L;
    switch(fftw){
		case 500:
        	B = 50; A = 100; L = 100;
            break;
		case 1000:
			B = 50; A = 100; L = 200;
        	break;
        case 2000:
			B = 100; A = 500; L = 500;
        	break;
        case 3000:
			B = 100; A = 500; L = 500;
        	break;
    }
	BOOL fReal = fRadio && m_ScaleAsRigFreq && m_pRadio && m_pRadio->m_FreqHz;
    BOOL fLSB = (fReal && m_pRadio->IsRigLSB());
    UINT f, fb, fu;
    if( fReal ){
    	int rfo = m_pRadio->m_CarrierFreq;
		if( fLSB ){
		    fb = m_pRadio->m_FreqHz + rfo - m_FFTB;
    		fu = fb - fftw;
        }
        else {
		    fb = m_pRadio->m_FreqHz - rfo + m_FFTB;
    		fu = fb + fftw;
        }
    }
    else {
		fb = fftb;
    	fu = fftb+fftw;
    }
	int xx;
	f = fb - (fb % A);
  	if( !fReal && (int(f) < 0) ) f = 0;
#if 0	// For Debug
	sprintf(m_TextBuff, "%u,%u,%u,%u", m_pRadio ? m_pRadio->m_FreqHz:0, fb, fu, f);
    Caption = m_TextBuff;
#endif
	int _yt;
	TColor LCol, FCol;
	if( m_ScaleDetails ){
		LCol = pCanvas->Pen->Color;
    	FCol = pCanvas->Font->Color;
        _yt = 2;
    }
    else {
    	B = A;
        _yt = 0;
    }
	int vy = pCanvas->TextHeight("0") + _yt;
	if( fLSB ){
		for( ; f > fu; f -= B ){
		    xx = int(fb - f) * XW / fftw;
    	    if( xx >= XW ) break;
			if( !(f % 1000) || !(f % L) ){
				sprintf(bf, "%.1lf", double(f) * 0.001);
	            fw = pCanvas->TextWidth(bf)/2;
    	        if( (xx - fw) > 0 ){
                	pCanvas->TextOut(xx - fw, _yt, bf);
	    			if( fh ){
                    	pCanvas->MoveTo(xx, fh); pCanvas->LineTo(xx, YW);
                    }
                    else {
                    	pCanvas->MoveTo(xx, vy); pCanvas->LineTo(xx, vy+3);
                    }
                }
	        }
            else if( fh && !(f % A) ){
    			pCanvas->MoveTo(xx, _yt); pCanvas->LineTo(xx, YW);
            }
            if( m_ScaleDetails && !(f % B) ){
				pCanvas->Pen->Color = FCol;
				pCanvas->MoveTo(xx, 0); pCanvas->LineTo(xx, 2);
				pCanvas->Pen->Color = LCol;
            }
    	}
	}
    else {
		for( ; f < fu; f += B ){
		    xx = int(f - fb) * XW / fftw;
        	if( xx >= XW ) break;
			if( !(f % 1000) || !(f % L) ){
				if( fReal ){
					sprintf(bf, "%.1lf", double(f) * 0.001);
	            }
    	        else {
					sprintf(bf, "%u", f);
	            }
    	        fw = pCanvas->TextWidth(bf)/2;
        	    if( (xx - fw) > 0 ){
                	pCanvas->TextOut(xx - fw, _yt, bf);
	    			if( fh ){
                    	pCanvas->MoveTo(xx, fh); pCanvas->LineTo(xx, YW);
                    }
                    else {
                    	pCanvas->MoveTo(xx, vy); pCanvas->LineTo(xx, vy+3);
                    }
                }
    	    }
            else if( fh && !(f % A) ){
    			pCanvas->MoveTo(xx, _yt); pCanvas->LineTo(xx, YW);
            }
            if( m_ScaleDetails && !(f % B) ){
				pCanvas->Pen->Color = FCol;
				pCanvas->MoveTo(xx, 0); pCanvas->LineTo(xx, 2);
				pCanvas->Pen->Color = LCol;
            }
	    }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetInfoMsg(LPCSTR pStr)
{
	SetInfoMsg(pStr, 1);
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetMsgCount(void)
{
	if( SBWave->Down ){
		int n = DEMSAMPFREQ * 3 / m_Collect1.GetMax();
        if( !n ) n++;
        return n;
    }
    else {
		return 15;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetInfoMsg(LPCSTR pStr, int pos)
{
	if( (pos >= 0) && (pos < 5) ){
		m_InfoMsg[pos] = pStr;
		m_InfoMsgFlag |= _tBitData[pos];
		m_cInfoMsg[pos] = GetMsgCount();
    	m_fShowMsg = TRUE;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetErrorMsg(LPCSTR pStr)
{
	m_ErrorMsg = pStr;
	m_cErrorMsg = GetMsgCount();
    m_fShowMsg = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawErrorMsg(TCanvas *pCanvas, int XW, int YW, BOOL fReset)
{
	LPCSTR p = NULL;
	if( m_fShowMsg ){
		if( m_fSuspend ){
			p = sys.m_MsgEng ? "Suspend..." : "サスペンド中...";
        }
		else if( m_pSoundTimer ){
	        p = sys.m_MsgEng ? "SoundCard open error. Now trying to open every 1sec." : "サウンドカードオープンエラー, 再試行中(1秒毎)...";
	    }
		else if( m_LostSoundRX ){
			if( fReset ) m_LostSoundRX--;
    	    p = sys.m_MsgEng ? "Lost RX Sound":"受信サウンド欠落";
	    }
		else if( m_LostSoundTX ){
			if( fReset ) m_LostSoundTX--;
    	    p = sys.m_MsgEng ? "Lost TX Sound":"送信サウンド欠落";
	    }
		else if( m_cErrorMsg ){
        	m_cErrorMsg--;
            p = m_ErrorMsg.c_str();
		}
		else if( m_InfoMsgFlag ){
			for( int i = 0; i < 5; i++ ){
				if( m_InfoMsgFlag & _tBitData[i] ){
			        DrawMessage(pCanvas, XW, YW, m_InfoMsg[i].c_str(), i);
		        	m_cInfoMsg[i]--;
                    if( !m_cInfoMsg[i] ) m_InfoMsgFlag &= ~_tBitData[i];
                }
            }
        }
        else {
			m_fShowMsg = FALSE;
        }
	}
    else if( m_ScaleAsRigFreq && m_pRadio ){
		if( fReset ) m_RadioScaleCounter++;
        if( m_RadioScaleCounter & 4 ){
			if( m_pRadio->m_FreqHz ){
				if( (RADIO.PollOffset == 3) && !m_pRadio->m_LSB ){
					p = sys.m_MsgEng ? "Invalid RIG MODE":"未対応のRIGモード";
                }
    	    }
        	else {
				p = sys.m_MsgEng ? "NO RIG DATA" : "RIGデータ未受信";
	        }
        }
    }
#if 0
    if( !p && !SBAFC->Down && m_RxSet[0].m_SQ && m_RxSet[0].m_pDem->IsFreqErr() ){
		DrawMessage(pCanvas, XW, YW, sys.m_MsgEng ? "OFF FREQ":"Fズレ", 3);
    }
#endif
	if( p ) DrawMessage(pCanvas, XW, YW, p, 3);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawFFT(BOOL fPaint)
{
    TCanvas *pCanvas = m_pBitmapFFT->Canvas;
    TRect rc;
    rc.Left = 0; rc.Top = 0;
    rc.Right = m_fftXW; rc.Bottom = m_fftYW;
    pCanvas->Brush->Color = m_tFFTColset[0].c;
    pCanvas->Pen->Color = m_tFFTColset[0].c;
    pCanvas->FillRect(rc);

	if( !m_MouseDown ) CalcFFTFreq();
	pCanvas->Pen->Style = psDot;

    int xx, y;
    int fh = pCanvas->TextHeight("A");

	switch(m_FFTVType){
		case 0:
			pCanvas->Pen->Color = m_tFFTColset[3].c;
			for( y = 6; y <= 86; y += 20 ){
				xx = y * m_fftYW / 100;
    			pCanvas->MoveTo(0, xx); pCanvas->LineTo(m_fftXW, xx);
    		}
        	break;
        case 1:
			pCanvas->Pen->Color = m_tFFTColset[3].c;
			for( y = 6; y <= 66; y += 20 ){
				xx = y * m_fftYW / 60;
    			pCanvas->MoveTo(0, xx); pCanvas->LineTo(m_fftXW, xx);
    		}
        	break;
    }
	pCanvas->Font->Height = -12;
	pCanvas->Font->Color = m_tFFTColset[2].c;
	pCanvas->Pen->Color = m_tFFTColset[3].c;
	int rfo = UdRxCarrier->Position;
	DrawFreqScale(pCanvas, m_fftXW, m_fftYW, m_FFTB, m_FFTW, fh, TRUE);

    if( m_RxSet[0].m_pDem->GetSyncState() && m_RxSet[0].m_SQ && m_RxSet[0].m_pDem->m_Lock ){
//		DrawMessage(pCanvas, m_fftXW, m_fftYW, "SYNC", 1);
		pCanvas->TextOut(1, m_ScaleDetails ? 3 : 1, "SYNC");
    }
//	pCanvas->Pen->Color = clYellow;
//	xx = m_Hilbert.m_CarrierFreq * m_fftXW / m_FFTW;
//   	pCanvas->MoveTo(xx, 0); pCanvas->LineTo(xx, m_fftYW);
	pCanvas->Pen->Color = clGreen;
	xx = (DEMSAMPFREQ*0.5 - m_FFTB) * m_fftXW / m_FFTW;
   	pCanvas->MoveTo(xx, fh); pCanvas->LineTo(xx, m_fftYW);
    int tfo = UdTxCarrier->Position;
	POINT pt[4];
	if( (tfo != rfo) && !SBNET->Down ){
		pCanvas->Pen->Color = m_tFFTColset[5].c;
		xx = 0.5 + (tfo - m_FFTB) * m_fftXW / m_FFTW;
		pCanvas->MoveTo(xx, fh); pCanvas->LineTo(xx, m_fftYW);
		pt[0].x = xx; pt[0].y = fh;
    	pt[1].x = xx - 3; pt[1].y = fh+3;
    	pt[2].x = xx; pt[2].y = fh + 6;
    	pt[3].x = xx + 3; pt[3].y = fh+3;
		pCanvas->Brush->Color = m_tFFTColset[5].c;
		pCanvas->Polygon(pt, 3);
		if( Is170() ){
			double hw = m_RxSet[0].m_pDem->m_RTTYShift * 0.5;
			xx = 0.5 + (tfo - m_FFTB - hw) * m_fftXW / m_FFTW;
	        pCanvas->MoveTo(xx, fh+5);
    	    pCanvas->LineTo(xx, fh+3);
			xx = 0.5 + (tfo - m_FFTB + hw) * m_fftXW / m_FFTW;
	        pCanvas->LineTo(xx, fh+3);
    	    pCanvas->LineTo(xx, fh+6);
	    }
	    else if( m_RxSet[0].IsMFSK() ){
			xx = 0.5 + (tfo - m_FFTB) * m_fftXW / m_FFTW;
        	pCanvas->MoveTo(xx, fh+5);
        	pCanvas->LineTo(xx, fh+3);
			double bw = m_RxSet[0].m_pDem->m_MFSK_BW;
			if( sys.m_MFSK_Center ){
				int x = 0.5 + (tfo - m_FFTB - bw*0.5) * m_fftXW / m_FFTW;
    	    	pCanvas->LineTo(x, fh+3);
        		pCanvas->LineTo(x, fh+6);
				x = 0.5 + (tfo - m_FFTB + bw*0.5) * m_fftXW / m_FFTW;
	        	pCanvas->MoveTo(xx, fh+3);
    	    	pCanvas->LineTo(x, fh+3);
        		pCanvas->LineTo(x, fh+6);
            }
            else {
				xx = 0.5 + (tfo - m_FFTB + (m_RxSet[0].m_Mode == MODE_mfsk_U ? bw : -bw)) * m_fftXW / m_FFTW;
    	    	pCanvas->LineTo(xx, fh+3);
        		pCanvas->LineTo(xx, fh+6);
            }
    	}
    }
	pCanvas->Pen->Color = m_tFFTColset[4].c;
	xx = 0.5 + (rfo - m_FFTB) * m_fftXW / m_FFTW;
   	pCanvas->MoveTo(xx, fh); pCanvas->LineTo(xx, m_fftYW);
	pt[0].x = xx; pt[0].y = fh;
    pt[1].x = xx - 3; pt[1].y = fh+3;
    pt[2].x = xx; pt[2].y = fh + 6;
    pt[3].x = xx + 3; pt[3].y = fh+3;
	pCanvas->Brush->Color = m_tFFTColset[4].c;
	pCanvas->Polygon(pt, 3);
	if( Is170() ){
		double hw = m_RxSet[0].m_pDem->m_RTTYShift * 0.5;
		xx = 0.5 + (rfo - m_FFTB - hw) * m_fftXW / m_FFTW;
        pCanvas->MoveTo(xx, fh+5);
        pCanvas->LineTo(xx, fh+3);
		xx = 0.5 + (rfo - m_FFTB + hw) * m_fftXW / m_FFTW;
        pCanvas->LineTo(xx, fh+3);
        pCanvas->LineTo(xx, fh+6);
    }
    else if( m_RxSet[0].IsMFSK() ){
		xx = 0.5 + (rfo - m_FFTB) * m_fftXW / m_FFTW;
        pCanvas->MoveTo(xx, fh+5);
        pCanvas->LineTo(xx, fh+3);
		double bw = m_RxSet[0].m_pDem->m_MFSK_BW;
		if( sys.m_MFSK_Center ){
			int x = 0.5 + (rfo - m_FFTB - bw*0.5) * m_fftXW / m_FFTW;
   	    	pCanvas->LineTo(x, fh+3);
       		pCanvas->LineTo(x, fh+6);
			x = 0.5 + (rfo - m_FFTB + bw*0.5) * m_fftXW / m_FFTW;
        	pCanvas->MoveTo(xx, fh+3);
   	    	pCanvas->LineTo(x, fh+3);
       		pCanvas->LineTo(x, fh+6);
        }
		else {
			xx = 0.5 + (rfo - m_FFTB + (m_RxSet[0].m_Mode == MODE_mfsk_U ? bw : -bw)) * m_fftXW / m_FFTW;
    	    pCanvas->LineTo(xx, fh+3);
        	pCanvas->LineTo(xx, fh+6);
        }
    }
    if( (m_Wave.IsInOpen() || m_Wave.IsOutOpen()) && SBFFT->Down ){
		pCanvas->Pen->Style = psSolid;
		pCanvas->Pen->Color = m_tFFTColset[1].c;

    	int x, y;
		int xo = ((m_FFTB+(m_FFTSampFreq/FFT_SIZE))*FFT_SIZE/m_FFTSampFreq) + 0.5;
		switch(m_FFTVType){
			case 0:{			// 100dB
				double k = m_fftYW*0.01/m_fftSC;
		    	for( x = 0; x < m_fftXW; x++ ){
					xx = xo + (x * m_FFTWindow / m_fftXW);
        		    y = m_fftout[xx];
					y = m_fftYW - y * k;
					if( x ){
		    		    pCanvas->LineTo(x, y);
        	    	}
            		else {
						pCanvas->MoveTo(x, y);
	            	}
    			}
        	    break;}
			case 1:{			// 60dB
				double k = double(m_fftYW)/double(m_fftSC);
				int yo = m_fftYW * 40 / k;
                k = k / 60.0;
		    	for( x = 0; x < m_fftXW; x++ ){
					xx = xo + (x * m_FFTWindow / m_fftXW);
        		    y = m_fftout[xx];
					y = m_fftYW - ((y-yo)*k);
					if( x ){
		    		    pCanvas->LineTo(x, y);
        	    	}
            		else {
						pCanvas->MoveTo(x, y);
	            	}
    			}
        	    break;}
            default:{			// SQR
		    	for( x = 0; x < m_fftXW; x++ ){
					xx = xo + (x * m_FFTWindow / m_fftXW);
        		    y = m_fftout[xx];
					y = m_fftYW - (y/100);
					if( x ){
		    		    pCanvas->LineTo(x, y);
        	    	}
            		else {
						pCanvas->MoveTo(x, y);
    	        	}
    			}
            	break;}
        }
    }
    if( fPaint ) PBoxFFTPaint(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::InitWater(int sw)
{
	if( m_WaterNoiseL > m_WaterNoiseH ){
		int f = m_WaterNoiseL;
        m_WaterNoiseL = m_WaterNoiseH;
        m_WaterNoiseH = f;
	}
	if( m_FFT.m_FFTGain ){
		if( sw & iniwMETRIC ){
	        m_StgWater.Sum = 1000;
   		    m_StgWater.Max = 5120;
       		m_StgWater.VW = 5120;
        }
		if( sw & iniwLIMIT ){
	        m_StgWater.LimitL = DBToSqrt(m_WaterNoiseL*100-500);
    	    m_StgWater.LimitH = DBToSqrt(m_WaterNoiseH*100-500);
        }
    }
    else {
		if( sw & iniwMETRIC ){
	        m_StgWater.Sum = 5000;
   		    m_StgWater.Max = 8000;
       		m_StgWater.VW = 6000;
        }
		if( sw & iniwLIMIT ){
	        m_StgWater.LimitL = m_WaterNoiseL * 100 - 500;
    	    m_StgWater.LimitH = m_WaterNoiseH * 100 - 500;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawWater(BOOL fPaint, BOOL fClear)
{
    TCanvas *pCanvas = m_pBitmapFFT->Canvas;
    int fh = pCanvas->TextHeight("A");
    int ft = fh + 8;

    int b = m_FFTB;
    int fo = UdRxCarrier->Position;
	if( !m_MouseDown ) CalcFFTFreq();

    TRect rc;
   	rc.Left = 0; rc.Top = 0; rc.Right = m_fftXW;
	if( fClear || (b != m_FFTB)){
        rc.Bottom = m_fftYW;
        InitWater(iniwMETRIC);
    }
    else {
		rc.Bottom = ft;
    }
   	pCanvas->Brush->Color = m_tWaterColset[0].c;
   	pCanvas->Pen->Color = m_tWaterColset[0].c;
	pCanvas->Pen->Style = psSolid;
   	pCanvas->FillRect(rc);
    rc.Bottom = m_fftYW;

	pCanvas->Font->Height = -12;
	pCanvas->Font->Color = m_tWaterColset[2].c;
    pCanvas->Pen->Color = m_tWaterColset[2].c;
	DrawFreqScale(pCanvas, m_fftXW, m_fftYW, m_FFTB, m_FFTW, 0, TRUE);

    if( m_RxSet[0].m_pDem->GetSyncState() && m_RxSet[0].m_SQ && m_RxSet[0].m_pDem->m_Lock ){
		pCanvas->TextOut(1, m_ScaleDetails ? 3 : 1, "SYNC");
    }

    int xx;
	POINT pt[3];
	int tfo = UdTxCarrier->Position;
	if( tfo != fo && !SBNET->Down ){
		if( Is170() ){
			double hw = m_RxSet[0].m_pDem->m_RTTYShift * 0.5;
			pCanvas->Pen->Color = m_tWaterColset[5].c;
			xx = 0.5 + (tfo - m_FFTB - hw) * m_fftXW / m_FFTW;
	        pCanvas->MoveTo(xx, fh+5);
    	    pCanvas->LineTo(xx, fh+3);
			xx = 0.5 + (tfo - m_FFTB + hw) * m_fftXW / m_FFTW;
    	    pCanvas->LineTo(xx, fh+3);
        	pCanvas->LineTo(xx, fh+6);
	    }
	    else if( m_RxSet[0].IsMFSK() ){
			pCanvas->Pen->Color = m_tWaterColset[5].c;
			xx = 0.5 + (tfo - m_FFTB) * m_fftXW / m_FFTW;
	        pCanvas->MoveTo(xx, fh+5);
    	    pCanvas->LineTo(xx, fh+3);
			double bw = m_RxSet[0].m_pDem->m_MFSK_BW;
			if( sys.m_MFSK_Center ){
				int x = 0.5 + (tfo - m_FFTB - bw*0.5) * m_fftXW / m_FFTW;
   	    		pCanvas->LineTo(x, fh+3);
       			pCanvas->LineTo(x, fh+6);
				x = 0.5 + (tfo - m_FFTB + bw*0.5) * m_fftXW / m_FFTW;
	        	pCanvas->MoveTo(xx, fh+3);
   	    		pCanvas->LineTo(x, fh+3);
       			pCanvas->LineTo(x, fh+6);
	        }
			else {
				xx = 0.5 + (tfo - m_FFTB + (m_RxSet[0].m_Mode == MODE_mfsk_U ? bw : -bw)) * m_fftXW / m_FFTW;
		        pCanvas->LineTo(xx, fh+3);
    		    pCanvas->LineTo(xx, fh+6);
            }
	    }
		xx = 0.5 + (tfo - m_FFTB) * m_fftXW / m_FFTW;
	    pt[0].x = xx - 4; pt[0].y = fh;
	    pt[1].x = xx + 4; pt[1].y = fh;
	    pt[2].x = xx; pt[2].y = fh + 6;
		pCanvas->Pen->Color = m_tWaterColset[3].c;
		pCanvas->Brush->Color = m_tWaterColset[5].c;
		pCanvas->Polygon(pt, 2);
    }
	pCanvas->Pen->Color = m_tWaterColset[3].c;
	if( Is170() ){
		double hw = m_RxSet[0].m_pDem->m_RTTYShift * 0.5;
		pCanvas->Pen->Color = m_tWaterColset[4].c;
		xx = 0.5 + (fo - m_FFTB - hw) * m_fftXW / m_FFTW;
        pCanvas->MoveTo(xx, fh+5);
        pCanvas->LineTo(xx, fh+3);
		xx = 0.5 + (fo - m_FFTB + hw) * m_fftXW / m_FFTW;
        pCanvas->LineTo(xx, fh+3);
        pCanvas->LineTo(xx, fh+6);
    }
    else if( m_RxSet[0].IsMFSK() ){
		pCanvas->Pen->Color = m_tWaterColset[4].c;
		xx = 0.5 + (fo - m_FFTB) * m_fftXW / m_FFTW;
        pCanvas->MoveTo(xx, fh+5);
        pCanvas->LineTo(xx, fh+3);
		double bw = m_RxSet[0].m_pDem->m_MFSK_BW;
		if( sys.m_MFSK_Center ){
			int x = 0.5 + (fo - m_FFTB - bw*0.5) * m_fftXW / m_FFTW;
    		pCanvas->LineTo(x, fh+3);
   			pCanvas->LineTo(x, fh+6);
			x = 0.5 + (fo - m_FFTB + bw*0.5) * m_fftXW / m_FFTW;
        	pCanvas->MoveTo(xx, fh+3);
    		pCanvas->LineTo(x, fh+3);
   			pCanvas->LineTo(x, fh+6);
        }
		else {
			xx = 0.5 + (fo - m_FFTB + (m_RxSet[0].m_Mode == MODE_mfsk_U ? bw : -bw)) * m_fftXW / m_FFTW;
    	    pCanvas->LineTo(xx, fh+3);
        	pCanvas->LineTo(xx, fh+6);
        }
    }
	xx = 0.5 + (fo - m_FFTB) * m_fftXW / m_FFTW;
    pt[0].x = xx - 4; pt[0].y = fh;
    pt[1].x = xx + 4; pt[1].y = fh;
    pt[2].x = xx; pt[2].y = fh + 6;
	pCanvas->Pen->Color = m_tWaterColset[3].c;
	pCanvas->Brush->Color = m_tWaterColset[4].c;
	pCanvas->Polygon(pt, 2);

    if( (m_Wave.IsInOpen() || m_Wave.IsOutOpen()) && SBWater->Down ){
		TRect trc = rc;
		rc.Top += ft;
        rc.Bottom--;
		trc.Top += ft + 1;
		pCanvas->CopyRect(trc, pCanvas, rc);
		int xo = ((m_FFTB+(m_FFTSampFreq/FFT_SIZE))*FFT_SIZE/m_FFTSampFreq) + 0.5;
    	int x, y;
		int n = 0;
		int sum = m_StgWater.LimitL;
        int max = 0;
        int wmax = 0;
		int xl = 0.5 + (fo - m_RxSet[0].m_Speed - m_FFTB) * m_fftXW / m_FFTW;
		int xh = 0.5 + (fo + m_RxSet[0].m_Speed - m_FFTB) * m_fftXW / m_FFTW;
        double k = 256.0 / m_StgWater.VW;
    	for( x = 0; x < m_fftXW; x++ ){
			xx = xo + (x * m_FFTWindow / m_fftXW);
            y = m_fftout[xx];
            if( max < y ) max = y;
			if( (x >= xl) && (x <= xh) ){
	            if( wmax < y ) wmax = y;
            }
			if( (y > m_StgWater.LimitL) && (y < m_StgWater.LimitH) ){
	            sum += y;
   	            n++;
            }
            y = (y - m_StgWater.Sum) * k;
			if( y < 0 ) y = 0;
            if( y >= 256 ) y = 255;
            pCanvas->Pixels[x][ft] = m_tWaterColors[y];
    	}
		if( m_TX == txINTERNAL ){
			sum = m_StgWater.LimitL;
            if( m_RxSet[0].IsMFSK() || m_RxSet[0].Is170() ){
				sum = (sum + m_StgWater.LimitH)/2;
            }
        }
        else if( n < 16 ){
			sum = m_StgWater.LimitL;
        }
        else {
        	sum /= n;
        }
		m_StgWater.Sum = (m_StgWater.Sum + sum) / 2;
        if( m_StgWater.Sum >= m_StgWater.LimitH ) m_StgWater.Sum = m_StgWater.LimitH;
		if( (wmax-sum) >= 320 ){
        	max = wmax;
        }
        m_StgWater.Max = (m_StgWater.Max + max) / 2;
        m_StgWater.VW = m_StgWater.Max - m_StgWater.Sum;
#if DEBUG
//		sprintf(m_TextBuff, "Max:%d, Sum:%d, DB:%d, VW:%d", max, sum, int(DBToSqrt(4000)), m_StgWater.VW);
//        Caption = m_TextBuff;
#endif
		int low, high;
		if( m_fWaterAGC ){		// AGC
			if( m_FFT.m_FFTGain ){
				low = 3000; high = 7000;
            }
            else {
				low = 4000; high = 10000;
            }
        }
        else {
			low = high = m_FFT.m_FFTGain ? 5120 : 5000;
        }
        if( m_TX == txINTERNAL ) high = 100000;
        if( m_StgWater.VW < low ) m_StgWater.VW = low;
        if( m_StgWater.VW > high ) m_StgWater.VW = high;
    }
    if( fPaint ) PBoxFFTPaint(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawWave(BOOL fPaint)
{
    TCanvas *pCanvas = m_pBitmapFFT->Canvas;
    TRect rc;
    rc.Left = 0; rc.Top = 0;
    rc.Right = m_fftXW; rc.Bottom = m_fftYW;
    pCanvas->Brush->Color = clBlack;
    pCanvas->Pen->Color = clBlack;
    pCanvas->FillRect(rc);

	pCanvas->Pen->Color = clGray;
	pCanvas->Pen->Style = psDot;
	int O1 = m_fftYW / 4;
   	pCanvas->MoveTo(0, O1); pCanvas->LineTo(m_fftXW, O1);
	int O2 = m_fftYW * 3 / 4;
   	pCanvas->MoveTo(0, O2); pCanvas->LineTo(m_fftXW, O2);

    if( (m_Wave.IsInOpen() || m_Wave.IsOutOpen()) && SBWave->Down ){
		double k = m_fftYW * 0.3 / 32768.0;
		pCanvas->Pen->Style = psSolid;
		pCanvas->Pen->Color = TColor(RGB(0,255,0));
    	int x, y, xx;
		int W = m_Collect1.GetMax();
		int M = m_Collect1.GetCount();
        double *t1 = m_Collect1.GetZP();
        double *t2 = m_Collect2.GetZP();
    	for( x = 0; x < m_fftXW; x++ ){
			xx = x * W / m_fftXW;
            if( xx >= W ) xx = W - 1;
			if( xx >= M ){
				break;
            }
			y = O1 - (t1[xx] * k);
			if( x ){ pCanvas->LineTo(x, y); } else { pCanvas->MoveTo(x, y); }
    	}
		if( m_WaveType ){
			double d;
			double max = 128;
			for( x = 0; x < M; x++ ){
				d = fabs(t2[x]);
                if( max < d ) max = d;
            }
            k = m_fftYW * 0.2 / max;
			for( x = 0; x < M; x++ ){
				xx = x * m_fftXW / W;
                y = O2 - (t2[x] * k);
				if( x ){ pCanvas->LineTo(xx, y); } else { pCanvas->MoveTo(xx, y); }
            }
        }
        else {
	    	for( x = 0; x < m_fftXW; x++ ){
				xx = x * W / m_fftXW;
        	    if( xx >= W ) xx = W - 1;
				if( xx >= M ){
					break;
    	        }
				y = O2 - (t2[xx] * k);
				if( x ){ pCanvas->LineTo(x, y); } else { pCanvas->MoveTo(x, y); }
	    	}
		}
	    if( m_RxSet[0].m_pDem->GetSyncState() && m_RxSet[0].m_SQ ){
			pCanvas->TextOut(1, 1, "SYNC");
    	}
    }
    if( fPaint ) PBoxFFTPaint(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawLevel(BOOL fPaint)
{
    TCanvas *pCanvas = m_pBitmapLevel->Canvas;
    TRect rc;
    rc.Left = 0; rc.Top = 0;
    rc.Right = m_levelXW; rc.Bottom = m_levelYW;
    pCanvas->Brush->Color = clBlack;
    pCanvas->Pen->Color = clBlack;
    pCanvas->FillRect(rc);

//	pCanvas->Pen->Color = clYellow;
	int d = m_RxSet[0].m_StgFFT.Sig - 500;
	if( (m_TX != txINTERNAL) && m_RxSet[0].IsMFSK() && sys.m_MFSK_SQ_Metric ){
		d = m_RxSet[0].m_pDem->GetMFSKMetric(0);
	}
   	if( d > LEVELMAX ) d = LEVELMAX;
    if( m_Wave.IsInOpen() || m_Wave.IsOutOpen() ){
		if( m_RxSet[0].m_pDem->m_AGC.GetOver() && !m_TX ){
			pCanvas->Brush->Color = clRed;
        }
		else if( !m_RxSet[0].m_pDem->m_Lock ){
			pCanvas->Brush->Color = m_RxSet[0].m_SQ ? clBlue : clGray;
        }
        else {
			TColor col = (m_TX == txINTERNAL) ? clYellow : TColor(RGB(0,255,0));
			pCanvas->Brush->Color = m_RxSet[0].m_SQ ? col : clGray;
        }
    	rc.Top = m_levelYW - (d * m_levelYW / LEVELMAX);
    	pCanvas->FillRect(rc);
    }
    else {
		m_RxSet[0].m_SQ = FALSE;
    }
    pCanvas->Pen->Color = m_RxSet[0].m_SQ ? clBlack : clWhite;
	d = m_levelYW - (m_RxSet[0].m_SQLevel * m_levelYW / LEVELMAX);
	pCanvas->MoveTo(0, d); pCanvas->LineTo(m_levelXW, d);

    if( fPaint ) PBoxLevelPaint(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::RemoveUselessMessage(UINT wParam)
{
	MSG msg;
	while( ::PeekMessage(&msg, Handle, WM_WAVE, WM_WAVE, PM_NOREMOVE) ){
		if( msg.wParam == wParam ){
			::PeekMessage(&msg, Handle, WM_WAVE, WM_WAVE, PM_REMOVE);
#if DEBUG
			RxStatus(m_RxSet, "Remove");
#endif
		}
		else {
			break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnWave(void)
{
	OnWaveIn();
    OnWaveOut();
}
//---------------------------------------------------------------------------
// サウンドカードからのイベント
void __fastcall TMainVARI::OnWaveEvent(TMessage Message)
{
	switch(Message.WParam){
		case waveIN:			// 入力イベント
			OnWaveIn();
        	break;
        case waveOUT:			// 出力イベント
			OnWaveOut();
        	break;
        case waveClockAdj:		// Clock ADJ window
			if( Message.LParam == int(m_pClockView) ){
				delete m_pClockView;
                m_pClockView = NULL;
            }
        	break;
#if DEBUG
        case waveRepeatMacro:
			{
				TSpeedButton *pButton = (TSpeedButton *)Message.LParam;
				pButton->Down = TRUE;
                pButton->Click();
            }
            break;
#endif
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnWaveIn(void)
{
	while(m_Wave.IsInOpen() && !m_Wave.IsInBufNull()){
		if( !m_Wave.InRead(m_wBuffer) ){
			m_Wave.InClose();
        	OpenSound(FALSE);
            m_LostSoundRX = LOSTMSGTIME * int(SAMPFREQ) / m_BufferSize;
            m_fShowMsg = TRUE;
        }
		m_WaveFile.ReadWrite(m_wBuffer, m_BufferSize);
		if( m_PlayBack.IsActive() && !m_TX ){
			if( m_PlayBack.IsPlaying() ){
				for( int i = 0; i < sys.m_PlayBackSpeed; i++ ){
		        	if( m_PlayBack.Read(m_wBuffer) ){
						DoDem();
                    }
                    else {
						break;
                    }
/*
					MSG msg;
					if( ::PeekMessage(&msg, Handle, WM_LBUTTONDOWN, WM_LBUTTONDOWN, PM_NOREMOVE) ){
						break;
                    }
*/
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
void __fastcall TMainVARI::OnWaveOut(void)
{
#if DEBUG
	if( sys.m_test ){
		while(m_Wave.IsOutOpen() && !m_Wave.IsOutBufFull()){
			DoDem();
			if( !m_Wave.OutWrite(m_wBuffer) ){
				m_Wave.OutAbort();
				OpenSound(TRUE);
	            m_LostSoundTX = LOSTMSGTIME * int(SAMPFREQ) / m_BufferSize;
	            m_fShowMsg = TRUE;
			}
		}
		if( m_fpTest ){
			while(!m_ModTest.m_Encode.IsBuffFull()){
				int c = fgetc(m_fpTest);
    			if( m_RxSet[0].m_MBCS.IsLead(BYTE(c)) ){
					c = c << 8;
					c += fgetc(m_fpTest);
				}
				m_ModTest.m_Encode.PutChar(c);
				if( feof(m_fpTest) ){
					rewind(m_fpTest);
				}
            }
		}
	}
	else {
#endif
		while(m_Wave.IsOutOpen() && !m_Wave.IsOutBufFull()){
			if( (m_WaveFile.m_mode == 1) && (!m_TX) ){	// Read func
            	m_WaveFile.ReadWrite(m_wBuffer, m_BufferSize);
                DoDem();
            }
            else {
				DoMod();
				if( (sys.m_LoopBack == loopINTERNAL) ) DoDem();
            }
			if( !m_Wave.OutWrite(m_wBuffer) ){
				m_Wave.OutAbort();
				OpenSound(TRUE);
	            m_LostSoundTX = LOSTMSGTIME * int(SAMPFREQ) / m_BufferSize;
	            m_fShowMsg = TRUE;
			}
			if( m_WaveFile.m_mode == 2 ){	// Write func
            	m_WaveFile.ReadWrite(m_wBuffer, m_BufferSize);
            }
		}
#if DEBUG
	}
#endif
	if( m_fReqRX ){
		if( m_fTone ){
			if( m_fReqRX == 1 ){
                m_ModFSK.m_OutVol = 0;
				m_Wave.SetOutBCC(m_Wave.GetOutBC());
				m_fReqRX++;
				SetTXCaption();
            }
			else if( m_Wave.GetOutBCC() < 0 ){
				ToRX();
			}
        }
//   		else if( m_ModFSK.m_Encode.m_Idle && !m_Edit[m_SendingEdit].GetCharCount(TRUE) && m_Edit[m_SendingEdit].IsCursorLast() ){
   		else if( m_ModFSK.m_Encode.m_Idle ){
			if( m_fReqRX == 1 ){
				m_fReqRX++;
				SetTXCaption();
				if( KOAO->Checked && m_fSendChar ){
    				m_ModFSK.m_Encode.PutChar('\r');
    				m_ModFSK.m_Encode.PutChar('\n');
				}
				else {
					m_ModFSK.m_Encode.SetMark();
					m_Wave.SetOutBCC(m_Wave.GetOutBC());
					m_fReqRX++;
				}
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
void __fastcall TMainVARI::DoDem(double d)
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
	if( SBWave->Down ){
		if( m_WaveType ){
			m_Collect1.Do(pDem->GetS());
			m_Collect2.Do(pDem->m_d);
			if( m_Collect1.IsFull() ){
    		    DrawWave(TRUE);
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
    			    DrawWave(TRUE);
			    	m_Collect1.Clear(); m_Collect2.Clear();
            	    m_Lock = FALSE;
            	}
	        }
    	    else if( m_Lock ){
    		    DrawWave(TRUE);
		    	m_Collect1.Clear(); m_Collect2.Clear();
				m_Lock = FALSE;
        	}
		}
    }
    if( m_fSubWindow ){
		pRxSet = &m_RxSet[1];
	    for( int i = 1; i < RXMAX; i++, pRxSet++ ){
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
void __fastcall TMainVARI::DoDem(void)
{
	int i;
    SHORT *wp = m_wBuffer;

#if DEBUG
	if( sys.m_test ){
		if( m_TestTimer ) m_TestTimer--;
	    for( i = 0; i < m_BufferSize; i++, wp++ ){
			if( !m_TestTimer ){
				if( sys.m_testCarrier1 ) *wp = SHORT(m_QSB.Do(m_ModTest.Do()));
				if( sys.m_testCarrier2 ) *wp += SHORT(m_VCOTest.Do(0)*sys.m_testGain2);
            }
            else {
				*wp = 0;
            }
    	}
		if( SAMPFREQ < 7000.0 ){
			AddGaussian(m_wBuffer, m_BufferSize, sys.m_testNoiseGain*0.5);
		}
        else {
			AddGaussian(m_wBuffer, m_BufferSize, sys.m_testNoiseGain);
        }
	    wp = m_wBuffer;
		for( i = 0; i < m_BufferSize; i++, wp++ ){
			*wp = m_TestHPF.Do(*wp);
			if( sys.m_test500 ) *wp = m_BPF500.Do(*wp);
        }
	    wp = m_wBuffer;
    }
	if( m_pDebugButton ){
		::PostMessage(Handle, WM_WAVE, waveRepeatMacro, DWORD(m_pDebugButton));
        m_pDebugButton = NULL;
    }
#endif
    double *dp = m_fftbuf;
    BOOL fNOTCH = m_fNOTCH && (m_TX != txINTERNAL);
	switch(SAMPTYPE){
        case 0:		// 11025Hz
		    for( i = 0; i < m_BufferSize; i++, wp++ ){
				if( m_fHPF ) *wp = m_HPF.Do(*wp);
				if( fNOTCH ) *wp = m_NOTCH.Do(*wp);
				if( m_Dec2.Do(*wp) ){
					DoDem(m_Dec2.GetOut());
        		}
				*dp++ = *wp;
        	}
        	break;
        case 1:		// 8000Hz
		case 2:		// 6000Hz
		    for( i = 0; i < m_BufferSize; i++, wp++ ){
				if( m_fHPF ) *wp = m_HPF.Do(*wp);
				if( fNOTCH ) *wp = m_NOTCH.Do(*wp);
				DoDem(*wp);
				*dp++ = *wp;
        	}
        	break;
        case 4:		// 16000Hz
        case 6:		// 22050Hz
		    for( i = 0; i < m_BufferSize; i++, wp++ ){
				if( m_fHPF ) *wp = m_HPF.Do(*wp);
				if( fNOTCH ) *wp = m_NOTCH.Do(*wp);
				if( m_Dec2.Do(*wp) ){
					DoDem(m_Dec2.GetOut());
        		}
                if( !(i & 1) ) *dp++ = m_Dec2.GetOut();
        	}
        	break;
        default:
//					12000Hz
//					18000Hz
//					24000Hz
//					44100Hz
//					48000Hz
//					50000Hz
		    for( i = 0; i < m_BufferSize; i++, wp++ ){
				if( m_fHPF ) *wp = m_HPF.Do(*wp);
				if( m_Dec2.Do(*wp) ){
					if( fNOTCH ) m_Dec2.SetOut(m_NOTCH.Do(m_Dec2.GetOut()));
					DoDem(m_Dec2.GetOut());
					*dp++ = m_Dec2.GetOut();
        		}
        	}
        	break;
    }
	try {
	    m_FFT.Calc(m_fftbuf, 3001 * FFT_SIZE / m_FFTSampFreq, m_fftSC * 10.0, m_FFTSmooth, m_fftout);
    }
    catch(...){
		m_FFT.InitFFT();
        m_FFT.m_FFTDIS = 0;
    }
	for( i = 0; i < RXMAX; i++ ){
		CalcStgFFT(&m_RxSet[i]);
    }
	if( SBFFT->Down ){
    	DrawFFT(TRUE);
    }
    else if( SBWater->Down ){
		DrawWater(TRUE, FALSE);
    }
	DrawLevel(TRUE);
    DrawPF(TRUE);

    m_fDisEvent++;
	if( m_TX != txINTERNAL ){
		if( m_AFCKeyTimer ){
        	m_AFCKeyTimer--;
        }
		else if( SBAFC->Down ){
			UdRxCarrier->Position = short(m_RxSet[0].m_pDem->m_CarrierFreq+0.5);
	        if( m_pRadio ) m_pRadio->SetCarrierFreq(UdRxCarrier->Position);
#if 0
            if( !m_TX && SBNET->Down ){
				UdTxCarrier->Position = UdRxCarrier->Position;
            }
#endif
			m_RxSet[0].m_pDem->UpdateBPF();
        }
		if( IsRTTY() ){
			if( m_RxSet[0].m_pDem->m_Decode.IsRTTYTmg() ){
				sprintf(m_TextBuff, "%.2lf", m_RxSet[0].m_pDem->m_Decode.GetRTTYTmg());
				EATC->Text = m_TextBuff;
            }
            else {
           		EATC->Text = "***";
            }
        }
		else if( m_RxSet[0].IsMFSK() ){
			sprintf(m_TextBuff, "%d", m_RxSet[0].m_pDem->GetClockError());
			EATC->Text = m_TextBuff;
        }
		else if( SBATC->Down ){
			int d = m_RxSet[0].m_pDem->GetClockError();
			EATC->Text = d;
        }
    }
	m_fDisEvent--;

//    StatusBar->Panels->Items[statusSYNC]->Text = (m_RxSet[0].m_SQ && m_RxSet[0].m_pDem->m_Decode.GetSyncState()) ? "SYNC" : "";

    if( m_fSubWindow ){
		CRxSet *pRxSet = &m_RxSet[1];
   		for( i = 1; i < RXMAX; i++, pRxSet++ ){
			if( pRxSet->IsActive() ){
				pRxSet->m_pDem->UpdateBPF();
            }
        }
    }
    if( m_fHintUpdate ){
		DrawHint();
    }
    if( m_pClockView ){
		wp = m_wBuffer;
		for( i = 0; i < m_BufferSize; i++ ){
			m_pClockView->Do(*wp++);
        }
    	m_pClockView->UpdateFFT();
    }
#if DEBUG && 0

	sprintf(m_TextBuff, "%9.1lf, %9.4lf, %4u",
    	m_RxSet[0].m_pDem->m_RxFreq,
    	m_RxSet[0].m_pDem->m_out,
    	m_RxSet[0].m_AFCFQ,
    );
    Caption = m_TextBuff;
#endif
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoMod(void)
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
void __fastcall TMainVARI::PutDumpChar(int d, CRxSet *pRxSet)
{
	if( pRxSet->IsRTTY() ){
//		if( d ) pDump->PutChar(d, m_TX ? 2 : 1);
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
//				if( !pRxSet->m_fMBCS ) pDump->PutChar(m, m_TX ? 2 : 1);
   	        }
       	}
        else {
//			pDump->PutChar(m, m_TX ? 2 : 1);
       	}
   }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::InitStgFFT(FFTSTG *pStg)
{
	if( m_FFT.m_FFTGain ){
        pStg->Sum = 1024;
	    pStg->Max = 1024;
    	pStg->WMax = 1024;
    }
    else {
        pStg->Sum = 5000;
   	    pStg->Max = 5000;
   	    pStg->WMax = 5000;
    }
    pStg->Sig = 0;
}
//---------------------------------------------------------------------------
double __fastcall TMainVARI::SqrtToDB(double d)
{
	d /= (0.00345 * 10.0 * m_fftSC);
    d = d * d;
	d = d * d;
    return m_fftSC * 10.0 * (log10(d+2.81458e4) - 4.4494132);
}
//---------------------------------------------------------------------------
double __fastcall TMainVARI::DBToSqrt(double d)
{
	d /= m_fftSC * 10.0;
    d += 4.4494132;
    d = pow(10, d) - 2.81458e4;
	d = pow(d, 0.25);
    return d * (0.00345 * 10.0 * m_fftSC);
}
//---------------------------------------------------------------------------
double __fastcall TMainVARI::AdjDB(double d)
{
    if( m_FFT.m_FFTGain ){
		return SqrtToDB(d);
    }
    else {
		return d * 100.0 / m_fftSC;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CalcStgFFT(CRxSet *pRxSet)
{
	if( !pRxSet->IsActive() ) return;

	CDEMFSK *pDem = pRxSet->m_pDem;
	int fo = pDem->m_CarrierFreq;

	int fm, xl, xh, xnl, xnh, xol, xoh;
    switch(pRxSet->m_Mode){
		case MODE_GMSK:
		    fm = pRxSet->m_Speed * 2 + 100;
			xl = (fo - fm) * FFT_SIZE / m_FFTSampFreq;
		    xh = (fo + fm) * FFT_SIZE / m_FFTSampFreq;
			xnl = (fo - pRxSet->m_Speed*1.6) * FFT_SIZE / m_FFTSampFreq;
		    xnh = (fo + pRxSet->m_Speed*1.6) * FFT_SIZE / m_FFTSampFreq;
		    xol = (fo - pRxSet->m_Speed/4) * FFT_SIZE / m_FFTSampFreq;
		    xoh = (fo + pRxSet->m_Speed/4) * FFT_SIZE / m_FFTSampFreq;
			break;
        case MODE_FSK:
		    fm = pRxSet->m_Speed * 4 + 100;
			xl = (fo - fm) * FFT_SIZE / m_FFTSampFreq;
		    xh = (fo + fm) * FFT_SIZE / m_FFTSampFreq;
			xnl = (fo - pRxSet->m_Speed*3.2) * FFT_SIZE / m_FFTSampFreq;
		    xnh = (fo + pRxSet->m_Speed*3.2) * FFT_SIZE / m_FFTSampFreq;
		    xol = (fo - pRxSet->m_Speed/2) * FFT_SIZE / m_FFTSampFreq;
		    xoh = (fo + pRxSet->m_Speed/2) * FFT_SIZE / m_FFTSampFreq;
        	break;
		case MODE_N_BPSK:
		case MODE_BPSK:
		    fm = pRxSet->m_Speed * 2 + 100;
			xl = (fo - fm) * FFT_SIZE / m_FFTSampFreq;
		    xh = (fo + fm) * FFT_SIZE / m_FFTSampFreq;
			xnl = (fo - pRxSet->m_Speed*1.6) * FFT_SIZE / m_FFTSampFreq;
		    xnh = (fo + pRxSet->m_Speed*1.6) * FFT_SIZE / m_FFTSampFreq;
		    xol = (fo - pRxSet->m_Speed*0.6) * FFT_SIZE / m_FFTSampFreq;
		    xoh = (fo + pRxSet->m_Speed*0.6) * FFT_SIZE / m_FFTSampFreq;
			break;
		case MODE_FSKW:
		case MODE_RTTY:
        case MODE_U_RTTY:
#if 1
			fm = (pRxSet->m_pDem->m_RTTYShift * 0.5) + 150;
            if( fm > 1500 ) fm = 1500;
			xl = (fo - fm) * FFT_SIZE / m_FFTSampFreq;
		    xh = (fo + fm) * FFT_SIZE / m_FFTSampFreq;
			xnl = xl;
		    xnh = xh;
			fm = (pRxSet->m_pDem->m_RTTYShift * 0.5) + 15;
		    xol = (fo - fm) * FFT_SIZE / m_FFTSampFreq;
		    xoh = (fo + fm) * FFT_SIZE / m_FFTSampFreq;
#else
		    fm = (pRxSet->m_pDem->m_RTTYShift * 500)/170;
            if( fm > 1500 ) fm = 1500;
			xl = (fo - fm) * FFT_SIZE / m_FFTSampFreq;
		    xh = (fo + fm) * FFT_SIZE / m_FFTSampFreq;
			xnl = (fo - (fm/2)) * FFT_SIZE / m_FFTSampFreq;
		    xnh = (fo + (fm/2)) * FFT_SIZE / m_FFTSampFreq;
			fm = (pRxSet->m_pDem->m_RTTYShift * 0.5) + 15;
		    xol = (fo - fm) * FFT_SIZE / m_FFTSampFreq;
		    xoh = (fo + fm) * FFT_SIZE / m_FFTSampFreq;
#endif
        	break;
		case MODE_mfsk_L:
			fm = pRxSet->m_pDem->m_MFSK_BW;
			if( sys.m_MFSK_Center ){
				xl = (fo - fm/2 - 150) * FFT_SIZE / m_FFTSampFreq;
			    xh = (fo + fm/2 + 150) * FFT_SIZE / m_FFTSampFreq;
				xnl = xl;
		    	xnh = xh;
			    xol = (fo - fm/2 - 15) * FFT_SIZE / m_FFTSampFreq;
			    xoh = (fo + fm/2 + 15) * FFT_SIZE / m_FFTSampFreq;
            }
            else {
				xl = (fo - fm - 150) * FFT_SIZE / m_FFTSampFreq;
			    xh = (fo + 150) * FFT_SIZE / m_FFTSampFreq;
				xnl = xl;
		    	xnh = xh;
			    xol = (fo - fm - 15) * FFT_SIZE / m_FFTSampFreq;
			    xoh = (fo + 15) * FFT_SIZE / m_FFTSampFreq;
            }
            break;
		case MODE_mfsk_U:
			fm = pRxSet->m_pDem->m_MFSK_BW;
			if( sys.m_MFSK_Center ){
				xl = (fo - fm/2 - 150) * FFT_SIZE / m_FFTSampFreq;
			    xh = (fo + fm/2 + 150) * FFT_SIZE / m_FFTSampFreq;
				xnl = xl;
		    	xnh = xh;
			    xol = (fo - fm/2 - 15) * FFT_SIZE / m_FFTSampFreq;
			    xoh = (fo + fm/2 + 15) * FFT_SIZE / m_FFTSampFreq;
            }
			else {
				xl = (fo - 150) * FFT_SIZE / m_FFTSampFreq;
			    xh = (fo + fm + 150) * FFT_SIZE / m_FFTSampFreq;
				xnl = xl;
			    xnh = xh;
			    xol = (fo - 15) * FFT_SIZE / m_FFTSampFreq;
			    xoh = (fo + fm + 15) * FFT_SIZE / m_FFTSampFreq;
            }
            break;
    }
	fm = 200 * FFT_SIZE / m_FFTSampFreq;
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
		pRxSet->m_StgFFT.dBSum = SqrtToDB(pRxSet->m_StgFFT.Sum);
		pRxSet->m_StgFFT.dBMax = SqrtToDB(pRxSet->m_StgFFT.Max);
		pRxSet->m_StgFFT.dBWMax = SqrtToDB(pRxSet->m_StgFFT.WMax);
		if( m_TX == txINTERNAL ) pRxSet->m_StgFFT.dBSum = 0;
    }
    else {
		double k = 100.0 / m_fftSC;
		pRxSet->m_StgFFT.dBSum = pRxSet->m_StgFFT.Sum * k;
		pRxSet->m_StgFFT.dBMax = pRxSet->m_StgFFT.Max * k;
		pRxSet->m_StgFFT.dBWMax = pRxSet->m_StgFFT.WMax * k;
		if( m_TX == txINTERNAL ) pRxSet->m_StgFFT.dBSum = -200;
    }

	BOOL fMFSKMet = FALSE;
	int d = pRxSet->m_StgFFT.Sig - 500;
    if( d > LEVELMAX ) d = LEVELMAX;
	int sq = pRxSet->m_SQ;
	if( (m_TX != txINTERNAL) && pRxSet->IsMFSK() && sys.m_MFSK_SQ_Metric ){
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
				if( sys.m_fAutoTS && (!pRxSet->m_cAutoTS1 || !pRxSet->m_cAutoTS2) ){
					RxStatus(pRxSet, "SON");
        	    }
	           	pRxSet->m_PeakSig = 0;
   		        pRxSet->m_AvgSig.Create(32);
            }
        }
		if( m_TX ){
			pRxSet->m_cAutoTS1 = -5 * SAMPFREQ/m_BufferSize;
        }
		else if( pRxSet->m_cAutoTS1 >= 0 ){
			pRxSet->m_cAutoTS1 = 10 * SAMPFREQ/m_BufferSize;
        }
    }
    else {
		pRxSet->m_SQTimer = 0;
		if( pRxSet->m_cAutoTS1 ){
			if( pRxSet->m_cAutoTS1 > 0 ){
				pRxSet->m_cAutoTS1--;
				if( sys.m_fAutoTS && !pRxSet->m_cAutoTS1 ){
					RxStatus(pRxSet, "SOFF");
            	}
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
		m = Is170() ? (1700+300) : (2000+300);
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
				xl = (fo - m_AFCWidth) * FFT_SIZE / m_FFTSampFreq;
    			xh = (fo + m_AFCWidth) * FFT_SIZE / m_FFTSampFreq;
				int o = (xh + xl)/2;
				int m = (xh - xl)/2;
				int rmax = 0;
				int lmax = 0;
				int rc = 0;
				int lc = 0;
				int lm = 200 * FFT_SIZE / m_FFTSampFreq;
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
				lmax = AdjDB(lmax) - pRxSet->m_StgFFT.dBSum;
				rmax = AdjDB(rmax) - pRxSet->m_StgFFT.dBSum;
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
					int fq = o * m_FFTSampFreq / FFT_SIZE;
					o = fq - fo;
					m = ABS(o);
					if( m < m_AFCWidth ){
						int diff;
						BOOL bPSK = FALSE;
						switch(pRxSet->m_Mode){
							case MODE_N_BPSK:
							case MODE_BPSK:
                    			bPSK = TRUE;
        	        			diff = pRxSet->m_Speed*1.2;
								break;
							case MODE_GMSK:
        	        			diff = pRxSet->m_Speed;
								break;
                			case MODE_FSK:
        	        			diff = pRxSet->m_Speed*2;
                				break;
							case MODE_FSKW:
                			case MODE_RTTY:
                			case MODE_U_RTTY:
        	        			diff = pRxSet->m_pDem->m_RTTYShift;
                				break;
                            case MODE_mfsk_L:
								if( !sys.m_MFSK_Center ) fq += pRxSet->m_pDem->m_MFSK_BW/2;
                            	diff = pRxSet->m_pDem->m_MFSK_BW*1.5;
                                break;
                            case MODE_mfsk_U:
								if( !sys.m_MFSK_Center ) fq -= pRxSet->m_pDem->m_MFSK_BW/2;
                            	diff = pRxSet->m_pDem->m_MFSK_BW*1.5;
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
//						pRxSet->m_AFCFQ = (pRxSet->m_AFCFQ + (fq * 2)) / 3;
						pRxSet->m_AFCFQ = pRxSet->m_AvgAFC.DoZ(fq);
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
			fm = GetSignalFreq(fo, bw*0.75, pRxSet, pRxSet->m_SQ ? 600 : (m_AFCLevel*100)+450);
			pRxSet->m_AFCFQ = pRxSet->m_AvgAFC.DoZ(fm);
			if( pRxSet->m_SQ ){
				m = ABS(fo - pRxSet->m_AFCFQ);
		        fm = bw * 0.5;
   			    if( fm < m_AFCWidth ) fm = m_AFCWidth;
				if( (m >= int(bw*0.2)) && (m < fm) ){
					pRxSet->m_AFCTimerPSK++;
					if( pRxSet->m_AFCTimerPSK >= UINT(IsBPSK() ? 6 : 8) ){
						fm = (pRxSet->m_AFCFQ - fo)*0.75;
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
		fm = GetSignalFreq(fo, bw*0.75, pRxSet, pRxSet->m_SQ ? 600 : (m_AFCLevel*100)+450);
		pRxSet->m_AFCFQ = pRxSet->m_AvgAFC.DoZ(fm);
    }
	pRxSet->m_AFCSQ = pRxSet->m_SQ;
    if( pRxSet->m_SQ ){
        pRxSet->m_AFCTimerW = 3 * SAMPFREQ / m_BufferSize;
    }
	else if( pRxSet->m_AFCTimerW ){
    	pRxSet->m_AFCTimerW--;
        pRxSet->m_AFCTimerW2 = 0;
    }
    sum = int((pRxSet->m_StgFFT.Sig-500)*0.01);
	if( m_TX != txINTERNAL ){
		if( m_FFT.m_FFTGain ){
			if( sum >= 38 ){
				sum += (sum - 38) * 1.75;
            }
        }
        else {
			if( sum >= 32 ){
				sum += (sum - 32) * 0.357;
            }
        }
    }
    if( sum < 0 ) sum = 0;
    if( sum > 96 ) sum = 96;
    pRxSet->m_StgFFT.DispSig = sum;
	if( pRxSet == &m_RxSet[0] ){
	    sprintf(m_TextBuff, "S/N=%ddB", sum);
        DrawStatus(statusSN, m_TextBuff);
    }
	if( !m_TX ){
       	if( pRxSet->m_PeakSig < sum ) pRxSet->m_PeakSig = sum;
		if( pRxSet->m_SQ ) pRxSet->m_AvgSig.Do(sum);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoAFCMFSK(CRxSet *pRxSet, int fo, BOOL fUpdate)
{
	if( !m_AFCWidth ) return;

	int bw;
	int xl, xh, x, xx;
    CDEMFSK *pDem = pRxSet->m_pDem;

	if( sys.m_MFSK_Center ){
		bw = (pDem->m_MFSK_BW*0.5) + 0.5;
    	xl = fo - bw;
    	xh = fo + bw;
    }
    else {
		bw = pDem->m_MFSK_BW + 0.5;
		if( pRxSet->m_Mode == MODE_mfsk_U ){
			xl = fo; xh = fo + bw;
        }
        else {
			xl = fo - bw; xh = fo;
        }
    }
	xl = int((xl * FFT_SIZE / m_FFTSampFreq));
	xh = int((xh * FFT_SIZE / m_FFTSampFreq) + 0.5);
	int f;
    if( pRxSet->m_AFCTimerMFSK < MFSKAFC_2ndATACK ){
		if( pRxSet->m_AFCTimerMFSK < MFSKAFC_1stATACK ){
			f = m_AFCWidth;
   			if( f >= 32 ) f = 32;
	    }
    	else {
			f = pDem->m_MFSK_SPEED;
	    }
	    f = f * FFT_SIZE / m_FFTSampFreq;
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
		avgdb = SqrtToDB(avg);
		maxdb = SqrtToDB(max);
    }
    else {
		double k = 100.0 / m_fftSC;
        avgdb = avg * k;
        maxdb = max * k;
	}
	if( (maxdb - avgdb) < 750 ) return;

   	xl = m_fftout[xx] - m_fftout[xx-1];
   	xh = m_fftout[xx] - m_fftout[xx+1];
	if( (xl > 0) && (xh > 0) ){	// FFT間隔補正
        double off = double(xl - xh) * 0.5 / double(xl + xh);
		x = ((xx + off) * m_FFTSampFreq / FFT_SIZE) + 0.5;
//		sprintf(m_TextBuff, "%u", x);
//        Caption = m_TextBuff;
   	}
    else {
		x = (xx * m_FFTSampFreq / FFT_SIZE) + 0.5;
    }
	f = 0;
	if( sys.m_MFSK_Center ){
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
		pRxSet->m_AFCFQ = pRxSet->m_AvgAFC.Do(fq) + 0.5;
		pDem->m_RxFreq = pRxSet->m_AFCFQ;
		if( fUpdate ){
           	if( pRxSet->m_AFCFQ != int(pDem->m_CarrierFreq) ){
       			pDem->SetCarrierFreq(pRxSet->m_AFCFQ);
            }
        }
    }
    else if( f ){
		pRxSet->m_AFCFQ = pRxSet->m_AvgAFC.Do(f)+0.5;
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
void __fastcall TMainVARI::AttachFocus(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DettachFocus(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PCTXEnter(TObject *Sender)
{
	AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PCTXExit(TObject *Sender)
{
	DettachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxTXClick(TObject *Sender)
{
	DeleteMacroTimer();
	AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnChar(int Key)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FormKeyPress(TObject *Sender, char &Key)
{
#if !KEY_DIRECT
	if( ActiveControl == PCTX ){
		OnChar(Key & 0x00ff);
    }
#endif
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateWaveCaption(void)
{
	SBWave->Caption = m_WaveType ? "Wav.":"Sync";
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetTXCaption(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetPTT(BOOL fTX)
{
	if( m_pCom ) m_pCom->SetPTT(fTX);
    if( m_pRadio ){
		if( fTX ) WaitICOM();
    	m_pRadio->SetPTT(fTX);
    }
    m_Wave.SetPTT(fTX);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::RxStatus(CRxSet *pRxSet, LPCSTR p)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetTXInternal(void)
{
	if( sys.m_LoopBack == loopINTERNAL ){
		m_RxSet[0].SetCarrierFreq(UdTxCarrier->Position * SAMPFREQ/(SAMPFREQ+SAMPTXOFFSET));
   		CRxSet *pRxSet = m_RxSet;
		for( int i = 0; i < RXMAX; i++, pRxSet++ ){
			if( pRxSet->IsActive() ){
				if( i ){
       	        	pRxSet->SetSampleFreq((SAMPFREQ+SAMPTXOFFSET)*m_DecFactor);
					if( pRxSet->m_fAFC ){
						int f = pRxSet->m_pDem->m_CarrierFreq - UdTxCarrier->Position;
    	                f = ABS(f);
   	    	            if( f < (pRxSet->m_Speed * 0.5) ) pRxSet->SetCarrierFreq(UdTxCarrier->Position);
                    }
				}
		        pRxSet->m_pDem->MakeBPF(8);		// CPU負荷低減のため
				pRxSet->m_pDem->m_Decode.m_fATC = FALSE;
				pRxSet->m_pDem->m_fAFC = FALSE;
				pRxSet->m_pDem->m_Decode.Create();
//				pRxSet->m_pDem->m_Decode.SetTmg(i ? 0 : 1e6*((SAMPFREQ+SAMPTXOFFSET)/SAMPFREQ - 1.0));
				pRxSet->m_pDem->SetTmg(i ? 0 : 1e6*((SAMPFREQ+SAMPTXOFFSET)/SAMPFREQ - 1.0));
		    	pRxSet->m_pDem->m_AGC.Reset();
//                pRxSet->m_PeakSig = 0;
//                pRxSet->m_AvgSig.Create(32);
				for( int j = 0; j < m_BufferSize; j++ ){
       				pRxSet->m_pDem->Do(0);
		        }
				pRxSet->InitStgFFT();
            }
		}
		FFTSampleFreq(SAMPFREQ + SAMPTXOFFSET);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::ToTone(void)
{
	if( m_fSuspend ) return;
	if( !SBTX->Enabled ) return;
	m_fTone = TRUE;
    SBTX->Down = TRUE;
	m_TX = (sys.m_LoopBack == loopINTERNAL) ? txINTERNAL : txEXTERNAL;
	if( sys.m_LoopBack == loopINTERNAL ){
    	m_Wave.InClose();
		DeleteSoundTimer();
    }
   	if( SBNET->Down ){
       	m_fDisEvent++;
        UdTxCarrier->Position = UdRxCarrier->Position;
        m_fDisEvent--;
    }
	m_ModFSK.Reset();	// For Signal/CW gain
	m_ModFSK.SetCarrierFreq(UdTxCarrier->Position);
	SetTXInternal();
	m_fSendChar = FALSE;
	m_Wave.m_hWnd = Handle;
	InitWater(iniwMETRIC);
	if( !m_Wave.IsOutOpen() ) OpenSound(TRUE);
	memset(m_wBuffer, 0, sizeof(m_wBuffer));
	SetPTT(TRUE);
    SetTXCaption();
	RxStatus(&m_RxSet[0], "TX");
	if( m_Wave.IsOutFirst() ){
		while(!m_Wave.IsOutBufFull()){
			DoMod();
			if( sys.m_LoopBack == loopINTERNAL ) DoDem();
			m_Wave.OutWrite(m_wBuffer);
   		}
    }
    UpdateUI();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::ToTX(void)
{
#if 0
	if( m_fSuspend ) return;
	if( !SBTX->Enabled ) return;
	StopPlayBack();
	m_SendingEdit = m_CurrentEdit;
	BOOL fATone = m_fTone;
	m_fTone = FALSE;
	SBTX->Down = TRUE;
	m_TX = (sys.m_LoopBack == loopINTERNAL) ? txINTERNAL : txEXTERNAL;
	if( sys.m_LoopBack == loopINTERNAL ){
    	m_Wave.InClose();
		DeleteSoundTimer();
    }
   	if( SBNET->Down ){
       	m_fDisEvent++;
        UdTxCarrier->Position = UdRxCarrier->Position;
        m_fDisEvent--;
    }
	CDump *pEdit = &m_Edit[m_CurrentEdit];
	BOOL fCW = (pEdit->GetCharNB() & 0xff00) == 0x100;
	if( !fATone ) m_ModFSK.Reset();	// For Signal/CW gain
	m_ModFSK.m_Encode.Reset(fCW);
	while(pEdit->GetCharNB() == 0x200){
		pEdit->GetChar(TRUE);
	   	m_ModFSK.m_Encode.PutChar(0x200);
    }
	if( KOAI->Checked && !fCW ){
	   	m_ModFSK.m_Encode.PutChar('\r');
	   	m_ModFSK.m_Encode.PutChar('\n');
    }
	if( !fATone ) m_ModFSK.SetCarrierFreq(SBNET->Down ? m_RxSet[0].m_pDem->m_CarrierFreq : UdTxCarrier->Position);
	m_RxSet[0].m_pDem->ResetMFSK();
	SetTXInternal();
	m_fSendChar = FALSE;
	m_Wave.m_hWnd = Handle;
	InitWater(iniwMETRIC);
	if( !m_Wave.IsOutOpen() ) OpenSound(TRUE);
	if( !fATone ){
		SetPTT(TRUE);
		memset(m_wBuffer, 0, sizeof(m_wBuffer));
    }
	if( sys.m_LoopBack == loopINTERNAL ){
		DoDem();
		if( !m_RxSet[0].IsMFSK() ){
			for( int i = 0; i < 8192; i++ ){
				m_RxSet[0].m_pDem->m_Decode.Do(1, TRUE, FALSE);
        	}
        }
    }
	if( !fATone ) RxStatus(&m_RxSet[0], "TX");
	if( m_Wave.IsOutFirst() ){
		while(!m_Wave.IsOutBufFull()){
			DoMod();
			if( sys.m_LoopBack == loopINTERNAL ) DoDem();
			m_Wave.OutWrite(m_wBuffer);
   		}
    }
    UpdateUI();
    if( m_fReqRX ){
		m_fReqRX = 1;
		m_ModFSK.m_Encode.m_fReqRX = TRUE;
	    SBTX->Caption = "ReqRX";
    }
#endif
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::ToRX(void)
{
	m_fTone = FALSE;
	RxStatus(&m_RxSet[0], "RX");

	if( m_fpText ){
		fclose(m_fpText);
        m_fpText = NULL;
    }
	SBTX->Down = FALSE;
	m_TX = txRX;
	m_fReqRX = FALSE;
	m_ModFSK.m_Encode.m_fReqRX = FALSE;
    SetTXCaption();
	if( m_WaveFile.m_mode != 1 ){
    	m_Wave.OutAbort();
		DeleteSoundTimer();
    }
	SetPTT(FALSE);
	InitWater(iniwMETRIC);
	if( !m_Wave.IsInOpen() ){		// Loopback == loopINTERNAL
		DoSetBPF(m_nBPFType);
	    m_RxSet[0].SetCarrierFreq( (UdRxCarrier->Position != int(m_ModFSK.m_CarrierFreq+0.5)) ? UdRxCarrier->Position : m_ModFSK.m_CarrierFreq);
        CRxSet *pRxSet = m_RxSet;
        int atctmg;
        if( sscanf(EATC->Text.c_str(), "%d", &atctmg) != 1 ) atctmg = 0;
		for(int i = 0; i < RXMAX; i++, pRxSet++ ){
			if( pRxSet->IsActive() ){
				if( i ){
					pRxSet->SetSampleFreq(DEMSAMPFREQ);
                	pRxSet->m_pDem->MakeBPF(m_RxSet[0].m_pDem->m_PreBPFTaps);
                }
				pRxSet->m_pDem->m_AFCCount = 0;
				pRxSet->m_pDem->m_fAFC = i ? pRxSet->m_fAFC : SBAFC->Down;
				pRxSet->m_pDem->m_Decode.m_fATC = i ? TRUE : SBATC->Down;
//				if( !pRxSet->IsRTTY() ) pRxSet->m_pDem->m_Decode.SetTmg(i ? 0 : atctmg);
				pRxSet->m_pDem->SetTmg((i || pRxSet->IsMFSK()) ? 0 : atctmg);
                pRxSet->m_pDem->m_Decode.ResetMeasRTTY();
                pRxSet->m_pDem->ResetMFSK();
				for( int j = 0; j < m_BufferSize; j++ ){
		        	pRxSet->m_pDem->Do(0);
		            pRxSet->m_pDem->m_Decode.Do(0, 0, FALSE);
        		}
                pRxSet->m_pDem->m_AGC.Reset();
				pRxSet->InitStgFFT();
		        pRxSet->m_SQ = FALSE;
        		pRxSet->m_AFCTimerW = 5 * SAMPFREQ / m_BufferSize;
		        pRxSet->m_AFCTimerN = SAMPFREQ / m_BufferSize;
                pRxSet->m_SQTimer = 0;
				pRxSet->m_AFCTimerPSK = 0;
				pRxSet->m_AFCTimerMFSK = MFSKAFC_MAX;
                pRxSet->m_PeakSig = 0;
                pRxSet->m_AvgSig.Create(32);
            }
        }
		if( m_WaveFile.m_mode != 1 ) OpenSound(FALSE);
		FFTSampleFreq(SAMPFREQ);
    	m_FFT.ClearStg();
    }
    else {								// loopEXTERNAL
        m_RxSet[0].m_AFCTimerN = 3 * SAMPFREQ / m_BufferSize;
    }
	m_RxSet[0].m_PeakSig = 0;
	m_RxSet[0].m_AFCTimerPSK = 0;
	m_RxSet[0].m_AFCTimerMFSK = MFSKAFC_MAX;

	if( m_ReqMacroTimer ) CreateMacroTimer(m_ReqMacroTimer);
    UpdateUI();
	if( m_ReqAutoReturn ){
		m_ReqAutoReturn = FALSE;
    	SetEditPage(m_SaveEditPage);
    }
    if( m_ReqAutoNET ){
		m_ReqAutoNET = FALSE;
		if( !SBNET->Down ){
        	SBNET->Down = TRUE;
            SBNETClick(NULL);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DeleteSoundTimer(void)
{
	if( m_pSoundTimer ){
		m_pSoundTimer->Enabled = FALSE;
		delete m_pSoundTimer;
        m_pSoundTimer = NULL;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBTXMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button == mbRight ){
		if( m_TX ){
			if( m_fTone && (m_fReqRX != 1) ){
				m_fReqRX = 1;
				m_ModFSK.m_Encode.m_fReqRX = TRUE;
            }
        }
        else {
			ToTone();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBTXClick(TObject *Sender)
{
#if 0
	DeleteMacroTimerS();
	if( SBTX->Down ){
		ToTX();
    }
    else if( m_fReqRX ){
		if( m_fReqRX == 1 ){
			m_fReqRX = FALSE;
			m_ModFSK.m_Encode.m_fReqRX = FALSE;
        }
       	SBTX->Down = SBTX->Enabled ? TRUE : FALSE;
    	if( !SBTX->Down ) m_TX = txRX;
    }
    else {
		m_fReqRX = TRUE;
		m_ModFSK.m_Encode.m_fReqRX = TRUE;
       	SBTX->Down = SBTX->Enabled ? TRUE : FALSE;
    	if( !SBTX->Down ) m_TX = txRX;
        m_Edit[m_CurrentEdit].MoveCursor(dmpMoveLAST);
    }
    SetTXCaption();
#endif
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoTXOFF(void)
{
	m_ReqAutoClear = FALSE;
	DeleteMacroTimer();
	if( m_TX ){
    	ToRX();
    }
    else if( m_fReqRX ){
    	m_fReqRX = FALSE;
		SetTXCaption();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBFFTClick(TObject *Sender)
{
    m_fftMX = 0;
	Draw(TRUE);
}
//---------------------------------------------------------------------------
TSpeedButton *__fastcall TMainVARI::GetDraw(int n)
{
	switch(n){
		case 1:
        	return SBWater;
        case 2:
        	return SBWave;
        default:
        	return SBFFT;
    }
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetDrawType(void)
{
	for( int i = 0; i < 3; i++ ){
		if( GetDraw(i)->Down ) return i;
    }
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetDrawType(int n)
{
	TSpeedButton *pButton = GetDraw(n);
    pButton->Down = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoSetBPF(int n)
{
	if( m_fDisEvent ) return;

	m_nBPFType = n;
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
void __fastcall TMainVARI::SetTxFreq(int fq)
{
    if( (fq >= MIN_CARRIER) && (fq <= sys.m_MaxCarrier) ){
		m_ModFSK.SetCarrierFreq(fq);
		m_fDisEvent++;
		UdTxCarrier->Position = short(fq);
		if( m_TX && SBNET->Down ){
			UdRxCarrier->Position = short(fq);
        }
        if( UdTxCarrier->Position != UdRxCarrier->Position ) SBNET->Down = FALSE;
        m_fDisEvent--;
		if( (m_TX == txINTERNAL) || SBNET->Down ){
        	m_RxSet[0].SetCarrierFreq(fq);
        }
        UpdateUI();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetRxFreq(int fq)
{
    if( (fq >= MIN_CARRIER) && (fq <= sys.m_MaxCarrier) ){
		if( m_TX != txINTERNAL ){
			m_RxSet[0].SetCarrierFreq(fq);
        }
		m_fDisEvent++;
		UdRxCarrier->Position = short(fq);
        if( m_pRadio ) m_pRadio->SetCarrierFreq(fq);
		if( !m_TX && SBNET->Down ){
			UdTxCarrier->Position = short(fq);
        }
        m_fDisEvent--;
        m_RxSet[0].m_AFCTimerW = 1;
        m_RxSet[0].m_SQTimer = 0;
        if( !m_RxSet[0].IsBPSK() ) m_PFTimer = 4;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UdTxCarrierClick(TObject *Sender,
      TUDBtnType Button)
{
	if( m_fDisEvent ) return;

	SetTxFreq(UdTxCarrier->Position);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CBTxCarrierChange(TObject *Sender)
{
	if( m_fDisEvent ) return;

	int fq;
    sscanf(CBTxCarrier->Text.c_str(), "%u", &fq);
	SetTxFreq(fq);
    if( m_fDrop ){
		m_fDrop = FALSE;
		SetTXFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UdRxCarrierClick(TObject *Sender,
      TUDBtnType Button)
{
	if( m_fDisEvent ) return;

	SetRxFreq(UdRxCarrier->Position);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CBRXCarrierChange(TObject *Sender)
{
	if( m_fDisEvent ) return;

	int fq;
	sscanf(CBRXCarrier->Text.c_str(), "%u", &fq);
	SetRxFreq(fq);
    if( m_fDrop ){
		m_fDrop = FALSE;
		SetTXFocus();
    }
    else {
		m_AFCKeyTimer = 3 * SAMPBASE / m_BufferSize;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetSpeedInfo(double b)
{
	if( m_RxSet[0].IsMFSK() ){
		switch(MFSK_Speed2Type(b)){
			case typMFSK16:
            	strcpy(m_TextBuff, "mfsk16 (BAUD=15.625)");
            	break;
            case typMFSK8:
            	strcpy(m_TextBuff, "mfsk8 (BAUD=7.8125)");
            	break;
            case typMFSK31:
            	strcpy(m_TextBuff, "mfsk31 (BAUD=31.25)");
            	break;
		}
    }
    else {
		StrDbl(m_TextBuff, b);
    	strcat(m_TextBuff, "Bps");
    }
	switch( m_RxSet[0].m_Mode ){
		case MODE_BPSK:
        case MODE_N_BPSK:
			if( b == 31.25 ){
            	strcat(m_TextBuff, " (PSK31)");
            }
            else if( b == 62.5 ){
            	strcat(m_TextBuff, " (PSK63)");
            }
            else if( b == 125 ){
            	strcat(m_TextBuff, " (PSK125)");
            }
            else if( b == 250 ){
            	strcat(m_TextBuff, " (PSK250)");
            }
        	break;
        case MODE_RTTY:
        case MODE_U_RTTY:
			if( b == 45.45 ){
				strcat(m_TextBuff, sys.m_MsgEng ? "(Standard)":"(標準)");
            }
        	break;
    }
	if( IsRTTY() ){
		if( b != 45.45 ){
			strcat(m_TextBuff, sys.m_MsgEng ? " : 45.45Bps is standard" : " : 標準は45.45Bps");
	    }
	}
    else if( !m_RxSet[0].IsMFSK() ){
		if( b != 31.25 ){
			strcat(m_TextBuff, sys.m_MsgEng ? " : 31.25Bps is standard" : " : 標準は31.25Bps");
	    }
    }
    SetInfoMsg(m_TextBuff);
    DoEvent(macOnSpeed);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CBSpeedChange(TObject *Sender)
{
	if( m_fDisEvent ) return;

	double b;
    sscanf(CBSpeed->Text.c_str(), "%lf", &b);
    SpeedChange(b);
    if( m_fDrop ){
		m_fDrop = FALSE;
		SetTXFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SpeedChange(double b)
{
	if( m_RxSet[0].IsMFSK() ){
        m_RxSet[0].SetMFSKType(MFSK_Speed2Type(b));
#if DEBUG
		m_ModTest.SetMFSKType(MFSK_Speed2Type(b));
#endif
		m_ModFSK.SetMFSKType(MFSK_Speed2Type(b));
		InitCollect();
		SetSpeedInfo(b);
    }
    else if( (b >= MIN_SPEED) && (b <= MAX_SPEED) && (b != m_RxSet[0].m_Speed) ){
        m_RxSet[0].SetSpeed(b);
#if DEBUG
		m_ModTest.SetSpeed(b);
#endif
		m_ModFSK.SetSpeed(b);
		InitCollect();
		SetSpeedInfo(b);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateSpeed(CRxSet *pRxSet, double b)
{
	if( pRxSet == &m_RxSet[0] ){
		if( pRxSet->IsMFSK() ){
			SpeedChange(b);
	        UpdateUI();
        }
        else {
			m_fDisEvent++;
			char bf[256];
			CBSpeed->Text = StrDbl(bf, b);
	    	m_fDisEvent--;
		    CBSpeedChange(NULL);
        }
    }
	else if( pRxSet->IsMFSK() ){
		pRxSet->SetMFSKType(MFSK_Speed2Type(b));
    }
    else {
		pRxSet->SetSpeed(b);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateMode(CRxSet *pRxSet, int offset)
{
	if( pRxSet == &m_RxSet[0] ){
    	if( IsRTTY() ){
			GBTiming->Caption = "Timing(ms)";
        }
        else {
			GBTiming->Caption = "Timing(ppm)";
        }
        if( offset ){
        	SetRxFreq(UdRxCarrier->Position + offset);
			if( m_TX || !SBNET->Down ){
	        	SetTxFreq(UdTxCarrier->Position + offset);
            }
        }
    }
    else if( offset ){
		pRxSet->SetCarrierFreq(pRxSet->m_CarrierFreq + offset);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetFFTWidth(int fw)
{
//	int fo = m_FFTB + m_FFTW/2;
	if( SBWater->Down ) InitWater(iniwMETRIC);
	int fo = UdRxCarrier->Position;
	switch(fw){
		case 500:
			SBFFT500->Down = TRUE;
            break;
        case 1000:
			SBFFT1K->Down = TRUE;
            break;
        case 2000:
			SBFFT2K->Down = TRUE;
            break;
        default:
			SBFFT3K->Down = TRUE;
			fw = 3000;
            break;
    }
    m_FFTW = fw;
    m_FFTWindow = m_FFTW * FFT_SIZE / m_FFTSampFreq;
	CalcFFTCenter(fo);
    Draw(TRUE);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBFFT500Click(TObject *Sender)
{
	int fw;
	if( SBFFT3K->Down ){
		fw = 3000;
    }
    else if( SBFFT2K->Down ){
		fw = 2000;
    }
    else if( SBFFT500->Down ){
		fw = 500;
    }
	else {
		fw = 1000;
    }
    SetFFTWidth(fw);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UdATCClick(TObject *Sender,
      TUDBtnType Button)
{
	if( m_fDisEvent ) return;

	int d;
    sscanf(EATC->Text.c_str(), "%d", &d);
	d = d - (d % 200);
	if( Button == Comctrls::btNext ){
		d += 200;
	}
	else {
		d -= 200;
	}
	if( d < -m_ATCLimit ) d = -m_ATCLimit;
    if( d > m_ATCLimit ) d = m_ATCLimit;
	m_RxSet[0].m_pDem->m_Decode.SetTmg(d);
    EATC->Text = d;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBAFCClick(TObject *Sender)
{
	if( m_TX != txINTERNAL ){
		CDEMFSK *pDem = m_RxSet[0].m_pDem;
		m_RxSet[0].m_fAFC = pDem->m_fAFC = SBAFC->Down;
		if( pDem->m_fAFC && m_RxSet[0].IsMFSK() ){
			m_RxSet[0].m_AFCFQ = pDem->m_CarrierFreq;
			m_RxSet[0].m_AvgAFC.Reset(pDem->m_CarrierFreq);
			pDem->m_RxFreq = pDem->m_CarrierFreq;
			if( m_RxSet[0].m_AFCTimerMFSK < MFSKAFC_1stATACK ){
				m_RxSet[0].m_AFCTimerMFSK = MFSKAFC_1stATACK;
			}
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBATCClick(TObject *Sender)
{
	UpdateUI();
	if( m_TX == txINTERNAL ) return;

	m_RxSet[0].m_pDem->m_Decode.m_fATC = SBATC->Down;
	if( !SBATC->Down ){
		int atctmg;
        if( sscanf(EATC->Text.c_str(), "%d", &atctmg) == 1 ){
			m_RxSet[0].m_pDem->m_Decode.SetTmg(atctmg);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxFFTMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	DeleteMacroTimer();
	m_MouseDown = FALSE;
	if( SBFFT->Down || SBWater->Down ){
		double fq = m_FFTB + X * m_FFTW / m_fftXW;
		if( fq < MIN_CARRIER ) fq = MIN_CARRIER;
        if( fq > sys.m_MaxCarrier ) fq = sys.m_MaxCarrier;
		if( Button == mbRight ){
			m_RightX = X;
			m_RightFreq = fq + 0.5;
			RECT rc;
			::GetWindowRect(PFFT->Handle, &rc);
		    rc.left += X;
			if( !m_MouseNotch && (!m_fSubWindow || !m_MouseSubChannel) ){
				m_fftMX = ((fq - m_FFTB) * m_fftXW / m_FFTW) + 0.5;
				PBoxFFTPaint(NULL);
		    }
			PupSpec->Popup(rc.left, rc.bottom);
		    m_fftMX = 0;
			PBoxFFTPaint(NULL);
        }
        else if( Button == mbLeft ){
			if( m_MouseSubChannel || m_MouseNotch ){
				m_MouseDown = TRUE;
            }
			else if( m_TX != txINTERNAL ){
				m_RxSet[0].m_pDem->m_Decode.Reset();
                m_RxSet[0].m_pDem->ResetMeasMFSK();
                m_RxSet[0].m_PeakSig = 0;
                m_RxSet[0].m_AFCTimerMFSK = (SBFFT500->Down || SBFFT1K->Down) ? MFSKAFC_MAX : 0;
				if( !SBFFT500->Down ){
					fq = GetSignalFreq(fq, SBFFT3K->Down ? 50 : 32, &m_RxSet[0]);
                }
				SetRxFreq(fq);
				m_MouseDown = TRUE;
            }
        }
	}
    else {
		if( Button == mbRight ){
			m_fftMX = 0;
        }
        else {
			m_fftMX = X;
        }
		PBoxFFTPaint(NULL);
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::PBoxLevelMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	m_MouseDown = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FormResize(TObject *Sender)
{
	if( !m_pBitmapFFT ) return;

	if( (ClientWidth < 600) || (ClientHeight < 400) ){
		if( ClientWidth < 600 ) ClientWidth = 600;
        if( ClientHeight < 400 ) ClientHeight = 400;
        return;
    }
    PFFT->Font->Charset = sys.m_FontCharset;
    PFFT->Font->Height = -12;
	int x = SBFFT->Left + SBFFT->Width + PCLevel->Width + 2;
	PFFT->Width = ClientWidth - x;
	if( PBoxFFT->Align != alClient ) CreateMacExButton();
    m_fftXW = PBoxFFT->Width;
    if( m_fftXW != m_pBitmapFFT->Width ){
		m_pBitmapFFT->Width = m_fftXW;
        DrawWater(FALSE, TRUE);
   	}
//	StatusBar->SizeGrip = (WindowState == wsNormal);
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::CBRXCarrierDropDown(TObject *Sender)
{
	m_fDrop = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetMacButtonMax(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CreateMacButton(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UdMacClick(TObject *Sender, TUDBtnType Button)
{
	CreateMacButton();
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetMacButtonNo(TSpeedButton *pButton)
{
	MACBUTTON *pList = m_tMacButton;
    int i;
    for( i = 0; i < MACBUTTONALL; i++, pList++ ){
		if( pButton == pList->pButton ) return i;
    }
    return -1;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnMacButtonClick(TObject *Sender)
{
	TSpeedButton *pButton = (TSpeedButton *)Sender;
	MACBUTTON *pList = m_tMacButton;
    int i;
    for( i = 0; i < MACBUTTONALL; i++, pList++ ){
		if( pButton == pList->pButton ){
			if( pButton->Down ){
				m_fMacroRepeat = FALSE;
   	           	SendButton(i);
            }
            else {
				DeleteMacroTimer();
            }
           	break;
    	}
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnMacButtonDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
}
//---------------------------------------------------------------------------
static void __fastcall AdjustCallsign(AnsiString &as)
{
	AnsiString cs;

    LPCSTR p;
	int nl = 0;
    int nu = 0;
    for(p = as.c_str(); *p; p++ ){
		if( isupper(*p) ) nu++;
   	    if( islower(*p) ) nl++;
    }
	for( p = as.c_str(); *p; p++ ){
		if( isdigit(*p) ) break;
		if( nl > nu ){
			if( islower(*p) ) break;
        }
        else {
			if( isupper(*p) ) break;
        }
	}
    for( ; *p; p++ ){
		if( *p != '/' ){
			if( nl > nu ){
				if( isupper(*p) ) break;
       	    }
           	else {
				if( islower(*p) ) break;
            }
        }
       	cs += *p;
    }
	if( IsCall(cs.c_str()) ){
	    as = cs;
    }
   	jstrupr(as.c_str());
}
//---------------------------------------------------------------------------

static BOOL FindDlmStr(LPCSTR v, LPCSTR s)
{
	LPSTR pBF = StrDupe(v);
	BOOL f = FALSE;
	LPSTR p = pBF;
    LPSTR t;
    while(*p){
		p = StrDlm(t, p, -1);
        if( *t && !strcmpi(t, s) ){
			f = TRUE;
            break;
        }
    }
    delete pBF;
    return f;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoSetFFTType(int type)
{
	int vtype = m_FFTVType;
	switch(type){
		case 0:
			m_FFTVType = 0;
			m_FFT.m_FFTGain = 0;
            break;
        case 1:
			m_FFTVType = 1;
			m_FFT.m_FFTGain = 0;
            break;
        case 2:
			m_FFTVType = 2;
			m_FFT.m_FFTGain = 1;
            break;
    }
	if( vtype == m_FFTVType ) return;

    InitWater(iniwBOTH);
	for( int i = 0; i < RXMAX; i++ ){
	    m_RxSet[i].m_StgFFT.Timer = SAMPFREQ/m_BufferSize;
	    m_RxSet[i].m_AFCTimerW = SAMPFREQ / m_BufferSize;
    }
    DoEvent(macOnFFTScale);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBNETClick(TObject *Sender)
{
	UpdateUI();
    if( SBNET->Down && !m_TX ){
		SetTxFreq(UdRxCarrier->Position);
    }
    m_ReqAutoNET = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KVFS2Click(TObject *Sender)
{
	m_FFTSmooth = 2;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KVFS3Click(TObject *Sender)
{
	m_FFTSmooth = 3;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KVFS4Click(TObject *Sender)
{
	m_FFTSmooth = 4;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KOVOClick(TObject *Sender)
{
	HWND hWnd = ::FindWindow("Volume Control", NULL);
	if( hWnd != NULL ){
		::PostMessage(hWnd, WM_CLOSE, 0, 0);
		::Sleep(200);
        OnWave();
	}
	char cmd[128];
	strcpy(cmd, "SNDVOL32.EXE");
	BOOL f  = FALSE;
	if( Sender == KOVI ){
		f = sys.m_WinNT;
		char bf[32];
        if( !f && StrWindowsVer(bf) && (!strcmp(bf, "ME")||!strcmp(bf, "98SE")) ){
			f = TRUE;
        }
    }
	if( f ) strcat(cmd, " /R");
	WinExec(cmd, SW_SHOW);
	if( !f && (Sender == KOVI) ){
		CWaitCursor w;
		int i;
		for( i = 0; i < 30; i++ ){
			OnWave();
			::Sleep(100);
			hWnd = ::FindWindow("Volume Control", NULL);
			if( hWnd != NULL ) break;
		}
		if( i < 30 ){
			::SetForegroundWindow(hWnd);
			::Sleep(100);
            OnWave();
			const short _tt[]={
				VK_MENU, 'P', 'P'|0x8000, VK_MENU|0x8000,
				'R', 'R'|0x8000, VK_TAB, VK_TAB|0x8000,
				VK_DOWN, VK_DOWN|0x8000, VK_RETURN, VK_RETURN|0x8000,
				0
			};
			KeyEvent(_tt);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KOOClick(TObject *Sender)
{
	TOptDlgBox *pBox = new TOptDlgBox(this);
    DettachFocus();
    if( pBox->Execute(DWORD(Sender)) ){
		if( pBox->m_fComChange ) OpenCom();
		if( RADIO.change ) OpenRadio();
		if( pBox->m_fLangChange ){
        	SetSystemFont();
	        CreateMacButton();
			if( PBoxFFT->Align != alClient ) CreateMacExButton();
        }
		if( sys.m_fPlayBack ){
			if( !m_PlayBack.IsActive() ) m_PlayBack.Init(m_BufferSize, SAMPBASE);
        }
        else {
			m_PlayBack.Delete();
        }
		OnWave();
		SetTXCaption();
        UpdateUI();
        OnWave();
		UpdateShowCtrl();
        CreateWaterColors();
        InitWater(iniwLIMIT);
		UpdateMacroOnTimer();
    }
	delete pBox;
	OnWave();
	UpdateCharset();
    AttachFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::InitCheckValKey(LPCSTR pKey, AnsiString *pAS)
{
	m_pCheckKey = pKey;
    m_pCheckBuff = pAS;
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::CheckKey(LPCSTR pTemplate)
{
	if( *m_pCheckKey != *pTemplate ) return FALSE;
	return !strcmp(m_pCheckKey, pTemplate);
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TMainVARI::CheckValKey(LPCSTR pTemplate)
{
	return CheckValKey(m_pCheckKey, pTemplate);
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TMainVARI::CheckValKey(LPCSTR pKey, LPCSTR pTemplate)
{
	if( *pKey != *pTemplate ) return NULL;

	BOOL	f = TRUE;
	LPCSTR pBase = pKey;
    LPCSTR pEnd = NULL;
    int seq = 0;
	for( ; *pKey && *pTemplate; pKey++, pTemplate++ ){
		switch(*pTemplate){
			case '*':
				pBase = pKey;
				for( ; *pKey; pKey++ ){
					if( (*pKey == '<') && (*(pKey+1) == '%') ){
						seq++;
                    }
					else if( *pKey == '>' ){
						if( seq ){
							seq--;
                        }
                        else {
							pEnd = pKey;
                        	break;
                        }
                    }
                }
                pTemplate++;
            	break;
            default:
//				if( *pKey != *pTemplate ) f = FALSE;
				if( *pKey != *pTemplate ) return NULL;
            	break;
        }
    }
	if( *pKey != *pTemplate ) f = FALSE;
	if( f && pEnd ){
    	int len = pEnd - pBase;
        if( len < 0 ) len = 0;
        if( len >= 4095 ) len = 4095;
		LPSTR pBF = new char[len+1];
		if( len > 0 ){
			memcpy(pBF, pBase, len); pBF[len] = 0;
        }
        else {
			pBF[0] = 0;
        }
        *m_pCheckBuff = pBF;
        pBase = m_pCheckBuff->c_str();
        delete pBF;
    }
    return f ? pBase : NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::MacroDate(LPSTR t, SYSTEMTIME &now)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::MacroHisName(LPSTR t)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::MacroGreeting(LPSTR t, LPCSTR pCall, int type)
{
}
//---------------------------------------------------------------------------
static BOOL IsMMVARI(BYTE charset, int mode)
{
	BOOL r = FALSE;

	switch(mode){
		case MODE_GMSK:
        case MODE_FSK:
        case MODE_FSKW:
        case MODE_BPSK:
			switch(charset){
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
//---------------------------------------------------------------------------
static LPCSTR GetVariType(BYTE charset, int mode)
{
	if( (mode == MODE_RTTY) || (mode == MODE_U_RTTY) ){
		return "BAUDOT";
    }
	else if( IsMFSK(mode) ){
    	LPCSTR p;
		switch(charset){
			case SHIFTJIS_CHARSET:
				p = "VariMFSK/JA";
				break;
			case HANGEUL_CHARSET:
		        case JOHAB_CHARSET:
       		   	p = "VariMFSK/HL";
	           	break;
			case CHINESEBIG5_CHARSET:   //
   	    	   	p = "VariMFSK/BV";
       	    	break;
			case 134:       // 簡略
				p = "VariMFSK/BY";
				break;
			default:
            	p = "VariMFSK";
   		        break;
	    }
        return p;
    }
	BOOL f = mode == (MODE_N_BPSK);
	LPCSTR p = "VariSTD";
	switch(charset){
		case SHIFTJIS_CHARSET:
			p = f ? "VariSTD/JA":"VariJA";
			break;
		case HANGEUL_CHARSET:
	        case JOHAB_CHARSET:
       	   	p = f ? "VariSTD/HL":"VariHL";
           	break;
		case CHINESEBIG5_CHARSET:   //
   	       	p = f ? "VariSTD/BV":"VariBV";
       	    break;
		case 134:       // 簡略
			p = f ? "VariSTD/BY":"VariBY";
			break;
		default:
   	        break;
    }
	return p;
}
//---------------------------------------------------------------------------
BOOL __fastcall ValCond(double dd, double dv, int op)
{
   	return FALSE;
}
//---------------------------------------------------------------------------
BOOL __fastcall CheckCond(LPCSTR p, LPCSTR v, int type, int &op, double &d, AnsiString &as, AnsiString &vs)
{
    return TRUE;
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::IsMBCSStr(LPCSTR p)
{
	while(*p){
		if( m_RxSet[0].m_MBCS.IsLead(*p) ) return TRUE;
        p++;
    }
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::GetDataCond(LPCSTR p, int err, TSpeedButton *pButton)
{
    return 0;
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::GetDataConds(LPCSTR p, int err, TSpeedButton *pButton)
{
    return 0;
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TMainVARI::Cond(LPCSTR p, TSpeedButton *pButton)
{
    return NULL;
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::ConvMacro(LPSTR t, LPCSTR p, TSpeedButton *pButton)
{
    return 0;
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::ConvMacro(AnsiString &as, LPCSTR p, TSpeedButton *pButton)
{
    return 0;
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::ConvMacro_(AnsiString &as, LPCSTR p, TSpeedButton *pButton)
{
    return 0;
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TMainVARI::GetMacroValue(LPCSTR pVal)
{
    return NULL;
}
//---------------------------------------------------------------------------
LPCSTR __fastcall TMainVARI::GetMacroStr(AnsiString &as, LPCSTR pVal)
{
   	return NULL;
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetMacroInt(LPCSTR pVal)
{
    return 0;
}
//---------------------------------------------------------------------------
double __fastcall TMainVARI::GetMacroDouble(LPCSTR pVal)
{
    return 0;
}
//---------------------------------------------------------------------------
static LPCSTR __fastcall ClipRST(LPSTR bf, LPCSTR p)
{
    return NULL;
}
//---------------------------------------------------------------------------
static LPCSTR __fastcall ClipNR(LPCSTR p)
{
	return NULL;
}
//---------------------------------------------------------------------------
static LPSTR __fastcall strchrl(LPSTR p, LPSTR v)
{
	LPSTR t;
	while(*v){
		if( (t = strchr(p, *v++))!= NULL ) return t;
    }
	return NULL;
}
//---------------------------------------------------------------------------
static void __fastcall EncodeColors(LPSTR t, UCOL *pColors, int max)
{
	int i;
	for( i = 0; i < max; i++ ){
		if( i ) *t++ = ',';
		sprintf(t, "%06X", pColors[i].d); t += strlen(t);
	}
}
//---------------------------------------------------------------------------
static BOOL __fastcall DecodeColors(LPCSTR p, UCOL *pColors, int max)
{
    AnsiString as;
	LPSTR pBF = StrDupe(MainVARI->GetMacroStr(as, p));
	LPSTR pp, tt;
    pp = SkipSpace(pBF);
	int n = 0;
	while(*pp && (n < max)){
		pp = SkipSpace(StrDlm(tt, pp));
		pColors[n].d = htoin(MainVARI->GetMacroStr(as, tt), -1);
		n++;
	}
    delete pBF;
    return n != 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnMacroMenuClick(TObject *Sender)
{
	TMenuItem *pMenu = (TMenuItem *)Sender;
	m_MacroInput = pMenu->Caption;

	if( !m_pMacroPopup ) return;
	int i, n;
    for( i = n = 0; i < m_pMacroPopup->Items->Count; i++ ){
		TMenuItem *pRef = m_pMacroPopup->Items->Items[i];
        if( pRef->Caption != "-" ){
			if( pRef == pMenu ){
				m_MacroMenuNo = n + 1;
    	        break;
            }
            n++;
        }
    }
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::ConvMacro_(LPSTR t, LPCSTR p, TSpeedButton *pButton)
{
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoMacroReturn(int f)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoMacro(LPCSTR pMacro, TSpeedButton *pButton)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SendButton(int n)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FormPaint(TObject *Sender)
{
	if( m_fInitFirst ){
		m_fInitFirst = FALSE;
		OpenSound(FALSE);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBATCMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button == mbRight ){
		if( m_TX != txINTERNAL ){
			m_fDisEvent++;
			EATC->Text = "0";
			m_RxSet[0].m_pDem->m_Decode.SetTmg(0);
            m_RxSet[0].m_pDem->m_Decode.ClearLPF();
        	m_fDisEvent--;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CreateWaterColors(void)
{
	int i, O, W;
    UCOL c;

    W = m_tWaterLevel[0];
	for( i = 0; i < W; i++ ){
		c = GetGrade2(m_tWaterColset, i, W);
        m_tWaterColors[i] = c.c;
    }
	UCOL col[2];
	W = m_tWaterLevel[1] - m_tWaterLevel[0];
    O = m_tWaterLevel[0];
    col[0].c = m_tWaterColset[1].c;
    col[1].c = m_tWaterColset[6].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
	W = m_tWaterLevel[2] - m_tWaterLevel[1];
    O = m_tWaterLevel[1];
    col[0].c = m_tWaterColset[6].c;
    col[1].c = m_tWaterColset[7].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
	W = m_tWaterLevel[3] - m_tWaterLevel[2];
    O = m_tWaterLevel[2];
    col[0].c = m_tWaterColset[7].c;
    col[1].c = m_tWaterColset[8].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
	W = m_tWaterLevel[4] - m_tWaterLevel[3];
    O = m_tWaterLevel[3];
    col[0].c = m_tWaterColset[8].c;
    col[1].c = m_tWaterColset[9].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
	W = m_tWaterLevel[5] - m_tWaterLevel[4];
    O = m_tWaterLevel[4];
    col[0].c = m_tWaterColset[9].c;
    col[1].c = m_tWaterColset[10].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
	W = 256 - m_tWaterLevel[5];
    O = m_tWaterLevel[5];
    col[0].c = m_tWaterColset[10].c;
    col[1].c = m_tWaterColset[11].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawStatusBar(const Windows::TRect &Rect, LPCSTR pText, TColor col)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawHint(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawStatus(int n, LPCSTR pText)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawStatus(int n)
{
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetStatusIndex(int x)
{
	return 0;
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetPopupIndex(TComponent *pComponent)
{
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::HisRSTChange(TObject *Sender)
{
	if( m_fDisEvent ) return;

    if( m_fDrop ){
		m_fDrop = FALSE;
		SetTXFocus();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnLogFreq(BOOL fLink)
{
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::UpdateCallsign(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateLogMode(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateLogData(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateTextData(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::FindCall(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::AutoLogSave(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateLogLink(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CheckLogLink(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CMMML(TMessage Message)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CMMMR(TMessage Message)
{
	if( m_pRadio == NULL ) return;

	switch(Message.WParam){
		case MMR_DEFCOMMAND:
			{
				LPCSTR p;
				switch(Message.LParam){
					case 1:
						p = RADIO.CmdRx.c_str();
						break;
					case 2:
						p = RADIO.CmdTx.c_str();
						break;
					default:
						p = RADIO.CmdInit.c_str();
						break;
				}
				m_pRadio->SendCommand(p);
			}
			break;
		case MMR_VFO:
			m_pRadio->UpdateFreq(double(Message.LParam)/10000.0);
			break;
	}
	Message.Result = TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::InitDefKey(void)
{
	sys.m_DefKey[kkTX] = VK_F12;
    sys.m_DefKey[kkTXOFF] = VK_PAUSE;
}
//---------------------------------------------------------------------------
// 初回の初期化
__fastcall CRxSet::CRxSet()
{
	m_Mode = sys.m_DefaultMode;
    m_fTWO = 0;
    m_fMBCS = 0;
    m_fAFC = TRUE;
    m_SQLevel = 300;
	m_SQ = FALSE;
    m_SQTimer = 0;
    m_Speed = SPEED;
    m_AFCTimerW = 0;
    m_AFCTimerN = 0;
    m_AFCTimerW2 = 0;
    m_AFCFQ = 0;
    m_AFCSQ = 0;
    m_fATC = FALSE;
    m_WaterW = 400;
    m_pFFT = &MainVARI->m_FFT;
	m_CarrierFreq = 1750;
	m_MFSK_TYPE = typMFSK16;
	m_AFCTimerPSK = m_AFCTimerMFSK = 0;

	memset(&m_StgFFT, 0, sizeof(m_StgFFT));
	m_StgFFT.VW = 100;
	InitStgFFT();

	m_FontData.m_Name = "";
    m_FontData.m_Charset = 0;
    m_FontData.m_Height = 0;
    m_FontData.m_Style = 0;

    m_cAutoTS1 = m_cAutoTS2 = 0;
	m_AvgAFC.Create(8);

	m_pDem = NULL;
}
//---------------------------------------------------------------------------
__fastcall CRxSet::~CRxSet()
{
	Delete();
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetMFSKType(int type)
{
	m_MFSK_TYPE = type;
	if( IsActive() ) m_pDem->SetMFSKType(type);
}
//---------------------------------------------------------------------------
double __fastcall CRxSet::GetSpeed(void)
{
	if( IsMFSK() ){
		return IsActive() ? m_pDem->m_MFSK_SPEED : 15.625;
    }
    else {
		return m_Speed;
    }
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetSpeed(double b)
{
	if( b < MIN_SPEED ) return;
   	if( b > MAX_SPEED ) return;
	if( m_Speed != b ){
		m_Speed = b;
   		if( IsActive() ) m_pDem->SetSpeed(b);
    }
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetMode(int mode)
{
	int Offset = 0;
    double Speed = 0.0;
	if( m_Mode != mode ){
		if( ::IsRTTY(mode) ){
			if( !::IsRTTY(m_Mode) ){
				Speed = 45.45;
            }
        }
		else if( ::IsMFSK(mode) ){
			if( !::IsMFSK(m_Mode) ){
				Speed = 15.625;
            }
        }
        else {
			if( ::IsRTTY(m_Mode) || ::IsMFSK(m_Mode) ){
				Speed = 31.25;
            }
        }
        if( ::IsMFSK(mode) && ::IsMFSK(m_Mode) && !sys.m_MFSK_Center ){
			int bw = m_pDem ? m_pDem->m_MFSK_BW : 234.375;
			Offset = (mode == MODE_mfsk_U) ? -bw : bw;
        }
    }
	m_Mode = mode;
	if( Speed > 1.0 ) MainVARI->UpdateSpeed(this, Speed);
	MainVARI->InitCollect(this, ::IsRTTY(m_Mode) ? 7 : 14);
	switch(m_Mode){
		case MODE_RTTY:
        case MODE_U_RTTY:
		case MODE_GMSK:
        case MODE_FSK:
        case MODE_FSKW:
        case MODE_BPSK:
			m_fTWO = FALSE;
           	break;
        case MODE_N_BPSK:
        case MODE_mfsk_L:
        case MODE_mfsk_U:
			m_fTWO = TRUE;
           	break;
    }
	m_fMBCS = FALSE;
	m_AFCTimerPSK = m_AFCTimerMFSK = 0;
    if( IsActive() ) m_pDem->SetType(m_Mode);
	m_AvgAFC.Create(IsMFSK() ? 4 : 8);
    m_AvgAFC.Reset(m_pDem->m_CarrierFreq);
	MainVARI->UpdateMode(this, Offset);
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetCarrierFreq(double f)
{
	if( IsActive() ){
		m_pDem->SetCarrierFreq(f);
        m_CarrierFreq = f;
    }
    m_AvgAFC.Reset(f);
	m_AFCTimerPSK = 0;
    m_AFCFQ = f;
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::Create(BOOL fView)
{
    m_SQLevel = 300;
	m_SQ = FALSE;
    m_SQTimer = 0;
    m_AFCTimerW = 0;
    m_AFCTimerN = 0;
    m_AFCTimerW2 = 0;
    m_AFCFQ = 0;
    m_AFCSQ = 0;
    m_fATC = FALSE;
    m_pFFT = &MainVARI->m_FFT;

	m_StgFFT.dBSum = 0;
    m_StgFFT.dBMax = 0;
	m_StgFFT.dBWMax = 0;
	m_StgFFT.VW = 100;
    m_StgFFT.DispSig = 0;
    m_StgFFT.Timer = 0;
	InitStgFFT();

    m_cAutoTS1 = m_cAutoTS2 = 0;

    m_PeakSig = 0;
    m_AvgSig.Create(32);
	LimitInt(&m_CarrierFreq, MIN_CARRIER, sys.m_MaxCarrier);
	LimitInt(&m_SQLevel, 0, LEVELMAX);
    LimitDbl(&m_Speed, MIN_SPEED, MAX_SPEED);

	if( IsRTTY() ) m_Speed = 45.45;

	EPHASE(P_SUBCREATE);
	if( !m_pDem ){
		m_pDem = new CDEMFSK;
		if( !m_pDem ){
			MainVARI->SetErrorMsg("Out of memory");
			return;
        }
		m_pDem->SetMFSKType(m_MFSK_TYPE);
        m_pDem->SetSampleFreq(DEMSAMPFREQ);
        m_pDem->SetSpeed(m_Speed);
        m_pDem->SetType(m_Mode);
        m_pDem->m_Decode.SetATCSpeed(MainVARI->m_ATCSpeed);
        m_pDem->m_Decode.SetATCLimit(MainVARI->m_ATCLimit);
		if( fView ){			// サブウインドウの場合
            m_pDem->m_Decode.m_fATC = TRUE;
            m_pDem->m_fAFC = TRUE;
            m_pDem->m_fEnableAFC = TRUE;
			CDEMFSK *pDem = MainVARI->m_RxSet[0].m_pDem;
            m_pDem->MakeBPF(pDem->m_PreBPFTaps);
            m_pDem->m_pBPF = &pDem->m_inBPF;
			SetCarrierFreq(m_CarrierFreq);
//            m_fJA = MainVARI->m_RxSet[0].m_fJA;
        }
        InitStgFFT();
	    m_StgFFT.Timer = 0;
    }
	EPHASE(P_SUBVIEW);
#if 0
    if( fView && !m_pView ){
        m_pView = new TRxViewDlg(MainVARI);
		if( !m_pView ){
			delete m_pDem; m_pDem = NULL;
			MainVARI->SetErrorMsg("Out of memory");
			return;
        }
        m_pView->m_pRxSet = this;
 		m_pView->SetBounds(m_rcView.left, m_rcView.top, m_rcView.right, m_rcView.bottom);
        m_pView->UpdateWaterWidth();
		m_pView->m_Dump.SetRTTY(IsRTTY());
		m_pView->m_Dump.ShowCtrl(sys.m_fShowCtrlCode);
		if( m_FontData.m_Height ){
			FontData2Font(m_pView->PC->Font, &m_FontData);
        }
		m_pView->OnUpdateFont();
        memcpy(m_pView->m_Dump.m_Color, MainVARI->m_Dump.m_Color, sizeof(MainVARI->m_Dump.m_Color));
//        ::SetWindowPos(m_pView->Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
    }
#endif
	EPHASE(P_NULL);
    SetMode(m_Mode);
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::Delete(void)
{
	if( m_pDem ){
		m_CarrierFreq = m_pDem->m_CarrierFreq + 0.5;
    	delete m_pDem;
        m_pDem = NULL;
    }
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::SetSampleFreq(double f)
{
	if( !IsActive() ) return;
	m_pDem->SetSampleFreq(f);
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::InitStgFFT(void)
{
	if( m_pFFT->m_FFTGain ){
		m_StgFFT.Sum = 1024;
	    m_StgFFT.Max = 1024;
    	m_StgFFT.WMax = 1024;
    }
    else {
        m_StgFFT.Sum = 5000;
   	    m_StgFFT.Max = 5000;
   	    m_StgFFT.WMax = 5000;
    }
    m_StgFFT.Sig = 0;
}
//---------------------------------------------------------------------------
void __fastcall CRxSet::ClearWindow(void)
{
	if( !IsActive() ) return;
}
//---------------------------------------------------------------------------
double __fastcall CRxSet::GetBandWidth(void)
{
	switch(m_Mode){
		case MODE_GMSK:
        case MODE_BPSK:
        case MODE_N_BPSK:
			return m_Speed;
        case MODE_FSK:
			return m_Speed*2;
        case MODE_FSKW:
		case MODE_RTTY:
        case MODE_U_RTTY:
			return m_pDem->m_RTTYShift;
        case MODE_mfsk_L:
        case MODE_mfsk_U:
			if( m_pDem ){
				return m_pDem->m_MFSK_BW;
            }
            else {
	        	return 234.375;
            }
    }
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateSubWindow(void)
{
	m_fSubWindow = FALSE;
	int i;
	for( i = 1; i < RXMAX; i++ ){
		if( m_RxSet[i].IsActive() ) m_fSubWindow = TRUE;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::ShowSubChannel(int n, int sw)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxFFTMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	if( SBWave->Down ) return;

    // FFTまたはWaterFallの場合
	if( m_MouseDown ){
		int fq = m_FFTB + X * m_FFTW / m_fftXW;
        if( fq < MIN_CARRIER ) fq = MIN_CARRIER;
        if( fq > sys.m_MaxCarrier ) fq = sys.m_MaxCarrier;
		if( m_MouseNotch ){
			m_NotchFreq = fq;
            SetNotchFreq();
			PBoxFFTPaint(NULL);
        }
		else if( m_MouseSubChannel ){
			CRxSet *pRxSet = &m_RxSet[m_MouseSubChannel];
			if( pRxSet->IsActive() ){
				pRxSet->m_pDem->m_Decode.Reset();
				pRxSet->m_pDem->ResetMeasMFSK();
				pRxSet->SetCarrierFreq(fq);
				PBoxFFTPaint(NULL);
            }
        }
        else {
			m_RxSet[0].m_pDem->m_Decode.Reset();
			SetRxFreq(fq);
   	    }
    }
    else if( m_fSubWindow || m_fNOTCH ){
		m_MouseNotch = FALSE;
    	m_MouseSubChannel = FALSE;
		if( m_fNOTCH ){
			int xx = X - m_NotchX;
    	    int yy = Y - m_NotchY;
        	int d = (xx * xx + yy * yy);
	        if( d < 64 ){
				m_MouseNotch = TRUE;
        	}
        }
		if( !m_MouseNotch ){
			CRxSet *pRxSet = &m_RxSet[4];
			for( int i = RXMAX-1; i > 0; i--, pRxSet-- ){
				if( pRxSet->IsActive() ){
					int xx = X - pRxSet->m_X;
                	int yy = Y - pRxSet->m_Y;
	                int d = (xx * xx + yy * yy);
    	            if( d < 64 ){
						m_MouseSubChannel = i;
            	        break;
                	}
	            }
    	    }
        }
        PBoxFFT->Cursor = (m_MouseNotch || m_MouseSubChannel) ? crSizeWE : crDefault;
    }
    else {
		m_MouseSubChannel = FALSE;
        m_MouseNotch = FALSE;
//		int XO = (UdRxCarrier->Position - m_FFTB) * m_fftXW / m_FFTW;
//		XO -= X; XO *= XO;
//        PBoxFFT->Cursor = (XO < 64) ? crSizeWE : crDefault;
        PBoxFFT->Cursor = crDefault;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxFFTMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( m_MouseDown && !SBFFT500->Down ){
		int fq = m_FFTB + X * m_FFTW / m_fftXW;
        if( fq < MIN_CARRIER ) fq = MIN_CARRIER;
        if( fq > sys.m_MaxCarrier ) fq = sys.m_MaxCarrier;
		if( m_MouseNotch ){
			m_NotchFreq = fq;
            SetNotchFreq();
			PBoxFFTPaint(NULL);
        }
    	else if( m_MouseSubChannel ){
			CRxSet *pRxSet = &m_RxSet[m_MouseSubChannel];
			if( pRxSet->IsActive() ){
				fq = GetSignalFreq(fq, SBFFT3K->Down ? 50 : 32, pRxSet);
				pRxSet->m_pDem->m_Decode.Reset();
				pRxSet->m_pDem->ResetMeasMFSK();
                pRxSet->m_PeakSig = 0;
				pRxSet->SetCarrierFreq(fq);
                pRxSet->m_AFCTimerMFSK = (SBFFT500->Down || SBFFT1K->Down) ? MFSKAFC_MAX : 0;
				PBoxFFTPaint(NULL);
            }
		}
        else {
			fq = GetSignalFreq(fq, SBFFT3K->Down ? 50 : 32, &m_RxSet[0]);
			m_RxSet[0].m_pDem->m_Decode.Reset();
			SetRxFreq(fq);
			m_RxSet[0].m_AFCTimerMFSK = (SBFFT500->Down || SBFFT1K->Down) ? MFSKAFC_MAX : 0;
            m_RxSet[0].m_PeakSig = 0;
        }
	}
	m_MouseDown = FALSE;
    m_MouseSubChannel = FALSE;
    m_MouseNotch = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KCallClick(TObject *Sender)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::AddCall(LPCSTR p)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetTXFocus(void)
{
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::DeleteMacroTimerS(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DeleteMacroTimer(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CreateMacroTimer(int n)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateLogHeight(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateMacButtonVW(int n)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KORClick(TObject *Sender)
{
	DettachFocus();
	TRADIOSetDlg *pBox = new TRADIOSetDlg(this);
	RADIO.change = pBox->Execute();
    delete pBox;
	AttachFocus();
    if( RADIO.change ) OpenRadio();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateMFSKSpeed(int mode)
{
	if( IsMFSK(mode) ){
#if ENABLE_MFSK8
		SetComboBox(CBSpeed, "7.8125,15.625,31.25");
#else
		SetComboBox(CBSpeed, "15.625,31.25");
#endif
    }
    else {
		SetComboBox(CBSpeed, m_ListBAUD.c_str());
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetMode(int mode)
{
	if( m_RxSet[0].m_Mode != mode ){
		if( IsMFSK(mode) && !IsMFSK(m_RxSet[0].m_Mode) ){
			UpdateMFSKSpeed(mode);
        }
        else if( !IsMFSK(mode) && IsMFSK(m_RxSet[0].m_Mode) ){
			UpdateMFSKSpeed(mode);
        }
		m_RxSet[0].SetMode(mode);
	    m_ModFSK.SetType(mode);
#if DEBUG
		m_ModTest.SetType(mode);
#endif
        UpdateLogMode();
        UpdateUI();
		switch(mode){
			case MODE_GMSK:
				SetInfoMsg(sys.m_MsgEng ? "GMSK : Use this mode usually for MBCS language":"GMSK : 通常はこのモードを使う");
            	break;
			case MODE_FSK:
				SetInfoMsg(sys.m_MsgEng ? "FSK : For V/UHF":"FSK : V/UHF帯用モード");
            	break;
			case MODE_FSKW:
				SetInfoMsg(sys.m_MsgEng ? "FSK-W : For V/UHF and satellite":"FSK-W : V/UHF帯/衛星通信用モード");
            	break;
            case MODE_BPSK:
            	SetInfoMsg(sys.m_MsgEng ? "BPSK : No compatible on MBCS (TX disabled on VariJA)":"BPSK : 従来方式と互換性なし(VariJAは送信禁止)");
                break;
            case MODE_N_BPSK:
            	SetInfoMsg(sys.m_MsgEng ? "bpsk : Compatible with conventionally VARICODE":"bpsk : 従来方式と互換性あり");
                break;
            case MODE_RTTY:
            	SetInfoMsg("rtty(LSB) : BAUDOT");
                break;
            case MODE_U_RTTY:
            	SetInfoMsg("rtty(USB) : BAUDOT");
                break;
            case MODE_mfsk_L:
            case MODE_mfsk_U:
				sprintf(m_TextBuff, "mfsk%d (%s)", int((250.0/m_RxSet[0].m_pDem->m_MFSK_TONES)+0.5), mode == MODE_mfsk_L ? "LSB":"USB");
            	SetInfoMsg(m_TextBuff);
                break;
        }
		UpdateLogMode();
		DoEvent(macOnMode);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoSuspend(BOOL fMinimize)
{
	if( !m_fSuspend ){
		if( m_TX ) ToRX();
		m_Wave.InClose();
    	m_Wave.OutAbort();
        if( m_pCom ){
			delete m_pCom;
            m_pCom = NULL;
        }
        if( m_pRadio ){
			delete m_pRadio;
            m_pRadio = NULL;
        }
        UpdateLogPanel();
		m_fSuspend = TRUE;
		m_fShowMsg = TRUE;
    }
    if( fMinimize && (WindowState != wsMinimized) ) Application->Minimize();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoResume(void)
{
	if( m_fSuspend ){
		if( OpenSound(FALSE) ){
			OpenCom();
            OpenRadio();
            m_fSuspend = FALSE;
            if( WindowState == wsMinimized ) Application->Restore();
        }
        else {
			Draw(FALSE);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetEditPage(int cno)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::InitCollect(CRxSet *pRxSet, int n)
{
	if( pRxSet != &m_RxSet[0] ) return;

	if( n < 4 ) n = 4;
	if( n > 24 ) n = 24;
	m_WaveBitMax = n;
	InitCollect();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::InitCollect(void)
{
	double speed = m_RxSet[0].m_Speed;
    if( m_RxSet[0].IsMFSK() ) speed = m_RxSet[0].m_pDem->m_MFSK_SPEED;
    m_Collect1.Create(DEMSAMPFREQ*m_WaveBitMax/speed);
    m_Collect2.Create(DEMSAMPFREQ*m_WaveBitMax/speed);
	if( m_RxSet[0].IsActive() ){
	    int n = m_WaveBitMax;
    	if( !(n & 1) ) n++;
	    m_RxSet[0].m_pDem->m_Decode.m_cBWave = n;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SBWaveMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if( Button == mbRight ){
		m_WaveType = m_WaveType ? 0 : 1;
        UpdateWaveCaption();
    }
}
//---------------------------------------------------------------------------
#if DEBUG
void __fastcall TMainVARI::TestSignal(void)
{
}
#endif
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetATCSpeed(int f)
{
	m_ATCSpeed = f;
    CRxSet *pRxSet = m_RxSet;
	for( int i = 0; i < RXMAX; i++, pRxSet++ ){
    	if( pRxSet->IsActive() ) pRxSet->m_pDem->m_Decode.SetATCSpeed(f);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetATCLimit(int f)
{
	m_ATCLimit = f;
    CRxSet *pRxSet = m_RxSet;
	for( int i = 0; i < RXMAX; i++, pRxSet++ ){
    	if( pRxSet->IsActive() ) pRxSet->m_pDem->m_Decode.SetATCLimit(f);
    }
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetSignalFreq(int fo, int fm, CRxSet *pRxSet)
{
	return GetSignalFreq(fo, fm, pRxSet, 600);
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetSignalFreq(int fo, int fm, CRxSet *pRxSet, int th)
{
	if( pRxSet->IsMFSK() ){
		return fo;
    }
	else if( pRxSet->Is170() ){
    	if( (pRxSet != &m_RxSet[0]) || pRxSet->m_fAFC ){
			fm = pRxSet->m_pDem->m_RTTYShift + 30;
        }
        else {
        	return fo;
        }
    }
	int xl = (fo - fm) * FFT_SIZE / m_FFTSampFreq;
	int xh = (fo + fm) * FFT_SIZE / m_FFTSampFreq;
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
		avgdb = SqrtToDB(avg);
		maxdb = SqrtToDB(max);
    }
    else {
		double k = 100.0 / m_fftSC;
        avgdb = avg * k;
        maxdb = max * k;
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
			xl = (pRxSet->m_pDem->m_RTTYShift - 20) * FFT_SIZE/m_FFTSampFreq;
    	    xh = (pRxSet->m_pDem->m_RTTYShift + 30) * FFT_SIZE/m_FFTSampFreq;
        }
        else {
			xl = 0;
    	    xh = (pRxSet->m_Speed + 10) * FFT_SIZE/m_FFTSampFreq;
        }
       	max = ABS(fh-fl);
		if( ((max >= xl) && (max <= xh)) ){
	    	x = (fl + fh) / 2;
			fo = x * m_FFTSampFreq / FFT_SIZE;
        }
	}
    return fo;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::LoadMacro(LPCSTR pName)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SaveMacro(LPCSTR pName)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::LoadMacro(TMemIniFile *pIniFile)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SaveMacro(TMemIniFile *pIniFile)
{
}
//---------------------------------------------------------------------------
int __fastcall TMainVARI::GetOverlayTop(void)
{
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetConvAlpha(BOOL f)
{
}
//---------------------------------------------------------------------------

void __fastcall TMainVARI::KVSDClick(TObject *Sender)
{
	m_ScaleDetails = m_ScaleDetails ? FALSE : TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KORSCClick(TObject *Sender)
{
	m_fRttyWordOut = (Sender == KORSW);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::SetNotchFreq(void)
{
	if( m_fNOTCH ){
		if( m_NotchFreq < 100 ) m_NotchFreq = 100;
    	if( m_NotchFreq > sys.m_MaxCarrier ) m_NotchFreq = sys.m_MaxCarrier;
		double fl = m_NotchFreq - m_NotchWidth;
		double fh = m_NotchFreq + m_NotchWidth;
        int att = m_NotchWidth > 50 ? 60 : 10;
		int taps;
        double samp = m_FFTSampFreq;
		switch(SAMPTYPE){
	        case 0:		// 11025Hz
				taps = m_NotchTaps;
        		break;
	        case 1:		// 8000Hz
			case 2:		// 6000Hz
				taps = m_NotchTaps/2;
        		break;
	        case 4:		// 16000Hz
    	    case 6:		// 22050Hz
				taps = m_NotchTaps*2;
                samp = SAMPFREQ;
        		break;
	        default:
				taps = m_NotchTaps/2;
        		break;
	    }
		if( fl < 100.0 ){
			m_NOTCH.Create(taps, ffHPF, samp, fh, fh, 60, 1.0);
        }
        else if( (fh > sys.m_MaxCarrier) && (m_NotchWidth >= 10) ){
			m_NOTCH.Create(taps, ffLPF, samp, fl, fl, 60, 1.0);
		}
        else {
			m_NOTCH.Create(taps, ffBEF, samp, fl, fh, att, 1.0);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KSNClick(TObject *Sender)
{
	m_fNOTCH = TRUE;
    if( m_fNOTCH ) m_NotchFreq = m_RightFreq;
    SetNotchFreq();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KSTXClick(TObject *Sender)
{
	if( m_TX || !SBNET->Down ){
		SetTxFreq(m_RightFreq);
    }
    else if( SBNET->Down ){
		SBNET->Down = FALSE;
		SetTxFreq(m_RightFreq);
	}
	UpdateUI();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KSRXClick(TObject *Sender)
{
	m_RxSet[0].m_pDem->m_Decode.Reset();
	SetRxFreq(m_RightFreq);
    m_RxSet[0].m_AFCTimerMFSK = MFSKAFC_MAX;
    UpdateUI();
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KSNRClick(TObject *Sender)
{
	m_fNOTCH = FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KSS1Click(TObject *Sender)
{
	TMenuItem *pMenu = (TMenuItem *)Sender;

	int RN = 1;
    if( pMenu == KSS2 ){
		RN = 2;
    }
    else if( pMenu == KSS3 ){
		RN = 3;
    }
    else if( pMenu == KSS4 ){
		RN = 4;
    }
	ShowSubChannel(RN, 1);

	CRxSet *pRxSet = &m_RxSet[RN];
	double fq = GetSignalFreq(m_RightFreq, SBFFT3K->Down ? 50 : 32, pRxSet);
	pRxSet->m_pDem->m_Decode.Reset();
	pRxSet->SetCarrierFreq(fq);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::KSSRClick(TObject *Sender)
{
	ShowSubChannel(m_MouseSubChannel, 0);
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::IsFreqErr(double d)
{
	if( IsBPSK() ){
		return fabs(d) > 0.30;		// 0.15 X 2.0
    }
	else {
		return fabs(d) > 0.20;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DrawPF(BOOL fPaint)
{
	if( !m_PFTimer ){
		TCanvas *pCanvas = m_pBitmapPF->Canvas;
    	TRect rc;
	   	rc.Left = 0; rc.Top = 0; rc.Right = m_pfXW; rc.Bottom = m_pfYW;
   		pCanvas->Brush->Color = clBlack;
	   	pCanvas->Pen->Color = clBlack;
		pCanvas->Pen->Style = psSolid;
	   	pCanvas->FillRect(rc);

	    if( m_Wave.IsInOpen() || m_Wave.IsOutOpen() ){
			rc.Top ++;
            rc.Bottom--;
			BOOL fTX = (m_TX == txINTERNAL);
			CRxSet *pRxSet = m_RxSet;
			CDEMFSK *pDem = pRxSet->m_pDem;
			double d = pDem->GetFreqErr();
			if( IsBPSK() ){
				if( m_AFCWidth && !fTX ){
					double f = double(pRxSet->m_AFCFQ - UdRxCarrier->Position)/pRxSet->GetBandWidth();
					if( fabs(f) >= 0.2 ) d = f;
                }
                d *= 2.0;
            }
            if( fTX ) d *= 0.05;
			if( d > 0.0 ){
				rc.Left = m_pfXC + (d * m_pfXW);
    		    rc.Right = m_pfXC;
	    	}
	    	else {
				rc.Left = m_pfXC;
    		    rc.Right = m_pfXC + (d * m_pfXW);
		    }
			if( SBAFC->Down || fTX ){
	    		pCanvas->Brush->Color = TColor(RGB(144,144,144));
	        }
    	    else {
	    		pCanvas->Brush->Color = IsFreqErr(d) ? clRed : clYellow;
	        }
		   	pCanvas->FillRect(rc);
	    }
	    pCanvas->Pen->Color = SBAFC->Down ? TColor(RGB(0,255,0)) : clWhite;
    	pCanvas->MoveTo(m_pfXC, 0);
	    pCanvas->LineTo(m_pfXC, m_pfYW);
    	if( fPaint ) PBoxPFPaint(NULL);
	}
    else {
		m_PFTimer--;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::PBoxPFPaint(TObject *Sender)
{
	PBoxPF->Canvas->Draw(0, 0, m_pBitmapPF);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutYaesuVU(int hz)
{
	if( !m_pRadio ) return;

	char bf[256];
	int hz1 = hz / 10000000;
    hz = hz % 10000000;
    int hz2 = hz / 100000;
    hz = hz % 100000;
    int hz3 = hz / 1000;
    hz = hz % 1000;
	sprintf(bf, "\\$%02u%02u%02u%02u01", hz1, hz2, hz3, hz/10);
	m_pRadio->SendCommand(bf);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutYaesuHF(int hz)
{
	if( !m_pRadio ) return;

	char bf[256];
	int hz1 = hz / 10000000;
    hz = hz % 10000000;
    int hz2 = hz / 100000;
    hz = hz % 100000;
    int hz3 = hz / 1000;
    hz = hz % 1000;
	sprintf(bf, "\\$%02u%02u%02u%02u0A", hz/10, hz3, hz2, hz1);
	m_pRadio->SendCommand(bf);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutCIV(int hz)
{
	if( !m_pRadio ) return;

	char bf[256];
	int hz1 = hz / 100000000;
	hz = hz % 100000000;
	int hz2 = hz / 1000000;
	hz = hz % 1000000;
	int hz3 = hz / 10000;
	hz = hz % 10000;
    int hz4 = hz / 100;
    hz = hz % 100;
	sprintf(bf, "\\$FEFExxE005%02u%02u%02u%02u%02uFD", hz, hz4, hz3, hz2, hz1);
	WaitICOM();
	m_pRadio->SendCommand(bf);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutCIV4(int hz)
{
	if( !m_pRadio ) return;

	char bf[256];
	if( hz >= 100000000 ){	// 100MHz and up
		int hz1 = hz / 100000000;
		hz = hz % 100000000;
		int hz2 = hz / 1000000;
		hz = hz % 1000000;
		int hz3 = hz / 10000;
		hz = hz % 10000;
        int hz4 = hz / 100;
        hz = hz % 100;
		sprintf(bf, "\\$FEFExxE005%02u%02u%02u%02u%02uFD", hz, hz4, hz3, hz2, hz1);
    }
    else {
		int hz1 = hz / 1000000;
		hz = hz % 1000000;
		int hz2 = hz / 10000;
		hz = hz % 10000;
		int hz3 = hz / 100;
		hz = hz % 100;
		sprintf(bf, "\\$FEFExxE005%02u%02u%02u%02uFD", hz, hz3, hz2, hz1);
	}
	WaitICOM();
	m_pRadio->SendCommand(bf);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutKENWOOD(int hz)
{
	if( !m_pRadio ) return;

	char bf[256];
    sprintf(bf, "FA0%010lu;", hz);
    m_pRadio->SendCommand(bf);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutJST245(int hz)
{
	if( !m_pRadio ) return;

	char bf[256];
    sprintf(bf, "H1\\rF%08luA\\rH0\\rI1\\r", hz);
    m_pRadio->SendCommand(bf);
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutModeYaesuHF(LPCSTR pMode)
{
	if( !m_pRadio ) return;

    const BYTE _td[]={
		0x00, 0x01, 0x02, 0x04, 0x06, 0x08, 0x0a,
    };
    int n = FindStringTable(g_tRadioMode, pMode, AN(_td));
	if( n >= 0 ){
		char bf[256];
		sprintf(bf, "\\$000000%02X0C", _td[n]);
		m_pRadio->SendCommand(bf);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutModeYaesuVU(LPCSTR pMode)
{
	if( !m_pRadio ) return;

    const BYTE _td[]={
		0x00, 0x01, 0x82, 0x04, 0x08,
    };
    int n = FindStringTable(g_tRadioMode, pMode, AN(_td));
	if( n >= 0 ){
		char bf[256];
		sprintf(bf, "\\$%02X00000007", _td[n]);
		m_pRadio->SendCommand(bf);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutModeCIV(LPCSTR pMode)
{
	if( !m_pRadio ) return;

    const BYTE _td[]={
		0x00, 0x01, 0x03, 0x02, 0x05, 0x04,
    };
    int n = FindStringTable(g_tRadioMode, pMode, AN(_td));
	if( n >= 0 ){
		char bf[256];
		sprintf(bf, "\\$FEFExxE006%02XFD", _td[n]);
		WaitICOM();
		m_pRadio->SendCommand(bf);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutModeKENWOOD(LPCSTR pMode)
{
	if( !m_pRadio ) return;

    const BYTE _td[]={
		1, 2, 3, 5, 4, 6,
    };
    int n = FindStringTable(g_tRadioMode, pMode, AN(_td));
	if( n >= 0 ){
		char bf[256];
		sprintf(bf, "MD%d;", int(_td[n]));
		m_pRadio->SendCommand(bf);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OutModeJST245(LPCSTR pMode)
{
	if( !m_pRadio ) return;

    const BYTE _td[]={
		3, 2, 1, 4, 5, 0,
    };
    int n = FindStringTable(g_tRadioMode, pMode, AN(_td));
	if( n >= 0 ){
		char bf[256];
		sprintf(bf, "H1\rD%d\rH0\rI1\r", int(_td[n]));
		m_pRadio->SendCommand(bf);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::WaitICOM(void)
{
	if( m_pRadio && m_pRadioTimer ){
		m_pRadio->WaitICOM(m_pRadioTimer->Interval);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DeleteMacExButton(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::CreateMacExButton(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoMacroMenu(LPCSTR pVal, TSpeedButton *pButton, BOOL fRadio)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdatePlayBack(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoPlayBack(int s)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::StopPlayBack(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateShowCtrl(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoEvent(int n)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::MacroOnTimer(TObject *Sender)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateMacroOnTimer(void)
{
}
//---------------------------------------------------------------------------
// システムシャットダウン
void __fastcall TMainVARI::ShutDown(void)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::UpdateModGain(void)
{
	m_ModGainR = m_ModGain;
    if( m_ModGainR < 1024 ) m_ModGainR = 1024;
    if( m_ModGainR > 31774 ) m_ModGainR = 31774;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoBench(LPSTR t, int type)
{
}
//---------------------------------------------------------------------------
// インデックスからメニューを得る
TMenuItem* __fastcall TMainVARI::GetMenuItem(TMenuItem *pMainMenu, LPCSTR pTitle)
{
	return NULL;
}
//---------------------------------------------------------------------------
TMenuItem* __fastcall TMainVARI::FindMenu(TMenuItem *pMainMenu, LPCSTR pTitle)
{
    return NULL;
}
//---------------------------------------------------------------------------
TMenuItem* __fastcall TMainVARI::GetMainMenu(LPCSTR pTitle, BOOL fNew)
{
    return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnXmClick(TObject *Sender)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::AddExtensionMenu(LPCSTR pTitle, LPCSTR pCaption, LPCSTR pHint)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::InsExtensionMenu(LPCSTR pTitle, LPCSTR pPos, LPCSTR pCaption, LPCSTR pHint)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::ShortCutExtensionMenu(TMenuItem *pMenu, LPCSTR pKey)
{
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::IsXMenu(TMenuItem *pItem)
{
    return FALSE;
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::OnXClick(TObject *Sender)
{
}
//---------------------------------------------------------------------------
BOOL __fastcall TMainVARI::OnMenuProc(TMenuItem *pMenu, LPCSTR pProc, LPCSTR pPara, BOOL fStop)
{
	return FALSE;
}
//---------------------------------------------------------------------------
// メニューの更新ハンドラ
void __fastcall TMainVARI::OnMenuProc(TMenuItem *pMenu, LPCSTR pCaption)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::DoParentClick(TMenuItem *pMenu)
{
}
//---------------------------------------------------------------------------
TMenuItem* __fastcall TMainVARI::GetMenuArg(AnsiString &arg, LPCSTR pVal, BOOL fArg)
{
}
//---------------------------------------------------------------------------
TSpeedButton* __fastcall TMainVARI::FindButton(TComponent *pMainControl, LPCSTR pCaption, TSpeedButton *pButton, BOOL fErrMsg)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainVARI::ClickButton(TSpeedButton *pButton)
{
}
//---------------------------------------------------------------------------



