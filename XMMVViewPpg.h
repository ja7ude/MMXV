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

#if !defined(AFX_XMMVVIEWPPG_H__17E71C33_CD49_4A40_AF7C_34E01770B0C6__INCLUDED_)
#define AFX_XMMVVIEWPPG_H__17E71C33_CD49_4A40_AF7C_34E01770B0C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// XMMVViewPpg.h : CXMMVViewPropPage プロパティ ページ クラスの宣言を行います

////////////////////////////////////////////////////////////////////////////
// CXMMVViewPropPage : このクラスの動作の定義に関しては XMMVViewPpg.cpp.cpp ファイルを参照してください

class CXMMVViewPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CXMMVViewPropPage)
	DECLARE_OLECREATE_EX(CXMMVViewPropPage)

// コンストラクタ
public:
	CXMMVViewPropPage();

// ダイアログ データ
	//{{AFX_DATA(CXMMVViewPropPage)
	enum { IDD = IDD_PROPPAGE_XMMVVIEW };
		// メモ - ClassWizard はこの位置にメンバを追加します
		//    この位置に生成されるコードを編集しないでください！
	//}}AFX_DATA

// インプリメンテーション
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// メッセージ マップ
protected:
	//{{AFX_MSG(CXMMVViewPropPage)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します
		//    この位置に生成されるコードを編集しないでください！
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_XMMVVIEWPPG_H__17E71C33_CD49_4A40_AF7C_34E01770B0C6__INCLUDED)
