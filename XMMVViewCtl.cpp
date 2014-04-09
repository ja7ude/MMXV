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

// XMMVViewCtl.cpp : CXMMVViewCtrl ActiveX コントロール クラスのインプリメンテーション

#include "stdafx.h"
#include "MMVARI.h"
#include "XMMVViewCtl.h"
#include "XMMVViewPpg.h"
#include "MMVARICtl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CXMMVViewCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// メッセージ マップ

BEGIN_MESSAGE_MAP(CXMMVViewCtrl, COleControl)
	//{{AFX_MSG_MAP(CXMMVViewCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// ディスパッチ マップ

BEGIN_DISPATCH_MAP(CXMMVViewCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CXMMVViewCtrl)
	DISP_PROPERTY_NOTIFY(CXMMVViewCtrl, "dwFreqHz", m_dwFreqHz, OnDwFreqHzChanged, VT_I4)
	DISP_PROPERTY_NOTIFY(CXMMVViewCtrl, "bLSB", m_bLSB, OnBLSBChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CXMMVViewCtrl, "wBaseFreq", m_wBaseFreq, OnWBaseFreqChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CXMMVViewCtrl, "wWidthFreq", m_wWidthFreq, OnWWidthFreqChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CXMMVViewCtrl, "bWaterAGC", m_bWaterAGC, OnBWaterAGCChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CXMMVViewCtrl, "Visible", m_visible, OnVisibleChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CXMMVViewCtrl, "bFollowRxFreq", m_bFollowRxFreq, OnBFollowRxFreqChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CXMMVViewCtrl, "wType", m_wType, OnWTypeChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CXMMVViewCtrl, "wWaterNoiseL", m_wWaterNoiseL, OnWWaterNoiseLChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CXMMVViewCtrl, "wWaterNoiseH", m_wWaterNoiseH, OnWWaterNoiseHChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CXMMVViewCtrl, "bShowSync", m_bShowSync, OnBShowSyncChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CXMMVViewCtrl, "wCursorFreq", m_wCursorFreq, OnWCursorFreqChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CXMMVViewCtrl, "bHighQuality", m_bHighQuality, OnBHighQualityChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CXMMVViewCtrl, "bShowErrorMsg", m_bShowErrorMsg, OnBShowErrorMsgChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CXMMVViewCtrl, "bAttachMouse", m_bAttachMouse, OnBAttachMouseChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CXMMVViewCtrl, "bWindowsMouseEvent", m_bWindowsMouseEvent, OnBWindowsMouseEventChanged, VT_BOOL)
	DISP_PROPERTY_EX(CXMMVViewCtrl, "wWaterNoise", GetWWaterNoise, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CXMMVViewCtrl, "wPixelXW", GetWSizeXW, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CXMMVViewCtrl, "wPixelYW", GetWPixelYW, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CXMMVViewCtrl, "wMouseNotch", GetWMouseNotch, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CXMMVViewCtrl, "wMouseChannel", GetWMouseChannel, SetNotSupported, VT_I2)
	DISP_FUNCTION(CXMMVViewCtrl, "Draw", Draw, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CXMMVViewCtrl, "DrawWave", DrawWave, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CXMMVViewCtrl, "ClearWater", ClearWater, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CXMMVViewCtrl, "DrawFreqErr", DrawFreqErr, VT_EMPTY, VTS_I4 VTS_I2)
	DISP_FUNCTION(CXMMVViewCtrl, "DrawSmallWater", DrawSmallWater, VT_EMPTY, VTS_I4 VTS_I2)
	DISP_PROPERTY_PARAM(CXMMVViewCtrl, "ColorFFT", GetColorFFT, SetColorFFT, VT_COLOR, VTS_I2)
	DISP_PROPERTY_PARAM(CXMMVViewCtrl, "ColorWater", GetColorWater, SetColorWater, VT_COLOR, VTS_I2)
	DISP_PROPERTY_PARAM(CXMMVViewCtrl, "ColorWave", GetColorWave, SetColorWave, VT_COLOR, VTS_I2)
	DISP_PROPERTY_PARAM(CXMMVViewCtrl, "wWaterLevel", GetWWaterLevel, SetWWaterLevel, VT_I2, VTS_I2)
	DISP_PROPERTY_PARAM(CXMMVViewCtrl, "WaterPalette", GetWaterPalette, SetWaterPalette, VT_COLOR, VTS_I2)
	DISP_PROPERTY_PARAM(CXMMVViewCtrl, "strMsg", GetStrMsg, SetStrMsg, VT_BSTR, VTS_I2)
	DISP_PROPERTY_PARAM(CXMMVViewCtrl, "ColorFreqErr", GetColorFreqErr, SetColorFreqErr, VT_COLOR, VTS_I2)
	DISP_PROPERTY_PARAM(CXMMVViewCtrl, "strErrorMsg", GetStrErrorMsg, SetStrErrorMsg, VT_BSTR, VTS_I2)
	DISP_STOCKFUNC_REFRESH()
	DISP_STOCKPROP_BORDERSTYLE()
	DISP_STOCKPROP_FONT()
	DISP_STOCKPROP_HWND()
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// イベント マップ

BEGIN_EVENT_MAP(CXMMVViewCtrl, COleControl)
	//{{AFX_EVENT_MAP(CXMMVViewCtrl)
	EVENT_CUSTOM("OnLMouseDown", FireOnLMouseDown, VTS_I2)
	EVENT_CUSTOM("OnLMouseUp", FireOnLMouseUp, VTS_I2)
	EVENT_CUSTOM("OnLMouseMove", FireOnLMouseMove, VTS_I2)
	EVENT_CUSTOM("OnRMouseDown", FireOnRMouseDown, VTS_I2)
	EVENT_CUSTOM("OnRMouseMove", FireOnRMouseMove, VTS_I2)
	EVENT_CUSTOM("OnRMouseUp", FireOnRMouseUp, VTS_I2)
	EVENT_CUSTOM("OnWindowsLMouseDown", FireOnWindowsLMouseDown, VTS_I2  VTS_I2)
	EVENT_CUSTOM("OnWindowsLMouseUp", FireOnWindowsLMouseUp, VTS_I2  VTS_I2)
	EVENT_CUSTOM("OnWindowsRMouseUp", FireOnWindowsRMouseUp, VTS_I2  VTS_I2)
	EVENT_CUSTOM("OnWindowsRMouseDown", FireOnWindowsRMouseDown, VTS_I2  VTS_I2)
	EVENT_CUSTOM("OnWindowsMouseMove", FireOnWindowsMouseMove, VTS_I4  VTS_I2  VTS_I2)
	EVENT_CUSTOM("OnWindowsLMouseDblClk", FireOnWindowsLMouseDblClk, VTS_I2  VTS_I2)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// プロパティ ページ

// TODO: プロパティ ページを追加して、BEGIN 行の最後にあるカウントを増やしてください。
BEGIN_PROPPAGEIDS(CXMMVViewCtrl, 1)
	PROPPAGEID(CXMMVViewPropPage::guid)
END_PROPPAGEIDS(CXMMVViewCtrl)


/////////////////////////////////////////////////////////////////////////////
// クラス ファクトリと guid を初期化します

IMPLEMENT_OLECREATE_EX(CXMMVViewCtrl, "XMMVVIEW.XMMVViewCtrl.1",
	0x702cbf07, 0xc159, 0x44f2, 0xb8, 0xa6, 0xdf, 0x8e, 0xa1, 0, 0x1e, 0x8)


/////////////////////////////////////////////////////////////////////////////
// タイプ ライブラリ ID とバージョン

IMPLEMENT_OLETYPELIB(CXMMVViewCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// インターフェイス ID

const IID BASED_CODE IID_DXMMVView =
		{ 0xd821b75d, 0x3e58, 0x4a6e, { 0xa6, 0x2, 0x39, 0x2f, 0xe4, 0xb9, 0xf, 0x50 } };
const IID BASED_CODE IID_DXMMVViewEvents =
		{ 0xc5f81d0f, 0xbf24, 0x4300, { 0x92, 0xfa, 0xdf, 0xb6, 0xde, 0x38, 0xc1, 0x1 } };


/////////////////////////////////////////////////////////////////////////////
// コントロールの型情報の制御

static const DWORD BASED_CODE _dwXMMVViewOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CXMMVViewCtrl, IDS_XMMVVIEW, _dwXMMVViewOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CXMMVViewCtrl::CXMMVViewCtrlFactory::UpdateRegistry -
// CXMMVViewCtrl のシステム レジストリのエントリを追加または削除します 

BOOL CXMMVViewCtrl::CXMMVViewCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_XMMVVIEW,
			IDB_XMMVVIEW,
			afxRegApartmentThreading,
			_dwXMMVViewOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVViewCtrl::CXMMVViewCtrl - コンストラクタ

CXMMVViewCtrl::CXMMVViewCtrl()
{
	InitializeIIDs(&IID_DXMMVView, &IID_DXMMVViewEvents);

	m_bMouseDown = FALSE;
	m_bmpSize.cx = 0;
	m_bmpSize.cy = 0;
	memset(m_NotchX, 0, sizeof(m_NotchX));
	memset(m_NotchY, 0, sizeof(m_NotchY));
	for( int i = 0; i < RXMAX; i++ ) m_SX[i] = m_SY[i] = -1;
	memset(m_cInfoMsg, 0, sizeof(m_cInfoMsg));
	m_MouseSubChannel = 0;
	m_MouseNotch = 0;

	CWinApp *pApp = AfxGetApp();
	m_StdCursor[CF_ARROW] = pApp->LoadStandardCursor(IDC_ARROW);
	m_StdCursor[CF_SIZEWE] = pApp->LoadStandardCursor(IDC_SIZEWE);

	m_pMainCtrl = NULL;
	m_bWaterAGC = FALSE;
	m_FFTGain = 0;

	m_wWaterNoiseL = 35;
	m_wWaterNoiseH = 65;

	m_strErrorMsg[errorLostRX] = "RX sound was lost";
	m_strErrorMsg[errorLostTX] = "TX sound was lost";
	m_strErrorMsg[errorSoundOpen] = "Failed to open the soundcard";
	m_strErrorMsg[errorComOpen] = "Failed to open the COM port";

	m_clFFT[0] = RGB(0,0,0);		// back color
	m_clFFT[1] = RGB(0,255,0);		// signal color
	m_clFFT[2] = RGB(255,255,255);	// text
	m_clFFT[3] = RGB(128,128,128);	// gage
	m_clFFT[4] = RGB(0,255,255);	// Rx cursro
	m_clFFT[5] = RGB(0,255,0);		// Tx cursro

	m_tWaterLevel[0] = 10;
	m_tWaterLevel[1] = 60;
	m_tWaterLevel[2] = 134;
	m_tWaterLevel[3] = 192;
	m_tWaterLevel[4] = 220;
	m_tWaterLevel[5] = 240;

    m_clWATER[0].c = RGB(0,0,0);		// back
    m_clWATER[1].d = 16711680;			// low
    m_clWATER[2].c = RGB(255,255,255);	// char
    m_clWATER[3].c = RGB(255,0,0);		// WAKU
	m_clWATER[4].c = RGB(0,255,255);	// RX
    m_clWATER[5].c = RGB(0,0,255);		// TX
    m_clWATER[6].d = 16776960;			// mid-low
    m_clWATER[7].d = 65535;				// mid
    m_clWATER[8].d = 33023;				// mid-high
    m_clWATER[9].c = RGB(255,0,0);		// Peak
    m_clWATER[10].c = RGB(255,0,0);		// Peak
    m_clWATER[11].c = RGB(255,0,0);		// Peak

	m_clWAVE[0] = RGB(0,0,0);			// back
	m_clWAVE[1] = RGB(0,255,0);			// signal
	m_clWAVE[2] = RGB(255,255,255);		// text
	m_clWAVE[3] = RGB(128,128,128);		// scale

	m_clFREQERR[0] = RGB(0,0,0);		// back
	m_clFREQERR[1] = RGB(255,255,0);	// signal
	m_clFREQERR[2] = RGB(192,192,192);	// signalTX
	m_clFREQERR[3] = RGB(255,0,0);		// over
	m_clFREQERR[4] = RGB(255,255,255);	// line
	m_nIndex = 0;

	m_wType = m_wPrevType = viewtypeFFT;
	m_bLSB = FALSE;
	m_dwFreqHz = 0;
	m_wPrevBaseFreq = m_wBaseFreq = 0;
	m_wPrevWidthFreq = m_wWidthFreq = 3000;
	m_visible = TRUE;
	m_bFollowRxFreq = TRUE;
	m_bShowSync = TRUE;
	m_wCursorFreq = 0;
	m_bHighQuality = TRUE;
	m_bShowErrorMsg = TRUE;
	m_bAttachMouse = TRUE;
	m_bWindowsMouseEvent = FALSE;
	CreateWaterColors();
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVViewCtrl::~CXMMVViewCtrl - デストラクタ

CXMMVViewCtrl::~CXMMVViewCtrl()
{
	m_bmpScope.DeleteObject();
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVViewCtrl::OnDraw - 描画関数

void CXMMVViewCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if( !m_visible ) return;
	UpdateSize(rcBounds);
	DrawBitmap(pdc, rcBounds);
	DrawWnd(pdc);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVViewCtrl::DoPropExchange - 永続性のサポート

void CXMMVViewCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	PX_Bool(pPX, _T("Visible"), m_visible, TRUE);
	PX_Bool(pPX, _T("bFollowRxFreq"), m_bFollowRxFreq, TRUE);
	PX_Bool(pPX, _T("bLSB"), m_bLSB, FALSE);
	PX_Bool(pPX, _T("bWaterAGC"), m_bWaterAGC, FALSE);
	PX_Bool(pPX, _T("bShowSync"), m_bShowSync, TRUE);
	PX_Bool(pPX, _T("bHighQuality"), m_bHighQuality, TRUE);
	PX_Bool(pPX, _T("bShowErrorMsg"), m_bShowErrorMsg, TRUE);
	PX_Bool(pPX, _T("bAttachMouse"), m_bAttachMouse, TRUE);
	PX_Bool(pPX, _T("bWindowsMouseEvent"), m_bWindowsMouseEvent, FALSE);

	PX_Short(pPX, _T("wType"), m_wType, viewtypeFFT);
	PX_Short(pPX, _T("wBaseFreq"), m_wBaseFreq, 0);
	PX_Short(pPX, _T("wWidthFreq"), m_wWidthFreq, 3000);
	PX_Short(pPX, _T("wWaterNoiseL"), m_wWaterNoiseL, 35);
	PX_Short(pPX, _T("wWaterNoiseH"), m_wWaterNoiseH, 65);
	PX_Short(pPX, _T("wCursorFreq"), m_wCursorFreq, 0);
	PX_Long(pPX, _T("dwFreqHz"), m_dwFreqHz, 0);
	if( pPX->IsLoading() ){
		CreateWaterColors();
		if( m_wWidthFreq < 200 ){
			m_wWidthFreq = 200;
		}
		if( m_wWidthFreq > 3000 ) m_wWidthFreq = 3000;
		if( (m_wBaseFreq + m_wWidthFreq) > 3000 ){
			m_wBaseFreq = 3000 - m_wWidthFreq;
		}
		m_wPrevType = m_wType;
		m_wPrevBaseFreq = m_wBaseFreq;
		m_wPrevWidthFreq = m_wWidthFreq;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVViewCtrl::OnResetState - コントロールのプロパティ値をリセット

void CXMMVViewCtrl::OnResetState()
{
	COleControl::OnResetState();  // DoPropExchange を呼び出してデフォルト値にリセット

	// この位置にコントロールの状態をリセットする処理を追加してください
}

//---------------------------------------------------------------------------
BOOL __fastcall CXMMVViewCtrl::IsTypeFreq(void)
{
	return (m_wType == viewtypeFFT) || (m_wType == viewtypeWATER);
}

//---------------------------------------------------------------------------
BOOL __fastcall CXMMVViewCtrl::IsTypeMouse(void)
{
	return (m_wType == viewtypeFFT) || (m_wType == viewtypeWATER) || (m_wType == viewtypeSMALLWATER);
}

//---------------------------------------------------------------------------
BOOL __fastcall CXMMVViewCtrl::IsTypeWater(void)
{
	return (m_wType == viewtypeWATER) || (m_wType == viewtypeSMALLWATER);
}

//---------------------------------------------------------------------------
void __fastcall CXMMVViewCtrl::InitWater(int sw)
{
	if( !m_pMainCtrl ) return;

	m_WaterSemaphore = m_pMainCtrl->m_WaterSemaphore;
	m_FFTGain = m_pMainCtrl->m_FFT.m_FFTGain;
	if( m_wWaterNoiseL > m_wWaterNoiseH ){
		short f = m_wWaterNoiseL;
        m_wWaterNoiseL = m_wWaterNoiseH;
        m_wWaterNoiseH = f;
	}
	if( m_FFTGain ){
		if( sw & iniwMETRIC ){
	        m_StgWater.Sum = 1000;
   		    m_StgWater.Max = 5120;
       		m_StgWater.VW = 5120;
        }
		if( sw & iniwLIMIT ){
	        m_StgWater.LimitL = int(DBToSqrt(m_wWaterNoiseL*100-500));
    	    m_StgWater.LimitH = int(DBToSqrt(m_wWaterNoiseH*100-500));
        }
    }
    else {
		if( sw & iniwMETRIC ){
	        m_StgWater.Sum = 5000;
   		    m_StgWater.Max = 8000;
       		m_StgWater.VW = 6000;
        }
		if( sw & iniwLIMIT ){
	        m_StgWater.LimitL = m_wWaterNoiseL * 100 - 500;
    	    m_StgWater.LimitH = m_wWaterNoiseH * 100 - 500;
        }
    }
}

//---------------------------------------------------------------------------
void CXMMVViewCtrl::InitialUpdate(void)
{
	if( !m_visible ) return;
	CRect rc;
	GetClientRect(&rc);
	UpdateSize(&rc);
}
//---------------------------------------------------------------------------
void CXMMVViewCtrl::Initialize(void)
{
	if( !m_visible ) return;
	if( !m_bmpSize.cx ) InitialUpdate();
}
//---------------------------------------------------------------------------
void CXMMVViewCtrl::DrawBitmap(CDC *pDC, const CRect &rc)
{
	if( !m_visible ) return;
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp = dc.SelectObject(&m_bmpScope);
	pDC->BitBlt(rc.left, rc.top, m_bmpSize.cx, m_bmpSize.cy, &dc, 0, 0, SRCCOPY);
	dc.SelectObject(pOldBmp);
	dc.DeleteDC();
}
//---------------------------------------------------------------------------
void CXMMVViewCtrl::UpdateSize(const CRect &rc)
{
	if( !m_visible ) return;
	m_rcClient = rc;
	if( (rc.Width() != m_bmpSize.cx) || (rc.Height() != m_bmpSize.cy) ){
		m_bmpSize.cx = rc.Width();
		m_bmpSize.cy = rc.Height();
		m_bmpScope.DeleteObject();
		CDC *pDC = GetDC();
		VERIFY(pDC->GetMapMode() == MM_TEXT);
		m_bmpScope.CreateCompatibleBitmap(pDC, m_bmpSize.cx, m_bmpSize.cy);
		ReleaseDC(pDC);
		switch(m_wType){
		case viewtypeFFT:
			DrawFFT();
			break;
		case viewtypeWATER:
			DrawWater(FALSE);
			break;
		case viewtypeWAVE:
			DrawWaveInternal();
			break;
		case viewtypeFREQERR:
			DrawFreqErrInternal();
			break;
		case viewtypeSMALLWATER:
			DrawSmallWaterInternal(TRUE);
			break;
		}
	}
}
//---------------------------------------------------------------------------
void CXMMVViewCtrl::CreateWaterColors(void)
{
	int i, O, W;
    UCOL c;

    W = m_tWaterLevel[0];
	for( i = 0; i < W; i++ ){
		c = GetGrade2(m_clWATER, i, W);
        m_tWaterColors[i] = c.c;
    }
	UCOL col[2];
	W = m_tWaterLevel[1] - m_tWaterLevel[0];
    O = m_tWaterLevel[0];
    col[0].c = m_clWATER[1].c;
    col[1].c = m_clWATER[6].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
	W = m_tWaterLevel[2] - m_tWaterLevel[1];
    O = m_tWaterLevel[1];
    col[0].c = m_clWATER[6].c;
    col[1].c = m_clWATER[7].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
	W = m_tWaterLevel[3] - m_tWaterLevel[2];
    O = m_tWaterLevel[2];
    col[0].c = m_clWATER[7].c;
    col[1].c = m_clWATER[8].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
	W = m_tWaterLevel[4] - m_tWaterLevel[3];
    O = m_tWaterLevel[3];
    col[0].c = m_clWATER[8].c;
    col[1].c = m_clWATER[9].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
	W = m_tWaterLevel[5] - m_tWaterLevel[4];
    O = m_tWaterLevel[4];
    col[0].c = m_clWATER[9].c;
    col[1].c = m_clWATER[10].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
	W = 256 - m_tWaterLevel[5];
    O = m_tWaterLevel[5];
    col[0].c = m_clWATER[10].c;
    col[1].c = m_clWATER[11].c;
    for( i = 0; i < W; i++ ){
		c = GetGrade2(col, i, W);
        m_tWaterColors[i+O] = c.c;
    }
}
//---------------------------------------------------------------------------
void CXMMVViewCtrl::DrawWnd(CDC *pDC)
{
	if( m_bShowErrorMsg && m_pMainCtrl ){
		if( m_pMainCtrl->m_nErrorCode ){
			if( !m_strErrorMsg[m_pMainCtrl->m_nErrorCode].IsEmpty() ){
				LPCSTR pStr = m_strErrorMsg[m_pMainCtrl->m_nErrorCode];
				CFont *pOldFont = SelectStockFont(pDC);
				::DrawMessage(pDC, m_bmpSize.cx, m_bmpSize.cy, pStr, 3);
				pDC->SelectObject(pOldFont);
			}
		}
		else {
			for( int i = 0; i < 5; i++ ){
				if( m_cInfoMsg[i] ){
					CFont *pOldFont = SelectStockFont(pDC);
					::DrawMessage(pDC, m_bmpSize.cx, m_bmpSize.cy, m_strInfoMsg[i], i);
					pDC->SelectObject(pOldFont);
				}
			}
		}
	}
	if( (m_wCursorFreq > 0) && IsTypeFreq() ){
		CPen pend(PS_DOT, 1, RGB(255,255,0));
		CBrush brushd(RGB(0,0,0));
		CPen *pOldPen = pDC->SelectObject(&pend);
		CBrush *pOldBrush = pDC->SelectObject(&brushd);
		int x = (m_wCursorFreq - m_wBaseFreq) * m_bmpSize.cx / m_wWidthFreq;
		pDC->MoveTo(x, 1);
		pDC->LineTo(x, m_bmpSize.cy);
		CPen pens(PS_SOLID, 1, RGB(255,255,0));
		pDC->SelectObject(&pens);
		char bf[32];
		sprintf(bf, "%uHz", m_wCursorFreq);
		CFont *pOldFont = SelectStockFont(pDC);
		DrawMessage(pDC, m_bmpSize.cx, m_bmpSize.cy, bf, x);
		pDC->SelectObject(pOldFont);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}
	if( m_wType == viewtypeSMALLWATER ){
		DrawSmallWaterCursor(pDC);
	}
}
//---------------------------------------------------------------------------
void CXMMVViewCtrl::DrawDirect(BOOL fScroll)
{
	if( !m_visible ) return;
	if( !m_bmpSize.cx ) return;
	switch(m_wType){
	case viewtypeFFT:
		DrawFFT();
		break;
	case viewtypeWATER:
		DrawWater(fScroll);
		break;
	case viewtypeWAVE:
		DrawWaveInternal();
		break;
	case viewtypeFREQERR:
		DrawFreqErrInternal();
		break;
	case viewtypeSMALLWATER:
		DrawSmallWaterInternal(fScroll);
		break;
	}
	CClientDC dc(this);
	DrawBitmap(&dc, m_rcClient);
	DrawWnd(&dc);
}
//---------------------------------------------------------------------------
int CXMMVViewCtrl::LimitFreq(int fq)
{
	if( fq < MIN_CARRIER ) fq = MIN_CARRIER;
	if( m_pMainCtrl && (fq > m_pMainCtrl->m_MaxCarrier) ) fq = m_pMainCtrl->m_MaxCarrier;
	return fq;
}
//---------------------------------------------------------------------------
void CXMMVViewCtrl::ClearWaterBitmap(void)
{
	if( IsTypeWater() ){
		Initialize();
		CDC dc;
		CDC *pDC = &dc;
		dc.CreateCompatibleDC(NULL);
		CBitmap *pOldBmp = dc.SelectObject(&m_bmpScope);
		dc.FillSolidRect(&m_rcClient, GetRefColor(m_clWATER[0].c));
		dc.SelectObject(pOldBmp);
		dc.DeleteDC();
	}
}
//---------------------------------------------------------------------------
void CXMMVViewCtrl::DrawFFT(void)
{
	if( !m_bmpSize.cx ) return;
	if( !m_bMouseDown ) CalcFFTFreq();

	CDC dc;
	CDC *pDC = &dc;

	pDC->CreateCompatibleDC(NULL);
	CBitmap *pOldBmp = pDC->SelectObject(&m_bmpScope);
	pDC->FillSolidRect(&m_rcClient, GetRefColor(m_clFFT[0]));
	CPen *pOldPen;
	CBrush *pOldBrush;

	RECT rcView;
	rcView.left = rcView.top = 0;
	rcView.right = m_bmpSize.cx; rcView.bottom = m_bmpSize.cy;
	int xx, yy;
	int xw = rcView.right - rcView.left;
	int yw = rcView.bottom - rcView.top;

	if( m_pMainCtrl ){
		if( !m_pMainCtrl->m_FFT.m_FFTGain ){
			CPen penL(PS_DOT, 1, m_clFFT[3]);
			pOldPen = pDC->SelectObject(&penL);
			int y;
			for( y = 6; y <= 86; y += 20 ){
				yy = y * yw / 100;
    			pDC->MoveTo(0, yy); pDC->LineTo(xw, yy);
    		}
			pDC->SelectObject(pOldPen);
			penL.DeleteObject();
		}
	}
	CFont *pOldFont = SelectStockFont(pDC);
	pDC->SetTextColor(m_clFFT[2]);

	CSize fSize = pDC->GetTextExtent("A");
	DrawFreqScale(pDC, xw, yw,  m_wBaseFreq, m_wWidthFreq, fSize.cy, m_clFFT[3], m_clFFT[2]);
	if( m_pMainCtrl ){
		DrawSubChannel(pDC);
		CRxSet *pRxSet = m_pMainCtrl->m_RxSet;
		int rfo = m_pMainCtrl->m_wRxCarrier;
		int tfo = m_pMainCtrl->m_wTxCarrier;
		POINT pt[4];
		if( (tfo != rfo) && !m_pMainCtrl->m_bNET ){
			CPen pen5(PS_SOLID, 1, m_clFFT[5]);
			pOldPen = pDC->SelectObject(&pen5);
			xx = int(0.5 + (tfo - m_wBaseFreq) * xw / m_wWidthFreq);
			pDC->MoveTo(xx, fSize.cy); pDC->LineTo(xx, xw);
			pt[0].x = xx; pt[0].y = fSize.cy;
    		pt[1].x = xx - 3; pt[1].y = fSize.cy+3;
    		pt[2].x = xx; pt[2].y = fSize.cy + 6;
    		pt[3].x = xx + 3; pt[3].y = fSize.cy+3;
			CBrush brush5(m_clFFT[5]);
			pOldBrush = pDC->SelectObject(&brush5);
			pDC->Polygon(pt, 4);
			if( pRxSet->Is170() ){
				double hw = m_pMainCtrl->m_ModFSK.m_RTTYShift * 0.5;
				xx = int(0.5 + (tfo - m_wBaseFreq - hw) * xw / m_wWidthFreq);
				pDC->MoveTo(xx, fSize.cy+5);
    			pDC->LineTo(xx, fSize.cy+3);
				xx = int(0.5 + (tfo - m_wBaseFreq + hw) * xw / m_wWidthFreq);
				pDC->LineTo(xx, fSize.cy+3);
    			pDC->LineTo(xx, fSize.cy+6);
			}
			else if( pRxSet->IsMFSK() ){
				xx = int(0.5 + (tfo - m_wBaseFreq) * xw / m_wWidthFreq);
        		pDC->MoveTo(xx, fSize.cy+5);
        		pDC->LineTo(xx, fSize.cy+3);
				double bw = pRxSet->m_pDem->m_MFSK_BW;
				if( m_pMainCtrl->m_MFSK_Center ){
					int x = int(0.5 + (tfo - m_wBaseFreq - bw*0.5) * xw / m_wWidthFreq);
    	    		pDC->LineTo(x, fSize.cy+3);
        			pDC->LineTo(x, fSize.cy+6);
					x = int(0.5 + (tfo - m_wBaseFreq + bw*0.5) * xw / m_wWidthFreq);
	        		pDC->MoveTo(xx, fSize.cy+3);
    	    		pDC->LineTo(x, fSize.cy+3);
        			pDC->LineTo(x, fSize.cy+6);
				}
				else {
					xx = int(0.5 + (tfo - m_wBaseFreq + (pRxSet->m_Mode == MODE_mfsk_U ? bw : -bw)) * xw / m_wWidthFreq);
    	    		pDC->LineTo(xx, fSize.cy+3);
        			pDC->LineTo(xx, fSize.cy+6);
				}
    		}
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldPen);
			pen5.DeleteObject(); brush5.DeleteObject();
		}
		CPen pen4(PS_SOLID, 1, m_clFFT[4]);
		pOldPen = pDC->SelectObject(&pen4);
		xx = int(0.5 + (rfo - m_wBaseFreq) * xw / m_wWidthFreq);
   		pDC->MoveTo(xx, fSize.cy); pDC->LineTo(xx, xw);
		pt[0].x = xx; pt[0].y = fSize.cy;
		pt[1].x = xx - 3; pt[1].y = fSize.cy+3;
		pt[2].x = xx; pt[2].y = fSize.cy + 6;
		pt[3].x = xx + 3; pt[3].y = fSize.cy+3;
		CBrush brush4(m_clFFT[4]);
		pOldBrush = pDC->SelectObject(&brush4);
		pDC->Polygon(pt, 4);
		if( pRxSet->Is170() ){
			double hw = pRxSet->m_pDem->m_RTTYShift * 0.5;
			xx = int(0.5 + (rfo - m_wBaseFreq - hw) * xw / m_wWidthFreq);
			pDC->MoveTo(xx, fSize.cy+5);
			pDC->LineTo(xx, fSize.cy+3);
			xx = int(0.5 + (rfo - m_wBaseFreq + hw) * xw / m_wWidthFreq);
			pDC->LineTo(xx, fSize.cy+3);
			pDC->LineTo(xx, fSize.cy+6);
		}
		else if( pRxSet->IsMFSK() ){
			xx = int(0.5 + (rfo - m_wBaseFreq) * xw / m_wWidthFreq);
			pDC->MoveTo(xx, fSize.cy+5);
			pDC->LineTo(xx, fSize.cy+3);
			double bw = pRxSet->m_pDem->m_MFSK_BW;
			if( m_pMainCtrl->m_MFSK_Center ){
				int x = int(0.5 + (rfo - m_wBaseFreq - bw*0.5) * xw / m_wWidthFreq);
   	    		pDC->LineTo(x, fSize.cy+3);
       			pDC->LineTo(x, fSize.cy+6);
				x = int(0.5 + (rfo - m_wBaseFreq + bw*0.5) * xw / m_wWidthFreq);
        		pDC->MoveTo(xx, fSize.cy+3);
   	    		pDC->LineTo(x, fSize.cy+3);
       			pDC->LineTo(x, fSize.cy+6);
			}
			else {
				xx = int(0.5 + (rfo - m_wBaseFreq + (pRxSet->m_Mode == MODE_mfsk_U ? bw : -bw)) * xw / m_wWidthFreq);
    			pDC->LineTo(xx, fSize.cy+3);
        		pDC->LineTo(xx, fSize.cy+6);
			}
		}
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		if( m_pMainCtrl->m_bActive && m_pMainCtrl->IsSoundOpen() ){
			CPen pen1(PS_SOLID, 1, m_clFFT[1]);
			pOldPen = pDC->SelectObject(&pen1);

			int x, y;
			if( m_bHighQuality ){
				int xo = int(m_wBaseFreq * m_pMainCtrl->m_FFT_SIZE / m_pMainCtrl->m_FFTSampFreq + 0.5);
				int xe = xo + int(m_wWidthFreq * m_pMainCtrl->m_FFT_SIZE / m_pMainCtrl->m_FFTSampFreq + 0.5);
				double kx = double(xw) / double(xe-xo);
				double k = yw / (FFTSC*100.0);
				int *ip = m_pMainCtrl->m_fftout;
				for( x = xo; x <= xe; x++ ){
        			y = ip[x];
					y = int(yw - (y * k));
					xx = int((x - xo) * kx);
					if( x > xo ){
		    			pDC->LineTo(xx, y);
        			}
            		else {
						pDC->MoveTo(xx, y);
    				}
    			}
			}
			else {
				int xo = int(((m_wBaseFreq+(m_pMainCtrl->m_FFTSampFreq / m_pMainCtrl->m_FFT_SIZE))*m_pMainCtrl->m_FFT_SIZE/m_pMainCtrl->m_FFTSampFreq) + 0.5);
				int ww = int(m_wWidthFreq * m_pMainCtrl->m_FFT_SIZE / m_pMainCtrl->m_FFTSampFreq);
				double k = yw / (FFTSC*100.0);
				int *ip = m_pMainCtrl->m_fftout;
				for( x = 0; x < xw; x++ ){
					xx = xo + (x * ww / xw);
        			y = ip[xx];
					y = int(yw - (y * k));
					if( x ){
		    			pDC->LineTo(x, y);
        			}
            		else {
						pDC->MoveTo(x, y);
    				}
    			}
			}
			if( m_bShowSync && pRxSet->m_pDem->GetSyncState() && pRxSet->m_SQ ){
				pDC->SetTextColor(m_clFFT[2]);
				int bkmode = pDC->SetBkMode(TRANSPARENT);
				pDC->TextOut(1, 2, "SYNC");
				pDC->SetBkMode(bkmode);
    		}
			pDC->SelectObject(pOldPen);
#if 0
			int max = pRxSet->m_pDem->m_Collect[0].GetMax();
			if( max ){
				long *ip = pRxSet->m_pDem->m_Collect[0].GetZP();
				long *qp = pRxSet->m_pDem->m_Collect[1].GetZP();
				for( int i = 0; i < max; i++ ){
					x = *ip++ * 40 / 32768;
					y = *qp++ * 40 / 32768;
					pDC->SetPixelV(x+50, y+50, RGB(0,255,0));
				}
			}
#endif
		}
	}
	if( m_sBorderStyle ){
		RECT rc;
		rc.left = rc.top = 0;
		rc.right = m_bmpSize.cx; rc.bottom = m_bmpSize.cy;
		DrawBorderStyle(pDC, rc);
	}
	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldBmp);
	pDC->DeleteDC();
}

//---------------------------------------------------------------------------
void CXMMVViewCtrl::DrawWater(BOOL fScroll)
{
	if( !m_bmpSize.cx ) return;
	if( !m_bMouseDown ) CalcFFTFreq();

	CDC dc;
	CDC *pDC = &dc;
	pDC->CreateCompatibleDC(NULL);
	CBitmap *pOldBmp = pDC->SelectObject(&m_bmpScope);

	CFont *pOldFont = SelectStockFont(pDC);
	pDC->SetTextColor(m_clWATER[2].c);

	CPen *pOldPen;
	CBrush *pOldBrush;

	RECT rcView;
	rcView.left = rcView.top = 0;
	rcView.right = m_bmpSize.cx; rcView.bottom = m_bmpSize.cy;
	int xw = rcView.right - rcView.left;
	int yw = rcView.bottom - rcView.top;

	CSize fSize = pDC->GetTextExtent("A");
	int fh = fSize.cy;
    int ft = fh + 8;

    int b = m_wBaseFreq;

	RECT rc;
   	rc.left = 0; rc.top = 0; rc.right = xw; rc.bottom = ft;
	pDC->FillSolidRect(&rc, GetRefColor(m_clWATER[0].c));
    rc.bottom = yw;

	DrawFreqScale(pDC, xw, yw,  m_wBaseFreq, m_wWidthFreq, 0, m_clWATER[2].c, m_clWATER[2].c);
	
	if( m_pMainCtrl ){
		DrawSubChannel(pDC);
		CRxSet *pRxSet = m_pMainCtrl->m_RxSet;
		int fo = m_pMainCtrl->m_wRxCarrier;
		int xx;
		POINT pt[3];
		int tfo = m_pMainCtrl->m_wTxCarrier;
		CPen pen3(PS_SOLID, 1, m_clWATER[3].c);
		if( tfo != fo && !m_pMainCtrl->m_bNET ){
			CPen pen5(PS_SOLID, 1, m_clWATER[5].c);
			pOldPen = pDC->SelectObject(&pen5);
			if( pRxSet->Is170() ){
				double hw = m_pMainCtrl->m_ModFSK.m_RTTYShift * 0.5;
				xx = int(0.5 + (tfo - m_wBaseFreq - hw) * xw / m_wWidthFreq);
				pDC->MoveTo(xx, fh+5);
    			pDC->LineTo(xx, fh+3);
				xx = int(0.5 + (tfo - m_wBaseFreq + hw) * xw / m_wWidthFreq);
    			pDC->LineTo(xx, fh+3);
        		pDC->LineTo(xx, fh+6);
			}
			else if( pRxSet->IsMFSK() ){
				xx = int(0.5 + (tfo - m_wBaseFreq) * xw / m_wWidthFreq);
				pDC->MoveTo(xx, fh+5);
    			pDC->LineTo(xx, fh+3);
				double bw = pRxSet->m_pDem->m_MFSK_BW;
				if( m_pMainCtrl->m_MFSK_Center ){
					int x = int(0.5 + (tfo - m_wBaseFreq - bw*0.5) * xw / m_wWidthFreq);
   	    			pDC->LineTo(x, fh+3);
       				pDC->LineTo(x, fh+6);
					x = int(0.5 + (tfo - m_wBaseFreq + bw*0.5) * xw / m_wWidthFreq);
	        		pDC->MoveTo(xx, fh+3);
   	    			pDC->LineTo(x, fh+3);
       				pDC->LineTo(x, fh+6);
				}
				else {
					xx = int(0.5 + (tfo - m_wBaseFreq + (pRxSet->m_Mode == MODE_mfsk_U ? bw : -bw)) * xw / m_wWidthFreq);
					pDC->LineTo(xx, fh+3);
    				pDC->LineTo(xx, fh+6);
				}
			}
			xx = int(0.5 + (tfo - m_wBaseFreq) * xw / m_wWidthFreq);
			pt[0].x = xx - 4; pt[0].y = fh;
			pt[1].x = xx + 4; pt[1].y = fh;
			pt[2].x = xx; pt[2].y = fh + 6;
			pDC->SelectObject(&pen3);
			CBrush brush5(m_clWATER[5].c);
			pOldBrush = pDC->SelectObject(&brush5);
			pDC->Polygon(pt, 3);
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldPen);
		}
		CPen pen4(PS_SOLID, 1, m_clWATER[4].c);
		pOldPen = pDC->SelectObject(&pen4);
		if( pRxSet->Is170() ){
			double hw = pRxSet->m_pDem->m_RTTYShift * 0.5;
			xx = int(0.5 + (fo - m_wBaseFreq - hw) * xw / m_wWidthFreq);
			pDC->MoveTo(xx, fh+5);
			pDC->LineTo(xx, fh+3);
			xx = int(0.5 + (fo - m_wBaseFreq + hw) * xw / m_wWidthFreq);
			pDC->LineTo(xx, fh+3);
			pDC->LineTo(xx, fh+6);
		}
		else if( pRxSet->IsMFSK() ){
			xx = int(0.5 + (fo - m_wBaseFreq) * xw / m_wWidthFreq);
			pDC->MoveTo(xx, fh+5);
			pDC->LineTo(xx, fh+3);
			double bw = pRxSet->m_pDem->m_MFSK_BW;
			if( m_pMainCtrl->m_MFSK_Center ){
				int x = int(0.5 + (fo - m_wBaseFreq - bw*0.5) * xw / m_wWidthFreq);
    			pDC->LineTo(x, fh+3);
   				pDC->LineTo(x, fh+6);
				x = int(0.5 + (fo - m_wBaseFreq + bw*0.5) * xw / m_wWidthFreq);
        		pDC->MoveTo(xx, fh+3);
    			pDC->LineTo(x, fh+3);
   				pDC->LineTo(x, fh+6);
			}
			else {
				xx = int(0.5 + (fo - m_wBaseFreq + (pRxSet->m_Mode == MODE_mfsk_U ? bw : -bw)) * xw / m_wWidthFreq);
    			pDC->LineTo(xx, fh+3);
        		pDC->LineTo(xx, fh+6);
			}
		}
		xx = int(0.5 + (fo - m_wBaseFreq) * xw / m_wWidthFreq);
		pt[0].x = xx - 4; pt[0].y = fh;
		pt[1].x = xx + 4; pt[1].y = fh;
		pt[2].x = xx; pt[2].y = fh + 6;
		pDC->SelectObject(&pen3);
		CBrush brush4(m_clWATER[4].c);
		pOldBrush = pDC->SelectObject(&brush4);
		pDC->Polygon(pt, 3);

		if( m_pMainCtrl->m_bActive && fScroll && m_pMainCtrl->IsSoundOpen() ){
			if( m_pMainCtrl->m_FFT.m_FFTGain != m_FFTGain ) InitWater(iniwBOTH);
			if( m_pMainCtrl->m_WaterSemaphore != m_WaterSemaphore ) InitWater(iniwMETRIC);
			RECT trc = rc;
			rc.top += ft;
			rc.bottom--;
			trc.top += ft + 1;
			int tyw = yw - ft;
			if( m_sBorderStyle ){
				tyw--;
			}
			pDC->StretchBlt(0, trc.top, m_bmpSize.cx, tyw, &dc, 0, rc.top, m_bmpSize.cx, tyw, SRCCOPY);
			FFTSTG *pStg = &m_StgWater;
			int xo = int(((m_wBaseFreq+(m_pMainCtrl->m_FFTSampFreq/m_pMainCtrl->m_FFT_SIZE))*m_pMainCtrl->m_FFT_SIZE/m_pMainCtrl->m_FFTSampFreq) + 0.5);
    		int x, y;
			int n = 0;
			int sum = pStg->LimitL;
			int max = 0;
			int wmax = 0;
			int xl = int(0.5 + (fo - pRxSet->m_Speed - m_wBaseFreq) * xw / m_wWidthFreq);
			int xh = int(0.5 + (fo + pRxSet->m_Speed - m_wBaseFreq) * xw / m_wWidthFreq);
			int ww = int(m_wWidthFreq * m_pMainCtrl->m_FFT_SIZE / m_pMainCtrl->m_FFTSampFreq);
			double k = 256.0 / pStg->VW;
			int *ip = m_pMainCtrl->m_fftout;
    		for( x = 0; x < xw; x++ ){
				xx = xo + (x * ww / xw);
				y = ip[xx];
				if( max < y ) max = y;
				if( (x >= xl) && (x <= xh) ){
					if( wmax < y ) wmax = y;
				}
				if( (y > pStg->LimitL) && (y < pStg->LimitH) ){
					sum += y;
   					n++;
				}
				y = int((y - pStg->Sum) * k);
				if( y < 0 ) y = 0;
				if( y >= 256 ) y = 255;
				pDC->SetPixelV(x, ft, m_tWaterColors[y]);
    		}
			if( m_pMainCtrl->m_TX == txINTERNAL ){
				sum = pStg->LimitL;
				if( pRxSet->IsMFSK() || pRxSet->Is170() ){
					sum = (sum + pStg->LimitH)/2;
				}
			}
			else if( n < 16 ){
				sum = pStg->LimitL;
			}
			else {
        		sum /= n;
			}
			pStg->Sum = (pStg->Sum + sum) / 2;
			if( pStg->Sum >= pStg->LimitH ) pStg->Sum = pStg->LimitH;
			if( (wmax-sum) >= 320 ){
        		max = wmax;
			}
			pStg->Max = (pStg->Max + max) / 2;
			pStg->VW = pStg->Max - pStg->Sum;
			int low, high;
			if( m_bWaterAGC ){		// AGC
				if( m_FFTGain ){
					low = 3000; high = 7000;
				}
				else {
					low = 4000; high = 10000;
				}
			}
			else {
				low = high = m_FFTGain ? 5120 : 5000;
			}
			if( m_pMainCtrl->m_TX == txINTERNAL ) high = 100000;
			if( pStg->VW < low ) pStg->VW = low;
			if( pStg->VW > high ) pStg->VW = high;
		}
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
		if( m_bShowSync && pRxSet->m_pDem->GetSyncState() && pRxSet->m_SQ ){
			pDC->SetTextColor(m_clWATER[2].c);
			int bkmode = pDC->SetBkMode(TRANSPARENT);
			pDC->TextOut(1, 2, "SYNC");
			pDC->SetBkMode(bkmode);
   		}
	}
	if( m_sBorderStyle ){
		RECT rc;
		rc.left = rc.top = 0;
		rc.right = m_bmpSize.cx; rc.bottom = m_bmpSize.cy;
		DrawBorderStyle(pDC, rc);
	}
	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldBmp);
	pDC->DeleteDC();
}

//---------------------------------------------------------------------------
void CXMMVViewCtrl::DrawWaveInternal(void)
{
	if( !m_bmpSize.cx ) return;

	CDC dc;
	CDC *pDC = &dc;
	pDC->CreateCompatibleDC(NULL);
	CBitmap *pOldBmp = pDC->SelectObject(&m_bmpScope);
	pDC->FillSolidRect(&m_rcClient, GetRefColor(m_clWAVE[0]));

	CFont *pOldFont = SelectStockFont(pDC);
	pDC->SetTextColor(m_clWAVE[2]);

	RECT rc;
    rc.left = rc.top = 0;
	rc.right = m_bmpSize.cx; rc.bottom = m_bmpSize.cy;
	int xw = m_bmpSize.cx;
	int yw = m_bmpSize.cy;

	CPen peng(PS_DOT, 1, m_clWAVE[3]);
	CPen *pOldPen = pDC->SelectObject(&peng);
	int O1 = yw / 4;
   	pDC->MoveTo(0, O1); pDC->LineTo(xw, O1);
	int O2 = yw * 3 / 4;
   	pDC->MoveTo(0, O2); pDC->LineTo(xw, O2);

    if( m_pMainCtrl && m_pMainCtrl->m_bActive ){
		double k = yw * 0.3 / 32768.0;
		CPen penl(PS_SOLID, 1, m_clWAVE[1]);
		pDC->SelectObject(&penl);
    	int x, y, xx;
		int W = m_pMainCtrl->m_Collect1.GetMax();
		int M = m_pMainCtrl->m_Collect1.GetCount();
        long *t1 = m_pMainCtrl->m_Collect1.GetZP();
        long *t2 = m_pMainCtrl->m_Collect2.GetZP();
    	for( x = 0; x < xw; x++ ){
			xx = x * W / xw;
            if( xx >= W ) xx = W - 1;
			if( xx >= M ){
				break;
            }
			y = int(O1 - (t1[xx] * k));
			if( x ){ pDC->LineTo(x, y); } else { pDC->MoveTo(x, y); }
    	}
		if( m_pMainCtrl->m_wCollectType == collectWAVE ){
			double d;
			double max = 128;
			for( x = 0; x < M; x++ ){
				d = fabs(t2[x]);
                if( max < d ) max = d;
            }
            k = yw * 0.2 / max;
			for( x = 0; x < M; x++ ){
				xx = x * xw / W;
                y = int(O2 - (t2[x] * k));
				if( x ){ pDC->LineTo(xx, y); } else { pDC->MoveTo(xx, y); }
            }
        }
        else {
	    	for( x = 0; x < xw; x++ ){
				xx = x * W / xw;
        	    if( xx >= W ) xx = W - 1;
				if( xx >= M ){
					break;
    	        }
				y = int(O2 - (t2[xx] * k));
				if( x ){ pDC->LineTo(x, y); } else { pDC->MoveTo(x, y); }
	    	}
		}
	    if( m_bShowSync && m_pMainCtrl->m_RxSet[0].m_pDem->GetSyncState() && m_pMainCtrl->m_RxSet[0].m_SQ ){
			pDC->TextOut(1, 1, "SYNC");
    	}
    }
	if( m_sBorderStyle ){
		RECT rc;
		rc.left = rc.top = 0;
		rc.right = m_bmpSize.cx; rc.bottom = m_bmpSize.cy;
		DrawBorderStyle(pDC, rc);
	}
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldBmp);
	pDC->DeleteDC();
}

//---------------------------------------------------------------------------
void __fastcall CXMMVViewCtrl::DrawFreqScale(CDC *pDC, int XW, int YW,  int fftb, int fftw, int fh, COLORREF LCol, COLORREF FCol)
{
	CPen penL(PS_DOT, 1, LCol);
	CPen penF(PS_SOLID, 1, FCol);
	CPen *pOldPen = pDC->SelectObject(&penL);
	pDC->SetTextColor(FCol);
    int fw;
	char bf[128];
	int A, B, L;
	if( fftw >= 3000 ){
		B = 100; A = 500; L = 500;
	}
	else if( fftw >= 1500 ){
		B = 100; A = 500; L = 500;
	}
	else if( fftw >= 750 ){
		B = 50; A = 100; L = 200;
	}
	else {
       	B = 50; A = 100; L = 100;
	}
    BOOL fLSB = (m_dwFreqHz && m_bLSB);
    UINT f, fb, fu;
    if( m_dwFreqHz ){
		if( fLSB ){
		    fb = m_dwFreqHz - m_wBaseFreq;
    		fu = fb - fftw;
        }
        else {
		    fb = m_dwFreqHz + m_wBaseFreq;
    		fu = fb + fftw;
        }
    }
    else {
		fb = fftb;
    	fu = fftb+fftw;
    }
	int xx;
	f = fb - (fb % A);
  	if( !m_dwFreqHz && (int(f) < 0) ) f = 0;
	int _yt = 2;
	CSize fSize = pDC->GetTextExtent("0", 1);
	int vy = fSize.cy + _yt;
	if( fLSB ){
		for( ; f > fu; f -= B ){
		    xx = int(fb - f) * XW / fftw;
    	    if( xx >= XW ) break;
			if( !(f % 1000) || !(f % L) ){
				sprintf(bf, "%.1lf", double(f) * 0.001);
				fSize = pDC->GetTextExtent(bf);
	            fw = fSize.cx/2;
    	        if( (xx - fw) > 0 ){
					pDC->TextOut(xx - fw, _yt, bf);
	    			if( fh ){
                    	pDC->MoveTo(xx, fh); pDC->LineTo(xx, YW);
                    }
                    else {
                    	pDC->MoveTo(xx, vy); pDC->LineTo(xx, vy+3);
                    }
                }
	        }
            else if( fh && !(f % A) ){
    			pDC->MoveTo(xx, _yt); pDC->LineTo(xx, YW);
            }
            if( !(f % B) ){
				pDC->SelectObject(&penF);
				pDC->MoveTo(xx, 0); pDC->LineTo(xx, 3);
				pDC->SelectObject(&penL);
            }
    	}
	}
    else {
		for( ; f < fu; f += B ){
		    xx = int(f - fb) * XW / fftw;
        	if( xx >= XW ) break;
			if( !(f % 1000) || !(f % L) ){
				if( m_dwFreqHz ){
					sprintf(bf, "%.1lf", double(f) * 0.001);
	            }
    	        else {
					sprintf(bf, "%u", f);
	            }
				fSize = pDC->GetTextExtent(bf);
    	        fw = fSize.cx/2;
        	    if( (xx - fw) > 0 ){
                	pDC->TextOut(xx - fw, _yt, bf);
	    			if( fh ){
                    	pDC->MoveTo(xx, fh); pDC->LineTo(xx, YW);
                    }
                    else {
                    	pDC->MoveTo(xx, vy); pDC->LineTo(xx, vy+3);
                    }
                }
    	    }
            else if( fh && !(f % A) ){
    			pDC->MoveTo(xx, _yt); pDC->LineTo(xx, YW);
            }
            if( !(f % B) ){
				pDC->SelectObject(&penF);
				pDC->MoveTo(xx, 0); pDC->LineTo(xx, 3);
				pDC->SelectObject(&penL);
            }
	    }
    }
	pDC->SelectObject(pOldPen);
	penL.DeleteObject();
	penF.DeleteObject();
}

//---------------------------------------------------------------------------
void __fastcall CXMMVViewCtrl::DrawSubChannel(CDC *pDC)
{
	CSize fSize = pDC->GetTextExtent("2");
    int fw = fSize.cx;
	int fh = fSize.cy;
	int ffw = fw - 2;
    int ffh = fh - 3;
	POINT pt[6];
	char bf[256];
	CBrush brushw(RGB(255,255,255));
	CPen penr(PS_SOLID, 1, RGB(255,0,0));
	CBrush *pOldBrush = pDC->SelectObject(&brushw);
	CPen *pOldPen = pDC->SelectObject(&penr);
	pDC->SetTextColor(RGB(0,0,0));
	int bkmode = pDC->SetBkMode(TRANSPARENT);
	CRxSet *pRxSet = &m_pMainCtrl->m_RxSet[1];
	for( int i = 1; i < RXMAX; i++, pRxSet++ ){
		if( pRxSet->IsActive() ){
			int fo = pRxSet->m_CarrierFreq;
			int xx = int(0.5 + (fo - m_wBaseFreq) * m_bmpSize.cx / m_wWidthFreq);
			pt[0].x = xx - ffw; pt[0].y = 0;
            pt[1].x = xx + ffw; pt[1].y = 0;
            pt[2].x = xx + ffw; pt[2].y = ffh;
            pt[3].x = xx; pt[3].y = ffh + 3;
            pt[4].x = xx - ffw; pt[4].y = ffh;
            pt[5].x = xx - ffw; pt[5].y = 0;
           	pDC->Polygon(pt, 6);
            m_SX[i] = xx;
            m_SY[i] = fh/2;
			sprintf(bf, "%u", i);
			fSize = pDC->GetTextExtent(bf);
			xx -= fSize.cx/2;
			pDC->TextOut(xx, 0, bf);
        }
    }
	pDC->SelectObject(pOldBrush);
    if( m_pMainCtrl->m_Notches.m_Count ){
		CBrush brushy(RGB(255,255,0));
		pOldBrush = pDC->SelectObject(&brushy);
		for( i = 0; i < m_pMainCtrl->m_Notches.m_Count; i++ ){
			int fo = m_pMainCtrl->m_Notches.m_pBase[i].Freq;
			int xx = int(0.5 + (fo - m_wBaseFreq) * m_bmpSize.cx / m_wWidthFreq);
			pt[0].x = xx - ffw; pt[0].y = 0;
			pt[1].x = xx + ffw; pt[1].y = 0;
			pt[2].x = xx + ffw; pt[2].y = ffh;
			pt[3].x = xx; pt[3].y = ffh + 3;
			pt[4].x = xx - ffw; pt[4].y = ffh;
			pt[5].x = xx - ffw; pt[5].y = 0;
      		pDC->Polygon(pt, 6);
			if( i < NOTCHMAX ){
				m_NotchX[i] = xx;
				m_NotchY[i] = fh/2;
			}
			strcpy(bf, "N");
			fSize = pDC->GetTextExtent(bf);
			xx -= fSize.cx/2;
			pDC->TextOut(xx, 0, bf);
		}
		pDC->SelectObject(pOldBrush);
    }
	pDC->SetBkMode(bkmode);
	pDC->SelectObject(pOldPen);
}

//---------------------------------------------------------------------------
void __fastcall CXMMVViewCtrl::CalcFFTCenter(int fo)
{
	if( m_wWidthFreq == 3000 ){
		m_wBaseFreq = 0;
    }
    int fb = fo - m_wWidthFreq/2;
    if( fb < 0 ) fb = 0;
	fb /= 100;
    fb *= 100;
	if( m_wWidthFreq == 3000 ){
		fb = 0;
    }
    else if( (fb + m_wWidthFreq) >= 3000 ){
		fb = 3000 - m_wWidthFreq;
    }
    m_wBaseFreq = fb;
}
//---------------------------------------------------------------------------
void __fastcall CXMMVViewCtrl::CalcFFTFreq(void)
{
	if( !m_bFollowRxFreq ) return;
	if( !m_pMainCtrl ) return;
	CRxSet *pRxSet = m_pMainCtrl->m_RxSet;
	int fo = pRxSet->m_CarrierFreq;
	int edge = 100;
    if( pRxSet->IsMFSK() ){
        edge = 200;
		if( !m_pMainCtrl->m_MFSK_Center ){
			int off = int(pRxSet->m_pDem->m_MFSK_BW/2);
			if( pRxSet->m_Mode == MODE_mfsk_L ){
        	   	fo -= off;
	        }
    	    else {
				fo += off;
	        }
        }
    }
    else if( pRxSet->IsRTTY() ){
		edge = 200;
    }
	if( m_wWidthFreq == 3000 ){
		m_wBaseFreq = 0;
    }
	if( (m_wBaseFreq >= 0) && ((m_wBaseFreq+m_wWidthFreq) <= 3000 ) ){
	    if( (fo > m_wBaseFreq+edge) && (fo < (m_wBaseFreq+m_wWidthFreq)-edge) ){
			return;
    	}
    }
	CalcFFTCenter(fo);
}

//---------------------------------------------------------------------------
void CXMMVViewCtrl::DrawFreqErrInternal(void)
{
	if( !m_bmpSize.cx ) return;

	CDC dc;
	CDC *pDC = &dc;
	dc.CreateCompatibleDC(NULL);
	CBitmap *pOldBmp = dc.SelectObject(&m_bmpScope);
	dc.FillSolidRect(&m_rcClient, GetRefColor(m_clFREQERR[0]));
	CPen *pOldPen;

	RECT rc;
    rc.left = rc.top = 0;
	rc.right = m_bmpSize.cx; rc.bottom = m_bmpSize.cy;
	CPen penc(PS_SOLID, 1, m_clFREQERR[4]);
	pOldPen = pDC->SelectObject(&penc);
	if( m_bmpSize.cx > m_bmpSize.cy ){		// 横長
		int xc = m_bmpSize.cx / 2;
		if( m_pMainCtrl && m_pMainCtrl->m_bActive ){
			CRxSet *pRxSet = &m_pMainCtrl->m_RxSet[m_nIndex];
			if( pRxSet->IsActive() ){
				double d = m_pMainCtrl->GetDblFreqErrorInternal(m_nIndex);
				if( d > 0.0 ){
					rc.left = int(xc + (d * m_bmpSize.cx));
    				rc.right = xc;
	    		}
	    		else {
					rc.left = xc;
    				rc.right = int(xc + (d * m_bmpSize.cx));
				}
				COLORREF col;
				if( pRxSet->m_fAFC || (m_pMainCtrl->m_TX == txINTERNAL) ){
					col = m_clFREQERR[2];
				}
    			else {
	    			col = pRxSet->IsFreqErr(d) ? m_clFREQERR[3] : m_clFREQERR[1];
				}
				pDC->FillSolidRect(&rc, col);
			}
		}
		pDC->MoveTo(xc, 0);
		pDC->LineTo(xc, m_bmpSize.cy);
	}
	else {									// 縦長
		int yc = m_bmpSize.cy / 2;
		if( m_pMainCtrl && m_pMainCtrl->m_bActive ){
			CRxSet *pRxSet = &m_pMainCtrl->m_RxSet[m_nIndex];
			if( pRxSet->IsActive() ){
				double d = m_pMainCtrl->GetDblFreqErrorInternal(m_nIndex);
				if( d > 0.0 ){
					rc.top = int(yc + (d * m_bmpSize.cy));
    				rc.bottom = yc;
	    		}
	    		else {
					rc.top = yc;
    				rc.bottom = int(yc + (d * m_bmpSize.cy));
				}
				COLORREF col;
				if( pRxSet->m_fAFC || (m_pMainCtrl->m_TX == txINTERNAL) ){
					col = m_clFREQERR[2];
				}
    			else {
	    			col = pRxSet->IsFreqErr(d) ? m_clFREQERR[3] : m_clFREQERR[1];
				}
				pDC->FillSolidRect(&rc, col);
			}
		}
		pDC->MoveTo(0, yc);
		pDC->LineTo(m_bmpSize.cy, yc);
	}
	pDC->SelectObject(pOldPen);
	if( m_sBorderStyle ){
		RECT rc;
		rc.left = rc.top = 0;
		rc.right = m_bmpSize.cx; rc.bottom = m_bmpSize.cy;
		DrawBorderStyle(&dc, rc);
	}
	pDC->SelectObject(pOldBmp);
	pDC->DeleteDC();
}

//---------------------------------------------------------------------------
void CXMMVViewCtrl::DrawSmallWaterInternal(BOOL fScroll)
{
	if( !m_bmpSize.cx ) return;

	CDC dc;
	CDC *pDC = &dc;
	dc.CreateCompatibleDC(NULL);
	CBitmap *pOldBmp = dc.SelectObject(&m_bmpScope);

	CRxSet *pRxSet = NULL;
	if( m_pMainCtrl && m_pMainCtrl->m_bActive ){
		pRxSet = &m_pMainCtrl->m_RxSet[m_nIndex];
		if( !pRxSet->IsActive() ) pRxSet = NULL;
	}
	if( pRxSet != NULL ){
		int fo = pRxSet->m_CarrierFreq;
		if( !m_bMouseDown ){
			int ww = m_wWidthFreq / ((pRxSet->Is170()||pRxSet->IsMFSK()) ? 4 : 8);
			int cfo = fo;
			if( pRxSet->IsMFSK() ){
				int off = int(pRxSet->m_pDem->m_MFSK_BW/2);
				if( pRxSet->m_Mode == MODE_mfsk_L ){
					cfo -= off;
				}
				else {
					cfo += off;
				}
			}
			if( (cfo < (m_wBaseFreq+ww)) || (cfo > (m_wBaseFreq+m_wWidthFreq-ww)) ){
				CalcSmallWaterCenter(pRxSet);
			}
		}
		if( fScroll ){
			if( m_pMainCtrl->m_FFT.m_FFTGain != m_FFTGain ) InitWater(iniwBOTH);
			if( m_pMainCtrl->m_WaterSemaphore != m_WaterSemaphore ) InitWater(iniwMETRIC);
			RECT rc;
			rc.left = 0; rc.top = 0; rc.right = m_bmpSize.cx; rc.bottom = m_bmpSize.cy;

			int ft = 0;
			if( m_sBorderStyle ) ft++;
			RECT trc = rc;
			rc.top += ft;
			rc.bottom--;
			trc.top += rc.top + 1;
			int tyw = m_bmpSize.cy - ft;
			if( m_sBorderStyle ){
				tyw--;
			}
			pDC->StretchBlt(0, trc.top, m_bmpSize.cx, tyw, &dc, 0, rc.top, m_bmpSize.cx, tyw, SRCCOPY);

			FFTSTG *pStg = &m_StgWater;
			int xo = int(((m_wBaseFreq+(m_pMainCtrl->m_FFTSampFreq/m_pMainCtrl->m_FFT_SIZE))*m_pMainCtrl->m_FFT_SIZE/m_pMainCtrl->m_FFTSampFreq) + 0.5);
    		int x, y, xx;
			int n = 0;
			int sum = pStg->LimitL;
			int max = 0;
			int wmax = 0;
			int xl = int(0.5 + (fo - pRxSet->m_Speed - m_wBaseFreq) * m_bmpSize.cx / m_wWidthFreq);
			int xh = int(0.5 + (fo + pRxSet->m_Speed - m_wBaseFreq) * m_bmpSize.cx / m_wWidthFreq);
			int ww = int(m_wWidthFreq * m_pMainCtrl->m_FFT_SIZE / m_pMainCtrl->m_FFTSampFreq);
			double k = 256.0 / pStg->VW;
			int *ip = m_pMainCtrl->m_fftout;
    		for( x = 0; x < m_bmpSize.cx; x++ ){
				xx = xo + (x * ww / m_bmpSize.cx);
				y = ip[xx];
				if( max < y ) max = y;
				if( (x >= xl) && (x <= xh) ){
					if( wmax < y ) wmax = y;
				}
				if( (y > pStg->LimitL) && (y < pStg->LimitH) ){
					sum += y;
   					n++;
				}
				y = int((y - pStg->Sum) * k);
				if( y < 0 ) y = 0;
				if( y >= 256 ) y = 255;
				pDC->SetPixelV(x, ft, m_tWaterColors[y]);
    		}
			if( m_pMainCtrl->m_TX == txINTERNAL ){
				sum = pStg->LimitL;
				if( pRxSet->IsMFSK() || pRxSet->Is170() ){
					sum = (sum + pStg->LimitH)/2;
				}
			}
			else if( n < 16 ){
				sum = pStg->LimitL;
			}
			else {
        		sum /= n;
			}
			pStg->Sum = (pStg->Sum + sum) / 2;
			if( pStg->Sum >= pStg->LimitH ) pStg->Sum = pStg->LimitH;
			if( (wmax-sum) >= 320 ){
        		max = wmax;
			}
			pStg->Max = (pStg->Max + max) / 2;
			pStg->VW = pStg->Max - pStg->Sum;
			int low, high;
			if( m_bWaterAGC ){		// AGC
				if( m_FFTGain ){
					low = 3000; high = 7000;
				}
				else {
					low = 4000; high = 10000;
				}
			}
			else {
				low = high = m_FFTGain ? 5120 : 5000;
			}
			if( m_pMainCtrl->m_TX == txINTERNAL ) high = 100000;
			if( pStg->VW < low ) pStg->VW = low;
			if( pStg->VW > high ) pStg->VW = high;

		}
	}
	if( m_sBorderStyle ){
		RECT rc;
		rc.left = rc.top = 0;
		rc.right = m_bmpSize.cx; rc.bottom = m_bmpSize.cy;
		DrawBorderStyle(&dc, rc);
	}
	pDC->SelectObject(pOldBmp);
	pDC->DeleteDC();
}
//---------------------------------------------------------------------------
void CXMMVViewCtrl::CalcSmallWaterCenter(CRxSet *pRxSet)
{
	if( !pRxSet ) return;
	int fo = pRxSet->m_CarrierFreq;
    int fw = int( (m_wWidthFreq - pRxSet->m_pDem->m_MFSK_BW)/2 );
	if( fw < 30 ) fw = 30;
	if( !m_pMainCtrl->m_MFSK_Center && pRxSet->IsMFSK() ){
		if( pRxSet->m_Mode == MODE_mfsk_U ){
       	    m_wBaseFreq = fo - fw;
        }
        else {
           	m_wBaseFreq = (fo + fw) - m_wWidthFreq;
        }
    }
    else {
	    m_wBaseFreq = fo - m_wWidthFreq/2;
    }
	if( m_wBaseFreq < 0 ){
		m_wBaseFreq = 0;
    }
    else if( (m_wBaseFreq + m_wWidthFreq) > 3000 ){
        m_wBaseFreq = 3000 - m_wWidthFreq;
	}
}

//---------------------------------------------------------------------------
void CXMMVViewCtrl::DrawSmallWaterCursor(CDC *pDC)
{
	if( !m_bmpSize.cx ) return;

	if( !m_pMainCtrl || !m_pMainCtrl->m_bActive ) return;
	CRxSet *pRxSet = &m_pMainCtrl->m_RxSet[m_nIndex];
	if( !pRxSet->IsActive() ) return;
	
    int fo = pRxSet->m_CarrierFreq;
	int xx = int(0.5 + (fo - m_wBaseFreq) * m_bmpSize.cx / m_wWidthFreq);
	if( xx < 4 ) return;
    if( xx > m_bmpSize.cx - 4 ) return;
	int fb = m_bmpSize.cy - 1;
	if( m_sBorderStyle ) fb--;
	POINT pt[3];
    pt[0].x = xx - 4; pt[0].y = fb;
    pt[1].x = xx + 4; pt[1].y = fb;
    pt[2].x = xx; pt[2].y = fb - 6;
	CPen pen(PS_SOLID, 1, m_clWATER[3].c);
	CBrush brush(m_clWATER[4].c);
	CPen *pOldPen = pDC->SelectObject(&pen);
	CBrush *pOldBrush = pDC->SelectObject(&brush);
	pDC->Polygon(pt, 3);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

#if 0
void CXMMVViewCtrl::DrawCloseAdjustInternal(void)
{
	if( !m_bmpSize.cx ) return;

	CDC dc;
	CDC *pDC = &dc;
	dc.CreateCompatibleDC(NULL);
	CBitmap *pOldBmp = dc.SelectObject(&m_bmpScope);

	if( m_pMainCtrl && m_pMainCtrl->m_Clock.IsActive() ){
		RECT rc;
		rc.left = 0; rc.top = 0; rc.right = m_bmpSize.cx; rc.bottom = m_bmpSize.cy;

		int ft = 0;
		if( m_sBorderStyle ) ft++;
		RECT trc = rc;
		rc.top += ft;
		rc.bottom--;
		trc.top += rc.top + 1;
		int tyw = m_bmpSize.cy - ft;
		if( m_sBorderStyle ){
			tyw--;
		}
		pDC->StretchBlt(0, trc.top, m_bmpSize.cx, tyw, &dc, 0, rc.top, m_bmpSize.cx, tyw, SRCCOPY);

		CClock *pClock = &m_Clock;
		int sum = 0;
		int max = 0;
		int mp;
		int d;
		for( int i = 0; (i < m_bmpSize.cx); i++ ){
			d = pClock->GetData(i);
			if( max < d ){
            	max = d;
				mp = i;
			}
			sum += d;

			d = (d - m_StgT.Sum) * 256 / m_StgT.VW;
			if( d >= 256 ) d = 255;
			if( d < 0 ) d = 0;
			pDC->SetPixelV(i, fh, m_tWaterColors[d].c);
		}
		pDC->SetPixelV(mp, fh, RGB(255,0,0));

		sum /= src.right;
		m_StgT.Sum = (m_StgT.Sum + sum) / 2;
		m_StgT.Max = (m_StgT.Max + max) / 2;
		m_StgT.VW = m_StgT.Max - m_StgT.Sum;
		if( m_StgT.VW < 128 ) m_StgT.VW = 128;
		m_PointY++;
		m_PointRX++;
		if( m_PointRX >= m_bmpSize.cy ) m_PointRX = m_bmpSize.cy;
	}
}
#endif
/////////////////////////////////////////////////////////////////////////////
// CXMMVViewCtrl メッセージ ハンドラ

int CXMMVViewCtrl::GetMouseFreq(int x)
{
	int fq = int(m_wBaseFreq + (x * m_wWidthFreq / m_bmpSize.cx)+0.5);
	return LimitFreq(fq);
}

void CXMMVViewCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_pMainCtrl ){
		m_bMouseDown = TRUE;
		if( IsTypeMouse() ){
			int fq = GetMouseFreq(point.x);
			if( m_bAttachMouse ){
				if( !m_MouseNotch && !m_MouseSubChannel ){
					int nIndex = m_MouseSubChannel;
					if( m_wType == viewtypeSMALLWATER ) nIndex = m_nIndex;
					CRxSet *pRxSet = &m_pMainCtrl->m_RxSet[nIndex];
					pRxSet->m_pDem->m_Decode.Reset();
					pRxSet->m_pDem->ResetMeasMFSK();
					pRxSet->m_PeakSig = 0;
					pRxSet->m_AFCTimerMFSK = m_wWidthFreq < 1500 ? MFSKAFC_MAX : 0;
					if( m_wWidthFreq >= 750 ){
						fq = m_pMainCtrl->GetSignalFreq(fq, m_wWidthFreq >= 2500 ? 50 : 32, pRxSet);
					}
					m_pMainCtrl->SetRxCarrier(nIndex, fq);
				}
			}
			SetCapture();
			FireOnLMouseDown(fq);
			if( m_bAttachMouse && (m_MouseNotch || m_MouseSubChannel) ) SetCursor(m_StdCursor[CF_SIZEWE]);
		}
	}
	if( m_bWindowsMouseEvent ) FireOnWindowsLMouseDown(short(point.x), short(point.y));
//	COleControl::OnLButtonDown(nFlags, point);
}

void CXMMVViewCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bMouseDown = FALSE;
	if( GetCapture() == this ){
		ReleaseCapture();
		if( m_pMainCtrl && IsTypeFreq() ){
			int fq = GetMouseFreq(point.x);
			if( m_bAttachMouse ){
				if( m_MouseNotch ){
					int n = m_MouseNotch - 1;
					if( !n ) m_pMainCtrl->m_NotchFreq = fq;
					m_pMainCtrl->m_Notches.SetFreq(n, fq);
				}
				else {
					int nIndex = m_MouseSubChannel;
					if( m_wType == viewtypeSMALLWATER ) nIndex = m_nIndex;
					CRxSet *pRxSet = &m_pMainCtrl->m_RxSet[nIndex];
					pRxSet->m_pDem->m_Decode.Reset();
					pRxSet->m_pDem->ResetMeasMFSK();
					pRxSet->m_PeakSig = 0;
					pRxSet->m_AFCTimerMFSK = m_wWidthFreq < 1500 ? MFSKAFC_MAX : 0;
					if( m_wWidthFreq >= 750 ){
						fq = m_pMainCtrl->GetSignalFreq(fq, m_wWidthFreq >= 2500 ? 50 : 32, pRxSet);
					}
					m_pMainCtrl->SetRxCarrier(nIndex, fq);
				}
			}
			FireOnLMouseUp(fq);
		}
	}
	if( m_bWindowsMouseEvent ) FireOnWindowsLMouseUp(short(point.x), short(point.y));
//	COleControl::OnLButtonUp(nFlags, point);
}

void CXMMVViewCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( (GetCapture() == this) && IsTypeMouse() ){
		int fq = GetMouseFreq(point.x);
		if( nFlags & MK_LBUTTON ){
			if( m_pMainCtrl && m_bAttachMouse ){
				if( m_MouseNotch ){
					int n = m_MouseNotch - 1;
					if( !n ) m_pMainCtrl->m_NotchFreq = fq;
					m_pMainCtrl->m_Notches.SetFreq(n, fq);
				}
				else if( m_wType == viewtypeSMALLWATER ){
					m_pMainCtrl->SetRxCarrier(m_nIndex, fq);
				}
				else {
					m_pMainCtrl->SetRxCarrier(m_MouseSubChannel, fq);
				}
				DrawDirect();
				if( m_MouseNotch || m_MouseSubChannel ) SetCursor(m_StdCursor[CF_SIZEWE]);
			}
			FireOnLMouseMove(fq);
		}
		else if( nFlags & MK_RBUTTON ){
			FireOnRMouseMove(fq);
		}
	}
	else if( m_pMainCtrl && IsTypeFreq() ){
		int i, d, xx, yy;
		m_MouseSubChannel = 0;
		m_MouseNotch = 0;
		if( m_pMainCtrl->m_Notches.m_Count ){
			for( i = 0; (i < m_pMainCtrl->m_Notches.m_Count) && (i < NOTCHMAX); i++ ){
				xx = point.x - m_NotchX[i];
    			yy = point.y - m_NotchY[i];
        		d = (xx * xx + yy * yy);
				if( d < 64 ){
					m_MouseNotch = i + 1;
        		}
			}
        }
		if( !m_MouseNotch ){
			CRxSet *pRxSet = &m_pMainCtrl->m_RxSet[1];
			for( i = 1; i < RXMAX; i++, pRxSet++ ){
				if( pRxSet->IsActive() ){
					xx = point.x - m_SX[i];
					yy = point.y - m_SY[i];
					d = (xx * xx + yy * yy);
    				if( d < 64 ){
						m_MouseSubChannel = i;
            			break;
					}
				}
			}
		}
		if( m_bAttachMouse ){
			SetCursor(m_StdCursor[(m_MouseSubChannel||m_MouseNotch) ? CF_SIZEWE : CF_ARROW]);
		}
	}
	if( m_bWindowsMouseEvent ) FireOnWindowsMouseMove(nFlags, short(point.x), short(point.y));
//	COleControl::OnMouseMove(nFlags, point);
}

void CXMMVViewCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if( m_bWindowsMouseEvent ) FireOnWindowsLMouseDblClk(short(point.x), short(point.y));
//	COleControl::OnLButtonDblClk(nFlags, point);
}

void CXMMVViewCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( nFlags == MK_RBUTTON ){
		if( IsTypeMouse() ){
			SetCapture();
			int fq = GetMouseFreq(point.x);
			FireOnRMouseDown(fq);
		}
	}
	if( m_bWindowsMouseEvent ) FireOnWindowsRMouseDown(short(point.x), short(point.y));
//	COleControl::OnRButtonDown(nFlags, point);
}

void CXMMVViewCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if( GetCapture() == this ){
		ReleaseCapture();
		if( IsTypeMouse() ){
			int fq = GetMouseFreq(point.x);
			FireOnRMouseUp(fq);
		}
	}
	if( m_bWindowsMouseEvent ) FireOnWindowsRMouseUp(short(point.x), short(point.y));
//	COleControl::OnRButtonUp(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// CXMMVViewCtrl プロパティ・メソッド

void CXMMVViewCtrl::OnDwFreqHzChanged() 
{
	if( m_bmpSize.cx && IsTypeFreq() && !m_pMainCtrl ) DrawDirect();
	SetModifiedFlag();
}

void CXMMVViewCtrl::OnBLSBChanged() 
{
	if( m_bmpSize.cx && IsTypeFreq() && m_dwFreqHz && !m_pMainCtrl ) DrawDirect();
	SetModifiedFlag();
}

void CXMMVViewCtrl::OnWBaseFreqChanged() 
{
	if( (m_wBaseFreq + m_wWidthFreq) > 3000 ) m_wBaseFreq = 3000 - m_wWidthFreq;
	if( m_wBaseFreq < 0 ) m_wBaseFreq = 0;
	if( m_bmpSize.cx && (m_wPrevBaseFreq != m_wBaseFreq) ){
		ClearWaterBitmap();
		DrawDirect();
	}
	m_wPrevBaseFreq = m_wBaseFreq;
	SetModifiedFlag();
}

void CXMMVViewCtrl::OnWWidthFreqChanged() 
{
	if( m_wWidthFreq < 200 ) m_wWidthFreq = 200;
	if( m_wWidthFreq > 3000 ) m_wWidthFreq = 3000;
	if( (m_wBaseFreq + m_wWidthFreq) > 3000 ) m_wBaseFreq = 3000 - m_wWidthFreq;
	if( m_wBaseFreq < 0 ) m_wBaseFreq = 0;
	if( m_bmpSize.cx && (m_wPrevWidthFreq != m_wWidthFreq) ){
		ClearWaterBitmap();
		DrawDirect();
	}
	m_wPrevWidthFreq = m_wWidthFreq;
	SetModifiedFlag();
}

void CXMMVViewCtrl::OnFontChanged() 
{
	COleControl::OnFontChanged();
	if( m_bmpSize.cx ){
		ClearWaterBitmap();
		DrawDirect();
	}
}

void CXMMVViewCtrl::Draw(long dwHandle) 
{
	if( dwHandle && IsTypeFreq() ){
		BOOL f = m_pMainCtrl != NULL;
		m_pMainCtrl = (CMMVARICtrl *)dwHandle;
		if( !f ) InitWater(iniwBOTH);
		DrawDirect(TRUE);
	}
}

void CXMMVViewCtrl::OnBWaterAGCChanged() 
{
	SetModifiedFlag();
}

void CXMMVViewCtrl::OnVisibleChanged() 
{
	ShowWindow(m_visible ? SW_SHOW : SW_HIDE);
	SetModifiedFlag();
}

void CXMMVViewCtrl::DrawWave(long dwHandle) 
{
	if( dwHandle && (m_wType == viewtypeWAVE) ){
		BOOL f = m_pMainCtrl != NULL;
		m_pMainCtrl = (CMMVARICtrl *)dwHandle;
		if( !f ) InitWater(iniwBOTH);
		DrawDirect();
	}
}

void CXMMVViewCtrl::OnBFollowRxFreqChanged() 
{
	// TODO: ﾊﾝﾄﾞﾗ ｺｰﾄﾞを追加してください

	SetModifiedFlag();
}

OLE_COLOR CXMMVViewCtrl::GetColorFFT(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= AN(m_clFFT)) ) return RGB(0,0,0);
	return m_clFFT[nIndex];
}

void CXMMVViewCtrl::SetColorFFT(short nIndex, OLE_COLOR nNewValue) 
{
	if( (nIndex < 0) || (nIndex >= AN(m_clFFT)) ) return;
	m_clFFT[nIndex] = nNewValue;
	if( m_wType == viewtypeFFT ) DrawDirect();
	SetModifiedFlag();
}

OLE_COLOR CXMMVViewCtrl::GetColorWater(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= AN(m_clWATER)) ) return RGB(0,0,0);
	return m_clWATER[nIndex].c;
}

void CXMMVViewCtrl::SetColorWater(short nIndex, OLE_COLOR nNewValue) 
{
	if( (nIndex < 0) || (nIndex >= AN(m_clWATER)) ) return;
	m_clWATER[nIndex].c = nNewValue;
	CreateWaterColors();
	if( IsTypeWater() && m_bmpSize.cx ) DrawDirect();
	SetModifiedFlag();
}

OLE_COLOR CXMMVViewCtrl::GetColorWave(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= AN(m_clWAVE)) ) return RGB(0,0,0);
	return m_clWAVE[nIndex];
}

void CXMMVViewCtrl::SetColorWave(short nIndex, OLE_COLOR nNewValue) 
{
	if( (nIndex < 0) || (nIndex >= AN(m_clWAVE)) ) return;
	m_clWAVE[nIndex] = nNewValue;
	if( (m_wType == viewtypeWAVE) && m_bmpSize.cx ) DrawDirect();
	SetModifiedFlag();
}

short CXMMVViewCtrl::GetWWaterLevel(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= AN(m_tWaterLevel)) ) return 0;
	return m_tWaterLevel[nIndex];
}

void CXMMVViewCtrl::SetWWaterLevel(short nIndex, short nNewValue) 
{
	if( (nIndex < 0) || (nIndex >= AN(m_tWaterLevel)) ) return;
	m_tWaterLevel[nIndex] = nNewValue;
	CreateWaterColors();
	SetModifiedFlag();
}


void CXMMVViewCtrl::OnWTypeChanged() 
{
	if( m_wType < 0 ) m_wType = 0;
	if( m_wType >= viewtypeEND ) m_wType = viewtypeEND - 1;
	if( IsTypeFreq() && (m_wPrevType == viewtypeSMALLWATER) ){
		if( m_wWidthFreq == 400 ){
			m_wWidthFreq = 3000;
			m_wBaseFreq = 0;
		}
	}
	else if( m_wType == viewtypeSMALLWATER ){
		m_wWidthFreq = 400;
	}
	m_wPrevType = m_wType;
	if( m_bmpSize.cx ){
		ClearWaterBitmap();
		DrawDirect();
	}
	SetModifiedFlag();
}

OLE_COLOR CXMMVViewCtrl::GetWaterPalette(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= AN(m_tWaterColors)) ) return RGB(0,0,0);
	return m_tWaterColors[nIndex];
}

void CXMMVViewCtrl::SetWaterPalette(short nIndex, OLE_COLOR nNewValue) 
{
	if( (nIndex < 0) || (nIndex >= AN(m_tWaterColors)) ) return;
	m_tWaterColors[nIndex] = nNewValue;
	SetModifiedFlag();
}

void CXMMVViewCtrl::OnWWaterNoiseLChanged() 
{
	// TODO: ﾊﾝﾄﾞﾗ ｺｰﾄﾞを追加してください

	SetModifiedFlag();
}

void CXMMVViewCtrl::OnWWaterNoiseHChanged() 
{
	// TODO: ﾊﾝﾄﾞﾗ ｺｰﾄﾞを追加してください

	SetModifiedFlag();
}

short CXMMVViewCtrl::GetWWaterNoise() 
{
	if( m_pMainCtrl ){
		double db = m_StgWater.Sum;
		if( m_FFTGain ) db = SqrtToDB(db);
		return short((db * 0.01) + 0.5);
	}
	else {
		return 0;
	}
}

void CXMMVViewCtrl::OnBShowSyncChanged() 
{
	SetModifiedFlag();
}

BSTR CXMMVViewCtrl::GetStrMsg(short nIndex) 
{
	CString strResult;
	if( (nIndex >= 0) && (nIndex < AN(m_cInfoMsg)) ){
		strResult = m_strInfoMsg[nIndex];
	}
	else {
		strResult = "";
	}
	return strResult.AllocSysString();
}

void CXMMVViewCtrl::SetStrMsg(short nIndex, LPCTSTR lpszNewValue) 
{
	if( (nIndex < 0) || (nIndex >= AN(m_cInfoMsg)) ) return;
	if( !lpszNewValue ) lpszNewValue = "";
	m_strInfoMsg[nIndex] = lpszNewValue;
	m_cInfoMsg[nIndex] = *lpszNewValue;
	SetModifiedFlag();
}

void CXMMVViewCtrl::OnWCursorFreqChanged() 
{
	if( m_wCursorFreq > 3000 ) m_wCursorFreq = 3000;
	DrawDirect();
	SetModifiedFlag();
}

void CXMMVViewCtrl::OnBHighQualityChanged() 
{
	SetModifiedFlag();
}

void CXMMVViewCtrl::ClearWater() 
{
	if( m_bmpSize.cx && IsTypeWater() ){
		ClearWaterBitmap();
		InitWater(iniwBOTH);
	}
}

void CXMMVViewCtrl::OnBShowErrorMsgChanged() 
{
	SetModifiedFlag();
}

void CXMMVViewCtrl::DrawFreqErr(long dwHandle, short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return;

	if( dwHandle && (m_wType == viewtypeFREQERR) ){
		m_pMainCtrl = (CMMVARICtrl *)dwHandle;
		m_nIndex = nIndex;
		DrawDirect();
	}
}

OLE_COLOR CXMMVViewCtrl::GetColorFreqErr(short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= AN(m_clFREQERR)) ) return RGB(0,0,0);
	return m_clFREQERR[nIndex];
}

void CXMMVViewCtrl::SetColorFreqErr(short nIndex, OLE_COLOR nNewValue) 
{
	if( (nIndex < 0) || (nIndex >= AN(m_clFREQERR)) ) return;
	m_clFREQERR[nIndex] = nNewValue;
	if( m_wType == viewtypeFREQERR ) DrawDirect();
	SetModifiedFlag();
}

void CXMMVViewCtrl::DrawSmallWater(long dwHandle, short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return;

	if( dwHandle && (m_wType == viewtypeSMALLWATER) ){
		m_pMainCtrl = (CMMVARICtrl *)dwHandle;
		m_nIndex = nIndex;
		DrawDirect(TRUE);
	}
}

void CXMMVViewCtrl::OnBAttachMouseChanged() 
{
	SetModifiedFlag();
}

BSTR CXMMVViewCtrl::GetStrErrorMsg(short nIndex) 
{
	CString strResult;
	if( (nIndex >= 0) && (nIndex < errorEND) ){
		strResult = m_strErrorMsg[nIndex];
	}
	return strResult.AllocSysString();
}

void CXMMVViewCtrl::SetStrErrorMsg(short nIndex, LPCTSTR lpszNewValue) 
{
	if( (nIndex >= 0) && (nIndex < errorEND) ){
		m_strErrorMsg[nIndex] = lpszNewValue;
		SetModifiedFlag();
	}
}

short CXMMVViewCtrl::GetWSizeXW() 
{
	return short(m_bmpSize.cx);
}

short CXMMVViewCtrl::GetWPixelYW() 
{
	return short(m_bmpSize.cy);
}

void CXMMVViewCtrl::OnBWindowsMouseEventChanged() 
{
	SetModifiedFlag();
}

short CXMMVViewCtrl::GetWMouseNotch() 
{
	return m_MouseNotch;
}

short CXMMVViewCtrl::GetWMouseChannel() 
{
	return m_MouseSubChannel;
}

