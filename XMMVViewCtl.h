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

#if !defined(AFX_XMMVVIEWCTL_H__20DAB3AA_616E_4AA1_96F4_B3F5335A9DAC__INCLUDED_)
#define AFX_XMMVVIEWCTL_H__20DAB3AA_616E_4AA1_96F4_B3F5335A9DAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// XMMVViewCtl.h : CXMMVViewCtrl ActiveX コントロール クラスの宣言
typedef enum {
	viewtypeFFT,
	viewtypeWATER,
	viewtypeWAVE,
	viewtypeFREQERR,
	viewtypeSMALLWATER,
	viewtypeEND,
}MMVX_VIEWTYPE;

#include "dsp.h"

class CMMVARICtrl;
/////////////////////////////////////////////////////////////////////////////
// CXMMVViewCtrl : このクラスの動作の定義に関しては XMMVViewCtl.cpp を参照してください
typedef enum {
	CF_ARROW,
	CF_SIZEWE,
	CF_END,	
}MMVX_CURSOR;

#define		NOTCHMAX	256

class CXMMVViewCtrl : public COleControl
{
	DECLARE_DYNCREATE(CXMMVViewCtrl)
public:
	CString		m_strErrorMsg[errorEND];
private:
	CMMVARICtrl *m_pMainCtrl;
	BOOL		m_bMouseDown;
	UINT		m_WaterSemaphore;
	int			m_FFTGain;

    FFTSTG		m_StgWater;

	COLORREF	m_clFFT[6];
	UCOL		m_clWATER[12];
	COLORREF	m_clWAVE[4];
	COLORREF	m_clFREQERR[5];
	int			m_nIndex;

	int			m_tWaterLevel[6];
	COLORREF	m_tWaterColors[256];

	CRect		m_rcClient;
	CSize		m_bmpSize;
	CBitmap		m_bmpScope;

	int			m_MouseSubChannel;
	int			m_MouseNotch;
	int			m_NotchX[NOTCHMAX];
	int			m_NotchY[NOTCHMAX];
	int			m_SX[RXMAX];
	int			m_SY[RXMAX];
	HCURSOR		m_StdCursor[CF_END];

	int			m_cInfoMsg[5];
	CString		m_strInfoMsg[5];

	short		m_wPrevType;
	short		m_wPrevBaseFreq;
	short		m_wPrevWidthFreq;
private:
	int LimitFreq(int fq);
	void InitialUpdate(void);
	void Initialize(void);
	void DrawBitmap(CDC *pDC, const CRect &rc);
	void UpdateSize(const CRect &rc);
	void CreateWaterColors(void);

	void ClearWaterBitmap(void);
	void DrawWnd(CDC *pDC);
	void DrawDirect(BOOL fScroll = FALSE);
	void DrawFFT(void);
	void DrawWater(BOOL fScroll);
	void DrawWaveInternal(void);
	void DrawFreqErrInternal(void);
	void DrawSmallWaterInternal(BOOL fScroll);

	void CalcSmallWaterCenter(CRxSet *pRxSet);
	void DrawSmallWaterCursor(CDC *pDC);

	void __fastcall DrawFreqScale(CDC *pDC, int XW, int YW,  int fftb, int fftw, int fh, COLORREF LCol, COLORREF FCol);
	void __fastcall DrawSubChannel(CDC *pDC);

	void __fastcall CalcFFTCenter(int fo);
	void __fastcall CalcFFTFreq(void);
	void __fastcall InitWater(int sw);

	int GetMouseFreq(int x);

	BOOL __fastcall IsTypeFreq(void);
	BOOL __fastcall IsTypeWater(void);
	BOOL __fastcall IsTypeMouse(void);

// コンストラクタ
public:
	CXMMVViewCtrl();

// オーバーライド
	// ClassWizard は仮想関数のオーバライドを生成します
	//{{AFX_VIRTUAL(CXMMVViewCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual void OnFontChanged();
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	~CXMMVViewCtrl();

	DECLARE_OLECREATE_EX(CXMMVViewCtrl)   // クラス ファクトリ と guid
	DECLARE_OLETYPELIB(CXMMVViewCtrl)     // GetTypeInfo
	DECLARE_PROPPAGEIDS(CXMMVViewCtrl)    // プロパティ ページ ID
	DECLARE_OLECTLTYPE(CXMMVViewCtrl)     // タイプ名とその他のステータス

// メッセージ マップ
	//{{AFX_MSG(CXMMVViewCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// ディスパッチ マップ
	//{{AFX_DISPATCH(CXMMVViewCtrl)
	long m_dwFreqHz;
	afx_msg void OnDwFreqHzChanged();
	BOOL m_bLSB;
	afx_msg void OnBLSBChanged();
	short m_wBaseFreq;
	afx_msg void OnWBaseFreqChanged();
	short m_wWidthFreq;
	afx_msg void OnWWidthFreqChanged();
	BOOL m_bWaterAGC;
	afx_msg void OnBWaterAGCChanged();
	BOOL m_visible;
	afx_msg void OnVisibleChanged();
	BOOL m_bFollowRxFreq;
	afx_msg void OnBFollowRxFreqChanged();
	short m_wType;
	afx_msg void OnWTypeChanged();
	short m_wWaterNoiseL;
	afx_msg void OnWWaterNoiseLChanged();
	short m_wWaterNoiseH;
	afx_msg void OnWWaterNoiseHChanged();
	BOOL m_bShowSync;
	afx_msg void OnBShowSyncChanged();
	short m_wCursorFreq;
	afx_msg void OnWCursorFreqChanged();
	BOOL m_bHighQuality;
	afx_msg void OnBHighQualityChanged();
	BOOL m_bShowErrorMsg;
	afx_msg void OnBShowErrorMsgChanged();
	BOOL m_bAttachMouse;
	afx_msg void OnBAttachMouseChanged();
	BOOL m_bWindowsMouseEvent;
	afx_msg void OnBWindowsMouseEventChanged();
	afx_msg short GetWWaterNoise();
	afx_msg short GetWSizeXW();
	afx_msg short GetWPixelYW();
	afx_msg short GetWMouseNotch();
	afx_msg short GetWMouseChannel();
	afx_msg void Draw(long dwHandle);
	afx_msg void DrawWave(long dwHandle);
	afx_msg void ClearWater();
	afx_msg void DrawFreqErr(long dwHandle, short nIndex);
	afx_msg void DrawSmallWater(long dwHandle, short nIndex);
	afx_msg OLE_COLOR GetColorFFT(short nIndex);
	afx_msg void SetColorFFT(short nIndex, OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetColorWater(short nIndex);
	afx_msg void SetColorWater(short nIndex, OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetColorWave(short nIndex);
	afx_msg void SetColorWave(short nIndex, OLE_COLOR nNewValue);
	afx_msg short GetWWaterLevel(short nIndex);
	afx_msg void SetWWaterLevel(short nIndex, short nNewValue);
	afx_msg OLE_COLOR GetWaterPalette(short nIndex);
	afx_msg void SetWaterPalette(short nIndex, OLE_COLOR nNewValue);
	afx_msg BSTR GetStrMsg(short nIndex);
	afx_msg void SetStrMsg(short nIndex, LPCTSTR lpszNewValue);
	afx_msg OLE_COLOR GetColorFreqErr(short nIndex);
	afx_msg void SetColorFreqErr(short nIndex, OLE_COLOR nNewValue);
	afx_msg BSTR GetStrErrorMsg(short nIndex);
	afx_msg void SetStrErrorMsg(short nIndex, LPCTSTR lpszNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// イベント マップ
	//{{AFX_EVENT(CXMMVViewCtrl)
	void FireOnLMouseDown(short wFreq)
		{FireEvent(eventidOnLMouseDown,EVENT_PARAM(VTS_I2), wFreq);}
	void FireOnLMouseUp(short wFreq)
		{FireEvent(eventidOnLMouseUp,EVENT_PARAM(VTS_I2), wFreq);}
	void FireOnLMouseMove(short wFreq)
		{FireEvent(eventidOnLMouseMove,EVENT_PARAM(VTS_I2), wFreq);}
	void FireOnRMouseDown(short wFreq)
		{FireEvent(eventidOnRMouseDown,EVENT_PARAM(VTS_I2), wFreq);}
	void FireOnRMouseMove(short wFreq)
		{FireEvent(eventidOnRMouseMove,EVENT_PARAM(VTS_I2), wFreq);}
	void FireOnRMouseUp(short wFreq)
		{FireEvent(eventidOnRMouseUp,EVENT_PARAM(VTS_I2), wFreq);}
	void FireOnWindowsLMouseDown(short wX, short wY)
		{FireEvent(eventidOnWindowsLMouseDown,EVENT_PARAM(VTS_I2  VTS_I2), wX, wY);}
	void FireOnWindowsLMouseUp(short wX, short wY)
		{FireEvent(eventidOnWindowsLMouseUp,EVENT_PARAM(VTS_I2  VTS_I2), wX, wY);}
	void FireOnWindowsRMouseUp(short wX, short wY)
		{FireEvent(eventidOnWindowsRMouseUp,EVENT_PARAM(VTS_I2  VTS_I2), wX, wY);}
	void FireOnWindowsRMouseDown(short wX, short wY)
		{FireEvent(eventidOnWindowsRMouseDown,EVENT_PARAM(VTS_I2  VTS_I2), wX, wY);}
	void FireOnWindowsMouseMove(long dwFlag, short wX, short wY)
		{FireEvent(eventidOnWindowsMouseMove,EVENT_PARAM(VTS_I4  VTS_I2  VTS_I2), dwFlag, wX, wY);}
	void FireOnWindowsLMouseDblClk(short wX, short wY)
		{FireEvent(eventidOnWindowsLMouseDblClk,EVENT_PARAM(VTS_I2  VTS_I2), wX, wY);}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// ディスパッチ と イベント ID
public:
	enum {
	//{{AFX_DISP_ID(CXMMVViewCtrl)
	dispidDwFreqHz = 1L,
	dispidBLSB = 2L,
	dispidWBaseFreq = 3L,
	dispidWWidthFreq = 4L,
	dispidBWaterAGC = 5L,
	dispidVisible = 6L,
	dispidBFollowRxFreq = 7L,
	dispidWType = 8L,
	dispidWWaterNoiseL = 9L,
	dispidWWaterNoiseH = 10L,
	dispidWWaterNoise = 17L,
	dispidBShowSync = 11L,
	dispidWCursorFreq = 12L,
	dispidBHighQuality = 13L,
	dispidBShowErrorMsg = 14L,
	dispidBAttachMouse = 15L,
	dispidWPixelXW = 18L,
	dispidWPixelYW = 19L,
	dispidBWindowsMouseEvent = 16L,
	dispidWMouseNotch = 20L,
	dispidWMouseChannel = 21L,
	dispidDraw = 22L,
	dispidDrawWave = 23L,
	dispidColorFFT = 27L,
	dispidColorWater = 28L,
	dispidColorWave = 29L,
	dispidWWaterLevel = 30L,
	dispidWaterPalette = 31L,
	dispidStrMsg = 32L,
	dispidClearWater = 24L,
	dispidDrawFreqErr = 25L,
	dispidColorFreqErr = 33L,
	dispidDrawSmallWater = 26L,
	dispidStrErrorMsg = 34L,
	eventidOnLMouseDown = 1L,
	eventidOnLMouseUp = 2L,
	eventidOnLMouseMove = 3L,
	eventidOnRMouseDown = 4L,
	eventidOnRMouseMove = 5L,
	eventidOnRMouseUp = 6L,
	eventidOnWindowsLMouseDown = 7L,
	eventidOnWindowsLMouseUp = 8L,
	eventidOnWindowsRMouseUp = 9L,
	eventidOnWindowsRMouseDown = 10L,
	eventidOnWindowsMouseMove = 11L,
	eventidOnWindowsLMouseDblClk = 12L,
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_XMMVVIEWCTL_H__20DAB3AA_616E_4AA1_96F4_B3F5335A9DAC__INCLUDED)
