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

// XMMVBtnPpg.cpp : CXMMVBtnPropPage プロパティ ページ クラスのインプリメンテーション

#include "stdafx.h"
#include "MMVARI.h"
#include "XMMVBtnPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CXMMVBtnPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// メッセージ マップ

BEGIN_MESSAGE_MAP(CXMMVBtnPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CXMMVBtnPropPage)
	// メモ - ClassWizard はこの位置にメッセージ マップのエントリを追加または削除します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// クラス ファクトリ と guid の初期化

IMPLEMENT_OLECREATE_EX(CXMMVBtnPropPage, "XMMVBTN.XMMVBtnPropPage.1",
	0xc550e35d, 0x9fb0, 0x4c42, 0x8f, 0xf, 0x19, 0x5, 0x9e, 0xc5, 0x5c, 0x52)


/////////////////////////////////////////////////////////////////////////////
// CXMMVBtnPropPage::CXMMVBtnPropPageFactory::UpdateRegistry -
// CXMMVBtnPropPage のシステム レジストリのエントリを追加または削除します

BOOL CXMMVBtnPropPage::CXMMVBtnPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_XMMVBTN_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVBtnPropPage::CXMMVBtnPropPage - コンストラクタ

CXMMVBtnPropPage::CXMMVBtnPropPage() :
	COlePropertyPage(IDD, IDS_XMMVBTN_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CXMMVBtnPropPage)
	// メモ: ClassWizard はこの位置にメンバの初期化処理を追加します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVBtnPropPage::DoDataExchange - ページとプロパティの間でデータの交換を行います

void CXMMVBtnPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CXMMVBtnPropPage)
	// メモ: ClassWizard はこの位置に DDP、DDX、DDV の呼び出しコードを追加します
	//    この位置に生成されるコードを編集しないでください！
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVBtnPropPage メッセージ ハンドラ
