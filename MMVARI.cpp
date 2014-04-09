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

// MMVARI.cpp : CMMVARIApp �N���X�̃C���v�������e�[�V��������� DLL �̓o�^���s���܂��B

#include "stdafx.h"
#include "MMVARI.h"
#include "ComLib.h"
#include "Dsp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CMMVARIApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x7b28d2b3, 0xe19f, 0x41d5, { 0x87, 0xfe, 0x29, 0xc, 0xed, 0x52, 0x57, 0xde } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 7;


////////////////////////////////////////////////////////////////////////////
// CMMVARIApp::InitInstance - DLL �̏�����

BOOL CMMVARIApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		char bf[256];
		::GetModuleFileName(m_hInstance, bf, sizeof(bf));
		SetDirName(sys.m_BgnDir, bf);
		// Windows version
		sys.m_dwVersion = ::GetVersion();
		if( sys.m_dwVersion < 0x80000000 ){                // Windows NT
			sys.m_WinNT = TRUE;
		}
		else {
			sys.m_WinNT = FALSE;
		}
		// ����
		sys.m_LCID = GetThreadLocale();
		sys.m_wLang = LANGIDFROMLCID(sys.m_LCID);
		if( sys.m_wLang == 0x0411 ){	// ���{��
			sys.m_MsgEng = FALSE;
		}
		else {
			sys.m_MsgEng = TRUE;
		}
		sys.m_fBaseMBCS = TRUE;
		switch(sys.m_wLang){
			case 0x0411:	// JA
				sys.m_BaseCharset = SHIFTJIS_CHARSET;
				break;
			case 0x0412:	// HL
				sys.m_BaseCharset = HANGEUL_CHARSET;
				break;
			case 0x0404:	// BV
				sys.m_BaseCharset = CHINESEBIG5_CHARSET;
				break;
			case 0x0804:	// BY
				sys.m_BaseCharset = 134;
				break;
			default:
				sys.m_BaseCharset = ANSI_CHARSET;
				sys.m_fBaseMBCS = FALSE;
				break;
		}

		sys.m_DefaultMode = sys.m_fBaseMBCS ? MODE_GMSK : MODE_N_BPSK;

		g_VariCode.Init();		// �o���R�[�h�e�[�u����������
	}

	return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CMMVARIApp::ExitInstance - DLL �̏I��

int CMMVARIApp::ExitInstance()
{
	// TODO: ���̈ʒu�Ƀ��W���[���̏I��������ǉ����Ă��������B

	return COleControlModule::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - �V�X�e�� ���W�X�g���̃G���g����ǉ�

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - �V�X�e�� ���W�X�g������̃G���g�����폜

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// CFontSelect
CFontSelect::CFontSelect(CDC *pDC, CFont *pFont)
{
	m_pDC = pDC;
	m_pFont = pDC->SelectObject(pFont);
}
void CFontSelect::Release(void)
{
	if( m_pFont ){
		m_pDC->SelectObject(m_pFont);
		m_pFont = NULL;
	}
}
// CPenSelect
CPenSelect::CPenSelect(CDC *pDC, CPen *pPen)
{
	m_pDC = pDC;
	m_pPen = pDC->SelectObject(pPen);
}
void CPenSelect::Release(void)
{
	if( m_pPen ){
		m_pDC->SelectObject(m_pPen);
		m_pPen = NULL;
	}
}
// CBrushSelect
CBrushSelect::CBrushSelect(CDC *pDC, CBrush *pBrush)
{
	m_pDC = pDC;
	m_pBrush = pDC->SelectObject(pBrush);
}
void CBrushSelect::Release(void)
{
	if( m_pBrush ){
		m_pDC->SelectObject(m_pBrush);
		m_pBrush = NULL;
	}
}

void DrawBorderStyle(CDC *pDC, const RECT &rc)
{
	CBrush brush(GetSysColor(COLOR_BTNSHADOW));
	pDC->FrameRect(&rc, &brush);
	brush.DeleteObject();
	CPen pen(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT));
	CPen *pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(rc.top, rc.bottom-1);
	pDC->LineTo(rc.right-1, rc.bottom-1);
	pDC->LineTo(rc.right-1, rc.top);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

COLORREF GetRefColor(COLORREF col)
{
	if( col & 0x80000000 ){
		return ::GetSysColor(col & 0x0000ffff);
	}
	else {
		return col;
	}
}