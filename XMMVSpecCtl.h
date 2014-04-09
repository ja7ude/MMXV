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

#if !defined(AFX_XMMVSPECCTL_H__8F8DDB07_4CE0_493C_BE1E_940A5A1B1850__INCLUDED_)
#define AFX_XMMVSPECCTL_H__8F8DDB07_4CE0_493C_BE1E_940A5A1B1850__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// XMMVSpecCtl.h : CXMMVSpecCtrl ActiveX コントロール クラスの宣言

/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecCtrl : このクラスの動作の定義に関しては XMMVSpecCtl.cpp を参照してください

class CXMMVSpecCtrl : public COleControl
{
	DECLARE_DYNCREATE(CXMMVSpecCtrl)

// コンストラクタ
public:
	CXMMVSpecCtrl();

// オーバーライド
	// ClassWizard は仮想関数のオーバライドを生成します
	//{{AFX_VIRTUAL(CXMMVSpecCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	~CXMMVSpecCtrl();

	DECLARE_OLECREATE_EX(CXMMVSpecCtrl)   // クラス ファクトリ と guid
	DECLARE_OLETYPELIB(CXMMVSpecCtrl)     // GetTypeInfo
	DECLARE_PROPPAGEIDS(CXMMVSpecCtrl)    // プロパティ ページ ID
	DECLARE_OLECTLTYPE(CXMMVSpecCtrl)     // タイプ名とその他のステータス

// メッセージ マップ
	//{{AFX_MSG(CXMMVSpecCtrl)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します
		//        この位置に生成されるコードを編集しないでください！
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// ディスパッチ マップ
	//{{AFX_DISPATCH(CXMMVSpecCtrl)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します
		//        この位置に生成されるコードを編集しないでください！
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// イベント マップ
	//{{AFX_EVENT(CXMMVSpecCtrl)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します
		//        この位置に生成されるコードを編集しないでください！
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// ディスパッチ と イベント ID
public:
	enum {
	//{{AFX_DISP_ID(CXMMVSpecCtrl)
		// メモ: ClassWizard はこの位置に列挙型の要素を追加または削除します
		//       この位置に生成されるコードを編集しないでください！
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_XMMVSPECCTL_H__8F8DDB07_4CE0_493C_BE1E_940A5A1B1850__INCLUDED)
