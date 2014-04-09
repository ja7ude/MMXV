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

// XMMVLvlCtl.cpp : CXMMVLvlCtrl ActiveX �R���g���[�� �N���X�̃C���v�������e�[�V����

#include "stdafx.h"
#include "MMVARI.h"
#include "MMVARICtl.h"
#include "XMMVLvlCtl.h"
#include "XMMVLvlPpg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CXMMVLvlCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// ���b�Z�[�W �}�b�v

BEGIN_MESSAGE_MAP(CXMMVLvlCtrl, COleControl)
	//{{AFX_MSG_MAP(CXMMVLvlCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// �f�B�X�p�b�` �}�b�v

BEGIN_DISPATCH_MAP(CXMMVLvlCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CXMMVLvlCtrl)
	DISP_PROPERTY_NOTIFY(CXMMVLvlCtrl, "Visible", m_visible, OnVisibleChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CXMMVLvlCtrl, "MaxLevel", m_maxLevel, OnMaxLevelChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CXMMVLvlCtrl, "OverColor", m_overColor, OnOverColorChanged, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CXMMVLvlCtrl, "UnlockColor", m_unlockColor, OnUnlockColorChanged, VT_COLOR)
	DISP_PROPERTY_EX(CXMMVLvlCtrl, "OnColor", GetOnColor, SetOnColor, VT_COLOR)
	DISP_PROPERTY_EX(CXMMVLvlCtrl, "OffColor", GetOffColor, SetOffColor, VT_COLOR)
	DISP_PROPERTY_EX(CXMMVLvlCtrl, "LineColor", GetLineColor, SetLineColor, VT_COLOR)
	DISP_PROPERTY_EX(CXMMVLvlCtrl, "BackColor", GetBackColor, SetBackColor, VT_COLOR)
	DISP_PROPERTY_EX(CXMMVLvlCtrl, "TxColor", GetTxColor, SetTxColor, VT_COLOR)
	DISP_FUNCTION(CXMMVLvlCtrl, "Clear", Clear, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CXMMVLvlCtrl, "Draw", Draw, VT_EMPTY, VTS_I4 VTS_I2)
	DISP_FUNCTION(CXMMVLvlCtrl, "DrawCustom", DrawCustom, VT_EMPTY, VTS_I2 VTS_I2 VTS_I2)
	DISP_STOCKPROP_BORDERSTYLE()
	DISP_STOCKPROP_HWND()
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// �C�x���g �}�b�v

BEGIN_EVENT_MAP(CXMMVLvlCtrl, COleControl)
	//{{AFX_EVENT_MAP(CXMMVLvlCtrl)
	EVENT_CUSTOM("OnLMouseDown", FireOnLMouseDown, VTS_I2)
	EVENT_CUSTOM("OnLMouseUp", FireOnLMouseUp, VTS_I2)
	EVENT_CUSTOM("OnLMouseMove", FireOnLMouseMove, VTS_I2)
	EVENT_CUSTOM("OnRMouseDown", FireOnRMouseDown, VTS_I2)
	EVENT_CUSTOM("OnRMouseUp", FireOnRMouseUp, VTS_I2)
	EVENT_CUSTOM("OnRMouseMove", FireOnRMouseMove, VTS_I2)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// �v���p�e�B �y�[�W

// TODO: �v���p�e�B �y�[�W��ǉ����āABEGIN �s�̍Ō�ɂ���J�E���g�𑝂₵�Ă��������B
BEGIN_PROPPAGEIDS(CXMMVLvlCtrl, 1)
	PROPPAGEID(CXMMVLvlPropPage::guid)
END_PROPPAGEIDS(CXMMVLvlCtrl)


/////////////////////////////////////////////////////////////////////////////
// �N���X �t�@�N�g���� guid �����������܂�

IMPLEMENT_OLECREATE_EX(CXMMVLvlCtrl, "XMMVLVL.XMMVLvlCtrl.1",
	0x438ef93a, 0x939d, 0x4b6b, 0x93, 0xa7, 0xdf, 0x9, 0x4, 0x9b, 0x85, 0x14)


/////////////////////////////////////////////////////////////////////////////
// �^�C�v ���C�u���� ID �ƃo�[�W����

IMPLEMENT_OLETYPELIB(CXMMVLvlCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// �C���^�[�t�F�C�X ID

const IID BASED_CODE IID_DXMMVLvl =
		{ 0x6b314f73, 0xebb7, 0x47a2, { 0x96, 0x3e, 0x73, 0x2d, 0xed, 0xf8, 0x7c, 0x83 } };
const IID BASED_CODE IID_DXMMVLvlEvents =
		{ 0x5056f966, 0xcbcd, 0x4d96, { 0xb1, 0xef, 0x7b, 0xd8, 0x5f, 0xfa, 0xe4, 0x5b } };


/////////////////////////////////////////////////////////////////////////////
// �R���g���[���̌^���̐���

static const DWORD BASED_CODE _dwXMMVLvlOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CXMMVLvlCtrl, IDS_XMMVLVL, _dwXMMVLvlOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CXMMVLvlCtrl::CXMMVLvlCtrlFactory::UpdateRegistry -
// CXMMVLvlCtrl �̃V�X�e�� ���W�X�g���̃G���g����ǉ��܂��͍폜���܂� 

BOOL CXMMVLvlCtrl::CXMMVLvlCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: �R���g���[�����A�p�[�g�����g ���f���̃X���b�h�����̋K���ɏ]���Ă��邱�Ƃ�
	// �m�F���Ă��������B�ڍׂɂ��Ă� MFC �̃e�N�j�J�� �m�[�g 64 ���Q�Ƃ��Ă��������B
	// �A�p�[�g�����g ���f���̃X���b�h�����̋K���ɏ]��Ȃ��R���g���[���̏ꍇ�́A6 �Ԗ�
	// �̃p�����[�^���ȉ��̂悤�ɕύX���Ă��������B
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_XMMVLVL,
			IDB_XMMVLVL,
			afxRegApartmentThreading,
			_dwXMMVLvlOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVLvlCtrl::CXMMVLvlCtrl - �R���X�g���N�^

CXMMVLvlCtrl::CXMMVLvlCtrl()
{
	InitializeIIDs(&IID_DXMMVLvl, &IID_DXMMVLvlEvents);

	m_clBack = RGB(0,0,0);
	m_clLevelTx = RGB(255,255,0);
	m_clLevelOn = RGB(0,255,0);
	m_clLevelOff = RGB(192,192,192);
	m_clLine = RGB(255,255,0);
	m_bmpSize.cx = 0;
	m_bmpSize.cy = 0;
	m_maxLevel = 2048;

	m_overColor = RGB(255,0,0);
	m_unlockColor = RGB(0,0,255);

	m_pMainCtrl = NULL;
	m_nIndex = 0;
	m_pPara = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVLvlCtrl::~CXMMVLvlCtrl - �f�X�g���N�^

CXMMVLvlCtrl::~CXMMVLvlCtrl()
{
	m_bmpLevel.DeleteObject();
	if( m_pPara ){
		delete m_pPara;
		m_pPara = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXMMVLvlCtrl::OnDraw - �`��֐�

void CXMMVLvlCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	UpdateSize(rcBounds);
	DrawBitmap(pdc, rcBounds);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVLvlCtrl::DoPropExchange - �i�����̃T�|�[�g

void CXMMVLvlCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	PX_Bool(pPX, _T("Visible"), m_visible, TRUE);
	PX_Color(pPX, _T("TxColor"), m_clLevelTx, RGB(255,255,0));
	PX_Color(pPX, _T("OnColor"), m_clLevelOn, RGB(0,255,0));
	PX_Color(pPX, _T("OffColor"), m_clLevelOff, RGB(192,192,192));
	PX_Color(pPX, _T("LineColor"), m_clLine, RGB(255,255,0));
	PX_Color(pPX, _T("BackColor"), m_clBack, RGB(0,0,0));
	PX_Color(pPX, _T("OverColor"), m_overColor, RGB(255,0,0));
	PX_Color(pPX, _T("UnlockColor"), m_unlockColor, RGB(0,0,255));
	PX_Short(pPX, _T("MaxLevel"), m_maxLevel, 2048);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVLvlCtrl::OnResetState - �R���g���[���̃v���p�e�B�l�����Z�b�g

void CXMMVLvlCtrl::OnResetState()
{
	COleControl::OnResetState();  // DoPropExchange ���Ăяo���ăf�t�H���g�l�Ƀ��Z�b�g

	// ���̈ʒu�ɃR���g���[���̏�Ԃ����Z�b�g���鏈����ǉ����Ă�������
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
void CXMMVLvlCtrl::InitialUpdate(void)
{
	CRect rc;
	GetClientRect(&rc);
	UpdateSize(&rc);
}
//---------------------------------------------------------------------------
void CXMMVLvlCtrl::Initialize(void)
{
	if( !m_bmpSize.cx ) InitialUpdate();
}
//---------------------------------------------------------------------------
void CXMMVLvlCtrl::DrawBitmap(CDC *pDC, const CRect &rc)
{
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp = dc.SelectObject(&m_bmpLevel);
	pDC->BitBlt(rc.left, rc.top, m_bmpSize.cx, m_bmpSize.cy, &dc, 0, 0, SRCCOPY);
	dc.SelectObject(pOldBmp);
	dc.DeleteDC();
}
//---------------------------------------------------------------------------
void CXMMVLvlCtrl::UpdateSize(const CRect &rc)
{
	m_rcClient = rc;
	if( (rc.Width() != m_bmpSize.cx) || (rc.Height() != m_bmpSize.cy) ){
		m_bmpSize.cx = rc.Width();
		m_bmpSize.cy = rc.Height();
		m_bmpLevel.DeleteObject();
		CDC *pDC = GetDC();
		VERIFY(pDC->GetMapMode() == MM_TEXT);
		m_bmpLevel.CreateCompatibleBitmap(pDC, m_bmpSize.cx, m_bmpSize.cy);
		ReleaseDC(pDC);
		DrawLevel(NULL);
	}
}
//---------------------------------------------------------------------------
void CXMMVLvlCtrl::DrawLevel(CRxSet *pRxSet)
{
	if( !m_bmpSize.cx ) return;

	RECT rc;
	CDC dc;
	dc.CreateCompatibleDC(NULL);
	CBitmap *pOldBmp = dc.SelectObject(&m_bmpLevel);
	dc.FillSolidRect(&m_rcClient, GetRefColor(m_clBack));

	if( pRxSet ){	
		int sig = pRxSet->m_StgFFT.Sig - 500;
		if( m_pMainCtrl && (m_pMainCtrl->m_TX != txINTERNAL) && pRxSet->IsMFSK() && m_pMainCtrl->m_MFSK_SQ_Metric ){
			if( pRxSet->IsActive() ) sig = pRxSet->m_pDem->GetMFSKMetric(0);
		}
	   	if( sig > m_maxLevel ) sig = m_maxLevel;
		if( pRxSet->IsActive() || (pRxSet == (CRxSet *)m_pPara) ){
			COLORREF col;
			if( pRxSet->m_fOver ){
				col = m_overColor;
			}
			else if( pRxSet->m_pDem && !pRxSet->m_pDem->m_Lock ){
				col = m_unlockColor;
			}
			else {
				col = (m_pMainCtrl && (m_pMainCtrl->m_TX == txINTERNAL)) ? m_clLevelTx : m_clLevelOn;
				if( !pRxSet->m_SQ && (sig < pRxSet->m_SQLevel) ) col = m_clLevelOff;
			}
			rc.left = rc.top = 0;
			rc.right = m_bmpSize.cx; rc.bottom = m_bmpSize.cy;
			if( m_bmpSize.cy > m_bmpSize.cx ){
				rc.top = m_bmpSize.cy - (sig * m_bmpSize.cy / m_maxLevel);
				dc.FillSolidRect(&rc, col);

				rc.top = m_bmpSize.cy - (pRxSet->m_SQLevel * m_bmpSize.cy / m_maxLevel);
				if( rc.top < m_sBorderStyle ) rc.top = m_sBorderStyle;
				if( rc.top < rc.bottom ){
					CPen penmarker(PS_SOLID, 0, GetRefColor(pRxSet->m_SQ ? m_clBack : m_clLine));
					CPen *pOldPen = dc.SelectObject(&penmarker);
					dc.MoveTo(0, rc.top);
					dc.LineTo(m_bmpSize.cx, rc.top);
					dc.SelectObject(pOldPen);
					penmarker.DeleteObject();
				}
			}
			else {
				rc.right = sig * m_bmpSize.cx / m_maxLevel;
				dc.FillSolidRect(&rc, col);

				rc.right = pRxSet->m_SQLevel * m_bmpSize.cx / m_maxLevel;
				if( rc.right >= (m_bmpSize.cx - m_sBorderStyle) ) rc.right = (m_bmpSize.cx - m_sBorderStyle - 1);
				if( rc.left < rc.right ){
					CPen penmarker(PS_SOLID, 0, GetRefColor(pRxSet->m_SQ ? m_clBack : m_clLine));
					CPen *pOldPen = dc.SelectObject(&penmarker);
					dc.MoveTo(rc.right, 0);
					dc.LineTo(rc.right, m_bmpSize.cy);
					dc.SelectObject(pOldPen);
					penmarker.DeleteObject();
				}
			}
		}
	}
	if( m_sBorderStyle ){
		rc.left = rc.top = 0;
		rc.right = m_bmpSize.cx; rc.bottom = m_bmpSize.cy;
		DrawBorderStyle(&dc, rc);
	}
	dc.SelectObject(pOldBmp);
	dc.DeleteDC();
}

//---------------------------------------------------------------------------
SHORT CXMMVLvlCtrl::GetLevel(CPoint &point)
{
	int level;
	if( m_bmpSize.cy > m_bmpSize.cx ){
		level = (m_bmpSize.cy - point.y) * m_maxLevel/ m_bmpSize.cy; 
	}
	else {
		level = point.x * m_maxLevel/ m_bmpSize.cx; 
	}
	if( level < 0 ) level = 0;
	if( level > m_maxLevel ) level = m_maxLevel;
	return SHORT(level);
}

void CXMMVLvlCtrl::DrawInternal(void)
{
	if( !m_bmpSize.cx ) return;

	CRxSet *pRxSet = NULL;
	if( m_pMainCtrl ){
		pRxSet = &m_pMainCtrl->m_RxSet[m_nIndex];
	}
	DrawLevel(pRxSet);
	CClientDC dc(this);
	DrawBitmap(&dc, m_rcClient);
}

void CXMMVLvlCtrl::DrawInternal(short wSig, short wSq, BOOL fOver)
{
	if( !m_bmpSize.cx ) return;

	m_pMainCtrl = NULL;
	m_nIndex = 0;
	if( !m_pPara ){
		m_pPara = new BYTE[sizeof(CRxSet)];
		memset(m_pPara, 0, sizeof(CRxSet));
	}
	CRxSet *pRxSet = (CRxSet *)m_pPara;
	pRxSet->m_fOver = fOver;
	pRxSet->m_StgFFT.Sig = wSig + 500;
	pRxSet->m_SQLevel = wSq;
	DrawLevel(pRxSet);
	CClientDC dc(this);
	DrawBitmap(&dc, m_rcClient);
}
/////////////////////////////////////////////////////////////////////////////
// CXMMVLvlCtrl ���b�Z�[�W �n���h��

OLE_COLOR CXMMVLvlCtrl::GetOnColor() 
{
	return m_clLevelOn;
}

void CXMMVLvlCtrl::SetOnColor(OLE_COLOR nNewValue) 
{
	m_clLevelOn = nNewValue;
	SetModifiedFlag();
}

OLE_COLOR CXMMVLvlCtrl::GetOffColor() 
{
	return m_clLevelOff;
}

void CXMMVLvlCtrl::SetOffColor(OLE_COLOR nNewValue) 
{
	m_clLevelOff = nNewValue;
	SetModifiedFlag();
}

OLE_COLOR CXMMVLvlCtrl::GetBackColor() 
{
	return m_clBack;
}

void CXMMVLvlCtrl::SetBackColor(OLE_COLOR nNewValue) 
{
	m_clBack = nNewValue;
	DrawInternal();
	SetModifiedFlag();
}

OLE_COLOR CXMMVLvlCtrl::GetLineColor() 
{
	return m_clLine;
}

void CXMMVLvlCtrl::SetLineColor(OLE_COLOR nNewValue) 
{
	m_clLine = nNewValue;
	DrawInternal();
	SetModifiedFlag();
}

void CXMMVLvlCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( nFlags == MK_LBUTTON ){
		SetCapture();
		short level = GetLevel(point);
		FireOnLMouseDown(level);
		if( m_pMainCtrl ){
			m_pMainCtrl->SetSQLevel(m_nIndex, level);
			if( IsWindowVisible() ) DrawInternal();
		}
	}
//	COleControl::OnLButtonDown(nFlags, point);
}

void CXMMVLvlCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( GetCapture() == this ){
		ReleaseCapture();
		FireOnLMouseUp(GetLevel(point));
	}
//	COleControl::OnLButtonUp(nFlags, point);
}

void CXMMVLvlCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( GetCapture() == this ){
		short level = GetLevel(point);
		if( nFlags & MK_LBUTTON ){
			FireOnLMouseMove(level);
			if( m_pMainCtrl ){
				m_pMainCtrl->SetSQLevel(m_nIndex, level);
				if( IsWindowVisible() ) DrawInternal();
			}
		}
		else if( nFlags & MK_RBUTTON ){
			FireOnRMouseMove(level);
		}
	}
//	COleControl::OnMouseMove(nFlags, point);
}

void CXMMVLvlCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( nFlags == MK_RBUTTON ){
		SetCapture();
		FireOnRMouseDown(GetLevel(point));
	}
//	COleControl::OnRButtonDown(nFlags, point);
}

void CXMMVLvlCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if( GetCapture() == this ){
		ReleaseCapture();
		FireOnRMouseUp(GetLevel(point));
	}
//	COleControl::OnRButtonUp(nFlags, point);
}

void CXMMVLvlCtrl::Clear() 
{
	DrawInternal();
}

void CXMMVLvlCtrl::OnVisibleChanged() 
{
	ShowWindow(m_visible ? SW_SHOW : SW_HIDE);
	SetModifiedFlag();
}

void CXMMVLvlCtrl::OnMaxLevelChanged() 
{
	if( m_maxLevel <= 0 ) m_maxLevel = 1;
	DrawInternal();
	SetModifiedFlag();
}

OLE_COLOR CXMMVLvlCtrl::GetTxColor() 
{
	return m_clLevelTx;
}

void CXMMVLvlCtrl::SetTxColor(OLE_COLOR nNewValue) 
{
	m_clLevelTx = nNewValue;
	DrawInternal();
	SetModifiedFlag();
}

void CXMMVLvlCtrl::Draw(long dwHandle, short nIndex) 
{
	if( (nIndex < 0) || (nIndex >= RXMAX) ) return;

	if( dwHandle ){
		CMMVARICtrl *pCtrl = (CMMVARICtrl *)dwHandle;
		m_pMainCtrl = pCtrl;
		m_nIndex = nIndex;
		if( !IsWindowVisible() ) return;
		DrawInternal();
	}
}

void CXMMVLvlCtrl::OnOverColorChanged() 
{
	SetModifiedFlag();
}

void CXMMVLvlCtrl::OnUnlockColorChanged() 
{
	SetModifiedFlag();
}

void CXMMVLvlCtrl::DrawCustom(short wSig, short wSq, short fOver) 
{
	if( !IsWindowVisible() ) return;
	DrawInternal(wSig, wSq, fOver);
}
