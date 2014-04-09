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

// XMMVLvlPpg.cpp : CXMMVLvlPropPage プロパティ ページ クラスのインプリメンテーション

#include "stdafx.h"
#include "MMVARI.h"
#include "XMMVLvlPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CXMMVLvlPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// メッセージ マップ

BEGIN_MESSAGE_MAP(CXMMVLvlPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CXMMVLvlPropPage)
	// メモ - ClassWizard はこの位置にメッセージ マップのエントリを追加または削除します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// クラス ファクトリ と guid の初期化

IMPLEMENT_OLECREATE_EX(CXMMVLvlPropPage, "XMMVLVL.XMMVLvlPropPage.1",
	0xdf211430, 0x1010, 0x4689, 0xa6, 0x89, 0x8c, 0xa6, 0xe2, 0x3c, 0x66, 0x3e)


/////////////////////////////////////////////////////////////////////////////
// CXMMVLvlPropPage::CXMMVLvlPropPageFactory::UpdateRegistry -
// CXMMVLvlPropPage のシステム レジストリのエントリを追加または削除します

BOOL CXMMVLvlPropPage::CXMMVLvlPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_XMMVLVL_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVLvlPropPage::CXMMVLvlPropPage - コンストラクタ

CXMMVLvlPropPage::CXMMVLvlPropPage() :
	COlePropertyPage(IDD, IDS_XMMVLVL_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CXMMVLvlPropPage)
	// メモ: ClassWizard はこの位置にメンバの初期化処理を追加します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVLvlPropPage::DoDataExchange - ページとプロパティの間でデータの交換を行います

void CXMMVLvlPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CXMMVLvlPropPage)
	// メモ: ClassWizard はこの位置に DDP、DDX、DDV の呼び出しコードを追加します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVLvlPropPage メッセージ ハンドラ
