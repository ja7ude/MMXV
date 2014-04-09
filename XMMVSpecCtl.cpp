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

// XMMVSpecCtl.cpp : CXMMVSpecCtrl ActiveX コントロール クラスのインプリメンテーション

#include "stdafx.h"
#include "XMMVSpec.h"
#include "XMMVSpecCtl.h"
#include "XMMVSpecPpg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CXMMVSpecCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// メッセージ マップ

BEGIN_MESSAGE_MAP(CXMMVSpecCtrl, COleControl)
	//{{AFX_MSG_MAP(CXMMVSpecCtrl)
	// メモ - ClassWizard はメッセージ マップのエントリを追加または削除します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// ディスパッチ マップ

BEGIN_DISPATCH_MAP(CXMMVSpecCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CXMMVSpecCtrl)
	// メモ - ClassWizard はディスパッチ マップのエントリを追加または削除します
	//    この位置に生成されるコーﾄﾞを編集しないでください！
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// イベント マップ

BEGIN_EVENT_MAP(CXMMVSpecCtrl, COleControl)
	//{{AFX_EVENT_MAP(CXMMVSpecCtrl)
	// メモ - ClassWizard はイベント マップのエントリを追加または削除します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// プロパティ ページ

// TODO: プロパティ ページを追加して、BEGIN 行の最後にあるカウントを増やしてください。
BEGIN_PROPPAGEIDS(CXMMVSpecCtrl, 1)
	PROPPAGEID(CXMMVSpecPropPage::guid)
END_PROPPAGEIDS(CXMMVSpecCtrl)


/////////////////////////////////////////////////////////////////////////////
// クラス ファクトリと guid を初期化します

IMPLEMENT_OLECREATE_EX(CXMMVSpecCtrl, "XMMVSPEC.XMMVSpecCtrl.1",
	0xcf32d6e4, 0x9ea, 0x44b6, 0xa8, 0xaf, 0xde, 0x19, 0x39, 0x1c, 0xea, 0xaa)


/////////////////////////////////////////////////////////////////////////////
// タイプ ライブラリ ID とバージョン

IMPLEMENT_OLETYPELIB(CXMMVSpecCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// インターフェイス ID

const IID BASED_CODE IID_DXMMVSpec =
		{ 0xaaf1a6cb, 0xc872, 0x4ddd, { 0x96, 0xd6, 0x4d, 0xe0, 0xb8, 0x2, 0x98, 0x2e } };
const IID BASED_CODE IID_DXMMVSpecEvents =
		{ 0x29758b5e, 0x5890, 0x478a, { 0x9c, 0x35, 0x12, 0xf, 0xdb, 0xa3, 0xb5, 0x79 } };


/////////////////////////////////////////////////////////////////////////////
// コントロールの型情報の制御

static const DWORD BASED_CODE _dwXMMVSpecOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CXMMVSpecCtrl, IDS_XMMVSPEC, _dwXMMVSpecOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecCtrl::CXMMVSpecCtrlFactory::UpdateRegistry -
// CXMMVSpecCtrl のシステム レジストリのエントリを追加または削除します 

BOOL CXMMVSpecCtrl::CXMMVSpecCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_XMMVSPEC,
			IDB_XMMVSPEC,
			afxRegApartmentThreading,
			_dwXMMVSpecOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecCtrl::CXMMVSpecCtrl - コンストラクタ

CXMMVSpecCtrl::CXMMVSpecCtrl()
{
	InitializeIIDs(&IID_DXMMVSpec, &IID_DXMMVSpecEvents);

	// TODO: この位置にコントロールのインスタンス データの初期化処理を追加してください
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecCtrl::~CXMMVSpecCtrl - デストラクタ

CXMMVSpecCtrl::~CXMMVSpecCtrl()
{
	// TODO: この位置にコントロールのインスタンス データの後処理用のコードを追加してください
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecCtrl::OnDraw - 描画関数

void CXMMVSpecCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	// TODO: 以下のコードを描画用のコードに置き換えてください
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecCtrl::DoPropExchange - 永続性のサポート

void CXMMVSpecCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: 継続表示属性を持つ各カスタム プロパティ用の PX_ 関数の呼び出しを追加してください

}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecCtrl::OnResetState - コントロールのプロパティ値をリセット

void CXMMVSpecCtrl::OnResetState()
{
	COleControl::OnResetState();  // DoPropExchange を呼び出してデフォルト値にリセット

	// この位置にコントロールの状態をリセットする処理を追加してください
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecCtrl メッセージ ハンドラ
