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

#if !defined(AFX_XMMVSPECPPG_H__DCDD769E_2974_4032_971B_67FA4FBFB817__INCLUDED_)
#define AFX_XMMVSPECPPG_H__DCDD769E_2974_4032_971B_67FA4FBFB817__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// XMMVSpecPpg.h : CXMMVSpecPropPage プロパティ ページ クラスの宣言を行います

////////////////////////////////////////////////////////////////////////////
// CXMMVSpecPropPage : このクラスの動作の定義に関しては XMMVSpecPpg.cpp.cpp ファイルを参照してください

class CXMMVSpecPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CXMMVSpecPropPage)
	DECLARE_OLECREATE_EX(CXMMVSpecPropPage)

// コンストラクタ
public:
	CXMMVSpecPropPage();

// ダイアログ データ
	//{{AFX_DATA(CXMMVSpecPropPage)
	enum { IDD = IDD_PROPPAGE_XMMVSPEC };
		// メモ - ClassWizard はこの位置にメンバを追加します
		//    この位置に生成されるコードを編集しないでください！
	//}}AFX_DATA

// インプリメンテーション
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// メッセージ マップ
protected:
	//{{AFX_MSG(CXMMVSpecPropPage)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します
		//    この位置に生成されるコードを編集しないでください！
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_XMMVSPECPPG_H__DCDD769E_2974_4032_971B_67FA4FBFB817__INCLUDED)
