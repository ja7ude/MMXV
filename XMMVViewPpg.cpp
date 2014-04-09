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

// XMMVViewPpg.cpp : CXMMVViewPropPage プロパティ ページ クラスのインプリメンテーション

#include "stdafx.h"
#include "MMVARI.h"
#include "XMMVViewPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CXMMVViewPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// メッセージ マップ

BEGIN_MESSAGE_MAP(CXMMVViewPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CXMMVViewPropPage)
	// メモ - ClassWizard はこの位置にメッセージ マップのエントリを追加または削除します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// クラス ファクトリ と guid の初期化

IMPLEMENT_OLECREATE_EX(CXMMVViewPropPage, "XMMVVIEW.XMMVViewPropPage.1",
	0x9ef905e7, 0x4dc1, 0x4b71, 0xb0, 0x84, 0x3f, 0xd5, 0x58, 0x6c, 0x45, 0x8d)


/////////////////////////////////////////////////////////////////////////////
// CXMMVViewPropPage::CXMMVViewPropPageFactory::UpdateRegistry -
// CXMMVViewPropPage のシステム レジストリのエントリを追加または削除します

BOOL CXMMVViewPropPage::CXMMVViewPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_XMMVVIEW_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVViewPropPage::CXMMVViewPropPage - コンストラクタ

CXMMVViewPropPage::CXMMVViewPropPage() :
	COlePropertyPage(IDD, IDS_XMMVVIEW_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CXMMVViewPropPage)
	// メモ: ClassWizard はこの位置にメンバの初期化処理を追加します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVViewPropPage::DoDataExchange - ページとプロパティの間でデータの交換を行います

void CXMMVViewPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CXMMVViewPropPage)
	// メモ: ClassWizard はこの位置に DDP、DDX、DDV の呼び出しコードを追加します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVViewPropPage メッセージ ハンドラ
