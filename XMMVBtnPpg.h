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

#if !defined(AFX_XMMVBTNPPG_H__166082DE_6755_41D7_8DAA_ED17531E8B71__INCLUDED_)
#define AFX_XMMVBTNPPG_H__166082DE_6755_41D7_8DAA_ED17531E8B71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// XMMVBtnPpg.h : CXMMVBtnPropPage �v���p�e�B �y�[�W �N���X�̐錾���s���܂�

////////////////////////////////////////////////////////////////////////////
// CXMMVBtnPropPage : ���̃N���X�̓���̒�`�Ɋւ��Ă� XMMVBtnPpg.cpp.cpp �t�@�C�����Q�Ƃ��Ă�������

class CXMMVBtnPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CXMMVBtnPropPage)
	DECLARE_OLECREATE_EX(CXMMVBtnPropPage)

// �R���X�g���N�^
public:
	CXMMVBtnPropPage();

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CXMMVBtnPropPage)
	enum { IDD = IDD_PROPPAGE_XMMVBTN };
		// ���� - ClassWizard �͂��̈ʒu�Ƀ����o��ǉ����܂�
		//    ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������I
	//}}AFX_DATA

// �C���v�������e�[�V����
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ���b�Z�[�W �}�b�v
protected:
	//{{AFX_MSG(CXMMVBtnPropPage)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂�
		//    ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������I
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_XMMVBTNPPG_H__166082DE_6755_41D7_8DAA_ED17531E8B71__INCLUDED)
