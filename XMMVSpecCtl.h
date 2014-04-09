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

// XMMVSpecCtl.h : CXMMVSpecCtrl ActiveX �R���g���[�� �N���X�̐錾

/////////////////////////////////////////////////////////////////////////////
// CXMMVSpecCtrl : ���̃N���X�̓���̒�`�Ɋւ��Ă� XMMVSpecCtl.cpp ���Q�Ƃ��Ă�������

class CXMMVSpecCtrl : public COleControl
{
	DECLARE_DYNCREATE(CXMMVSpecCtrl)

// �R���X�g���N�^
public:
	CXMMVSpecCtrl();

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o���C�h�𐶐����܂�
	//{{AFX_VIRTUAL(CXMMVSpecCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	~CXMMVSpecCtrl();

	DECLARE_OLECREATE_EX(CXMMVSpecCtrl)   // �N���X �t�@�N�g�� �� guid
	DECLARE_OLETYPELIB(CXMMVSpecCtrl)     // GetTypeInfo
	DECLARE_PROPPAGEIDS(CXMMVSpecCtrl)    // �v���p�e�B �y�[�W ID
	DECLARE_OLECTLTYPE(CXMMVSpecCtrl)     // �^�C�v���Ƃ��̑��̃X�e�[�^�X

// ���b�Z�[�W �}�b�v
	//{{AFX_MSG(CXMMVSpecCtrl)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂�
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������I
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// �f�B�X�p�b�` �}�b�v
	//{{AFX_DISPATCH(CXMMVSpecCtrl)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂�
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������I
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// �C�x���g �}�b�v
	//{{AFX_EVENT(CXMMVSpecCtrl)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂�
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������I
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// �f�B�X�p�b�` �� �C�x���g ID
public:
	enum {
	//{{AFX_DISP_ID(CXMMVSpecCtrl)
		// ����: ClassWizard �͂��̈ʒu�ɗ񋓌^�̗v�f��ǉ��܂��͍폜���܂�
		//       ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������I
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_XMMVSPECCTL_H__8F8DDB07_4CE0_493C_BE1E_940A5A1B1850__INCLUDED)
