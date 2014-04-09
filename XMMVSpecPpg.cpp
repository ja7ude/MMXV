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

// XMMVSpecPpg.cpp : CXMMVSpecPropPage プロパティ ページ クラスのインプリメンテーション

#include "stdafx.h"
#include "XMMVSpec.h"
#include "XMMVSpecPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CXMMVSpecPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// メッセージ マップ

BEGIN_MESSAGE_MAP(CXMMVSpecPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CXMMVSpecPropPage)
	// メモ - ClassWizard はこの位置にメッセージ マップのエントリを追加または削除します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// クラス ファクトリ と guid の初期化

IMPLEMENT_OLECREATE_EX(CXMMVSpecPropPage, "XMMVSPEC.XMMVSpecPropPage.1",
	0xc2606206, 0x54ab, 0x4c52, 0x8f, 0x2d, 0x14, 0xda, 0xc1, 0x9, 0x97, 0x72)


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecPropPage::CXMMVSpecPropPageFactory::UpdateRegistry -
// CXMMVSpecPropPage のシステム レジストリのエントリを追加または削除します

BOOL CXMMVSpecPropPage::CXMMVSpecPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_XMMVSPEC_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecPropPage::CXMMVSpecPropPage - コンストラクタ

CXMMVSpecPropPage::CXMMVSpecPropPage() :
	COlePropertyPage(IDD, IDS_XMMVSPEC_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CXMMVSpecPropPage)
	// メモ: ClassWizard はこの位置にメンバの初期化処理を追加します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecPropPage::DoDataExchange - ページとプロパティの間でデータの交換を行います

void CXMMVSpecPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CXMMVSpecPropPage)
	// メモ: ClassWizard はこの位置に DDP、DDX、DDV の呼び出しコードを追加します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecPropPage メッセージ ハンドラ
