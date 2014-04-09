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

// XMMVSpecPpg.cpp : CXMMVSpecPropPage �v���p�e�B �y�[�W �N���X�̃C���v�������e�[�V����

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
// ���b�Z�[�W �}�b�v

BEGIN_MESSAGE_MAP(CXMMVSpecPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CXMMVSpecPropPage)
	// ���� - ClassWizard �͂��̈ʒu�Ƀ��b�Z�[�W �}�b�v�̃G���g����ǉ��܂��͍폜���܂�
	//    ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������I
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// �N���X �t�@�N�g�� �� guid �̏�����

IMPLEMENT_OLECREATE_EX(CXMMVSpecPropPage, "XMMVSPEC.XMMVSpecPropPage.1",
	0xc2606206, 0x54ab, 0x4c52, 0x8f, 0x2d, 0x14, 0xda, 0xc1, 0x9, 0x97, 0x72)


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecPropPage::CXMMVSpecPropPageFactory::UpdateRegistry -
// CXMMVSpecPropPage �̃V�X�e�� ���W�X�g���̃G���g����ǉ��܂��͍폜���܂�

BOOL CXMMVSpecPropPage::CXMMVSpecPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_XMMVSPEC_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecPropPage::CXMMVSpecPropPage - �R���X�g���N�^

CXMMVSpecPropPage::CXMMVSpecPropPage() :
	COlePropertyPage(IDD, IDS_XMMVSPEC_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CXMMVSpecPropPage)
	// ����: ClassWizard �͂��̈ʒu�Ƀ����o�̏�����������ǉ����܂�
	//    ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������I
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecPropPage::DoDataExchange - �y�[�W�ƃv���p�e�B�̊ԂŃf�[�^�̌������s���܂�

void CXMMVSpecPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CXMMVSpecPropPage)
	// ����: ClassWizard �͂��̈ʒu�� DDP�ADDX�ADDV �̌Ăяo���R�[�h��ǉ����܂�
	//    ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������I
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecPropPage ���b�Z�[�W �n���h��
