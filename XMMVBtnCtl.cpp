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

// XMMVBtnCtl.cpp : CXMMVBtnCtrl ActiveX コントロール クラスのインプリメンテーション

#include "stdafx.h"
#include "MMVARI.h"
#include "XMMVBtnCtl.h"
#include "XMMVBtnPpg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CXMMVBtnCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// メッセージ マップ

BEGIN_MESSAGE_MAP(CXMMVBtnCtrl, COleControl)
	//{{AFX_MSG_MAP(CXMMVBtnCtrl)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(OCM_COMMAND, OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// ディスパッチ マップ

BEGIN_DISPATCH_MAP(CXMMVBtnCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CXMMVBtnCtrl)
	DISP_PROPERTY_NOTIFY(CXMMVBtnCtrl, "Visible", m_visible, OnVisibleChanged, VT_BOOL)
	DISP_PROPERTY_EX(CXMMVBtnCtrl, "Caption", GetCaption, SetCaption, VT_BSTR)
	DISP_PROPERTY_EX(CXMMVBtnCtrl, "TextColor", GetTextColor, SetTextColor, VT_COLOR)
	DISP_PROPERTY_EX(CXMMVBtnCtrl, "Enabled", GetEnabled, SetEnabled, VT_BOOL)
	DISP_PROPERTY_EX(CXMMVBtnCtrl, "State", GetState, SetState, VT_BOOL)
	DISP_PROPERTY_EX(CXMMVBtnCtrl, "Toggled", GetToggled, SetToggled, VT_BOOL)
	DISP_PROPERTY_EX(CXMMVBtnCtrl, "FaceColor", GetFaceColor, SetFaceColor, VT_COLOR)
	DISP_STOCKPROP_FONT()
	DISP_STOCKPROP_HWND()
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// イベント マップ

BEGIN_EVENT_MAP(CXMMVBtnCtrl, COleControl)
	//{{AFX_EVENT_MAP(CXMMVBtnCtrl)
	EVENT_CUSTOM("OnLButtonClick", FireOnLButtonClick, VTS_NONE)
	EVENT_CUSTOM("OnRButtonClick", FireOnRButtonClick, VTS_NONE)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// プロパティ ページ

// TODO: プロパティ ページを追加して、BEGIN 行の最後にあるカウントを増やしてください。
BEGIN_PROPPAGEIDS(CXMMVBtnCtrl, 1)
	PROPPAGEID(CXMMVBtnPropPage::guid)
END_PROPPAGEIDS(CXMMVBtnCtrl)


/////////////////////////////////////////////////////////////////////////////
// クラス ファクトリと guid を初期化します

IMPLEMENT_OLECREATE_EX(CXMMVBtnCtrl, "XMMVBTN.XMMVBtnCtrl.1",
	0xbdf39bd, 0x46d0, 0x42e8, 0xb8, 0xb2, 0x60, 0x45, 0xfe, 0xac, 0x5f, 0x4f)


/////////////////////////////////////////////////////////////////////////////
// タイプ ライブラリ ID とバージョン

IMPLEMENT_OLETYPELIB(CXMMVBtnCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// インターフェイス ID

const IID BASED_CODE IID_DXMMVBtn =
		{ 0x3492b872, 0xa984, 0x4848, { 0x9c, 0xa8, 0xc7, 0xfc, 0xaa, 0x68, 0x3, 0x29 } };
const IID BASED_CODE IID_DXMMVBtnEvents =
		{ 0x636224d0, 0x2f16, 0x4fe6, { 0xb2, 0xc9, 0xef, 0x6, 0xd7, 0x70, 0xca, 0x2c } };


/////////////////////////////////////////////////////////////////////////////
// コントロールの型情報の制御

static const DWORD BASED_CODE _dwXMMVBtnOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CXMMVBtnCtrl, IDS_XMMVBTN, _dwXMMVBtnOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CXMMVBtnCtrl::CXMMVBtnCtrlFactory::UpdateRegistry -
// CXMMVBtnCtrl のシステム レジストリのエントリを追加または削除します 

BOOL CXMMVBtnCtrl::CXMMVBtnCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_XMMVBTN,
			IDB_XMMVBTN,
			afxRegApartmentThreading,
			_dwXMMVBtnOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVBtnCtrl::CXMMVBtnCtrl - コンストラクタ

CXMMVBtnCtrl::CXMMVBtnCtrl()
{
	InitializeIIDs(&IID_DXMMVBtn, &IID_DXMMVBtnEvents);

	m_Enabled = TRUE;
	m_Toggled = TRUE;
	m_clEdge[0] = GetSysColor(COLOR_BTNHILIGHT);
	m_clEdge[1] = GetSysColor(COLOR_BTNSHADOW);
	m_clEdge[2] = GetSysColor(COLOR_3DDKSHADOW);
	m_clFace = 0x80000000 + COLOR_BTNFACE;
	m_clPush = RGB(255,255,255);
	m_fState = FALSE;
	m_clText = RGB(0,0,0);
	m_clGrayText = GetSysColor(COLOR_GRAYTEXT);
	memset(&m_rcClient, 0, sizeof(m_rcClient));
	m_visible = TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVBtnCtrl::~CXMMVBtnCtrl - デストラクタ

CXMMVBtnCtrl::~CXMMVBtnCtrl()
{
	// TODO: この位置にコントロールのインスタンス データの後処理用のコードを追加してください
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVBtnCtrl::OnDraw - 描画関数

void CXMMVBtnCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	m_rcClient = rcBounds;
	OnDrawInternal(pdc);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVBtnCtrl::DoPropExchange - 永続性のサポート

void CXMMVBtnCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	PX_Bool(pPX, _T("Enabled"), m_Enabled, TRUE);
	PX_Bool(pPX, _T("State"), m_fState, FALSE);
	PX_Bool(pPX, _T("Toggled"), m_Toggled, TRUE);
	PX_Bool(pPX, _T("Visible"), m_visible, TRUE);
	PX_String(pPX, _T("Caption"), m_Text, _T(""));
	PX_Color(pPX, _T("TextColor"), m_clText, RGB(0,0,0));
	PX_Color(pPX, _T("FaceColor"), m_clFace);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVBtnCtrl::OnResetState - コントロールのプロパティ値をリセット

void CXMMVBtnCtrl::OnResetState()
{
	COleControl::OnResetState();  // DoPropExchange を呼び出してデフォルト値にリセット

	// この位置にコントロールの状態をリセットする処理を追加してください
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVBtnCtrl::PreCreateWindow - CreateWindowEx のパラメータを修正

BOOL CXMMVBtnCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = _T("BUTTON");
	return COleControl::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVBtnCtrl::IsSubclassedControl - サブクラス化されたコントロール

BOOL CXMMVBtnCtrl::IsSubclassedControl()
{
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVBtnCtrl::OnOcmCommand - コマンド メッセージのハンドル

LRESULT CXMMVBtnCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: この位置にスイッチ ステートメントで wNotifyCode を処理するコードを追加してください

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVBtnCtrl メッセージ ハンドラ
//---------------------------------------------------------------------------
void CXMMVBtnCtrl::InitialUpdate(void)
{
	GetClientRect(&m_rcClient);
}
//---------------------------------------------------------------------------
void CXMMVBtnCtrl::Initialize(void)
{
	if( !m_rcClient.right ) InitialUpdate();
}
//---------------------------------------------------------------------------
void CXMMVBtnCtrl::Draw(void)
{
	if( !IsWindowVisible() ) return;
	CClientDC dc(this);
	OnDrawInternal(&dc);
}
//---------------------------------------------------------------------------
void CXMMVBtnCtrl::SetWindowText(LPCTSTR lpszString)
{
	if( lstrcmp(m_Text, lpszString) ){
		m_Text = lpszString;
		if( m_rcClient.right ) Draw();
	}
}
//---------------------------------------------------------------------------
void CXMMVBtnCtrl::GetWindowText(CString& rString) const
{
	rString = m_Text;
}
//---------------------------------------------------------------------------
void CXMMVBtnCtrl::EnableWindow(BOOL bEnable)
{
	if( bEnable != m_Enabled ){
		m_Enabled = bEnable;
		if( m_rcClient.right ) Draw();
	}
}
//---------------------------------------------------------------------------
void CXMMVBtnCtrl::OnDrawInternal(CDC *pDC)
{
	Initialize();

	RECT rc = m_rcClient;
	pDC->FillSolidRect(&rc, m_clEdge[2]);
	if( m_fState ){		// push button
		rc.top++; rc.left++;
		pDC->FillSolidRect(&rc, m_clEdge[1]);
		rc.top++; rc.left++;
		pDC->FillSolidRect(&rc, m_clEdge[0]);
		rc.bottom--; rc.right--;
		pDC->FillSolidRect(&rc, m_clPush);
	}
	else {				// pop button
		rc.bottom--; rc.right--;
		pDC->FillSolidRect(&rc, m_clEdge[1]);
		rc.bottom--; rc.right--;
		pDC->FillSolidRect(&rc, m_clEdge[0]);
		rc.top++; rc.left++;
		pDC->FillSolidRect(&rc, GetRefColor(m_clFace));
	}
	CFont *pOldFont = SelectStockFont(pDC);
	pDC->SetTextColor(GetRefColor(m_Enabled ? m_clText : m_clGrayText));
	pDC->DrawText(m_Text, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	pDC->SelectObject(pOldFont);
}
//---------------------------------------------------------------------------
void CXMMVBtnCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( !m_Enabled ) return;
	if( m_Toggled ){
		SetState(!GetState());
	}
	else {
		SetState(TRUE);
		::Sleep(100);
		SetState(FALSE);
	}
	FireOnLButtonClick();
}
void CXMMVBtnCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
}
void CXMMVBtnCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if( !m_Enabled ) return;
	if( m_Toggled ){
		SetState(!GetState());
		FireOnLButtonClick();
	}
	else {
		FireDblClick();
	}
}
//---------------------------------------------------------------------------
void CXMMVBtnCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( m_Enabled ) FireOnRButtonClick();
	COleControl::OnRButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// CXMMBtnCtrl メッセージ ハンドラ
BSTR CXMMVBtnCtrl::GetCaption() 
{
	CString strResult;
	strResult = m_Text;
	return strResult.AllocSysString();
}

void CXMMVBtnCtrl::SetCaption(LPCTSTR lpszNewValue) 
{
	m_Text = lpszNewValue;
	if( m_rcClient.right ) Draw();
	SetModifiedFlag();
}

OLE_COLOR CXMMVBtnCtrl::GetTextColor() 
{
	return m_clText;
}

void CXMMVBtnCtrl::SetTextColor(OLE_COLOR nNewValue) 
{
	m_clText = nNewValue;
	if( m_rcClient.right ) Draw();
	SetModifiedFlag();
}

BOOL CXMMVBtnCtrl::GetEnabled() 
{
	return m_Enabled;
}

void CXMMVBtnCtrl::SetEnabled(BOOL bNewValue) 
{
	m_Enabled = bNewValue;
	if( m_rcClient.right ) Draw();
	SetModifiedFlag();
}

BOOL CXMMVBtnCtrl::GetState() 
{
	return m_fState;
}

void CXMMVBtnCtrl::SetState(BOOL bNewValue) 
{
	if( m_fState == bNewValue ) return;
	m_fState = bNewValue;
	if( m_rcClient.right ) Draw();
	SetModifiedFlag();
}

BOOL CXMMVBtnCtrl::GetToggled() 
{
	return m_Toggled;
}

void CXMMVBtnCtrl::SetToggled(BOOL bNewValue) 
{
	m_Toggled = bNewValue;
	if( !m_Toggled && GetState() ) SetState(FALSE);
	SetModifiedFlag();
}


OLE_COLOR CXMMVBtnCtrl::GetFaceColor() 
{
	return m_clFace;
}

void CXMMVBtnCtrl::SetFaceColor(OLE_COLOR nNewValue) 
{
	m_clFace = nNewValue;
	if( m_rcClient.right ) Draw();
	SetModifiedFlag();
}

void CXMMVBtnCtrl::OnVisibleChanged() 
{
	ShowWindow(m_visible ? SW_SHOW : SW_HIDE);
	SetModifiedFlag();
}
