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

// MMVARIPpg.cpp : CMMVARIPropPage プロパティ ページ クラスのインプリメンテーション

#include "stdafx.h"
#include "MMVARI.h"
#include "MMVARIPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CMMVARIPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// メッセージ マップ

BEGIN_MESSAGE_MAP(CMMVARIPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CMMVARIPropPage)
	// メモ - ClassWizard はこの位置にメッセージ マップのエントリを追加または削除します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// クラス ファクトリ と guid の初期化

IMPLEMENT_OLECREATE_EX(CMMVARIPropPage, "MMVARI.MMVARIPropPage.1",
	0x6d39b55f, 0x2f31, 0x45e1, 0x99, 0xd6, 0x3d, 0x34, 0xdc, 0x77, 0x97, 0x9d)


/////////////////////////////////////////////////////////////////////////////
// CMMVARIPropPage::CMMVARIPropPageFactory::UpdateRegistry -
// CMMVARIPropPage のシステム レジストリのエントリを追加または削除します

BOOL CMMVARIPropPage::CMMVARIPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_MMVARI_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CMMVARIPropPage::CMMVARIPropPage - コンストラクタ

CMMVARIPropPage::CMMVARIPropPage() :
	COlePropertyPage(IDD, IDS_MMVARI_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CMMVARIPropPage)
	// メモ: ClassWizard はこの位置にメンバの初期化処理を追加します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CMMVARIPropPage::DoDataExchange - ページとプロパティの間でデータの交換を行います

void CMMVARIPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CMMVARIPropPage)
	// メモ: ClassWizard はこの位置に DDP、DDX、DDV の呼び出しコードを追加します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CMMVARIPropPage メッセージ ハンドラ
