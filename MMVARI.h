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

#if !defined(AFX_MMVARI_H__FE316477_ECCA_466E_99F9_B7B98DCE0FA7__INCLUDED_)
#define AFX_MMVARI_H__FE316477_ECCA_466E_99F9_B7B98DCE0FA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// MMVARI.h : MMVARI.DLL �̃��C�� �w�b�_�[ �t�@�C���ł�

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // ���C�� �V���{��

/////////////////////////////////////////////////////////////////////////////
// CMMVARIApp : ���̃N���X�̓���̒�`�Ɋւ��Ă� MMVARI.cpp �t�@�C�����Q�Ƃ��Ă�������

class CMMVARIApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

/////////////////////////////////////////////////////////////////////////////
typedef enum {
	errorNONE,
	errorLostRX,
	errorLostTX,
	errorSoundOpen,
	errorComOpen,
	errorEND,
}MMVX_ERRORCODE;

typedef union {
	struct {
		BYTE    r;
		BYTE    g;
		BYTE    b;
		BYTE    s;
	}b;
	DWORD       d;
	COLORREF    c;
}UCOL;

/////////////////////////////////////////////////////////////////////////////
// CFontSelect
class CFontSelect
{
	CDC		*m_pDC;
	CFont	*m_pFont;
public:
	CFontSelect(CDC *pDC, CFont *pFont);
	inline ~CFontSelect(){Release();};
	void Release(void);
};
// CPenSelect
class CPenSelect
{
	CDC		*m_pDC;
	CPen	*m_pPen;
public:
	CPenSelect(CDC *pDC, CPen *pPen);
	inline ~CPenSelect(){Release();};
	void Release(void);
};
// CBrushSelect
class CBrushSelect
{
	CDC		*m_pDC;
	CBrush	*m_pBrush;
public:
	CBrushSelect(CDC *pDC, CBrush *pBrush);
	inline ~CBrushSelect(){Release();};
	void Release(void);
};

void DrawBorderStyle(CDC *pDC, const RECT &rc);
COLORREF GetRefColor(COLORREF col);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_MMVARI_H__FE316477_ECCA_466E_99F9_B7B98DCE0FA7__INCLUDED)
