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

// XMMVViewPpg.h : CXMMVViewPropPage �v���p�e�B �y�[�W �N���X�̐錾���s���܂�

////////////////////////////////////////////////////////////////////////////
// CXMMVViewPropPage : ���̃N���X�̓���̒�`�Ɋւ��Ă� XMMVViewPpg.cpp.cpp �t�@�C�����Q�Ƃ��Ă�������

class CXMMVViewPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CXMMVViewPropPage)
	DECLARE_OLECREATE_EX(CXMMVViewPropPage)

// �R���X�g���N�^
public:
	CXMMVViewPropPage();

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CXMMVViewPropPage)
	enum { IDD = IDD_PROPPAGE_XMMVVIEW };
		// ���� - ClassWizard �͂��̈ʒu�Ƀ����o��ǉ����܂�
		//    ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������I
	//}}AFX_DATA

// �C���v�������e�[�V����
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ���b�Z�[�W �}�b�v
protected:
	//{{AFX_MSG(CXMMVViewPropPage)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂�
		//    ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������I
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_XMMVVIEWPPG_H__17E71C33_CD49_4A40_AF7C_34E01770B0C6__INCLUDED)
