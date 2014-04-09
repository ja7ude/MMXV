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

#include "stdafx.h"
#include "MMVARI.h"
#include "MMVARICtl.h"
///----------------------------------------------------------------

#include "Comm.h"
#include "ComLib.h"

#define	WAITSTAT	0

#define	DEFFSOUND	3

COMMPARA	COMM;
void __fastcall InitCOMMPara(void)
{
	COMM.change = 1;
}

CComm *g_pComm = NULL;

VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if( g_pComm ){
		g_pComm->PutCharExtfsk();
	}
}

//---------------------------------------------------------------------------
void CALLBACK MMTimeProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	CComm *pComm = (CComm *)dwUser;
	if( uID != pComm->m_uMMTimerID ) return;

	pComm->m_FSK.Timer();
}

//***************************************************************************
// CFSK class
CFSK::CFSK(void)
{
	m_bPortD = 0;

	m_BLen = 5;
	Init();
#if MeasureAccuracy
	QueryPerformanceFrequency(&m_liFreq);
	m_liPOld.u.HighPart = -1;
#endif
}
//---------------------------------------------------------------------------
void __fastcall CFSK::Init(void)
{
	m_hPort = INVALID_HANDLE_VALUE;
	m_StgD = -1;
	m_Sequence = 0;
	m_Count = 0;
	m_oFSK = 1;
	m_aFSK = -1;
}
//---------------------------------------------------------------------------
void __fastcall CFSK::SetHandle(HANDLE hPort)
{
	m_hPort = hPort;
	m_StgD = -1;
	m_Sequence = 0;
	m_Count = 0;
	m_oFSK = 1;			// mark signal
	m_aFSK = -1;
}
//---------------------------------------------------------------------------
//	para:	Upper16bits	Speed(eg. 45)
//			Lower16bits	b1-b0	Stop (0-1, 1-1.5, 2-2)
//						b5-b2	Length
void __fastcall CFSK::SetPara(LONG para)
{
	m_BLen = (para >> 2) & 0x000f;
}
//---------------------------------------------------------------------------
// This function is called from the TimeProc(). and according to
//MSDN,  it may be an illegal operation.  MSDN said, Applications
//should not call any system-defined functions from inside a
//callback function,  except for several functions.
// However, the EscapeCommFunction() seems to be no problem on my
//PCs with Windows 2000 and Windows XP, but I am not sure if it
//works on every PC.
// BTW, EnterCriticalSection() and LeaveCriticalSection() had problem
//on this, and I gave up to use them....
//
void __fastcall CFSK::SetPort(int sw)
{
	::EscapeCommFunction(m_hPort, sw ? SETBREAK : CLRBREAK);
}
//---------------------------------------------------------------------------
// 11ms interval
void __fastcall CFSK::Timer(void)
{
	if( m_Count <= 0 ){
		switch(m_Sequence){
			case 1:			// output data
				m_oFSK = (m_NowD & 1) ? 1 : 0;
				m_NowD = m_NowD >> 1;
				m_BCount--;
				if( !m_BCount ){
					m_Sequence++;
				}
				m_Count = 1;
				break;
			case 2:			// output stop-bit
				m_oFSK = 1;
				m_Count = 2;
				m_Sequence = 0;
				break;
			default:
				if( m_StgD != -1 ){
					m_NowD = m_StgD;
					m_StgD = -1;
					m_BCount = m_BLen;
					m_oFSK = 0;		// output start-bit
					m_Sequence = 1;
					m_Count = 1;
					m_Idle = 0;
#if MeasureAccuracy
					if( QueryPerformanceCounter(&m_liPCur) ){
						if( m_liPOld.u.HighPart != -1 ){
							m_dlDiff = m_liPCur.QuadPart - m_liPOld.QuadPart;
						}
						m_liPOld = m_liPCur;
					}
#endif
				}
				else {
					m_oFSK = 1;		// output mark signal.
					m_Idle = 1;
#if MeasureAccuracy
					m_liPOld.u.HighPart = -1;
#endif
				}
				break;
		}
	}
	else {
		m_Count--;
	}

	if( !IsOpen() ) return;

	if( m_oFSK != m_aFSK ){
		m_aFSK = m_oFSK;
		SetPort(m_invFSK ? m_oFSK : !m_oFSK);
	}
}

//-----------------------------------------------------------------
BYTE __fastcall CComm::GetPutChar(void)
{
	BYTE c;

   	if( m_QueueExtfsk.IsEmpty() ){
		switch(m_Diddle){
			case diddleLTR:
				c = 0x1f;
				if( m_Fig ) m_FigOut = TRUE;
               	break;
			default:
				c = 0x00;
               	break;
        }
    }
	else if( m_FigOut ){
		c = 0x1b;
        m_FigOut = FALSE;
    }
    else {
		c = m_QueueExtfsk.Pop();
        switch(c){
			case 0x1f:		// 11111 LTR
				m_Fig = FALSE;
               	break;
            case 0x1b:		// 11011 FIG
               	m_Fig = TRUE;
                break;
		}
	}
    return c;
}
//-----------------------------------------------------------------
void __fastcall CComm::PutCharExtfsk(void)
{
	if( m_pEXT != NULL ){
		if( !m_pEXT->IsTxBusy() ){
			m_pEXT->PutChar(GetPutChar());
		}
	}
    else if( m_FSK.IsOpen() ){
		if( !m_FSK.IsBusy() ){
			m_FSK.PutByte(GetPutChar());
        }
    }
}
//-----------------------------------------------------------------
void __fastcall CComm::CreateTimer(void)
{
	if( !m_bFSKOUT ) return;
	if( !m_fSendChar ) return;

	m_Fig = FALSE;
	m_FigOut = FALSE;
	g_pComm = this;
	m_QueueExtfsk.Clear();
	if( !m_ID_ReadTimerExtfsk ){
		m_ID_ReadTimerExtfsk = ::SetTimer(NULL, 0, EXTFSK_CHECK_INTERVAL, TimerProc);
	}
	if( !m_pEXT && (m_fHnd != INVALID_HANDLE_VALUE) ){
		m_FSK.SetHandle(m_fHnd);
		m_FSK.ClearPort();

		m_uMMTimerID = 0;
		if( ::timeGetDevCaps(&m_TimeCaps, sizeof(m_TimeCaps)) == TIMERR_NOERROR ){
			::timeBeginPeriod(m_TimeCaps.wPeriodMin);
			m_uMMTimerID = ::timeSetEvent(11, 0, MMTimeProc, DWORD(this), TIME_PERIODIC);
		}
    }
}
//-----------------------------------------------------------------
void __fastcall CComm::DeleteTimer(void)
{
	if( m_ID_ReadTimerExtfsk ){
		::KillTimer(NULL, m_ID_ReadTimerExtfsk);
        m_ID_ReadTimerExtfsk = 0;
    }
	if( m_uMMTimerID ){
		m_FSK.Disable();

		::timeKillEvent(m_uMMTimerID);
		m_uMMTimerID = 0;
		::timeEndPeriod(m_TimeCaps.wPeriodMin);
	}
}

void __fastcall CComm::SetFSK(int bFSK, int bINV)
{
	m_bFSKOUT = bFSK;
    m_bINVFSK = bINV;
	m_FSK.SetInvFSK(m_bINVFSK);
}

CComm::CComm(void)
{
	m_CreateON = FALSE;	// �N���G�C�g�t���O
	m_Command = 0;
	m_fHnd = NULL;		// �t�@�C���n���h��
	m_ptt = m_scan = 0;
	m_pEXT = NULL;
	m_pMain = NULL;

	m_bFSKOUT = FALSE;
	m_bINVFSK = FALSE;

	m_ID_ReadTimerExtfsk = 0;
	m_fSendChar = 0;
    m_Diddle = diddleLTR;
	m_Fig = FALSE;
	m_FigOut = FALSE;

	m_uMMTimerID = 0;
}

/*#$%
==============================================================
	�ʐM������N���[�Y����
--------------------------------------------------------------
--------------------------------------------------------------
--------------------------------------------------------------
==============================================================
*/
void __fastcall CComm::Close(void)
{
	if( m_CreateON == TRUE ){
		DeleteTimer();
		if( m_pEXT != NULL ){
			delete m_pEXT;
			m_pEXT = NULL;
		}
		else {
			::CloseHandle(m_fHnd);
		}
		m_CreateON = FALSE;
	}
}

/*#$%
==============================================================
	�ʐM������I�[�v������
--------------------------------------------------------------
PortName : ����̖��O
pCP		 : COMMPARA�̃|�C���^�i�k���̎��̓f�t�H���g�ŏ������j
RBufSize : ��M�o�b�t�@�̃T�C�Y(default=2048)
TBufSize : ���M�o�b�t�@�̃T�C�Y(default=2048)
--------------------------------------------------------------
TRUE/FALSE
--------------------------------------------------------------
==============================================================
*/
BOOL __fastcall CComm::Open(LPCTSTR PortName, CMMVARICtrl *pMain)
{
	m_pMain = pMain;
	if( m_CreateON == TRUE ) Close();
	m_fHnd = ::CreateFile( PortName, GENERIC_READ | GENERIC_WRITE,
						0, NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL
	);
	if( m_fHnd == INVALID_HANDLE_VALUE ){
		CString cs = "\\\\.\\";
		cs += PortName;
		m_fHnd = ::CreateFile( cs, GENERIC_READ | GENERIC_WRITE,
							0, NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL
		);
	}
	if( m_fHnd == INVALID_HANDLE_VALUE ){
		m_pEXT = new CEXTFSK(PortName);
		if( m_pEXT->IsLib() ){
			LONG para;
			para = (45 << 16) | (5 << 2);
			m_pEXT->Open(para);
			m_CreateON = TRUE;
			return TRUE;
		}
		else {
			delete m_pEXT;
			m_pEXT = NULL;
		}
		return FALSE;
	}
	// setup device buffers
	if( ::SetupComm( m_fHnd, DWORD(1024), DWORD(2) ) == FALSE ){
		::CloseHandle(m_fHnd);
		return FALSE;
	}

	// purge any information in the buffer
	::PurgeComm( m_fHnd, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );

	// set up for overlapped I/O
	COMMTIMEOUTS TimeOut;

	TimeOut.ReadIntervalTimeout = 0xffffffff;
	TimeOut.ReadTotalTimeoutMultiplier = 0;
	TimeOut.ReadTotalTimeoutConstant = 0;
	TimeOut.WriteTotalTimeoutMultiplier = 0;
	TimeOut.WriteTotalTimeoutConstant = 20000;
//	TimeOut.WriteTotalTimeoutConstant = 1;
	if( !::SetCommTimeouts( m_fHnd, &TimeOut ) ){
		::CloseHandle( m_fHnd );
		return FALSE;
	}
	::GetCommState( m_fHnd, &m_dcb );
	m_dcb.BaudRate = 9600;
	m_dcb.fBinary = TRUE;
	m_dcb.ByteSize = 8;
	m_dcb.Parity = NOPARITY;
	m_dcb.StopBits = ONESTOPBIT;
	m_dcb.XonChar = 0x11;	// XON
	m_dcb.XoffChar = 0x13;	// XOFF
	m_dcb.fParity = 0;
	m_dcb.fOutxCtsFlow = FALSE;
	m_dcb.fInX = m_dcb.fOutX = FALSE;
	m_dcb.fOutxDsrFlow = FALSE;
	m_dcb.EvtChar = 0x0d;

	m_dcb.fRtsControl = RTS_CONTROL_DISABLE;		// ���M�֎~
	m_dcb.fDtrControl = DTR_CONTROL_DISABLE;		// ���M�֎~

//	m_dcb.fTXContinueOnXoff = TRUE;
	m_dcb.XonLim = USHORT(1024/4);			// 1/4 of RBufSize
	m_dcb.XoffLim = USHORT(1024*3/4);		// 3/4 of RBufSize
	m_dcb.DCBlength = sizeof( DCB );

	if( !::SetCommState( m_fHnd, &m_dcb ) ){
		::CloseHandle( m_fHnd );
		return FALSE;
	}

	// get any early notifications
	if( !::SetCommMask( m_fHnd, EV_RXFLAG ) ){
		::CloseHandle(m_fHnd);
		return FALSE;
	}
	::EscapeCommFunction(m_fHnd, CLRRTS);
	::EscapeCommFunction(m_fHnd, CLRDTR);
	::EscapeCommFunction(m_fHnd, CLRBREAK);
	m_CreateON = TRUE;
	return TRUE;
}
//-----------------------------------------------------------------
// PTT�؂�ւ��p
int __fastcall CComm::PTTOpen(void)
{
	if( m_CreateON == FALSE ){
		if( !strcmpi(m_pMain->m_PTTCOM, "NONE") ) return FALSE;
		Open(m_pMain->m_PTTCOM, m_pMain);
		if( m_CreateON == FALSE ) return FALSE;
	}
	return TRUE;
}
//-----------------------------------------------------------------
// PTT�؂�ւ��p
int __fastcall CComm::SetPTT(void)
{
	if( m_pEXT != NULL ){
		m_pEXT->SetPTT(m_ptt);
	}
	else if( m_pMain ){
		if( m_pMain->m_bPTTLines[pttlineRTS] ) ::EscapeCommFunction(m_fHnd, m_ptt ? SETRTS : CLRRTS);
		if( m_pMain->m_bPTTLines[pttlineDTR] ) ::EscapeCommFunction(m_fHnd, m_ptt ? SETDTR : CLRDTR);
		if( m_bFSKOUT && m_fSendChar ){
			if( m_pMain->m_bPTTLines[pttlineTXD] ) ::EscapeCommFunction(m_fHnd, m_ptt ? SETBREAK : CLRBREAK);
		}
	}
	if( m_ptt ){
		CreateTimer();
    }
    else {
		DeleteTimer();
    }
	return m_ptt;
}
//-----------------------------------------------------------------
// PTT�؂�ւ��p
void __fastcall CComm::SetPTT(int sw)
{
	m_ptt = sw;

	if( !PTTOpen() ) return;
	if( !SetPTT() && (!m_pMain->m_PTTLock) ) Close();
}

/*******************************************************************
	EXTFSK.DLL
*******************************************************************/
CEXTFSK::CEXTFSK(LPCSTR pName)
{
	char Name[128];
	sprintf(Name, "%s.%s", pName, strcmpi(pName, "EXTFSK") ? "fsk" : "dll");

	fextfskOpen	= NULL;
	fextfskClose = NULL;
	fextfskIsTxBusy = NULL;
	fextfskPutChar = NULL;
	fextfskSetPTT = NULL;
	m_hLib = ::LoadLibrary(Name);
	if( m_hLib != NULL ){
		fextfskOpen	= (extfskOpen)GetProc("_extfskOpen");
		fextfskClose = (extfskClose)GetProc("_extfskClose");
		fextfskIsTxBusy = (extfskIsTxBusy)GetProc("_extfskIsTxBusy");
		fextfskPutChar = (extfskPutChar)GetProc("_extfskPutChar");
		fextfskSetPTT = (extfskSetPTT)GetProc("_extfskSetPTT");
	}
}

//---------------------------------------------------------------------
CEXTFSK::~CEXTFSK()
{
	if( m_hLib != NULL ){
		Close();
		::FreeLibrary(m_hLib);
		m_hLib = NULL;
	}
}

//---------------------------------------------------------------------
FARPROC CEXTFSK::GetProc(LPCSTR pName)
{
	FARPROC fn = ::GetProcAddress(m_hLib, pName+1);
	if( fn == NULL ){
		fn = ::GetProcAddress(m_hLib, pName);
	}
	return fn;
}

long __fastcall CEXTFSK::Open(long para)
{
	if( !m_hLib || !fextfskOpen ) return FALSE;
	return fextfskOpen(para);
}

void __fastcall CEXTFSK::Close(void)
{
	if( !m_hLib || !fextfskClose ) return;
	fextfskClose();
}

long __fastcall CEXTFSK::IsTxBusy(void)
{
	if( !m_hLib || !fextfskIsTxBusy ) return FALSE;
	return fextfskIsTxBusy();
}

void __fastcall CEXTFSK::PutChar(BYTE c)
{
	if( !m_hLib || !fextfskPutChar ) return;
	fextfskPutChar(c);
}

void __fastcall CEXTFSK::SetPTT(long tx)
{
	if( !m_hLib || !fextfskSetPTT ) return;
	fextfskSetPTT(tx);
}

//--------------------------------------------------------------------------------------------------
CQUE::CQUE(void)
{
	Clear();
}

//--------------------------------------------------------------------------------------------------
void __fastcall CQUE::Clear(void)
{
	m_R = 0;
	m_W = 0;
	m_C = 0;
}
//--------------------------------------------------------------------------------------------------
void __fastcall CQUE::Push(BYTE c)
{
	if( m_C < QUEMAX ){
		m_tFifo[m_W++] = c;
		if( m_W >= QUEMAX ) m_W = 0;
		m_C++;
	}
}

//--------------------------------------------------------------------------------------------------
BYTE __fastcall CQUE::Pop(void)
{
	if( !m_C ) return 0;

	BYTE c = m_tFifo[m_R++];
	if( m_R >= QUEMAX ) m_R = 0;
	m_C--;
	return c;
}
