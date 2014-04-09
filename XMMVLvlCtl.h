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

#if !defined(AFX_XMMVLVLCTL_H__A11916AC_0445_4B86_B0B3_E3A85667C5BD__INCLUDED_)
#define AFX_XMMVLVLCTL_H__A11916AC_0445_4B86_B0B3_E3A85667C5BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// XMMVLvlCtl.h : CXMMVLvlCtrl ActiveX コントロール クラスの宣言
#include "dsp.h"
class CMMVARICtrl;
/////////////////////////////////////////////////////////////////////////////
// CXMMVLvlCtrl : このクラスの動作の定義に関しては XMMVLvlCtl.cpp を参照してください

class CXMMVLvlCtrl : public COleControl
{
	DECLARE_DYNCREATE(CXMMVLvlCtrl)

private:
	CMMVARICtrl *m_pMainCtrl;
	int			m_nIndex;

	COLORREF	m_clBack;
	COLORREF	m_clLevelTx;
	COLORREF	m_clLevelOn;
	COLORREF	m_clLevelOff;
	COLORREF	m_clLine;
	CRect		m_rcClient;
	CSize		m_bmpSize;
	CBitmap		m_bmpLevel;

	BYTE		*m_pPara;
private:
	void InitialUpdate(void);
	void Initialize(void);
	void DrawBitmap(CDC *pDC, const CRect &rc);
	void UpdateSize(const CRect &rc);
	void DrawLevel(CRxSet *pRxSet);
	SHORT GetLevel(CPoint &point);
	void DrawInternal(void);
	void DrawInternal(short wSig, short wSq, BOOL fOver);
// コンストラクタ
public:
	CXMMVLvlCtrl();

// オーバーライド
	// ClassWizard は仮想関数のオーバライドを生成します
	//{{AFX_VIRTUAL(CXMMVLvlCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	~CXMMVLvlCtrl();

	DECLARE_OLECREATE_EX(CXMMVLvlCtrl)   // クラス ファクトリ と guid
	DECLARE_OLETYPELIB(CXMMVLvlCtrl)     // GetTypeInfo
	DECLARE_PROPPAGEIDS(CXMMVLvlCtrl)    // プロパティ ページ ID
	DECLARE_OLECTLTYPE(CXMMVLvlCtrl)     // タイプ名とその他のステータス

// メッセージ マップ
	//{{AFX_MSG(CXMMVLvlCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// ディスパッチ マップ
	//{{AFX_DISPATCH(CXMMVLvlCtrl)
	BOOL m_visible;
	afx_msg void OnVisibleChanged();
	short m_maxLevel;
	afx_msg void OnMaxLevelChanged();
	OLE_COLOR m_overColor;
	afx_msg void OnOverColorChanged();
	OLE_COLOR m_unlockColor;
	afx_msg void OnUnlockColorChanged();
	afx_msg OLE_COLOR GetOnColor();
	afx_msg void SetOnColor(OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetOffColor();
	afx_msg void SetOffColor(OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetLineColor();
	afx_msg void SetLineColor(OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetBackColor();
	afx_msg void SetBackColor(OLE_COLOR nNewValue);
	afx_msg OLE_COLOR GetTxColor();
	afx_msg void SetTxColor(OLE_COLOR nNewValue);
	afx_msg void Clear();
	afx_msg void Draw(long dwHandle, short nIndex);
	afx_msg void DrawCustom(short wSig, short wSq, short fOver);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// イベント マップ
	//{{AFX_EVENT(CXMMVLvlCtrl)
	void FireOnLMouseDown(short level)
		{FireEvent(eventidOnLMouseDown,EVENT_PARAM(VTS_I2), level);}
	void FireOnLMouseUp(short level)
		{FireEvent(eventidOnLMouseUp,EVENT_PARAM(VTS_I2), level);}
	void FireOnLMouseMove(short level)
		{FireEvent(eventidOnLMouseMove,EVENT_PARAM(VTS_I2), level);}
	void FireOnRMouseDown(short level)
		{FireEvent(eventidOnRMouseDown,EVENT_PARAM(VTS_I2), level);}
	void FireOnRMouseUp(short level)
		{FireEvent(eventidOnRMouseUp,EVENT_PARAM(VTS_I2), level);}
	void FireOnRMouseMove(short level)
		{FireEvent(eventidOnRMouseMove,EVENT_PARAM(VTS_I2), level);}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// ディスパッチ と イベント ID
public:
	enum {
	//{{AFX_DISP_ID(CXMMVLvlCtrl)
	dispidVisible = 1L,
	dispidOnColor = 5L,
	dispidOffColor = 6L,
	dispidLineColor = 7L,
	dispidBackColor = 8L,
	dispidMaxLevel = 2L,
	dispidTxColor = 9L,
	dispidOverColor = 3L,
	dispidUnlockColor = 4L,
	dispidClear = 10L,
	dispidDraw = 11L,
	dispidDrawCustom = 12L,
	eventidOnLMouseDown = 1L,
	eventidOnLMouseUp = 2L,
	eventidOnLMouseMove = 3L,
	eventidOnRMouseDown = 4L,
	eventidOnRMouseUp = 5L,
	eventidOnRMouseMove = 6L,
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_XMMVLVLCTL_H__A11916AC_0445_4B86_B0B3_E3A85667C5BD__INCLUDED)
