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

// XMMVSpecCtl.cpp : CXMMVSpecCtrl ActiveX �R���g���[�� �N���X�̃C���v�������e�[�V����

#include "stdafx.h"
#include "XMMVSpec.h"
#include "XMMVSpecCtl.h"
#include "XMMVSpecPpg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CXMMVSpecCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// ���b�Z�[�W �}�b�v

BEGIN_MESSAGE_MAP(CXMMVSpecCtrl, COleControl)
	//{{AFX_MSG_MAP(CXMMVSpecCtrl)
	// ���� - ClassWizard �̓��b�Z�[�W �}�b�v�̃G���g����ǉ��܂��͍폜���܂�
	//    ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������I
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// �f�B�X�p�b�` �}�b�v

BEGIN_DISPATCH_MAP(CXMMVSpecCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CXMMVSpecCtrl)
	// ���� - ClassWizard �̓f�B�X�p�b�` �}�b�v�̃G���g����ǉ��܂��͍폜���܂�
	//    ���̈ʒu�ɐ��������R�[�ނ�ҏW���Ȃ��ł��������I
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// �C�x���g �}�b�v

BEGIN_EVENT_MAP(CXMMVSpecCtrl, COleControl)
	//{{AFX_EVENT_MAP(CXMMVSpecCtrl)
	// ���� - ClassWizard �̓C�x���g �}�b�v�̃G���g����ǉ��܂��͍폜���܂�
	//    ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������I
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// �v���p�e�B �y�[�W

// TODO: �v���p�e�B �y�[�W��ǉ����āABEGIN �s�̍Ō�ɂ���J�E���g�𑝂₵�Ă��������B
BEGIN_PROPPAGEIDS(CXMMVSpecCtrl, 1)
	PROPPAGEID(CXMMVSpecPropPage::guid)
END_PROPPAGEIDS(CXMMVSpecCtrl)


/////////////////////////////////////////////////////////////////////////////
// �N���X �t�@�N�g���� guid �����������܂�

IMPLEMENT_OLECREATE_EX(CXMMVSpecCtrl, "XMMVSPEC.XMMVSpecCtrl.1",
	0xcf32d6e4, 0x9ea, 0x44b6, 0xa8, 0xaf, 0xde, 0x19, 0x39, 0x1c, 0xea, 0xaa)


/////////////////////////////////////////////////////////////////////////////
// �^�C�v ���C�u���� ID �ƃo�[�W����

IMPLEMENT_OLETYPELIB(CXMMVSpecCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// �C���^�[�t�F�C�X ID

const IID BASED_CODE IID_DXMMVSpec =
		{ 0xaaf1a6cb, 0xc872, 0x4ddd, { 0x96, 0xd6, 0x4d, 0xe0, 0xb8, 0x2, 0x98, 0x2e } };
const IID BASED_CODE IID_DXMMVSpecEvents =
		{ 0x29758b5e, 0x5890, 0x478a, { 0x9c, 0x35, 0x12, 0xf, 0xdb, 0xa3, 0xb5, 0x79 } };


/////////////////////////////////////////////////////////////////////////////
// �R���g���[���̌^���̐���

static const DWORD BASED_CODE _dwXMMVSpecOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CXMMVSpecCtrl, IDS_XMMVSPEC, _dwXMMVSpecOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecCtrl::CXMMVSpecCtrlFactory::UpdateRegistry -
// CXMMVSpecCtrl �̃V�X�e�� ���W�X�g���̃G���g����ǉ��܂��͍폜���܂� 

BOOL CXMMVSpecCtrl::CXMMVSpecCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: �R���g���[�����A�p�[�g�����g ���f���̃X���b�h�����̋K���ɏ]���Ă��邱�Ƃ�
	// �m�F���Ă��������B�ڍׂɂ��Ă� MFC �̃e�N�j�J�� �m�[�g 64 ���Q�Ƃ��Ă��������B
	// �A�p�[�g�����g ���f���̃X���b�h�����̋K���ɏ]��Ȃ��R���g���[���̏ꍇ�́A6 �Ԗ�
	// �̃p�����[�^���ȉ��̂悤�ɕύX���Ă��������B
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_XMMVSPEC,
			IDB_XMMVSPEC,
			afxRegApartmentThreading,
			_dwXMMVSpecOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecCtrl::CXMMVSpecCtrl - �R���X�g���N�^

CXMMVSpecCtrl::CXMMVSpecCtrl()
{
	InitializeIIDs(&IID_DXMMVSpec, &IID_DXMMVSpecEvents);

	// TODO: ���̈ʒu�ɃR���g���[���̃C���X�^���X �f�[�^�̏�����������ǉ����Ă�������
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecCtrl::~CXMMVSpecCtrl - �f�X�g���N�^

CXMMVSpecCtrl::~CXMMVSpecCtrl()
{
	// TODO: ���̈ʒu�ɃR���g���[���̃C���X�^���X �f�[�^�̌㏈���p�̃R�[�h��ǉ����Ă�������
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecCtrl::OnDraw - �`��֐�

void CXMMVSpecCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	// TODO: �ȉ��̃R�[�h��`��p�̃R�[�h�ɒu�������Ă�������
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecCtrl::DoPropExchange - �i�����̃T�|�[�g

void CXMMVSpecCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: �p���\�����������e�J�X�^�� �v���p�e�B�p�� PX_ �֐��̌Ăяo����ǉ����Ă�������

}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecCtrl::OnResetState - �R���g���[���̃v���p�e�B�l�����Z�b�g

void CXMMVSpecCtrl::OnResetState()
{
	COleControl::OnResetState();  // DoPropExchange ���Ăяo���ăf�t�H���g�l�Ƀ��Z�b�g

	// ���̈ʒu�ɃR���g���[���̏�Ԃ����Z�b�g���鏈����ǉ����Ă�������
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecCtrl ���b�Z�[�W �n���h��
